#include "game.h"
#include "math_util.h"
#include "data/tables.h"
#include "native/actions.h"
#include "script/vm.h"
#include <math.h>

/* 4ab540/570/5d0: explicit unsigned wrap, low-word scaled draws, strict
 * per-thousand comparison. These two words belong to the battle, not libc. */
uint32_t battle_random(uint32_t range) {
  uint32_t value = g_battle.random[0] + g_battle.random[1];
  g_battle.random[0] = value;
  g_battle.random[1] += value - (value >> 31);
  return (uint32_t)((uint64_t)(value & 0xffffu) * range >> 16);
}

bool battle_chance(int per_thousand) {
  return battle_random(1000) < (uint32_t)maxi(0, per_thousand);
}

bool battle_cpu_action(Fighter *fighter, CpuAction action) {
  const CharacterDef *def = roster_def(fighter->roster);
  require(action >= 0 && action < CPU_ACTION_COUNT, "Invalid CPU action");
  FighterFn entry = def->cpu != NULL ? def->cpu->enter[action] : NULL;
  unsigned selection = action == CPU_SPECIAL_NEAR ? 0u : action == CPU_SPECIAL_1 ? 1u : action == CPU_SPECIAL_2 ? 2u : 3u;
  if (action == CPU_SPECIAL_NEAR || action == CPU_SPECIAL_1 || action == CPU_SPECIAL_2 || action == CPU_PROJECTILE) {
    uint32_t target = mk4_cpu_target(battle_data(), (unsigned)fighter->roster, selection);
    entry = target ? script_native_entry(target) : NULL;
  }
  if (fighter->armed && fighter->weapon != NULL) {
    if (action == CPU_JAB || action == CPU_CLOSE_HP) {
      entry = action_weapon_high_punch;
    } else if (action == CPU_HIGH_KICK || action == CPU_CLOSE_HK) {
      entry = action_weapon_low_punch;
    } else if (action == CPU_WEAPON_THROW) {
      entry = weapon_throw;
    }
  }
  fighter->held = 0;
  if (entry != NULL) {
    fighter->facing = fighter->body.pos[AXIS_X] <= fighter_opponent(fighter)->body.pos[AXIS_X] ? 1 : -1;
    if (!fighter_airborne(fighter)) {
      fighter_set_speed(fighter, 0);
    }
    fighter_interrupt(fighter, entry);
    return true;
  }
  /* Resolve fallback actions through the loaded stance controls, without
   * synthetic human command history or per-fighter C button callbacks. */
  Button button = action == CPU_LOW_KICK || action == CPU_SWEEP || action == CPU_AIR_LK       ? BUTTON_LOW_KICK
                  : action == CPU_HIGH_KICK || action == CPU_CLOSE_HK || action == CPU_AIR_HK ? BUTTON_HIGH_KICK
                  : action == CPU_THROW || action == CPU_CROUCH_LP                            ? BUTTON_LOW_PUNCH
                                                                                              : BUTTON_HIGH_PUNCH;
  if (action >= CPU_PROJECTILE) {
    return false;
  }
  if (action == CPU_UPPERCUT || action == CPU_CROUCH_LP) {
    fighter->held = BUTTON_BIT(BUTTON_DOWN);
    fighter_enter_state(fighter, S_CROUCH);
  } else if (action == CPU_SWEEP) {
    fighter->held = BUTTON_BIT(fighter->facing > 0 ? BUTTON_LEFT : BUTTON_RIGHT);
  }
  const ButtonTable *table =
      def->controls->stances[fighter_airborne(fighter) ? ((fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z]) ? STANCE_DIAGONAL_AIR : STANCE_JUMP)
                             : action == CPU_UPPERCUT || action == CPU_CROUCH_LP ? STANCE_CROUCHING
                                                                                 : STANCE_STANDING];
  if (table != NULL && table->table_address != 0) {
    uint32_t target = action_button_target(battle_data(), table->table_address, button);
    if (target != 0) {
      action_press_native_button(fighter, target);
      return true;
    }
  }
  return false;
}

enum {
  PLAN_FOOTWORK = CPU_ACTION_COUNT,
  PLAN_FORWARD_FLIP,
  PLAN_BACK_FLIP,
  PLAN_THROW_OR_CLOSE,
  PLAN_APPROACH_PROJECTILE,
  PLAN_CROUCH_COUNTER,
  PLAN_WAIT_CLONE
};

typedef struct CpuChoice {
  int action;
  q16 target;
  bool run;
} CpuChoice;

/* 433630: use the exact repeated entries in 4df478/498/4c0/4d8.
 * 438e50/60/70/80 select character-owned action rows through 439960. */
// clang-format off: Keep table entries on one row, including long rows.
static const CpuChoice far_choices[] = {
  {CPU_SPECIAL_2, 0, false},
  {CPU_PROJECTILE, 0, false},
  {PLAN_FORWARD_FLIP, 0x20000, true},
  {PLAN_FORWARD_FLIP, 0x20000, true},
  {CPU_PROJECTILE, -1, true},
  {CPU_CLOSE_HP, 0xb333, true},
  {PLAN_FOOTWORK, 0x20000, true},
};
static const CpuChoice medium_choices[] = {
  {PLAN_FOOTWORK, 0x20000, true},
  {CPU_SPECIAL_1, 0, false},
  {CPU_SPECIAL_2, 0, false},
  {CPU_PROJECTILE, 0, false},
  {PLAN_FORWARD_FLIP, 0x20000, true},
  {PLAN_FORWARD_FLIP, 0x20000, true},
  {CPU_PROJECTILE, -1, true},
  {CPU_CLOSE_HP, 0xb333, true},
};
static const CpuChoice near_choices[] = {
  {PLAN_FOOTWORK, 0x20000, true},
  {CPU_SPECIAL_NEAR, 0, false},
  {PLAN_FORWARD_FLIP, 0, false},
  {CPU_HIGH_KICK, 0x13333, true},
  {CPU_LOW_KICK, 0x13333, true},
};
static const CpuChoice close_choices[] = {
  {PLAN_BACK_FLIP, 0, false},
  {CPU_THROW, 0xb333, true},
  {PLAN_THROW_OR_CLOSE, 0xb333, true},
  {CPU_UPPERCUT, 0xcccc, true},
  {CPU_CLOSE_HP, 0xb333, true},
};
// clang-format on

static void choose(Fighter *fighter, const CpuChoice *choices, int count) {
  const CpuChoice *choice = &choices[battle_random((uint32_t)count)];
  fighter->cpu.plan = choice->action + 1;
  fighter->cpu.phase = 0;
  fighter->cpu.target = choice->target < 0 ? 0x18000 + (q16)battle_random(0x20000) : choice->target;
  fighter->cpu.timer = choice->run ? 30 : 0;
}

/* 4398c0, parameters 4ee82c: 25 + level*10 + adaptive progress*40
 * + rank*100, capped at 975/1000. Opponent wins join progress in arcade
 * adaptive mode. Local play can leave those context values at zero. */
static bool reacts(Fighter *fighter) {
  int progress = g_battle.cpu_progress + (g_battle.cpu_adaptive ? fighter_opponent(fighter)->wins : 0);
  int chance = mini(975, 25 + g_battle.cpu_level * 10 + progress * 40 + g_battle.cpu_rank * 100);
  return battle_chance(chance);
}

static void flip(Fighter *fighter, bool back) {
  const CharacterDef *def = roster_def(fighter->roster);
  fighter->held = BUTTON_BIT(BUTTON_UP) | BUTTON_BIT((fighter->facing > 0) != back ? BUTTON_RIGHT : BUTTON_LEFT);
  fighter_set_speed(fighter, 0);
  if (def->jump != NULL) {
    def->jump(fighter);
  }
  fighter->held = 0;
}

/* 4339b0 dispatches on a changed opponent action, not a periodic host
 * timer. Translate the normal, projectile and frozen-victim branches used
 * by Sub-Zero. Continuing plans retain their timers when no reaction wins. */
static bool respond(Fighter *fighter) {
  const Fighter *other = fighter_opponent(fighter);
  CpuState *cpu = &fighter->cpu;
  int action = other->action;
  if (action == cpu->seen_action) {
    return false;
  }
  cpu->seen_action = action;
  q16 distance = (q16)(fighter_distance(fighter) * Q16_ONE);
  if (action == 0x1003) {
    cpu->plan = CPU_UPPERCUT + 1;
    cpu->target = 0xcccc;
    cpu->timer = 30;
    return true;
  }
  bool clone = action == 0x405 || action == 0x406;
  if ((action & 0xf00) != 0x100 && (action & 0xf00) != 0x300 && action != 0x204 && !clone) {
    return false;
  }
  if (!reacts(fighter)) {
    return false;
  }
  if (clone || action == 0x204) {
    /* 436810/840 and 434b77: answer clones below 3 with the
     * projectile row; answer a distant slide the same way. */
    if (clone) {
      if (distance < 3 * Q16_ONE) {
        return battle_cpu_action(fighter, CPU_PROJECTILE);
      }
      if (action == 0x405) {
        /* 436840/4368b0, row 4df798: remain neutral until the
         * opposing clone disappears, with a 240-tick bound. */
        cpu->plan = PLAN_WAIT_CLONE + 1;
        cpu->timer = 240;
        return true;
      }
      return false;
    }
    return battle_cpu_action(fighter, distance > 3 * Q16_ONE ? CPU_PROJECTILE : CPU_CLOSE_HK);
  }
  if (action == 0x111 || action == 0x112 || action == 0x113) {
    /* 436180/1b0/435f90: close the gap for a throw/counter;
     * the longer back strike admits a projectile beyond 2.7. */
    if (distance < 0x14ccc) {
      cpu->plan = PLAN_THROW_OR_CLOSE + 1;
      cpu->target = 0xb333;
      cpu->timer = 30;
      return true;
    }
    if (action != 0x111 && distance > 0x2b333) {
      return battle_cpu_action(fighter, CPU_PROJECTILE);
    }
    if (action == 0x113) {
      /* 435fd0/439170: crouch until the attack class clears or
       * 120 ticks pass, then select the range-dependent counter. */
      cpu->plan = PLAN_CROUCH_COUNTER + 1;
      cpu->timer = 120;
      return true;
    }
    return battle_cpu_action(fighter, CPU_CLOSE_HK);
  }
  if ((action & 0xf00) == 0x300) {
    /* 435420/5590: close HK below 2; crouching LP or a forward
     * flip (50/50) below 4. At range, wait for the projectile,
     * approach until squared distance < 7, then flip. */
    if (distance < 0x20000) {
      return battle_cpu_action(fighter, CPU_CLOSE_HK);
    }
    if (distance < 0x40000) {
      if (battle_chance(500)) {
        return battle_cpu_action(fighter, CPU_CROUCH_LP);
      }
      flip(fighter, false);
      return true;
    }
    cpu->plan = PLAN_APPROACH_PROJECTILE + 1;
    cpu->phase = 0;
    cpu->timer = 0;
    return true;
  }
  if (action == 0x100 && distance < 0x1cccc) {
    return battle_cpu_action(fighter, CPU_SWEEP);
  }
  if ((action == 0x10a || action == 0x10b) && distance <= Q16_ONE) {
    return battle_cpu_action(fighter, CPU_THROW);
  }
  if (distance <= 0x28000) {
    return battle_cpu_action(fighter, CPU_CLOSE_HK);
  }
  return false;
}

static bool opponent_object(const Fighter *fighter, bool clone) {
  int owner = fighter == &g_battle.fighters[0] ? 1 : 0;
  for (int i = 0; i < MAX_PROJECTILES; ++i) {
    const Projectile *object = &g_battle.projectiles[i];
    if (object->kind != 0 && object->owner == owner && (object->kind == PROJECTILE_CLONE) == clone) {
      return true;
    }
  }
  return false;
}

/* 4345d1..43464b reads one target distance. It does not apply a
 * fighter-authored minimum or retreat gate before entering the native scene. */
static q16 finish_distance(const Fighter *fighter, int action) {
  if (action == CPU_FINISH_1 || action == CPU_FINISH_2) {
    return mk4_cpu_finish_distance(battle_data(), (unsigned)fighter->roster, action == CPU_FINISH_2);
  }
  return 0xb333;
}

static void finish(Fighter *fighter) {
  CpuState *cpu = &fighter->cpu;
  if (!battle_can_finish(fighter)) {
    return;
  }
  if (!cpu->finish_chosen) {
    cpu->finish_chosen = true;
    /* 4344a0/550: ordinary arcade chance 150, then 600 for the
     * second finisher. Stages 0 and 8 replace that choice. */
    int action = CPU_THROW;
    if (battle_chance(150)) {
      action = g_battle.arena == 0 ? CPU_FINISH_SPIKE : g_battle.arena == 8 ? CPU_FINISH_FAN : battle_chance(600) ? CPU_FINISH_2 : CPU_FINISH_1;
    }
    cpu->plan = action + 1;
    cpu->target = finish_distance(fighter, action);
    cpu->timer = 240;
  }
}

void battle_cpu_step(Fighter *fighter) {
  if (fighter->human) {
    return;
  }
  fighter->held = 0;
  if (fighter->pending != NULL || (fighter_move(fighter)->flags & SF_LOCK) || (fighter->flags & (FF_BUSY | FF_REACTING))) {
    if (fighter->flags & FF_REACTING) {
      fighter->cpu.plan = 0;
      fighter->cpu.rest = 0;
    }
    return;
  }
  if (fighter_airborne(fighter)) {
    return;
  }
  fighter->facing = fighter->body.pos[AXIS_X] <= fighter_opponent(fighter)->body.pos[AXIS_X] ? 1 : -1;
  CpuState *cpu = &fighter->cpu;
  if (g_battle.phase == PHASE_FINISH) {
    if (!battle_can_finish(fighter)) {
      return;
    }
    finish(fighter);
  } else if (g_battle.phase != PHASE_FIGHT) {
    return;
  } else {
    if (respond(fighter) && fighter->pending != NULL) {
      cpu->plan = 0;
      return;
    }
    if (fighter_airborne(fighter)) {
      return;
    }
    if (cpu->rest > 0) {
      cpu->rest--;
      return;
    }
    if (cpu->plan == 0) {
      q16 distance = (q16)(fighter_distance(fighter) * Q16_ONE);
      uint64_t weapon_elapsed = g_battle.ticks - cpu->weapon_tick;
      if (fighter->armed && weapon_elapsed >= 240 && distance >= 2 * Q16_ONE && battle_chance(weapon_elapsed > 600 ? 600 : 200)) {
        cpu->weapon_tick = g_battle.ticks;
        if (battle_cpu_action(fighter, CPU_WEAPON_THROW)) {
          return;
        }
      }
      if (g_battle.cpu_level != 1 && !fighter->armed && !fighter->weapon_used && g_battle.ticks >= 480 && battle_chance(100)) {
        cpu->weapon_tick = g_battle.ticks;
        if (battle_cpu_action(fighter, CPU_DRAW)) {
          return;
        }
      }
      if (g_battle.cpu_level <= 2 && battle_chance(500)) {
        cpu->rest = 60 + (int)battle_random(180);
        return;
      }
      if (distance < Q16_ONE) {
        choose(fighter, close_choices, 5);
      } else if (distance < 2 * Q16_ONE) {
        choose(fighter, near_choices, 5);
      } else if (distance < 3 * Q16_ONE) {
        choose(fighter, medium_choices, 8);
      } else {
        choose(fighter, far_choices, 7);
      }
    }
  }
  if (cpu->plan == 0) {
    return;
  }
  int action = cpu->plan - 1;
  q16 distance = (q16)(fighter_distance(fighter) * Q16_ONE);
  if (action == PLAN_WAIT_CLONE) {
    if (--cpu->timer <= 0 || !opponent_object(fighter, true)) {
      cpu->plan = 0;
    }
    return;
  }
  if (action == PLAN_FOOTWORK && cpu->phase > 0) {
    if (--cpu->timer <= 0) {
      if (cpu->phase == 4 || (cpu->phase == 2 && battle_chance(500))) {
        cpu->plan = 0;
        return;
      }
      cpu->phase++;
      cpu->timer = 8 + (int)battle_random(20);
    }
    bool forward = (cpu->phase & 1) != 0;
    fighter->held = BUTTON_BIT((fighter->facing > 0) == forward ? BUTTON_RIGHT : BUTTON_LEFT);
    return;
  }
  if (action == PLAN_CROUCH_COUNTER) {
    if (--cpu->timer > 0 && (fighter_opponent(fighter)->action & 0xf00) != 0) {
      fighter->held = BUTTON_BIT(BUTTON_DOWN);
      return;
    }
    if (distance < 0x12666) {
      cpu->plan = 0;
      battle_cpu_action(fighter, CPU_UPPERCUT);
      return;
    }
    cpu->plan = CPU_CLOSE_HK + 1;
    cpu->target = 0xb333;
    cpu->timer = 30;
    action = CPU_CLOSE_HK;
  }
  if (action == PLAN_APPROACH_PROJECTILE) {
    if (cpu->phase == 0 && !opponent_object(fighter, false)) {
      return;
    }
    cpu->phase = 1;
    if (q16_mul(distance, distance) >= 7 * Q16_ONE) {
      fighter->held = BUTTON_BIT(fighter->facing > 0 ? BUTTON_RIGHT : BUTTON_LEFT);
      return;
    }
    cpu->plan = PLAN_FORWARD_FLIP + 1;
    cpu->target = 0;
    action = PLAN_FORWARD_FLIP;
  }
  if (distance > cpu->target && cpu->target > 0) {
    if (cpu->timer-- > 0) {
      fighter->held = BUTTON_BIT(fighter->facing > 0 ? BUTTON_RIGHT : BUTTON_LEFT) | BUTTON_BIT(BUTTON_RUN);
      return;
    }
    cpu->plan = 0;
    return;
  }
  cpu->plan = 0;
  if (action == PLAN_FORWARD_FLIP || action == PLAN_BACK_FLIP) {
    flip(fighter, action == PLAN_BACK_FLIP);
  } else if (action == PLAN_FOOTWORK) {
    /* 436df0/436f90: four forward/back legs, 8..28 ticks each,
     * with a 50% early return after the second leg. */
    cpu->plan = PLAN_FOOTWORK + 1;
    cpu->phase = 1;
    cpu->timer = 8 + (int)battle_random(20);
  } else {
    if (action == PLAN_THROW_OR_CLOSE) {
      action = fighter_opponent(fighter)->flags & FF_REACTING ? CPU_CLOSE_HP : CPU_THROW;
    }
    battle_cpu_action(fighter, (CpuAction)action);
  }
}
