#include "../character.h"

static void flying_query(Fighter *fighter);
static void flying_branch(Fighter *fighter);
static void flying_hold(Fighter *fighter);
static const MoveDef s_flying_kick = {.clip = 92, .last = 5, .flags = RETAIL, .action = ACTION_RETAIL_0207};
static const AttackRecord flying_record = {0x14f5c, -0xf5c, 0xf5c2, 0x51eb, 31, 31, 0, 0, 0x23d7};

static void flying_query(Fighter *fighter) {
  fighter->mmove_hold = fighter_airborne(fighter) && battle_contact(fighter, &flying_record) == CONTACT_MISS;
}

static void flying_branch(Fighter *fighter) {
  if (!fighter_airborne(fighter)) {
    liu_kang_entry_retail_script_004e86d0(fighter);
  } else if (fighter->contact == CONTACT_BLOCK) {
    liu_kang_entry_retail_script_004e8718(fighter);
  } else {
    liu_kang_entry_retail_script_004e86e8(fighter);
  }
}

static void flying_hold(Fighter *fighter) {
  if (fighter_airborne(fighter) && battle_contact(fighter, &flying_record) == CONTACT_MISS) {
    native_begin(fighter, 0, 0, flying_branch);
    fighter->native.frames[0].thinkfunc = flying_query;
    native_play(fighter, false);
  } else {
    flying_branch(fighter);
  }
}

void liu_kang_flying_kick_script(Fighter *fighter) {
  native_finish_clip(fighter, flying_hold);
}

void liu_kang_flying_kick(Fighter *fighter) {
  if (g_battle.phase != PHASE_FIGHT || fighter_airborne(fighter)) {
    return;
  }
  mk4_script_stop_body(fighter);
  fighter_start_move(fighter, S_KICK, &s_flying_kick);
  fighter->sample_override = 0;
  fighter_set_speed(fighter, 0x23d7);
  fighter->body.vel[AXIS_Y] = -0x170a;
  fighter->body.gravity = 0x106;
  battle_sound(SOUND_INDEXED, 0);
  liu_kang_entry_retail_script_004e86a8(fighter);
}
