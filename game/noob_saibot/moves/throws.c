#include "../character.h"

static const MoveDef s_ground_thrown = {.clip = 157, .last = 130, .flags = SF_LOCK | SF_REACTION | SF_HOLD};

void noob_saibot_lk_throw(Fighter *fighter);
void noob_saibot_air_throw(Fighter *fighter);
static const MoveDef s_noob_air_throw = {.clip = 74, .last = 44, .flags = SF_LOCK | SF_RECORD, .action = 0x502};
static const MoveDef s_noob_air_thrown = {.clip = 150, .last = 27, .flags = SF_LOCK, .action = 0x4003};

/* 46ac10: both paired bodies fall with gravity 0x147a after an upward
 * 0x1999 kick, then 4e5ea0 advances the pair to sample 27. */
void noob_saibot_script_air_throw_continue(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter->body.gravity = 0x147a;
  victim->body.gravity = 0x147a;
  fighter->body.vel[AXIS_Y] = -0x1999;
  victim->body.vel[AXIS_Y] = -0x1999;
  noob_saibot_entry_retail_script_004e5ea0(fighter);
}

/* 46aca0 after the 4e5eb0 landing wait: 48de80 subtracts 0x30a3 from the
 * peer, then the shared slam tail 46acc0 releases it. */
void noob_saibot_script_air_throw_end(Fighter *fighter) {
  battle_damage((Fighter *)fighter_opponent(fighter), 0x30a3);
  noob_saibot_slam_tail(fighter);
}

void noob_saibot_lp_throw(Fighter *fighter) {
  if (!mk4_can_ground_throw(fighter, false)) {
    fighter_ready(fighter);
    return;
  }
  mk4_mmove_throw_start(fighter, &noob_saibot_s_throw, &s_ground_thrown, noob_saibot_entry_retail_script_004de798);
}

void noob_saibot_lk_throw(Fighter *fighter) {
  mk4_mmove_throw_start(fighter, &noob_saibot_s_throw, &s_ground_thrown, noob_saibot_entry_retail_script_004de2f8);
}

/* 46aba0: action 0x502, face the peer (490bd0), then the shared 480fc0(8)
 * and 48f9f0 capture before 4e5e78. 532254's 30-tick overlap suppression
 * is the portable paired-body exemption. */
void noob_saibot_air_throw(Fighter *fighter) {
  if (mk4_can_air_throw(fighter)) {
    fighter->facing = fighter->body.pos[AXIS_X] <= fighter_opponent(fighter)->body.pos[AXIS_X] ? 1 : -1;
  }
  mk4_air_throw_frames_with_moves(fighter, &s_noob_air_throw, &s_noob_air_thrown, noob_saibot_entry_retail_script_004e5e78, noob_saibot_slam_released_fall);
}
