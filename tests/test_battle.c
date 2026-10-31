#include "game.h"
#include "mk4_program.h"

static Mk4Image image;
#include "native/actions.h"
#include "native/helpers.h"
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
  battle_init(&image, 0, 4);
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
    check(view->asset == i && view->moves != NULL && view->move_count == S_COUNT, "Retail roster views must provide bounded engine state and model identity");
    check(view->moves[S_PUNCH]->clip == -1 && view->enter[S_PUNCH] == action_jab, "Unstarted script states must carry no authored animation or balance");
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
  actor->execution.cleanup = scene_actor_cleanup;
}

static void check_independent_scene_actor_lifetime(void) {
  for (int slot = 0; slot < 2; ++slot) {
    battle_init(&image, 14, 4);
    g_battle.phase = PHASE_FIGHT;
    Fighter *owner = &g_battle.fighters[slot], *source = &g_battle.fighters[1 - slot];
    const MoveDef *source_move = fighter_move(source);
    scene_actor_cleanups = 0;
    static const MoveDef pose = {.clip = 130, .last = 39};
    Fighter *actor = battle_scene_actor_start(owner, source, &pose, scene_actor_entry);
    check(actor != NULL && actor->roster == source->roster && actor->buttons == NULL, "Independent scene body must borrow the selected model without controls");
    check(fighter_move(source) == source_move && source->execution.frames == NULL, "Creating a scene body must preserve the source fighter's playback");
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
  image = mk4_image_load("build/mk4.data");
  battle_init(&image, 0, 4);
  check_roster();
  check_independent_scene_actor_lifetime();
  battle_init(&image, 0, 4);
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
