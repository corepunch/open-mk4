#include "game.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

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

static void check_roster(void) {
  const CharacterDef *human = roster_def(0);
  const CharacterDef *goro = roster_def(15);
  check(roster_count() == 18, "MK4 roster should be 18 fighters");
  check(human->game == GAME_MK4 && goro->game == GAME_MK4, "Current rows are MK4");
  check(human->controls != roster_def(4)->controls, "Scorpion owns its command and stance tables");
  for (int i = 0; i < roster_count(); ++i)
    check(roster_def(i)->moves != NULL && roster_def(i)->move_count == S_COUNT, "Every fighter owns a move table");
  check(human->moves != goro->moves, "Goro names clips from his own bank");
  check(roster_clip(human, S_IDLE, 77) == 77, "Idle reads the mesh idle clip");
  check(roster_clip(human, S_PUNCH, 0) == 294, "Scorpion punch clip");
  check(roster_clip(human, S_KICK, 0) == 66, "Scorpion kick from the 0x4e5ca4 table");
  check(roster_clip(roster_def(1), S_KICK, 0) == 71, "Raiden kick from the 0x4e5ca4 table");
  check(roster_clip(human, S_BLOCK, 0) == 322 && roster_clip(goro, S_BLOCK, 0) == 595, "Block reads slot 0x53ce30 or Goro's clip");
  check(roster_clip(roster_def(16), S_VICTORY, 0) == roster_clip(roster_def(2), S_VICTORY, 0), "Kitana reads Sonya's character entries");
  check(roster_clip(roster_def(17), S_THROW, 0) == roster_clip(roster_def(7), S_THROW, 0), "Noob Saibot reads Reiko's character entries");
  check(roster_clip(goro, S_PUNCH, 0) == 597, "Goro punch from his 0x4dfb40 script");
  check(goro->moves[S_CROUCH]->first > goro->moves[S_CROUCH]->last, "Goro's duck plays his rise backwards");
  check(roster_clip(roster_def(13), S_THROW, 0) == 83, "Jax throw from script 0x4de4f0");
  check(roster_clip(human, S_VICTORY, 0) == 115 && (human->moves[S_VICTORY]->flags & SF_HOLD), "Scorpion holds his 0x4e6a44 win pose");
  check(human->moves[S_PUNCH]->flags & SF_RECORD, "Scorpion uses script attack records");
  check(human->moves[S_BLOCK]->flags & SF_HOLD, "Block holds its last frame");
  check(human->moves[S_SWEEP]->flags & SF_LOW, "Sweep is a low");
  check(human->moves[S_THROW]->flags & SF_UNBLOCKABLE, "Throw is unblockable");
}

static void check_original_kicks(void) {
  struct KickFixture {
    int roster, clip, count;
    int frames[30];
  };
  static const struct KickFixture fixtures[] = {
#include "fixtures/original_kicks.inc"
  };
  for (size_t i = 0; i < sizeof fixtures / sizeof fixtures[0]; ++i) {
    const struct KickFixture *expected = &fixtures[i];
    fight(4.f);
    int player = expected->roster == 0 ? 0 : 1;
    Fighter *fighter = &g_battle.fighters[player];
    Input in[2] = {0};
    in[player].held = BUTTON_BIT(BUTTON_HIGH_KICK);
    battle_step(in);
    check(fighter->pending != NULL && fighter->state == S_IDLE, "Press must start its move on the next tick");
    in[player] = (Input){0};
    battle_step(in);
    check(fighter->current_move->clip == expected->clip, "Kick callback chose wrong move");
    int visits = 0;
    while (fighter->state == S_KICK && visits < 40) {
      int sample = fighter_frame(fighter, expected->clip == 66 ? 18 : 28);
      check(sample >= 0 && sample < (expected->clip == 66 ? 18 : 28) && (visits != 0 || sample == 0), "Generated kick playback left its decoded clip");
      in[player] = (Input){.held = BUTTON_BIT(BUTTON_LEFT)};
      battle_step(in);
      ++visits;
    }
    check(visits >= expected->count && visits <= expected->count + 3 && fighter->state == S_WALK && fighter->ticks == 0,
        "Generated kick must finish its bounded recovery and return movement control");
    check(fighter->current_move->sequence == NULL, "Movement kept stale kick playback");

    fighter_enter_state(fighter, S_KICK);
    advance_ticks(5, NULL);
    fighter_enter_state(fighter, S_HIT);
    check(fighter->current_move->sequence == NULL, "Reaction kept interrupted kick sequence");
    advance_ticks(30, NULL);
    check(fighter->state == S_IDLE, "Interrupted kick completion overwrote reaction");

    fight(1.f);
    fighter = &g_battle.fighters[player];
    in[0] = (Input){0};
    in[1] = (Input){0};
    in[player].held = BUTTON_BIT(BUTTON_HIGH_KICK);
    battle_step(in);
    advance_ticks(expected->roster == 0 ? 12 : 11, NULL);
    check(g_battle.fighters[1 - player].health == Q16_ONE, "Kick contact started before retail query");
    advance_ticks(1, NULL);
    check(g_battle.fighters[1 - player].health == Q16_ONE - 0x2147, "Kick missed its first retail contact step");
    advance_ticks(2, NULL);
    check(g_battle.fighters[1 - player].health == Q16_ONE - 0x2147, "Kick connected more than once");
  }
}

static void sequence_event(Fighter *fighter) {
  fighter->health--;
}

static void check_original_scorpion(void) {
  struct ScorpionFixture {
    const char *name;
    int state, result, contact_tick, press_count, presses[8], facing_away, count;
    int clips[256], frames[256];
  };
  static const struct ScorpionFixture fixtures[] = {
#include "fixtures/original_scorpion.inc"
  };
  for (size_t i = 0; i < sizeof fixtures / sizeof fixtures[0]; ++i) {
    const struct ScorpionFixture *expected = &fixtures[i];
    fight(4.f);
    Fighter *fighter = &g_battle.fighters[0];
    if (expected->facing_away) {
      fighter->facing = -1;
    }
    fighter_enter_state(fighter, expected->state);
    bool second = false;
    bool expected_second = false;
    for (int tick = 0; tick < expected->count; ++tick) {
      expected_second |= expected->clips[tick] == 295;
    }
    int tick = 0;
    for (; tick < expected->count + 16 && fighter->state == expected->state; ++tick) {
      int clip = fighter_move(fighter)->clip;
      int count = clip == 294 ? 23 : clip == 295 ? 19 : clip == 293 ? 20 : clip == 63 ? 46 : 18;
      check(fighter_frame(fighter, count) >= 0 && fighter_frame(fighter, count) < count, "Generated normal left its decoded animation");
      second |= clip == 295;
      if (expected->result != 0 && tick == expected->contact_tick) {
        fighter_note_contact(fighter, (ContactResult)expected->result);
      }
      for (int press = 0; press < expected->press_count; ++press) {
        if (expected->presses[press] == tick + 1) {
          fighter->buffered |= BUTTON_BIT(BUTTON_LOW_PUNCH);
        }
      }
      battle_step((Input[2]){0});
    }
    check(second == expected_second, "Jab must preserve the recorded repeat/away branch");
    check(fighter->state == S_IDLE && !fighter_fiber_active(fighter), "Generated normal must finish recovery and retire its stack");
  }
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

static void count_sequence_end(Fighter *fighter) {
  fighter->script_timer++;
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
  fighter_start_sequence(fighter, &once);
  advance_ticks(5, NULL);
  check(fighter->script_timer == 1, "A completed phase must call its completion callback only once");

  /* A new match reuses the singleton's storage. Old phase/object state must
   * not leak into that match or keep a stale completion callback alive. */
  fighter_start_sequence(fighter, &wait);
  g_battle.contacts_off = true;
  g_battle.camera.active = true;
  g_battle.projectiles[0].kind = PROJECTILE_SPEAR;
  battle_init(4, 0);
  check(g_battle.fighters[0].roster == 4 && g_battle.fighters[1].roster == 0 && g_battle.phase == PHASE_READY && g_battle.ticks == 0,
      "Singleton initialization must start the requested fresh match");
  check(g_battle.fighters[0].sequence == NULL && g_battle.fighters[0].script == NULL && g_battle.fighters[0].pending == NULL &&
            g_battle.projectiles[0].kind == 0 && !g_battle.camera.active && !g_battle.contacts_off,
      "Singleton initialization must discard old continuations, objects and scene state");
  advance_ticks(45, NULL);
  check(g_battle.phase == PHASE_READY && g_battle.fighters[0].health == Q16_ONE && g_battle.fighters[0].script_timer == 0,
      "A previous match's wait callback must not run in the fresh match");
}

static void check_scorpion_contact_branches(void) {
  for (int blocked = 0; blocked < 2; ++blocked) {
    fight(1.f);
    Fighter *fighter = &g_battle.fighters[0];
    fighter_enter_state(fighter, S_UPPERCUT);
    check(fighter_fiber_active(fighter), "Uppercut must run its branches in a fiber");
    Input in[2] = {0};
    if (blocked) {
      in[1].held = BUTTON_BIT(BUTTON_BLOCK);
    }
    advance_ticks(7, in);
    check(fighter->contact == (blocked ? CONTACT_BLOCK : CONTACT_HIT) && fighter->contact_tick == 7,
        "Battle did not deliver uppercut contact to its continuation");
    int end = blocked ? 31 : 61;
    advance_ticks(end - 8, in);
    check(fighter->state == S_UPPERCUT, "Contact recovery ended before retail return");
    advance_ticks(1, in);
    check(fighter->state == S_IDLE, "Contact recovery did not return on retail step");
  }
  {
    fight(4.f);
    Fighter *fighter = &g_battle.fighters[0];
    fighter_enter_state(fighter, S_PUNCH);
    advance_ticks(7, NULL);
    battle_step((Input[2]){{.held = BUTTON_BIT(BUTTON_LOW_PUNCH)}, {0}});
    battle_step((Input[2]){0});
    check(fighter_move(fighter)->clip == 295, "Jab repeat failed at its native branch");
    fighter_enter_state(fighter, S_HIT);
    advance_ticks(30, NULL);
    check(fighter->state == S_IDLE && fighter->pending == NULL && fighter->buffered == 0, "Interrupted jab retained a continuation or buffered repeat");
  }
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

static void check_spear_fiber_interruption(void) {
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
        check(fighter_fiber_active(owner), "Spear must retain its fiber until interrupted or pull arrival");
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
        check(!fighter_fiber_active(owner), "Interrupt must cancel a suspended spear stack immediately");
        check(victim->state == S_IDLE, "Canceling a pulling spear must release its victim immediately");
        for (int i = 0; i < MAX_PROJECTILES; ++i) {
          check(g_battle.projectiles[i].kind != PROJECTILE_SPEAR, "Cancellation left a live spear object");
        }
        advance_ticks(70, NULL);
        check(victim->health == health && victim->state == S_IDLE, "Interrupted startup/recovery/pull ran stale projectile or victim work");
        check(owner->action == (queued ? 0 : ACTION_RETAIL_1003) && !fighter_fiber_active(owner),
            "Old spear completion must not replace the interruption's controller");
      }
    }
  }
}

static void unused_projectile(Projectile *projectile) {
  (void)projectile;
}

static void check_spear_fiber_allocation_and_reset(void) {
  fight(10.f);
  for (int i = 0; i < MAX_PROJECTILES; ++i) {
    check(battle_spawn_projectile(&g_battle.fighters[1], PROJECTILE_ICE, 100, unused_projectile) != NULL, "Fill the independent object pool");
  }
  command(0, BUTTON_LEFT, BUTTON_LEFT, BUTTON_LOW_PUNCH);
  advance_ticks(60, NULL);
  check(fighter_fiber_active(&g_battle.fighters[0]), "Failed projectile allocation must retain the recovery wait");
  advance_ticks(1, NULL);
  check(!fighter_fiber_active(&g_battle.fighters[0]) && g_battle.fighters[0].state == S_IDLE, "Failed projectile allocation must still retire the spear fiber");
  for (int i = 0; i < MAX_PROJECTILES; ++i) {
    check(g_battle.projectiles[i].kind == PROJECTILE_ICE, "Failed spear allocation overwrote another object");
  }

  fight(4.f);
  command(0, BUTTON_LEFT, BUTTON_LEFT, BUTTON_LOW_PUNCH);
  advance_ticks(27, NULL);
  check(g_battle.fighters[1].action == 0x4000, "Reset must occur during a live spear pull");
  battle_reset_match();
  check(!fighter_fiber_active(&g_battle.fighters[0]) && g_battle.fighters[0].script == NULL && g_battle.fighters[1].state == S_IDLE,
      "Rematch must cancel the spear stack and discard both paired controllers");
  advance_ticks(100, NULL);
  check(g_battle.fighters[0].health == Q16_ONE && g_battle.fighters[1].health == Q16_ONE && g_battle.fighters[0].state == S_IDLE,
      "A canceled pull must not resume after the new match begins");

  /* Both actors own a sleeping stack. The earlier projectile interrupts the
   * other stack, whose cleanup must remove its in-flight projectile too. */
  battle_init(0, 0);
  g_battle.phase = PHASE_FIGHT;
  g_battle.fighters[0].body.pos[AXIS_X] = -2 * Q16_ONE;
  g_battle.fighters[1].body.pos[AXIS_X] = 2 * Q16_ONE;
  command(0, BUTTON_LEFT, BUTTON_LEFT, BUTTON_LOW_PUNCH);
  command(1, BUTTON_RIGHT, BUTTON_RIGHT, BUTTON_LOW_PUNCH);
  check(fighter_fiber_active(&g_battle.fighters[0]) && fighter_fiber_active(&g_battle.fighters[1]), "Mirror spears must have independent suspended stacks");
  advance_ticks(45, NULL);
  check(g_battle.fighters[0].health == Q16_ONE && g_battle.fighters[1].health == Q16_ONE - 0xccc && g_battle.fighters[1].action == 0x1008,
      "Earlier spear must cancel the other fiber and projectile, then finish its victim pull");
  check(fighter_fiber_active(&g_battle.fighters[0]) && fighter_fiber_active(&g_battle.fighters[1]), "Mirror pull arrival must start return and stun fibers");
  advance_ticks(100, NULL);
  check(g_battle.fighters[0].state == S_IDLE && g_battle.fighters[1].state == S_IDLE, "Mirror pull must return both actors without stale waits");
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
      check(fighter->action == 0x1007 && fighter_fiber_active(fighter) && fighter_sample(fighter) == 30,
          "Spear startup must enter a fresh recovery wait with action 0x1007");
      advance_ticks(44, NULL);
      check(fighter->state == S_PUNCH && fighter_sample(fighter) == 30, "Spear recovery returned before its 45-tick wait");
      advance_ticks(2, NULL);
      check(fighter->state == S_IDLE && !fighter_fiber_active(fighter),
          "Normal and fast spear must each retire after its 45-tick recovery and tagged return wait");
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
    check(fighter->current_move->clip == 14 && fighter->script != NULL, "B,B,LP must enter spear for either player");
    advance_ticks(45, NULL);
    check(victim->health == Q16_ONE - 0xccc && fighter_distance(fighter) < 0.9f, "Spear must damage once and pull into follow-up range");
    check(victim->action == 0x1008 && victim->damage_scale == 0x9999, "Spear must leave scaled combo stun");
    check(fighter_fiber_active(fighter) && fighter_fiber_active(victim), "Pull arrival must start independent return and stun fibers");
    advance_ticks(100, NULL);
    check(fighter->state == S_IDLE && victim->state == S_IDLE, "Spear must release both fighters");
    for (int i = 0; i < MAX_PROJECTILES; ++i) {
      check(g_battle.projectiles[i].kind == 0, "Spear leaked a projectile");
    }
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
    advance_ticks(120, NULL);
    check(!g_battle.fighters[0].armed && g_battle.fighters[0].weapon_used, "Throw must detach and prevent a new sword");
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
  for (int followup = 0; followup < 3; ++followup) {
    fight(1.f);
    Fighter *fighter = &g_battle.fighters[0];
    fighter_enter_state(fighter, S_JUMP);
    fighter->body.pos[AXIS_Y] = fighter->body.ground - 0x14ccc;
    fighter->body.gravity = 1;
    Fighter *victim = &g_battle.fighters[1];
    fighter_enter_state(victim, S_JUMP);
    victim->body.pos[AXIS_Y] = victim->body.ground - 0x14ccc;
    victim->body.gravity = 1;
    battle_step((Input[2]){{BUTTON_BIT(BUTTON_HIGH_PUNCH)}, {0}});
    advance_ticks(6, NULL);
    check(g_battle.fighters[1].health == Q16_ONE, "Jump HP must not hit before its strike hold");
    advance_ticks(1, NULL);
    check(g_battle.fighters[1].health == Q16_ONE - 5898 && fighter->body.gravity == 0 && fighter->buttons != NULL,
        "High jump HP contact must deal nine percent and open its frozen follow-up");
    q16 y = fighter->body.pos[AXIS_Y];
    Button next = followup == 0 ? BUTTON_HIGH_PUNCH : followup == 1 ? BUTTON_LOW_PUNCH : BUTTON_HIGH_KICK;
    battle_step((Input[2]){{BUTTON_BIT(next)}, {0}});
    advance_ticks(1, NULL);
    check(fighter->body.pos[AXIS_Y] == y, "Air follow-up must preserve the contact freeze");
    check(fighter_move(fighter)->clip == (followup == 0 ? 97 : followup == 1 ? 96 : 327), "Jump HP allows LP or kick, but cannot repeat HP");
    advance_ticks(110, NULL);
    check(fighter->state == S_IDLE && !fighter_airborne(fighter), "Air contact/follow-up must eventually fall and recover");
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

static void start_scorpion_throw(int player, bool lk) {
  battle_init(0, 0);
  g_battle.phase = PHASE_FIGHT;
  g_battle.fighters[0].body.pos[AXIS_X] = -0x4ccc;
  g_battle.fighters[1].body.pos[AXIS_X] = 0x4ccc;
  fighter_press(&g_battle.fighters[player], lk ? BUTTON_LOW_KICK : BUTTON_LOW_PUNCH);
  advance_ticks(1, NULL);
}

static void check_ground_throws(void) {
  for (int player = 0; player < 2; ++player) {
    for (int lk = 0; lk < 2; ++lk) {
      start_scorpion_throw(player, lk);
      Fighter *owner = &g_battle.fighters[player], *victim = &g_battle.fighters[1 - player];
      check(fighter_move(owner)->clip == (lk ? 37 : 75) && fighter_move(victim)->clip == (lk ? 157 : 152), "Ground throws must choose paired clips");
      advance_ticks(41, NULL);
      check(victim->health == Q16_ONE, "Ground throw damage must wait for sample 42");
      advance_ticks(1, NULL);
      check(victim->health == Q16_ONE - (lk ? 0x2e14 : 0x2666), "Ground throw damage must match selected script");
      advance_ticks(160, NULL);
      check(owner->state == S_IDLE && victim->state == S_IDLE && !((owner->flags | victim->flags) & FF_PAIRED), "Ground throws must release both controllers");

      start_scorpion_throw(player, lk);
      advance_ticks(41, NULL);
      fighter_interrupt(&g_battle.fighters[lk ? player : 1 - player], fighter_ready);
      advance_ticks(180, NULL);
      check(g_battle.fighters[1 - player].health == Q16_ONE && !((g_battle.fighters[0].flags | g_battle.fighters[1].flags) & FF_PAIRED) &&
                !fighter_fiber_active(&g_battle.fighters[0]) && !fighter_fiber_active(&g_battle.fighters[1]),
          "Interrupted generated throws must cancel damage and retire both pair lifetimes");
    }
    start_scorpion_throw(player, true);
    g_battle.fighters[1 - player].health = 0x1000;
    advance_ticks(200, NULL);
    check(g_battle.fighters[1 - player].health == 0 && !((g_battle.fighters[0].flags | g_battle.fighters[1].flags) & FF_PAIRED),
        "Lethal generated throw must detach both actors");
    battle_reset_match();
    check(
        !fighter_fiber_active(&g_battle.fighters[0]) && !fighter_fiber_active(&g_battle.fighters[1]), "Reset must retire a committed throw's remaining stacks");
  }
}

static void check_air_throw(void) {
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
    check(owner->current_move->clip == 82 && victim->current_move->clip == 202, "Air throw must take both participants");
    advance_ticks(27, NULL);
    check(victim->health == Q16_ONE && owner->body.gravity == 0, "Air throw must hold before its damage event");
    advance_ticks(1, NULL);
    check(victim->health == Q16_ONE - 0x2147, "Air throw must deal 13 percent at release");
    advance_ticks(140, NULL);
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

int main(void) {
  check_spear_fiber_interruption();
  check_spear_fiber_allocation_and_reset();
  check_fatalities();
  check_script_reactions();
  check_ground_throws();
  check_air_throw();
  check_ground_normals();
  check_air_attacks();
  check_weapon();
  check_specials();
  check_retail_input();
  check_roster();
  check_original_kicks();
  check_original_scorpion();
  check_sequence_events();
  check_wait_phases();
  check_scorpion_contact_branches();
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
    check(!fighter_airborne(&g_battle.fighters[1]) && fighter_sample(&g_battle.fighters[1]) == 109,
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
