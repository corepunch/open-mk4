#include "retail_vm.h"
#include "../character.h"
#include "math_util.h"

/* Descriptor 4ece78: type 67, shared_g rig 506d90 and button table
 * 4ecd68 = HP 495d40, LP 496100 and the shared stance handlers. */
static void weapon_hp(Fighter *fighter);
static void weapon_lp(Fighter *fighter);
static void weapon_back_hp(Fighter *fighter);
static void weapon_back_lp(Fighter *fighter);
static void equip_weapon(Fighter *fighter);
// clang-format off: Keep table entries on one row, including long rows.
/* 4ec628 (HP, also follow-up 4eca28) and 4ec658 (LP): clip, recovery,
 * queries, target and windup bytes, record selector, first sample, cue. */
static const WeaponStrike strikes[] = {
  {{.clip = 230, .last = 44, .flags = SF_LOCK | SF_RECORD, .action = 0x111}, {0x251eb, -0xf333, 0x1851e, 0x14c8b, 63, 63, 119, 0, 0x2666}, 0, 8, 10, 3, 5, 0, 0, 0, 34},
  {{.clip = 240, .last = 44, .flags = SF_LOCK | SF_RECORD, .action = 0x111}, {0x18000, -0xd47a, 0x111eb, 0x13ae1, 144, 144, 119, 0, 0x1eb8}, 0, 8, 13, 3, 6, 0, 0, 0, 34},
};
// clang-format on
/* 495db0 queries WEAPON[20]; 496140 queries WEAPON[19]. */
static const AttackRecord flurry_record = {0x16666, -0x12e14, 0x1147a, 0xe624, 137, 10, 112, 0, 0xccc};
static const AttackRecord sweep_record = {0x1cccc, -0xe666, 0x1451e, 0xb0a3, 96, 10, 119, 0, 0x2147};
static const MoveDef flurry_start = {.clip = 248, .last = 12, .flags = SF_LOCK | SF_RECORD, .action = 0x404};
static const MoveDef flurry_cycle = {.clip = 249, .last = 11, .flags = SF_LOCK | SF_RECORD, .action = 0x404};
static const MoveDef back_low = {.clip = 233, .last = 44, .flags = SF_LOCK | SF_RECORD, .action = 0x10e};
static const WeaponDef native_weapon = {
    0x67, weapon_hp, weapon_lp, weapon_back_hp, weapon_back_lp, NULL, weapon_throw, equip_weapon, SCENE_MODEL_REPTILE_WEAPON};

static void equip_weapon(Fighter *fighter) {
  fighter_equip_weapon(fighter, &native_weapon);
}

static void weapon_cleanup(Fighter *fighter) {
  native_motion_stop(fighter);
  mk4_script_stop_body(fighter);
}

static void flurry_cycle_start(Fighter *fighter);

/* 496007: advance one sample, then 428d10 plays out the current clip
 * before the next 495ec6 cycle. */
static void flurry_playout(Fighter *fighter) {
  int sample = fighter_sample(fighter), last = fighter_move(fighter)->last;
  int rows = maxi(1, last - sample);
  native_begin(fighter, 1, rows, flurry_cycle_start);
  for (int row = 1; row <= rows; ++row) {
    native_frame(fighter, row, mini(last, sample + row), NULL);
  }
  native_play(fighter, false);
}

static void flurry_friction(Fighter *fighter) {
  fighter->action = 0x100f;
  fighter->body.vel[AXIS_X] = q16_mul(fighter->body.vel[AXIS_X], 0xe666);
  fighter->body.vel[AXIS_Z] = q16_mul(fighter->body.vel[AXIS_Z], 0xe666);
}

/* 495f88: speed -[4ec654] = -0x3333, then 496080 holds eight visits with
 * action 100f and 0xe666 friction before 496007. */
static void flurry_recoil(Fighter *fighter) {
  fighter_set_speed(fighter, -0x3333);
  int pose = fighter_sample(fighter);
  native_begin(fighter, 1, 8, flurry_playout);
  for (int row = 1; row <= 8; ++row) {
    native_frame(fighter, row, pose, flurry_friction);
  }
  native_play(fighter, false);
}

static bool holds_forward(const Fighter *fighter) {
  bool facing_left = fighter_opponent(fighter)->body.pos[AXIS_X] < fighter->body.pos[AXIS_X];
  return (fighter->held & BUTTON_BIT(facing_left ? BUTTON_LEFT : BUTTON_RIGHT)) != 0;
}

static void flurry_ready(Fighter *fighter) {
  fighter->action = ACTION_RETAIL_1006;
  mk4_return_to_stance(fighter);
}

/* 495e30: friction, 496d20 steers 0x1999 toward a held forward or back
 * direction, and a held HP (45f3e0 raw switches) keeps the flurry. */
static void flurry_continue(Fighter *fighter) {
  fighter->body.vel[AXIS_X] = q16_mul(fighter->body.vel[AXIS_X], 0xe666);
  fighter->body.vel[AXIS_Z] = q16_mul(fighter->body.vel[AXIS_Z], 0xe666);
  if (fighter_holds_back(fighter)) {
    fighter_set_speed(fighter, -0x1999);
  } else if (holds_forward(fighter)) {
    fighter_set_speed(fighter, 0x1999);
  }
  if (fighter->held & BUTTON_BIT(BUTTON_HIGH_PUNCH)) {
    flurry_playout(fighter);
  } else {
    fighter_finish_mmove(fighter, flurry_ready);
  }
}

static void flurry_query(Fighter *fighter) {
  /* Every 495de8 query is a fresh WEAPON[20] contact for its cycle. */
  ContactResult result = battle_contact(fighter, &flurry_record);
  fighter->connected = false;
  if (result == CONTACT_BLOCK || (result != CONTACT_MISS && fighter_opponent(fighter)->hits >= 3)) {
    flurry_recoil(fighter);
  } else {
    flurry_continue(fighter);
  }
}

/* 495ec6: C[0x1f], 428750 binds 53d578 (clip 249) and 428d30 plays to 6,
 * then 495de8 queries WEAPON[20]. */
static void flurry_cycle_start(Fighter *fighter) {
  battle_sound(SOUND_C, 0x1f);
  fighter_start_move(fighter, S_PUNCH, &flurry_cycle);
  fighter->mmove_cleanup = weapon_cleanup;
  fighter->sample_override = 0;
  native_begin(fighter, 1, 6, NULL);
  for (int row = 1; row <= 6; ++row) {
    native_frame(fighter, row, row, row == 6 ? flurry_query : NULL);
  }
  native_play(fighter, false);
}

/* 495db0 state 0 at 248/8: a blocked query recoils; anything else plays
 * out 248 into the first 249 cycle. */
void reptile_weapon_flurry(Fighter *fighter) {
  require(fighter_move(fighter)->clip == 248 && fighter_sample(fighter) == 8, "Reptile weapon flurry needs its generated 248 startup");
  ContactResult result = battle_contact(fighter, &flurry_record);
  fighter->connected = false;
  if (result == CONTACT_BLOCK) {
    flurry_recoil(fighter);
  } else {
    flurry_playout(fighter);
  }
}

static void weapon_back_hp(Fighter *fighter) {
  /* 495d70: action 404, attack id 314 and VM 4ec640 (clip 248 to 8). */
  fighter_start_move(fighter, S_PUNCH, &flurry_start);
  fighter->mmove_cleanup = weapon_cleanup;
  fighter->sample_override = 0;
  retail_vm_enter_owned(fighter, 0x4ec640, fighter_ready, 10);
}

static void back_low_recover(Fighter *fighter) {
  /* 496840: action 1006, 428d10 plays out and 46f650 returns. */
  fighter->action = ACTION_RETAIL_1006;
  int sample = fighter_sample(fighter), last = back_low.last;
  native_begin(fighter, 1, last - sample, mk4_return_to_stance);
  for (int row = 1; row <= last - sample; ++row) {
    native_frame(fighter, row, sample + row, NULL);
  }
  native_play(fighter, false);
}

static void back_low_query(Fighter *fighter) {
  if (!fighter->connected && battle_contact(fighter, &sweep_record) != CONTACT_MISS) {
    back_low_recover(fighter);
  }
}

/* 496140: action 10e, 4947d0 schedules record 4ec670 (delay 1, 0x3333 for
 * seven visits), 53d57c binds clip 233; 428d30 plays to 4, C[0x1f] and
 * 429230 advances two samples per visit to 13, 428d70 to 16, then four
 * advancing WEAPON[19] queries before 496840. */
static void back_low_cue(Fighter *fighter) {
  (void)fighter;
  battle_sound(SOUND_C, 0x1f);
}

static void weapon_back_lp(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &back_low);
  fighter->mmove_cleanup = weapon_cleanup;
  fighter->sample_override = 0;
  native_schedule_speed(fighter, 0x3333, 0, 7);
  static const int poses[] = {1, 2, 3, 4, 6, 8, 10, 12, 13, 13, 14, 15, 16, 16, 17, 18, 19, 20};
  int rows = (int)(sizeof poses / sizeof poses[0]);
  native_begin(fighter, 1, rows, back_low_recover);
  for (int row = 1; row <= rows; ++row) {
    native_frame(fighter, row, poses[row - 1], row == 4 ? back_low_cue : row > rows - 4 ? back_low_query : NULL);
  }
  native_play(fighter, false);
}

static void weapon_hp(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    retail_air_high_punch(fighter);
  } else if (fighter_holds_back(fighter)) {
    weapon_back_hp(fighter);
  } else {
    /* 495d40: action 111 and shared strike 4ec628. */
    weapon_start_strike(fighter, &strikes[0], false);
  }
}

static void weapon_lp(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    retail_air_low_punch(fighter);
  } else if (mk4_can_ground_throw(fighter, false)) {
    /* 496100 -> 46dbb0: a close, unrestricted LP throws instead. */
    retail_lp_throw(fighter);
  } else if (fighter_holds_back(fighter)) {
    weapon_back_lp(fighter);
  } else {
    weapon_start_strike(fighter, &strikes[1], false);
  }
}

void reptile_script_draw_equip(Fighter *fighter) {
  equip_weapon(fighter);
  retail_vm_enter_owned(fighter, 0x4e62d8, fighter_ready, 10);
}

/* 46d1d0: armed throws (2000d), a follow-up draw (494610) equips, reacts
 * the peer and runs 4ecb40[7] = 495d20, the 4eca28 HP strike; otherwise
 * action 1004 and VM 4e62c0. */
void reptile_weapon_action(Fighter *fighter) {
  if (fighter->armed) {
    fighter->weapon->throw_move(fighter);
  } else if (fighter->flags & FF_FOLLOW_UP) {
    equip_weapon(fighter);
    weapon_followup_victim(fighter);
    weapon_start_strike(fighter, &strikes[0], false);
  } else {
    weapon_draw(fighter, &native_weapon);
    fighter->action = 0x1004;
    retail_vm_enter_owned(fighter, 0x4e62c0, fighter_ready, 10);
  }
}

static const MoveDef weapon_victim = {.clip = 182, .last = 64, .flags = SF_LOCK | SF_REACTION, .action = 0x4003};
static const MoveDef weapon_stagger = {.clip = 311, .last = 18, .flags = SF_LOCK | SF_REACTION, .action = 0x4001};
static const MoveDef weapon_launch = {.clip = 189, .last = 29, .flags = SF_LOCK | SF_REACTION, .action = 0x4003};

/* Weapon victims: 96 (4821c0: disarm, VM 4e8bc0), 137 (482270: VM 4e8c98
 * and 482280) and 144 (482300: damage scale 0x8000, wall recoil, disarm,
 * VM 4e8cc0 and 482790). */
void reptile_weapon_reaction(Fighter *fighter) {
  int reaction = fighter->reaction;
  require(reaction == 96 || reaction == 137 || reaction == 144, "Invalid Reptile weapon victim reaction");
  if (reaction == 144) {
    fighter->damage_scale = 0x8000;
    battle_wall_recoil(fighter);
  }
  if (reaction != 137) {
    mk4_script_disarm_self(fighter);
  }
  fighter->flags |= FF_REACTING;
  if (reaction == 96) {
    fighter_start_move(fighter, S_HIT, &weapon_victim);
    retail_vm_enter_owned(fighter, 0x4e8bc0, mk4_mmove_thrown_finish, 10);
  } else if (reaction == 137) {
    fighter_start_move(fighter, S_HIT, &weapon_stagger);
    retail_vm_enter_owned(fighter, 0x4e8c98, mk4_mmove_thrown_finish, 10);
  } else {
    fighter_start_move(fighter, S_KNOCKDOWN, &weapon_launch);
    retail_vm_enter_owned(fighter, 0x4e8cc0, mk4_mmove_thrown_finish, 10);
  }
}

/* 482280 -> 489020: replay 311 from 0 to its end, then 46f650. */
void reptile_weapon_stagger_tail(Fighter *fighter) {
  fighter->sample_override = 0;
  native_begin(fighter, 1, 19, mk4_mmove_thrown_finish);
  for (int row = 1; row <= 18; ++row) {
    native_frame(fighter, row, row, NULL);
  }
  native_frame(fighter, 19, 18, NULL);
  native_play(fighter, false);
}

void reptile_weapon_getup_back(Fighter *fighter) {
  mk4_mmove_finish_getup(fighter, 0x4ea210);
}
