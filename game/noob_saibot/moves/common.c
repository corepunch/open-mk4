#include "../character.h"
#include "math_util.h"

static const MoveDef s_noob_shadow_low = {.clip = 26, .last = 13, .flags = SF_LOCK, .action = 0x309};
static const MoveDef s_noob_shadow_up = {.clip = 24, .last = 9, .flags = SF_LOCK, .action = 0x30a};
static const MoveDef s_lp_thrown = {153, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION, NULL, 0};

void noob_saibot_script_lk_return(Fighter *fighter) {
  mk4_mmove_lk_owner_return(fighter, fighter_ready);
}

void noob_saibot_script_lp_select(Fighter *fighter) {
  q16 along = 0;
  int64_t distance2 = 0;
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    q16 delta = fighter->saved_motion[axis];
    along += q16_mul(delta, fighter_axis(fighter, axis));
    distance2 += ((int64_t)delta * delta) >> 16;
  }
  if (along >= 0 && distance2 < 0x280000) {
    noob_saibot_entry_retail_script_004de6b8(fighter);
  } else {
    noob_saibot_entry_retail_script_004de640(fighter);
  }
}

void noob_saibot_script_lp_end(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  noob_saibot_entry_retail_script_004de664(victim);
  noob_saibot_entry_retail_script_004de668(fighter);
}

void noob_saibot_script_lp_return(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter->flags &= ~(unsigned)FF_PAIRED;
  victim->flags &= ~(unsigned)FF_PAIRED;
  mk4_script_stop_body(fighter);
  fighter_finish_mmove(fighter, fighter_ready);
}

void noob_saibot_script_finish(Fighter *fighter) {
  native_advancing_sweep(fighter);
}

void noob_saibot_script_opponent_reaction(Fighter *fighter, int reaction) {
  if (reaction == 175) {
    return;
  }

  if (reaction == 77) {
    mk4_mmove_lk_release(fighter, reaction);
    return;
  }
  require(false, "Unexpected Noob Saibot opponent reaction");
}

void noob_saibot_shadow_low(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &s_noob_shadow_low);
  noob_saibot_entry_retail_script_004ed268(fighter);
}

void noob_saibot_shadow_up(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &s_noob_shadow_up);
  noob_saibot_entry_retail_script_004ed2c0(fighter);
}

void noob_saibot_shadow_task_low(Fighter *fighter, int task_type) {
  require(task_type == 75, "Unexpected Noob Saibot shadow task type");
  battle_shadow_actor_start(fighter, SHADOW_ACTOR_LOW);
}

void noob_saibot_shadow_task_up(Fighter *fighter, int task_type) {
  require(task_type == 75, "Unexpected Noob Saibot shadow task type");
  battle_shadow_actor_start(fighter, SHADOW_ACTOR_UP);
}

static const MoveDef s_shadow_fall = {.clip = 310, .last = 18, .flags = SF_LOCK, .action = 0x1005};
static const MoveDef s_shadow_land = {.clip = 310, .last = MOVE_END, .flags = SF_LOCK, .action = 0x1005};

static void shadow_land_finish(Fighter *fighter) {
  mk4_return_to_stance(fighter);
}

/* 480cf0 state 1 on touchdown: pose 18 of clip 310, then 428d10 plays
 * the rest one pose per visit before the controller (46f650). */
static void shadow_land(Fighter *fighter) {
  fighter_start_move(fighter, S_JUMP, &s_shadow_land);
  fighter->sample_override = 18;
  int last = fighter->assets != NULL ? (int)fighter_clip(fighter->assets, fighter)->frames.count - 1 : 18;
  if (last <= 18) {
    native_hold(fighter, 1, shadow_land_finish);
    return;
  }
  native_begin(fighter, 1, last - 18, shadow_land_finish);
  for (int frame = 1; frame <= last - 18; ++frame) {
    native_frame(fighter, frame, 18 + frame, NULL);
  }
  native_play(fighter, false);
}

static void shadow_fall_frame(Fighter *fighter) {
  if (!fighter_airborne(fighter)) {
    shadow_land(fighter);
    return;
  }
  /* 4285d0 advances the falling pose one sample per visit up to 18. */
  fighter->sample_override = mini(18, fighter_sample(fighter) + 1);
  fighter->mmove_hold = true;
}

/* 498eb0 -> 480cf0(0x20c, 0) in the owner's context, after 4ed2c0 spawned
 * the rising shadow: gravity 0x20c, Y velocity 0, action 0x1005, face the
 * peer and fall on clip 310 from pose 7. The shared shadow-actor event
 * keeps its existing portable completion signal. */
void noob_saibot_shadow_install_physics(Fighter *fighter) {
  battle_shadow_actor_install_physics(fighter);
  fighter->facing = fighter->body.pos[AXIS_X] <= fighter_opponent(fighter)->body.pos[AXIS_X] ? 1 : -1;
  fighter_start_move(fighter, S_JUMP, &s_shadow_fall);
  fighter->body.gravity = 0x20c;
  fighter->body.vel[AXIS_Y] = 0;
  fighter->sample_override = 7;
  native_begin(fighter, 0, 0, shadow_land);
  native_frame(fighter, 0, 7, shadow_fall_frame);
  native_play(fighter, false);
}

void noob_saibot_script_lp_reaction(Fighter *fighter, int reaction) {
  require(reaction == 102 || reaction == 107, "Unexpected noob_saibot LP victim reaction");
  Fighter *victim = mk4_script_release_pair(fighter);
  victim->reaction = reaction;
  victim->flags |= FF_REACTING;
  fighter_start_move(victim, S_KNOCKDOWN, &s_lp_thrown);
  if (reaction == 102) {
    noob_saibot_entry_retail_script_004e9bd0(victim);
  } else {
    noob_saibot_entry_retail_script_004e9b40(victim);
  }
}
