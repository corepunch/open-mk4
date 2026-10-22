#include "game.h"
#include "retail_native.h"
#include "script.h"
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

static void advance_ticks(int n, const InputFrame *in) {
  const InputFrame zero[2] = {0};
  int i;
  for (i = 0; i < n; ++i)
    battle_step(in ? in : zero);
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

/* Match lifecycle, movement and body constraints belong to our engine. Retail
 * authored move timing/damage is covered by the source scripts, not matrices. */
int main(void) {
  check_roster();
  check_sequence_events();
  check_wait_phases();
  check_independent_scene_actor_lifetime();
  battle_init(0, 4);
  advance_ticks(89, NULL);
  check(g_battle.phase == PHASE_READY, "Countdown ends too soon");
  advance_ticks(1, NULL);
  check(g_battle.phase == PHASE_FIGHT, "Countdown never starts fight");
  fight(4.f);
  g_battle.fighters[0].wins = 1;
  battle_damage(&g_battle.fighters[1], Q16_ONE);
  advance_ticks(1, NULL);
  check(g_battle.phase == PHASE_FINISH && g_battle.winner == 0 && g_battle.fighters[0].wins == 2,
      "Final KO must award the decided result and open the finish window");
  battle_reset_match();
  check(g_battle.round == 1 && g_battle.fighters[0].wins == 0 && g_battle.fighters[1].health == Q16_ONE, "Rematch reset failed");
  fight(4.f);
  g_battle.time_ticks = 1;
  g_battle.fighters[0].health = 75;
  g_battle.fighters[1].health = 50;
  battle_step((InputFrame[2]){0});
  check(g_battle.winner == 0 && g_battle.phase == PHASE_ROUND_OVER, "Timeout winner incorrect");
  fight(4.f);
  g_battle.time_ticks = 1;
  battle_step((InputFrame[2]){0});
  check(g_battle.winner == -1 && g_battle.fighters[0].wins == 0 && g_battle.fighters[1].wins == 0, "Draw should not award wins");
  fight(4.f);
  InputFrame input[2] = {0};
  input[0].held = BUTTON_BIT(BUTTON_UP);
  battle_step(input);
  advance_ticks(90, NULL);
  check(!fighter_airborne(&g_battle.fighters[0]) && g_battle.fighters[0].body.vel[AXIS_Y] == 0, "Jump never lands");
  fight(1.f);
  input[0].held = BUTTON_BIT(BUTTON_RIGHT);
  input[1].held = BUTTON_BIT(BUTTON_LEFT);
  advance_ticks(600, input);
  check(fabsf(fighter_x(&g_battle.fighters[0]) - fighter_x(&g_battle.fighters[1])) >= .499f, "Bodies overlap");
  input[0].held = BUTTON_BIT(BUTTON_LEFT);
  advance_ticks(600, input);
  for (int slot = 0; slot < 2; ++slot) {
    check(fabsf(fighter_x(&g_battle.fighters[slot])) <= q16_float(g_battle.arena_radius), "Arena wall violated");
  }
  puts("PASS: shared roster views, engine callbacks, waits, scene ownership, match lifecycle, timeout, draw, jump and body constraints");
  return 0;
}
