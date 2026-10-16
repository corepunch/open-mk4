#include "game.h"
#include "math_util.h"
#include <math.h>

/* 4ab540/570/5d0: explicit unsigned wrap, low-word scaled draws, strict
 * per-thousand comparison. These two words belong to the battle, not libc. */
uint32_t battle_random(Battle *battle, uint32_t range) {
  uint32_t value = battle->random[0] + battle->random[1];
  battle->random[0] = value;
  battle->random[1] += value - (value >> 31);
  return (uint32_t)((uint64_t)(value & 0xffffu) * range >> 16);
}

bool battle_chance(Battle *battle, int per_thousand) {
  return battle_random(battle, 1000) < (uint32_t)maxi(0, per_thousand);
}

bool battle_cpu_action(Battle *battle, Fighter *fighter, CpuAction action) {
  const CharacterDef *def = roster_def(fighter->roster);
  require(action >= 0 && action < CPU_ACTION_COUNT, "Invalid CPU action");
  FighterFn entry = def->cpu != NULL ? def->cpu->enter[action] : NULL;
  if (fighter->armed && fighter->weapon != NULL) {
    if (action == CPU_JAB || action == CPU_CLOSE_HP) {
      entry = fighter->weapon->hp;
    } else if (action == CPU_HIGH_KICK || action == CPU_CLOSE_HK) {
      entry = fighter->weapon->lp;
    } else if (action == CPU_WEAPON_THROW) {
      entry = fighter->weapon->throw_move;
    }
  }
  fighter->held = 0;
  if (entry != NULL) {
    fighter->facing = fighter->body.pos[AXIS_X] <= fighter_opponent(battle, fighter)->body.pos[AXIS_X] ? 1 : -1;
    if (!fighter_airborne(fighter)) {
      fighter_set_speed(battle, fighter, 0);
    }
    fighter_interrupt(fighter, entry);
    return true;
  }
  /* Unported roster rows retain their own stance entries. No synthetic
   * direction history or duplicate character move implementation is needed. */
  Button button = action == CPU_LOW_KICK || action == CPU_SWEEP                                                       ? BUTTON_LOW_KICK
                  : action == CPU_HIGH_KICK || action == CPU_CLOSE_HK || action == CPU_AIR_HK || action == CPU_AIR_LK ? BUTTON_HIGH_KICK
                  : action == CPU_THROW                                                                               ? BUTTON_LOW_PUNCH
                                                                                                                      : BUTTON_HIGH_PUNCH;
  if (action >= CPU_PROJECTILE) {
    return false;
  }
  if (action == CPU_UPPERCUT || action == CPU_CROUCH_LP) {
    fighter->held = BUTTON_BIT(BUTTON_DOWN);
    fighter_enter_state(battle, fighter, S_CROUCH);
  } else if (action == CPU_SWEEP) {
    fighter->held = BUTTON_BIT(fighter->facing > 0 ? BUTTON_LEFT : BUTTON_RIGHT);
  }
  const ButtonTable *table = def->controls->stances[fighter_airborne(fighter)                           ? STANCE_JUMP
                                                    : action == CPU_UPPERCUT || action == CPU_CROUCH_LP ? STANCE_CROUCHING
                                                                                                        : STANCE_STANDING];
  if (table != NULL && table->press[button] != NULL) {
    table->press[button](battle, fighter);
    return true;
  }
  return false;
}

enum {
  PLAN_FOOTWORK = CPU_ACTION_COUNT,
  PLAN_FORWARD_FLIP,
  PLAN_BACK_FLIP,
  PLAN_THROW_OR_CLOSE,
  PLAN_APPROACH_PROJECTILE,
  PLAN_CROUCH_COUNTER
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
  {CPU_SPECIAL_1, 0, false},
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

static void choose(Battle *battle, Fighter *fighter, const CpuChoice *choices, int count) {
  const CpuChoice *choice = &choices[battle_random(battle, (uint32_t)count)];
  fighter->cpu.plan = choice->action + 1;
  fighter->cpu.phase = 0;
  fighter->cpu.target = choice->target < 0 ? 0x18000 + (q16)battle_random(battle, 0x20000) : choice->target;
  fighter->cpu.timer = choice->run ? 30 : 0;
}

/* 4398c0, parameters 4ee82c: 25 + level*10 + adaptive progress*40
 * + rank*100, capped at 975/1000. Opponent wins join progress in arcade
 * adaptive mode. Local play can leave those context values at zero. */
static bool reacts(Battle *battle, Fighter *fighter) {
  int progress = battle->cpu_progress + (battle->cpu_adaptive ? fighter_opponent(battle, fighter)->wins : 0);
  int chance = mini(975, 25 + battle->cpu_level * 10 + progress * 40 + battle->cpu_rank * 100);
  return battle_chance(battle, chance);
}

static void flip(Battle *battle, Fighter *fighter, bool back) {
  const CharacterDef *def = roster_def(fighter->roster);
  fighter->held = BUTTON_BIT(BUTTON_UP) | BUTTON_BIT((fighter->facing > 0) != back ? BUTTON_RIGHT : BUTTON_LEFT);
  fighter_set_speed(battle, fighter, 0);
  if (def->jump != NULL) {
    def->jump(battle, fighter);
  }
  fighter->held = 0;
}

/* 4339b0 dispatches on a changed opponent action, not a periodic host
 * timer. Translate the normal, projectile and frozen-victim branches used
 * by Sub-Zero. Continuing plans retain their timers when no reaction wins. */
static bool respond(Battle *battle, Fighter *fighter) {
  const Fighter *other = fighter_opponent(battle, fighter);
  CpuState *cpu = &fighter->cpu;
  int action = other->action;
  if (action == cpu->seen_action) {
    return false;
  }
  cpu->seen_action = action;
  q16 distance = (q16)(fighter_distance(battle, fighter) * Q16_ONE);
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
  if (!reacts(battle, fighter)) {
    return false;
  }
  if (clone || action == 0x204) {
    /* 436810/840 and 434b77: answer clones below 3 with the
     * projectile row; answer a distant slide the same way. */
    if (clone) {
      return distance < 3 * Q16_ONE && battle_cpu_action(battle, fighter, CPU_PROJECTILE);
    }
    return battle_cpu_action(battle, fighter, distance > 3 * Q16_ONE ? CPU_PROJECTILE : CPU_CLOSE_HK);
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
      return battle_cpu_action(battle, fighter, CPU_PROJECTILE);
    }
    if (action == 0x113) {
      /* 435fd0/439170: crouch until the attack class clears or
       * 120 ticks pass, then select the range-dependent counter. */
      cpu->plan = PLAN_CROUCH_COUNTER + 1;
      cpu->timer = 120;
      return true;
    }
    return battle_cpu_action(battle, fighter, CPU_CLOSE_HK);
  }
  if ((action & 0xf00) == 0x300) {
    /* 435420/5590: close HK below 2; crouching LP or a forward
     * flip (50/50) below 4. At range, wait for the projectile,
     * approach until squared distance < 7, then flip. */
    if (distance < 0x20000) {
      return battle_cpu_action(battle, fighter, CPU_CLOSE_HK);
    }
    if (distance < 0x40000) {
      if (battle_chance(battle, 500)) {
        return battle_cpu_action(battle, fighter, CPU_CROUCH_LP);
      }
      flip(battle, fighter, false);
      return true;
    }
    cpu->plan = PLAN_APPROACH_PROJECTILE + 1;
    cpu->phase = 0;
    cpu->timer = 0;
    return true;
  }
  if (action == 0x100 && distance < 0x1cccc) {
    return battle_cpu_action(battle, fighter, CPU_SWEEP);
  }
  if ((action == 0x10a || action == 0x10b) && distance <= Q16_ONE) {
    return battle_cpu_action(battle, fighter, CPU_THROW);
  }
  if (distance <= 0x28000) {
    return battle_cpu_action(battle, fighter, CPU_CLOSE_HK);
  }
  return false;
}

static bool projectile_alive(const Battle *battle, const Fighter *fighter) {
  int owner = fighter == &battle->fighters[0] ? 1 : 0;
  for (int i = 0; i < MAX_PROJECTILES; ++i) {
    if (battle->projectiles[i].kind != 0 && battle->projectiles[i].owner == owner) {
      return true;
    }
  }
  return false;
}

static void finish(Battle *battle, Fighter *fighter) {
  CpuState *cpu = &fighter->cpu;
  if (!battle_can_finish(battle, fighter)) {
    return;
  }
  if (!cpu->finish_chosen) {
    cpu->finish_chosen = true;
    /* 4344a0/550: ordinary arcade chance 150, then 600 for the
     * second finisher. Stages 0 and 8 replace that choice. */
    int action = CPU_THROW;
    if (battle_chance(battle, 150)) {
      action = battle->arena == 0 ? CPU_FINISH_SPIKE : battle->arena == 8 ? CPU_FINISH_FAN : battle_chance(battle, 600) ? CPU_FINISH_2 : CPU_FINISH_1;
    }
    cpu->plan = action + 1;
    cpu->target = action == CPU_FINISH_2 ? 0x26666 : 0xb333;
    cpu->timer = 240;
  }
}

void battle_cpu_step(Battle *battle, Fighter *fighter) {
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
  fighter->facing = fighter->body.pos[AXIS_X] <= fighter_opponent(battle, fighter)->body.pos[AXIS_X] ? 1 : -1;
  CpuState *cpu = &fighter->cpu;
  if (battle->phase == PHASE_FINISH) {
    if (!battle_can_finish(battle, fighter)) {
      return;
    }
    finish(battle, fighter);
  } else if (battle->phase != PHASE_FIGHT) {
    return;
  } else {
    if (respond(battle, fighter) && fighter->pending != NULL) {
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
      q16 distance = (q16)(fighter_distance(battle, fighter) * Q16_ONE);
      uint64_t weapon_elapsed = battle->ticks - cpu->weapon_tick;
      if (fighter->armed && weapon_elapsed >= 240 && distance >= 2 * Q16_ONE && battle_chance(battle, weapon_elapsed > 600 ? 600 : 200)) {
        cpu->weapon_tick = battle->ticks;
        if (battle_cpu_action(battle, fighter, CPU_WEAPON_THROW)) {
          return;
        }
      }
      if (battle->cpu_level != 1 && !fighter->armed && !fighter->weapon_used && battle->ticks >= 480 && battle_chance(battle, 100)) {
        cpu->weapon_tick = battle->ticks;
        if (battle_cpu_action(battle, fighter, CPU_DRAW)) {
          return;
        }
      }
      if (battle->cpu_level <= 2 && battle_chance(battle, 500)) {
        cpu->rest = 60 + (int)battle_random(battle, 180);
        return;
      }
      if (distance < Q16_ONE) {
        choose(battle, fighter, close_choices, 5);
      } else if (distance < 2 * Q16_ONE) {
        choose(battle, fighter, near_choices, 5);
      } else if (distance < 3 * Q16_ONE) {
        choose(battle, fighter, medium_choices, 8);
      } else {
        choose(battle, fighter, far_choices, 7);
      }
    }
  }
  if (cpu->plan == 0) {
    return;
  }
  int action = cpu->plan - 1;
  q16 distance = (q16)(fighter_distance(battle, fighter) * Q16_ONE);
  if (action == PLAN_FOOTWORK && cpu->phase > 0) {
    if (--cpu->timer <= 0) {
      if (cpu->phase == 4 || (cpu->phase == 2 && battle_chance(battle, 500))) {
        cpu->plan = 0;
        return;
      }
      cpu->phase++;
      cpu->timer = 8 + (int)battle_random(battle, 20);
    }
    bool forward = (cpu->phase & 1) != 0;
    fighter->held = BUTTON_BIT((fighter->facing > 0) == forward ? BUTTON_RIGHT : BUTTON_LEFT);
    return;
  }
  if (action == PLAN_CROUCH_COUNTER) {
    if (--cpu->timer > 0 && (fighter_opponent(battle, fighter)->action & 0xf00) != 0) {
      fighter->held = BUTTON_BIT(BUTTON_DOWN);
      return;
    }
    if (distance < 0x12666) {
      cpu->plan = 0;
      battle_cpu_action(battle, fighter, CPU_UPPERCUT);
      return;
    }
    cpu->plan = CPU_CLOSE_HK + 1;
    cpu->target = 0xb333;
    cpu->timer = 30;
    action = CPU_CLOSE_HK;
  }
  if (action == PLAN_APPROACH_PROJECTILE) {
    if (cpu->phase == 0 && !projectile_alive(battle, fighter)) {
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
  bool retreat = action == CPU_FINISH_2 && distance < 0x1cccc;
  if ((distance >= cpu->target && cpu->target > 0) || retreat) {
    if (cpu->timer-- > 0) {
      fighter->held = BUTTON_BIT((fighter->facing > 0) != retreat ? BUTTON_RIGHT : BUTTON_LEFT);
      if (!retreat) {
        fighter->held |= BUTTON_BIT(BUTTON_RUN);
      }
      return;
    }
    cpu->plan = 0;
    return;
  }
  cpu->plan = 0;
  if (action == PLAN_FORWARD_FLIP || action == PLAN_BACK_FLIP) {
    flip(battle, fighter, action == PLAN_BACK_FLIP);
  } else if (action == PLAN_FOOTWORK) {
    /* 436df0/436f90: four forward/back legs, 8..28 ticks each,
     * with a 50% early return after the second leg. */
    cpu->plan = PLAN_FOOTWORK + 1;
    cpu->phase = 1;
    cpu->timer = 8 + (int)battle_random(battle, 20);
  } else {
    if (action == PLAN_THROW_OR_CLOSE) {
      action = fighter_opponent(battle, fighter)->flags & FF_REACTING ? CPU_CLOSE_HP : CPU_THROW;
    }
    battle_cpu_action(battle, fighter, (CpuAction)action);
  }
}
