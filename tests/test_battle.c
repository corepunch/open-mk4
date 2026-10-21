#include "retail_native.h"
#include "retail_vm.h"
#include "game.h"
#include "mk4_characters.h"
#include "script.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

void goro_fireball(Fighter *fighter);
void goro_stomp_hold_separation(Fighter *fighter);
void cage_weapon_action(Fighter *fighter);
void quan_chi_weapon_action(Fighter *fighter);
bool fujin_can_dive(const Fighter *fighter);
void fujin_dive(Fighter *fighter);
bool tanya_can_first_fatality(const Fighter *fighter);
void jarek_weapon_action(Fighter *fighter);
void shinnok_weapon_action(Fighter *fighter);
void fujin_weapon_action(Fighter *fighter);
void fujin_wind_grab(Fighter *fighter);
bool reptile_can_first_fatality(const Fighter *fighter);
bool fujin_can_wind_grab(const Fighter *fighter);
void fujin_triple_kick(Fighter *fighter);
void fujin_wind(Fighter *fighter);
void fujin_wind_slam(Fighter *fighter);
bool fujin_can_wind(const Fighter *fighter);
bool fujin_can_wind_slam(const Fighter *fighter);
void kai_draw_weapon(Fighter *fighter);
bool kai_can_handstand(const Fighter *fighter);
void kai_input_47bd10(Fighter *fighter);
void kai_input_478df0(Fighter *fighter);
void kai_rising_skull(Fighter *fighter);
void kai_ground_skull(Fighter *fighter);
bool kai_can_rising_skull(const Fighter *fighter);
bool kai_can_ground_skull(const Fighter *fighter);
bool kai_can_first_fatality(const Fighter *fighter);

static void check(bool yes, const char *message) {
  if (!yes) {
    fprintf(stderr, "FAIL: %s\n", message);
    exit(1);
  }
}

static void fight(float distance) {
  battle_init(0, 4);
  g_battle.phase = PHASE_FIGHT;
  g_battle.fighters[0].body.pos[AXIS_X] = (q16)(-distance * 0.5f * Q16_ONE);
  g_battle.fighters[1].body.pos[AXIS_X] = (q16)(distance * 0.5f * Q16_ONE);
}

static void advance_ticks(int n, const Input *in) {
  const Input zero[2] = {0};
  int i;
  for (i = 0; i < n; ++i)
    battle_step(in ? in : zero);
}

static void do_attack(int state, bool guard, bool crouch) {
  Input in[2] = {0};
  if (guard)
    in[1].held |= BUTTON_BIT(BUTTON_BLOCK);
  if (crouch)
    in[1].held |= BUTTON_BIT(BUTTON_DOWN);
  /* Fighter 0 stands left of fighter 1, so back is left. */
  if (state == S_PUNCH)
    in[0].held |= BUTTON_BIT(BUTTON_LOW_PUNCH);
  if (state == S_KICK)
    in[0].held |= BUTTON_BIT(BUTTON_HIGH_KICK);
  if (state == S_SWEEP)
    in[0].held |= BUTTON_BIT(BUTTON_LOW_KICK) | BUTTON_BIT(BUTTON_LEFT);
  if (state == S_UPPERCUT)
    in[0].held |= BUTTON_BIT(BUTTON_HIGH_PUNCH) | BUTTON_BIT(BUTTON_DOWN);
  if (state == S_THROW)
    in[0].held |= BUTTON_BIT(BUTTON_LOW_PUNCH);
  battle_step(in);
  in[0] = (Input){0};
  advance_ticks(20, in);
}

static int live_blood(BloodKind kind) {
  int count = 0;
  for (int i = 0; i < MAX_BLOOD_DROPS; ++i) {
    count += kind == BLOOD_NONE ? g_battle.blood[i].kind != BLOOD_NONE : g_battle.blood[i].kind == kind;
  }
  return count;
}

static void check_hit_blood(void) {
  fight(1.f);
  Input in[2] = {0};
  in[0].held = BUTTON_BIT(BUTTON_LOW_PUNCH);
  for (int i = 0; i < 20 && live_blood(BLOOD_DROP) == 0; ++i) {
    battle_step(in);
    in[0] = (Input){0};
  }
  check(g_battle.fighters[1].health < Q16_ONE && live_blood(BLOOD_DROP) >= 6 && live_blood(BLOOD_SPATTER) == 1, "A damaging hit must spray drops and spatter");
  int forward = 0, back = 0;
  for (int i = 0; i < MAX_BLOOD_DROPS; ++i) {
    if (g_battle.blood[i].kind == BLOOD_DROP) {
      forward += g_battle.blood[i].vel[AXIS_X] > 0;
      back += g_battle.blood[i].vel[AXIS_X] < 0;
    }
  }
  check(forward > back, "Most hit blood must follow the blow away from the attacker");
  bool splashed = false;
  for (int tick = 0; tick < 150 && live_blood(BLOOD_NONE) > 0; ++tick) {
    advance_ticks(1, NULL);
    for (int i = 0; i < MAX_BLOOD_DROPS; ++i) {
      if (g_battle.blood[i].kind == BLOOD_SPLASH) {
        splashed = true;
        check(g_battle.blood[i].pos[AXIS_Y] <= 0 && g_battle.blood[i].pos[AXIS_Y] > -Q16_ONE / 8, "Blood must splash at the floor");
      }
    }
  }
  check(splashed && live_blood(BLOOD_NONE) == 0, "Hit blood must splash on the floor and then expire");
  int stains = 0;
  for (int i = 0; i < MAX_BLOOD_SPLATS; ++i) {
    const BloodSplat *splat = &g_battle.splats[i];
    if (splat->ticks > 0) {
      ++stains;
      check(splat->sprite == 163 || splat->sprite == 164 || splat->sprite == 166 || splat->sprite == 167 || splat->sprite == 168,
          "Floor stains must use the retail red splat sprites");
    }
  }
  check(stains > 0, "Larger landed drops must stain the floor");
  advance_ticks(BLOOD_SPLAT_TICKS, NULL);
  for (int i = 0; i < MAX_BLOOD_SPLATS; ++i) {
    check(g_battle.splats[i].ticks == 0, "Floor stains must fade away");
  }
  fight(1.f);
  do_attack(S_PUNCH, true, false);
  check(live_blood(BLOOD_NONE) == 0, "Blocked hits must not bleed");
  fight(1.f);
  do_attack(S_PUNCH, false, false);
  advance_ticks(60, NULL);
  bool stained = false;
  for (int i = 0; i < MAX_BLOOD_SPLATS; ++i) {
    stained |= g_battle.splats[i].ticks > 0;
  }
  check(stained, "Hit blood must stain the floor");
  battle_next_round();
  stained = false;
  for (int i = 0; i < MAX_BLOOD_SPLATS; ++i) {
    stained |= g_battle.splats[i].ticks > 0;
  }
  check(live_blood(BLOOD_NONE) == 0 && !stained, "Round reset must discard hit blood and floor stains");
}

static void check_roster(void) {
  check(roster_count() == MK4_ROSTER_COUNT, "Retail roster views must cover the loaded fighter bank");
  for (int i = 0; i < roster_count(); ++i) {
    const CharacterDef *view = roster_def(i);
    check(view->game == GAME_MK4 && view->asset == i && view->moves != NULL && view->move_count == S_COUNT,
        "Retail roster views must provide bounded engine state and model identity");
    check(view->moves[S_PUNCH]->clip == -1 && view->enter[S_PUNCH] == retail_jab, "Unstarted script states must carry no authored animation or balance");
  }
}

static void sequence_event(Fighter *fighter) {
  fighter->health--;
}

static void sequence_end(Fighter *fighter) {
  fighter_enter_state(fighter, S_IDLE);
}

static void check_sequence_events(void) {
  static const MoveSegment segments[] = {
      {0, 0, 1, 1},
      {2, 1, 0, 1},
  };
  static const MoveEvent events[] = {
      {0, sequence_event},
      {2, sequence_event},
  };
  static const MoveSequence sequence = {segments, 2, events, 2, 3, sequence_end, NULL};
  static const MoveDef move = {.clip = 294, .flags = SF_LOCK, .sequence = &sequence};
  fight(4.f);
  Fighter *fighter = &g_battle.fighters[0];
  fighter_start_move(fighter, S_PUNCH, &move);
  check(fighter->health == Q16_ONE - 1, "Entry frame event missing");
  for (int i = 0; i < 5; ++i) {
    check(fighter_frame(fighter, 23) == 0, "Unexpected entry sample");
  }
  check(fighter->health == Q16_ONE - 1, "Rendering/sample reads repeated a frame event");
  advance_ticks(1, NULL);
  check(fighter->health == Q16_ONE - 1, "Event fired on empty frame");
  advance_ticks(1, NULL);
  check(fighter->health == Q16_ONE - 2, "Repeated sample lost its own frame event");
  advance_ticks(1, NULL);
  check(fighter->state == S_IDLE, "Sequence did not transition through its end callback");
  advance_ticks(5, NULL);
  check(fighter->health == Q16_ONE - 2, "Completed sequence kept firing events");
}

static void restart_sequence(Fighter *fighter) {
  fighter_start_sequence(fighter, fighter->sequence);
}

static int sequence_completions;

static void count_sequence_end(Fighter *fighter) {
  (void)fighter;
  sequence_completions++;
}

static void check_wait_phases(void) {
  static const MoveSegment hold[] = {{0, 10, 0, 1}};
  static const MoveEvent events[] = {{0, sequence_event}, {15, sequence_event}};
  static const MoveSequence wait = {hold, 1, events, 2, 45, sequence_end, NULL};
  static const MoveDef move = {.clip = 294, .flags = SF_LOCK, .action = 0x1007};
  fight(10.f);
  Fighter *fighter = &g_battle.fighters[0];
  fighter_start_move(fighter, S_PUNCH, &move);
  fighter_start_sequence(fighter, &wait);
  fighter_step_sequence(fighter);
  check(fighter->health == Q16_ONE - 1, "Re-entering a phase pump repeated its entry event");
  advance_ticks(44, NULL);
  check(fighter->state == S_PUNCH && fighter->action == 0x1007 && fighter_sample(fighter) == 10 && fighter->health == Q16_ONE - 2,
      "Wait must retain the move/action/pose and run its timed events exactly once");
  advance_ticks(1, NULL);
  check(fighter->state == S_IDLE, "45-tick wait must return exactly on tick 45");
  fighter_start_move(fighter, S_PUNCH, &move);
  fighter_start_sequence(fighter, &wait);
  advance_ticks(44, NULL);
  fighter_enter_state(fighter, S_KNOCKDOWN);
  const MoveDef *replacement = fighter->current_move;
  advance_ticks(2, NULL);
  check(fighter->sequence != &wait && fighter->current_move == replacement, "Interrupted wait fired a stale controller return");

  static const MoveEvent restarting[] = {{0, sequence_event}, {2, restart_sequence}, {2, sequence_event}};
  static const MoveSequence restart = {hold, 1, restarting, 3, 4, sequence_end, NULL};
  fighter_start_move(fighter, S_PUNCH, &move);
  q16 health = fighter->health;
  fighter_start_sequence(fighter, &restart);
  advance_ticks(2, NULL);
  check(fighter->ticks == 0 && fighter->health == health - 2, "Restarting the same phase must stop processing the old phase's events");

  static const MoveSequence once = {hold, 1, NULL, 0, 2, count_sequence_end, NULL};
  sequence_completions = 0;
  fighter_start_sequence(fighter, &once);
  advance_ticks(5, NULL);
  check(sequence_completions == 1, "A completed phase must call its completion callback only once");

  /* A new match reuses the singleton's storage. Old phase/object state must
   * not leak into that match or keep a stale completion callback alive. */
  fighter_start_sequence(fighter, &wait);
  g_battle.contacts_off = true;
  g_battle.camera.active = true;
  g_battle.projectiles[0].kind = PROJECTILE_SPEAR;
  sequence_completions = 0;
  battle_init(4, 0);
  check(g_battle.fighters[0].roster == 4 && g_battle.fighters[1].roster == 0 && g_battle.phase == PHASE_READY && g_battle.ticks == 0,
      "Singleton initialization must start the requested fresh match");
  check(g_battle.fighters[0].sequence == NULL && g_battle.fighters[0].current_mmove == NULL && g_battle.fighters[0].pending == NULL &&
            g_battle.projectiles[0].kind == 0 && !g_battle.camera.active && !g_battle.contacts_off,
      "Singleton initialization must discard old continuations, objects and scene state");
  advance_ticks(45, NULL);
  check(g_battle.phase == PHASE_READY && g_battle.fighters[0].health == Q16_ONE && sequence_completions == 0,
      "A previous match's wait callback must not run in the fresh match");
}

/* docs/scorpion/input-and-commands.md: two-sample press rule, history, latency. */
static void check_retail_input(void) {
  fight(4.f);
  Fighter *fighter = &g_battle.fighters[0];
  Input tap[2] = {{BUTTON_BIT(BUTTON_DOWN)}, {0}};
  battle_step(tap);
  check(fighter->directions.token[0] == BUTTON_DOWN + 1 && fighter->directions.tick[0] == g_battle.ticks, "Direction press not stamped");
  battle_step((Input[2]){0});
  battle_step(tap);
  check(fighter->directions.token[1] == 0, "A one-tick release must not repeat a press");
  battle_step((Input[2]){0});
  battle_step((Input[2]){0});
  battle_step(tap);
  check(fighter->directions.token[1] == BUTTON_DOWN + 1, "A two-tick release must allow a new press");
  battle_step((Input[2]){0});
  battle_step((Input[2]){0});
  battle_step((Input[2]){{BUTTON_BIT(BUTTON_HIGH_PUNCH) | BUTTON_BIT(BUTTON_HIGH_KICK)}, {0}});
  check(fighter->latch[BUTTON_HIGH_PUNCH] == g_battle.ticks && fighter->latch[BUTTON_HIGH_KICK] == 0, "Only HP, LP, BLK, LK and RUN latch presses");
  check(fighter->pending != NULL && fighter->buttons == NULL && (fighter->flags & FF_BUSY), "First handled press must lock the fighter");
  battle_step((Input[2]){0});
  check(fighter->state == S_PUNCH && fighter->ticks == 0, "Interrupted entry must start on the next tick");
}

static void command(int player, Button first, Button second, Button attack) {
  Input input[2] = {0};
  input[player].held = BUTTON_BIT(first);
  battle_step(input);
  advance_ticks(2, NULL);
  input[player].held = BUTTON_BIT(second);
  battle_step(input);
  advance_ticks(2, NULL);
  input[player].held = BUTTON_BIT(attack);
  battle_step(input);
  battle_step((Input[2]){0});
}

static void check_spear_interruption(void) {
  for (int player = 0; player < 2; ++player) {
    for (int phase = 0; phase < 3; ++phase) {
      for (int queued = 0; queued < 2; ++queued) {
        battle_init(player ? 4 : 0, player ? 0 : 4);
        g_battle.phase = PHASE_FIGHT;
        q16 half = phase == 1 ? 5 * Q16_ONE : 2 * Q16_ONE;
        g_battle.fighters[0].body.pos[AXIS_X] = -half;
        g_battle.fighters[1].body.pos[AXIS_X] = half;
        Fighter *owner = &g_battle.fighters[player], *victim = &g_battle.fighters[1 - player];
        Button back = player ? BUTTON_RIGHT : BUTTON_LEFT;
        command(player, back, back, BUTTON_LOW_PUNCH);
        advance_ticks(phase == 0 ? 1 : phase == 1 ? 15 : 27, NULL);
        check(owner->retail_vm.active, "Spear must retain its retail script until interrupted or pull arrival");
        if (phase == 1) {
          check(owner->action == ACTION_RETAIL_1007, "Recovery interruption must reach the 45-tick nested wait");
        } else if (phase == 2) {
          check(victim->action == 0x4000 && victim->current_move->clip == 199, "Pull interruption must capture a snared victim");
        }
        q16 health = victim->health;
        if (queued) {
          fighter_interrupt(owner, fighter_ready);
        } else {
          battle_react(owner, 67);
        }
        check(!owner->retail_vm.active, "Interrupt must cancel a waiting spear program immediately");
        check(victim->state == S_IDLE, "Canceling a pulling spear must release its victim immediately");
        for (int i = 0; i < MAX_PROJECTILES; ++i) {
          check(g_battle.projectiles[i].kind != PROJECTILE_SPEAR, "Cancellation left a live spear object");
        }
        advance_ticks(70, NULL);
        check(victim->health == health && victim->state == S_IDLE, "Interrupted startup/recovery/pull ran stale projectile or victim work");
        check(owner->action == (queued ? 0 : ACTION_RETAIL_1003) && fighter_move(owner)->clip != 14,
            "Old spear completion must not replace the interruption's controller");
      }
    }
  }
}

static void unused_projectile(Projectile *projectile) {
  (void)projectile;
}

static void check_spear_allocation_and_reset(void) {
  fight(10.f);
  for (int i = 0; i < MAX_PROJECTILES; ++i) {
    check(battle_spawn_projectile(&g_battle.fighters[1], PROJECTILE_ICE, 100, unused_projectile) != NULL, "Fill the independent object pool");
  }
  command(0, BUTTON_LEFT, BUTTON_LEFT, BUTTON_LOW_PUNCH);
  advance_ticks(59, NULL);
  check(g_battle.fighters[0].retail_vm.active, "Failed projectile allocation must retain the recovery wait");
  advance_ticks(2, NULL);
  check(!g_battle.fighters[0].retail_vm.active && g_battle.fighters[0].state == S_IDLE, "Failed projectile allocation must still retire the spear program");
  for (int i = 0; i < MAX_PROJECTILES; ++i) {
    check(g_battle.projectiles[i].kind == PROJECTILE_ICE, "Failed spear allocation overwrote another object");
  }

  fight(4.f);
  command(0, BUTTON_LEFT, BUTTON_LEFT, BUTTON_LOW_PUNCH);
  advance_ticks(27, NULL);
  check(g_battle.fighters[1].action == 0x4000, "Reset must occur during a live spear pull");
  battle_reset_match();
  check(!g_battle.fighters[0].retail_vm.active && g_battle.fighters[1].state == S_IDLE,
      "Rematch must cancel the spear program and discard both paired controllers");
  advance_ticks(100, NULL);
  check(g_battle.fighters[0].health == Q16_ONE && g_battle.fighters[1].health == Q16_ONE && g_battle.fighters[0].state == S_IDLE,
      "A canceled pull must not resume after the new match begins");

  /* Both actors own a waiting program. The earlier projectile interrupts the
   * other program, whose cleanup must remove its in-flight projectile too. */
  battle_init(0, 0);
  g_battle.phase = PHASE_FIGHT;
  g_battle.fighters[0].body.pos[AXIS_X] = -2 * Q16_ONE;
  g_battle.fighters[1].body.pos[AXIS_X] = 2 * Q16_ONE;
  command(0, BUTTON_LEFT, BUTTON_LEFT, BUTTON_LOW_PUNCH);
  command(1, BUTTON_RIGHT, BUTTON_RIGHT, BUTTON_LOW_PUNCH);
  check(g_battle.fighters[0].retail_vm.active && g_battle.fighters[1].retail_vm.active, "Mirror spears must have independent waiting programs");
  advance_ticks(45, NULL);
  check(g_battle.fighters[0].health == Q16_ONE && g_battle.fighters[1].health == Q16_ONE - 0xccc && g_battle.fighters[1].action == 0x1008,
      "Earlier spear must cancel the other program and projectile, then finish its victim pull");
  check(g_battle.fighters[0].retail_vm.active && g_battle.fighters[1].retail_vm.active, "Mirror pull arrival must start independent return and native stun");
  advance_ticks(100, NULL);
  check(g_battle.fighters[0].state == S_IDLE && g_battle.fighters[1].state == S_IDLE, "Mirror pull must return both actors without stale waits");
  /* Our VM cancellation must not park a victim after a lethal spear retires
   * its projectile. This checks the engine bridge, not retail move balance. */
  fight(4.f);
  g_battle.fighters[1].health = 1;
  command(0, BUTTON_LEFT, BUTTON_LEFT, BUTTON_LOW_PUNCH);
  advance_ticks(120, NULL);
  check(g_battle.fighters[1].health == 0 && !g_battle.fighters[1].retail_vm.active, "Lethal spear must not leave a parked victim task");
}

static void check_specials(void) {
  for (int player = 0; player < 2; ++player) {
    for (int fast = 0; fast < 2; ++fast) {
      battle_init(player ? 4 : 0, player ? 0 : 4);
      g_battle.phase = PHASE_FIGHT;
      g_battle.fighters[0].body.pos[AXIS_X] = -5 * Q16_ONE;
      g_battle.fighters[1].body.pos[AXIS_X] = 5 * Q16_ONE;
      Fighter *fighter = &g_battle.fighters[player];
      if (fast) {
        fighter->flags |= FF_FOLLOW_UP;
      }
      command(player, player ? BUTTON_RIGHT : BUTTON_LEFT, player ? BUTTON_RIGHT : BUTTON_LEFT, BUTTON_LOW_PUNCH);
      check(fighter->script_phase == fast, "Spear must select the requested normal/fast startup");
      advance_ticks(fast ? 11 : 15, NULL);
      check(fighter->action == 0x1007 && fighter->retail_vm.active && fighter_sample(fighter) == 30,
          "Spear startup must enter a fresh recovery wait with action 0x1007");
      advance_ticks(44, NULL);
      check(fighter->state == S_PUNCH && fighter_sample(fighter) == 30, "Spear recovery returned before its 45-tick wait");
      advance_ticks(2, NULL);
      check(fighter->state == S_IDLE && !fighter->retail_vm.active, "Normal and fast spear must each retire after its 45-tick recovery and tagged return wait");
    }
  }
  for (int player = 0; player < 2; ++player) {
    fight(4.f);
    if (player == 1) {
      battle_init(4, 0);
      g_battle.phase = PHASE_FIGHT;
      g_battle.fighters[0].body.pos[AXIS_X] = -2 * Q16_ONE;
      g_battle.fighters[1].body.pos[AXIS_X] = 2 * Q16_ONE;
    }
    Fighter *fighter = &g_battle.fighters[player], *victim = &g_battle.fighters[1 - player];
    command(player, player ? BUTTON_RIGHT : BUTTON_LEFT, player ? BUTTON_RIGHT : BUTTON_LEFT, BUTTON_LOW_PUNCH);
    check(fighter->current_move->clip == 14 && fighter->retail_vm.active, "B,B,LP must enter spear for either player");
    advance_ticks(45, NULL);
    check(victim->health == Q16_ONE - 0xccc && fighter_distance(fighter) < 0.9f, "Spear must damage once and pull into follow-up range");
    check(victim->action == 0x1008 && victim->damage_scale == 0x9999, "Spear must leave scaled combo stun");
    check(fighter->retail_vm.active && victim->retail_vm.active, "Pull arrival must start independent return and native stun");
    advance_ticks(100, NULL);
    check(fighter->state == S_IDLE && victim->state == S_IDLE, "Spear must release both fighters");
    for (int i = 0; i < MAX_PROJECTILES; ++i) {
      check(g_battle.projectiles[i].kind == 0, "Spear leaked a projectile");
    }
  }
  {
    /* 49a6a0/49b170: the spear flies and pulls along the throw direction. */
    fight(4.f);
    for (int i = 0; i < 2; ++i) {
      g_battle.fighters[i].body.pos[AXIS_X] = (q16)((i ? 1.6f : -1.6f) * Q16_ONE);
      g_battle.fighters[i].body.pos[AXIS_Z] = (q16)((i ? 1.2f : -1.2f) * Q16_ONE);
    }
    advance_ticks(1, NULL);
    command(0, BUTTON_LEFT, BUTTON_LEFT, BUTTON_LOW_PUNCH);
    advance_ticks(45, NULL);
    const Fighter *victim = &g_battle.fighters[1];
    check(victim->health == Q16_ONE - 0xccc && fighter_distance(victim) < 0.9f && fabsf(fighter_x(victim) * .75f - fighter_z(victim)) < .02f,
        "Off-axis spear must hit and pull its victim along the throw line");
  }
  {
    fight(4.f);
    command(0, BUTTON_LEFT, BUTTON_LEFT, BUTTON_LOW_PUNCH);
    Input guard[2] = {{0}, {BUTTON_BIT(BUTTON_BLOCK)}};
    advance_ticks(60, guard);
    check(g_battle.fighters[1].health == Q16_ONE - q16_mul(0xccc, 0x3333), "Blocked spear must only chip");
    check(fighter_distance(&g_battle.fighters[0]) > 3.f, "Blocked spear must not pull");
  }
  {
    fight(4.f);
    command(0, BUTTON_LEFT, BUTTON_LEFT, BUTTON_LOW_PUNCH);
    advance_ticks(27, NULL);
    fighter_enter_state(&g_battle.fighters[0], S_HIT);
    advance_ticks(90, NULL);
    check(g_battle.fighters[1].state == S_IDLE, "Interrupted spear must release the victim");
  }
  for (int guard = 0; guard < 2; ++guard) {
    fight(2.f);
    command(0, BUTTON_DOWN, BUTTON_RIGHT, BUTTON_LOW_PUNCH);
    check(g_battle.fighters[0].current_move->clip == 448, "D,F,LP must enter fire");
    Input input[2] = {{0}, {guard ? BUTTON_BIT(BUTTON_BLOCK) : 0}};
    advance_ticks(60, input);
    check(g_battle.fighters[1].health == Q16_ONE - 0x1eb8, "Fire must deal one unblockable 12-percent hit");
    check(g_battle.fighters[0].state == S_IDLE, "Fire must recover");
  }
  for (int guard = 0; guard < 2; ++guard) {
    fight(4.f);
    command(0, BUTTON_DOWN, BUTTON_LEFT, BUTTON_HIGH_PUNCH);
    check(g_battle.fighters[0].current_move->clip == 312, "D,B,HP must enter teleport");
    Input input[2] = {{0}, {guard ? BUTTON_BIT(BUTTON_BLOCK) : 0}};
    advance_ticks(12, input);
    check(fighter_x(&g_battle.fighters[0]) > fighter_x(&g_battle.fighters[1]), "Teleport must cross to the far side");
    advance_ticks(100, input);
    check(g_battle.fighters[0].state == S_IDLE && !(g_battle.fighters[0].flags & FF_TELEPORTING), "Teleport must land and clear its state");
    check(g_battle.fighters[1].health == Q16_ONE - (guard ? q16_mul(0x170a, 0x3333) : 0x170a), "Teleport hit/block damage");
  }
  {
    /* Retail 480140 off-axis row: from (-2, 0) past (1, 2) along the pair axis. */
    fight(4.f);
    g_battle.fighters[1].body.pos[AXIS_X] = Q16_ONE;
    g_battle.fighters[1].body.pos[AXIS_Z] = 2 * Q16_ONE;
    advance_ticks(1, NULL);
    command(0, BUTTON_DOWN, BUTTON_LEFT, BUTTON_HIGH_PUNCH);
    advance_ticks(12, NULL);
    float dx = fighter_x(&g_battle.fighters[0]) - 1.f, dz = fighter_z(&g_battle.fighters[0]) - 2.f;
    check(fabsf(dx * 2.f - dz * 3.f) < .05f && dx > 1.5f, "Off-axis teleport must reappear past the opponent along the pair axis");
  }
}

static void draw_sword(void) {
  command(0, BUTTON_RIGHT, BUTTON_RIGHT, BUTTON_HIGH_KICK);
  check(g_battle.fighters[0].current_move->clip == 244, "F,F,HK must draw the sword");
  advance_ticks(21, NULL);
  check(!g_battle.fighters[0].armed, "Sword equipped before the script create event");
  advance_ticks(28, NULL);
  check(g_battle.fighters[0].armed && g_battle.fighters[0].current_move->clip == 286, "Draw must equip and return to armed stance");
}

static void check_weapon(void) {
  /* VM opcode 42 must take the common recovery on block, and fall through
   * all three contact queries on a miss. Existing cases below exercise hit. */
  for (int blocked = 0; blocked < 2; ++blocked) {
    fight(blocked ? 1.1f : 6.f);
    draw_sword();
    q16 before = g_battle.fighters[1].health;
    Input input[2] = {{BUTTON_BIT(BUTTON_LEFT) | BUTTON_BIT(blocked ? BUTTON_LOW_PUNCH : BUTTON_HIGH_PUNCH)}, {blocked ? BUTTON_BIT(BUTTON_BLOCK) : 0}};
    battle_step(input);
    input[0] = (Input){0};
    advance_ticks(80, input);
    check(g_battle.fighters[0].state == S_IDLE && g_battle.fighters[0].armed, "Generated back-sword miss/block must recover with its weapon");
    check(
        g_battle.fighters[1].health == before - (blocked ? q16_mul(0x3333, 0x3333) : 0), "Generated back-sword contact branch must preserve miss/chip damage");
  }

  for (int hp = 0; hp < 2; ++hp) {
    fight(1.3f);
    draw_sword();
    Button punch = hp ? BUTTON_HIGH_PUNCH : BUTTON_LOW_PUNCH;
    battle_step((Input[2]){{BUTTON_BIT(BUTTON_LEFT) | BUTTON_BIT(punch)}, {0}});
    advance_ticks(1, NULL);
    check(fighter_move(&g_battle.fighters[0])->clip == (hp ? 238 : 237), "Tapped back+sword punch must retain its selected variant after release");
    advance_ticks(90, NULL);
    check(g_battle.fighters[0].state == S_IDLE && g_battle.fighters[0].armed, "Back sword attacks must recover with the weapon equipped");
    check(g_battle.fighters[1].health < Q16_ONE, "Back sword attack must reach its contact window");
  }
  for (int guard = 0; guard < 2; ++guard) {
    fight(1.3f);
    draw_sword();
    Input in[2] = {{BUTTON_BIT(BUTTON_HIGH_PUNCH)}, {guard ? BUTTON_BIT(BUTTON_BLOCK) : 0}};
    battle_step(in);
    in[0].held = 0;
    advance_ticks(45, in);
    check(g_battle.fighters[1].health == Q16_ONE - (guard ? q16_mul(0x3333, 0x3333) : 0x3333), "Sword slash damage/chip must apply once");
    check(g_battle.fighters[0].armed && g_battle.fighters[0].state == S_IDLE, "Slash must retain sword and recover");
  }
  {
    fight(4.f);
    draw_sword();
    command(0, BUTTON_RIGHT, BUTTON_RIGHT, BUTTON_HIGH_KICK);
    check(g_battle.fighters[0].current_move->clip == 208, "Drawing while armed must throw");
    advance_ticks(21, NULL);
    check(g_battle.fighters[0].armed, "Throw must retain its weapon before the release frame");
    advance_ticks(1, NULL);
    check(!g_battle.fighters[0].armed && g_battle.fighters[0].weapon_used, "Throw release frame must detach and prevent a new sword");
    advance_ticks(98, NULL);
    check(g_battle.fighters[1].health == Q16_ONE - 0x4000, "Thrown sword damage must be 25 percent");
    int resting = -1;
    for (int i = 0; i < MAX_PROJECTILES; ++i) {
      if (g_battle.projectiles[i].kind == PROJECTILE_SWORD && g_battle.projectiles[i].phase == 2) {
        resting = i;
      }
    }
    check(resting >= 0, "Connected thrown sword must fall and become pickable");
    g_battle.fighters[0].body.pos[AXIS_X] = g_battle.projectiles[resting].pos[AXIS_X] - Q16_ONE / 2;
    battle_step((Input[2]){{BUTTON_BIT(BUTTON_DOWN)}, {0}});
    battle_step((Input[2]){{BUTTON_BIT(BUTTON_DOWN) | BUTTON_BIT(BUTTON_RUN)}, {0}});
    battle_step((Input[2]){{BUTTON_BIT(BUTTON_DOWN)}, {0}});
    check(g_battle.fighters[0].armed && g_battle.projectiles[resting].kind == 0, "Crouch+RUN must attach a nearby resting sword");
    advance_ticks(20, NULL);
    battle_react(&g_battle.fighters[0], 34);
    check(!g_battle.fighters[0].armed, "Launch reaction must knock out the held sword");
    advance_ticks(100, NULL);
    battle_next_round();
    check(!g_battle.fighters[0].armed && !g_battle.fighters[0].weapon_used, "New round must reset weapon availability");
    for (int i = 0; i < MAX_PROJECTILES; ++i) {
      check(g_battle.projectiles[i].kind == 0, "Round reset must remove world weapons");
    }
  }
  {
    fight(4.f);
    draw_sword();
    battle_step((Input[2]){{BUTTON_BIT(BUTTON_DOWN)}, {0}});
    battle_step((Input[2]){{BUTTON_BIT(BUTTON_DOWN) | BUTTON_BIT(BUTTON_LOW_PUNCH)}, {0}});
    Input hold[2] = {{BUTTON_BIT(BUTTON_LOW_PUNCH)}, {0}};
    advance_ticks(20, hold);
    check(g_battle.fighters[0].current_move->clip == 224, "Crouching armed LP must enter held spin");
    advance_ticks(2, NULL);
    check(g_battle.fighters[0].state == S_IDLE, "Releasing LP must end spin");
  }
  for (int behind = 0; behind < 2; ++behind) {
    /* 496d20/491050: Right steers along the pair axis toward screen right. */
    fight(4.f);
    for (int i = 0; i < 2; ++i) {
      g_battle.fighters[i].body.pos[AXIS_X] = (q16)((i ? 1.6f : -1.6f) * Q16_ONE);
      g_battle.fighters[i].body.pos[AXIS_Z] = (q16)((i ? 1.2f : -1.2f) * Q16_ONE);
    }
    advance_ticks(1, NULL);
    draw_sword();
    g_battle.view = behind ? (Vec3){-.6f, 0, .8f} : (Vec3){.6f, 0, -.8f};
    battle_step((Input[2]){{BUTTON_BIT(BUTTON_DOWN)}, {0}});
    battle_step((Input[2]){{BUTTON_BIT(BUTTON_DOWN) | BUTTON_BIT(BUTTON_LOW_PUNCH)}, {0}});
    Input steer[2] = {{BUTTON_BIT(BUTTON_LOW_PUNCH) | BUTTON_BIT(BUTTON_RIGHT)}, {0}};
    for (int i = 0; i < 20; ++i) {
      g_battle.view = behind ? (Vec3){-.6f, 0, .8f} : (Vec3){.6f, 0, -.8f};
      battle_step(steer);
    }
    const Body *body = &g_battle.fighters[0].body;
    q16 along = (behind ? -1 : 1) * 0xf5c;
    check(g_battle.fighters[0].current_move->clip == 224 && abs(body->vel[AXIS_X] - q16_mul(0xcccc, along)) <= 2 &&
              abs(body->vel[AXIS_Z] - q16_mul(0x9999, along)) <= 2,
        "Steered spin must move along the pair axis toward screen right");
  }
}

static void check_air_attacks(void) {
  {
    fight(0.75f);
    Fighter *fighter = &g_battle.fighters[0];
    fighter_enter_state(fighter, S_JUMP);
    fighter->body.pos[AXIS_Y] = fighter->body.ground - Q16_ONE / 3;
    fighter->body.gravity = 1;
    battle_step((Input[2]){{BUTTON_BIT(BUTTON_HIGH_PUNCH)}, {0}});
    advance_ticks(7, NULL);
    check(!fighter_airborne(fighter) && (fighter->flags & FF_FOLLOW_UP), "Low jump contact must open ground follow-ups");
    check(g_battle.fighters[1].flags & FF_REACTING, "Low jump contact must put the victim in hitstun");
    battle_step((Input[2]){{BUTTON_BIT(BUTTON_HIGH_PUNCH)}, {0}});
    advance_ticks(1, NULL);
    check(fighter_move(fighter)->clip == 262, "Low jump HP follow-up must select close HP even while the victim is reacting");
    advance_ticks(90, NULL);
    check(fighter->state == S_IDLE, "Ground follow-up must recover");
  }
  const Button button[] = {BUTTON_HIGH_PUNCH, BUTTON_LOW_PUNCH, BUTTON_HIGH_KICK, BUTTON_HIGH_KICK, BUTTON_LOW_KICK};
  const int clip[] = {97, 96, 327, 55, 58};
  for (int i = 0; i < 5; ++i) {
    fight(8.f);
    Fighter *fighter = &g_battle.fighters[0];
    battle_step((Input[2]){{BUTTON_BIT(BUTTON_UP) | (i >= 3 ? BUTTON_BIT(BUTTON_RIGHT) : 0)}, {0}});
    advance_ticks(5, NULL);
    battle_step((Input[2]){{BUTTON_BIT(button[i])}, {0}});
    advance_ticks(1, NULL);
    check(fighter_move(fighter)->clip == clip[i] && fighter_sample(fighter) == 0, "Air button must select its own attack script");
    bool landing = false;
    for (int tick = 0; tick < 100 && fighter->state != S_IDLE; ++tick) {
      battle_step((Input[2]){0});
      landing |= fighter_move(fighter)->clip == 304 && fighter_sample(fighter) >= 18;
    }
    check(landing && fighter->state == S_IDLE && !fighter_airborne(fighter), "Air miss must play landing and restore controls");
    check(g_battle.fighters[1].health == Q16_ONE, "Distant air attack must miss");
  }
  fight(1.f);
  Fighter *fighter = &g_battle.fighters[0];
  Fighter *victim = &g_battle.fighters[1];
  fighter_enter_state(fighter, S_JUMP);
  fighter_enter_state(victim, S_JUMP);
  fighter->body.pos[AXIS_Y] = fighter->body.ground - 0x14ccc;
  victim->body.pos[AXIS_Y] = victim->body.ground - 0x14ccc;
  fighter->body.gravity = victim->body.gravity = 1;
  battle_step((Input[2]){{BUTTON_BIT(BUTTON_HIGH_PUNCH)}, {0}});
  for (int visit = 0; !fighter->connected && visit < 32; ++visit) {
    advance_ticks(1, NULL);
  }
  check(fighter->connected && fighter->body.gravity == 0 && fighter->buttons != NULL,
      "Native air contact must transfer ownership to its live follow-up controls");
  battle_step((Input[2]){{BUTTON_BIT(BUTTON_LOW_PUNCH)}, {0}});
  advance_ticks(110, NULL);
  check(fighter->state == S_IDLE && !fighter_airborne(fighter), "Interrupting frozen air contact with a loaded follow-up must eventually release flight");
}

static void check_kai_handstand_frames(void) {
  for (int slot = 0; slot < 2; ++slot) {
    for (int branch = 0; branch < 10; ++branch) {
      int attack = branch == 9 ? 3 : branch / 3;
      int scenario = branch % 3;
      battle_init(slot ? 4 : 11, slot ? 11 : 4);
      g_battle.phase = PHASE_FIGHT;
      Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
      q16 half_distance = scenario == 2 ? 3 * Q16_ONE : 0x8ccc;
      owner->body.pos[AXIS_X] = slot ? half_distance : -half_distance;
      victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
      Input input[2] = {0};
      input[slot].held = BUTTON_BIT(BUTTON_BLOCK);
      advance_ticks(50, input);
      owner->held |= BUTTON_BIT(BUTTON_LOW_KICK);
      check(kai_can_handstand(owner), "Handstand gate must accept an old BLOCK latch");
      const Button forbidden[] = {BUTTON_UP, BUTTON_HIGH_PUNCH, BUTTON_LOW_PUNCH, BUTTON_HIGH_KICK, BUTTON_RUN};
      for (size_t index = 0; index < sizeof forbidden / sizeof forbidden[0]; ++index) {
        owner->held |= BUTTON_BIT(forbidden[index]);
        check(!kai_can_handstand(owner), "Handstand gate must reject additional attack and UP switches");
        owner->held &= ~BUTTON_BIT(forbidden[index]);
      }
      owner->held |= BUTTON_BIT(BUTTON_DOWN) | BUTTON_BIT(BUTTON_LEFT) | BUTTON_BIT(BUTTON_RIGHT);
      check(kai_can_handstand(owner), "Handstand gate must allow DOWN and horizontal switches");
      owner->held = BUTTON_BIT(BUTTON_BLOCK);
      input[slot].held |= BUTTON_BIT(BUTTON_LOW_KICK);
      advance_ticks(2, input);
      check(fighter_move(owner)->clip == 1, "BLOCK+LK must select Kai's handstand intro through the real command gate");
      input[slot].held = 0;
      advance_ticks(120, input);
      check(fighter_move(owner)->clip == 4 && owner->action == 0x200d, "Handstand must install persistent controls after its intro");
      if (attack == 3) {
        input[slot].held = BUTTON_BIT(BUTTON_RIGHT);
        advance_ticks(10, input);
        check(fighter_move(owner)->clip == 5 && owner->body.vel[AXIS_X] > 0, "Handstand walking must follow raw horizontal input in both player slots");
        input[slot].held = 0;
        advance_ticks(2, input);
        check(fighter_move(owner)->clip == 4, "Releasing handstand movement must restore idle");
        input[slot].held = BUTTON_BIT(BUTTON_BLOCK);
        battle_step(input);
        input[slot].held = 0;
        advance_ticks(60, input);
        check(owner->state == S_IDLE, "Handstand BLOCK must reverse the intro and restore ordinary controls");
        continue;
      }
      if (scenario == 1) {
        input[1 - slot].held = BUTTON_BIT(BUTTON_BLOCK);
      }
      Button button = attack == 0 ? BUTTON_HIGH_KICK : attack == 1 ? BUTTON_LOW_KICK : BUTTON_LOW_PUNCH;
      input[slot].held = BUTTON_BIT(button);
      advance_ticks(2, input);
      check(fighter_move(owner)->clip == (attack == 0 ? 0 : attack == 1 ? 6 : 2), "Each handstand button must select its own retail attack clip");
      for (int tick = 0; tick < 35; ++tick) {
        battle_step(input);
        if (attack != 2) {
          input[slot].held = 0;
        }
      }
      if (scenario != 2 && victim->health == Q16_ONE) {
        fprintf(stderr, "Kai handstand contact miss slot%d attack%d clip%d sample%d\n", slot, attack, fighter_move(owner)->clip, fighter_sample(owner));
      }
      if (attack != 2 || scenario == 2) {
        q16 damage = scenario == 2 ? 0 : attack == 0 ? 0x1c28 : 0x170a;
        if (scenario == 1) {
          damage = q16_mul(damage, 0x3333);
        }
        check(victim->health == Q16_ONE - damage, "Handstand kicks must preserve native hit, chip and miss damage");
      } else {
        check(victim->health < Q16_ONE, "Held handstand spin must execute its native attack record");
      }
      input[slot].held = 0;
      advance_ticks(180, input);
      check(fighter_move(owner)->clip == 4, "Handstand attacks must recover to their persistent stance");
      battle_react(owner, 37);
      advance_ticks(100, NULL);
      check(owner->state == S_IDLE, "Interrupting handstand must restore ordinary control and retire its motion");
    }
  }
}

static void check_roster_low_kick_frames(void) {
  const int roster[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 16, 17};
  for (size_t row = 0; row < sizeof roster / sizeof roster[0]; ++row) {
    for (int slot = 0; slot < 2; ++slot) {
      int scenarios = row == 0 ? 3 : 1;
      for (int scenario = 0; scenario < scenarios; ++scenario) {
        battle_init(slot ? 4 : roster[row], slot ? roster[row] : 4);
        g_battle.phase = PHASE_FIGHT;
        Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
        q16 distance = scenario == 2 ? 6 * Q16_ONE : 0x11999;
        owner->body.pos[AXIS_X] = slot ? distance / 2 : -distance / 2;
        victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
        Input input[2] = {0};
        input[slot].held = BUTTON_BIT(BUTTON_LOW_KICK);
        if (scenario == 1) {
          input[1 - slot].held = BUTTON_BIT(BUTTON_BLOCK);
        }
        battle_step(input);
        input[slot].held = 0;
        for (int tick = 0; tick < 100; ++tick) {
          battle_step(input);
        }
        q16 damage = scenario == 2 ? 0 : scenario == 1 ? q16_mul(0x1c28, 0x3333) : 0x1c28;
        if (victim->health != Q16_ONE - damage) {
          fprintf(stderr, "Low kick roster%d slot%d scenario%d health%d expected%d\n", roster[row], slot, scenario, victim->health, Q16_ONE - damage);
        }
        check(victim->health == Q16_ONE - damage && owner->state == S_IDLE && !owner->avoids_high,
            "Low kick must apply native damage once and restore controls without retaining avoidance");
      }
    }
  }
  for (int slot = 0; slot < 2; ++slot) {
    battle_init(slot ? 0 : 12, slot ? 12 : 0);
    g_battle.phase = PHASE_FIGHT;
    Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
    owner->body.pos[AXIS_X] = slot ? 0x8ccc : -0x8ccc;
    victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
    Input input[2] = {0};
    input[slot].held = BUTTON_BIT(BUTTON_LOW_KICK);
    input[1 - slot].held = BUTTON_BIT(BUTTON_LOW_PUNCH);
    battle_step(input);
    advance_ticks(100, NULL);
    check(owner->health == Q16_ONE && victim->health == Q16_ONE - 0x1c28,
        "Jarek's native low-kick avoidance must reject the simultaneous high jab before delivering its own hit");
    battle_init(slot ? 0 : 12, slot ? 12 : 0);
    g_battle.phase = PHASE_FIGHT;
    owner = &g_battle.fighters[slot];
    g_battle.fighters[0].body.pos[AXIS_X] = -3 * Q16_ONE;
    g_battle.fighters[1].body.pos[AXIS_X] = 3 * Q16_ONE;
    input[1 - slot].held = 0;
    battle_step(input);
    advance_ticks(8, NULL);
    check(owner->avoids_high, "Jarek must enter the native set-one avoidance window after startup");
    fighter_interrupt(owner, fighter_ready);
    check(!owner->avoids_high, "Interrupt must retire avoidance immediately before the next entry runs");
    advance_ticks(100, NULL);
    check(owner->state == S_IDLE, "Interrupted low kick must not resume its old reversal");
  }
}

static void check_reconnected_uppercuts(void) {
  const int roster[] = {5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 17};
  for (size_t row = 0; row < sizeof roster / sizeof roster[0]; ++row) {
    for (int slot = 0; slot < 2; ++slot) {
      int scenarios = row == 0 ? 5 : 1;
      for (int scenario = 0; scenario < scenarios; ++scenario) {
        battle_init(slot ? 4 : roster[row], slot ? roster[row] : 4);
        g_battle.phase = PHASE_FIGHT;
        Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
        q16 distance = scenario == 2 ? 6 * Q16_ONE : Q16_ONE / 2;
        owner->body.pos[AXIS_X] = slot ? distance / 2 : -distance / 2;
        victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
        Input input[2] = {0};
        input[slot].held = BUTTON_BIT(BUTTON_DOWN) | BUTTON_BIT(BUTTON_HIGH_PUNCH);
        if (scenario == 1) {
          input[1 - slot].held = BUTTON_BIT(BUTTON_BLOCK);
        }
        battle_step(input);
        battle_step(input);
        check(owner->state == S_UPPERCUT && fighter_move(owner)->clip == 293, "Crouch HP must enter the owned uppercut startup");
        if (scenario == 3) {
          owner->flags |= FF_FOLLOW_UP;
        }
        input[slot].held = 0;
        for (int tick = 0; tick < 120; ++tick) {
          battle_step(input);
          if (scenario == 4 && tick == 2) {
            fighter_enter_state(owner, S_HIT);
          }
        }
        q16 damage = scenario == 2 || scenario == 4 ? 0 : scenario == 1 ? q16_mul(0x28f5, 0x3333) : scenario == 3 ? 0xa3d : 0x28f5;
        if (victim->health != Q16_ONE - damage) {
          fprintf(stderr, "Uppercut roster%d slot%d scenario%d health%d expected%d\n", roster[row], slot, scenario, victim->health, Q16_ONE - damage);
        }
        check(victim->health == Q16_ONE - damage, "Uppercut must reach its native A2/A6 hit, chip, miss or interruption branch once");
        check(owner->state == S_IDLE, "Uppercut branch must finish its recovery and restore controls");
      }
    }
  }
}

static void check_roster_high_punch_frames(void) {
  const int rosters[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 16, 17};
  for (size_t row = 0; row < sizeof rosters / sizeof rosters[0]; ++row) {
    for (int slot = 0; slot < 2; ++slot) {
      for (int scenario = 0; scenario < 4; ++scenario) {
        battle_init(slot ? 4 : rosters[row], slot ? rosters[row] : 4);
        g_battle.phase = PHASE_FIGHT;
        Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
        q16 half_distance = scenario == 2 ? 3 * Q16_ONE : Q16_ONE / 2;
        owner->body.pos[AXIS_X] = slot ? half_distance : -half_distance;
        victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
        Input input[2] = {0};
        input[slot].held = BUTTON_BIT(BUTTON_HIGH_PUNCH);
        if (scenario == 1) {
          input[1 - slot].held = BUTTON_BIT(BUTTON_BLOCK);
        }
        advance_ticks(2, input);
        check(fighter_move(owner)->clip == 296 && owner->action == ACTION_RETAIL_010A, "Plain standing HP must select its native first clip and action");
        input[slot].held = 0;
        for (int tick = 0; tick < 90; ++tick) {
          input[slot].held = scenario == 3 && tick == 4 ? BUTTON_BIT(BUTTON_HIGH_PUNCH) : 0;
          battle_step(input);
        }
        q16 damage = scenario == 2 ? 0 : scenario == 1 ? q16_mul(0xccc, 0x3333) : scenario == 3 ? 2 * 0xccc : 0xccc;
        if (victim->health != Q16_ONE - damage) {
          fprintf(stderr, "HP roster%d slot%d scenario%d health%d expected%d\n", rosters[row], slot, scenario, victim->health, Q16_ONE - damage);
        }
        check(victim->health == Q16_ONE - damage && owner->state == S_IDLE, "HP must preserve its five-percent hit, buffered repeat, chip, miss and recovery");
      }
    }
  }
}

static void check_roster_jab_frames(void) {
  const int roster[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 16, 17};
  for (size_t row = 0; row < sizeof roster / sizeof roster[0]; ++row) {
    for (int slot = 0; slot < 2; ++slot) {
      for (int repeat = 0; repeat < 2; ++repeat) {
        battle_init(slot ? 4 : roster[row], slot ? roster[row] : 4);
        g_battle.phase = PHASE_FIGHT;
        Fighter *owner = &g_battle.fighters[slot];
        g_battle.fighters[0].body.pos[AXIS_X] = -3 * Q16_ONE;
        g_battle.fighters[1].body.pos[AXIS_X] = 3 * Q16_ONE;
        Input input[2] = {0};
        input[slot].held = BUTTON_BIT(BUTTON_LOW_PUNCH);
        battle_step(input);
        bool repeated = false;
        for (int tick = 0; tick < 150; ++tick) {
          input[slot].held = repeat && tick < 45 && tick % 4 == 2 ? BUTTON_BIT(BUTTON_LOW_PUNCH) : 0;
          battle_step(input);
          repeated |= fighter_move(owner)->clip == 295;
        }
        if (repeat && !repeated) {
          fprintf(stderr, "Jab frames roster%d slot%d did not repeat\n", roster[row], slot);
        }
        check(!repeat || repeated, "Repeated LP presses must reach the real second-punch loop before release");
        check(owner->state == S_IDLE && g_battle.fighters[1 - slot].health == Q16_ONE,
            "Released jab must finish its recovery and leave a distant opponent untouched");
      }
    }
  }
}

static void check_reconnected_roundhouses(void) {
  const int rosters[] = {5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 17};
  for (size_t row = 0; row < sizeof rosters / sizeof rosters[0]; ++row) {
    for (int hit = 0; hit < 2; ++hit) {
      battle_init(rosters[row], 4);
      g_battle.phase = PHASE_FIGHT;
      Fighter *owner = &g_battle.fighters[0];
      Fighter *victim = &g_battle.fighters[1];
      q16 distance = hit ? 0xe666 : 8 * Q16_ONE;
      owner->body.pos[AXIS_X] = -distance / 2;
      victim->body.pos[AXIS_X] = distance / 2;
      battle_step((Input[2]){{BUTTON_BIT(BUTTON_LEFT) | BUTTON_BIT(BUTTON_HIGH_KICK)}, {0}});
      advance_ticks(1, NULL);
      int clip = fighter_move(owner)->clip;
      check(clip == 64 || clip == 313, "Back HK must enter a roundhouse, not the ordinary high kick");
      for (int tick = 0; tick < 100; ++tick) {
        advance_ticks(1, NULL);
      }
      if (owner->state != S_IDLE || victim->health != (hit ? Q16_ONE - 0x2e14 : Q16_ONE)) {
        fprintf(stderr, "Roundhouse roster %d hit %d state %d health %d\n", rosters[row], hit, owner->state, victim->health);
      }
      check(owner->state == S_IDLE && owner->buttons != NULL, "Reconnected roundhouse must recover");
      check(victim->health == (hit ? Q16_ONE - 0x2e14 : Q16_ONE), "Roundhouse must use its retail eighteen-percent record and miss at range");
    }
  }
}

static void check_ground_normals(void) {
  const unsigned buttons[] = {BUTTON_BIT(BUTTON_HIGH_PUNCH), BUTTON_BIT(BUTTON_LOW_KICK), BUTTON_BIT(BUTTON_HIGH_KICK) | BUTTON_BIT(BUTTON_LEFT),
      BUTTON_BIT(BUTTON_DOWN) | BUTTON_BIT(BUTTON_LOW_PUNCH), BUTTON_BIT(BUTTON_DOWN) | BUTTON_BIT(BUTTON_LOW_KICK),
      BUTTON_BIT(BUTTON_DOWN) | BUTTON_BIT(BUTTON_HIGH_KICK)};
  const int clips[] = {296, 67, 313, 260, 315, 54};
  for (int i = 0; i < 6; ++i) {
    fight(4.f);
    battle_step((Input[2]){{buttons[i]}, {0}});
    advance_ticks(1, NULL);
    check(fighter_move(&g_battle.fighters[0])->clip == clips[i], "Ground button selects wrong script");
    advance_ticks(80, NULL);
    check(g_battle.fighters[0].state == S_IDLE && g_battle.fighters[1].health == Q16_ONE, "Ground miss must recover without damage");
  }
  {
    fight(1.f);
    battle_step((Input[2]){{BUTTON_BIT(BUTTON_LOW_KICK)}, {0}});
    advance_ticks(12, NULL);
    check(g_battle.fighters[1].health == Q16_ONE - 7208, "Standing LK uses its eleven-percent record");
  }
  {
    fight(0.65f);
    Fighter *fighter = &g_battle.fighters[0];
    battle_step((Input[2]){{BUTTON_BIT(BUTTON_HIGH_PUNCH)}, {0}});
    advance_ticks(8, NULL);
    check(fighter_move(fighter)->clip == 262 && (fighter->flags & FF_FOLLOW_UP), "Close HP must open its own cancel table on contact");
    battle_step((Input[2]){{BUTTON_BIT(BUTTON_HIGH_PUNCH)}, {0}});
    advance_ticks(1, NULL);
    check(fighter_move(fighter)->clip == 261, "Close HP follow-up must use combo clip 261");
    advance_ticks(40, NULL);
    check(fighter->state == S_IDLE && !(fighter->flags & FF_FOLLOW_UP), "Combo must close its cancel window on return");
  }
  {
    fight(1.f);
    Fighter *fighter = &g_battle.fighters[0];
    battle_step((Input[2]){{BUTTON_BIT(BUTTON_HIGH_PUNCH)}, {0}});
    advance_ticks(4, NULL);
    battle_step((Input[2]){{BUTTON_BIT(BUTTON_HIGH_PUNCH)}, {0}});
    advance_ticks(7, NULL);
    check(fighter_move(fighter)->clip == 297, "Repeated far HP must advance to its second punch clip");
    advance_ticks(50, NULL);
    check(fighter->state == S_IDLE, "HP repeat must stop when presses stop");
  }
}

static void finish_window(int winner, float distance, int arena) {
  battle_init(0, 0);
  g_battle.phase = PHASE_FIGHT;
  g_battle.arena = arena;
  g_battle.fighters[winner].wins = 1;
  battle_damage(&g_battle.fighters[1 - winner], Q16_ONE);
  advance_ticks(1, NULL);
  check(g_battle.phase == PHASE_FINISH, "Deciding KO must prepare finish window");
  for (int i = 0; i < 180 && g_battle.finish_waiting; ++i) {
    advance_ticks(1, NULL);
  }
  check(!g_battle.finish_waiting, "Defeat recovery must reach the finish controller");
  g_battle.fighters[0].body.pos[AXIS_X] = (q16)(-distance * .5f * Q16_ONE);
  g_battle.fighters[1].body.pos[AXIS_X] = (q16)(distance * .5f * Q16_ONE);
}

static void check_branch_frame_lp_throw(int roster, int extended_clip, uint32_t alternate) {
  for (int slot = 0; slot < 2; ++slot) {
    /* A zero alternate marks a roster whose 4de79c slot reaches only the short throw. */
    for (int extended = 0; extended < (alternate != 0 ? 2 : 1); ++extended) {
      for (int interrupted = 0; interrupted < 2; ++interrupted) {
        battle_init(slot ? 4 : roster, slot ? roster : 4);
        g_battle.phase = PHASE_FIGHT;
        Fighter *owner = &g_battle.fighters[slot];
        Fighter *victim = &g_battle.fighters[1 - slot];
        owner->body.pos[AXIS_X] = slot ? Q16_ONE / 4 : -Q16_ONE / 4;
        victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
        fighter_press(owner, BUTTON_LOW_PUNCH);
        advance_ticks(1, NULL);
        check(fighter_move(owner)->clip == 76, "Close LP input must reach Branch LP's selected capture");
        if (extended) {
          /* Exercise the real alternate VM entry with its acquired pair;
           * arena-dependent native branch selection is a separate boundary. */
          retail_vm_enter_owned(owner, alternate, fighter_ready, (unsigned)roster);
        }
        check(fighter_move(owner)->clip == (extended ? extended_clip : 76), "Branch LP LP entries must retain both reviewed owner/victim branches");
        advance_ticks(8, NULL);
        if (interrupted) {
          fighter_enter_state(owner, S_HIT);
        }
        bool selected_victim = false;
        for (int tick = 0; tick < 160; ++tick) {
          advance_ticks(1, NULL);
          selected_victim |= fighter_move(victim)->clip == (extended ? 172 : 142);
        }
        if (!interrupted && (roster == 7 || roster == 10 || roster == 17)) {
          check(selected_victim, "Released LP victim must execute its selected generated reaction rather than an approximate fallback");
        }
        q16 damage = interrupted ? 0 : extended ? 11796 : 9830;
        check(victim->health == Q16_ONE - damage && owner->health == Q16_ONE,
            "Branch LP throw must damage its captured victim once and never react against its owner");
        check(owner->state == S_IDLE && !(owner->flags & FF_PAIRED) && !(victim->flags & FF_PAIRED),
            "Branch LP LP throw or cancellation must release the pair and return controls");
      }
    }
  }
}

static void check_peer_release_lp_throw(int roster, int clip) {
  for (int slot = 0; slot < 2; ++slot) {
    for (int interrupted = 0; interrupted < 2; ++interrupted) {
      battle_init(slot ? 4 : roster, slot ? roster : 4);
      g_battle.phase = PHASE_FIGHT;
      Fighter *owner = &g_battle.fighters[slot];
      Fighter *victim = &g_battle.fighters[1 - slot];
      owner->body.pos[AXIS_X] = slot ? Q16_ONE / 4 : -Q16_ONE / 4;
      victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
      fighter_press(owner, BUTTON_LOW_PUNCH);
      advance_ticks(1, NULL);
      check(fighter_move(owner)->clip == clip, "Close LP input must reach Peer-release LP's selected capture");
      advance_ticks(8, NULL);
      if (interrupted) {
        fighter_enter_state(owner, S_HIT);
      }
      for (int tick = 0; tick < 160; ++tick) {
        advance_ticks(1, NULL);
      }
      q16 damage = interrupted ? 0 : 9830;
      check(victim->health == Q16_ONE - damage && owner->health == Q16_ONE,
          "Peer-release LP throw must damage its captured victim once and never react against its owner");
      check(owner->state == S_IDLE && !(owner->flags & FF_PAIRED) && !(victim->flags & FF_PAIRED),
          "Peer-release LP LP throw or cancellation must release the pair and return controls");
    }
  }
}

static void check_goro_frame_lp_throw(void) {
  for (int slot = 0; slot < 2; ++slot) {
    for (int interrupted = 0; interrupted < 2; ++interrupted) {
      battle_init(slot ? 4 : 15, slot ? 15 : 4);
      g_battle.phase = PHASE_FIGHT;
      Fighter *owner = &g_battle.fighters[slot];
      Fighter *victim = &g_battle.fighters[1 - slot];
      owner->body.pos[AXIS_X] = slot ? Q16_ONE / 4 : -Q16_ONE / 4;
      victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
      fighter_press(owner, BUTTON_LOW_PUNCH);
      advance_ticks(1, NULL);
      check(fighter_move(owner)->clip == 604, "Close LP input must reach Goro LP's selected capture");
      advance_ticks(8, NULL);
      if (interrupted) {
        fighter_enter_state(owner, S_HIT);
      }
      for (int tick = 0; tick < 300; ++tick) {
        advance_ticks(1, NULL);
      }
      q16 damage = interrupted ? 0 : 10 * 1966 + 9830;
      check(victim->health == Q16_ONE - damage && owner->health == Q16_ONE,
          "Goro LP throw must damage its captured victim through its five damage cycles and never react against its owner");
      check(owner->state == S_IDLE && !(owner->flags & FF_PAIRED) && !(victim->flags & FF_PAIRED),
          "Goro LP LP throw or cancellation must release the pair and return controls");
    }
  }
}

static void check_selected_high_kick(void) {
  const int roster[] = {5, 6, 8, 10, 11, 12, 14};
  for (size_t index = 0; index < sizeof roster / sizeof roster[0]; ++index) {
    for (int scenario = 0; scenario < 3; ++scenario) {
      battle_init(roster[index], 4);
      g_battle.phase = PHASE_FIGHT;
      Fighter *owner = &g_battle.fighters[0];
      Fighter *victim = &g_battle.fighters[1];
      q16 distance = scenario == 2 ? 5 * Q16_ONE : Q16_ONE;
      owner->body.pos[AXIS_X] = -distance / 2;
      victim->body.pos[AXIS_X] = distance / 2;
      Input input[2] = {0};
      if (scenario == 1) {
        input[1].held = BUTTON_BIT(BUTTON_BLOCK);
      }
      fighter_press(owner, BUTTON_HIGH_KICK);
      for (int tick = 0; tick < 45; ++tick) {
        advance_ticks(1, input);
      }
      /* B[0]/B[1]/B[2] parameter runners share 13% damage and native 20% chip. */
      q16 damage = scenario == 0 ? 0x2147 : scenario == 1 ? (int)(((int64_t)0x2147 * 0x3333) >> 16) : 0;
      if (victim->health != Q16_ONE - damage) {
        fprintf(stderr, "Selected kick roster%d scenario%d health%d expected%d\n", roster[index], scenario, victim->health, Q16_ONE - damage);
      }
      check(victim->health == Q16_ONE - damage, "Selected high kick must latch one native contact, chip or miss");
      check(owner->state == S_IDLE, "Selected high kick must finish its own clip recovery");
    }
  }
}

static void check_weapon_frame_startups(void) {
  for (int roster = 1; roster <= 3; ++roster) {
    for (int slot = 0; slot < 2; ++slot) {
      for (int high = 0; high < 2; ++high) {
        battle_init(slot ? 4 : roster, slot ? roster : 4);
        g_battle.phase = PHASE_FIGHT;
        Fighter *owner = &g_battle.fighters[slot];
        g_battle.fighters[0].body.pos[AXIS_X] = -4 * Q16_ONE;
        g_battle.fighters[1].body.pos[AXIS_X] = 4 * Q16_ONE;
        check(battle_cpu_action(owner, CPU_DRAW), "Weapon draw must retain its real CPU entry");
        for (int tick = 0; tick < 70; ++tick) {
          advance_ticks(1, NULL);
        }
        check(owner->armed && owner->state == S_IDLE, "Frame draw must equip and restore armed controls");
        fighter_enter_state(owner, S_JUMP);
        owner->body.pos[AXIS_Y] = -2 * Q16_ONE;
        owner->body.vel[AXIS_Y] = -0x1000;
        owner->body.gravity = 0x28f;
        Input input[2] = {0};
        input[slot].held = BUTTON_BIT(high ? BUTTON_HIGH_PUNCH : BUTTON_LOW_PUNCH);
        battle_step(input);
        battle_step(input);
        check(fighter_move(owner)->clip == (high ? 252 : 251), "Armed air punches must select their weapon clip");
        for (int tick = 0; tick < 90; ++tick) {
          advance_ticks(1, NULL);
        }
        check(owner->state == S_IDLE && owner->armed, "Weapon air landing must restore armed controls");
      }
    }
  }
}

static void check_reaction_motion_components(void) {
  for (int slot = 0; slot < 2; ++slot) {
    for (int behind = 0; behind < 2; ++behind) {
      fight(2.f);
      g_battle.view = (Vec3){0, 0, behind ? 1 : -1};
      Fighter *fighter = &g_battle.fighters[slot];
      /* Native490910 with an X-aligned pair: heading -.04, lateral .05.
       * Camera reversal changes lateral depth; changing slots reverses X. */
      mk4_script_reaction_motion(fighter, -0xa3d, 0xccc, -0x147a, 0xde);
      check(fighter->body.vel[AXIS_X] == (slot ? 0xa3d : -0xa3d) && fighter->body.vel[AXIS_Z] == (behind ? 0xccc : -0xccc),
          "Reaction motion must combine independent heading and camera-side components for both players");
      check(fighter->body.vel[AXIS_Y] == -0x147a && fighter->body.gravity == 0xde, "Horizontal reaction composition must preserve native vertical motion");
    }
  }
}

static void check_kai_weapon_commands(void) {
  for (int slot = 0; slot < 2; ++slot) {
    for (int strike = 0; strike < 3; ++strike) {
      for (int scenario = 0; scenario < 4; ++scenario) {
        battle_init(slot ? 4 : 11, slot ? 11 : 4);
        g_battle.phase = PHASE_FIGHT;
        Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
        g_battle.fighters[0].body.pos[AXIS_X] = -4 * Q16_ONE;
        g_battle.fighters[1].body.pos[AXIS_X] = 4 * Q16_ONE;
        Button back = slot ? BUTTON_RIGHT : BUTTON_LEFT;
        command(slot, BUTTON_DOWN, back, BUTTON_LOW_PUNCH);
        check(fighter_move(owner)->clip == 244 && !owner->armed, "Kai's real draw command must play before equipping");
        advance_ticks(65, NULL);
        check(owner->armed && owner->weapon->type == 0x63 && owner->weapon->model == SCENE_MODEL_KAI_WEAPON && owner->state == S_IDLE,
            "Kai must equip his own extracted weapon rig and persistent armed controls");
        q16 half_distance = scenario == 2 ? 4 * Q16_ONE : 0xa666;
        owner->body.pos[AXIS_X] = slot ? half_distance : -half_distance;
        victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
        if (scenario == 3) {
          victim->health = 1;
        }
        Input input[2] = {0};
        input[slot].held = BUTTON_BIT(strike == 1 ? BUTTON_LOW_PUNCH : BUTTON_HIGH_PUNCH);
        if (strike == 2) {
          input[slot].held |= BUTTON_BIT(back);
        }
        if (scenario == 1) {
          input[1 - slot].held = BUTTON_BIT(BUTTON_BLOCK);
        }
        advance_ticks(2, input);
        const int clips[] = {230, 229, 226};
        check(fighter_move(owner)->clip == clips[strike], "Kai armed punches must select their own native strike rather than a normal or recursive callback");
        input[slot].held = 0;
        for (int tick = 0; tick < 180; ++tick) {
          battle_step(input);
        }
        q16 damage = scenario == 2 ? 0 : strike == 1 ? 10485 : 11796;
        if (scenario == 1) {
          damage = q16_mul(damage, 0x3333);
        }
        q16 expected = scenario == 3 ? 0 : Q16_ONE - damage;
        if (victim->health != expected) {
          fprintf(stderr, "Kai weapon slot%d strike%d scenario%d health%d expected%d\n", slot, strike, scenario, victim->health, expected);
        }
        check(victim->health == expected, "Kai weapon records must preserve native damage, chip and miss behavior");
        check(owner->state == S_IDLE && owner->armed && owner->body.physics == NULL && owner->body.vel[AXIS_X] == 0,
            "Kai weapon recovery must clear travel and restore armed controls");
        check(victim->body.ground == roster_def(victim->roster)->ground, "Weapon victim positioning must preserve the arena floor baseline");
        if (scenario == 3) {
          check(victim->current_mmove == NULL, "Weapon KO recovery must retire without standing the defeated victim");
          battle_reset_match();
          check(!owner->armed, "Rematch must clear the weapon and both move lifetimes");
        } else if (scenario == 2 && strike == 2) {
          input[slot].held = BUTTON_BIT(BUTTON_BLOCK) | BUTTON_BIT(BUTTON_LOW_KICK);
          advance_ticks(2, input);
          input[slot].held = 0;
          advance_ticks(100, input);
          check(fighter_move(owner)->clip == 4 && owner->armed, "Armed handstand must follow its native intro/front-kick branch into persistent controls");
        }
      }
    }
  }
}

/* Jarek: cannonball 47b310 (B,F+LK, CPU rows) and upward ball 4797f0
 * (F,D,F+HP) share D[19] and the 47d940 contact recoil. */
static void check_jarek_balls(void) {
  for (int slot = 0; slot < 2; ++slot) {
    for (int kind = 0; kind < 2; ++kind) {
      for (int branch = 0; branch < 4; ++branch) {
        battle_init(slot ? 4 : 12, slot ? 12 : 4);
        g_battle.phase = PHASE_FIGHT;
        Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
        q16 distance = branch == 0 ? 12 * Q16_ONE : Q16_ONE;
        owner->body.pos[AXIS_X] = slot ? distance / 2 : -distance / 2;
        victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
        Button back = slot ? BUTTON_RIGHT : BUTTON_LEFT, forward = slot ? BUTTON_LEFT : BUTTON_RIGHT;
        Input input[2] = {0};
        if (branch == 2) {
          input[1 - slot].held = BUTTON_BIT(BUTTON_BLOCK);
        }
        fighter_press(owner, kind ? forward : back);
        if (kind) {
          fighter_press(owner, BUTTON_DOWN);
        }
        fighter_press(owner, forward);
        fighter_press(owner, kind ? BUTTON_HIGH_PUNCH : BUTTON_LOW_KICK);
        advance_ticks(1, input);
        check(fighter_move(owner)->clip == 301 && owner->action == (kind ? 0x21a : 0x214), "Jarek ball commands must bind their tuck clip and native action");
        bool recoiled = false, interrupted = false;
        for (int tick = 0; tick < 150 && owner->state != S_IDLE; ++tick) {
          advance_ticks(1, input);
          recoiled |= owner->action == 0x1010;
          if (branch == 3 && tick == 2) {
            fighter_enter_state(owner, S_HIT);
            interrupted = true;
          }
        }
        if (branch == 3) {
          check(interrupted && victim->health == Q16_ONE, "An interrupted ball must not leave a delayed contact behind");
        } else {
          if (owner->state != S_IDLE) {
            fprintf(stderr, "Jarek ball slot%d kind%d branch%d state%d action%x clip%d y%d vy%d\n", slot, kind, branch, owner->state, owner->action,
                fighter_move(owner)->clip, owner->body.pos[AXIS_Y], owner->body.vel[AXIS_Y]);
          }
          check(owner->state == S_IDLE && !fighter_airborne(owner) && owner->body.root_y == 0 && owner->body.angle[AXIS_X] == 0,
              "Jarek balls must land upright and return to control");
          check(recoiled == (branch != 0), "Only a D[19] contact may enter the 47d940 recoil");
          q16 expected = branch == 0 ? Q16_ONE : branch == 2 ? Q16_ONE - q16_mul(0x23d7, 0x3333) : Q16_ONE - 0x23d7;
          check(victim->health == expected, "Jarek balls must apply D[19] damage once, or block chip");
          if (branch == 1) {
            check(victim->reaction == 42, "A ball hit must select reaction 42");
          }
        }
        advance_ticks(240, NULL);
        check(victim->state == S_IDLE && owner->body.physics == NULL, "The ball victim must get up and both actors must retire native motion");
      }
    }
    battle_init(slot ? 4 : 12, slot ? 12 : 4);
    g_battle.phase = PHASE_FIGHT;
    check(battle_cpu_action(&g_battle.fighters[slot], CPU_SPECIAL_NEAR), "Jarek CPU special rows must be registered");
    advance_ticks(1, NULL);
    check(fighter_move(&g_battle.fighters[slot])->clip == 301 && g_battle.fighters[slot].action == 0x214, "Jarek CPU rows must select the cannonball");
    advance_ticks(180, NULL);
  }
}

/* Jarek: B,D,B+HK 47b5d0 stamps the floor; 47b600 hurts only a grounded peer. */
static void check_jarek_quake(void) {
  for (int slot = 0; slot < 2; ++slot) {
    for (int airborne = 0; airborne < 2; ++airborne) {
      battle_init(slot ? 4 : 12, slot ? 12 : 4);
      g_battle.phase = PHASE_FIGHT;
      Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
      owner->body.pos[AXIS_X] = slot ? 3 * Q16_ONE : -3 * Q16_ONE;
      victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
      Button back = slot ? BUTTON_RIGHT : BUTTON_LEFT;
      fighter_press(owner, back);
      fighter_press(owner, BUTTON_DOWN);
      fighter_press(owner, back);
      fighter_press(owner, BUTTON_HIGH_KICK);
      advance_ticks(1, NULL);
      check(fighter_move(owner)->clip == 107 && owner->action == 0x105, "Jarek's B,D,B+HK must enter his own 4e81a8 stamp");
      if (airborne) {
        victim->body.pos[AXIS_Y] = victim->body.ground - 2 * Q16_ONE;
        victim->body.gravity = 0;
      }
      bool reacted = false;
      for (int tick = 0; tick < 120 && owner->state != S_IDLE; ++tick) {
        advance_ticks(1, NULL);
        if (airborne) {
          victim->body.pos[AXIS_Y] = victim->body.ground - 2 * Q16_ONE;
        }
        reacted |= victim->reaction == 136;
      }
      check(owner->state == S_IDLE, "Jarek's stamp must finish clip 107 and return");
      check(airborne ? victim->health == Q16_ONE && !reacted : victim->health == Q16_ONE - 0x23d7 && reacted,
          "The stamp must hit only a grounded peer, once, with reaction 136");
      if (!airborne) {
        advance_ticks(120, NULL);
        check(victim->state == S_IDLE, "The reaction-136 stumble must return the victim to control");
      }
    }
  }
}

/* Jarek: D,B+LP and the CPU projectile row 497540 throw the 4ed170 blade. */
static void check_jarek_blade(void) {
  for (int slot = 0; slot < 2; ++slot) {
    for (int branch = 0; branch < 4; ++branch) {
      battle_init(slot ? 4 : 12, slot ? 12 : 4);
      g_battle.phase = PHASE_FIGHT;
      Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
      owner->body.pos[AXIS_X] = slot ? 2 * Q16_ONE : -2 * Q16_ONE;
      victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
      Input input[2] = {0};
      if (branch == 1) {
        input[1 - slot].held = BUTTON_BIT(BUTTON_BLOCK);
      } else if (branch == 2) {
        input[1 - slot].held = BUTTON_BIT(BUTTON_DOWN);
      }
      if (branch == 3) {
        check(battle_cpu_action(owner, CPU_PROJECTILE), "Jarek's CPU projectile row must be registered");
      } else {
        fighter_press(owner, BUTTON_DOWN);
        fighter_press(owner, slot ? BUTTON_RIGHT : BUTTON_LEFT);
        fighter_press(owner, BUTTON_LOW_PUNCH);
      }
      advance_ticks(1, input);
      check(fighter_move(owner)->clip == 27 && owner->action == 0x310, "Jarek's blade throw must enter 4ed118 with action 310");
      bool spawned = false;
      for (int tick = 0; tick < 120; ++tick) {
        advance_ticks(1, input);
        for (int i = 0; i < MAX_PROJECTILES; ++i) {
          spawned |= g_battle.projectiles[i].kind == PROJECTILE_FIREBALL && g_battle.projectiles[i].mesh_animation != NULL;
        }
      }
      check(spawned && owner->state == S_IDLE, "The blade must leave as an independent object while Jarek recovers");
      q16 expected = branch == 0 || branch == 3 ? Q16_ONE - 0x1eb8 : branch == 1 ? Q16_ONE - q16_mul(0x1eb8, 0x3333) : Q16_ONE;
      if (victim->health != expected) {
        fprintf(stderr, "Jarek blade slot%d branch%d health %d expected %d\n", slot, branch, victim->health, expected);
      }
      check(victim->health == expected, "The blade must hit once with C[4], chip on block and pass over a crouch");
      for (int i = 0; i < MAX_PROJECTILES; ++i) {
        check(g_battle.projectiles[i].kind == 0, "The blade object must expire after its stab, deflect or flight lifetime");
      }
    }
  }
}

/* Jarek's fan scene: 474e70 selects owner clip 414 cycles for a male victim
 * and the 415 cycle for a female one, then the shared release completes. */
static void check_jarek_fan_victims(void) {
  const int victims[] = {4, 2};
  for (int row = 0; row < 2; ++row) {
    battle_init(12, victims[row]);
    g_battle.phase = PHASE_FIGHT;
    g_battle.arena = 8;
    g_battle.fighters[0].wins = 1;
    battle_damage(&g_battle.fighters[1], Q16_ONE);
    advance_ticks(1, NULL);
    for (int tick = 0; tick < 180 && g_battle.finish_waiting; ++tick) {
      advance_ticks(1, NULL);
    }
    g_battle.fighters[0].body.pos[AXIS_X] = -0x6666;
    g_battle.fighters[1].body.pos[AXIS_X] = 0x6666;
    retail_fan_finish(&g_battle.fighters[0]);
    check(g_battle.phase == PHASE_FATALITY && g_battle.finisher == FINISH_FAN, "Jarek's fan entry must start the fan scene");
    bool cycled = false;
    for (int tick = 0; tick < 650; ++tick) {
      advance_ticks(1, NULL);
      cycled |= fighter_move(&g_battle.fighters[0])->clip == 414;
    }
    check(cycled == (row == 0), "The fan cycle must use clip 414 only for a non-female victim");
    check(g_battle.phase == PHASE_MATCH_OVER, "Jarek's fan scene must complete for either victim");
  }
}

/* Enter a Jarek finisher command from his generated table through presses. */
static void jarek_finish_command(Fighter *fighter, FighterFn enter, int slot) {
  const Controls *controls = roster_def(12)->controls;
  const CommandDef *selected = NULL;
  for (int i = 0; i < controls->command_count; ++i) {
    if (controls->commands[i].enter == enter) {
      selected = &controls->commands[i];
    }
  }
  check(selected != NULL, "Jarek fatality must have its own generated retail command");
  for (int i = 0; i < selected->count; ++i) {
    CommandToken token = selected->tokens[i];
    fighter_press(fighter, token == TOKEN_UP     ? BUTTON_UP
                           : token == TOKEN_DOWN ? BUTTON_DOWN
                           : token == TOKEN_BACK ? (slot ? BUTTON_RIGHT : BUTTON_LEFT)
                                                 : (slot ? BUTTON_LEFT : BUTTON_RIGHT));
  }
  fighter_press(fighter, selected->button);
  advance_ticks(1, NULL);
}

static void jarek_finish_window(int slot) {
  battle_init(slot ? 4 : 12, slot ? 12 : 4);
  g_battle.phase = PHASE_FIGHT;
  g_battle.fighters[slot].wins = 1;
  battle_damage(&g_battle.fighters[1 - slot], Q16_ONE);
  advance_ticks(1, NULL);
  for (int tick = 0; tick < 180 && g_battle.finish_waiting; ++tick) {
    advance_ticks(1, NULL);
  }
  check(battle_can_finish(&g_battle.fighters[slot]), "Jarek fatality must enter through the deciding-KO window");
}

/* 467cd0 -> 41df20: owner clip 577, victim handoff to 182/206 with the
 * separated head, victory 4e6a40 and completion after the victim hold. */
static void check_jarek_second_fatality(void) {
  for (int slot = 0; slot < 2; ++slot) {
    for (int reset_at = 0; reset_at < 2; ++reset_at) {
      jarek_finish_window(slot);
      Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
      owner->body.pos[AXIS_X] = slot ? Q16_ONE / 2 : -Q16_ONE / 2;
      victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
      check(!retail_can_finish_range(owner), "Jarek's second fatality must reject distances below 1.8");
      owner->body.pos[AXIS_X] = slot ? 0x11999 : -0x11999;
      victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
      check(retail_can_finish_range(owner) && !retail_can_finish_range(victim), "Jarek's second gate must accept 1.8..3 for the winner only");
      jarek_finish_command(owner, retail_finish_second, slot);
      check(g_battle.phase == PHASE_FATALITY && fighter_move(owner)->clip == 577, "Jarek's U,U,F,F+BLK must start his own clip-577 scene");
      bool cut = false, fall = false, victory = false;
      int limit = reset_at ? 70 : 600;
      for (int tick = 0; tick < limit && g_battle.phase == PHASE_FATALITY; ++tick) {
        advance_ticks(1, NULL);
        cut |= fighter_move(victim)->clip == 182 && (victim->hidden_joints[0] & (UINT64_C(1) << 2));
        fall |= fighter_move(victim)->clip == 206;
        victory |= fighter_move(owner)->clip == 125;
      }
      if (!reset_at) {
        check(
            cut && fall && victory && g_battle.phase == PHASE_MATCH_OVER, "Jarek's laser must separate the head, drop the victim, reach victory and complete");
      }
      battle_reset_match();
      check(victim->hidden_joints[0] == 0, "Reset must restore both fighters");
      for (int object = 0; object < MAX_PROJECTILES; ++object) {
        check(g_battle.projectiles[object].kind == 0, "Reset must remove the laser glow");
      }
    }
  }
  jarek_finish_window(0);
  g_battle.fighters[0].body.pos[AXIS_X] = -0x11999;
  g_battle.fighters[1].body.pos[AXIS_X] = 0x11999;
  check(battle_cpu_action(&g_battle.fighters[0], CPU_FINISH_2), "Jarek's CPU second fatality must be registered");
  advance_ticks(1, NULL);
  check(g_battle.phase == PHASE_FATALITY && g_battle.finisher == 26 && fighter_move(&g_battle.fighters[0])->clip == 577,
      "Jarek's CPU second fatality must enter the same scene");
}

/* 467ce0 -> 446d00: the owner clip follows the victim's roster, the victim
 * task plays 404, the heart rides joint 10 and the scene completes. */
static void check_jarek_first_fatality(void) {
  for (int run = 0; run < 3; ++run) {
    int slot = run == 1, female = run == 2;
    battle_init(slot ? (female ? 2 : 4) : 12, slot ? 12 : (female ? 2 : 4));
    g_battle.phase = PHASE_FIGHT;
    Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
    owner->wins = 1;
    battle_damage(victim, Q16_ONE);
    advance_ticks(1, NULL);
    for (int tick = 0; tick < 180 && g_battle.finish_waiting; ++tick) {
      advance_ticks(1, NULL);
    }
    owner->body.pos[AXIS_X] = slot ? 0x8ccc : -0x8ccc;
    victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
    check(!retail_can_finish_close(owner), "Jarek's first fatality must reject distances above one unit");
    owner->body.pos[AXIS_X] = slot ? 0x6666 : -0x6666;
    victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
    check(retail_can_finish_close(owner) && !retail_can_finish_close(victim), "Jarek's first gate must accept the close winner only");
    jarek_finish_command(owner, retail_finish_first, slot);
    check(g_battle.phase == PHASE_FATALITY, "Jarek's F,B,F,F+LK must start his own scene");
    bool owner_clip = false, ripped = false, heart = false;
    for (int tick = 0; tick < 700 && g_battle.phase == PHASE_FATALITY; ++tick) {
      advance_ticks(1, NULL);
      owner_clip |= fighter_move(owner)->clip == (female ? 403 : 402);
      check(fighter_move(owner)->clip != (female ? 402 : 403), "Jarek's heart rip must select the owner clip by victim roster");
      ripped |= fighter_move(victim)->clip == 404 && fighter_sample(victim) >= 148;
      for (int i = 0; i < MAX_PROJECTILES; ++i) {
        heart |= g_battle.projectiles[i].kind == PROJECTILE_EFFECT && g_battle.projectiles[i].mesh_animation != NULL;
      }
    }
    if (!(owner_clip && ripped && heart && g_battle.phase == PHASE_MATCH_OVER)) {
      fprintf(stderr, "Jarek F1 run%d owner%d ripped%d heart%d phase%d oclip%d os%d vclip%d vs%d\n", run, owner_clip, ripped, heart, g_battle.phase,
          fighter_move(owner)->clip, fighter_sample(owner), fighter_move(victim)->clip, fighter_sample(victim));
    }
    check(owner_clip && ripped && heart && g_battle.phase == PHASE_MATCH_OVER, "Jarek's heart rip must play both actors, attach the heart and complete");
    battle_reset_match();
  }
  jarek_finish_window(0);
  g_battle.fighters[0].body.pos[AXIS_X] = -0x6666;
  g_battle.fighters[1].body.pos[AXIS_X] = 0x6666;
  check(battle_cpu_action(&g_battle.fighters[0], CPU_FINISH_1), "Jarek's CPU first fatality must be registered");
  advance_ticks(1, NULL);
  check(g_battle.phase == PHASE_FATALITY && g_battle.finisher == 25, "Jarek's CPU first fatality must enter the same scene");
}

static void check_jarek_weapon_commands(void) {
  for (int slot = 0; slot < 2; ++slot) {
    for (int strike = 0; strike < 4; ++strike) {
      for (int scenario = 0; scenario < 4; ++scenario) {
        battle_init(slot ? 4 : 12, slot ? 12 : 4);
        g_battle.phase = PHASE_FIGHT;
        Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
        g_battle.fighters[0].body.pos[AXIS_X] = -4 * Q16_ONE;
        g_battle.fighters[1].body.pos[AXIS_X] = 4 * Q16_ONE;
        Button back = slot ? BUTTON_RIGHT : BUTTON_LEFT;
        command(slot, slot ? BUTTON_LEFT : BUTTON_RIGHT, slot ? BUTTON_LEFT : BUTTON_RIGHT, BUTTON_HIGH_PUNCH);
        check(fighter_move(owner)->clip == 244 && !owner->armed, "Jarek's real draw command must play before equipping");
        advance_ticks(65, NULL);
        check(owner->armed && owner->weapon->type == 0x6a && owner->weapon->model == SCENE_MODEL_JAREK_WEAPON && owner->state == S_IDLE,
            "Jarek must equip his own extracted weapon rig and persistent armed controls");
        q16 half_distance = scenario == 2 ? 4 * Q16_ONE : 0xa666;
        owner->body.pos[AXIS_X] = slot ? half_distance : -half_distance;
        victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
        if (scenario == 3) {
          victim->health = 1;
        }
        Input input[2] = {0};
        input[slot].held = BUTTON_BIT(strike == 1 || strike == 3 ? BUTTON_LOW_PUNCH : BUTTON_HIGH_PUNCH);
        if (strike >= 2) {
          input[slot].held |= BUTTON_BIT(back);
        }
        if (scenario == 1) {
          input[1 - slot].held = BUTTON_BIT(BUTTON_BLOCK);
        }
        advance_ticks(2, input);
        const int clips[] = {239, 229, 226, 228};
        check(fighter_move(owner)->clip == clips[strike], "Jarek armed punches must select their own native strike rather than a normal or recursive callback");
        input[slot].held = 0;
        for (int tick = 0; tick < 180; ++tick) {
          battle_step(input);
        }
        q16 damage = scenario == 2 ? 0 : strike == 0 ? 10485 : strike == 2 ? 11796 : 9830;
        if (scenario == 1) {
          damage = q16_mul(damage, 0x3333);
        }
        q16 expected = scenario == 3 ? 0 : Q16_ONE - damage;
        if (victim->health != expected) {
          fprintf(stderr, "Jarek weapon slot%d strike%d scenario%d health%d expected%d\n", slot, strike, scenario, victim->health, expected);
        }
        check(victim->health == expected, "Jarek weapon records must preserve native damage, chip and miss behavior");
        check((owner->state == S_IDLE || (scenario == 3 && owner->state == S_VICTORY)) && owner->armed && owner->body.physics == NULL &&
                  owner->body.vel[AXIS_X] == 0,
            "Jarek weapon recovery must clear travel and restore armed controls");
        check(victim->body.ground == roster_def(victim->roster)->ground, "Weapon victim positioning must preserve the arena floor baseline");
        if (scenario == 3) {
          check(victim->current_mmove == NULL, "Weapon KO recovery must retire without standing the defeated victim");
          battle_reset_match();
          check(!owner->armed, "Rematch must clear the weapon and both move lifetimes");
        }
      }
    }
  }
}

/* Shinnok: 469a00..469ad0 morphs and the 48e1e0 neutral-reset restore. */
static bool queued_sound(int bank, int selector) {
  for (int i = 0; i < g_battle.sound_count; ++i) {
    if (g_battle.sounds[i].bank == bank && g_battle.sounds[i].selector == selector) {
      return true;
    }
  }
  return false;
}

static bool shinnok_morph_into_sub_zero(int slot) {
  /* LP record 4e4648: Down, Back + LP selects 469a40 (roster 4). */
  Button back = slot ? BUTTON_RIGHT : BUTTON_LEFT;
  command(slot, BUTTON_DOWN, back, BUTTON_LOW_PUNCH);
  bool cue = false;
  for (int tick = 0; tick < 6; ++tick) {
    cue |= queued_sound(SOUND_INDEXED, 38 + 4);
    advance_ticks(1, NULL);
  }
  return cue | queued_sound(SOUND_INDEXED, 38 + 4);
}

static bool shinnok_jab_recovers(int slot, int ticks) {
  Input input[2] = {0};
  input[slot].held = BUTTON_BIT(BUTTON_LOW_PUNCH);
  battle_step(input);
  bool restored = false;
  for (int tick = 0; tick < ticks; ++tick) {
    advance_ticks(1, NULL);
    restored |= queued_sound(SOUND_INDEXED, 38 + 6);
  }
  return restored;
}

static void check_shinnok_morph(void) {
  for (int slot = 0; slot < 2; ++slot) {
    battle_init(slot ? 0 : 6, slot ? 6 : 0);
    g_battle.phase = PHASE_FIGHT;
    Fighter *owner = &g_battle.fighters[slot];
    g_battle.fighters[0].body.pos[AXIS_X] = -3 * Q16_ONE;
    g_battle.fighters[1].body.pos[AXIS_X] = 3 * Q16_ONE;
    check(shinnok_morph_into_sub_zero(slot), "Shinnok's morph must queue the target's name cue once the four-tick hold ends");
    check(owner->roster == 4 && owner->morphed && fighter_selected_roster(owner) == 6 && owner->state == S_IDLE && owner->buttons != NULL,
        "Shinnok's morph must install the target character and return to its standing controls");
    check(
        owner->body.ground == roster_def(4)->ground && owner->body.pos[AXIS_Y] == roster_def(4)->ground, "A morph must adopt the target rig's standing ground");
    /* The target's own command table now drives the actor: Down, Forward + LP is Sub-Zero's ice. */
    Button forward = slot ? BUTTON_LEFT : BUTTON_RIGHT;
    command(slot, BUTTON_DOWN, forward, BUTTON_LOW_PUNCH);
    advance_ticks(1, NULL);
    check(fighter_move(owner)->clip == 16, "A morphed Shinnok must use the target's own special commands");
    advance_ticks(120, NULL);
    check(owner->roster == 4 && owner->state == S_IDLE, "A neutral reset inside 600 ticks must keep the morph");
    advance_ticks(600, NULL);
    check(owner->roster == 4, "The morph expires only at a neutral reset, not while standing");
    check(shinnok_jab_recovers(slot, 60), "The expiry reset must queue Shinnok's indexed name cue 0x2c");
    check(owner->roster == 6 && !owner->morphed && owner->state == S_IDLE && owner->body.ground == roster_def(6)->ground,
        "The first neutral reset 600 ticks after the morph must restore Shinnok");

    check(shinnok_morph_into_sub_zero(slot) && owner->roster == 4, "Shinnok must be able to morph again after reverting");
    battle_next_round();
    check(owner->roster == 6 && !owner->morphed, "Round setup must rebuild the actor from the selected character");
    g_battle.phase = PHASE_FIGHT;

    check(shinnok_morph_into_sub_zero(slot) && owner->roster == 4, "Morph before the round winner is known");
    g_battle.ko_winner = slot + 1;
    shinnok_jab_recovers(slot, 60);
    check(owner->roster == 6, "A round winner must restore the selected character at the next neutral reset");
    shinnok_morph_into_sub_zero(slot);
    check(owner->roster == 6, "45e6a0 must reject morph commands once a round winner exists");
  }
}

static void check_shinnok_weapon_commands(void) {
  for (int slot = 0; slot < 2; ++slot) {
    for (int strike = 0; strike < 4; ++strike) {
      for (int scenario = 0; scenario < (strike == 1 || strike == 3 ? 5 : 4); ++scenario) {
        battle_init(slot ? 4 : 6, slot ? 6 : 4);
        g_battle.phase = PHASE_FIGHT;
        Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
        g_battle.fighters[0].body.pos[AXIS_X] = -4 * Q16_ONE;
        g_battle.fighters[1].body.pos[AXIS_X] = 4 * Q16_ONE;
        Button back = slot ? BUTTON_RIGHT : BUTTON_LEFT;
        command(slot, back, slot ? BUTTON_LEFT : BUTTON_RIGHT, BUTTON_LOW_PUNCH);
        check(fighter_move(owner)->clip == 244 && !owner->armed, "Shinnok's real draw command must play before equipping");
        advance_ticks(65, NULL);
        check(owner->armed && owner->weapon->type == 0x64 && owner->weapon->model == SCENE_MODEL_SHINNOK_WEAPON && owner->state == S_IDLE,
            "Shinnok must equip his own extracted weapon rig and persistent armed controls");
        q16 half_distance = scenario == 2 ? 4 * Q16_ONE : 0xa666;
        owner->body.pos[AXIS_X] = slot ? half_distance : -half_distance;
        victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
        if (scenario == 3) {
          victim->health = 1;
        }
        Input input[2] = {0};
        input[slot].held = BUTTON_BIT(strike == 1 || strike == 3 ? BUTTON_LOW_PUNCH : BUTTON_HIGH_PUNCH);
        if (strike >= 2) {
          input[slot].held |= BUTTON_BIT(back);
        }
        if (scenario == 1) {
          input[1 - slot].held = BUTTON_BIT(BUTTON_BLOCK);
        }
        advance_ticks(2, input);
        const int clips[] = {237, 238, 242, 243};
        check(
            fighter_move(owner)->clip == clips[strike], "Shinnok armed punches must select their own native strike rather than a normal or recursive callback");
        input[slot].held = 0;
        bool crouch_getup = false;
        for (int tick = 0; tick < 180; ++tick) {
          if (scenario == 4 && victim->health < Q16_ONE) {
            input[1 - slot].held = BUTTON_BIT(BUTTON_DOWN);
          }
          battle_step(input);
          crouch_getup |= fighter_move(victim)->clip == 184 || fighter_move(victim)->clip == 196;
        }
        if (scenario == 4) {
          check(crouch_getup && victim->state == S_CROUCH, "Polearm victim must honor the native held-DOWN getup branch");
        }
        q16 damage = scenario == 2 ? 0 : strike == 0 ? 11141 : strike == 1 ? 13107 : strike == 2 ? 9830 : 11796;
        if (scenario == 1) {
          damage = q16_mul(damage, 0x3333);
        }
        q16 expected = scenario == 3 ? 0 : Q16_ONE - damage;
        if (victim->health != expected) {
          fprintf(stderr, "Shinnok weapon slot%d strike%d scenario%d health%d expected%d\n", slot, strike, scenario, victim->health, expected);
        }
        check(victim->health == expected, "Shinnok weapon records must preserve native damage, chip and miss behavior");
        check((owner->state == S_IDLE || (scenario == 3 && owner->state == S_VICTORY)) && owner->armed && owner->body.physics == NULL &&
                  owner->body.vel[AXIS_X] == 0,
            "Shinnok weapon recovery must clear travel and restore armed controls");
        check(victim->body.ground == roster_def(victim->roster)->ground, "Weapon victim positioning must preserve the arena floor baseline");
        if (scenario == 3) {
          check(victim->current_mmove == NULL, "Weapon KO recovery must retire without standing the defeated victim");
          battle_reset_match();
          check(!owner->armed, "Rematch must clear the weapon and both move lifetimes");
        }
      }
    }
  }
}

static void check_cage_weapon_commands(void) {
  for (int slot = 0; slot < 2; ++slot) {
    for (int strike = 0; strike < 4; ++strike) {
      for (int scenario = 0; scenario < (strike == 1 || strike == 2 ? 5 : 4); ++scenario) {
        battle_init(slot ? 4 : 14, slot ? 14 : 4);
        g_battle.phase = PHASE_FIGHT;
        Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
        g_battle.fighters[0].body.pos[AXIS_X] = -4 * Q16_ONE;
        g_battle.fighters[1].body.pos[AXIS_X] = 4 * Q16_ONE;
        Button back = slot ? BUTTON_RIGHT : BUTTON_LEFT;
        Input draw[2] = {0};
        draw[slot].held = BUTTON_BIT(slot ? BUTTON_LEFT : BUTTON_RIGHT);
        battle_step(draw);
        advance_ticks(2, NULL);
        command(slot, BUTTON_DOWN, slot ? BUTTON_LEFT : BUTTON_RIGHT, BUTTON_LOW_KICK);
        check(fighter_move(owner)->clip == 244 && !owner->armed, "Cage's real draw command must play before equipping");
        advance_ticks(65, NULL);
        check(owner->armed && owner->weapon->type == 0x6e && owner->weapon->model == SCENE_MODEL_CAGE_WEAPON && owner->state == S_IDLE,
            "Cage must equip his own extracted weapon rig and persistent armed controls");
        q16 half_distance = scenario == 2 ? 4 * Q16_ONE : 0xa666;
        owner->body.pos[AXIS_X] = slot ? half_distance : -half_distance;
        victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
        if (scenario == 3) {
          victim->health = 1;
        }
        Input input[2] = {0};
        input[slot].held = BUTTON_BIT(strike == 1 || strike == 3 ? BUTTON_LOW_PUNCH : BUTTON_HIGH_PUNCH);
        if (strike >= 2) {
          input[slot].held |= BUTTON_BIT(back);
        }
        if (scenario == 1) {
          input[1 - slot].held = BUTTON_BIT(BUTTON_BLOCK);
        }
        advance_ticks(2, input);
        const int clips[] = {226, 230, 239, 227};
        check(fighter_move(owner)->clip == clips[strike], "Cage armed punches must select their own native strike rather than a normal or recursive callback");
        input[slot].held = 0;
        bool crouch_getup = false;
        for (int tick = 0; tick < 180; ++tick) {
          if (scenario == 4 && victim->health < Q16_ONE) {
            input[1 - slot].held = BUTTON_BIT(BUTTON_DOWN);
          }
          battle_step(input);
          crouch_getup |= fighter_move(victim)->clip == 184 || fighter_move(victim)->clip == 190;
        }
        if (scenario == 4) {
          check(crouch_getup && victim->state == S_CROUCH, "Held DOWN must select the native crouching getup and return to crouch controls");
        }
        q16 damage = scenario == 2 ? 0 : strike == 0 ? 11796 : strike == 1 ? 9830 : strike == 2 ? 10485 : 9175;
        if (scenario == 1) {
          damage = q16_mul(damage, 0x3333);
        }
        q16 expected = scenario == 3 ? 0 : Q16_ONE - damage;
        if (victim->health != expected) {
          fprintf(stderr, "Cage weapon slot%d strike%d scenario%d health%d expected%d\n", slot, strike, scenario, victim->health, expected);
        }
        check(victim->health == expected, "Cage weapon records must preserve native damage, chip and miss behavior");
        check((owner->state == S_IDLE || (scenario == 3 && owner->state == S_VICTORY)) && owner->armed && owner->body.physics == NULL &&
                  owner->body.vel[AXIS_X] == 0,
            "Cage weapon recovery must clear travel and restore armed controls");
        check(victim->body.ground == roster_def(victim->roster)->ground, "Weapon victim positioning must preserve the arena floor baseline");
        if (scenario == 3) {
          check(victim->current_mmove == NULL, "Weapon KO recovery must retire without standing the defeated victim");
          battle_reset_match();
          check(!owner->armed, "Rematch must clear the weapon and both move lifetimes");
        }
      }
    }
  }
}

static void check_fujin_crossbow(void) {
  for (int slot = 0; slot < 2; ++slot) {
    for (int shot = 0; shot < 3; ++shot) {
      for (int scenario = 0; scenario < 5; ++scenario) {
        battle_init(slot ? 4 : 5, slot ? 5 : 4);
        g_battle.phase = PHASE_FIGHT;
        Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
        owner->body.pos[AXIS_X] = slot ? 4 * Q16_ONE : -4 * Q16_ONE;
        victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
        Button back = slot ? BUTTON_RIGHT : BUTTON_LEFT;
        command(slot, back, back, BUTTON_LOW_PUNCH);
        advance_ticks(65, NULL);
        check(owner->armed && owner->weapon->type == 0x68 && owner->weapon->model == SCENE_MODEL_FUJIN_WEAPON && owner->state == S_IDLE,
            "Fujin's Back,Back+LP must equip his extracted crossbow and armed controls");
        Input input[2] = {0};
        input[slot].held = BUTTON_BIT(shot == 1 ? BUTTON_HIGH_PUNCH : BUTTON_LOW_PUNCH) | (shot == 2 ? BUTTON_BIT(back) : 0);
        if (scenario == 1) {
          input[1 - slot].held = BUTTON_BIT(BUTTON_BLOCK);
        }
        battle_step(input);
        advance_ticks(1, input);
        check(fighter_move(owner)->clip == (shot == 1 ? 212 : shot == 2 ? 214 : 211), "Crossbow buttons must select their actual distinct shooting clips");
        input[slot].held = 0;
        bool spawned = false, independent = false, victim_pose = false;
        int launches = 0;
        for (int tick = 0; tick < 250; ++tick) {
          if (scenario == 2) {
            victim->body.pos[AXIS_Z] = 4 * Q16_ONE;
          }
          if (scenario == 3 && tick == 1) {
            battle_react(owner, 11);
          }
          for (int i = 0; i < MAX_PROJECTILES; ++i) {
            Projectile *bolt = &g_battle.projectiles[i];
            if (bolt->kind == PROJECTILE_FIREBALL && bolt->owner == slot && bolt->mesh_animation != NULL && bolt->mesh_animation->first == 106) {
              if (!spawned) {
                launches++;
                spawned = true;
                check(bolt->vel[AXIS_Y] == (shot == 1      ? -0x3333
                                               : shot == 2 ? 0x3333
                                                           : 0),
                    "Crossbow bolts must retain straight, upward and downward launch motion");
                if (scenario == 4) {
                  battle_react(owner, 11);
                  independent = true;
                }
              }
              if (scenario == 0 || scenario == 1 || scenario == 4) {
                bolt->pos[AXIS_X] = victim->body.pos[AXIS_X] - q16_mul(bolt->heading[0], 0x3333);
                bolt->pos[AXIS_Z] = victim->body.pos[AXIS_Z] - q16_mul(bolt->heading[1], 0x3333);
                bolt->pos[AXIS_Y] = victim->body.ground;
                bolt->vel[AXIS_Y] = 0;
              }
            }
          }
          battle_step(input);
          victim_pose |= fighter_move(victim)->clip == 311;
        }
        check((scenario == 3 ? !spawned : spawned && launches == 1) && owner->state == S_IDLE && owner->armed == (scenario < 3),
            "Crossbow shooting must recover; interrupting hits must drop the weapon and cancel only shots not yet launched");
        q16 damage = scenario == 2 || scenario == 3 ? 0 : scenario == 1 ? q16_mul(0xccc, 0x3333) : 0xccc;
        if (victim->health != Q16_ONE - damage) {
          fprintf(
              stderr, "Crossbow slot%d shot%d scenario%d health%d expected%d launches%d\n", slot, shot, scenario, victim->health, Q16_ONE - damage, launches);
        }
        check(victim->health == Q16_ONE - damage, "Crossbow C[16] must preserve five-percent damage, chip, miss and interruption");
        check((scenario != 0 && scenario != 4) || victim_pose, "A grounded bolt hit must play the actual311 friction reaction");
        check(scenario != 4 || independent, "A launched crossbow bolt must survive owner interruption");
        battle_reset_match();
        for (int i = 0; i < MAX_PROJECTILES; ++i) {
          check(g_battle.projectiles[i].kind == 0, "Crossbow reset must remove every independent bolt");
        }
      }
    }
    battle_init(slot ? 4 : 5, slot ? 5 : 4);
    g_battle.phase = PHASE_FIGHT;
    Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
    owner->flags |= FF_FOLLOW_UP;
    victim->flags |= FF_UNTOUCHABLE;
    fighter_interrupt(owner, fujin_weapon_action);
    int shots = 0;
    for (int tick = 0; tick < 180; ++tick) {
      battle_step((Input[2]){0});
      for (int i = 0; i < MAX_PROJECTILES; ++i) {
        Projectile *bolt = &g_battle.projectiles[i];
        shots += bolt->kind == PROJECTILE_FIREBALL && bolt->owner == slot && bolt->ticks == 1;
      }
    }
    check(shots == 4 && owner->armed && owner->state == S_IDLE, "Type68 draw follow-up must fire three short bolts and one full shot before recovery");
    battle_reset_match();
  }
}

static void check_weapon_followup_selection(void) {
  const int rosters[] = {14, 12, 6, 11};
  const FighterFn entries[] = {cage_weapon_action, jarek_weapon_action, shinnok_weapon_action, kai_draw_weapon};
  const int clips[] = {227, 229, 237, 230};
  const q16 damages[] = {9175, 9830, 13107, 11796};
  for (int row = 0; row < 4; ++row) {
    for (int slot = 0; slot < 2; ++slot) {
      for (int air = 0; air < 2; ++air) {
        battle_init(slot ? 4 : rosters[row], slot ? rosters[row] : 4);
        g_battle.phase = PHASE_FIGHT;
        Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
        owner->body.pos[AXIS_X] = slot ? 0xa666 : -0xa666;
        victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
        if (air) {
          owner->body.pos[AXIS_Y] -= 0x2000;
          owner->body.gravity = 0;
        }
        owner->flags |= FF_FOLLOW_UP;
        fighter_interrupt(owner, entries[row]);
        advance_ticks(1, NULL);
        check(owner->armed && fighter_move(owner)->clip == clips[row], "Follow-up draw must select its native table entry even from a frozen airborne contact");
        advance_ticks(180, NULL);
        if (victim->health != Q16_ONE - damages[row] || owner->state != S_IDLE) {
          fprintf(stderr, "Weapon followup row%d slot%d air%d health%d state%d clip%d pose%d\n", row, slot, air, victim->health, owner->state,
              fighter_move(owner)->clip, fighter_sample(owner));
        }
        check(victim->health == Q16_ONE - damages[row] && owner->state == S_IDLE,
            "Weapon follow-up must apply its selected record and recover without a substitute attack");
        check(owner->body.pos[AXIS_Y] == owner->body.ground && owner->body.physics == NULL,
            "Native controller return must restore the owner floor instead of leaving a frozen airborne fighter");
      }
    }
  }
}

static void check_type69_weapon_commands(void) {
  const int roster[] = {7, 13, 17};
  for (size_t index = 0; index < sizeof roster / sizeof roster[0]; ++index) {
    for (int player = 0; player < 2; ++player) {
      for (int strike = 0; strike < 4; ++strike) {
        battle_init(player ? 4 : roster[index], player ? roster[index] : 4);
        g_battle.phase = PHASE_FIGHT;
        Fighter *owner = &g_battle.fighters[player];
        Fighter *victim = &g_battle.fighters[1 - player];
        g_battle.fighters[0].body.pos[AXIS_X] = -3 * Q16_ONE;
        g_battle.fighters[1].body.pos[AXIS_X] = 3 * Q16_ONE;
        Button back = player ? BUTTON_RIGHT : BUTTON_LEFT;
        Button forward = player ? BUTTON_LEFT : BUTTON_RIGHT;
        command(player, roster[index] == 17 ? forward : BUTTON_DOWN, roster[index] == 7 ? back : forward,
            roster[index] == 17 ? BUTTON_HIGH_KICK : BUTTON_HIGH_PUNCH);
        if (fighter_move(owner)->clip != 244 || owner->armed) {
          fprintf(stderr, "Axe draw roster%d slot%d clip%d armed%d action%x\n", roster[index], player, fighter_move(owner)->clip, owner->armed, owner->action);
        }
        check(fighter_move(owner)->clip == 244 && !owner->armed, "Axe command must start its draw before equipping");
        for (int tick = 0; tick < 65; ++tick) {
          advance_ticks(1, NULL);
        }
        check(owner->armed && owner->weapon->type == 0x69 && owner->weapon->model == SCENE_MODEL_AXE && owner->state == S_IDLE,
            "All three axe commands must equip their extracted model and restore armed controls");
        Input input[2] = {0};
        input[player].held = BUTTON_BIT(strike & 1 ? BUTTON_LOW_PUNCH : BUTTON_HIGH_PUNCH);
        if (strike >= 2) {
          input[player].held |= BUTTON_BIT(back);
        }
        battle_step(input);
        battle_step(input);
        const int clips[] = {231, 232, 229, 230};
        if (fighter_move(owner)->clip != clips[strike]) {
          fprintf(stderr, "Axe strike roster%d slot%d strike%d clip%d state%d\n", roster[index], player, strike, fighter_move(owner)->clip, owner->state);
        }
        check(fighter_move(owner)->clip == clips[strike], "Armed buttons must enter the matching axe strike, without recursive callbacks");
        for (int tick = 0; tick < 100; ++tick) {
          advance_ticks(1, NULL);
        }
        check(owner->state == S_IDLE && victim->health == Q16_ONE && owner->body.physics == NULL,
            "Missed axe strike must recover, clear strafe and leave a distant victim untouched");
      }
    }
  }
}

static void check_roster_throw_state_entries(void) {
  const int roster[] = {5, 7, 10, 14, 15, 17};
  for (size_t index = 0; index < sizeof roster / sizeof roster[0]; ++index) {
    battle_init(roster[index], 4);
    g_battle.phase = PHASE_FIGHT;
    Fighter *owner = &g_battle.fighters[0];
    Fighter *victim = &g_battle.fighters[1];
    owner->body.pos[AXIS_X] = -Q16_ONE / 4;
    victim->body.pos[AXIS_X] = Q16_ONE / 4;
    fighter_enter_state(owner, S_THROW);
    check((owner->flags & FF_PAIRED) && (victim->flags & FF_PAIRED),
        "Shared throw entry must invoke the carrier's owned capture, not its approximate move header");
    advance_ticks(300, NULL);
    check(owner->state == S_IDLE && victim->health < Q16_ONE && !((owner->flags | victim->flags) & FF_PAIRED),
        "Owned shared-state throw must apply damage, release its victim and recover");
  }
}

static void check_cage_groin_gate(void) {
  for (int stale = 0; stale < 2; ++stale) {
    battle_init(14, 0);
    g_battle.phase = PHASE_FIGHT;
    g_battle.fighters[0].body.pos[AXIS_X] = -2 * Q16_ONE;
    g_battle.fighters[1].body.pos[AXIS_X] = 2 * Q16_ONE;
    Input input[2] = {0};
    if (stale) {
      input[0].held = BUTTON_BIT(BUTTON_BLOCK);
      battle_step(input);
      advance_ticks(5, input);
    }
    input[0].held = BUTTON_BIT(BUTTON_BLOCK) | BUTTON_BIT(BUTTON_LOW_PUNCH) | (stale ? 0 : BUTTON_BIT(BUTTON_HIGH_KICK));
    battle_step(input);
    advance_ticks(1, NULL);
    check(fighter_move(&g_battle.fighters[0])->clip != 438, "Cage groin gate must reject an extra attack switch or an old BLOCK latch");
  }
}

static void check_goro_projectile_completion(void) {
  for (int slot = 0; slot < 2; ++slot) {
    battle_init(slot ? 4 : 15, slot ? 15 : 4);
    g_battle.phase = PHASE_FIGHT;
    Fighter *owner = &g_battle.fighters[slot];
    goro_fireball(owner);
    for (int tick = 0; tick < 100; ++tick) {
      advance_ticks(1, NULL);
    }
    check(owner->state == S_IDLE, "Goro projectile must return its owner to controls after recovery");
  }
}

static void check_short_diagonal_air_recovery(void) {
  battle_init(4, 0);
  g_battle.phase = PHASE_FIGHT;
  Fighter *fighter = &g_battle.fighters[0];
  fighter_start_move(fighter, S_JUMP, roster_def(4)->moves[S_JUMP]);
  fighter->body.pos[AXIS_Y] = fighter->body.ground - 3 * Q16_ONE;
  fighter->body.gravity = 1;
  fighter->body.vel[AXIS_X] = 1;
  fighter_enter_stance(fighter, STANCE_DIAGONAL_AIR);
  fighter_press(fighter, BUTTON_LOW_KICK);
  advance_ticks(1, NULL);
  check(fighter_move(fighter)->clip == 59, "Diagonal LK must select Sub-Zero's six-pose clip");
  bool extended = false, reversed = false;
  for (int tick = 0; tick < 40; ++tick) {
    /* EXE descriptor 4fb040 stores six poses. An assetless simulation using
     * the old 65536-pose sentinel hid the same out-of-bounds render crash. */
    int sample = (int)fighter_frame(fighter, 6);
    extended |= sample == 5;
    reversed |= extended && sample == 0;
    advance_ticks(1, NULL);
  }
  check(extended && reversed, "Short diagonal air attack must extend and reverse within its stored poses");
}

static void check_jax_air_grab_command(void) {
  for (int slot = 0; slot < 2; ++slot) {
    for (int scenario = 0; scenario < 5; ++scenario) {
      battle_init(slot ? 4 : 13, slot ? 13 : 4);
      g_battle.phase = PHASE_FIGHT;
      Fighter *owner = &g_battle.fighters[slot];
      Fighter *victim = &g_battle.fighters[1 - slot];
      for (int player = 0; player < 2; ++player) {
        Fighter *fighter = &g_battle.fighters[player];
        fighter_start_move(fighter, S_JUMP, roster_def(fighter->roster)->moves[S_JUMP]);
        fighter->body.pos[AXIS_Y] = fighter->body.ground - 2 * Q16_ONE;
        fighter->body.gravity = 0x28f;
      }
      owner->body.pos[AXIS_X] = slot ? Q16_ONE / 4 : -Q16_ONE / 4;
      victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
      if (scenario == 3) {
        victim->health = 0x1000;
        owner->wins = 1;
      }
      fighter_press(owner, BUTTON_BLOCK);
      advance_ticks(1, NULL);
      check(fighter_move(owner)->clip == 84 && fighter_move(victim)->clip == 144 && (owner->flags & FF_PAIRED) && (victim->flags & FF_PAIRED),
          "Jax air-grab command must acquire its distinct owner/victim pair");
      advance_ticks(7, NULL);
      if (scenario == 1 || scenario == 2) {
        fighter_enter_state(scenario == 1 ? owner : victim, S_HIT);
      } else if (scenario == 4) {
        battle_init(13, 4);
        advance_ticks(5, NULL);
        check(!(g_battle.fighters[0].flags & FF_PAIRED) && !(g_battle.fighters[1].flags & FF_PAIRED), "Reset must discard the Jax air-grab pair");
        continue;
      } else {
        bool falling_pair = false;
        for (int tick = 0; tick < 40 && !falling_pair; ++tick) {
          advance_ticks(1, NULL);
          falling_pair = owner->body.gravity == 0x3d7 && victim->body.gravity == 0x3d7;
        }
        check(falling_pair, "Jax air-grab handoff must restore the native falling-pair gravity");
      }
      advance_ticks(240, NULL);
      q16 expected = scenario == 0 ? Q16_ONE - 0x2147 : scenario == 3 ? 0 : Q16_ONE;
      if (victim->health != expected || fighter_airborne(owner) || fighter_airborne(victim)) {
        fprintf(stderr, "Jax air grab slot %d scenario %d health %d ownerY %d victimY %d phase %d\n", slot, scenario, victim->health, owner->body.pos[AXIS_Y],
            victim->body.pos[AXIS_Y], g_battle.phase);
      }
      check(victim->health == expected, "Jax air-grab damage must occur once after its paired contact, with interruption and KO");
      check(!(owner->flags & FF_PAIRED) && !(victim->flags & FF_PAIRED) && !fighter_airborne(owner) && !fighter_airborne(victim),
          "Jax air-grab completion or interruption must release both actors and let them land");
    }
  }
}

static void check_reptile_reconnected_specials(void) {
  for (int slot = 0; slot < 2; ++slot) {
    for (int dash = 0; dash < 2; ++dash) {
      for (int scenario = 0; scenario < 4; ++scenario) {
        battle_init(slot ? 4 : 10, slot ? 10 : 4);
        g_battle.phase = PHASE_FIGHT;
        Fighter *owner = &g_battle.fighters[slot];
        Fighter *victim = &g_battle.fighters[1 - slot];
        owner->body.pos[AXIS_X] = slot ? Q16_ONE / 2 : -Q16_ONE / 2;
        victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
        if (scenario == 1) {
          victim->flags |= FF_UNTOUCHABLE;
        }
        Input input[2] = {0};
        if (scenario == 3) {
          input[1 - slot].held = BUTTON_BIT(BUTTON_BLOCK);
          advance_ticks(2, input);
        }
        fighter_press(owner, slot ? BUTTON_RIGHT : BUTTON_LEFT);
        fighter_press(owner, slot ? BUTTON_LEFT : BUTTON_RIGHT);
        fighter_press(owner, dash ? BUTTON_LOW_KICK : BUTTON_LOW_PUNCH);
        advance_ticks(1, input);
        check(fighter_move(owner)->clip == (dash ? 302 : 106), "Reptile motion command must enter its owned movement attack");
        if (scenario == 2) {
          fighter_enter_state(owner, S_HIT);
        }
        bool reacted = false;
        for (int tick = 0; tick < 180; ++tick) {
          advance_ticks(1, input);
          /* D[23] selects reaction 158 (clip 179); D[21] selects 40 (clip 183). */
          reacted |= fighter_move(victim)->clip == (dash ? 179 : 183);
        }
        check(reacted == (scenario == 0), "Only an unblocked Reptile special enters its own victim reaction");
        q16 damage = dash ? 0x1eb8 : 0x170a;
        q16 expected = scenario == 0 ? Q16_ONE - damage : scenario == 3 ? Q16_ONE - q16_mul(damage, 0x3333) : Q16_ONE;
        if (owner->state != S_IDLE || victim->health != expected) {
          fprintf(stderr, "Reptile slot %d dash %d scenario %d state %d health %d expected %d\n", slot, dash, scenario, owner->state, victim->health, expected);
        }
        check(victim->health == expected, "Reptile special must apply its owned contact once, with miss and interruption gates");
        check(owner->state == S_IDLE, "Reptile special must recover after its contact branch");
      }
    }
    battle_init(slot ? 4 : 10, slot ? 10 : 4);
    g_battle.phase = PHASE_FIGHT;
    check(battle_cpu_action(&g_battle.fighters[slot], CPU_SPECIAL_NEAR), "Reptile CPU dash must be registered");
    advance_ticks(1, NULL);
    check(fighter_move(&g_battle.fighters[slot])->clip == 302, "Reptile CPU near row must select the dash");
    const Controls *controls = roster_def(10)->controls;
    CommandPredicate gate = NULL;
    for (int i = 0; i < controls->command_count; ++i) {
      if (controls->commands[i].button == BUTTON_LOW_KICK && controls->commands[i].count == 2) {
        gate = controls->commands[i].predicate;
      }
    }
    check(gate != NULL && !gate(&g_battle.fighters[slot]), "Dash entry must arm the cooldown used by its command gate");
    advance_ticks(180, NULL);
    check(gate(&g_battle.fighters[slot]), "Completed dash cooldown must allow the next command");
    g_battle.fighters[1 - slot].hits = 5;
    check(!gate(&g_battle.fighters[slot]), "Dash gate must reject a victim beyond the retail combo cap");
    g_battle.fighters[1 - slot].hits = 4;
    check(gate(&g_battle.fighters[slot]), "Dash combo cap must include its fourth-hit boundary");
    battle_init(slot ? 4 : 10, slot ? 10 : 4);
    g_battle.phase = PHASE_FIGHT;
    check(battle_cpu_action(&g_battle.fighters[slot], CPU_SPECIAL_2), "Reptile CPU far attack must be registered");
    advance_ticks(1, NULL);
    check(fighter_move(&g_battle.fighters[slot])->clip == 106, "Reptile CPU far row must select the forward attack");
    advance_ticks(180, NULL);
  }
}

/* Goro frame-only specials: retail command entries in both slots. */
static void goro_scene(int slot, q16 distance, Fighter **owner, Fighter **victim) {
  battle_init(slot ? 4 : 15, slot ? 15 : 4);
  g_battle.phase = PHASE_FIGHT;
  *owner = &g_battle.fighters[slot];
  *victim = &g_battle.fighters[1 - slot];
  (*owner)->body.pos[AXIS_X] = slot ? distance / 2 : -distance / 2;
  (*victim)->body.pos[AXIS_X] = -(*owner)->body.pos[AXIS_X];
}

static void goro_tokens(Fighter *owner, const CommandToken *tokens, int count, Button button) {
  bool left = fighter_opponent(owner)->body.pos[AXIS_X] < owner->body.pos[AXIS_X];
  for (int i = 0; i < count; ++i) {
    Button direction = tokens[i] == TOKEN_UP     ? BUTTON_UP
                       : tokens[i] == TOKEN_DOWN ? BUTTON_DOWN
                       : tokens[i] == TOKEN_BACK ? (left ? BUTTON_RIGHT : BUTTON_LEFT)
                                                 : (left ? BUTTON_LEFT : BUTTON_RIGHT);
    fighter_press(owner, direction);
  }
  fighter_press(owner, button);
  advance_ticks(1, NULL);
}

static void check_goro_frame_specials(void) {
  for (int slot = 0; slot < 2; ++slot) {
    Fighter *owner, *victim;
    /* 43b320: rise, relocate above the victim, drop and stomp once for 30%. */
    goro_scene(slot, 4 * Q16_ONE, &owner, &victim);
    goro_tokens(owner, (const CommandToken[]){TOKEN_FORWARD, TOKEN_FORWARD, TOKEN_BACK}, 3, BUTTON_HIGH_KICK);
    check(fighter_move(owner)->clip == 601 && owner->action == 0x409, "Goro HK F,F,B must enter his clip-601 leap stomp");
    q16 lowest = 0;
    for (int tick = 0; tick < 40; ++tick) {
      advance_ticks(1, NULL);
      lowest = owner->body.pos[AXIS_Y] < lowest ? owner->body.pos[AXIS_Y] : lowest;
    }
    check(lowest <= -0x3cccc, "Goro leap must rise past its relocation height");
    advance_ticks(400, NULL);
    check(victim->health == Q16_ONE - 0x4ccc, "Goro leap stomp must deal its native 30% once");
    check(owner->state == S_IDLE && victim->state == S_IDLE && !(owner->flags & FF_HOLD_SEPARATION) && owner->body.physics == NULL,
        "Goro leap stomp and its 191 victim must both return to control");

    /* Interrupting the leap before its drop must not leave a later stomp. */
    goro_scene(slot, 4 * Q16_ONE, &owner, &victim);
    goro_tokens(owner, (const CommandToken[]){TOKEN_FORWARD, TOKEN_FORWARD, TOKEN_BACK}, 3, BUTTON_HIGH_KICK);
    advance_ticks(20, NULL);
    battle_react(owner, 10);
    advance_ticks(400, NULL);
    check(victim->health == Q16_ONE && owner->body.physics != goro_stomp_hold_separation && owner->state == S_IDLE,
        "Interrupted Goro leap must cancel its drop and stomp hold");

    /* 43b7e0: the landing quake damages only a grounded victim. */
    for (int airborne = 0; airborne < 2; ++airborne) {
      goro_scene(slot, 3 * Q16_ONE, &owner, &victim);
      goro_tokens(owner, (const CommandToken[]){TOKEN_BACK, TOKEN_FORWARD, TOKEN_DOWN, TOKEN_DOWN}, 4, BUTTON_HIGH_KICK);
      check(fighter_move(owner)->clip == 602, "Goro HK B,F,D,D must blend into clip 602");
      if (airborne) {
        victim->body.pos[AXIS_Y] = -0x40000;
        victim->body.gravity = 0x147;
      }
      advance_ticks(300, NULL);
      check(victim->health == (airborne ? Q16_ONE : Q16_ONE - 0x3d70), "Goro quake must damage only a grounded victim");
      check(owner->state == S_IDLE && victim->state == S_IDLE, "Goro quake and reaction 136 must return to control");
    }

    /* 43b640: close clip-600 uppercut, D[35] and reaction 192. */
    goro_scene(slot, Q16_ONE, &owner, &victim);
    goro_tokens(owner, (const CommandToken[]){TOKEN_DOWN, TOKEN_DOWN}, 2, BUTTON_HIGH_PUNCH);
    check(fighter_move(owner)->clip == 600, "Goro HP D,D must bind clip 600");
    advance_ticks(300, NULL);
    check(victim->health == Q16_ONE - 0x23d7 && victim->state == S_IDLE && owner->state == S_IDLE,
        "Goro uppercut must deal D[35] once and release reaction 192 to its getup");

    /* 49ca70: HP F,B launches the independent C[22] object. */
    goro_scene(slot, 3 * Q16_ONE, &owner, &victim);
    goro_tokens(owner, (const CommandToken[]){TOKEN_FORWARD, TOKEN_BACK}, 2, BUTTON_HIGH_PUNCH);
    check(fighter_move(owner)->clip == 592, "Goro HP F,B must enter his clip-592 fireball");
    advance_ticks(120, NULL);
    check(victim->health == Q16_ONE - 0x23d7 && owner->state == S_IDLE, "Goro fireball must hit once with C[22] and return its owner");

    /* 460f40/461030 stance selections: standing HP and back+HK. */
    goro_scene(slot, 3 * Q16_ONE, &owner, &victim);
    fighter_press(owner, BUTTON_HIGH_PUNCH);
    advance_ticks(1, NULL);
    check(fighter_move(owner)->clip == 296, "Goro ranged HP must enter native high punch 46d290");
    goro_scene(slot, 3 * Q16_ONE, &owner, &victim);
    owner->held = BUTTON_BIT(slot ? BUTTON_RIGHT : BUTTON_LEFT);
    fighter_press(owner, BUTTON_HIGH_KICK);
    advance_ticks(1, NULL);
    check(fighter_move(owner)->clip == 313, "Goro back+HK must enter the shared roundhouse 46f1f0");
  }
}

/* Reptile fighter block: acid spit 4988d0/4ed1e8 and its reaction 41. */
static void check_reptile_acid_spit(void) {
  for (int slot = 0; slot < 2; ++slot) {
    for (int scenario = 0; scenario < 3; ++scenario) {
      battle_init(slot ? 4 : 10, slot ? 10 : 4);
      g_battle.phase = PHASE_FIGHT;
      Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
      owner->body.pos[AXIS_X] = slot ? Q16_ONE : -Q16_ONE;
      victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
      Input input[2] = {0};
      if (scenario == 1) {
        input[1 - slot].held = BUTTON_BIT(BUTTON_BLOCK);
      }
      battle_step(input);
      fighter_press(owner, BUTTON_DOWN);
      fighter_press(owner, slot ? BUTTON_LEFT : BUTTON_RIGHT);
      fighter_press(owner, BUTTON_HIGH_PUNCH);
      battle_step(input);
      check(fighter_move(owner)->clip == 25 && owner->action == 0x30b, "Down, forward+HP must enter Reptile's acid spit");
      const Controls *controls = roster_def(10)->controls;
      CommandPredicate gate = NULL;
      for (int i = 0; i < controls->command_count; ++i) {
        if (controls->commands[i].button == BUTTON_HIGH_PUNCH && controls->commands[i].count == 2) {
          gate = controls->commands[i].predicate;
        }
      }
      check(gate != NULL && !gate(owner), "Acid spit must stamp its family-7 delay");
      bool opened = false, victim_clip = false;
      for (int tick = 0; tick < 150; ++tick) {
        battle_step(input);
        if (scenario == 2 && tick == 4) {
          fighter_enter_state(owner, S_HIT);
        }
        opened |= owner->mesh_swap_joint == 2;
        victim_clip |= fighter_move(victim)->clip == 182;
      }
      q16 damage = scenario == 0 ? 0x1999 : scenario == 1 ? q16_mul(0x1999, 0x3333) : 0;
      if (victim->health != Q16_ONE - damage || owner->state != S_IDLE) {
        fprintf(stderr, "Reptile acid slot%d scenario%d health%d expected%d owner%d victim%d\n", slot, scenario, victim->health, Q16_ONE - damage, owner->state,
            victim->state);
      }
      check(victim->health == Q16_ONE - damage, "Acid must hit, chip or be cancelled before its carrier spawns");
      check(victim_clip == (scenario == 0), "Only an unblocked acid hit enters reaction 41's clip 182");
      check(opened == (scenario != 2) && owner->mesh_swap_joint == 0, "Acid spit must open and then restore Reptile's head mesh");
      check(owner->state == S_IDLE && victim->state == (scenario == 1 ? S_BLOCK : S_IDLE), "Acid spit owner and victim must recover");
      check(gate(owner), "Acid spit delay must expire after 0x50 ticks");
    }
  }
}

/* Reptile fighter block: 4698a0 BLOCK+HK chord visibility toggle. */
static void check_reptile_invisibility(void) {
  for (int slot = 0; slot < 2; ++slot) {
    battle_init(slot ? 4 : 10, slot ? 10 : 4);
    g_battle.phase = PHASE_FIGHT;
    Fighter *owner = &g_battle.fighters[slot];
    for (int use = 0; use < 3; ++use) {
      Input input[2] = {0};
      input[slot].held = BUTTON_BIT(BUTTON_BLOCK) | (use == 2 ? BUTTON_BIT(BUTTON_LOW_PUNCH) : 0);
      battle_step(input);
      battle_step(input);
      input[slot].held |= BUTTON_BIT(BUTTON_HIGH_KICK);
      battle_step(input);
      battle_step(input);
      check((fighter_move(owner)->clip == 12) == (use != 2), "BLOCK+HK chord must enter clip 12, and a held LP must reject it");
      input[slot].held = 0;
      bool toggled_late = true;
      for (int tick = 0; tick < 70; ++tick) {
        battle_step(input);
        if (tick == 10) {
          toggled_late = owner->invisible == (use == 1);
        }
      }
      check(toggled_late && owner->invisible == (use == 0), "Invisibility must toggle once at its clip midpoint and toggle back on reuse");
      check(owner->state == S_IDLE, "Invisibility must return controls");
    }
    battle_next_round();
    check(!owner->invisible, "A new round must restore Reptile's visibility");
  }
}

/* Reptile fighter block: 467cd0 -> 43bf40 acid fatality and reaction 180. */
static void check_reptile_second_fatality(void) {
  for (int slot = 0; slot < 2; ++slot) {
    for (int target = 2; target <= 4; target += 2) {
      for (int reset_at = 0; reset_at < 3; ++reset_at) {
        battle_init(slot ? target : 10, slot ? 10 : target);
        g_battle.phase = PHASE_FIGHT;
        Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
        owner->wins = 1;
        battle_damage(victim, Q16_ONE);
        advance_ticks(1, NULL);
        for (int tick = 0; tick < 180 && g_battle.finish_waiting; ++tick) {
          advance_ticks(1, NULL);
        }
        check(battle_can_finish(owner), "Reptile acid fatality needs the deciding-KO command window");
        owner->body.pos[AXIS_X] = slot ? 0x18200 : -0x18200;
        victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
        check(!retail_can_finish_range(owner), "45e530 rejects 3.0+ unit separations");
        owner->body.pos[AXIS_X] = slot ? Q16_ONE : -Q16_ONE;
        victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
        check(retail_can_finish_range(owner), "45e530 accepts a two-unit separation");
        static const Button sequence[] = {BUTTON_UP, BUTTON_DOWN, BUTTON_DOWN, BUTTON_DOWN};
        for (int i = 0; i < 4; ++i) {
          Input input[2] = {0};
          /* Holding BLOCK suppresses the Up jump; it is released for the final press. */
          input[slot].held = BUTTON_BIT(sequence[i]) | (i == 3 ? BUTTON_BIT(BUTTON_HIGH_PUNCH) : BUTTON_BIT(BUTTON_BLOCK));
          battle_step(input);
          input[slot].held &= BUTTON_BIT(BUTTON_BLOCK);
          advance_ticks(2, input);
        }
        check(g_battle.phase == PHASE_FATALITY && fighter_move(owner)->clip == 493, "Up,Down,Down,Down+HP must start Reptile's acid scene");
        bool hovered = false, vomit = false, melt = false, flayed = false, fall = false;
        int limit = reset_at == 1 ? 120 : reset_at == 2 ? 260 : 600;
        for (int tick = 0; tick < limit && g_battle.phase == PHASE_FATALITY; ++tick) {
          advance_ticks(1, NULL);
          hovered |= owner->body.pos[AXIS_Y] < owner->body.ground - Q16_ONE;
          vomit |= fighter_move(owner)->clip == 544 || fighter_move(owner)->clip == 545;
          melt |= fighter_move(victim)->clip == 546;
          flayed |= victim->native_variant == 14;
          fall |= fighter_move(victim)->clip == 550;
        }
        if (reset_at == 0) {
          check(hovered && vomit && melt && flayed && fall && g_battle.phase == PHASE_MATCH_OVER,
              "Reptile acid scene must hover, vomit, tear all joints, fall in flesh and finish");
        }
        battle_reset_match();
        advance_ticks(180, NULL);
        check(owner->scene_model == NULL && victim->scene_model == NULL && owner->health == Q16_ONE && victim->health == Q16_ONE && !fighter_airborne(owner),
            "Reset must clear the acid scene, its hover and replacement models");
      }
    }
  }
}

/* Reptile fighter block: 467ce0 -> 4539b0 head-bite fatality. */
static void check_reptile_first_fatality(void) {
  for (int slot = 0; slot < 2; ++slot) {
    for (int target = 2; target <= 4; target += 2) {
      for (int reset_at = 0; reset_at < 3; ++reset_at) {
        battle_init(slot ? target : 10, slot ? 10 : target);
        g_battle.phase = PHASE_FIGHT;
        Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
        owner->wins = 1;
        battle_damage(victim, Q16_ONE);
        advance_ticks(1, NULL);
        for (int tick = 0; tick < 180 && g_battle.finish_waiting; ++tick) {
          advance_ticks(1, NULL);
        }
        owner->body.pos[AXIS_X] = slot ? Q16_ONE / 2 : -Q16_ONE / 2;
        victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
        unsigned chord = BUTTON_BIT(BUTTON_HIGH_PUNCH) | BUTTON_BIT(BUTTON_HIGH_KICK) | BUTTON_BIT(BUTTON_LOW_PUNCH) | BUTTON_BIT(BUTTON_LOW_KICK);
        owner->held = chord | BUTTON_BIT(BUTTON_BLOCK);
        check(!reptile_can_first_fatality(owner), "45e250 rejects a held BLOCK");
        owner->held = chord;
        check(reptile_can_first_fatality(owner), "45e250 accepts HP+HK+LP+LK within one unit");
        fighter_press(owner, BUTTON_UP);
        advance_ticks(1, NULL);
        check(g_battle.phase == PHASE_FATALITY, "UP with the four-attack chord must start Reptile's head-bite scene");
        bool owner_clip = false, victim_clip = false, headless = false, looped = false;
        int limit = reset_at == 1 ? 60 : reset_at == 2 ? 300 : 900;
        for (int tick = 0; tick < limit && g_battle.phase == PHASE_FATALITY; ++tick) {
          advance_ticks(1, NULL);
          owner_clip |= fighter_move(owner)->clip == 432;
          victim_clip |= fighter_move(victim)->clip == 431;
          headless |= (victim->hidden_joints[0] & (UINT64_C(1) << 2)) != 0;
          looped |= fighter_move(owner)->clip == 432 && fighter_sample(owner) >= 294;
        }
        if (reset_at == 0) {
          check(owner_clip && victim_clip && headless && looped && g_battle.phase == PHASE_MATCH_OVER,
              "Reptile head-bite scene must pair clips 432/431, remove the head, loop the owner and finish");
        }
        battle_reset_match();
        advance_ticks(180, NULL);
        check(victim->hidden_joints[0] == 0 && owner->health == Q16_ONE && victim->health == Q16_ONE, "Reset must clear the head-bite scene");
      }
    }
  }
}

/* Reptile fighter block: type-67 weapon draw, strikes and victims. */
static void check_reptile_weapon_commands(void) {
  for (int slot = 0; slot < 2; ++slot) {
    for (int strike = 0; strike < 4; ++strike) {
      for (int scenario = 0; scenario < 3; ++scenario) {
        battle_init(slot ? 4 : 10, slot ? 10 : 4);
        g_battle.phase = PHASE_FIGHT;
        Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
        g_battle.fighters[0].body.pos[AXIS_X] = -4 * Q16_ONE;
        g_battle.fighters[1].body.pos[AXIS_X] = 4 * Q16_ONE;
        Button back = slot ? BUTTON_RIGHT : BUTTON_LEFT;
        command(slot, back, back, BUTTON_LOW_KICK);
        check(fighter_move(owner)->clip == 244 && !owner->armed, "Reptile's back,back+LK must play the weapon draw");
        advance_ticks(65, NULL);
        check(owner->armed && owner->weapon->type == 0x67 && owner->weapon->model == SCENE_MODEL_REPTILE_WEAPON && owner->state == S_IDLE,
            "Reptile must equip his own type-67 weapon rig and armed controls");
        q16 half_distance = scenario == 2 ? 4 * Q16_ONE : 0xa666;
        owner->body.pos[AXIS_X] = slot ? half_distance : -half_distance;
        victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
        Input input[2] = {0};
        input[slot].held = BUTTON_BIT(strike == 1 || strike == 3 ? BUTTON_LOW_PUNCH : BUTTON_HIGH_PUNCH);
        if (strike >= 2) {
          input[slot].held |= BUTTON_BIT(back);
        }
        if (scenario == 1) {
          input[1 - slot].held = BUTTON_BIT(BUTTON_BLOCK);
        }
        advance_ticks(2, input);
        const int clips[] = {230, 240, 248, 233};
        check(fighter_move(owner)->clip == clips[strike], "Reptile armed punches must select their own native strike");
        input[slot].held = 0;
        bool reacted = false;
        for (int tick = 0; tick < 200; ++tick) {
          battle_step(input);
          int clip = fighter_move(victim)->clip;
          reacted |= strike == 0 ? clip != 322 && victim->reaction == 63 : strike == 1 ? clip == 189 : strike == 2 ? clip == 311 : clip == 182;
        }
        /* WEAPON[18], [21], [20] and [19]; the flurry may land once per cycle. */
        const q16 damages[] = {0x2666, 0x1eb8, 0xccc, 0x2147};
        q16 lost = Q16_ONE - victim->health;
        bool damaged = scenario == 2   ? lost == 0
                       : scenario == 1 ? lost > 0 && lost % q16_mul(damages[strike], 0x3333) == 0
                                       : lost > 0 && lost % damages[strike] == 0 && (strike == 2 || lost == damages[strike]);
        if (!damaged || (scenario == 0 && !reacted)) {
          fprintf(stderr, "Reptile weapon slot%d strike%d scenario%d lost%d reacted%d\n", slot, strike, scenario, lost, reacted);
        }
        check(damaged, "Reptile weapon records must keep their native damage, chip and miss");
        check(scenario != 0 || reacted, "Unblocked Reptile weapon hits must enter their own victim programs");
        check(owner->state == S_IDLE && owner->armed && owner->body.physics == NULL, "Reptile weapon recovery must restore armed controls");
        check(victim->health > 0 && (victim->state == S_IDLE || victim->state == S_BLOCK || victim->state == S_CROUCH), "Weapon victims must recover");
      }
    }
  }
}

static void check_goro_reconnected_counters(void) {
  for (int slot = 0; slot < 2; ++slot) {
    for (int kick = 0; kick < 2; ++kick) {
      for (int scenario = 0; scenario < 4; ++scenario) {
        battle_init(slot ? 4 : 15, slot ? 15 : 4);
        g_battle.phase = PHASE_FIGHT;
        Fighter *owner = &g_battle.fighters[slot];
        Fighter *victim = &g_battle.fighters[1 - slot];
        q16 distance = scenario == 1 ? 8 * Q16_ONE : Q16_ONE;
        owner->body.pos[AXIS_X] = slot ? distance / 2 : -distance / 2;
        victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
        Button direction = kick ? (slot ? BUTTON_RIGHT : BUTTON_LEFT) : (slot ? BUTTON_LEFT : BUTTON_RIGHT);
        fighter_press(owner, direction);
        fighter_press(owner, direction);
        fighter_press(owner, kick ? BUTTON_HIGH_KICK : BUTTON_HIGH_PUNCH);
        advance_ticks(1, NULL);
        check(fighter_move(owner)->clip == (kick ? 598 : 599), "Goro counter command must select its owned rig clip");
        if (scenario == 3) {
          fighter_enter_state(owner, S_HIT);
        }
        Input held[2] = {0};
        if (scenario == 2) {
          held[1 - slot].held = BUTTON_BIT(BUTTON_BLOCK);
        }
        advance_ticks(140, held);
        q16 expected = scenario == 0 ? Q16_ONE - 0x2666 : scenario == 2 ? Q16_ONE - q16_mul(0x2666, 0x3333) : Q16_ONE;
        if (!(victim->health == expected && owner->state == S_IDLE)) {
          fprintf(stderr, "Goro counter slot %d kick %d scenario %d health %d state %d\n", slot, kick, scenario, victim->health, owner->state);
        }
        check(victim->health == expected, "Goro counter must resolve exactly one retail contact, with miss, block and cancellation gates");
        check(owner->state == S_IDLE, "Goro counter must finish its remaining clip and recover");
      }
    }
  }
}

static Projectile *jax_missile_object(void) {
  for (int i = 0; i < MAX_PROJECTILES; ++i) {
    Projectile *projectile = &g_battle.projectiles[i];
    if (projectile->kind == PROJECTILE_FIREBALL && projectile->mesh_animation != NULL && projectile->mesh_animation->first == 43) {
      return projectile;
    }
  }
  return NULL;
}

/* Jax 496f80/4ed090: D,F,LP and the CPU projectile row cast clip 29 and an
 * independent C[4] missile that hits, chips on block, survives owner
 * interruption and retires at the arena bounds. */
static void check_jax_missile(void) {
  for (int slot = 0; slot < 2; ++slot) {
    for (int scenario = 0; scenario < 4; ++scenario) {
      battle_init(slot ? 4 : 13, slot ? 13 : 4);
      advance_ticks(90, NULL);
      check(g_battle.phase == PHASE_FIGHT, "Jax missile scenarios start after the countdown");
      Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
      owner->body.pos[AXIS_X] = slot ? 3 * Q16_ONE : -3 * Q16_ONE;
      victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
      command(slot, BUTTON_DOWN, slot ? BUTTON_LEFT : BUTTON_RIGHT, BUTTON_LOW_PUNCH);
      check(fighter_move(owner)->clip == 29 && owner->action == 0x314, "Jax D,F,LP must select the native missile cast");
      if (scenario == 2) {
        /* The flight keeps the cast-time heading snapshot. */
        victim->body.pos[AXIS_Z] = 2 * Q16_ONE;
      }
      Input input[2] = {0};
      if (scenario == 1) {
        input[1 - slot].held = BUTTON_BIT(BUTTON_BLOCK);
      }
      bool launched = false, retired = false;
      for (int tick = 0; tick < 240; ++tick) {
        battle_step(input);
        Projectile *projectile = jax_missile_object();
        if (!launched && projectile != NULL) {
          launched = true;
          check(projectile->initial_mesh == 42 && projectile->vel[AXIS_Y] == 0, "Jax missile must spawn from its native rig and fly level");
          if (scenario == 3) {
            battle_react(owner, 37);
          }
        }
        retired |= launched && projectile == NULL;
      }
      q16 damage = scenario == 0 || scenario == 3 ? 0x1eb8 : scenario == 1 ? q16_mul(0x1eb8, 0x3333) : 0;
      if (victim->health != Q16_ONE - damage) {
        fprintf(stderr, "Jax missile slot%d scenario%d health%d expected%d launched%d\n", slot, scenario, victim->health, Q16_ONE - damage, launched);
      }
      check(launched && retired && victim->health == Q16_ONE - damage, "Jax missile must hit once, chip on block, miss and retire, and outlive interruption");
      check(owner->state == S_IDLE, "Jax missile caster must recover");
    }
    battle_init(slot ? 4 : 13, slot ? 13 : 4);
    g_battle.phase = PHASE_FIGHT;
    check(battle_cpu_action(&g_battle.fighters[slot], CPU_PROJECTILE), "Jax CPU projectile row must be registered");
    advance_ticks(1, NULL);
    check(fighter_move(&g_battle.fighters[slot])->clip == 29 && g_battle.fighters[slot].action == 0x314, "Jax CPU projectile row must cast the missile");
  }
}

/* Jax 47ab10: D,B,LP dashes into 15 D[22] visits. A grounded hit takes
 * two more 157 reactions and the final 168 slide; a block leaves Jax
 * without guard for 40 ticks; a miss rewinds clip 299 before returning. */
static void check_jax_dash_punch(void) {
  for (int slot = 0; slot < 2; ++slot) {
    for (int scenario = 0; scenario < 3; ++scenario) {
      battle_init(slot ? 4 : 13, slot ? 13 : 4);
      advance_ticks(90, NULL);
      Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
      q16 half = scenario == 2 ? 4 * Q16_ONE : Q16_ONE;
      owner->body.pos[AXIS_X] = slot ? half : -half;
      victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
      command(slot, BUTTON_DOWN, slot ? BUTTON_RIGHT : BUTTON_LEFT, BUTTON_LOW_PUNCH);
      check(fighter_move(owner)->clip == 299 && owner->action == 0x215, "Jax D,B,LP must start the native dash punch");
      Input input[2] = {0};
      if (scenario == 1) {
        input[1 - slot].held = BUTTON_BIT(BUTTON_BLOCK);
      }
      bool final = false, unguarded = false, stagger = false;
      for (int tick = 0; tick < 150; ++tick) {
        battle_step(input);
        stagger |= victim->reaction == 157 && fighter_move(victim)->clip == 311;
        final |= victim->reaction == 168 && fighter_move(victim)->clip == 130;
        unguarded |= (owner->flags & FF_NO_GUARD) != 0;
      }
      q16 expected = scenario == 0 ? Q16_ONE - 0x28f5 : scenario == 1 ? Q16_ONE - q16_mul(0x28f5, 0x3333) : Q16_ONE;
      if (victim->health != expected) {
        fprintf(stderr, "Jax dash slot%d scenario%d health%d expected%d\n", slot, scenario, victim->health, expected);
      }
      check(victim->health == expected, "Jax dash punch must apply D[22] once, chip on block and miss at range");
      check((scenario == 0) == final && (scenario == 0) == stagger, "Only a grounded hit must reach the 157 pummel and the 168 slide");
      check(unguarded == (scenario != 0), "Blocked and missed dashes must leave Jax without guard");
      check(owner->state == S_IDLE && !(owner->flags & FF_NO_GUARD) && owner->body.physics == NULL, "Jax dash punch must return and clear its callback");
      check(scenario != 0 || victim->state == S_IDLE, "The dash-punch victim must recover from the slide");
    }
    battle_init(slot ? 4 : 13, slot ? 13 : 4);
    g_battle.phase = PHASE_FIGHT;
    check(battle_cpu_action(&g_battle.fighters[slot], CPU_SPECIAL_NEAR), "Jax near CPU row must be registered");
    advance_ticks(1, NULL);
    check(fighter_move(&g_battle.fighters[slot])->clip == 299, "Jax near CPU row must select the dash punch");
  }
}

/* Jax 468f00: F,F,D,LK pounds the floor at pose 35 of clip 108. Only a
 * grounded opponent takes 0x23d7 and reaction 136, regardless of range. */
static void check_jax_ground_pound(void) {
  for (int slot = 0; slot < 2; ++slot) {
    for (int airborne = 0; airborne < 2; ++airborne) {
      battle_init(slot ? 4 : 13, slot ? 13 : 4);
      advance_ticks(90, NULL);
      Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
      owner->body.pos[AXIS_X] = slot ? 3 * Q16_ONE : -3 * Q16_ONE;
      victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
      Button forward = slot ? BUTTON_LEFT : BUTTON_RIGHT;
      Input input[2] = {0};
      input[slot].held = BUTTON_BIT(forward);
      battle_step(input);
      advance_ticks(2, NULL);
      battle_step(input);
      advance_ticks(2, NULL);
      command(slot, forward, BUTTON_DOWN, BUTTON_LOW_KICK);
      check(fighter_move(owner)->clip == 108 && owner->action == 0x101, "Jax F,F,D,LK must start the native ground pound");
      bool reacted = false;
      for (int tick = 0; tick < 120; ++tick) {
        if (airborne) {
          victim->body.pos[AXIS_Y] = victim->body.ground - Q16_ONE;
          victim->body.gravity = 0;
        }
        battle_step((Input[2]){0});
        reacted |= victim->reaction == 136 && fighter_move(victim)->clip == 182;
      }
      q16 expected = airborne ? Q16_ONE : Q16_ONE - 0x23d7;
      check(victim->health == expected && reacted == !airborne, "Jax ground pound must hit only a grounded opponent, once and at range");
      check(owner->state == S_IDLE, "Jax ground pound must return to the controller");
    }
    battle_init(slot ? 4 : 13, slot ? 13 : 4);
    g_battle.phase = PHASE_FIGHT;
    check(battle_cpu_action(&g_battle.fighters[slot], CPU_SPECIAL_1), "Jax medium CPU row must be registered");
    advance_ticks(1, NULL);
    check(fighter_move(&g_battle.fighters[slot])->clip == 108, "Jax medium CPU row must select the ground pound");
  }
}

/* Jax LP throw 4de4f0: each held 45f2b0 chord at a decision adds a slam
 * and 5%; the release costs a last 5% through reaction 100 or 125. */
static void check_jax_multi_slam(void) {
  const unsigned chords[4] = {
      BUTTON_BIT(BUTTON_BLOCK) | BUTTON_BIT(BUTTON_HIGH_KICK) | BUTTON_BIT(BUTTON_RUN),
      BUTTON_BIT(BUTTON_HIGH_PUNCH) | BUTTON_BIT(BUTTON_LOW_PUNCH) | BUTTON_BIT(BUTTON_LOW_KICK),
      BUTTON_BIT(BUTTON_HIGH_PUNCH) | BUTTON_BIT(BUTTON_BLOCK) | BUTTON_BIT(BUTTON_LOW_KICK),
      BUTTON_BIT(BUTTON_HIGH_PUNCH) | BUTTON_BIT(BUTTON_HIGH_KICK) | BUTTON_BIT(BUTTON_LOW_PUNCH) | BUTTON_BIT(BUTTON_LOW_KICK),
  };
  for (int slot = 0; slot < 2; ++slot) {
    for (int slams = 0; slams < 3; ++slams) {
      battle_init(slot ? 4 : 13, slot ? 13 : 4);
      g_battle.phase = PHASE_FIGHT;
      Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
      owner->body.pos[AXIS_X] = slot ? 0x4000 : -0x4000;
      victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
      fighter_press(owner, BUTTON_LOW_PUNCH);
      advance_ticks(1, NULL);
      check(fighter_move(owner)->clip == 83 && fighter_move(victim)->clip == 145, "Jax LP throw must open its 83/145 slam pair");
      int allowed = slams == 0 ? 0 : slams == 1 ? 1 : 4;
      bool released = false;
      for (int tick = 0; tick < 400; ++tick) {
        Input input[2] = {0};
        if (owner->native_variant < allowed) {
          input[slot].held = chords[owner->native_variant];
        }
        battle_step(input);
        released |= victim->reaction == (slams == 1 ? 125 : 100);
      }
      q16 expected = Q16_ONE - (allowed + 1) * 0xccc;
      if (victim->health != expected) {
        fprintf(stderr, "Jax slam slot%d slams%d health%d expected%d count%d\n", slot, slams, victim->health, expected, owner->native_variant);
      }
      check(victim->health == expected && released, "Jax multi-slam must add 5% per held chord and release with its retail reaction");
      check(owner->state == S_IDLE && victim->state == S_IDLE && !(owner->flags & FF_PAIRED) && !(victim->flags & FF_PAIRED),
          "Jax multi-slam must release and recover both fighters");
    }
  }
}

static Projectile *cage_orb(void) {
  for (int i = 0; i < MAX_PROJECTILES; ++i) {
    Projectile *projectile = &g_battle.projectiles[i];
    if (projectile->kind == PROJECTILE_FIREBALL && projectile->mesh_animation != NULL && projectile->mesh_animation->first == 271) {
      return projectile;
    }
  }
  return NULL;
}

static void check_cage_orb_commands(void) {
  for (int slot = 0; slot < 2; ++slot) {
    for (int high = 0; high < 2; ++high) {
      for (int scenario = 0; scenario < 7; ++scenario) {
        battle_init(slot ? 4 : 14, slot ? 14 : 4);
        g_battle.phase = PHASE_FIGHT;
        Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
        q16 half_distance = scenario == 3 ? 6 * Q16_ONE : high ? 4 * Q16_ONE : 3 * Q16_ONE;
        owner->body.pos[AXIS_X] = slot ? half_distance : -half_distance;
        victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
        command(
            slot, BUTTON_DOWN, high ? (slot ? BUTTON_LEFT : BUTTON_RIGHT) : (slot ? BUTTON_RIGHT : BUTTON_LEFT), high ? BUTTON_HIGH_PUNCH : BUTTON_LOW_PUNCH);
        check(fighter_move(owner)->clip == 28 && owner->action == (high ? 0x312 : 0x311),
            "Cage orb commands must select the shared cast with distinct native actions");
        Input input[2] = {0};
        if (scenario == 1) {
          input[1 - slot].held = BUTTON_BIT(BUTTON_BLOCK);
        } else if (scenario == 2) {
          input[1 - slot].held = BUTTON_BIT(BUTTON_DOWN);
        } else if (scenario == 6) {
          victim->body.pos[AXIS_Z] = 2 * Q16_ONE;
        }
        if (scenario == 4) {
          battle_react(owner, 37);
        }
        bool launched = false;
        for (int tick = 0; tick < 180; ++tick) {
          battle_step(input);
          Projectile *projectile = cage_orb();
          if (!launched && projectile != NULL) {
            launched = true;
            check(projectile->gravity == (high ? 0x28f : 0x189), "Each Cage orb must retain its native arc");
            check(projectile->mesh_animation->source == MESH_SHARED2 && projectile->heading[1] == 0,
                "Orb geometry and heading must come from the native bank and cast-time snapshot");
            if (scenario == 5) {
              battle_react(owner, 37);
            }
          }
        }
        q16 damage = scenario == 0 || scenario == 5 ? 0x1999 : scenario == 1 ? q16_mul(0x1999, 0x3333) : 0;
        if (victim->health != Q16_ONE - damage) {
          fprintf(
              stderr, "Cage orb slot%d high%d scenario%d health%d expected%d launched%d\n", slot, high, scenario, victim->health, Q16_ONE - damage, launched);
        }
        check(launched == (scenario != 4) && victim->health == Q16_ONE - damage,
            "Orb must preserve native hit/chip/crouch/miss behavior and independent lifetime across owner interruption");
        check(owner->state == S_IDLE && cage_orb() == NULL, "Cage caster and independent orb must recover and retire");
      }
    }
    battle_init(slot ? 4 : 14, slot ? 14 : 4);
    g_battle.phase = PHASE_FIGHT;
    check(battle_cpu_action(&g_battle.fighters[slot], CPU_PROJECTILE), "Cage CPU projectile row must be registered");
    advance_ticks(1, NULL);
    check(
        fighter_move(&g_battle.fighters[slot])->clip == 28 && g_battle.fighters[slot].action == 0x311, "Cage CPU row must use the low orb rather than a sweep");
  }
}

static void check_cage_special_commands(void) {
  for (int slot = 0; slot < 2; ++slot) {
    for (int kind = 0; kind < 2; ++kind) {
      for (int miss = 0; miss < 2; ++miss) {
        battle_init(slot ? 4 : 14, slot ? 14 : 4);
        g_battle.phase = PHASE_FIGHT;
        Fighter *owner = &g_battle.fighters[slot];
        Fighter *victim = &g_battle.fighters[1 - slot];
        q16 distance = miss ? 12 * Q16_ONE : Q16_ONE;
        owner->body.pos[AXIS_X] = slot ? distance / 2 : -distance / 2;
        victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
        Button back = slot ? BUTTON_RIGHT : BUTTON_LEFT;
        Button forward = slot ? BUTTON_LEFT : BUTTON_RIGHT;
        fighter_press(owner, back);
        fighter_press(owner, kind ? BUTTON_DOWN : forward);
        if (kind) {
          fighter_press(owner, back);
        }
        fighter_press(owner, kind ? BUTTON_HIGH_PUNCH : BUTTON_LOW_KICK);
        advance_ticks(1, NULL);
        check(fighter_move(owner)->clip == (kind ? 85 : 308), "Cage motion command must start its owned special stream");
        advance_ticks(180, NULL);
        if (!(owner->state == S_IDLE && (miss ? victim->health == Q16_ONE : victim->health < Q16_ONE))) {
          fprintf(stderr, "Cage special slot %d kind %d miss %d state %d health %d clip %d\n", slot, kind, miss, owner->state, victim->health,
              fighter_move(owner)->clip);
        }
        check(owner->state == S_IDLE, "Cage special must finish recovery");
        check(miss ? victim->health == Q16_ONE : victim->health < Q16_ONE, "Cage special must use its contact branch rather than inflict damage on a miss");
      }
    }
    for (int kind = 0; kind < 2; ++kind) {
      battle_init(slot ? 4 : 14, slot ? 14 : 4);
      g_battle.phase = PHASE_FIGHT;
      Fighter *owner = &g_battle.fighters[slot];
      Fighter *victim = &g_battle.fighters[1 - slot];
      owner->body.pos[AXIS_X] = slot ? Q16_ONE / 2 : -Q16_ONE / 2;
      victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
      const CommandDef *command = NULL;
      const Controls *controls = roster_def(14)->controls;
      for (int i = 0; i < controls->command_count; ++i) {
        if (controls->commands[i].button == (kind ? BUTTON_HIGH_PUNCH : BUTTON_LOW_KICK) && controls->commands[i].predicate == NULL) {
          command = &controls->commands[i];
          break;
        }
      }
      check(command != NULL, "Cage special command must remain registered");
      fighter_interrupt(owner, command->enter);
      advance_ticks(1, NULL);
      fighter_enter_state(owner, S_HIT);
      advance_ticks(180, NULL);
      check(victim->health == Q16_ONE, "Interrupted Cage special must not leave a delayed contact behind");
    }
    battle_init(slot ? 4 : 14, slot ? 14 : 4);
    g_battle.phase = PHASE_FIGHT;
    check(battle_cpu_action(&g_battle.fighters[slot], CPU_SPECIAL_NEAR), "Cage CPU special row must be registered");
    advance_ticks(1, NULL);
    check(fighter_move(&g_battle.fighters[slot])->clip == 308, "Cage CPU special must select the same owned kick as the retail row");
    advance_ticks(180, NULL);
  }
}

static void check_cage_powered_punch_branches(void) {
  for (int slot = 0; slot < 2; ++slot) {
    for (int branch = 0; branch < 4; ++branch) {
      battle_init(slot ? 4 : 14, slot ? 14 : 4);
      g_battle.phase = PHASE_FIGHT;
      Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
      q16 distance = branch == 0 ? 12 * Q16_ONE : Q16_ONE;
      owner->body.pos[AXIS_X] = slot ? distance / 2 : -distance / 2;
      victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
      Input input[2] = {0};
      if (branch == 2) {
        input[1 - slot].held = BUTTON_BIT(BUTTON_BLOCK);
      }
      fighter_press(owner, slot ? BUTTON_RIGHT : BUTTON_LEFT);
      fighter_press(owner, BUTTON_DOWN);
      fighter_press(owner, slot ? BUTTON_RIGHT : BUTTON_LEFT);
      fighter_press(owner, BUTTON_HIGH_PUNCH);
      unsigned seen = 0;
      bool active = false, interrupted = false;
      for (int tick = 0; tick < 180; ++tick) {
        advance_ticks(1, input);
        if (owner->state == S_IDLE && active) {
          break;
        }
        active = true;
        check(fighter_move(owner)->clip != 105, "Powered punch must never execute the adjacent flip-kick script from a callback descriptor");
        if (victim->reaction == 163) {
          seen |= 1;
        } else if (victim->reaction == 94) {
          seen |= 2;
        } else if (victim->reaction == 95) {
          seen |= 4;
        }
        if (branch == 3 && !interrupted && (owner->flags & FF_PAIRED)) {
          fighter_enter_state(owner, S_HIT);
          interrupted = true;
        }
      }
      if (!(active && (owner->state == S_IDLE || (branch == 3 && owner->state == S_HIT)))) {
        fprintf(stderr, "Power slot%d branch%d state%d clip%d sample%d y%d vy%d gravity%d seen%u\n", slot, branch, owner->state, fighter_move(owner)->clip,
            fighter_sample(owner), owner->body.pos[AXIS_Y], owner->body.vel[AXIS_Y], owner->body.gravity, seen);
      }
      check(active && (owner->state == S_IDLE || (branch == 3 && owner->state == S_HIT)), "Powered punch must complete its authored recovery");
      if (branch == 0) {
        check(victim->health == Q16_ONE, "Powered-punch whiff must preserve opponent health");
      } else if (branch == 2) {
        check(victim->health == Q16_ONE - q16_mul(0x1eb8, 0x3333), "Powered-punch block must apply only retail D26 chip");
      } else {
        check(victim->health == Q16_ONE - 0x1eb8, "Powered punch must apply retail twelve-percent damage exactly once across its paired stages");
        check(branch == 3 ? interrupted : seen == 7, "Powered punch must reach163/94/95 or release its captured victim on interruption");
      }
      advance_ticks(180, NULL);
      if (victim->state != S_IDLE) {
        fprintf(stderr, "Power victim slot%d branch%d state%d reaction%d y%d vy%d gravity%d\n", slot, branch, victim->state, victim->reaction,
            victim->body.pos[AXIS_Y], victim->body.vel[AXIS_Y], victim->body.gravity);
      }
      check(!(owner->flags & FF_PAIRED) && !(victim->flags & FF_PAIRED) && victim->state == S_IDLE,
          "Powered-punch victim must land and regain control after completion or interruption");
      battle_reset_match();
      check(owner->body.physics == NULL, "Reset must retire powered-punch native motion");
    }
  }
}

static void check_cage_shadow_kick_branches(void) {
  for (int slot = 0; slot < 2; ++slot) {
    for (int branch = 0; branch < 4; ++branch) {
      battle_init(slot ? 4 : 14, slot ? 14 : 4);
      g_battle.phase = PHASE_FIGHT;
      Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
      bool late = branch == 3;
      q16 distance = branch == 0 || late ? 12 * Q16_ONE : Q16_ONE;
      owner->body.pos[AXIS_X] = slot ? distance / 2 : -distance / 2;
      victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
      Input input[2] = {0};
      if (branch == 2) {
        input[1 - slot].held = BUTTON_BIT(BUTTON_BLOCK);
      }
      fighter_press(owner, slot ? BUTTON_RIGHT : BUTTON_LEFT);
      fighter_press(owner, slot ? BUTTON_LEFT : BUTTON_RIGHT);
      fighter_press(owner, BUTTON_LOW_KICK);
      bool active = false, moved_target = false;
      for (int tick = 0; tick < 100; ++tick) {
        advance_ticks(1, input);
        if (owner->state == S_IDLE && active) {
          break;
        }
        check(fighter_move(owner)->clip == 308, "Shadow kick hit, block, and miss must retain their own clip rather than start powered punch");
        active = true;
        if (late && !moved_target && fighter_sample(owner) == 24) {
          victim->body.pos[AXIS_X] = owner->body.pos[AXIS_X] + (slot ? -0xcccc : 0xcccc);
          victim->body.pos[AXIS_Z] = owner->body.pos[AXIS_Z];
          moved_target = true;
        }
      }
      check(active && owner->state == S_IDLE, "Shadow kick must complete its own contact or miss recovery");
      if (branch == 0) {
        check(victim->health == Q16_ONE, "Whiffed shadow kick must preserve peer health");
      } else if (branch == 2) {
        if (victim->health != Q16_ONE - q16_mul(0x23d7, 0x3333)) {
          fprintf(stderr, "Shadow block slot%d health%d reaction%d state%d held%u\n", slot, victim->health, victim->reaction, victim->state, victim->held);
        }
        check(victim->health == Q16_ONE - q16_mul(0x23d7, 0x3333), "Standing block must use shadow-kick D25 chip");
      } else {
        check(victim->health == Q16_ONE - 0x23d7, "Initial or late shadow-kick contact must apply retail D25 fourteen-percent damage exactly once");
        check(victim->reaction == 162, "Shadow kick must choose its own ballistic reaction162");
      }
      check(!late || moved_target, "Late-hit case must exercise the held-pose24 contact window");
      advance_ticks(160, NULL);
      check(victim->health == 0 || victim->state == S_IDLE || victim->state == S_BLOCK, "Shadow-kick victim must land and regain control");
    }
  }
}

static void check_reconnected_stage_commands(void) {
  const int rosters[] = {5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 17};
  for (size_t row = 0; row < sizeof rosters / sizeof rosters[0]; ++row) {
    for (int stage = 0; stage < 2; ++stage) {
      for (int slot = 0; slot < 2; ++slot) {
        battle_init(slot ? 4 : rosters[row], slot ? rosters[row] : 4);
        g_battle.phase = PHASE_FIGHT;
        g_battle.arena = stage ? 0 : 8;
        g_battle.fighters[slot].wins = 1;
        battle_damage(&g_battle.fighters[1 - slot], Q16_ONE);
        advance_ticks(1, NULL);
        for (int tick = 0; tick < 180 && g_battle.finish_waiting; ++tick) {
          advance_ticks(1, NULL);
        }
        check(battle_can_finish(&g_battle.fighters[slot]), "Stage command needs the completed defeat/finish window");
        g_battle.fighters[0].body.pos[AXIS_X] = -0x6666;
        g_battle.fighters[1].body.pos[AXIS_X] = 0x6666;
        const Controls *controls = roster_def(rosters[row])->controls;
        const CommandDef *command = NULL;
        for (int i = 0; i < controls->command_count; ++i) {
          if (controls->commands[i].predicate == (stage ? mk4_can_spike_finish : mk4_can_fan_finish)) {
            command = &controls->commands[i];
            break;
          }
        }
        check(command != NULL, "Retail-listed stage command must be registered");
        Fighter *fighter = &g_battle.fighters[slot];
        for (int i = 0; i < command->count; ++i) {
          CommandToken token = command->tokens[i];
          Button button = token == TOKEN_UP     ? BUTTON_UP
                          : token == TOKEN_DOWN ? BUTTON_DOWN
                          : token == TOKEN_BACK ? (slot ? BUTTON_RIGHT : BUTTON_LEFT)
                                                : (slot ? BUTTON_LEFT : BUTTON_RIGHT);
          fighter_press(fighter, button);
        }
        fighter_press(fighter, command->button);
        advance_ticks(1, NULL);
        if (g_battle.phase != PHASE_FATALITY || g_battle.finisher != (stage ? FINISH_SPIKE : FINISH_FAN)) {
          fprintf(stderr, "Stage command roster %d slot %d stage %d phase %d finisher %d\n", rosters[row], slot, stage, g_battle.phase, g_battle.finisher);
        }
        check(g_battle.phase == PHASE_FATALITY && g_battle.finisher == (stage ? FINISH_SPIKE : FINISH_FAN),
            "Stage command must acquire the correct scene for either player");
        for (int tick = 0; tick < 650; ++tick) {
          advance_ticks(1, NULL);
        }
        if (g_battle.phase != PHASE_MATCH_OVER) {
          fprintf(stderr, "Stage recovery roster %d slot %d stage %d phase %d\n", rosters[row], slot, stage, g_battle.phase);
        }
        check(g_battle.phase == PHASE_MATCH_OVER, "Reconnected stage scene must complete");
      }
    }
  }
}

static void finish_command(int player, int id) {
  Button back = player ? BUTTON_RIGHT : BUTTON_LEFT;
  Button forward = player ? BUTTON_LEFT : BUTTON_RIGHT;
  Button tokens[4] = {back, forward, forward, back};
  int count = 4;
  Button attack = BUTTON_BLOCK;
  if (id == FINISH_BEAST) {
    tokens[2] = BUTTON_DOWN;
    tokens[3] = BUTTON_UP;
    attack = BUTTON_HIGH_PUNCH;
  } else if (id == FINISH_FAN) {
    tokens[0] = forward;
    tokens[1] = tokens[2] = BUTTON_DOWN;
    count = 3;
    attack = BUTTON_LOW_KICK;
  } else if (id == FINISH_SPIKE) {
    count = 3;
    attack = BUTTON_LOW_KICK;
  }
  for (int i = 0; i < count; ++i) {
    Input in[2] = {0};
    in[player].held = BUTTON_BIT(tokens[i]);
    if (i == count - 1) {
      in[player].held |= BUTTON_BIT(attack);
    }
    battle_step(in);
    advance_ticks(2, NULL);
  }
}

static const mframe_t scene_actor_rows[3] = {{0}};
static const mmove_t scene_actor_phase = {.firstframe = 0, .lastframe = 2, .frame = scene_actor_rows, .endfunc = battle_scene_actor_stop};

static int scene_actor_cleanups;

static void scene_actor_cleanup(Fighter *actor) {
  (void)actor;
  ++scene_actor_cleanups;
}

static void scene_actor_entry(Fighter *actor) {
  fighter_mmove_phase(actor, &scene_actor_phase);
  actor->mmove_cleanup = scene_actor_cleanup;
}

static void check_independent_scene_actor_lifetime(void) {
  for (int slot = 0; slot < 2; ++slot) {
    battle_init(14, 4);
    g_battle.phase = PHASE_FIGHT;
    Fighter *owner = &g_battle.fighters[slot], *source = &g_battle.fighters[1 - slot];
    const MoveDef *source_move = fighter_move(source);
    scene_actor_cleanups = 0;
    static const MoveDef pose = {.clip = 130, .last = 39};
    Fighter *actor = battle_scene_actor_start(owner, source, &pose, scene_actor_entry);
    check(actor != NULL && actor->roster == source->roster && actor->buttons == NULL, "Independent scene body must borrow the selected model without controls");
    check(fighter_move(source) == source_move && source->current_mmove == NULL, "Creating a scene body must preserve the source fighter's playback");
    for (int i = 1; i < MAX_SCENE_ACTORS; ++i) {
      check(battle_scene_actor_start(owner, source, &pose, scene_actor_entry) != NULL, "Scene actor pool must fill every bounded slot");
    }
    check(battle_scene_actor_start(owner, source, &pose, scene_actor_entry) == NULL, "Scene actor pool must remain bounded");
    advance_ticks(4, NULL);
    for (int i = 0; i < MAX_SCENE_ACTORS; ++i) {
      check(!g_battle.scene_actors[i].active, "Scene frame completion must release its own actor slot");
    }
    check(scene_actor_cleanups == MAX_SCENE_ACTORS, "Each completed scene actor must clean up once");
    check(battle_scene_actor_start(owner, source, &pose, scene_actor_entry) != NULL, "A completed scene slot must be reusable");
    battle_reset_match();
    for (int i = 0; i < MAX_SCENE_ACTORS; ++i) {
      check(!g_battle.scene_actors[i].active, "Reset must retire every independent scene body");
    }
    check(scene_actor_cleanups == MAX_SCENE_ACTORS + 1, "Reset must run the remaining scene actor cleanup exactly once");
  }
}

static void drive_owned_command(int slot, FighterFn entry) {
  const Controls *controls = roster_def(g_battle.fighters[slot].roster)->controls;
  const CommandDef *command = NULL;
  for (int i = 0; i < controls->command_count; ++i) {
    if (controls->commands[i].enter == entry) {
      command = &controls->commands[i];
      break;
    }
  }
  check(command != NULL, "Requested move must be connected to a real generated command");
  for (int i = 0; i < command->count; ++i) {
    CommandToken token = command->tokens[i];
    Button direction = token == TOKEN_UP        ? BUTTON_UP
                       : token == TOKEN_DOWN    ? BUTTON_DOWN
                       : token == TOKEN_FORWARD ? (slot ? BUTTON_LEFT : BUTTON_RIGHT)
                                                : (slot ? BUTTON_RIGHT : BUTTON_LEFT);
    Input input[2] = {0};
    input[slot].held = BUTTON_BIT(direction);
    battle_step(input);
    advance_ticks(2, NULL);
  }
  Input input[2] = {0};
  input[slot].held = BUTTON_BIT(command->button);
  battle_step(input);
  advance_ticks(2, NULL);
}

/* Kai D,F HP (47bd10) and D,F LK (478df0): real command records, hit/block/
 * miss branches, one-time native damage and frame-only recovery. */
static void check_kai_df_specials(void) {
  for (int slot = 0; slot < 2; ++slot) {
    for (int move = 0; move < 2; ++move) {
      for (int scenario = 0; scenario < 3; ++scenario) {
        battle_init(slot ? 4 : 11, slot ? 11 : 4);
        g_battle.phase = PHASE_FIGHT;
        Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
        q16 half_distance = scenario == 2 ? 4 * Q16_ONE : move == 0 ? 0x10000 : 0x9999;
        owner->body.pos[AXIS_X] = slot ? half_distance : -half_distance;
        victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
        Input input[2] = {0};
        if (scenario == 1) {
          input[1 - slot].held = BUTTON_BIT(BUTTON_BLOCK);
          advance_ticks(4, input);
        }
        drive_owned_command(slot, move == 0 ? kai_input_47bd10 : kai_input_478df0);
        check(fighter_move(owner)->clip == (move == 0 ? 298 : 551) && owner->action == (move == 0 ? 0x210 : 0x20b),
            "Kai D,F command must enter its own retail clip and action");
        bool stopped = false, fell = false;
        for (int tick = 0; tick < 200 && !(owner->state == S_IDLE && !fighter_airborne(owner)); ++tick) {
          if (move == 0 && scenario != 2 && owner->action == 0x210 && fighter_sample(owner) == 9 && !owner->connected) {
            /* The rising hop passes over a standing target; place the target
             * on the D[16] plane to check the contact branch itself. */
            victim->body.pos[AXIS_X] = owner->body.pos[AXIS_X] + q16_mul(owner->heading[0], 0x9999);
            victim->body.pos[AXIS_Z] = owner->body.pos[AXIS_Z] + q16_mul(owner->heading[1], 0x9999);
            victim->body.pos[AXIS_Y] = scenario == 1 ? victim->body.ground : owner->body.pos[AXIS_Y];
            if (scenario == 1) {
              owner->body.pos[AXIS_Y] = owner->body.ground - 0x6666;
            }
            victim->body.gravity = 0;
            victim->body.on_land = NULL;
          }
          battle_step(input);
          stopped |= move == 0 && owner->action == ACTION_RETAIL_1006 && fighter_airborne(owner) && owner->body.vel[AXIS_X] == 0;
          fell |= move == 0 && fighter_move(owner)->clip == 310;
        }
        check(owner->state == S_IDLE && !fighter_airborne(owner) && owner->body.physics == NULL, "Kai D,F specials must land and return to control");
        q16 damage = scenario == 2 ? 0 : move == 0 ? 0x2147 : 0x23d7;
        if (scenario == 1) {
          damage = q16_mul(damage, 0x3333);
        }
        if (victim->health != Q16_ONE - damage) {
          fprintf(stderr, "Kai D,F slot%d move%d scenario%d health %x expected %x\n", slot, move, scenario, victim->health, Q16_ONE - damage);
        }
        check(victim->health == Q16_ONE - damage, "Kai D,F specials must preserve native hit, chip and miss damage once");
        if (move == 0) {
          check(fell && stopped == (scenario != 2), "Kai aerial punch contact must stop in the air before the shared fall; a miss times out into it");
        }
        input[1 - slot].held = 0;
        advance_ticks(200, input);
        check(victim->state == S_IDLE, "Kai D,F victims must recover through their reactions");
      }
    }
  }
}

/* Kai B,B HP (499410) and F,F LP (498fc0) skulls: real commands in both
 * slots, independent relocated objects, one-time C[8]/C[9] damage, block
 * chip, a peer that leaves the saved target, gates and recovery. */
static void check_kai_skulls(void) {
  for (int slot = 0; slot < 2; ++slot) {
    for (int move = 0; move < 2; ++move) {
      for (int scenario = 0; scenario < 3; ++scenario) {
        battle_init(slot ? 4 : 11, slot ? 11 : 4);
        g_battle.phase = PHASE_FIGHT;
        Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
        owner->body.pos[AXIS_X] = slot ? 0x18000 : -0x18000;
        victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
        Input input[2] = {0};
        if (scenario == 1) {
          input[1 - slot].held = BUTTON_BIT(BUTTON_BLOCK);
          advance_ticks(4, input);
        }
        drive_owned_command(slot, move == 0 ? kai_rising_skull : kai_ground_skull);
        check(fighter_move(owner)->clip == (move == 0 ? 21 : 22) && owner->action == (move == 0 ? 0x307 : 0x308),
            "Kai skull commands must enter their own retail cast");
        bool spawned = false, relocated = false;
        for (int tick = 0; tick < 240; ++tick) {
          const Projectile *object = &g_battle.projectiles[0];
          spawned |= object->kind == PROJECTILE_FIREBALL;
          if (object->kind == PROJECTILE_FIREBALL && object->phase != 0) {
            relocated |= object->pos[AXIS_X] == victim->body.pos[AXIS_X] || scenario == 2;
            if (scenario == 2) {
              /* Step back from the target position saved at spawn. */
              victim->body.pos[AXIS_X] = slot ? -0x28000 : 0x28000;
            }
          }
          battle_step(input);
        }
        check(spawned && relocated && g_battle.projectiles[0].kind == 0, "Kai skulls must relocate over the saved target and retire");
        q16 damage = scenario == 2 ? 0 : scenario == 1 ? q16_mul(0x2147, 0x3333) : 0x2147;
        if (victim->health != Q16_ONE - damage) {
          fprintf(stderr, "Kai skull slot%d move%d scenario%d health %x expected %x\n", slot, move, scenario, victim->health, Q16_ONE - damage);
        }
        check(victim->health == Q16_ONE - damage, "Kai skulls must preserve native hit, chip and miss damage once");
        check(owner->state == S_IDLE && victim->state == (scenario == 1 ? S_BLOCK : S_IDLE) && !fighter_airborne(owner),
            "Kai skull owner and victim must recover");
        if (move == 0) {
          check(owner->skull_ready > 0 && kai_can_rising_skull(owner), "Rising skull must stamp its cooldown when the cast completes");
          owner->skull_ready = g_battle.ticks + 1;
          check(!kai_can_rising_skull(owner), "Rising skull must reject a cast inside its 48-tick slot-3 window");
        } else {
          victim->action = 0x100a;
          check(!kai_can_ground_skull(owner), "Ground skull must reject a peer in action 100a");
          victim->action = 0;
          victim->hits = 4;
          check(!kai_can_ground_skull(owner), "Ground skull must reject a peer with more than three combo hits");
        }
      }
    }
  }
}

/* Kai fatalities: deciding-KO window shared by both finisher checks. */
static void kai_finish_window(int slot, int target, q16 half_distance) {
  battle_init(slot ? target : 11, slot ? 11 : target);
  g_battle.phase = PHASE_FIGHT;
  Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
  owner->wins = 1;
  battle_damage(victim, Q16_ONE);
  advance_ticks(1, NULL);
  for (int tick = 0; tick < 180 && g_battle.finish_waiting; ++tick) {
    advance_ticks(1, NULL);
  }
  check(battle_can_finish(owner), "Kai fatality needs the deciding-KO command window");
  owner->body.pos[AXIS_X] = slot ? half_distance : -half_distance;
  victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
}

/* Type a generated Kai command while holding extra buttons throughout. */
static void kai_enter_command(int slot, FighterFn entry, unsigned held) {
  const Controls *controls = roster_def(11)->controls;
  const CommandDef *command = NULL;
  for (int i = 0; i < controls->command_count; ++i) {
    if (controls->commands[i].enter == entry) {
      command = &controls->commands[i];
    }
  }
  check(command != NULL, "Kai fatality needs its real generated command");
  Input input[2] = {0};
  input[slot].held = held;
  advance_ticks(2, input);
  for (int i = 0; i < command->count; ++i) {
    CommandToken token = command->tokens[i];
    Button direction = token == TOKEN_UP        ? BUTTON_UP
                       : token == TOKEN_DOWN    ? BUTTON_DOWN
                       : token == TOKEN_FORWARD ? (slot ? BUTTON_LEFT : BUTTON_RIGHT)
                                                : (slot ? BUTTON_RIGHT : BUTTON_LEFT);
    input[slot].held = held | BUTTON_BIT(direction);
    battle_step(input);
    input[slot].held = held;
    advance_ticks(2, input);
  }
  if (held & BUTTON_BIT(command->button)) {
    /* 45c560 needs the switch up for two samples before a new press. */
    input[slot].held = held & ~BUTTON_BIT(command->button);
    advance_ticks(2, input);
  }
  input[slot].held = held | BUTTON_BIT(command->button);
  battle_step(input);
  advance_ticks(2, NULL);
}

static void check_kai_first_fatality(void) {
  for (int slot = 0; slot < 2; ++slot) {
    for (int variant = 0; variant < 3; ++variant) {
      bool female = variant == 1;
      kai_finish_window(slot, female ? 2 : 4, 0x6666);
      Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
      owner->held = BUTTON_BIT(BUTTON_BLOCK) | BUTTON_BIT(BUTTON_HIGH_KICK) | BUTTON_BIT(BUTTON_UP);
      check(kai_can_first_fatality(owner), "Kai first gate must accept BLK+HK with Up outside its mask");
      owner->held |= BUTTON_BIT(BUTTON_LOW_PUNCH);
      check(!kai_can_first_fatality(owner), "Kai first gate must reject another held attack");
      owner->held = BUTTON_BIT(BUTTON_BLOCK) | BUTTON_BIT(BUTTON_HIGH_KICK);
      owner->body.pos[AXIS_X] = slot ? 0xa000 : -0xa000;
      victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
      check(!kai_can_first_fatality(owner), "Kai first gate must reject a pair beyond one unit");
      owner->body.pos[AXIS_X] = slot ? 0x6666 : -0x6666;
      victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
      owner->held = 0;
      kai_enter_command(slot, retail_finish_first, BUTTON_BIT(BUTTON_BLOCK));
      check(g_battle.phase == PHASE_FATALITY, "Kai first command must start its own frame-only scene");
      int timer = g_battle.time_ticks;
      bool torso = false, paired = false;
      int limit = variant == 2 ? 300 : 900;
      for (int tick = 0; tick < limit && g_battle.phase == PHASE_FATALITY; ++tick) {
        advance_ticks(1, NULL);
        paired |= fighter_move(owner)->clip == 440 && fighter_move(victim)->clip == 441 && owner->body.pos[AXIS_X] == victim->body.pos[AXIS_X];
        for (int i = 0; i < MAX_SCENE_ACTORS; ++i) {
          const SceneActor *body = &g_battle.scene_actors[i];
          if (body->active) {
            torso = true;
            check(body->source == 1 - slot && body->owner == slot && fighter_move(&body->actor)->clip == 442 &&
                      body->actor.hidden_joints[0] == ((UINT64_C(1) << 11) | (UINT64_C(1) << 12)),
                "Kai's detached torso must keep its source rig, clip 442 and leg mask");
          }
        }
      }
      check(paired && torso, "Kai first fatality must share the 440/441 origin and detach the torso");
      if (variant != 2) {
        check(g_battle.phase == PHASE_MATCH_OVER && fighter_move(owner)->clip == 443 && fighter_sample(victim) == 401 && owner->native_variant == 7,
            "Kai first fatality must finish victim and torso before the result while the owner loops 443");
        check(victim->hidden_joints[0] == (UINT64_C(1) << 3) && owner->wins == 2 && victim->health == 0 && g_battle.time_ticks == timer,
            "Kai first fatality must preserve the severed waist, decided result and frozen clock");
      }
      battle_reset_match();
      check(victim->hidden_joints[0] == 0, "Kai first fatality reset must restore the victim");
      for (int i = 0; i < MAX_SCENE_ACTORS; ++i) {
        check(!g_battle.scene_actors[i].active, "Kai first fatality reset must retire the torso");
      }
    }
  }
}

static void check_kai_second_fatality(void) {
  for (int slot = 0; slot < 2; ++slot) {
    for (int reset = 0; reset < 2; ++reset) {
      kai_finish_window(slot, 4, 0xc000);
      Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
      check(!retail_can_finish_range(owner), "Kai second gate must reject a pair closer than 1.8 units");
      owner->body.pos[AXIS_X] = slot ? 0x1a000 : -0x1a000;
      victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
      check(!retail_can_finish_range(owner), "Kai second gate must reject a pair beyond three units");
      owner->body.pos[AXIS_X] = slot ? 0x10000 : -0x10000;
      victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
      check(retail_can_finish_range(owner) && !retail_can_finish_range(victim), "Kai second gate must accept 1.8..3 units for the winner");
      kai_enter_command(slot, retail_finish_second, BUTTON_BIT(BUTTON_BLOCK));
      check(g_battle.phase == PHASE_FATALITY && fighter_move(owner)->clip == 493, "Kai second command must start the levitation scene");
      check(owner->body.pos[AXIS_X] == 0 && victim->body.pos[AXIS_X] == (slot ? -5 : 5) * Q16_ONE, "Kai second scene must use its fixed origin layout");
      int timer = g_battle.time_ticks;
      bool hovered = false, skull = false, struck = false;
      for (int tick = 0; tick < (reset ? 220 : 600) && g_battle.phase == PHASE_FATALITY; ++tick) {
        advance_ticks(1, NULL);
        hovered |= owner->body.pos[AXIS_Y] < owner->body.ground - 0x8000;
        for (int i = 0; i < MAX_PROJECTILES; ++i) {
          skull |= g_battle.projectiles[i].kind == PROJECTILE_FIREBALL;
        }
        struck |= victim->reaction == 178 && fighter_move(victim)->clip == 130;
      }
      check(hovered && skull && struck, "Kai second fatality must levitate, throw its skull object and install reaction 178");
      if (!reset) {
        check(g_battle.phase == PHASE_MATCH_OVER && fighter_move(victim)->clip == 206 && fighter_sample(victim) == 99 && fighter_move(owner)->clip == 487,
            "Kai second fatality must drop the victim through clip 206 before the result");
        check(owner->wins == 2 && victim->health == 0 && g_battle.time_ticks == timer, "Kai second fatality must preserve the decided result and frozen clock");
      }
      battle_reset_match();
      for (int i = 0; i < MAX_PROJECTILES; ++i) {
        check(g_battle.projectiles[i].kind == 0, "Kai second fatality reset must retire its hover and skull tasks");
      }
    }
  }
}

static void check_partial_cpu_stage_dispatch(void) {
  const int rosters[] = {15, 17};
  for (size_t row = 0; row < sizeof rosters / sizeof rosters[0]; ++row) {
    for (int slot = 0; slot < 2; ++slot) {
      for (int fan = 0; fan < 2; ++fan) {
        int selected = rosters[row];
        battle_init(slot ? 4 : selected, slot ? selected : 4);
        g_battle.phase = PHASE_FIGHT;
        g_battle.arena = fan ? 8 : 0;
        Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
        owner->wins = 1;
        battle_damage(victim, Q16_ONE);
        advance_ticks(1, NULL);
        for (int tick = 0; tick < 180 && g_battle.finish_waiting; ++tick) {
          advance_ticks(1, NULL);
        }
        owner->body.pos[AXIS_X] = slot ? 0x4ccc : -0x4ccc;
        victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
        const CpuMoves *cpu = roster_def(selected)->cpu;
        check(cpu != NULL && cpu->enter[CPU_FINISH_FAN] != NULL && cpu->enter[CPU_FINISH_SPIKE] != NULL,
            "Partial fighter must attach its generated CPU stage table to the roster");
        check(battle_cpu_action(owner, fan ? CPU_FINISH_FAN : CPU_FINISH_SPIKE), "CPU must reach the reviewed stage entry");
        advance_ticks(1, NULL);
        check(g_battle.phase == PHASE_FATALITY && g_battle.finisher == (fan ? FINISH_FAN : FINISH_SPIKE),
            "CPU stage entry must acquire its actual fan or spike scene");
        advance_ticks(650, NULL);
        check(g_battle.phase == PHASE_MATCH_OVER, "CPU stage scene must complete for both player slots");
        battle_reset_match();
      }
    }
  }
}

static void check_generated_cpu_finish_dispatch(void) {
  const int fighters[] = {14, 5, 6, 11};
  for (size_t index = 0; index < sizeof fighters / sizeof fighters[0]; ++index) {
    int selected = fighters[index];
    for (int slot = 0; slot < 2; ++slot) {
      for (int finish = 0; finish < 2; ++finish) {
        for (int far = 0; far < 2; ++far) {
          battle_init(slot ? 4 : selected, slot ? selected : 4);
          g_battle.phase = PHASE_FIGHT;
          Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
          owner->wins = 1;
          battle_damage(victim, Q16_ONE);
          advance_ticks(1, NULL);
          for (int tick = 0; tick < 180 && g_battle.finish_waiting; ++tick) {
            advance_ticks(1, NULL);
          }
          const CpuMoves *moves = roster_def(selected)->cpu;
          check(moves != NULL && moves->enter[CPU_FINISH_1] != NULL && moves->enter[CPU_FINISH_2] != NULL,
              "Generated CPU table must connect fatalities independently of human commands");
          q16 distance = far ? 5 * Q16_ONE : 0x9999;
          owner->body.pos[AXIS_X] = slot ? distance / 2 : -distance / 2;
          victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
          owner->human = false;
          owner->cpu =
              (CpuState){.finish_chosen = true, .plan = (finish ? CPU_FINISH_2 : CPU_FINISH_1) + 1, .target = moves->finish_distance[finish], .timer = 240};
          for (int tick = 0; tick < 240 && g_battle.phase == PHASE_FINISH; ++tick) {
            advance_ticks(1, NULL);
          }
          int expected = selected == 14 ? 17 + finish : selected == 6 ? 21 + finish : selected == 11 ? 23 + finish : FINISH_FUJIN_1 + finish;
          if (g_battle.phase != PHASE_FATALITY || g_battle.finisher != expected) {
            fprintf(stderr, "CPU finish roster%d slot%d finish%d far%d phase%d id%d distance%f\n", selected, slot, finish, far, g_battle.phase,
                g_battle.finisher, fighter_distance(owner));
          }
          check(g_battle.phase == PHASE_FATALITY && g_battle.finisher == expected,
              "CPU must approach or retreat into the authored range and enter its selected real fatality");
          check(owner->fujin_finish_inputs == 0, "CPU scene dispatch must bypass human input counting and use existing frame entries");
          battle_reset_match();
        }
      }
      for (int stage = 0; stage < 2; ++stage) {
        battle_init(slot ? 4 : selected, slot ? selected : 4);
        g_battle.phase = PHASE_FIGHT;
        g_battle.arena = stage ? 8 : 0;
        Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
        owner->wins = 1;
        battle_damage(victim, Q16_ONE);
        advance_ticks(1, NULL);
        for (int tick = 0; tick < 180 && g_battle.finish_waiting; ++tick) {
          advance_ticks(1, NULL);
        }
        owner->body.pos[AXIS_X] = slot ? 0x4ccc : -0x4ccc;
        victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
        check(battle_cpu_action(owner, stage ? CPU_FINISH_FAN : CPU_FINISH_SPIKE), "Reviewed CPU stage-finisher slot must be registered");
        advance_ticks(2, NULL);
        check(g_battle.phase == PHASE_FATALITY && g_battle.finisher == (stage ? FINISH_FAN : FINISH_SPIKE),
            "CPU stage dispatch must retain its own fan or spike scene");
        battle_reset_match();
      }
    }
  }
}

static void check_fujin_first_fatality(void) {
  for (int slot = 0; slot < 2; ++slot) {
    for (int target = 2; target <= 4; target += 2) {
      for (int reset_at = 0; reset_at < 4; ++reset_at) {
        battle_init(slot ? target : 5, slot ? 5 : target);
        g_battle.phase = PHASE_FIGHT;
        Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
        owner->wins = 1;
        battle_damage(victim, Q16_ONE);
        advance_ticks(1, NULL);
        for (int tick = 0; tick < 180 && g_battle.finish_waiting; ++tick) {
          advance_ticks(1, NULL);
        }
        owner->body.pos[AXIS_X] = slot ? Q16_ONE : -Q16_ONE;
        victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
        check(battle_can_finish(owner), "Fujin first fatality needs a deciding KO");
        for (int button = BUTTON_BLOCK; button <= BUTTON_RUN; ++button) {
          Input input[2] = {0};
          input[slot].held = BUTTON_BIT(button);
          battle_step(input);
          advance_ticks(3, NULL);
        }
        check(owner->fujin_finish_inputs == 0, "Fujin counted finish gate must require Block and Run together");
        for (int press = 0; press < 20 && g_battle.phase == PHASE_FINISH; ++press) {
          Input input[2] = {0};
          input[slot].held = BUTTON_BIT(BUTTON_BLOCK) | BUTTON_BIT(BUTTON_RUN);
          battle_step(input);
          advance_ticks(3, NULL);
        }
        check(g_battle.phase == PHASE_FATALITY && g_battle.finisher == FINISH_FUJIN_1 && owner->fujin_finish_inputs >= 14,
            "The fourteenth valid native Block+Run gate visit must start Fujin's first scene");
        bool wind = false, draw = false, fire = false, lifted = false, spinning = false, visual = false;
        int bolts = 0;
        int limit = reset_at == 1 ? 80 : reset_at == 2 ? 250 : reset_at == 3 ? 420 : 700;
        for (int tick = 0; tick < limit; ++tick) {
          advance_ticks(1, NULL);
          wind |= fighter_move(owner)->clip == 399;
          draw |= fighter_move(owner)->clip == 210 || fighter_move(owner)->clip == 212;
          fire |= fighter_move(owner)->clip == 213;
          lifted |= victim->body.pos[AXIS_Y] < victim->body.ground - Q16_ONE;
          spinning |= victim->body.spin[AXIS_Y] != 0;
          for (int i = 0; i < MAX_PROJECTILES; ++i) {
            const Projectile *object = &g_battle.projectiles[i];
            bolts += object->kind != 0 && object->mesh_animation != NULL && object->mesh_animation->first == 106 && object->ticks == 0;
          }
          for (int i = 0; i < MAX_PARTICLES; ++i) {
            const Particle *particle = &g_battle.particles[i];
            visual |= particle->ticks > 0 && particle->visual != NULL && particle->visual->animation.source == MESH_FUJIN_WIND;
          }
        }
        if (reset_at == 0) {
          check(wind && draw && fire && lifted && spinning && visual && owner->native_variant == 3 && bolts == 3 && g_battle.phase == PHASE_MATCH_OVER,
              "Fujin first scene must lift/spin, draw its crossbow, fire three times and complete");
          check(owner->armed && fighter_move(owner)->clip == 213 && victim->appearance == APPEARANCE_HIDDEN,
              "Fujin first ending must retain its actual armed pose and exploded victim");
        }
        battle_reset_match();
        advance_ticks(180, NULL);
        check(owner->health == Q16_ONE && victim->health == Q16_ONE && !owner->armed && owner->fujin_finish_inputs == 0,
            "Reset must clear counted input, tornado state and pending crossbow shots");
        for (int i = 0; i < MAX_PROJECTILES; ++i) {
          check(g_battle.projectiles[i].kind == 0, "Reset must retire all wind and finisher bolts");
        }
      }
    }
  }
}

static void check_fujin_second_fatality(void) {
  for (int slot = 0; slot < 2; ++slot) {
    for (int target = 2; target <= 4; target += 2) {
      for (int reset_at = 0; reset_at < 3; ++reset_at) {
        battle_init(slot ? target : 5, slot ? 5 : target);
        g_battle.phase = PHASE_FIGHT;
        Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
        owner->wins = 1;
        battle_damage(victim, Q16_ONE);
        advance_ticks(1, NULL);
        for (int tick = 0; tick < 180 && g_battle.finish_waiting; ++tick) {
          advance_ticks(1, NULL);
        }
        check(battle_can_finish(owner), "Fujin wind fatality needs the deciding-KO command window");
        owner->body.pos[AXIS_X] = slot ? Q16_ONE : -Q16_ONE;
        victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
        check(retail_can_finish_range(owner), "Fujin second fatality must accept its two-unit range");
        const Controls *controls = roster_def(5)->controls;
        const CommandDef *command = NULL;
        for (int i = 0; i < controls->command_count; ++i) {
          if (controls->commands[i].enter == retail_finish_second) {
            command = &controls->commands[i];
          }
        }
        check(command != NULL && command->count == 4, "Fujin second fatality must retain Down,Forward,Forward,Up+Block");
        for (int i = 0; i < command->count; ++i) {
          Button direction = command->tokens[i] == TOKEN_DOWN ? BUTTON_DOWN : command->tokens[i] == TOKEN_UP ? BUTTON_UP : slot ? BUTTON_LEFT : BUTTON_RIGHT;
          Input input[2] = {0};
          input[slot].held = BUTTON_BIT(direction);
          if (i == command->count - 1) {
            input[slot].held |= BUTTON_BIT(command->button);
          }
          battle_step(input);
          advance_ticks(2, NULL);
        }
        check(g_battle.phase == PHASE_FATALITY && g_battle.finisher == FINISH_FUJIN_2, "Fujin actual second command must select its wind scene");
        bool cast = false, held = false, fall = false, torn = false;
        int limit = reset_at == 1 ? 80 : reset_at == 2 ? 250 : 700;
        for (int tick = 0; tick < limit; ++tick) {
          advance_ticks(1, NULL);
          cast |= fighter_move(owner)->clip == 547 || fighter_move(owner)->clip == 548;
          held |= fighter_move(victim)->clip == 549;
          fall |= fighter_move(victim)->clip == 550;
          torn |= victim->native_variant == 14;
        }
        if (reset_at == 0) {
          check(cast && held && fall && torn && g_battle.phase == PHASE_MATCH_OVER, "Fujin wind scene must tear all native joints, retain550 and finish");
          check(fighter_move(owner)->clip == 548 && victim->health == 0, "Fujin must retain its own ending pose and defeated victim");
        }
        battle_reset_match();
        advance_ticks(180, NULL);
        check(owner->scene_model == NULL && victim->scene_model == NULL && owner->health == Q16_ONE && victim->health == Q16_ONE,
            "Reset must clear wind scene replacement models and delayed actions");
      }
    }
  }
}

static void check_advancing_sweeps(void) {
  const int roster[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17};
  for (size_t character = 0; character < sizeof roster / sizeof roster[0]; ++character) {
    int selected = roster[character];
    int sweep_clip = selected % 2 && selected != 15 ? 62 : 63;
    for (int slot = 0; slot < 2; ++slot) {
      for (int scenario = 0; scenario < 5; ++scenario) {
        battle_init(slot ? 4 : selected, slot ? selected : 4);
        g_battle.phase = PHASE_FIGHT;
        Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
        q16 distance = scenario == 3 ? 6 * Q16_ONE : Q16_ONE;
        owner->body.pos[AXIS_X] = slot ? distance / 2 : -distance / 2;
        victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
        Input input[2] = {0};
        input[slot].held = BUTTON_BIT(BUTTON_LOW_KICK) | BUTTON_BIT(slot ? BUTTON_RIGHT : BUTTON_LEFT);
        if (scenario == 1 || scenario == 2) {
          input[1 - slot].held = BUTTON_BIT(BUTTON_BLOCK);
        }
        if (scenario == 2) {
          input[1 - slot].held |= BUTTON_BIT(BUTTON_DOWN);
        }
        battle_step(input);
        input[slot] = (Input){0};
        advance_ticks(2, input);
        if (owner->state != S_SWEEP || fighter_move(owner)->clip != sweep_clip || owner->action != ACTION_RETAIL_010C) {
          fprintf(stderr, "sweep roster%d slot%d scenario%d state%d clip%d action%x facing%d\n", selected, slot, scenario, owner->state,
              fighter_move(owner)->clip, owner->action, owner->facing);
        }
        check(owner->state == S_SWEEP && fighter_move(owner)->clip == sweep_clip && owner->action == ACTION_RETAIL_010C,
            "Back+LK must enter the selected generated retail sweep");
        input[slot] = (Input){0};
        bool recovered = false;
        q16 previous_health = victim->health;
        for (int tick = 0; tick < 100; ++tick) {
          if (scenario == 4 && tick == 3) {
            battle_react(owner, 11);
          }
          battle_step(input);
          if (victim->health != previous_health) {
            check(fighter_sample(owner) == 23 || fighter_sample(owner) == 24, "Retail sweep contacts must advance to authored23/24 rather than holding22");
            previous_health = victim->health;
          }
          recovered |= owner->action == 0x1006;
        }
        q16 damage = scenario >= 3 ? 0 : scenario == 2 ? q16_mul(0x23d7, 0x3333) : 0x23d7;
        check(victim->health == Q16_ONE - damage, "Retail sweep must retain fourteen-percent damage, low guard and startup interruption");
        check(owner->state == S_IDLE && !fighter_airborne(owner) && (scenario == 4 || recovered),
            "Advancing sweep must reach native action1006 recovery and return to its controller");
        battle_reset_match();
      }
    }
  }
}

static void check_fujin_wind_grab(void) {
  for (int slot = 0; slot < 2; ++slot) {
    for (int scenario = 0; scenario < 5; ++scenario) {
      battle_init(slot ? 4 : 5, slot ? 5 : 4);
      g_battle.phase = PHASE_FIGHT;
      Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
      owner->wins = 1;
      owner->body.pos[AXIS_X] = slot ? 4 * Q16_ONE : -4 * Q16_ONE;
      victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
      victim->hits = 3;
      check(!fujin_can_wind_grab(owner), "Wind grab must reject targets beyond its two-hit combo gate");
      victim->hits = 0;
      victim->action = 0x1001;
      check(!fujin_can_wind_grab(owner), "Wind grab must reject an existing vortex victim");
      victim->action = 0;
      if (scenario == 4) {
        victim->health = 1;
      }
      drive_owned_command(slot, fujin_wind_grab);
      check(fighter_move(owner)->clip == 10 && owner->action == 0x20a, "Forward,Down+LP must select the real wind-grab opening");
      q16 start = owner->body.pos[AXIS_X];
      Input input[2] = {0};
      bool captured = false, orbit = false, blocked = false, interrupted = false, released = false;
      for (int tick = 0; tick < 350; ++tick) {
        input[slot].held = scenario == 0 && tick >= 20 ? 0 : BUTTON_BIT(BUTTON_LOW_PUNCH);
        if (scenario != 0 && !captured && !blocked) {
          input[slot].held |= BUTTON_BIT(slot ? BUTTON_LEFT : BUTTON_RIGHT);
        }
        input[1 - slot].held = scenario == 2 ? BUTTON_BIT(BUTTON_BLOCK) : 0;
        if (scenario == 3 && captured && !interrupted) {
          battle_react(owner, 11);
          interrupted = true;
        }
        battle_step(input);
        captured |= victim->action == 0x100d;
        blocked |= owner->action == 0x1015 && fighter_move(owner)->clip == 13;
        orbit |= victim->action == 0x100d && abs(victim->body.pos[AXIS_Z] - owner->body.pos[AXIS_Z]) > 0x1999;
        if (scenario == 0 && owner->state == S_IDLE) {
          released = true;
          check(owner->wind_grab_ready == g_battle.ticks + 90 && !fujin_can_wind_grab(owner), "LP release must start the actual ninety-tick reuse delay");
          uint64_t ready = owner->wind_grab_ready;
          advance_ticks(89, NULL);
          check(g_battle.ticks == ready - 1 && !fujin_can_wind_grab(owner), "Wind-grab cooldown must not open one tick early");
          advance_ticks(1, NULL);
          check(fujin_can_wind_grab(owner), "Wind-grab reuse gate must open on its native boundary");
          break;
        }
      }
      if (scenario == 0) {
        check(released && !captured && victim->health == Q16_ONE, "Releasing LP before reaching the target must cancel without delayed damage");
      } else if (scenario == 2) {
        check(blocked && !captured && victim->health == Q16_ONE && victim->reaction == 7,
            "A grounded block must select reaction7 and the long owner recovery without grab damage");
      } else {
        check(
            captured && orbit && abs(owner->body.pos[AXIS_X] - start) > 2 * Q16_ONE, "Held LP and screen steering must reach and orbit the captured opponent");
        q16 expected = scenario == 3 ? Q16_ONE : scenario == 4 ? 0 : Q16_ONE - 0x1eb8;
        if (victim->health != expected) {
          fprintf(stderr, "Wind grab slot%d scenario%d health%d expected%d reaction%d owneraction%x\n", slot, scenario, victim->health, expected,
              victim->reaction, owner->action);
        }
        check(victim->health == expected, "Wind grab must preserve twelve-percent delayed damage and cancel it on owner interruption");
      }
      check(!(victim->flags & (FF_PAIRED | FF_UNTOUCHABLE)) && !fighter_airborne(victim),
          "Orbit release or interruption must restore contact and settle the victim");
      battle_reset_match();
      for (int i = 0; i < MAX_PROJECTILES; ++i) {
        check(g_battle.projectiles[i].kind == 0, "Wind-grab reset must retire both independent wind objects");
      }
    }
    for (int delay = 0; delay < 2; ++delay) {
      battle_init(slot ? 4 : 5, slot ? 5 : 4);
      g_battle.phase = PHASE_FIGHT;
      fighter_interrupt(&g_battle.fighters[slot], fujin_wind_grab);
      Input input[2] = {0};
      input[slot].held = BUTTON_BIT(BUTTON_LOW_PUNCH) | BUTTON_BIT(slot ? BUTTON_LEFT : BUTTON_RIGHT);
      advance_ticks(delay ? 80 : 2, input);
      battle_reset_match();
      advance_ticks(120, NULL);
      check(g_battle.fighters[0].health == Q16_ONE && g_battle.fighters[1].health == Q16_ONE,
          "Reset during wind-grab startup or capture must discard its delayed damage and actor callbacks");
    }
  }
}

static void check_fujin_triple_kick(void) {
  for (int slot = 0; slot < 2; ++slot) {
    for (int scenario = 0; scenario < 7; ++scenario) {
      battle_init(slot ? 4 : 5, slot ? 5 : 4);
      g_battle.phase = PHASE_FIGHT;
      Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
      owner->wins = 1;
      q16 distance = scenario == 2 ? 4 * Q16_ONE : Q16_ONE;
      owner->body.pos[AXIS_X] = slot ? distance : -distance;
      victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
      if (scenario == 4) {
        victim->health = 1;
      }
      if (scenario == 5) {
        owner->flags |= FF_FOLLOW_UP;
      }
      drive_owned_command(slot, fujin_triple_kick);
      check(fighter_move(owner)->clip == 100 && owner->action == 0x208, "Down,Forward+HK must select Fujin's actual advancing triple kick");
      Input input[2] = {0};
      if (scenario == 1) {
        input[1 - slot].held = BUTTON_BIT(BUTTON_BLOCK);
      }
      bool interrupted = false, first = false, middle = false, last = false, recoil = false;
      for (int tick = 0; tick < 300; ++tick) {
        first |= victim->reaction == 93 && fighter_move(victim)->clip == 127;
        middle |= victim->reaction == 94 && fighter_move(victim)->clip == 311;
        last |= victim->reaction == 95 && fighter_move(victim)->clip == 131;
        recoil |= fighter_airborne(owner) && owner->body.gravity == 0x147;
        if ((scenario == 3 && first && !interrupted) || (scenario == 6 && tick == 1)) {
          battle_react(owner, 11);
          interrupted = true;
        }
        battle_step(input);
      }
      q16 damage = scenario == 2 || scenario == 6 ? 0 : scenario == 1 ? q16_mul(0x30a3, 0x3333) : 0x30a3;
      q16 expected = scenario == 4 ? 0 : Q16_ONE - damage;
      if (victim->health != expected) {
        fprintf(stderr, "Triple kick slot%d scenario%d health%d expected%d stages%d%d%d recoil%d\n", slot, scenario, victim->health, expected, first, middle,
            last, recoil);
      }
      check(victim->health == expected, "Fujin triple kick must apply its nineteen-percent D9 record exactly once, with native chip/miss behavior");
      if (scenario == 0 || scenario == 5) {
        if (!(first && middle && last)) {
          fprintf(stderr, "Triple stages slot%d scenario%d seen%d%d%d finalreaction%d clip%d ownerstage%d\n", slot, scenario, first, middle, last,
              victim->reaction, fighter_move(victim)->clip, owner->native_variant);
        }
        check(first && middle && last, "Successful triple kick must reach all three actual victim stages");
      }
      if (scenario == 4) {
        check(owner->wins == 2 && g_battle.winner == slot, "Lethal triple-kick contact must award the result once and cancel captured follow-through safely");
      }
      check(scenario != 1 || recoil, "Blocked triple kick must enter its native backward recoil and landing");
      check(scenario != 3 || (interrupted && !middle && !last), "Interrupting the captured first stage must cancel later paired stages");
      check(!fighter_airborne(owner) && !fighter_airborne(victim) && !(owner->flags & FF_PAIRED) && !(victim->flags & FF_PAIRED),
          "Triple-kick completion or interruption must release both actors and settle their floor positions");
      battle_reset_match();
    }
    battle_init(slot ? 4 : 5, slot ? 5 : 4);
    g_battle.phase = PHASE_FIGHT;
    check(battle_cpu_action(&g_battle.fighters[slot], CPU_SPECIAL_NEAR), "Fujin's CPU triple-kick row must remain registered");
    advance_ticks(2, NULL);
    check(fighter_move(&g_battle.fighters[slot])->clip == 100, "Fujin CPU near choice must enter the same actual triple-kick implementation");
    battle_reset_match();
  }
}

static void check_fujin_wind(void) {
  for (int slot = 0; slot < 2; ++slot) {
    for (int scenario = 0; scenario < 6; ++scenario) {
      battle_init(slot ? 4 : 5, slot ? 5 : 4);
      g_battle.phase = PHASE_FIGHT;
      Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
      owner->wins = 1;
      owner->body.pos[AXIS_X] = slot ? 0x14ccc : -0x14ccc;
      victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
      check(!fujin_can_wind_slam(owner), "Wind slam must reject a target not caught in the vortex");
      victim->hits = 3;
      check(!fujin_can_wind(owner), "Wind catch must enforce its native two-hit combo limit");
      victim->hits = 0;
      if (scenario == 5) {
        victim->health = 0x1000;
      }
      drive_owned_command(slot, fujin_wind);
      check(fighter_move(owner)->clip == 98, "Fujin's Back,Forward,Back+HP must enter the actual wind caster clip");
      if (scenario == 2) {
        victim->body.pos[AXIS_X] = slot ? -6 * Q16_ONE : 6 * Q16_ONE;
      }
      Input input[2] = {0};
      if (scenario == 1) {
        input[1 - slot].held = BUTTON_BIT(BUTTON_BLOCK);
      }
      bool caught = false, slammed = false, interrupted = false, bounced = false, effect = false;
      int lifts = 0;
      for (int tick = 0; tick < 500; ++tick) {
        if (victim->action == 0x1001) {
          caught = true;
          lifts = victim->native_variant > lifts ? victim->native_variant : lifts;
          check(!fujin_can_wind(owner) && fujin_can_wind_slam(owner), "Only the slam may target the active vortex");
          if ((scenario == 3 || scenario == 5) && owner->state == S_IDLE && !slammed) {
            drive_owned_command(slot, fujin_wind_slam);
            slammed = true;
            check(fighter_move(owner)->clip == 99, "Back,Forward,Down+LK must select the matching wind slam instead of a normal kick");
          }
          if (scenario == 4 && !interrupted) {
            battle_react(owner, 11);
            interrupted = true;
          }
        }
        battle_step(input);
        bounced |= fighter_move(victim)->clip == 179;
        for (int i = 0; i < MAX_PARTICLES; ++i) {
          const Particle *p = &g_battle.particles[i];
          effect |= p->ticks > 0 && p->visual != NULL && p->visual->animation.first >= 84 && p->visual->animation.first <= 91;
        }
      }
      check((scenario == 1 || scenario == 2) ? !caught : caught && effect, "Wind contact must distinguish hit, grounded block and miss");
      if (scenario == 3 || scenario == 5) {
        check(slammed && bounced && victim->health == (scenario == 5 ? 0 : Q16_ONE - 0x147a),
            "Wind slam must preserve its eight-percent damage, victim bounce and lethal result");
      } else {
        check(victim->health == Q16_ONE, "Wind catch alone, block, miss and owner interruption must not invent damage");
        if (scenario == 0 || scenario == 4) {
          check(lifts == 3, "Unslammed vortex must finish all four native acceleration cycles");
        }
      }
      if (fighter_airborne(victim) || (scenario != 5 && victim->state != (scenario == 1 ? S_BLOCK : S_IDLE))) {
        fprintf(stderr, "Wind slot%d scenario%d caught%d slam%d state%d action%x clip%d sample%d y%d floor%d vy%d gravity%d lifts%d\n", slot, scenario, caught,
            slammed, victim->state, victim->action, fighter_move(victim)->clip, fighter_sample(victim), victim->body.pos[AXIS_Y], victim->body.ground,
            victim->body.vel[AXIS_Y], victim->body.gravity, lifts);
      }
      check(!fighter_airborne(victim) && (scenario == 5 || victim->state == (scenario == 1 ? S_BLOCK : S_IDLE)),
          "Wind victim must land and recover, or remain defeated after KO");
      battle_reset_match();
      for (int i = 0; i < MAX_PROJECTILES; ++i) {
        check(g_battle.projectiles[i].kind == 0, "Wind reset must remove its independent emitter");
      }
    }
    for (int delay = 0; delay < 2; ++delay) {
      battle_init(slot ? 4 : 5, slot ? 5 : 4);
      g_battle.phase = PHASE_FIGHT;
      g_battle.fighters[slot].body.pos[AXIS_X] = slot ? 0x14ccc : -0x14ccc;
      g_battle.fighters[1 - slot].body.pos[AXIS_X] = -g_battle.fighters[slot].body.pos[AXIS_X];
      drive_owned_command(slot, fujin_wind);
      advance_ticks(delay ? 50 : 2, NULL);
      battle_reset_match();
      advance_ticks(120, NULL);
      check(g_battle.fighters[0].health == Q16_ONE && g_battle.fighters[1].health == Q16_ONE,
          "Reset before or during the vortex must not leave delayed wind damage or a retained victim continuation");
    }
  }
}

static void check_fujin_dive(void) {
  for (int slot = 0; slot < 2; ++slot) {
    for (int scenario = 0; scenario < 4; ++scenario) {
      battle_init(slot ? 4 : 5, slot ? 5 : 4);
      g_battle.phase = PHASE_FIGHT;
      Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
      owner->body.pos[AXIS_Y] = -0x18000;
      check(!fujin_can_dive(owner), "Fujin dive must reject heights below1.8");
      owner->body.pos[AXIS_Y] = -0x24000;
      victim->hits = 5;
      check(!fujin_can_dive(owner), "Fujin dive must reject an opponent already hit five times");
      victim->hits = 0;
      check(fujin_can_dive(owner), "Fujin dive must accept sufficient height and a valid target combo count");
      owner->body.gravity = 0;
      owner->body.on_land = NULL;
      Input input[2] = {0};
      input[slot].held = BUTTON_BIT(BUTTON_UP);
      battle_step(input);
      input[slot].held = BUTTON_BIT(BUTTON_DOWN) | BUTTON_BIT(BUTTON_LOW_KICK);
      battle_step(input);
      advance_ticks(2, NULL);
      check(fighter_move(owner)->clip == 58 && owner->action == 0x20f, "Fujin's Up,Down+LK command must enter the actual aerial dive");
      int health = victim->health;
      bool recovered = false;
      for (int tick = 0; tick < 240; ++tick) {
        bool querying = owner->action == 0x20f && fighter_sample(owner) == 9;
        if (scenario != 0 && querying && !owner->connected) {
          /* Place the target at the moving contact plane so this checks the
           * contact/recoil path without depending on incidental trajectory. */
          victim->body.pos[AXIS_X] = owner->body.pos[AXIS_X] + q16_mul(owner->heading[0], 0x9999);
          victim->body.pos[AXIS_Z] = owner->body.pos[AXIS_Z] + q16_mul(owner->heading[1], 0x9999);
          victim->body.pos[AXIS_Y] = scenario == 2 ? victim->body.ground : owner->body.pos[AXIS_Y];
          if (scenario == 2) {
            owner->body.pos[AXIS_Y] = owner->body.ground - 0x6666;
          }
          victim->body.gravity = 0;
          victim->body.on_land = NULL;
        } else if (scenario == 0) {
          victim->body.pos[AXIS_X] = slot ? -0x50000 : 0x50000;
        }
        if (scenario == 2) {
          input[1 - slot].held = BUTTON_BIT(BUTTON_BLOCK);
        }
        if (scenario == 3 && tick == 3) {
          battle_react(owner, 11);
        }
        battle_step(input);
        input[slot].held = 0;
        if (owner->state == S_IDLE && !fighter_airborne(owner)) {
          recovered = true;
          break;
        }
      }
      check(recovered, "Fujin dive must recover on hit, block, miss and interruption");
      if (scenario == 0 || scenario == 3) {
        check(victim->health == health, "Fujin dive miss or pre-contact interruption must not damage its target");
      } else if (scenario == 1) {
        check(victim->health == health - 0x147a, "Fujin dive must apply its D[15] eight-percent damage exactly once");
      } else {
        check(victim->health < health && victim->health > health - 0x147a, "Fujin dive block must apply chip instead of full damage");
      }
      battle_reset_match();
      check(owner->body.on_land == NULL, "Reset must discard Fujin dive motion and continuations");
    }
  }
}

static void check_cage_first_fatality(void) {
  for (int slot = 0; slot < 2; ++slot) {
    for (int female = 0; female < 2; ++female) {
      for (int reset_at = 0; reset_at < 3; ++reset_at) {
        int target = female ? 2 : 4;
        battle_init(slot ? target : 14, slot ? 14 : target);
        g_battle.phase = PHASE_FIGHT;
        Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
        owner->wins = 1;
        battle_damage(victim, Q16_ONE);
        advance_ticks(1, NULL);
        for (int tick = 0; tick < 180 && g_battle.finish_waiting; ++tick) {
          advance_ticks(1, NULL);
        }
        check(battle_can_finish(owner), "Cage first fatality needs the deciding-KO command window");
        owner->body.pos[AXIS_X] = slot ? 0x6666 : -0x6666;
        victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
        const Controls *controls = roster_def(14)->controls;
        const CommandDef *selected = NULL;
        for (int i = 0; i < controls->command_count; ++i) {
          if (controls->commands[i].enter == retail_finish_first) {
            selected = &controls->commands[i];
          }
        }
        check(selected != NULL, "Cage first fatality needs its real generated command");
        for (int i = 0; i < selected->count; ++i) {
          bool forward = selected->tokens[i] == TOKEN_FORWARD;
          Button direction = selected->tokens[i] == TOKEN_DOWN ? BUTTON_DOWN : (forward != (slot != 0) ? BUTTON_RIGHT : BUTTON_LEFT);
          Input input[2] = {0};
          input[slot].held = BUTTON_BIT(direction);
          if (i == selected->count - 1) {
            input[slot].held |= BUTTON_BIT(selected->button);
          }
          battle_step(input);
          advance_ticks(2, NULL);
        }
        check(g_battle.phase == PHASE_FATALITY && fighter_move(owner)->clip == (female ? 407 : 406),
            "Cage first command must select its matching male or female paired scene");
        int timer = g_battle.time_ticks;
        bool severed = false;
        int limit = reset_at == 1 ? 100 : reset_at == 2 ? 690 : 1100;
        for (int tick = 0; tick < limit && g_battle.phase == PHASE_FATALITY; ++tick) {
          advance_ticks(1, NULL);
          for (int i = 0; i < MAX_SCENE_ACTORS; ++i) {
            SceneActor *body = &g_battle.scene_actors[i];
            if (body->active) {
              severed = true;
              check(body->source == 1 - slot && body->owner == slot && fighter_move(&body->actor)->clip == 408 &&
                        body->actor.hidden_joints[0] == ((UINT64_C(1) << 11) | (UINT64_C(1) << 12)),
                  "Detached torso must keep its source model, independent clip and correct body mask");
            }
          }
        }
        if (reset_at == 0) {
          check(severed && g_battle.phase == PHASE_MATCH_OVER && fighter_move(owner)->clip == 443 && fighter_sample(victim) == 309,
              "Cage torso rip must finish the lower body and detached torso before the result");
          check(owner->native_variant == 7 && victim->hidden_joints[0] == (UINT64_C(1) << 3) && owner->wins == 2 && victim->health == 0 &&
                    g_battle.time_ticks == timer,
              "Cage first fatality must preserve the decided result and frozen clock");
        }
        battle_reset_match();
        check(victim->hidden_joints[0] == 0 && owner->hidden_joints[0] == 0, "Cage first fatality reset must restore both bodies");
        for (int i = 0; i < MAX_SCENE_ACTORS; ++i) {
          check(!g_battle.scene_actors[i].active, "Reset before or after severing must retire the independent torso");
        }
      }
    }
  }
}

/* Jax fatalities: the LK-release F,F,D,F arm rip needs a 240-tick LK hold;
 * BLOCK B,F,F,D claps the head. Both scenes run on frame phases, keep the
 * decided result and reset cleanly. */
static const CommandDef *jax_finish_command(Button button, int count, unsigned flags) {
  const Controls *controls = roster_def(13)->controls;
  for (int i = 0; i < controls->command_count; ++i) {
    const CommandDef *command = &controls->commands[i];
    if (command->button == button && command->count == count && (command->flags & 0x100) == flags) {
      return command;
    }
  }
  return NULL;
}

static void jax_enter_finish(int slot, int target) {
  battle_init(slot ? target : 13, slot ? 13 : target);
  g_battle.phase = PHASE_FIGHT;
  Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
  owner->wins = 1;
  battle_damage(victim, Q16_ONE);
  advance_ticks(1, NULL);
  for (int tick = 0; tick < 180 && g_battle.finish_waiting; ++tick) {
    advance_ticks(1, NULL);
  }
  check(battle_can_finish(owner), "Jax fatality needs the deciding-KO window");
  owner->body.pos[AXIS_X] = slot ? 0x6666 : -0x6666;
  victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
}

static void jax_enter_tokens(int slot, const CommandDef *command, unsigned held, bool release) {
  for (int i = 0; i < command->count; ++i) {
    Button direction = command->tokens[i] == TOKEN_DOWN ? BUTTON_DOWN : (command->tokens[i] == TOKEN_FORWARD) != (slot != 0) ? BUTTON_RIGHT : BUTTON_LEFT;
    Input input[2] = {0};
    input[slot].held = BUTTON_BIT(direction) | held;
    if (i == command->count - 1 && !release) {
      input[slot].held |= BUTTON_BIT(command->button);
    }
    battle_step(input);
    input[slot].held = held;
    if (i < command->count - 1 || release) {
      battle_step(input);
      battle_step(input);
    }
  }
  if (release) {
    Input input[2] = {0};
    battle_step(input);
    advance_ticks(2, NULL);
  }
}

static void check_jax_first_fatality(void) {
  const CommandDef *command = jax_finish_command(BUTTON_LOW_KICK, 4, 0x100);
  check(command != NULL, "Jax first fatality needs its LK-release command record");
  for (int slot = 0; slot < 2; ++slot) {
    for (int listed = 0; listed < 2; ++listed) {
      for (int reset_at = 0; reset_at < 2; ++reset_at) {
        int target = listed ? 4 : 2;
        battle_init(slot ? target : 13, slot ? 13 : target);
        g_battle.phase = PHASE_FIGHT;
        Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
        owner->body.pos[AXIS_X] = slot ? 3 * Q16_ONE : -3 * Q16_ONE;
        victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
        owner->wins = 1;
        Input hold[2] = {0};
        hold[slot].held = BUTTON_BIT(BUTTON_LOW_KICK);
        battle_step(hold);
        uint64_t pressed = g_battle.ticks;
        for (int tick = 0; tick < 60; ++tick) {
          battle_step(hold);
        }
        battle_damage(victim, Q16_ONE);
        uint64_t needed = slot == 0 && listed && reset_at ? 150 : 232;
        for (int tick = 0; tick < 400 && (g_battle.finish_waiting || g_battle.ticks - pressed < needed); ++tick) {
          battle_step(hold);
        }
        check(battle_can_finish(owner) && owner->latch[BUTTON_LOW_KICK] == pressed, "Jax arm rip test needs the window with LK held since its press");
        owner->body.pos[AXIS_X] = slot ? 0x4000 : -0x4000;
        victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
        jax_enter_tokens(slot, command, BUTTON_BIT(BUTTON_LOW_KICK), true);
        if (needed < 232) {
          check(g_battle.phase == PHASE_FINISH, "Jax arm rip must reject an LK hold shorter than 240 ticks");
          continue;
        }
        check(g_battle.phase == PHASE_FATALITY, "Jax F,F,D,F LK release must start the arm rip");
        bool owner_scene = false, victim_scene = false, first_arm = false, second_arm = false;
        int limit = reset_at ? 200 : 900;
        for (int tick = 0; tick < limit && g_battle.phase == PHASE_FATALITY; ++tick) {
          advance_ticks(1, NULL);
          owner_scene |= fighter_move(owner)->clip == 384;
          victim_scene |= fighter_move(victim)->clip == (listed ? 204 : 400);
          first_arm |= (victim->hidden_joints[0] & (UINT64_C(1) << 5)) != 0;
          second_arm |= (victim->hidden_joints[0] & (UINT64_C(1) << 6)) != 0;
        }
        if (!reset_at) {
          check(owner_scene && victim_scene && first_arm && second_arm, "Jax arm rip must reach both arms on the roster-selected victim clip");
          check(g_battle.phase == PHASE_MATCH_OVER && fighter_move(owner)->clip == 443 && owner->wins == 2 && victim->health == 0,
              "Jax arm rip must end in its victory loop with the decided result");
        }
        battle_reset_match();
        check(victim->hidden_joints[0] == 0, "Jax arm rip reset must restore the victim");
        for (int i = 0; i < MAX_SCENE_ATTACHMENTS; ++i) {
          check(g_battle.attachments[i].def == NULL, "Jax arm rip reset must retire the detached limbs");
        }
      }
    }
  }
}

static void check_jax_second_fatality(void) {
  const CommandDef *command = jax_finish_command(BUTTON_BLOCK, 4, 0);
  check(command != NULL, "Jax second fatality needs its BLOCK command record");
  for (int slot = 0; slot < 2; ++slot) {
    for (int reset_at = 0; reset_at < 2; ++reset_at) {
      jax_enter_finish(slot, 4);
      Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
      jax_enter_tokens(slot, command, 0, false);
      advance_ticks(1, NULL);
      check(g_battle.phase == PHASE_FATALITY && fighter_move(owner)->clip == 284, "Jax B,F,F,D,BLK must start the head clap");
      bool clap = false, head = false, fall = false, loop = false;
      int limit = reset_at ? 60 : 600;
      for (int tick = 0; tick < limit && g_battle.phase == PHASE_FATALITY; ++tick) {
        advance_ticks(1, NULL);
        clap |= fighter_move(owner)->clip == 555;
        head |= (victim->hidden_joints[0] & (UINT64_C(1) << 2)) != 0;
        fall |= fighter_move(victim)->clip == 206;
        loop |= fighter_move(owner)->clip == 114;
      }
      if (!reset_at) {
        check(clap && head && fall && loop && g_battle.phase == PHASE_MATCH_OVER, "Jax head clap must crush the head, drop the body and end the scene");
        check(owner->wins == 2 && victim->health == 0, "Jax head clap must preserve the decided result");
      }
      battle_reset_match();
      check(victim->hidden_joints[0] == 0, "Jax head clap reset must restore the victim");
    }
  }
}

static void check_cage_second_fatality(void) {
  for (int slot = 0; slot < 2; ++slot) {
    for (int reset_at = 0; reset_at < 3; ++reset_at) {
      battle_init(slot ? 4 : 14, slot ? 14 : 4);
      g_battle.phase = PHASE_FIGHT;
      Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
      owner->wins = 1;
      battle_damage(victim, Q16_ONE);
      advance_ticks(1, NULL);
      for (int tick = 0; tick < 180 && g_battle.finish_waiting; ++tick) {
        advance_ticks(1, NULL);
      }
      check(battle_can_finish(owner), "Cage fatality must enter through the real deciding-KO window");
      owner->body.pos[AXIS_X] = slot ? 0x8ccc : -0x8ccc;
      victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
      check(!retail_can_finish_close(owner) && !retail_can_finish_close(victim), "Cage close fatality must reject distance above one and the defeated player");
      owner->body.pos[AXIS_X] = slot ? 0x6666 : -0x6666;
      victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
      const Controls *controls = roster_def(14)->controls;
      const CommandDef *selected = NULL;
      for (int i = 0; i < controls->command_count; ++i) {
        if (controls->commands[i].enter == retail_finish_second) {
          selected = &controls->commands[i];
        }
      }
      check(selected != NULL, "Cage second fatality must have its own generated retail command");
      for (int i = 0; i < selected->count; ++i) {
        Button button = selected->tokens[i] == TOKEN_DOWN ? BUTTON_DOWN : slot ? BUTTON_LEFT : BUTTON_RIGHT;
        Input input[2] = {0};
        input[slot].held = BUTTON_BIT(button);
        if (i == selected->count - 1) {
          input[slot].held |= BUTTON_BIT(selected->button);
        }
        battle_step(input);
        advance_ticks(2, NULL);
      }
      check(g_battle.phase == PHASE_FATALITY && fighter_move(owner)->clip == 293, "Cage second command must start its own uppercut scene");
      int time = g_battle.time_ticks;
      bool hit = false, fall = false, victory = false;
      int limit = reset_at == 1 ? 15 : reset_at == 2 ? 90 : 450;
      for (int tick = 0; tick < limit && g_battle.phase == PHASE_FATALITY; ++tick) {
        advance_ticks(1, NULL);
        hit |= fighter_move(victim)->clip == 130;
        fall |= fighter_move(victim)->clip == 206;
        victory |= fighter_move(owner)->clip == 111;
      }
      if (reset_at == 0) {
        check(hit && fall && victory && g_battle.phase == PHASE_MATCH_OVER, "Cage uppercut must reach its actual victim fall and victory before completing");
        check((victim->hidden_joints[0] & (UINT64_C(1) << 2)) && victim->health == 0 && owner->wins == 2 && g_battle.time_ticks == time,
            "Cage decapitation must preserve the decided result, severed head and frozen match clock");
      }
      battle_reset_match();
      check(owner->hidden_joints[0] == 0 && victim->hidden_joints[0] == 0, "Reset before or after Cage's fatality impact must restore both fighters");
      for (int piece = 0; piece < MAX_SCENE_PIECES; ++piece) {
        check(!g_battle.pieces[piece].active, "Cage fatality reset must remove detached scene pieces");
      }
      for (int object = 0; object < MAX_PROJECTILES; ++object) {
        check(g_battle.projectiles[object].kind == 0, "Cage fatality reset must remove charge effects");
      }
    }
  }
}

/* Shinnok: 455000 hand grab and 43c6d0 hand clap through their own records. */
static void shinnok_fatality_command(int slot, FighterFn entry) {
  const Controls *controls = roster_def(6)->controls;
  const CommandDef *selected = NULL;
  for (int i = 0; i < controls->command_count; ++i) {
    if (controls->commands[i].enter == entry) {
      selected = &controls->commands[i];
    }
  }
  check(selected != NULL && selected->count == 4, "Shinnok fatality must have its own generated retail command");
  for (int i = 0; i < selected->count; ++i) {
    CommandToken token = selected->tokens[i];
    Button button = token == TOKEN_DOWN ? BUTTON_DOWN : token == TOKEN_UP ? BUTTON_UP : (token == TOKEN_FORWARD) == (slot == 0) ? BUTTON_RIGHT : BUTTON_LEFT;
    Input input[2] = {0};
    input[slot].held = BUTTON_BIT(button);
    if (i == selected->count - 1) {
      input[slot].held |= BUTTON_BIT(selected->button);
    } else if (token == TOKEN_UP) {
      /* Block-held Up directions do not jump (CMD_BLOCK_OK record 4e4694). */
      input[slot].held |= BUTTON_BIT(BUTTON_BLOCK);
    }
    battle_step(input);
    advance_ticks(2, NULL);
  }
}

static int shinnok_scene_clip(int clip) {
  int count = 0;
  for (int i = 0; i < MAX_SCENE_ACTORS; ++i) {
    count += g_battle.scene_actors[i].active && fighter_move(&g_battle.scene_actors[i].actor)->clip == clip;
  }
  return count;
}

static void check_shinnok_fatalities(void) {
  for (int second = 0; second < 2; ++second) {
    for (int slot = 0; slot < 2; ++slot) {
      for (int reset_at = 0; reset_at < 2; ++reset_at) {
        battle_init(slot ? 4 : 6, slot ? 6 : 4);
        g_battle.phase = PHASE_FIGHT;
        Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
        owner->wins = 1;
        battle_damage(victim, Q16_ONE);
        advance_ticks(1, NULL);
        for (int tick = 0; tick < 180 && g_battle.finish_waiting; ++tick) {
          advance_ticks(1, NULL);
        }
        owner->body.pos[AXIS_X] = slot ? 0x8ccc : -0x8ccc;
        victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
        check(battle_can_finish(owner) && !retail_can_finish_close(owner), "Shinnok's 45e570 gate must reject distance above one unit");
        owner->body.pos[AXIS_X] = slot ? 0x6666 : -0x6666;
        victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
        shinnok_fatality_command(slot, second ? retail_finish_second : retail_finish_first);
        check(g_battle.phase == PHASE_FATALITY, "Shinnok fatality command must start its own scene");
        if (second) {
          check(owner->body.pos[AXIS_X] == 0 && owner->body.pos[AXIS_Z] == 0 && victim->body.pos[AXIS_X] == (slot ? -Q16_ONE : Q16_ONE),
              "43c6d0 must place the owner at the origin and the victim one unit to its own side");
        } else {
          check(fighter_distance(owner) > 1.39f && fighter_distance(owner) < 1.41f, "475530 must align the hand-grab pair 1.4 units apart");
        }
        bool hidden = false, hands = false, lifted = false, burst = false, crushed = false, landed = false;
        int time = g_battle.time_ticks;
        for (int tick = 0; tick < (reset_at ? 200 : 1000) && g_battle.phase == PHASE_FATALITY; ++tick) {
          advance_ticks(1, NULL);
          hidden |= owner->appearance == APPEARANCE_HIDDEN;
          hands |= shinnok_scene_clip(second ? 541 : 434) == (second ? 2 : 1);
          lifted |= victim->body.pos[AXIS_Y] < victim->body.ground - Q16_ONE / 2;
          burst |= second && victim->appearance == APPEARANCE_HIDDEN;
          crushed |= !second && fighter_move(victim)->clip == 435 && fighter_sample(victim) >= 139;
          landed |= hidden && owner->appearance == APPEARANCE_NORMAL && fighter_move(owner)->clip == 443;
        }
        if (reset_at) {
          check(hidden && hands, "Shinnok must vanish and raise his hand before the reset point");
        } else {
          check(hidden && hands && landed && g_battle.phase == PHASE_MATCH_OVER, "Shinnok must vanish, use the hands, return and finish with his victory loop");
          check(second ? lifted && burst : crushed, "Each fatality must reach its own victim crush");
          check(victim->health == 0 && owner->wins == 2 && g_battle.time_ticks == time, "The fatality must preserve the decided result and frozen clock");
        }
        battle_reset_match();
        check(owner->appearance == APPEARANCE_NORMAL && victim->appearance == APPEARANCE_NORMAL && shinnok_scene_clip(434) + shinnok_scene_clip(541) == 0,
            "Reset during or after a Shinnok fatality must restore both fighters and retire the hands");
      }
    }
  }
}

static void check_fatalities(void) {
  for (int player = 0; player < 2; ++player) {
    for (int id = FINISH_BURN; id <= FINISH_SPIKE; ++id) {
      finish_window(player, id == FINISH_BURN ? 2.4f : .8f, id == FINISH_FAN ? 8 : 0);
      int timer = g_battle.time_ticks;
      finish_command(player, id);
      check(g_battle.phase == PHASE_FATALITY && g_battle.finisher == id, "Fatality command must select its scene for either player");
      check(!battle_can_finish(&g_battle.fighters[player]), "Fatality cannot start twice");
      advance_ticks(160, NULL);
      if (id == FINISH_BEAST) {
        check(g_battle.fighters[player].appearance == APPEARANCE_BEAST, "Fatality II must switch to the creature asset");
        check(g_battle.fighters[1 - player].current_move->clip == 584, "Creature fatality must play its victim sequence");
      }
      advance_ticks(440, NULL);
      check(g_battle.phase == PHASE_MATCH_OVER && g_battle.fighters[player].wins == 2, "Fatality must complete without awarding another win");
      check(g_battle.fighters[1 - player].health == 0 && g_battle.time_ticks == timer, "Fatality must retain KO and frozen match timer");
      if (id == FINISH_FAN) {
        check(g_battle.fighters[1 - player].appearance == APPEARANCE_HIDDEN, "Fan must remove the finished victim");
      }
      if (id == FINISH_SPIKE) {
        check(g_battle.fighters[1 - player].body.pos[AXIS_Y] == -0x59999, "Spike victim must stay impaled until rematch");
      }
      int result_age = g_battle.phase_ticks;
      advance_ticks(360, NULL);
      check(g_battle.phase == PHASE_MATCH_OVER && g_battle.phase_ticks == result_age + 360 && g_battle.fighters[player].wins == 2,
          "Completed Scorpion scene must not restart completion or award another result");
      battle_reset_match();
      check(g_battle.finisher == 0 && g_battle.fighters[player].appearance == APPEARANCE_NORMAL, "Rematch must clear finisher state and restore model");
    }
  }
  for (int id = FINISH_BURN; id <= FINISH_SPIKE; ++id) {
    finish_window(0, id == FINISH_BURN ? .8f : 2.4f, 0);
    finish_command(0, id);
    check(g_battle.phase == PHASE_FINISH, "Out-of-range fatality must be rejected");
  }
  finish_window(0, .8f, 1);
  finish_command(0, FINISH_FAN);
  check(g_battle.phase == PHASE_FINISH, "Fan fatality must require Prison");
  advance_ticks(3, NULL);
  finish_command(0, FINISH_SPIKE);
  check(g_battle.phase == PHASE_FINISH, "Spike fatality must require Goro's Lair");
  finish_window(0, .8f, 0);
  finish_command(1, FINISH_BEAST);
  check(g_battle.phase == PHASE_FINISH, "Defeated player cannot enter a fatality");
}

static void check_script_reactions(void) {
  for (int player = 0; player < 2; ++player) {
    fight(4.f);
    Fighter *victim = &g_battle.fighters[player];
    victim->damage_total = 0x3333;
    battle_react(victim, 37);
    advance_ticks(1, NULL);
    check(victim->current_mmove != NULL && victim->sequence == NULL, "Shared stagger must own frame callbacks instead of an elapsed-tick sequence");
    advance_ticks(19, NULL);
    check(victim->state == S_HIT && victim->body.vel[AXIS_X] == 0 && victim->body.vel[AXIS_Z] == 0, "Stagger frame callback must stop sliding before recovery");
    battle_react(victim, 34);
    advance_ticks(160, NULL);
    check(victim->state == S_IDLE && !fighter_airborne(victim) && victim->current_mmove == NULL,
        "Replacement uppercut must land, get up and retire its frame program");
    fight(4.f);
    victim = &g_battle.fighters[player];
    g_battle.fighters[1 - player].wins = 1;
    battle_damage(victim, Q16_ONE);
    battle_react(victim, 49);
    advance_ticks(180, NULL);
    check(g_battle.phase == PHASE_FINISH && !g_battle.finish_waiting && victim->current_mmove == NULL,
        "Completed KO launcher must retire its frames and open the deciding finish window");
  }
  static const int ids[] = {3, 5, 8, 10, 12, 17, 20, 21, 22, 23, 25, 27, 28, 29, 32, 38, 39, 43, 48, 49, 50, 90, 99, 100, 143, 154, 155, 156, 173};
  for (size_t n = 0; n < sizeof ids / sizeof ids[0]; ++n) {
    for (int air = 0; air < 2; ++air) {
      if (air && (ids[n] < 10 || ids[n] == 143)) {
        continue;
      }
      fight(4.f);
      Fighter *victim = &g_battle.fighters[1];
      if (air) {
        victim->body.pos[AXIS_Y] -= Q16_ONE;
      }
      battle_react(victim, ids[n]);
      advance_ticks(1, NULL);
      check(victim->current_move != roster_def(4)->moves[S_HIT], "Scorpion records must not use generic hitstun");
      advance_ticks(250, NULL);
      if (victim->state != S_IDLE || fighter_airborne(victim)) {
        fprintf(stderr, "reaction %d air %d state %d height %f\n", ids[n], air, victim->state, fighter_height(victim));
      }
      check(victim->state == S_IDLE && !fighter_airborne(victim), "Reaction must finish on the floor and restore controls");
      check(!(victim->flags & (FF_REACTING | FF_NO_GUARD)), "Reaction must clear combo-only guard restrictions");
    }
  }
  fight(4.f);
  Fighter *victim = &g_battle.fighters[1];
  battle_react(victim, 43);
  advance_ticks(1, NULL);
  check(victim->action == 0x4002 && victim->damage_scale == 0x9999 && (victim->flags & FF_NO_GUARD), "Close punch must create its scaled combo reaction");
  advance_ticks(8, NULL);
  battle_react(victim, 49);
  advance_ticks(1, NULL);
  check(victim->current_move->clip == 131 && victim->body.vel[AXIS_Y] < 0, "Sword launcher must replace hitstun and launch");
  advance_ticks(200, NULL);
  check(victim->state == S_IDLE && victim->damage_scale == Q16_ONE, "New reaction must own recovery and reset combo scaling");
  fight(4.f);
  victim = &g_battle.fighters[1];
  victim->crouching = true;
  battle_react(victim, 28);
  advance_ticks(1, NULL);
  check(victim->current_move->clip == 254, "Crouching victim must use the crouch reaction branch");
  fight(4.f);
  victim = &g_battle.fighters[1];
  victim->body.pos[AXIS_Y] -= Q16_ONE;
  battle_react(victim, 90);
  advance_ticks(1, NULL);
  check(victim->current_move->clip == 183 && abs(victim->body.vel[AXIS_X]) == 0x7ae, "Air teleport victim must use its own retreat speed");
  fight(4.f);
  victim = &g_battle.fighters[1];
  battle_react(victim, 23);
  advance_ticks(5, NULL);
  check(fighter_sample(victim) == 6, "Diagonal kick reaction must advance its opening animation twice per tick");
  advance_ticks(4, NULL);
  check(fighter_sample(victim) == 14, "Diagonal kick reaction must switch to normal speed after its fast opening");
  advance_ticks(1, NULL);
  check(fighter_sample(victim) == 15, "Diagonal kick reaction must continue at normal speed");
}

static void start_ground_throw(int player, bool lk) {
  battle_init(0, 0);
  g_battle.phase = PHASE_FIGHT;
  g_battle.fighters[0].body.pos[AXIS_X] = -0x4ccc;
  g_battle.fighters[1].body.pos[AXIS_X] = 0x4ccc;
  fighter_press(&g_battle.fighters[player], lk ? BUTTON_LOW_KICK : BUTTON_LOW_PUNCH);
  advance_ticks(1, NULL);
}

static void check_ground_throw_lifetimes(void) {
  for (int player = 0; player < 2; ++player) {
    for (int lk = 0; lk < 2; ++lk) {
      start_ground_throw(player, lk);
      Fighter *owner = &g_battle.fighters[player], *victim = &g_battle.fighters[1 - player];
      check((owner->flags & FF_PAIRED) && (victim->flags & FF_PAIRED), "Native throw capture must acquire both contexts");
      for (int visit = 0; visit < 256 && (owner->state != S_IDLE || victim->state != S_IDLE); ++visit) {
        advance_ticks(1, NULL);
      }
      check(owner->state == S_IDLE && victim->state == S_IDLE && !((owner->flags | victim->flags) & FF_PAIRED), "Ground throws must release both controllers");

      start_ground_throw(player, lk);
      fighter_interrupt(&g_battle.fighters[lk ? player : 1 - player], fighter_ready);
      advance_ticks(180, NULL);
      check(g_battle.fighters[1 - player].health == Q16_ONE && !((g_battle.fighters[0].flags | g_battle.fighters[1].flags) & FF_PAIRED),
          "Interrupted native capture must cancel damage and retire both pair lifetimes");
    }
    start_ground_throw(player, true);
    g_battle.fighters[1 - player].health = 0x1000;
    advance_ticks(200, NULL);
    check(g_battle.fighters[1 - player].health == 0 && !((g_battle.fighters[0].flags | g_battle.fighters[1].flags) & FF_PAIRED),
        "Lethal native throw must detach both actors");
    battle_reset_match();
  }
}

static void check_air_capture_lifetimes(void) {
  for (int player = 0; player < 2; ++player) {
    battle_init(0, 0);
    g_battle.phase = PHASE_FIGHT;
    Fighter *owner = &g_battle.fighters[player], *victim = &g_battle.fighters[1 - player];
    owner->body.pos[AXIS_X] = 0;
    victim->body.pos[AXIS_X] = Q16_ONE;
    owner->body.pos[AXIS_Y] = victim->body.pos[AXIS_Y] = -3 * Q16_ONE;
    fighter_enter_state(owner, S_JUMP);
    fighter_enter_state(victim, S_JUMP);
    Input input[2] = {0};
    input[player].held = BUTTON_BIT(BUTTON_BLOCK);
    battle_step(input);
    advance_ticks(1, NULL);
    check((owner->flags & FF_PAIRED) && (victim->flags & FF_PAIRED), "Air capture must acquire both actor contexts");
    for (int visit = 0; visit < 256 && (owner->state != S_IDLE || victim->state != S_IDLE); ++visit) {
      advance_ticks(1, NULL);
    }
    check(owner->state == S_IDLE && victim->state == S_IDLE, "Both air throw participants must recover");
    check(!(owner->flags & FF_PAIRED) && !(victim->flags & FF_PAIRED), "Air throw must release body attachment");
  }
  fight(1.f);
  for (int i = 0; i < 2; ++i) {
    g_battle.fighters[i].body.pos[AXIS_Y] = -3 * Q16_ONE;
    fighter_enter_state(&g_battle.fighters[i], S_JUMP);
  }
  fighter_press(&g_battle.fighters[0], BUTTON_BLOCK);
  advance_ticks(1, NULL);
  battle_react(&g_battle.fighters[0], 11);
  advance_ticks(150, NULL);
  check(g_battle.fighters[1].state == S_IDLE && g_battle.fighters[1].health == Q16_ONE, "Interrupted throw must release victim without damage");
  fight(1.f);
  fighter_press(&g_battle.fighters[0], BUTTON_BLOCK);
  check(g_battle.fighters[0].pending == NULL, "Grounded BLOCK must not air throw");
}

/* Tanya (roster 9) native specials. */
static Projectile *tanya_fireball_object(void) {
  for (int i = 0; i < MAX_PROJECTILES; ++i) {
    Projectile *projectile = &g_battle.projectiles[i];
    if (projectile->kind == PROJECTILE_FIREBALL && projectile->mesh_animation != NULL && projectile->mesh_animation->first == 261) {
      return projectile;
    }
  }
  return NULL;
}

static void check_tanya_fireballs(void) {
  for (int slot = 0; slot < 2; ++slot) {
    Button forward = slot ? BUTTON_LEFT : BUTTON_RIGHT, back = slot ? BUTTON_RIGHT : BUTTON_LEFT;
    /* Ground D,F+HP: hit, chip, crouch (action 2001) and caster interruption. */
    for (int scenario = 0; scenario < 4; ++scenario) {
      battle_init(slot ? 4 : 9, slot ? 9 : 4);
      g_battle.phase = PHASE_FIGHT;
      Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
      owner->body.pos[AXIS_X] = slot ? 3 * Q16_ONE : -3 * Q16_ONE;
      victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
      command(slot, BUTTON_DOWN, forward, BUTTON_HIGH_PUNCH);
      check(fighter_move(owner)->clip == 26 && owner->action == 0x309, "Tanya D,F+HP must enter the native 309 clip-26 cast");
      Input input[2] = {0};
      if (scenario == 1) {
        input[1 - slot].held = BUTTON_BIT(BUTTON_BLOCK);
      } else if (scenario == 2) {
        input[1 - slot].held = BUTTON_BIT(BUTTON_DOWN);
      }
      bool launched = false;
      for (int tick = 0; tick < 180; ++tick) {
        battle_step(input);
        Projectile *projectile = tanya_fireball_object();
        if (!launched && projectile != NULL) {
          launched = true;
          check(projectile->vel[AXIS_Y] == 0 && projectile->gravity == 0 && projectile->mesh_animation->source == MESH_SHARED2,
              "Tanya's ground fireball must fly straight with its share2 meshes");
          if (scenario == 3) {
            battle_react(owner, 37);
          }
        }
      }
      q16 damage = scenario == 0 || scenario == 3 ? 0x1999 : scenario == 1 ? q16_mul(0x1999, 0x3333) : 0;
      check(launched && victim->health == Q16_ONE - damage, "Tanya fireball must keep C0 hit, chip and crouch-avoidance behavior after interruption");
      check(owner->state == S_IDLE && tanya_fireball_object() == NULL, "Tanya caster and her independent fireball must recover and retire");
    }
    /* Airborne D,B+LP: the descending object ignores action 2001 and the
     * caster falls on clip 310 before returning to stance. */
    for (int scenario = 0; scenario < 2; ++scenario) {
      battle_init(slot ? 4 : 9, slot ? 9 : 4);
      g_battle.phase = PHASE_FIGHT;
      Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
      owner->body.pos[AXIS_X] = slot ? Q16_ONE : -Q16_ONE;
      victim->body.pos[AXIS_X] = slot ? -0x18000 : 0x18000;
      command(slot, BUTTON_DOWN, back, BUTTON_LOW_PUNCH);
      check(owner->action != 0x30a, "Tanya's air fireball gate must reject a grounded caster");
      advance_ticks(60, NULL);
      Input input[2] = {0};
      input[slot].held = BUTTON_BIT(BUTTON_UP);
      battle_step(input);
      advance_ticks(8, NULL);
      check(fighter_airborne(owner) && owner->body.gravity != 0, "Tanya must be in a falling jump before the air fireball");
      command(slot, BUTTON_DOWN, back, BUTTON_LOW_PUNCH);
      check(fighter_move(owner)->clip == 24 && owner->action == 0x30a && owner->body.gravity == 0, "Tanya air D,B+LP must stop motion and enter clip 24");
      if (scenario == 1) {
        input[1 - slot].held = BUTTON_BIT(BUTTON_DOWN);
      }
      input[slot].held = 0;
      bool launched = false, fell = false;
      for (int tick = 0; tick < 180; ++tick) {
        battle_step(input);
        Projectile *projectile = tanya_fireball_object();
        launched |= projectile != NULL && projectile->vel[AXIS_Y] == 0x23d7;
        fell |= fighter_move(owner)->clip == 310 && owner->action == 0x1005 && owner->body.gravity == 0x20c;
      }
      check(launched && fell && !fighter_airborne(owner) && owner->state == S_IDLE, "Tanya air fireball caster must fall on clip 310, land and recover");
      check(victim->health < Q16_ONE && tanya_fireball_object() == NULL, "Tanya's descending fireball must strike even a crouching victim");
    }
    battle_init(slot ? 4 : 9, slot ? 9 : 4);
    g_battle.phase = PHASE_FIGHT;
    check(battle_cpu_action(&g_battle.fighters[slot], CPU_PROJECTILE), "Tanya CPU projectile row must be registered");
    advance_ticks(1, NULL);
    check(fighter_move(&g_battle.fighters[slot])->clip == 26 && g_battle.fighters[slot].action == 0x309, "Tanya CPU projectile row must cast her fireball");
  }
}

static void check_tanya_kicks(void) {
  for (int slot = 0; slot < 2; ++slot) {
    Button forward = slot ? BUTTON_LEFT : BUTTON_RIGHT, back = slot ? BUTTON_RIGHT : BUTTON_LEFT;
    /* F,F+LK 479520: hit, blocked 47d940 recoil, miss, then the 60-tick gate. */
    for (int scenario = 0; scenario < 3; ++scenario) {
      battle_init(slot ? 4 : 9, slot ? 9 : 4);
      g_battle.phase = PHASE_FIGHT;
      Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
      q16 half = scenario == 2 ? 5 * Q16_ONE : Q16_ONE;
      owner->body.pos[AXIS_X] = slot ? half : -half;
      victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
      Input input[2] = {0};
      input[1 - slot].held = scenario == 1 ? BUTTON_BIT(BUTTON_BLOCK) : 0;
      command(slot, forward, forward, BUTTON_LOW_KICK);
      check(fighter_move(owner)->clip == 109 && owner->action == 0x21b && owner->body.vel[AXIS_X] != 0, "Tanya F,F+LK must start the advancing clip-109 spin");
      bool recoiled = false, rewound = false;
      for (int tick = 0; tick < 150; ++tick) {
        battle_step(input);
        recoiled |= owner->action == 0x1010 && fighter_move(owner)->clip == 301;
        rewound |= owner->action == 0x21b && (owner->flags & FF_NO_GUARD) && owner->body.vel[AXIS_X] == 0;
      }
      q16 damage = scenario == 0 ? 0x1eb8 : scenario == 1 ? q16_mul(0x1eb8, 0x3333) : 0;
      check(victim->health == Q16_ONE - damage, "Tanya spin kick must apply D28 hit, chip or miss once");
      check(recoiled == (scenario == 1) && rewound == (scenario != 1), "Tanya spin kick must select the blocked recoil or its reverse recovery");
      check(owner->state == S_IDLE && !fighter_airborne(owner) && !(owner->flags & FF_NO_GUARD), "Tanya spin kick must return to stance");
      if (scenario == 2) {
        command(slot, forward, forward, BUTTON_LOW_KICK);
        check(owner->action == 0x21b, "Tanya spin kick must be available again after its 60-tick stamp");
        battle_init(slot ? 4 : 9, slot ? 9 : 4);
        g_battle.phase = PHASE_FIGHT;
        g_battle.fighters[1 - slot].hits = 5;
        command(slot, forward, forward, BUTTON_LOW_KICK);
        check(g_battle.fighters[slot].action != 0x21b, "Tanya spin kick gate must reject an opponent combo count above four");
      }
    }
    /* F,D,B+LK 47b9f0: grounded hit adds reaction 95, block chips, miss recovers. */
    for (int scenario = 0; scenario < 3; ++scenario) {
      battle_init(slot ? 4 : 9, slot ? 9 : 4);
      g_battle.phase = PHASE_FIGHT;
      Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
      q16 half = scenario == 2 ? 5 * Q16_ONE : Q16_ONE;
      owner->body.pos[AXIS_X] = slot ? half : -half;
      victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
      Input input[2] = {0};
      input[1 - slot].held = scenario == 1 ? BUTTON_BIT(BUTTON_BLOCK) : 0;
      Input step[2] = {0};
      const Button sequence[] = {forward, BUTTON_DOWN, back, BUTTON_LOW_KICK};
      for (int entry = 0; entry < 4; ++entry) {
        step[slot].held = BUTTON_BIT(sequence[entry]);
        battle_step(step);
        advance_ticks(entry < 3 ? 2 : 1, NULL);
      }
      check(fighter_move(owner)->clip == 103 && owner->action == 0x211, "Tanya F,D,B+LK must enter the clip-103 drill kick");
      bool followup = false;
      for (int tick = 0; tick < 150; ++tick) {
        battle_step(input);
        followup |= victim->reaction == 95;
      }
      q16 damage = scenario == 0 ? 0x2666 : scenario == 1 ? q16_mul(0x2666, 0x3333) : 0;
      check(victim->health == Q16_ONE - damage && followup == (scenario == 0), "Tanya drill kick must keep D17 damage and the grounded reaction-95 follow-up");
      check(owner->state == S_IDLE && fighter_move(victim)->clip != 103, "Tanya drill kick caster must finish clip 103 and recover");
    }
    battle_init(slot ? 4 : 9, slot ? 9 : 4);
    g_battle.phase = PHASE_FIGHT;
    check(battle_cpu_action(&g_battle.fighters[slot], CPU_SPECIAL_NEAR), "Tanya near CPU row must be registered");
    advance_ticks(1, NULL);
    check(g_battle.fighters[slot].action == 0x211, "Tanya CPU range rows must select the drill kick");
  }
}

static Projectile *tanya_weapon_object(void) {
  for (int i = 0; i < MAX_PROJECTILES; ++i) {
    if (g_battle.projectiles[i].kind == PROJECTILE_SWORD) {
      return &g_battle.projectiles[i];
    }
  }
  return NULL;
}

static void tanya_draw(int slot) {
  Fighter *owner = &g_battle.fighters[slot];
  Button forward = slot ? BUTTON_LEFT : BUTTON_RIGHT;
  Input input[2] = {0};
  input[slot].held = BUTTON_BIT(forward);
  battle_step(input);
  advance_ticks(2, NULL);
  battle_step(input);
  advance_ticks(1, NULL);
  input[slot].held = BUTTON_BIT(BUTTON_HIGH_KICK);
  battle_step(input);
  advance_ticks(1, NULL);
  check(fighter_move(owner)->clip == 244 && !owner->armed, "Tanya F,F+HK must play the real draw before equipping");
  advance_ticks(65, NULL);
  check(owner->armed && owner->weapon->type == 0x6d && owner->weapon->model == SCENE_MODEL_TANYA_WEAPON && owner->state == S_IDLE,
      "Tanya must equip her type-6d weapon rig and armed controls");
}

static void check_tanya_weapon(void) {
  for (int slot = 0; slot < 2; ++slot) {
    Button back = slot ? BUTTON_RIGHT : BUTTON_LEFT;
    /* Armed HP 4955b0 (WEAPON 42 -> 169) and held-back 4955e0 (WEAPON 43 -> 170). */
    for (int strike = 0; strike < 2; ++strike) {
      battle_init(slot ? 4 : 9, slot ? 9 : 4);
      g_battle.phase = PHASE_FIGHT;
      Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
      g_battle.fighters[0].body.pos[AXIS_X] = -4 * Q16_ONE;
      g_battle.fighters[1].body.pos[AXIS_X] = 4 * Q16_ONE;
      tanya_draw(slot);
      owner->body.pos[AXIS_X] = slot ? 0xc000 : -0xc000;
      victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
      Input input[2] = {0};
      input[slot].held = BUTTON_BIT(BUTTON_HIGH_PUNCH) | (strike ? BUTTON_BIT(back) : 0);
      advance_ticks(2, input);
      check(fighter_move(owner)->clip == (strike ? 231 : 229) && owner->action == 0x111, "Tanya armed HP must select its own native strike");
      input[slot].held = 0;
      bool reacted = false;
      for (int tick = 0; tick < 180; ++tick) {
        battle_step(input);
        reacted |= victim->reaction == (strike ? 170 : 169) && (victim->flags & FF_REACTING);
      }
      check(victim->health == Q16_ONE - (strike ? 0x23d7 : 0x28f5) && reacted, "Tanya armed strikes must keep WEAPON 42/43 damage and reactions");
      check(owner->state == S_IDLE && owner->armed && owner->body.vel[AXIS_X] == 0 && victim->state == S_IDLE, "Tanya armed strike and victim must recover");
    }
    /* Armed LP 495610: the boomerang hits or reaches the arena edge, returns
     * and is caught; a busy owner lets it settle as a free world weapon. */
    for (int scenario = 0; scenario < 3; ++scenario) {
      battle_init(slot ? 4 : 9, slot ? 9 : 4);
      g_battle.phase = PHASE_FIGHT;
      Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
      g_battle.fighters[0].body.pos[AXIS_X] = -4 * Q16_ONE;
      g_battle.fighters[1].body.pos[AXIS_X] = 4 * Q16_ONE;
      tanya_draw(slot);
      q16 half = scenario == 0 ? 2 * Q16_ONE + 0x8000 : 2 * Q16_ONE;
      owner->body.pos[AXIS_X] = slot ? half : -half;
      victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
      Input input[2] = {0};
      if (scenario != 0) {
        input[1 - slot].held = BUTTON_BIT(BUTTON_DOWN);
      }
      input[slot].held = BUTTON_BIT(BUTTON_LOW_PUNCH);
      battle_step(input);
      input[slot].held = 0;
      battle_step(input);
      check(fighter_move(owner)->clip == 229 && owner->action == 0x316, "Tanya armed LP must start the action-316 boomerang throw");
      bool released = false, settled = false;
      for (int tick = 0; tick < 400; ++tick) {
        battle_step(input);
        Projectile *object = tanya_weapon_object();
        released |= object != NULL && !owner->armed;
        if (scenario == 2 && object != NULL && object->phase == 2 && object->step != NULL && owner->state == S_IDLE) {
          settled = true;
        }
        if (scenario == 2 && released && !settled && object != NULL) {
          owner->flags |= FF_BUSY;
        }
      }
      q16 damage = scenario == 0 ? 0x3333 : 0;
      check(released && victim->health == Q16_ONE - damage, "Tanya's boomerang must apply C20 once and miss a crouching victim");
      if (scenario < 2) {
        check(owner->armed && owner->state == S_IDLE && tanya_weapon_object() == NULL, "Tanya must catch her returning boomerang");
      } else {
        owner->flags &= ~(unsigned)FF_BUSY;
        check(
            !owner->armed && tanya_weapon_object() != NULL && tanya_weapon_object()->phase == 2, "A busy owner must leave the boomerang settled in the arena");
      }
    }
  }
}

/* Enter Tanya's deciding-KO window and type a generated finisher record. */
static void tanya_finisher_command(int slot, FighterFn entry, unsigned chord) {
  Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
  owner->wins = 1;
  battle_damage(victim, Q16_ONE);
  advance_ticks(1, NULL);
  for (int tick = 0; tick < 180 && g_battle.finish_waiting; ++tick) {
    advance_ticks(1, NULL);
  }
  check(battle_can_finish(owner), "Tanya fatality must enter through the real deciding-KO window");
  owner->body.pos[AXIS_X] = slot ? 0x8ccc : -0x8ccc;
  victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
  check(!retail_can_finish_close(owner) && !retail_can_finish_close(victim), "Tanya close fatality must reject distance above one and the defeated player");
  owner->body.pos[AXIS_X] = slot ? 0x6666 : -0x6666;
  victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
  if (chord != 0) {
    /* Pressing the held chord first may start an ordinary attack; type the
     * pattern once that has recovered, still holding the chord. */
    Input held[2] = {0};
    held[slot].held = chord;
    owner->body.pos[AXIS_X] = slot ? 0x30000 : -0x30000;
    victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
    for (int tick = 0; tick < 60; ++tick) {
      battle_step(held);
    }
    owner->body.pos[AXIS_X] = slot ? 0x6666 : -0x6666;
    victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
  }
  const Controls *controls = roster_def(9)->controls;
  const CommandDef *selected = NULL;
  for (int i = 0; i < controls->command_count; ++i) {
    if (controls->commands[i].enter == entry) {
      selected = &controls->commands[i];
    }
  }
  check(selected != NULL, "Tanya fatality must have its own generated retail command");
  for (int i = 0; i < selected->count; ++i) {
    CommandToken token = selected->tokens[i];
    Button button = token == TOKEN_DOWN ? BUTTON_DOWN : token == TOKEN_UP ? BUTTON_UP : slot ? BUTTON_LEFT : BUTTON_RIGHT;
    Input input[2] = {0};
    /* With a chord, block is held through the pattern so Up cannot jump,
     * then released for the final block press. */
    input[slot].held = BUTTON_BIT(button) | chord | (chord != 0 && i < selected->count - 1 ? BUTTON_BIT(BUTTON_BLOCK) : 0);
    if (i == selected->count - 1) {
      battle_step(input);
      battle_step(input);
      input[slot].held |= BUTTON_BIT(selected->button);
      battle_step(input);
    }
    battle_step(input);
    Input gap[2] = {0};
    gap[slot].held = chord | (chord != 0 ? BUTTON_BIT(BUTTON_BLOCK) : 0);
    battle_step(gap);
    battle_step(gap);
  }
}

static void check_tanya_second_fatality(void) {
  for (int slot = 0; slot < 2; ++slot) {
    for (int reset_at = 0; reset_at < 2; ++reset_at) {
      battle_init(slot ? 2 : 9, slot ? 9 : 2);
      g_battle.phase = PHASE_FIGHT;
      Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
      tanya_finisher_command(slot, retail_finish_second, 0);
      check(g_battle.phase == PHASE_FATALITY && fighter_move(owner)->clip == 439, "Tanya HK D,F,D,F must start her second fatality from the roster stance");
      bool owner_scene = false, victim_scene = false, collapse = false, victory = false;
      int limit = reset_at ? 100 : 800;
      for (int tick = 0; tick < limit && g_battle.phase == PHASE_FATALITY; ++tick) {
        advance_ticks(1, NULL);
        owner_scene |= fighter_move(owner)->clip == 565;
        victim_scene |= fighter_move(victim)->clip == 568 && victim->body.pos[AXIS_X] == owner->body.pos[AXIS_X];
        collapse |= fighter_move(victim)->clip == 569;
        victory |= fighter_move(owner)->clip == 124;
      }
      if (reset_at == 0) {
        check(owner_scene && victim_scene && collapse && victory && g_battle.phase == PHASE_MATCH_OVER,
            "Tanya second fatality must run the paired 565/568 cycles, the 569 collapse and victory");
        check(victim->health == 0 && owner->wins == 2, "Tanya second fatality must preserve the decided result");
      }
      battle_reset_match();
      check(g_battle.phase != PHASE_FATALITY, "Reset during Tanya's fatality must restore both fighters");
    }
  }
}

static void check_tanya_first_fatality(void) {
  for (int slot = 0; slot < 2; ++slot) {
    battle_init(slot ? 0 : 9, slot ? 9 : 0);
    g_battle.phase = PHASE_FIGHT;
    Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
    tanya_finisher_command(slot, retail_finish_first, 0);
    check(g_battle.phase != PHASE_FATALITY, "Tanya's first fatality must require HP held with BLOCK");
    battle_init(slot ? 0 : 9, slot ? 9 : 0);
    g_battle.phase = PHASE_FIGHT;
    tanya_finisher_command(slot, retail_finish_first, BUTTON_BIT(BUTTON_HIGH_PUNCH));
    check(g_battle.phase == PHASE_FATALITY, "Tanya HP+BLK D,D,U,D must start her first fatality");
    bool kiss = false, swell = false, shake = false, burst = false;
    for (int tick = 0; tick < 900 && g_battle.phase == PHASE_FATALITY; ++tick) {
      advance_ticks(1, NULL);
      kiss |= fighter_move(owner)->clip == 396;
      swell |= fighter_move(victim)->clip == 397 && victim->body.pos[AXIS_X] == owner->body.pos[AXIS_X];
      shake |= fighter_move(victim)->clip == 409;
      burst |= (victim->hidden_joints[0] & (UINT64_C(1) << 2)) != 0;
    }
    check(kiss && swell && shake && burst && g_battle.phase == PHASE_MATCH_OVER && owner->wins == 2,
        "Tanya first fatality must reach the kiss, swelling victim, shaking, burst and completion");
    battle_reset_match();
    check(victim->hidden_joints[0] == 0, "Reset after Tanya's first fatality must restore both fighters");
  }
}

static void check_tanya_high_kick(void) {
  for (int slot = 0; slot < 2; ++slot) {
    battle_init(slot ? 4 : 9, slot ? 9 : 4);
    g_battle.phase = PHASE_FIGHT;
    Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
    owner->body.pos[AXIS_X] = slot ? 0x9999 : -0x9999;
    victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
    Input input[2] = {0};
    input[slot].held = BUTTON_BIT(BUTTON_HIGH_KICK);
    battle_step(input);
    for (int tick = 0; tick < 90; ++tick) {
      battle_step((Input[2]){0});
    }
    check(victim->health == Q16_ONE - 0x2147 && owner->state == S_IDLE, "Tanya high kick must query B[1] once at pose 17 and recover");
  }
}

static void check_tanya_ports(void) {
  check_tanya_high_kick();
  check_tanya_first_fatality();
  check_tanya_fireballs();
  check_tanya_kicks();
  check_tanya_weapon();
  check_tanya_second_fatality();
}

/* Quan Chi checks: retail command and CPU records for roster slot 8. */

static Projectile *quan_chi_skull_object(void) {
  for (int i = 0; i < MAX_PROJECTILES; ++i) {
    Projectile *projectile = &g_battle.projectiles[i];
    if (projectile->kind == PROJECTILE_FIREBALL && projectile->mesh_animation != NULL && projectile->mesh_animation->first == 4 &&
        projectile->mesh_animation->source == MESH_SHARED2) {
      return projectile;
    }
  }
  return NULL;
}

static void check_quan_chi_skull(void) {
  for (int slot = 0; slot < 2; ++slot) {
    for (int scenario = 0; scenario < 6; ++scenario) {
      battle_init(slot ? 4 : 8, slot ? 8 : 4);
      g_battle.phase = PHASE_FIGHT;
      Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
      owner->body.pos[AXIS_X] = slot ? 3 * Q16_ONE : -3 * Q16_ONE;
      if (scenario == 5) {
        victim->health = 1;
        owner->wins = 1;
      }
      victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
      Button forward = slot ? BUTTON_LEFT : BUTTON_RIGHT;
      command(slot, forward, forward, BUTTON_LOW_PUNCH);
      check(fighter_move(owner)->clip == 23 && owner->action == 0x306, "Quan Chi LP [F,F] must enter native 499740 skull startup");
      Input input[2] = {0};
      if (scenario == 1) {
        input[1 - slot].held = BUTTON_BIT(BUTTON_BLOCK);
      } else if (scenario == 2) {
        input[1 - slot].held = BUTTON_BIT(BUTTON_DOWN);
      } else if (scenario == 3) {
        battle_react(owner, 37);
      }
      bool launched = false, bitten = false;
      for (int tick = 0; tick < 240; ++tick) {
        battle_step(input);
        Projectile *skull = quan_chi_skull_object();
        if (!launched && skull != NULL) {
          launched = true;
          check(skull->vel[AXIS_Y] == 0 && skull->gravity == 0, "The skull must fly level at its 0.2-unit heading speed");
          if (scenario == 4) {
            battle_react(owner, 37);
          }
        }
        bitten = bitten || (skull != NULL && skull->phase != 0);
        bitten = bitten || fighter_move(victim)->clip == 182;
      }
      q16 damage = scenario == 0 || scenario == 4 ? 0x1999 : scenario == 1 ? q16_mul(0x1999, 0x3333) : 0;
      if (scenario == 5 ? victim->health > 0 : victim->health != Q16_ONE - damage) {
        fprintf(stderr, "Quan Chi skull slot%d scenario%d health%d launched%d bitten%d\n", slot, scenario, victim->health, launched, bitten);
      }
      check(launched == (scenario != 3), "Only an uninterrupted startup may release the skull");
      check(scenario == 5 ? victim->health <= 0 : victim->health == Q16_ONE - damage,
          "Skull must deal C[7] damage once, chip on block, and pass over a crouching victim");
      check(bitten == (scenario != 2 && scenario != 3), "Hit or blocked skull must bite; misses must continue to the arena bound");
      if (scenario == 5) {
        check(victim->current_mmove == NULL && quan_chi_skull_object() == NULL, "A lethal skull reaction must retire without standing the defeated victim");
        continue;
      }
      advance_ticks(30, NULL);
      check(owner->state == S_IDLE && victim->state == S_IDLE && quan_chi_skull_object() == NULL, "Quan Chi skull caster, victim and object must all retire");
    }
    battle_init(slot ? 4 : 8, slot ? 8 : 4);
    g_battle.phase = PHASE_FIGHT;
    check(battle_cpu_action(&g_battle.fighters[slot], CPU_PROJECTILE), "Quan Chi CPU projectile row must be registered");
    advance_ticks(1, NULL);
    check(fighter_move(&g_battle.fighters[slot])->clip == 23 && g_battle.fighters[slot].action == 0x306, "Quan Chi CPU projectile row must throw the skull");
  }
}

static void check_quan_chi_flip_kick(void) {
  for (int slot = 0; slot < 2; ++slot) {
    for (int scenario = 0; scenario < 4; ++scenario) {
      battle_init(slot ? 4 : 8, slot ? 8 : 4);
      g_battle.phase = PHASE_FIGHT;
      Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
      q16 half = scenario == 2 ? 6 * Q16_ONE : Q16_ONE;
      owner->body.pos[AXIS_X] = slot ? half : -half;
      victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
      Button forward = slot ? BUTTON_LEFT : BUTTON_RIGHT;
      command(slot, forward, forward, BUTTON_HIGH_KICK);
      check(fighter_move(owner)->clip == 105 && owner->action == 0x217, "Quan Chi HK [F,F] must enter native 47a370 flip kick");
      Input input[2] = {0};
      if (scenario == 1) {
        input[1 - slot].held = BUTTON_BIT(BUTTON_BLOCK);
      }
      bool launched = false;
      for (int tick = 0; tick < 200; ++tick) {
        if (scenario == 3 && tick == 5) {
          battle_react(owner, 37);
        }
        battle_step(input);
        launched = launched || fighter_move(victim)->clip == 189;
      }
      advance_ticks(60, NULL);
      q16 damage = scenario == 0 ? 0x2666 : scenario == 1 ? q16_mul(0x2666, 0x3333) : 0;
      if (victim->health != Q16_ONE - damage || owner->state != S_IDLE || victim->state != S_IDLE) {
        fprintf(stderr, "Quan Chi flip slot%d scenario%d health%d owner%d victim%d\n", slot, scenario, victim->health, owner->state, victim->state);
      }
      check(victim->health == Q16_ONE - damage && launched == (scenario == 0),
          "Flip kick must deal D[24] once with reaction 160, chip on block and nothing on a miss");
      check(owner->state == S_IDLE && victim->state == S_IDLE && !(owner->flags & FF_NO_GUARD) && owner->body.physics == NULL,
          "Flip kick owner and victim must recover and clear the native decay and no-guard state");
    }
    battle_init(slot ? 4 : 8, slot ? 8 : 4);
    g_battle.phase = PHASE_FIGHT;
    check(battle_cpu_action(&g_battle.fighters[slot], CPU_SPECIAL_NEAR), "Quan Chi near CPU row must be registered");
    advance_ticks(1, NULL);
    check(fighter_move(&g_battle.fighters[slot])->clip == 105, "Quan Chi near CPU row must select the flip kick");
  }
}

static void check_quan_chi_teleport_stomp(void) {
  for (int slot = 0; slot < 2; ++slot) {
    for (int scenario = 0; scenario < 4; ++scenario) {
      battle_init(slot ? 4 : 8, slot ? 8 : 4);
      g_battle.phase = PHASE_FIGHT;
      Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
      owner->body.pos[AXIS_X] = slot ? 2 * Q16_ONE : -2 * Q16_ONE;
      victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
      Button forward = slot ? BUTTON_LEFT : BUTTON_RIGHT;
      command(slot, forward, BUTTON_DOWN, BUTTON_LOW_KICK);
      check(fighter_move(owner)->clip == 87 && owner->action == 0x400, "Quan Chi LK [F,D] must enter native 47cf30 teleport stomp");
      Input input[2] = {0};
      if (scenario == 1) {
        input[1 - slot].held = BUTTON_BIT(BUTTON_BLOCK);
      }
      bool relocated = false, stomped = false;
      int stomp_ticks = 0;
      q16 target = victim->body.pos[AXIS_X];
      for (int tick = 0; tick < 300; ++tick) {
        if (scenario == 2 && tick == 17) {
          victim->body.pos[AXIS_X] = -target; /* leave the saved landing point */
        }
        if (scenario == 3 && tick == 5) {
          battle_react(owner, 37);
        }
        battle_step(input);
        relocated = relocated || (owner->body.pos[AXIS_Y] < -0x30000 && abs(owner->body.pos[AXIS_X] - target) < 0x1000);
        stomped = stomped || fighter_move(owner)->clip == 89;
        stomp_ticks += fighter_move(owner)->clip == 89;
      }
      advance_ticks(60, NULL);
      q16 damage = scenario == 0 ? 0x3851 : scenario == 1 ? q16_mul(0x3851, 0x3333) : 0;
      if (victim->health != Q16_ONE - damage || owner->state != S_IDLE || victim->state != S_IDLE || relocated != (scenario != 3)) {
        fprintf(stderr, "Quan Chi teleport slot%d scenario%d health%d owner%d/%d victim%d relocated%d stomped%d y%d\n", slot, scenario, victim->health,
            owner->state, fighter_move(owner)->clip, victim->state, relocated, stomped, owner->body.pos[AXIS_Y]);
      }
      check(relocated == (scenario != 3), "The teleport must reappear above the opponent position saved during the sink");
      check(victim->health == Q16_ONE - damage && stomped == (scenario == 0), "Teleport stomp must deal D[10] once and stomp only after a hit");
      check(scenario != 0 || stomp_ticks == 3 * 14, "A connected teleport stomp must repeat clip 89 three times");
      check(owner->state == S_IDLE && victim->state == S_IDLE && owner->body.pos[AXIS_Y] == owner->body.ground && owner->body.physics == NULL,
          "Teleport stomp owner and victim must recover on the floor");
    }
    for (int row = 0; row < 2; ++row) {
      battle_init(slot ? 4 : 8, slot ? 8 : 4);
      g_battle.phase = PHASE_FIGHT;
      check(battle_cpu_action(&g_battle.fighters[slot], row ? CPU_SPECIAL_2 : CPU_SPECIAL_1), "Quan Chi medium and far CPU rows must be registered");
      advance_ticks(1, NULL);
      check(fighter_move(&g_battle.fighters[slot])->clip == 87, "Quan Chi medium and far CPU rows must select the teleport stomp");
    }
  }
}

static void check_quan_chi_weapon_steal(void) {
  for (int slot = 0; slot < 2; ++slot) {
    for (int scenario = 0; scenario < 4; ++scenario) {
      battle_init(slot ? 14 : 8, slot ? 8 : 14);
      g_battle.phase = PHASE_FIGHT;
      Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
      owner->body.pos[AXIS_X] = slot ? 3 * Q16_ONE / 2 : -3 * Q16_ONE / 2;
      victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
      if (scenario != 1) {
        fighter_interrupt(victim, cage_weapon_action);
        advance_ticks(70, NULL);
        check(victim->armed && victim->weapon->type == 0x6e, "Weapon steal scenarios need Cage's drawn mace");
      }
      Button forward = slot ? BUTTON_LEFT : BUTTON_RIGHT, back = slot ? BUTTON_RIGHT : BUTTON_LEFT;
      command(slot, forward, back, BUTTON_HIGH_PUNCH);
      check(fighter_move(owner)->clip == 7 && owner->action == 0x401, "Quan Chi HP [F,B] must enter native 469c80 weapon steal");
      Input input[2] = {0};
      if (scenario == 2) {
        input[1 - slot].held = BUTTON_BIT(BUTTON_BLOCK);
      }
      for (int tick = 0; tick < 150; ++tick) {
        if (scenario == 3 && tick == 14) {
          battle_react(owner, 37);
        }
        battle_step(input);
      }
      advance_ticks(60, NULL);
      bool stolen = scenario == 0;
      q16 health = scenario == 0 || scenario == 3 ? Q16_ONE - 0x7ae : scenario == 2 ? Q16_ONE - q16_mul(0x7ae, 0x3333) : Q16_ONE;
      if (owner->armed != stolen || victim->health != health) {
        fprintf(stderr, "Quan Chi steal slot%d scenario%d owner armed%d victim armed%d health%d state%d/%d\n", slot, scenario, owner->armed, victim->armed,
            victim->health, owner->state, victim->state);
      }
      check(owner->armed == stolen && (!stolen || owner->weapon->type == 0x6e) && victim->armed == (scenario == 2),
          "Weapon steal must take the victim's own weapon only after an unblocked D[11] contact");
      check(victim->health == health, "Weapon steal must deal D[11] once, chip on block and skip an unarmed victim");
      check(owner->state == S_IDLE && victim->state == S_IDLE, "Weapon steal owner and victim must recover");
      bool dropped = false;
      for (int i = 0; i < MAX_PROJECTILES; ++i) {
        dropped = dropped || (g_battle.projectiles[i].kind == PROJECTILE_SWORD && g_battle.projectiles[i].phase == 2);
      }
      check(dropped == (scenario == 3), "An interrupted steal must leave the weapon on the floor for a normal pickup");
    }
  }
}

static void check_quan_chi_weapon(void) {
  const int clips[] = {238, 231, 234, 237};
  const q16 damage[] = {0x2666, 0x2e14, 0x2666, 0x2666};
  for (int slot = 0; slot < 2; ++slot) {
    for (int strike = 0; strike < 4; ++strike) {
      battle_init(slot ? 4 : 8, slot ? 8 : 4);
      g_battle.phase = PHASE_FIGHT;
      Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
      owner->body.pos[AXIS_X] = slot ? 4 * Q16_ONE : -4 * Q16_ONE;
      victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
      Button back = slot ? BUTTON_RIGHT : BUTTON_LEFT;
      command(slot, BUTTON_DOWN, back, BUTTON_HIGH_KICK);
      check(fighter_move(owner)->clip == 244 && !owner->armed, "Quan Chi HK [D,B] must play the shared draw before equipping");
      advance_ticks(65, NULL);
      check(owner->armed && owner->weapon->type == 0x6f && owner->weapon->model == SCENE_MODEL_QUAN_CHI_WEAPON && owner->state == S_IDLE,
          "Quan Chi must equip his own type-6f weapon rig");
      q16 half = strike == 0 || strike == 3 ? Q16_ONE : 0xa666;
      owner->body.pos[AXIS_X] = slot ? half : -half;
      victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
      Input input[2] = {0};
      input[slot].held = BUTTON_BIT(strike < 2 ? BUTTON_HIGH_PUNCH : BUTTON_LOW_PUNCH);
      if (strike == 1 || strike == 3) {
        input[slot].held |= BUTTON_BIT(back);
      }
      advance_ticks(2, input);
      check(fighter_move(owner)->clip == clips[strike], "Quan Chi armed punches must select table 4ecc48's own strikes");
      input[slot].held = 0;
      advance_ticks(150, input);
      if (victim->health != Q16_ONE - damage[strike]) {
        fprintf(stderr, "Quan Chi weapon slot%d strike%d health%d\n", slot, strike, victim->health);
      }
      check(victim->health == Q16_ONE - damage[strike], "Quan Chi weapon strikes must use their own weapon records");
      check(owner->state == S_IDLE && owner->armed, "Quan Chi weapon strikes must recover armed on frame tables");
    }
    battle_init(slot ? 4 : 8, slot ? 8 : 4);
    g_battle.phase = PHASE_FIGHT;
    Fighter *owner = &g_battle.fighters[slot];
    owner->body.pos[AXIS_X] = slot ? 4 * Q16_ONE : -4 * Q16_ONE;
    g_battle.fighters[1 - slot].body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
    fighter_interrupt(owner, quan_chi_weapon_action);
    advance_ticks(70, NULL);
    check(owner->armed, "Quan Chi draw entry must equip");
    fighter_interrupt(owner, quan_chi_weapon_action);
    advance_ticks(90, NULL);
    check(!owner->armed && owner->state == S_IDLE, "An armed Quan Chi weapon command must throw the weapon");
  }
}

static void quan_chi_finish_window(int slot, int victim_roster, q16 half_distance) {
  battle_init(slot ? victim_roster : 8, slot ? 8 : victim_roster);
  g_battle.phase = PHASE_FIGHT;
  Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
  owner->wins = 1;
  battle_damage(victim, Q16_ONE);
  advance_ticks(1, NULL);
  for (int tick = 0; tick < 180 && g_battle.finish_waiting; ++tick) {
    advance_ticks(1, NULL);
  }
  check(battle_can_finish(owner), "Quan Chi fatalities need the deciding-KO window");
  owner->body.pos[AXIS_X] = slot ? half_distance : -half_distance;
  victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
}

static void check_quan_chi_first_fatality(void) {
  for (int slot = 0; slot < 2; ++slot) {
    for (int held = 0; held < 2; ++held) {
      /* Hold LK out of kick range, then step into the close gate. */
      quan_chi_finish_window(slot, 4, 0x20000);
      Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
      Input input[2] = {0};
      input[slot].held = BUTTON_BIT(BUTTON_LOW_KICK);
      advance_ticks(held ? 0xf0 : 0x80, input);
      owner->body.pos[AXIS_X] = slot ? 0x6666 : -0x6666;
      victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
      Button forward = slot ? BUTTON_LEFT : BUTTON_RIGHT;
      const Button sequence[] = {forward, BUTTON_DOWN, forward};
      for (int i = 0; i < 3; ++i) {
        input[slot].held = BUTTON_BIT(BUTTON_LOW_KICK) | BUTTON_BIT(sequence[i]);
        battle_step(input);
        input[slot].held = BUTTON_BIT(BUTTON_LOW_KICK);
        advance_ticks(2, input);
      }
      advance_ticks(2, NULL);
      check((g_battle.phase == PHASE_FATALITY) == (held == 1), "Quan Chi's leg fatality needs LK held for 0xf0 ticks before F,D,F and release");
      if (!held) {
        continue;
      }
      check(g_battle.finisher == 31, "LK-release [F,D,F] must enter 44bcd0 rather than a stage finish");
      bool owner_clip = false, torn = false, beaten = false;
      int ended = -1;
      for (int tick = 0; tick < 420; ++tick) {
        advance_ticks(1, NULL);
        ended = ended < 0 && g_battle.phase == PHASE_MATCH_OVER ? tick : ended;
        owner_clip |= fighter_move(owner)->clip == 420;
        beaten |= fighter_move(owner)->clip == 421;
        for (int i = 0; i < MAX_SCENE_ACTORS; ++i) {
          torn |= g_battle.scene_actors[i].active && fighter_move(&g_battle.scene_actors[i].actor)->clip == 422;
        }
      }
      check(owner_clip && torn && beaten && (victim->hidden_joints[0] & (UINT64_C(1) << 12)),
          "The leg must be torn off into its own 422 actor and used for the 421 beating");
      check(ended >= 250 && ended < 290 && g_battle.phase == PHASE_MATCH_OVER && owner->wins == 2 && victim->health == 0,
          "The leg fatality result follows the 44c700 camera program while the beating continues");
      battle_reset_match();
      check(victim->hidden_joints[0] == 0 && owner->hidden_joints[0] == 0, "Reset must restore the torn leg");
    }
  }
}

static void check_quan_chi_second_fatality(void) {
  for (int slot = 0; slot < 2; ++slot) {
    for (int against = 0; against < 2; ++against) {
      int roster = against ? 8 : 14;
      quan_chi_finish_window(slot, roster, 0x10000);
      Fighter *owner = &g_battle.fighters[slot];
      check(retail_can_finish_range(owner), "Quan Chi second fatality must accept its 1.8..3 range");
      owner->body.pos[AXIS_X] = slot ? 0x6666 : -0x6666;
      g_battle.fighters[1 - slot].body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
      check(!retail_can_finish_range(owner), "Quan Chi second fatality must reject close range");
      fighter_interrupt(owner, retail_finish_second);
      advance_ticks(2, NULL);
      check(g_battle.phase == PHASE_FATALITY && g_battle.finisher == (against ? 31 : 18),
          "43be30 must perform the victim roster's own second fatality, or the leg fatality against Quan Chi");
      battle_reset_match();
    }
  }
}

/* Reiko (slot 7) and Noob Saibot (slot 17) share Reiko's retail script slot
 * and CPU rows. These checks exercise their own command and CPU entries. */
static void reiko_noob_fight(int owner_roster, int victim_roster, int slot, float distance) {
  battle_init(slot ? victim_roster : owner_roster, slot ? owner_roster : victim_roster);
  g_battle.phase = PHASE_FIGHT;
  Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
  owner->body.pos[AXIS_X] = (q16)((slot ? 0.5f : -0.5f) * distance * Q16_ONE);
  victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
}

static void check_reiko_noob_high_kick(void) {
  const int roster[] = {7, 17};
  for (size_t index = 0; index < sizeof roster / sizeof roster[0]; ++index) {
    for (int scenario = 0; scenario < 3; ++scenario) {
      reiko_noob_fight(roster[index], 4, 0, scenario == 2 ? 5.f : 1.f);
      Fighter *owner = &g_battle.fighters[0], *victim = &g_battle.fighters[1];
      Input input[2] = {0};
      if (scenario == 1) {
        input[1].held = BUTTON_BIT(BUTTON_BLOCK);
      }
      fighter_press(owner, BUTTON_HIGH_KICK);
      bool recovered = false;
      for (int tick = 0; tick < 60 && !recovered; ++tick) {
        advance_ticks(1, input);
        recovered = tick > 2 && owner->state == S_IDLE;
      }
      /* 4e5d30: B[1] 13% hit, 20% chip on block, latched once. */
      q16 damage = scenario == 0 ? 0x2147 : scenario == 1 ? q16_mul(0x2147, 0x3333) : 0;
      check(recovered && victim->health == Q16_ONE - damage, "Reiko/Noob high kick must latch one B[1] contact and recover");
    }
  }
}

/* Noob BLOCK record 4e4b08 -> 46aba0: the pair falls together, 46aca0
 * deals 19% on landing and 46acc0 slams the peer with reaction 105. */

/* Up record 4e4e20 (D,U) -> 47c070 for both fighters. Without BLOCK the
 * body rises off the top, reappears below the floor one unit past the peer
 * and emerges; BLOCK during the rise queries D[14] for the slam or the
 * 47fec0 recoil. */
static void check_reiko_noob_teleport(void) {
  const int roster[] = {7, 17};
  for (size_t index = 0; index < 2; ++index) {
    for (int slot = 0; slot < 2; ++slot) {
      for (int scenario = 0; scenario < 3; ++scenario) {
        reiko_noob_fight(roster[index], 4, slot, 2.f);
        Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
        owner->human = true;
        Input input[2] = {0};
        if (scenario == 2) {
          input[1 - slot].held = BUTTON_BIT(BUTTON_BLOCK);
        }
        q16 side = owner->body.pos[AXIS_X] - victim->body.pos[AXIS_X];
        fighter_press(owner, BUTTON_DOWN);
        fighter_press(owner, BUTTON_UP);
        bool high = false, under = false, pressed = false, slammed = false, recoiled = false;
        int tick = 0;
        for (; tick < 300; ++tick) {
          advance_ticks(1, input);
          high |= owner->body.pos[AXIS_Y] <= -0x40000;
          under |= high && owner->body.pos[AXIS_Y] > owner->body.ground;
          if (scenario != 0 && !pressed && under && owner->body.gravity == 0x51e && owner->body.vel[AXIS_Y] < 0) {
            fighter_press(owner, BUTTON_BLOCK);
            pressed = true;
          }
          slammed |= victim->reaction == 105;
          recoiled |= owner->action == ACTION_RETAIL_1011;
          if (tick > 40 && owner->state == S_IDLE && victim->state != S_KNOCKDOWN && !fighter_airborne(owner) && owner->current_mmove == NULL) {
            break;
          }
        }
        check(high && under, "Teleport must vanish four units up and reappear below the floor");
        check(tick < 300 && owner->state == S_IDLE, "Teleport must land and return to stance");
        if (scenario == 0) {
          check((owner->body.pos[AXIS_X] - victim->body.pos[AXIS_X] > 0) != (side > 0), "Teleport must reappear past the peer");
          check(victim->health == Q16_ONE, "Plain teleport must not damage");
        } else if (scenario == 1) {
          check(slammed && victim->health == Q16_ONE - 0x1c28, "Teleport slam must deal 47c4b0 damage once and apply reaction 105");
        } else {
          check(recoiled && !slammed && victim->health == Q16_ONE, "Blocked teleport slam must recoil without capture");
        }
        advance_ticks(60, NULL);
        check(victim->state == S_IDLE && !(owner->flags & FF_PAIRED) && !(victim->flags & FF_PAIRED), "Teleport peer must regain control");
      }
    }
  }
}

/* Reiko B,D,F+HK record 4e4e60 and both fighters' far CPU row enter 4791a0:
 * D[29] 12% with reaction 171, a no-guard block wait, and the 45e7a0
 * 90-tick reuse gate on the command. */
static void check_reiko_noob_flip_kick(void) {
  const int roster[] = {7, 17};
  for (size_t index = 0; index < 2; ++index) {
    for (int slot = 0; slot < 2; ++slot) {
      for (int scenario = 0; scenario < 3; ++scenario) {
        reiko_noob_fight(roster[index], 4, slot, scenario == 2 ? 6.f : 1.2f);
        Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
        Input input[2] = {0};
        if (scenario == 1) {
          input[1 - slot].held = BUTTON_BIT(BUTTON_BLOCK);
        }
        if (roster[index] == 7) {
          Button back = slot ? BUTTON_RIGHT : BUTTON_LEFT, forward = slot ? BUTTON_LEFT : BUTTON_RIGHT;
          fighter_press(owner, back);
          fighter_press(owner, BUTTON_DOWN);
          fighter_press(owner, forward);
          fighter_press(owner, BUTTON_HIGH_KICK);
        } else {
          check(battle_cpu_action(owner, CPU_SPECIAL_2), "Noob's far CPU row must select the flip kick");
        }
        advance_ticks(2, input);
        check(fighter_move(owner)->clip == 110, "Flip kick must play its own clip 110");
        bool no_guard = false;
        int tick = 0;
        for (; tick < 120 && owner->state != S_IDLE; ++tick) {
          advance_ticks(1, input);
          no_guard |= (owner->flags & FF_NO_GUARD) != 0;
        }
        check(owner->state == S_IDLE, "Flip kick must return to stance");
        if (scenario == 0) {
          check(victim->health == Q16_ONE - 0x1eb8 && victim->reaction == 171, "Flip kick must apply D[29] once with reaction 171");
        } else if (scenario == 1) {
          check(victim->health == Q16_ONE - q16_mul(0x1eb8, 0x3333) && no_guard, "Blocked flip kick must chip and wait without guard");
        } else {
          check(victim->health == Q16_ONE, "Distant flip kick must miss");
        }
        if (roster[index] == 7 && scenario == 2) {
          Button back = slot ? BUTTON_RIGHT : BUTTON_LEFT, forward = slot ? BUTTON_LEFT : BUTTON_RIGHT;
          fighter_press(owner, back);
          fighter_press(owner, BUTTON_DOWN);
          fighter_press(owner, forward);
          fighter_press(owner, BUTTON_HIGH_KICK);
          advance_ticks(2, NULL);
          check(fighter_move(owner)->clip != 110 || tick >= 88, "Flip kick reuse must wait 90 ticks after its stamp");
        }
      }
    }
  }
}

/* Reiko D,F+LP record 4e4e30 and both projectile CPU rows enter 496db0:
 * an independent task-82 star queries C[21]; reaction 45 staggers a
 * grounded victim on clip 182 and an airborne one continues as reaction 10. */
static void check_reiko_noob_star(void) {
  const int roster[] = {7, 17};
  for (size_t index = 0; index < 2; ++index) {
    for (int slot = 0; slot < 2; ++slot) {
      for (int scenario = 0; scenario < 3; ++scenario) {
        reiko_noob_fight(roster[index], 4, slot, 3.f);
        Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
        Input input[2] = {0};
        if (scenario == 1) {
          input[1 - slot].held = BUTTON_BIT(BUTTON_BLOCK);
        }
        if (roster[index] == 7) {
          fighter_press(owner, BUTTON_DOWN);
          fighter_press(owner, slot ? BUTTON_LEFT : BUTTON_RIGHT);
          fighter_press(owner, BUTTON_LOW_PUNCH);
        } else {
          check(battle_cpu_action(owner, CPU_PROJECTILE), "Noob's projectile CPU row must select the star");
        }
        bool spawned = false, staggered = false, air = false;
        for (int tick = 0; tick < 160; ++tick) {
          if (scenario == 2 && tick == 12) {
            victim->body.pos[AXIS_Y] = victim->body.ground - Q16_ONE;
            victim->body.gravity = 0x28f;
          }
          advance_ticks(1, input);
          for (int i = 0; i < MAX_PROJECTILES; ++i) {
            spawned |= g_battle.projectiles[i].kind == PROJECTILE_FIREBALL && g_battle.projectiles[i].owner == slot;
          }
          staggered |= fighter_move(victim)->clip == 182;
          air |= victim->reaction == 10;
        }
        check(spawned && owner->state == S_IDLE, "Star must spawn its object and the thrower must recover");
        if (scenario == 0) {
          check(victim->health == Q16_ONE - 0x2147 && staggered && victim->state == S_IDLE, "Grounded star hit must stagger on clip 182 and recover");
        } else if (scenario == 1) {
          check(victim->health == Q16_ONE - q16_mul(0x2147, 0x3333) && !staggered, "Blocked star must chip without the stagger");
        } else {
          check(victim->health == Q16_ONE - 0x2147 && air && !staggered, "Airborne star hit must continue as reaction 10");
        }
      }
    }
  }
  reiko_noob_fight(7, 4, 0, 3.f);
  Fighter *owner = &g_battle.fighters[0];
  fighter_press(owner, BUTTON_DOWN);
  fighter_press(owner, BUTTON_RIGHT);
  fighter_press(owner, BUTTON_LOW_PUNCH);
  advance_ticks(50, NULL);
  fighter_press(owner, BUTTON_DOWN);
  fighter_press(owner, BUTTON_RIGHT);
  fighter_press(owner, BUTTON_LOW_PUNCH);
  advance_ticks(2, NULL);
  check(fighter_move(owner)->clip != 437, "45e6d0 must reject a second star within 80 ticks");
}

/* Reiko B,F+LK record 4e4e80 -> 478ee0: twenty placements circle half way
 * around the peer, ending about 1.2 units behind it, then a 20-visit wait. */
static void check_reiko_orbit(void) {
  for (int slot = 0; slot < 2; ++slot) {
    for (int scenario = 0; scenario < 2; ++scenario) {
      reiko_noob_fight(7, 4, slot, scenario == 0 ? 3.f : 1.f);
      Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
      q16 side = owner->body.pos[AXIS_X] - victim->body.pos[AXIS_X];
      fighter_press(owner, slot ? BUTTON_RIGHT : BUTTON_LEFT);
      fighter_press(owner, slot ? BUTTON_LEFT : BUTTON_RIGHT);
      fighter_press(owner, BUTTON_LOW_KICK);
      advance_ticks(2, NULL);
      if (scenario == 1) {
        check(fighter_move(owner)->clip != 306, "45e710 must reject the circling step inside 1.3 units");
        continue;
      }
      check(fighter_move(owner)->clip == 306, "Circling step must play clip 306");
      int tick = 0;
      for (; tick < 80 && owner->state != S_IDLE; ++tick) {
        advance_ticks(1, NULL);
      }
      check(owner->state == S_IDLE && tick >= 30, "Circling step must hold its stance wait before returning");
      check((owner->body.pos[AXIS_X] - victim->body.pos[AXIS_X] > 0) != (side > 0) && fighter_distance(owner) < 1.6f,
          "Circling step must finish close behind the peer");
      check(victim->health == Q16_ONE, "Circling step has no contact");
    }
  }
}

/* Noob D,F+LP records 4e44b0/4e44bc: the grounded shadow charge returns to
 * the controller; the airborne one leaves through 498eb0 -> 480cf0, where
 * Noob himself falls on clip 310 with gravity 0x20c and lands. */
static void check_noob_shadow_moves(void) {
  for (int slot = 0; slot < 2; ++slot) {
    for (int air = 0; air < 2; ++air) {
      reiko_noob_fight(17, 4, slot, 1.5f);
      Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
      if (air) {
        owner->body.pos[AXIS_Y] = owner->body.ground - 2 * Q16_ONE;
        owner->body.gravity = 0x28f;
        fighter_enter_state(owner, S_JUMP);
      }
      fighter_press(owner, BUTTON_DOWN);
      fighter_press(owner, slot ? BUTTON_LEFT : BUTTON_RIGHT);
      fighter_press(owner, BUTTON_LOW_PUNCH);
      bool fell = false;
      int tick = 0;
      for (; tick < 160 && !(tick > 4 && owner->state == S_IDLE && !fighter_airborne(owner)); ++tick) {
        advance_ticks(1, NULL);
        fell |= fighter_move(owner)->clip == 310 && owner->body.gravity == 0x20c;
      }
      check(tick < 160 && owner->state == S_IDLE, "Noob shadow move must return to stance");
      check(!air || fell, "Air shadow must leave through the clip-310 owner fall");
      check(air || victim->health < Q16_ONE, "Grounded shadow charge must reach its peer");
    }
  }
}

/* Open Reiko's deciding-KO window and enter a finisher through its real
 * generated command record while holding the record's extra switches. */
static bool reiko_finish_command(int slot, FighterFn entry, unsigned held, q16 distance) {
  battle_init(slot ? 4 : 7, slot ? 7 : 4);
  g_battle.phase = PHASE_FIGHT;
  Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
  owner->wins = 1;
  battle_damage(victim, Q16_ONE);
  advance_ticks(1, NULL);
  for (int tick = 0; tick < 180 && g_battle.finish_waiting; ++tick) {
    advance_ticks(1, NULL);
  }
  owner->body.pos[AXIS_X] = slot ? distance / 2 : -distance / 2;
  victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
  const Controls *controls = roster_def(7)->controls;
  const CommandDef *selected = NULL;
  for (int i = 0; i < controls->command_count; ++i) {
    if (controls->commands[i].enter == entry) {
      selected = &controls->commands[i];
    }
  }
  check(selected != NULL, "Reiko finisher needs its real generated command");
  for (int i = 0; i < selected->count; ++i) {
    bool forward = selected->tokens[i] == TOKEN_FORWARD;
    Button direction = selected->tokens[i] == TOKEN_DOWN ? BUTTON_DOWN : (forward != (slot != 0) ? BUTTON_RIGHT : BUTTON_LEFT);
    Input input[2] = {0};
    input[slot].held = BUTTON_BIT(direction);
    if (i == selected->count - 1) {
      input[slot].held |= BUTTON_BIT(selected->button) | held;
    }
    battle_step(input);
    advance_ticks(2, NULL);
  }
  return g_battle.phase == PHASE_FATALITY;
}

/* 467ce0: F,D,F+BLK with BLK+HK+LP+LK held; four detached subtrees, then
 * the owner's 443 victory loop and the result. */
static void check_reiko_first_fatality(void) {
  unsigned chord = BUTTON_BIT(BUTTON_HIGH_KICK) | BUTTON_BIT(BUTTON_LOW_PUNCH) | BUTTON_BIT(BUTTON_LOW_KICK);
  for (int slot = 0; slot < 2; ++slot) {
    check(reiko_finish_command(slot, retail_finish_first, chord, 0xcccc), "Reiko first fatality must start from its held-switch command");
    Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
    int pieces = 0;
    for (int tick = 0; tick < 800 && g_battle.phase == PHASE_FATALITY; ++tick) {
      advance_ticks(1, NULL);
      int active = 0;
      for (int i = 0; i < MAX_SCENE_ACTORS; ++i) {
        active += g_battle.scene_actors[i].active && g_battle.scene_actors[i].actor.subtree_joint > 0;
      }
      pieces = active > pieces ? active : pieces;
    }
    check(pieces == 4 && (victim->hidden_joints[0] & (UINT64_C(1) << 3)), "Reiko's tear must detach torso, head and both arms");
    check(g_battle.phase == PHASE_MATCH_OVER && fighter_move(owner)->clip == 443, "Reiko first fatality must end in its victory loop");
  }
  check(!reiko_finish_command(0, retail_finish_first, 0, 0xcccc), "45e360 must require its held switches");
}

/* 467cd0: B,B,D,D+HK at 1.8..3 units; the star barrage's first arrival
 * starts the clip-554 stagger and push-back, and the victim falls. */
static void check_reiko_second_fatality(void) {
  for (int slot = 0; slot < 2; ++slot) {
    check(reiko_finish_command(slot, retail_finish_second, 0, 0x26666), "Reiko second fatality must start from its command");
    Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
    float start = fighter_distance(owner);
    bool staggered = false;
    for (int tick = 0; tick < 600 && g_battle.phase == PHASE_FATALITY; ++tick) {
      advance_ticks(1, NULL);
      staggered |= fighter_move(victim)->clip == 554;
    }
    check(staggered && fighter_distance(owner) > start + 2.5f, "Star arrivals must stagger and push the victim back");
    check(g_battle.phase == PHASE_MATCH_OVER && fighter_sample(victim) == 159, "Reiko second fatality must finish the fall before the result");
  }
  check(!reiko_finish_command(0, retail_finish_second, 0, 0x10000), "45e530 must reject a close victim");
}

static void check_reiko_noob(void) {
  check_reiko_second_fatality();
  check_reiko_first_fatality();
  check_reiko_noob_high_kick();
  check_noob_shadow_moves();
  check_reiko_orbit();
  check_reiko_noob_star();
  check_reiko_noob_flip_kick();
  check_reiko_noob_teleport();
}

int main(void) {
  check_reiko_noob();
  check_tanya_ports();
  check_spear_interruption();
  check_spear_allocation_and_reset();
  check_fatalities();
  check_generated_cpu_finish_dispatch();
  check_partial_cpu_stage_dispatch();
  check_fujin_first_fatality();
  check_fujin_second_fatality();
  check_advancing_sweeps();
  check_fujin_wind_grab();
  check_fujin_triple_kick();
  check_fujin_wind();
  check_fujin_dive();
  check_cage_first_fatality();
  check_cage_second_fatality();
  check_shinnok_fatalities();
  check_independent_scene_actor_lifetime();
  check_branch_frame_lp_throw(5, 81, 0x4de610);
  check_branch_frame_lp_throw(11, 81, 0x4de610);
  check_branch_frame_lp_throw(12, 80, 0x4de640);
  check_branch_frame_lp_throw(7, 80, 0x4de640);
  check_branch_frame_lp_throw(10, 80, 0);
  check_branch_frame_lp_throw(17, 80, 0x4de640);
  check_peer_release_lp_throw(6, 75);
  check_peer_release_lp_throw(8, 78);
  check_peer_release_lp_throw(9, 79);
  check_peer_release_lp_throw(14, 75);
  check_goro_frame_lp_throw();
  check_goro_projectile_completion();
  check_cage_groin_gate();
  check_weapon_frame_startups();
  check_type69_weapon_commands();
  check_kai_weapon_commands();
  check_cage_weapon_commands();
  check_jarek_weapon_commands();
  check_jarek_balls();
  check_jarek_quake();
  check_jarek_blade();
  check_jarek_fan_victims();
  check_jarek_second_fatality();
  check_jarek_first_fatality();
  check_shinnok_weapon_commands();
  check_shinnok_morph();
  check_fujin_crossbow();
  check_weapon_followup_selection();
  check_reaction_motion_components();
  check_roster_throw_state_entries();
  check_short_diagonal_air_recovery();
  check_selected_high_kick();
  check_jax_air_grab_command();
  check_jax_missile();
  check_jax_dash_punch();
  check_jax_ground_pound();
  check_jax_multi_slam();
  check_jax_first_fatality();
  check_jax_second_fatality();
  check_reptile_reconnected_specials();
  check_reptile_acid_spit();
  check_reptile_invisibility();
  check_reptile_second_fatality();
  check_reptile_first_fatality();
  check_reptile_weapon_commands();
  check_goro_reconnected_counters();
  check_goro_frame_specials();
  check_cage_special_commands();
  check_cage_shadow_kick_branches();
  check_cage_powered_punch_branches();
  check_cage_orb_commands();
  check_reconnected_stage_commands();
  check_script_reactions();
  check_ground_throw_lifetimes();
  check_air_capture_lifetimes();
  check_quan_chi_skull();
  check_quan_chi_flip_kick();
  check_quan_chi_teleport_stomp();
  check_quan_chi_weapon_steal();
  check_quan_chi_weapon();
  check_quan_chi_first_fatality();
  check_quan_chi_second_fatality();
  check_ground_normals();
  check_air_attacks();
  check_roster_low_kick_frames();
  check_kai_handstand_frames();
  check_kai_df_specials();
  check_kai_skulls();
  check_kai_first_fatality();
  check_kai_second_fatality();
  check_reconnected_uppercuts();
  check_roster_jab_frames();
  check_roster_high_punch_frames();
  check_reconnected_roundhouses();
  check_weapon();
  check_specials();
  check_retail_input();
  check_roster();
  check_sequence_events();
  check_wait_phases();
  {
    battle_init(0, 4);
    advance_ticks(89, NULL);
    check(g_battle.phase == PHASE_READY, "Countdown ends too soon");
    advance_ticks(1, NULL);
    check(g_battle.phase == PHASE_FIGHT, "Countdown never starts fight");
  }
  {
    fight(1.f);
    do_attack(S_PUNCH, false, false);
    check(g_battle.fighters[1].health == Q16_ONE - 0xa3d, "Punch must connect once");
    advance_ticks(30, NULL);
    check(g_battle.fighters[1].health == Q16_ONE - 0xa3d, "Repeated hit during recovery");
  }
  {
    fight(3);
    do_attack(S_KICK, false, false);
    check(g_battle.fighters[1].health == Q16_ONE, "Out-of-range attack connects");
  }
  {
    fight(1.f);
    do_attack(S_PUNCH, true, false);
    check(g_battle.fighters[1].health == Q16_ONE - q16_mul(0xa3d, 0x3333) && g_battle.effects[0].blocked, "Blocking fails");
  }
  check_hit_blood();
  {
    fight(1.f);
    do_attack(S_SWEEP, true, false);
    check(g_battle.fighters[1].health == Q16_ONE - 0x23d7, "Standing guard should lose to sweep");
  }
  {
    fight(1.f);
    do_attack(S_SWEEP, true, true);
    check(g_battle.fighters[1].health == Q16_ONE - q16_mul(0x23d7, 0x3333), "Crouch guard fails against sweep");
  }
  {
    fight(0.6f);
    do_attack(S_THROW, true, false);
    advance_ticks(23, NULL);
    check(g_battle.fighters[1].health == Q16_ONE - 0x2666, "LP throw should beat guard for 15 percent at release");
  }
  {
    fight(1.f);
    g_battle.fighters[1].body.pos[AXIS_Y] -= 2 * Q16_ONE;
    do_attack(S_PUNCH, false, false);
    check(g_battle.fighters[1].health == Q16_ONE, "Ground punch hits above its vertical reach");
  }
  {
    fight(1.f);
    do_attack(S_UPPERCUT, false, false);
    check(g_battle.fighters[1].health == Q16_ONE - 0x28f5 && g_battle.fighters[1].state == S_KNOCKDOWN && fighter_airborne(&g_battle.fighters[1]),
        "Uppercut does not launch");
    advance_ticks(100, NULL);
    check(!fighter_airborne(&g_battle.fighters[1]) && g_battle.fighters[1].state == S_IDLE, "Knockdown never recovers");
  }
  {
    fight(1.f);
    Input in[2] = {0};
    /* Both players use the shared recovered jab startup. */
    g_battle.fighters[1].roster = 0;
    in[0].held = in[1].held = BUTTON_BIT(BUTTON_LOW_PUNCH);
    battle_step(in);
    advance_ticks(9, NULL);
    check(g_battle.fighters[0].health == Q16_ONE && g_battle.fighters[1].health == Q16_ONE - 0xa3d, "Script contacts use stable fighter order");
  }
  {
    fight(1.f);
    g_battle.fighters[1].health = 0xa3d;
    do_attack(S_PUNCH, false, false);
    check(g_battle.phase == PHASE_ROUND_OVER && g_battle.winner == 0 && g_battle.fighters[0].wins == 1, "KO/round award failed");
    advance_ticks(180, NULL);
    check(g_battle.phase == PHASE_READY && g_battle.round == 2 && g_battle.fighters[1].health == Q16_ONE && g_battle.fighters[0].wins == 1,
        "Next round reset failed");
    g_battle.phase = PHASE_FIGHT;
    g_battle.fighters[0].body.pos[AXIS_X] = -Q16_ONE / 2;
    g_battle.fighters[1].body.pos[AXIS_X] = Q16_ONE / 2;
    g_battle.fighters[1].health = 0xa3d;
    do_attack(S_PUNCH, false, false);
    check(g_battle.phase == PHASE_FINISH && g_battle.fighters[0].wins == 2, "Final KO must open the finish window");
    for (int i = 0; i < 180 && g_battle.finish_waiting; ++i) {
      advance_ticks(1, NULL);
    }
    check(battle_can_finish(&g_battle.fighters[0]) && !battle_can_finish(&g_battle.fighters[1]), "Only winner may finish");
    advance_ticks(300, NULL);
    check(g_battle.phase == PHASE_MATCH_OVER, "Match result should wait for rematch");
    battle_reset_match();
    check(g_battle.round == 1 && g_battle.fighters[0].wins == 0, "Rematch failed");
  }
  {
    fight(1.f);
    g_battle.fighters[1].health = 0x28f5;
    do_attack(S_UPPERCUT, false, false);
    check(g_battle.phase == PHASE_ROUND_OVER, "Lethal uppercut must end the round");
    advance_ticks(100, NULL);
    check(!fighter_airborne(&g_battle.fighters[1]) && fighter_sample(&g_battle.fighters[1]) == 109 && g_battle.fighters[1].current_mmove == NULL,
        "KO must continue the uppercut landing sequence and hold the final pose");
  }
  {
    fight(1.f);
    g_battle.time_ticks = 1;
    g_battle.fighters[0].health = 75;
    g_battle.fighters[1].health = 50;
    battle_step((Input[2]){0});
    check(g_battle.winner == 0 && g_battle.phase == PHASE_ROUND_OVER, "Timeout winner incorrect");
  }
  {
    fight(1.f);
    g_battle.time_ticks = 1;
    battle_step((Input[2]){0});
    check(g_battle.winner == -1 && g_battle.fighters[0].wins == 0 && g_battle.fighters[1].wins == 0, "Draw should not award wins");
  }
  {
    fight(1.f);
    Input in[2] = {0};
    in[0].held = BUTTON_BIT(BUTTON_UP);
    battle_step(in);
    advance_ticks(90, NULL);
    check(!fighter_airborne(&g_battle.fighters[0]) && g_battle.fighters[0].body.vel[AXIS_Y] == 0, "Jump never lands");
  }
  {
    fight(1.f);
    Input in[2] = {0};
    int i;
    in[0].held = BUTTON_BIT(BUTTON_RIGHT);
    in[1].held = BUTTON_BIT(BUTTON_LEFT);
    advance_ticks(600, in);
    check(fabsf(fighter_x(&g_battle.fighters[0]) - fighter_x(&g_battle.fighters[1])) >= 0.499f, "Bodies overlap");
    in[0].held = BUTTON_BIT(BUTTON_LEFT);
    advance_ticks(600, in);
    for (i = 0; i < 2; ++i)
      check(fabsf(fighter_x(&g_battle.fighters[i])) <= q16_float(g_battle.arena_radius), "Arena wall violated");
  }
  {
    fight(1.f);
    int changes = 0, old = 2 * Q16_ONE, i;
    for (i = 0; i < 18000 && g_battle.phase != PHASE_MATCH_OVER; ++i) {
      Input in[2];
      int sum;
      input_cpu(0, &in[0]);
      input_cpu(1, &in[1]);
      battle_step(in);
      sum = g_battle.fighters[0].health + g_battle.fighters[1].health;
      if (sum < old)
        ++changes;
      old = sum;
    }
    check(changes > 4 && g_battle.phase == PHASE_MATCH_OVER, "CPU match does not progress to completion");
  }
  puts(
      "PASS: four fatalities/both players/gates/rematch; paired ground/air throws; ground normals/combos; victim reactions/recovery/interruption; air attack selection/miss/landing/contact/follow-ups; sword draw/slash/chip/throw/drop/pickup/spin/reset; Scorpion special commands/pull/stun/chip/interruption/teleport/fire; retail press rule/history/latency; 27 recorded normal branch scenarios; generated kick recovery; move callbacks/events/interruption; roster tables; countdown; range; single hits; block; sweep; throw; air reach; launch; ordered contacts; KO; rounds; timeout; draw; rematch; jump; walls; CPU match");
  return 0;
}
