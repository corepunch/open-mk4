#include "../character.h"

static void victory_finish(Fighter *fighter);
static void victory_watch(Fighter *fighter);

bool sub_zero_can_close_fatality(const Fighter *fighter) {
  return battle_can_finish(fighter) && fighter_distance(fighter) <= 1.f;
}

void sub_zero_finish_scene(Fighter *fighter) {
  (void)fighter;
  battle_end_finisher();
}

void sub_zero_scene_blend_cleanup(Fighter *fighter) {
  fighter->scene.clips = NULL;
  fighter->blend_duration = 0;
  fighter->blend_weight = -1;
}

static void victory_finish(Fighter *fighter) {
  (void)fighter;
  fighter_cancel_all_mmoves();
  battle_end_finisher();
}

static void victory_watch(Fighter *fighter) {
  if (fighter_move(fighter)->clip == 118) {
    native_motion_begin(fighter, 50, victory_finish);
    native_motion_play(fighter);
    native_motion_step(fighter);
  }
}

void sub_zero_script_victory(Fighter *fighter) {
  fighter->body.physics = victory_watch;
  retail_victory_continue(fighter);
}

void sub_zero_start_fatality(Fighter *fighter, int id) {
  require(id == FINISH_SUB_ZERO_1 || id == FINISH_SUB_ZERO_2, "Invalid native scene entry");
  if (!battle_begin_finisher(fighter, id)) {
    fighter_ready(fighter);
    return;
  }
  Fighter *victim = fighter_peer(fighter);
  retail_scene_pose(victim, 13, 0, false);
  retail_scene_pose(fighter, id == FINISH_SUB_ZERO_1 ? 385 : 552, 0, true);
  if (id == FINISH_SUB_ZERO_1) {
    /* 445100 -> 475530 aligns the scene pair at 1.45 units. */
    for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
      victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0x17333);
    }
  }
  FighterFn start = id == FINISH_SUB_ZERO_1 ? sub_zero_first_fatality_script : sub_zero_second_fatality_script;
  start(fighter);
}
