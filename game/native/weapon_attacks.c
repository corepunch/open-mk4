#include "native_services.h"
#include "retail_layout.h"
#include <math.h>
#include <stdlib.h>

/* Named byte addresses of native attack records, checked against the local EXE.
 * Decode the original data through the VM; do not keep copied balance tables. */
enum {
  BOLT_ATTACK_RECORD = 0x4ebea0,
  FLURRY_ATTACK_RECORD = 0x4eb7e0,
  SWEEP_ATTACK_RECORD = 0x4eb7f8,
  BOOMERANG_ATTACK_RECORD = 0x4ebde0
};

/* Native service cluster: 482790. */
static const MoveDef n482790_upper = {.clip = 226, .last = 59, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0113};

static void n482790_weapon_cleanup(Fighter *fighter) {
  native_motion_stop(fighter);
  mk4_script_stop_body(fighter);
}

void retail_weapon_native_495860(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &n482790_upper);
  fighter->mmove_cleanup = n482790_weapon_cleanup;
  native_schedule_speed(fighter, 0x1999, 7, 13);
  retail_vm_enter(fighter, 0x4ec4f0, fighter_ready);
}

/* Native service cluster: 49a480. */
void retail_weapon_native_495c30(Fighter *fighter);
void retail_weapon_native_495c60(Fighter *fighter);
void retail_weapon_native_495cc0(Fighter *fighter);
static void n49a480_bow_fire(Fighter *fighter);
void retail_weapon_native_495cf0(Fighter *fighter);

static const MoveDef n49a480_bow_move = {.clip = 211, .last = 28, .flags = SF_LOCK | SF_RECORD, .action = 0x303};
static const MoveDef n49a480_bolt_victim = {.clip = 311, .last = 18, .flags = SF_LOCK | SF_REACTION, .action = ACTION_RETAIL_4001};

/* Native4ed438 selects506dc8, whose sole mesh is shared106. */
static const MeshAnimation n49a480_bolt_mesh = {.first = 106, .count = 1, .period = 1, .scale = Q16_ONE, .source = MESH_SHARED};

static void n49a480_bow_cleanup(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter->body.on_land = NULL;
}

static void n49a480_bolt_step(Projectile *bolt) {
  AttackRecord n49a480_bolt_record = retail_vm_attack_record(retail_vm_image(), BOLT_ATTACK_RECORD);

  if (g_battle.contacts_off) {
    bolt->kind = 0;
    return;
  }
  for (int axis = 0; axis < 3; ++axis) {
    bolt->pos[axis] += bolt->vel[axis];
  }
  /* Native49a110 reflects angled bolts at absolute Y=-3 and Y=0. */
  if (bolt->phase != 0 && (bolt->pos[AXIS_Y] < -3 * Q16_ONE || bolt->pos[AXIS_Y] >= 0)) {
    bolt->pos[AXIS_Y] = bolt->pos[AXIS_Y] < -3 * Q16_ONE ? -3 * Q16_ONE : 0;
    bolt->vel[AXIS_Y] = -bolt->vel[AXIS_Y];
    bolt->angle[2] = -bolt->angle[2];
  }
  int64_t x = bolt->pos[AXIS_X], z = bolt->pos[AXIS_Z], radius = g_battle.arena_radius;
  if (x * x + z * z > radius * radius + (int64_t)12 * Q16_ONE * Q16_ONE) {
    bolt->kind = 0;
    return;
  }
  if (battle_projectile_contact(bolt, &n49a480_bolt_record) != CONTACT_MISS) {
    battle_sound(SOUND_C, 0x43);
    bolt->kind = 0;
  }
}

static void n49a480_bow_spawn(Fighter *fighter) {
  Projectile *bolt = battle_spawn_projectile(fighter, PROJECTILE_FIREBALL, 180, n49a480_bolt_step);
  if (bolt == NULL) {
    return;
  }
  int variant = fighter->native_variant;
  bolt->phase = variant == 1 || variant == 2 ? variant : 0;
  bolt->mesh_animation = &n49a480_bolt_mesh;
  bolt->initial_mesh = n49a480_bolt_mesh.first;
  bolt->heading[0] = fighter->saved_motion[0];
  bolt->heading[1] = fighter->saved_motion[1];
  bolt->pos[AXIS_Y] += -0x3333;
  bolt->vel[AXIS_Y] = variant == 1 ? -0x3333 : variant == 2 ? 0x3333 : 0;
  /* 49c370 adds a quarter turn to the native actor heading. In the
   * portable +X projectile basis this points shared106's -X nose forward.
   * Its pitch changes sign after that local-to-heading axis conversion. */
  bolt->angle[1] = 0x3243f;
  bolt->angle[2] = variant == 1 ? -Q16_ONE : variant == 2 ? Q16_ONE : 0;
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    int component = axis == AXIS_X ? 0 : 1;
    bolt->pos[axis] += q16_mul(bolt->heading[component], 0xccc);
    bolt->vel[axis] = q16_mul(bolt->heading[component], bolt->phase == 0 ? 0x3333 : 0x2666);
  }
}

/* Native429220 advances two poses per visit;428d30 advances to the
 * requested cursor from zero. These are native bodies, not VM transcriptions. */
static void n49a480_bow_native_to(Fighter *fighter, int target, int step, FighterFn end) {
  int first = fighter_sample(fighter), distance = target - first;
  require(distance >= 0 && target <= fighter_move(fighter)->last && (step == 1 || step == 2), "Invalid Fujin crossbow playback range");
  int visits = (distance + step - 1) / step;
  native_begin(fighter, 0, visits, end);
  for (int row = 0; row <= visits; ++row) {
    int sample = first + row * step;
    native_frame(fighter, row, sample < target ? sample : target, NULL);
  }
  native_play(fighter, false);
}

static void n49a480_bow_return(Fighter *fighter) {
  fighter_finish_mmove(fighter, mk4_mmove_thrown_finish);
}

void fujin_bow_finish(Fighter *fighter) {
  fighter->action = 0x1016;
  n49a480_bow_native_to(fighter, fighter_move(fighter)->last, 1, n49a480_bow_return);
}

static void n49a480_bow_fire(Fighter *fighter) {
  n49a480_bow_spawn(fighter);
  if (fighter->native_variant >= 3) {
    retail_vm_enter(fighter, 0x4ed788, fujin_bow_followup_next);
  } else if (fighter->native_variant == 0) {
    fighter->action = 0x1016;
    retail_vm_enter(fighter, 0x4ed408, fighter_ready);
  } else {
    fujin_bow_finish(fighter);
  }
}

static void n49a480_bow_curved_clip(Fighter *fighter) {
  int clip = fighter->native_variant == 1 ? 213 : 215;
  mk4_mmove_bind(fighter, clip, 30, 0);
  n49a480_bow_native_to(fighter, 4, 1, n49a480_bow_fire);
}

static void n49a480_bow_start(Fighter *fighter, int variant) {
  mk4_script_stop_body(fighter);
  fighter_start_move(fighter, S_PUNCH, &n49a480_bow_move);
  fighter->body.physics = fighter->body.on_land = NULL;
  fighter->mmove_cleanup = n49a480_bow_cleanup;
  fighter->native_variant = variant;
  fighter->saved_motion[0] = fighter_axis(fighter, AXIS_X);
  fighter->saved_motion[1] = fighter_axis(fighter, AXIS_Z);
  fighter->action = variant == 1 ? 0x301 : variant == 2 ? 0x302 : 0x303;
  battle_sound(SOUND_C, 0x29);
  if (variant == 1 || variant == 2) {
    mk4_mmove_bind(fighter, variant == 1 ? 212 : 214, 10, 0);
    n49a480_bow_native_to(fighter, 9, 2, n49a480_bow_curved_clip);
  } else {
    mk4_mmove_bind(fighter, 211, 29, 0);
    n49a480_bow_native_to(fighter, 4, 1, n49a480_bow_fire);
  }
}

void retail_weapon_native_495c30(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    retail_air_high_punch(fighter);
  } else {
    n49a480_bow_start(fighter, 1);
  }
}

void retail_weapon_native_495cc0(Fighter *fighter) {
  n49a480_bow_start(fighter, 2);
}

void retail_weapon_native_495c60(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    retail_air_low_punch(fighter);
  } else if (!(fighter->flags & FF_FOLLOW_UP) && mk4_can_ground_throw(fighter, false)) {
    fujin_enter_throw(fighter);
  } else {
    n49a480_bow_start(fighter, fighter_holds_back(fighter) ? 2 : 0);
  }
}

/* Native4ecb40[type68-60] ->495cf0 ->4ed828[25] ->49a190:
 * three short shots, then49a3b0's full ordinary LP shot and recovery. */
void fujin_bow_followup_next(Fighter *fighter) {
  int next = fighter->native_variant + 1;
  if (next < 6) {
    n49a480_bow_start(fighter, next);
  } else {
    n49a480_bow_start(fighter, 0);
  }
}

void retail_weapon_native_495cf0(Fighter *fighter) {
  fighter_upright(fighter);
  fighter->body.pos[AXIS_Y] = fighter->body.ground;
  n49a480_bow_start(fighter, 3);
}

void fujin_bolt_victim(Fighter *fighter) {
  mk4_script_disarm_self(fighter);
  fighter_start_move(fighter, S_HIT, &n49a480_bolt_victim);
  fighter->flags |= FF_REACTING;
  fighter->mmove_cleanup = n49a480_bow_cleanup;
  retail_vm_enter(fighter, 0x4e9e10, fighter_ready);
}

/* Native service cluster: native_weapon. */

/* 495980 has the throw/back gates but no air selector. The ordinary LP
 * callback handles air first; follow-up table4ecb40 enters this ground body. */

/* Native service cluster: 482790. */
void retail_weapon_native_495a90(Fighter *fighter);
static void n482790_2_weapon_cleanup(Fighter *fighter);
static const MoveDef n482790_2_back_move = {.clip = 226, .last = 59, .flags = SF_LOCK | SF_RECORD, .action = 0x10e};

static void n482790_2_weapon_cleanup(Fighter *fighter) {
  native_motion_stop(fighter);
  mk4_script_stop_body(fighter);
}

void retail_weapon_native_495a90(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &n482790_2_back_move);
  fighter->mmove_cleanup = n482790_2_weapon_cleanup;
  /* 495a90 /4ec598 installs delayed travel before the selected VM stream. */
  native_schedule_speed(fighter, 0x1999, 7, 13);
  retail_vm_enter(fighter, 0x4ec5a8, mk4_mmove_thrown_finish);
}

/* 4ecb40[type63-60] selects4959f0 ->4eca08/4eca18. These use
 * the HP strike data directly, bypassing ordinary air/back input selection. */

/* Native service cluster: 46db00, 496430. */
#define RETAIL SF_ATTACK | SF_LOCK | SF_RECORD
static void n46db00_sword_travel(Fighter *fighter, int delay, int duration);
static void n46db00_sword_cleanup(Fighter *fighter);
static bool n46db00_sword_repeat(Fighter *fighter);
static void n46db00_sword_blend_weight(Fighter *fighter, float weight);
static void n46db00_sword_blend(Fighter *fighter, int ticks, FighterFn end);
static void n46db00_sword_finish(Fighter *fighter);
static void n46db00_sword_settle(Fighter *fighter);
static void n46db00_sword_hit_blend(Fighter *fighter);
static void n46db00_sword_recover(Fighter *fighter);
static void n46db00_sword_contact(Fighter *fighter);
static void n46db00_sword_swing(Fighter *fighter);
static void n46db00_sword_strike_script(Fighter *fighter);
static void n46db00_sword_attack(Fighter *fighter, int index);
void retail_weapon_native_4963e0(Fighter *fighter);
void retail_weapon_native_4964d0(Fighter *fighter);
void retail_weapon_native_496670(Fighter *fighter);
void retail_weapon_native_496920(Fighter *fighter);
static const MoveDef n46db00_s_sword_hp = {.clip = 233, .last = 44, .flags = RETAIL, .action = ACTION_RETAIL_021C};

static void n46db00_sword_travel(Fighter *fighter, int delay, int duration) {
  q16 speed = fighter->weapon_strike == NULL ? 0x11eb : fighter->weapon_strike->speed;
  fighter->saved_motion[0] = speed;
  native_schedule_speed(fighter, speed, delay, duration);
}

static void n46db00_sword_cleanup(Fighter *fighter) {
  if (fighter->body.physics == native_motion_step) {
    native_motion_stop(fighter);
    fighter_set_speed(fighter, 0);
  }
}

static bool n46db00_sword_repeat(Fighter *fighter) {
  bool repeat = (fighter->buffered & BUTTON_BIT(BUTTON_HIGH_PUNCH)) != 0;
  fighter->buffered &= ~BUTTON_BIT(BUTTON_HIGH_PUNCH);
  return repeat;
}

static void n46db00_sword_blend_weight(Fighter *fighter, float weight) {
  fighter->blend_ticks = (int)weight;
}

static void n46db00_sword_blend(Fighter *fighter, int ticks, FighterFn end) {
  fighter->blend_clip = fighter_move(fighter)->clip;
  fighter->blend_sample = fighter_sample(fighter);
  fighter->blend_ticks = 0;
  fighter->blend_duration = ticks;
  native_begin(fighter, 1, ticks, end);
  for (int frame = 1; frame <= ticks; ++frame) {
    native_frame(fighter, frame, fighter->blend_sample, NULL);
    fighter->native.frames[frame - 1].aifunc = n46db00_sword_blend_weight;
    fighter->native.frames[frame - 1].dist = (float)frame;
  }
  native_play(fighter, false);
}

static void n46db00_sword_finish(Fighter *fighter) {
  fighter_finish_mmove(fighter, fighter_ready);
}

static void n46db00_sword_settle(Fighter *fighter) {
  native_hold(fighter, 1, n46db00_sword_finish);
}

static void n46db00_sword_hit_blend(Fighter *fighter) {
  n46db00_sword_blend(fighter, 10, n46db00_sword_settle); /* 494e10: fixed armed stance, step 0x1999. */
}

/* script_phase selects the strike. A hit LP repeats into the second strike;
 * otherwise the clip completes, with the back-HP hit blend, before return. */
static void n46db00_sword_recover(Fighter *fighter) {
  int index = fighter->script_phase;
  const WeaponStrike *strike = fighter->weapon_strike;
  bool hit = fighter->connected;
  fighter->action = ACTION_RETAIL_1006;
  if (index == 0 && hit && n46db00_sword_repeat(fighter)) {
    /* 496500 -> 4ec720/730, delayed travel then second strike. The move
     * rebind keeps the action's cleanup for the second strike. */
    fighter->mmove_cleanup = NULL;
    fighter->loaded_weapon_strike = weapon_retail_strike(0x4ec730, 0x4ec720, ACTION_RETAIL_010E);
    fighter_start_move(fighter, S_PUNCH, &fighter->loaded_weapon_strike.move);
    fighter->mmove_cleanup = n46db00_sword_cleanup;
    fighter->script_phase = 1;
    fighter->weapon_strike = &fighter->loaded_weapon_strike;
    fighter->sample_override = fighter->loaded_weapon_strike.first;
    n46db00_sword_travel(fighter, fighter->loaded_weapon_strike.delay, fighter->loaded_weapon_strike.travel);
    n46db00_sword_strike_script(fighter);
    return;
  }
  fighter_set_speed(fighter, 0);
  fighter->body.physics = NULL;
  if (index == 2 && hit) {
    native_play_rows(fighter, strike->move.last, 1, n46db00_sword_hit_blend);
  } else {
    native_finish_clip(fighter, n46db00_sword_finish);
  }
}

static void n46db00_sword_contact(Fighter *fighter) {
  const WeaponStrike *strike = fighter->weapon_strike;
  native_query_rows(fighter, &strike->record, strike->queries, 1, n46db00_sword_recover);
}

static void n46db00_sword_swing(Fighter *fighter) {
  const WeaponStrike *strike = fighter->weapon_strike;
  battle_sound(SOUND_C, strike->sound);
  native_play_rows(fighter, strike->target, 1, n46db00_sword_contact);
}

static void n46db00_sword_strike_script(Fighter *fighter) {
  const WeaponStrike *strike = fighter->weapon_strike;
  int opening = strike->windup - strike->first;
  if (opening <= 0) {
    n46db00_sword_swing(fighter);
    return;
  }
  int pose = fighter_sample(fighter), last = fighter_move(fighter)->last;
  native_begin(fighter, 1, opening, n46db00_sword_swing);
  for (int frame = 1; frame <= opening; ++frame) {
    native_frame(fighter, frame, last == MOVE_END ? pose + frame : mini(last, pose + frame), NULL);
  }
  native_play(fighter, false);
}

static void n46db00_sword_attack(Fighter *fighter, int index) {
  if (fighter_airborne(fighter)) {
    retail_air_attack(fighter, index == 2 ? 0 : 1);
    return;
  }
  if (index == 3) {
    static const MoveDef back_lp = {.clip = -1, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0112};
    fighter_start_move(fighter, S_PUNCH, &back_lp);
    const Mk4Image *image = retail_vm_image();
    native_schedule_speed(fighter, (q16)mk4_image_u32(image, 0x4ec7e4), maxi(0, (int)mk4_image_u32(image, 0x4ec7e0) - 1), (int)mk4_image_u32(image, 0x4ec7e8));
    fighter->mmove_cleanup = n46db00_sword_cleanup;
    retail_vm_enter(fighter, 0x4ec7f0, fighter_ready);
    return;
  }
  require(index >= 0 && index <= 2, "Invalid native weapon strike selection");
  if (index == 0) {
    fighter->loaded_weapon_strike = weapon_retail_strike(0x4ec708, 0, ACTION_RETAIL_010E);
  } else if (index == 1) {
    fighter->loaded_weapon_strike = weapon_retail_strike(0x4ec730, 0x4ec720, ACTION_RETAIL_010E);
  } else {
    fighter->loaded_weapon_strike = weapon_retail_strike(0x4ec750, 0x4ec740, ACTION_RETAIL_0212);
  }
  fighter_start_move(fighter, S_PUNCH, &fighter->loaded_weapon_strike.move);
  fighter->script_phase = index;
  fighter->weapon_strike = &fighter->loaded_weapon_strike;
  fighter->sample_override = fighter->weapon_strike->first;
  if (fighter->weapon_strike->travel != 0) {
    n46db00_sword_travel(fighter, fighter->weapon_strike->delay, fighter->weapon_strike->travel);
  }
  fighter->mmove_cleanup = n46db00_sword_cleanup;
  n46db00_sword_strike_script(fighter);
}

void retail_weapon_native_4963e0(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    retail_air_high_punch(fighter);
    return;
  }
  fighter_start_move(fighter, S_PUNCH, &n46db00_s_sword_hp);
  fighter->weapon_strike = NULL;
  fighter->sample_override = 0;
  fighter->mmove_cleanup = n46db00_sword_cleanup;
  retail_vm_enter(fighter, 0x4ec680, fighter_ready);
}

void retail_weapon_native_4964d0(Fighter *fighter) {
  n46db00_sword_attack(fighter, 0);
}

void retail_weapon_native_496670(Fighter *fighter) {
  n46db00_sword_attack(fighter, 2);
}

void retail_weapon_native_496920(Fighter *fighter) {
  n46db00_sword_attack(fighter, 3);
}

unsigned liu_kang_liu_script_sword_gate(Fighter *fighter) {
  return n46db00_sword_repeat(fighter) ? 0u : 4u;
}

void liu_kang_liu_script_sword_blend(Fighter *fighter) {
  n46db00_sword_blend(fighter, 5, n46db00_sword_finish);
}

#undef RETAIL

/* Native service cluster: native_weapon_2. */
/* Button table 4ecc48: HP 495780, LP 496ba0. 495780 tests Back (48ebd0) and
 * strikes 4ec9d8, otherwise 496a70 travels (4ec828) into 4ec838; 496ba0
 * gives 46dbb0's close LP throw priority, then 494990 strikes 4ec848 or
 * dispatches its Back variant 496be0 (travel 4ec860, strike 4ec870).
 * 495780 leaves the action unchanged; the portable back-HP strike uses the
 * same 112 action written by the weapon's other strikes. */
/* Descriptor 4ecf38: type 6f, rig 50c310 (share2_g.geo mesh 1). */

/* 46d260: equip after the draw clip reaches its grip sample, then 4e62d8. */

/* 46d1d0: an armed fighter throws (45f5f0); a follow-up equips through
 * 494610 and table 4ecb40[6f - 60] = 496be0; otherwise draw 4e62c0. */

/* Native service cluster: 482790, 488590. */
void raiden_script_weapon_victim_getup(Fighter *victim) {
  native_finish_clip(victim, retail_getup_flip);
}

void raiden_script_staff_victim_tail(Fighter *victim) {
  victim->body.physics = fighter_friction;
  native_hold(victim, 20, raiden_script_weapon_victim_getup);
}

/* Native service cluster: native_weapon_3. */
void retail_weapon_native_496d10(Fighter *fighter);
static void native_weapon_3_weapon_first_victim(Fighter *fighter);
static void native_weapon_3_weapon_second_victim(Fighter *fighter);
static const NativeAxeProfile native_weapon_3_axe_profile = {native_weapon_3_weapon_first_victim, native_weapon_3_weapon_second_victim};

void retail_weapon_native_496d10(Fighter *fighter) {
  weapon_axe_strike(fighter, 3, &native_weapon_3_axe_profile);
}

static void native_weapon_3_weapon_first_victim(Fighter *fighter) {
  weapon_axe_reaction(fighter, false, (RetailScriptContinuation){0x4ea040, mk4_script_thrown_ready});
}

static void native_weapon_3_weapon_second_victim(Fighter *fighter) {
  weapon_axe_reaction(fighter, true, (RetailScriptContinuation){0x4ea0b8, mk4_script_thrown_ready});
}

/* Native service cluster: 482280, 482790, 495db0. */
/* Descriptor 4ece78: type 67, shared_g rig 506d90 and button table
 * 4ecd68 = HP 495d40, LP 496100 and the shared stance handlers. */
void retail_weapon_native_495d70(Fighter *fighter);
void retail_weapon_native_496140(Fighter *fighter);

/* 495db0 queries WEAPON[20]; 496140 queries WEAPON[19]. */

static const MoveDef n482280_flurry_start = {.clip = 248, .last = 12, .flags = SF_LOCK | SF_RECORD, .action = 0x404};
static const MoveDef n482280_flurry_cycle = {.clip = 249, .last = 11, .flags = SF_LOCK | SF_RECORD, .action = 0x404};
static const MoveDef n482280_back_low = {.clip = 233, .last = 44, .flags = SF_LOCK | SF_RECORD, .action = 0x10e};

static void n482280_weapon_cleanup(Fighter *fighter) {
  native_motion_stop(fighter);
  mk4_script_stop_body(fighter);
}

static void n482280_flurry_cycle_start(Fighter *fighter);

/* 496007: advance one sample, then 428d10 plays out the current clip
 * before the next 495ec6 cycle. */
static void n482280_flurry_playout(Fighter *fighter) {
  int sample = fighter_sample(fighter), last = fighter_move(fighter)->last;
  int rows = maxi(1, last - sample);
  native_begin(fighter, 1, rows, n482280_flurry_cycle_start);
  for (int row = 1; row <= rows; ++row) {
    native_frame(fighter, row, mini(last, sample + row), NULL);
  }
  native_play(fighter, false);
}

static void n482280_flurry_friction(Fighter *fighter) {
  fighter->action = 0x100f;
  fighter->body.vel[AXIS_X] = q16_mul(fighter->body.vel[AXIS_X], 0xe666);
  fighter->body.vel[AXIS_Z] = q16_mul(fighter->body.vel[AXIS_Z], 0xe666);
}

/* 495f88: speed -[4ec654] = -0x3333, then 496080 holds eight visits with
 * action 100f and 0xe666 friction before 496007. */
static void n482280_flurry_recoil(Fighter *fighter) {
  fighter_set_speed(fighter, -0x3333);
  int pose = fighter_sample(fighter);
  native_begin(fighter, 1, 8, n482280_flurry_playout);
  for (int row = 1; row <= 8; ++row) {
    native_frame(fighter, row, pose, n482280_flurry_friction);
  }
  native_play(fighter, false);
}

static bool n482280_holds_forward(const Fighter *fighter) {
  bool facing_left = fighter_opponent(fighter)->body.pos[AXIS_X] < fighter->body.pos[AXIS_X];
  return (fighter->held & BUTTON_BIT(facing_left ? BUTTON_LEFT : BUTTON_RIGHT)) != 0;
}

static void n482280_flurry_ready(Fighter *fighter) {
  fighter->action = ACTION_RETAIL_1006;
  mk4_return_to_stance(fighter);
}

/* 495e30: friction, 496d20 steers 0x1999 toward a held forward or back
 * direction, and a held HP (45f3e0 raw switches) keeps the flurry. */
static void n482280_flurry_continue(Fighter *fighter) {
  fighter->body.vel[AXIS_X] = q16_mul(fighter->body.vel[AXIS_X], 0xe666);
  fighter->body.vel[AXIS_Z] = q16_mul(fighter->body.vel[AXIS_Z], 0xe666);
  if (fighter_holds_back(fighter)) {
    fighter_set_speed(fighter, -0x1999);
  } else if (n482280_holds_forward(fighter)) {
    fighter_set_speed(fighter, 0x1999);
  }
  if (fighter->held & BUTTON_BIT(BUTTON_HIGH_PUNCH)) {
    n482280_flurry_playout(fighter);
  } else {
    fighter_finish_mmove(fighter, n482280_flurry_ready);
  }
}

static void n482280_flurry_query(Fighter *fighter) {
  AttackRecord n482280_flurry_record = retail_vm_attack_record(retail_vm_image(), FLURRY_ATTACK_RECORD);

  /* Every 495de8 query is a fresh WEAPON[20] contact for its cycle. */
  ContactResult result = battle_contact(fighter, &n482280_flurry_record);
  fighter->connected = false;
  if (result == CONTACT_BLOCK || (result != CONTACT_MISS && fighter_opponent(fighter)->hits >= 3)) {
    n482280_flurry_recoil(fighter);
  } else {
    n482280_flurry_continue(fighter);
  }
}

/* 495ec6: C[0x1f], 428750 binds 53d578 (clip 249) and 428d30 plays to 6,
 * then 495de8 queries WEAPON[20]. */
static void n482280_flurry_cycle_start(Fighter *fighter) {
  battle_sound(SOUND_C, 0x1f);
  fighter_start_move(fighter, S_PUNCH, &n482280_flurry_cycle);
  fighter->mmove_cleanup = n482280_weapon_cleanup;
  fighter->sample_override = 0;
  native_begin(fighter, 1, 6, NULL);
  for (int row = 1; row <= 6; ++row) {
    native_frame(fighter, row, row, row == 6 ? n482280_flurry_query : NULL);
  }
  native_play(fighter, false);
}

/* 495db0 state 0 at 248/8: a blocked query recoils; anything else plays
 * out 248 into the first 249 cycle. */
void reptile_weapon_flurry(Fighter *fighter) {
  AttackRecord n482280_flurry_record = retail_vm_attack_record(retail_vm_image(), FLURRY_ATTACK_RECORD);

  require(fighter_move(fighter)->clip == 248 && fighter_sample(fighter) == 8, "Reptile weapon flurry needs its generated 248 startup");
  ContactResult result = battle_contact(fighter, &n482280_flurry_record);
  fighter->connected = false;
  if (result == CONTACT_BLOCK) {
    n482280_flurry_recoil(fighter);
  } else {
    n482280_flurry_playout(fighter);
  }
}

void retail_weapon_native_495d70(Fighter *fighter) {
  /* 495d70: action 404, attack id 314 and VM 4ec640 (clip 248 to 8). */
  fighter_start_move(fighter, S_PUNCH, &n482280_flurry_start);
  fighter->mmove_cleanup = n482280_weapon_cleanup;
  fighter->sample_override = 0;
  retail_vm_enter(fighter, 0x4ec640, fighter_ready);
}

static void n482280_back_low_recover(Fighter *fighter) {
  /* 496840: action 1006, 428d10 plays out and 46f650 returns. */
  fighter->action = ACTION_RETAIL_1006;
  int sample = fighter_sample(fighter), last = n482280_back_low.last;
  native_begin(fighter, 1, last - sample, mk4_return_to_stance);
  for (int row = 1; row <= last - sample; ++row) {
    native_frame(fighter, row, sample + row, NULL);
  }
  native_play(fighter, false);
}

static void n482280_back_low_query(Fighter *fighter) {
  AttackRecord n482280_sweep_record = retail_vm_attack_record(retail_vm_image(), SWEEP_ATTACK_RECORD);

  if (!fighter->connected && battle_contact(fighter, &n482280_sweep_record) != CONTACT_MISS) {
    n482280_back_low_recover(fighter);
  }
}

/* 496140: action 10e, 4947d0 schedules record 4ec670 (delay 1, 0x3333 for
 * seven visits), 53d57c binds clip 233; 428d30 plays to 4, C[0x1f] and
 * 429230 advances two samples per visit to 13, 428d70 to 16, then four
 * advancing WEAPON[19] queries before 496840. */
static void n482280_back_low_cue(Fighter *fighter) {
  (void)fighter;
  battle_sound(SOUND_C, 0x1f);
}

void retail_weapon_native_496140(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &n482280_back_low);
  fighter->mmove_cleanup = n482280_weapon_cleanup;
  fighter->sample_override = 0;
  native_schedule_speed(fighter, 0x3333, 0, 7);
  static const int poses[] = {1, 2, 3, 4, 6, 8, 10, 12, 13, 13, 14, 15, 16, 16, 17, 18, 19, 20};
  int rows = (int)(sizeof poses / sizeof poses[0]);
  native_begin(fighter, 1, rows, n482280_back_low_recover);
  for (int row = 1; row <= rows; ++row) {
    native_frame(fighter, row, poses[row - 1], row == 4 ? n482280_back_low_cue : row > rows - 4 ? n482280_back_low_query : NULL);
  }
  native_play(fighter, false);
}

/* 46d1d0: armed throws (2000d), a follow-up draw (494610) equips, reacts
 * the peer and runs 4ecb40[7] = 495d20, the 4eca28 HP strike; otherwise
 * action 1004 and VM 4e62c0. */

/* Weapon victims: 96 (4821c0: disarm, VM 4e8bc0), 137 (482270: VM 4e8c98
 * and 482280) and 144 (482300: damage scale 0x8000, wall recoil, disarm,
 * VM 4e8cc0 and 482790). */

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

/* Native service cluster: 470420. */
#define SWORD_MOVE(name, clip_id, action_id) static const MoveDef name = {.clip = clip_id, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = action_id}

enum {
  SWORD_BACK_LP_SCRIPT = 0x4ec8b0,
  SWORD_BACK_HP_SCRIPT = 0x4ec918,
  SWORD_SPIN_SCRIPT = 0x4e8850,
  SWORD_SPIN_ATTACK_SELECTOR = 22,
  SWORD_SPIN_STEERING_SPEED = 0xf5c /* Retail Q16 0.1 * 0.6. */
};

SWORD_MOVE(s_sword_back_lp, 237, ACTION_RETAIL_0112);
SWORD_MOVE(s_sword_back_hp, 238, ACTION_RETAIL_0112);
SWORD_MOVE(s_spin_start, 223, ACTION_RETAIL_0407);

/* Standing slashes use the shared retail parameter runner. These back
 * alternatives enter the original scripts directly (496c50/496c90). */
static void sword_back_attack(Fighter *fighter, bool hp) {
  if (fighter_airborne(fighter)) {
    retail_air_attack(fighter, hp ? 0 : 1);
    return;
  }
  fighter_start_move(fighter, S_PUNCH, hp ? &s_sword_back_hp : &s_sword_back_lp);
  fighter->sample_override = 0;
  retail_vm_enter(fighter, hp ? SWORD_BACK_HP_SCRIPT : SWORD_BACK_LP_SCRIPT, fighter_ready);
}

void retail_weapon_native_496c90(Fighter *fighter) {
  sword_back_attack(fighter, true);
}

void retail_weapon_native_496c50(Fighter *fighter) {
  sword_back_attack(fighter, false);
}

static void n470420_2_sword_spin_recover(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter->script_phase = 1;
  native_hold(fighter, 12, scorpion_special_ready);
}

void scorpion_sword_spin_control(Fighter *fighter) {
  if (fighter_opponent(fighter)->hits >= 3) {
    n470420_2_sword_spin_recover(fighter);
    return;
  }
  if (!(fighter->held & BUTTON_BIT(BUTTON_LOW_PUNCH))) {
    fighter_finish_mmove(fighter, scorpion_special_ready);
    return;
  }
  /* 496d20: raw Left, then Right (which wins), steer at 0.1 * 0.6 along the
   * screen-side axis. */
  int direction = (fighter->held & BUTTON_BIT(BUTTON_RIGHT)) ? 1 : (fighter->held & BUTTON_BIT(BUTTON_LEFT)) ? -1 : 0;
  fighter_set_screen_speed(fighter, direction * SWORD_SPIN_STEERING_SPEED);
}

void scorpion_sword_spin_query(Fighter *fighter) {
  const Mk4Image *image = retail_vm_image();
  uint32_t packed = mk4_image_u32(image, RETAIL_WEAPON_ATTACK_TABLE + SWORD_SPIN_ATTACK_SELECTOR * 4);
  AttackRecord record = retail_vm_attack_record(image, packed * 4);
  if (battle_contact(fighter, &record) == CONTACT_BLOCK) {
    n470420_2_sword_spin_recover(fighter);
  } else {
    scorpion_sword_spin_control(fighter);
  }
}

void retail_weapon_native_480770(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &s_spin_start);
  fighter->sample_override = 0;
  fighter->mmove_cleanup = mk4_script_stop_body;
  retail_vm_enter(fighter, SWORD_SPIN_SCRIPT, fighter_ready);
}

#undef SWORD_MOVE

/* Native service cluster: 483090, 4830a0. */

void shinnok_weapon_sweep_getup(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  if (fighter->health <= 0) {
    fighter_finish_mmove(fighter, NULL);
    return;
  }
  retail_vm_enter(fighter, 0x4e9498, mk4_mmove_thrown_finish);
}

/* 4830a0/483160 resumes clip197 at zero and finishes before controller
 * return. The portable runner shares the same bounded phase for both slots. */
void shinnok_weapon_sweep_finish(Fighter *fighter) {
  int first = fighter_sample(fighter), last = fighter_move(fighter)->last;
  native_begin(fighter, first, last, mk4_mmove_thrown_finish);
  for (int frame = first; frame <= last; ++frame) {
    native_frame(fighter, frame, frame, NULL);
  }
  native_play(fighter, false);
}

/* Native service cluster: 482790, 488c10. */
#define RETAIL SF_ATTACK | SF_LOCK | SF_RECORD
void retail_weapon_native_496720(Fighter *fighter);
static const MoveDef n482790_4_s_weapon_back_hp = {.clip = 228, .last = 39, .flags = RETAIL, .action = ACTION_RETAIL_0112};

void retail_weapon_native_496720(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &n482790_4_s_weapon_back_hp);
  mk4_script_schedule_motion(fighter, 0x1999, 3, 12);
  fighter->mmove_cleanup = sonya_special_cleanup;
  retail_vm_enter(fighter, 0x4ec7a0, fighter_ready);
}

#undef RETAIL

/* Native service cluster: 497b60. */
void retail_weapon_native_495610(Fighter *fighter);

/* 4955b0 descriptor 4ec420 and its held-back alternate 4955e0 (travel
 * 4ec438, strike 4ec448): clip, first/windup/target/queries/recovery,
 * WEAPON selector 42/43 and sound 0x20. */
/* T2[0x16]/[0x17] 497ae0/497ad0 -> 497af0: action 316 and VM 4ed190. */
static const MoveDef n497b60_boomerang_throw = {.clip = 229, .last = 44, .flags = SF_LOCK | SF_RECORD, .action = 0x316};

/* 497c50 waiter descriptor 4ed1a8 selects C[20] 4ebde0. */

static const MoveDef n497b60_victim_hit = {.clip = 130, .last = 39, .flags = SF_LOCK | SF_REACTION, .action = ACTION_RETAIL_4001};
static const MoveDef n497b60_victim_fall = {.clip = 182, .last = 64, .flags = SF_LOCK | SF_REACTION, .action = ACTION_RETAIL_4003};

enum {
  n497b60_BOOMERANG_OUT,
  n497b60_BOOMERANG_BACK,
  n497b60_BOOMERANG_HOME
};

/* 494990 takes descriptor word 3 (4955e0) when back is held. */

/* 497e20: one counter across all phases plays sound C 0x5a on the first
 * visit and every tenth visit after it. */
static void n497b60_boomerang_whoosh(Projectile *projectile) {
  if (projectile->ticks % 10 == 1) {
    battle_sound(SOUND_C, 0x5a);
  }
}

/* 493c20 turns the free rig by its stored spin. */
static void n497b60_boomerang_turn(Projectile *projectile) {
  for (int axis = 0; axis < 3; ++axis) {
    projectile->pos[axis] += projectile->vel[axis];
    projectile->angle[axis] = (projectile->angle[axis] + projectile->spin[axis]) % 0x6487e;
  }
  n497b60_boomerang_whoosh(projectile);
}

/* 497e60 state 0: reflect at -.2 horizontally, rise .2, for eight visits. */
static void n497b60_boomerang_bounce(Projectile *projectile) {
  projectile->phase = n497b60_BOOMERANG_BACK;
  projectile->bounce = 8;
  projectile->vel[AXIS_X] = q16_mul(projectile->vel[AXIS_X], -0x3333);
  projectile->vel[AXIS_Z] = q16_mul(projectile->vel[AXIS_Z], -0x3333);
  projectile->vel[AXIS_Y] = -0x3333;
}

/* 497e60's catch branch: an unarmed, free, grounded owner is interrupted
 * by 470420 and re-equipped through 46a180; otherwise 48a2c0 picks list
 * 4ea918 (tokens f5/8c) and 494590 settles the free weapon. */
static void n497b60_boomerang_arrive(Projectile *projectile) {
  Fighter *owner = &g_battle.fighters[projectile->owner];
  memset(projectile->vel, 0, sizeof projectile->vel);
  if (!owner->armed && !(owner->flags & (FF_BUSY | FF_REACTING)) && !fighter_airborne(owner) && owner->health > 0 && g_battle.phase == PHASE_FIGHT) {
    fighter_equip_weapon(owner, projectile->weapon);
    battle_sound(SOUND_C, projectile->owner == 0 ? 0x26 : 0x27);
    projectile->kind = 0;
    fighter_interrupt(owner, fighter_ready);
    return;
  }
  battle_sound(SOUND_B, battle_random(2) == 0 ? 0x72 : 0x3d);
  memset(projectile->spin, 0, sizeof projectile->spin);
  weapon_object_settle(projectile);
}

/* 490e60 steers .1 per visit toward the owner; below -1.5 it descends at
 * .05. Within squared distance .5 it reaches the owner's hand. */
static void n497b60_boomerang_home(Projectile *projectile) {
  const Fighter *owner = &g_battle.fighters[projectile->owner];
  double dx = (double)owner->body.pos[AXIS_X] - projectile->pos[AXIS_X];
  double dz = (double)owner->body.pos[AXIS_Z] - projectile->pos[AXIS_Z];
  double length = hypot(dx, dz);
  projectile->vel[AXIS_X] = length > 0 ? (q16)(dx / length * 0x1999) : 0;
  projectile->vel[AXIS_Z] = length > 0 ? (q16)(dz / length * 0x1999) : 0;
  projectile->vel[AXIS_Y] = projectile->pos[AXIS_Y] <= -0x18000 ? 0xccc : 0;
  q16 d[3] = {
      owner->body.pos[AXIS_X] - projectile->pos[AXIS_X], owner->body.pos[AXIS_Y] - projectile->pos[AXIS_Y], owner->body.pos[AXIS_Z] - projectile->pos[AXIS_Z]};
  if (q16_mul(d[0], d[0]) + q16_mul(d[1], d[1]) + q16_mul(d[2], d[2]) <= 0x8000) {
    n497b60_boomerang_arrive(projectile);
  }
}

/* 497c50/497de0: the outward flight probes C[20] through 49c690, which
 * skips a victim in action 2001, and turns back on contact or outside
 * the arena radius squared plus twelve. */
static void n497b60_boomerang_step(Projectile *projectile) {
  AttackRecord n497b60_boomerang_record = retail_vm_attack_record(retail_vm_image(), BOOMERANG_ATTACK_RECORD);

  if (projectile->phase == n497b60_BOOMERANG_HOME) {
    n497b60_boomerang_home(projectile);
    if (projectile->step == n497b60_boomerang_step && projectile->kind != 0) {
      n497b60_boomerang_turn(projectile);
    }
    return;
  }
  n497b60_boomerang_turn(projectile);
  if (projectile->phase == n497b60_BOOMERANG_BACK) {
    if (--projectile->bounce <= 0) {
      projectile->phase = n497b60_BOOMERANG_HOME;
    }
    return;
  }
  const Fighter *victim = fighter_opponent(&g_battle.fighters[projectile->owner]);
  if (!g_battle.contacts_off && victim->action != ACTION_RETAIL_2001 && battle_projectile_contact(projectile, &n497b60_boomerang_record) != CONTACT_MISS) {
    n497b60_boomerang_bounce(projectile);
    return;
  }
  int64_t x = projectile->pos[AXIS_X], z = projectile->pos[AXIS_Z];
  int64_t radius = g_battle.arena_radius;
  if (x * x + z * z > radius * radius + (int64_t)12 * Q16_ONE * Q16_ONE) {
    n497b60_boomerang_bounce(projectile);
  }
}

static void n497b60_boomerang_ready(Fighter *fighter) {
  fighter_finish_mmove(fighter, fighter_ready);
}

/* 497b60 at sample 7: 498480 detaches the weapon, task 51 497c50 places it
 * .6 along the heading with speed .24 (the held-back variant also rises
 * 0xf5c), then 428cf0 finishes clip 229. */
void tanya_boomerang_release(Fighter *fighter) {
  Projectile *projectile = weapon_release_object(fighter, 600, n497b60_boomerang_step);
  if (projectile != NULL) {
    projectile->phase = n497b60_BOOMERANG_OUT;
    for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
      q16 heading = fighter_axis(fighter, axis);
      projectile->pos[axis] = fighter->body.pos[axis] + q16_mul(heading, 0x9999);
      projectile->vel[axis] = q16_mul(heading, 0x3d70);
    }
    projectile->vel[AXIS_Y] = fighter->native_variant ? -0xf5c : 0;
  }
  native_begin(fighter, 8, 44, n497b60_boomerang_ready);
  for (int pose = 8; pose <= 44; ++pose) {
    native_frame(fighter, pose, pose, NULL);
  }
  native_play(fighter, false);
}

static void n497b60_boomerang_start(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &n497b60_boomerang_throw);
  fighter->native_variant = fighter_holds_back(fighter);
  retail_vm_enter(fighter, 0x4ed190, fighter_ready);
}

/* 495610: 46dbb0's LP throw gate first, then T2[0x16]/[0x17]. */
void retail_weapon_native_495610(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    retail_air_low_punch(fighter);
  } else if (mk4_can_ground_throw(fighter, false)) {
    fighter_enter_state(fighter, S_THROW);
  } else {
    n497b60_boomerang_start(fighter);
  }
}

/* 46d1d0: armed -> T2[0xd] throw; follow-up window -> 494610 equips, sets
 * up the victim and 4ecb40[0x6d-0x60] = 4955b0; otherwise draw 4e62c0. */

/* 490bd0 as a group callback keeps the falling victim facing its peer. */

/* Reactions 169 (486710 -> 4e9db8) and 170 (4821a0 -> 4e8bb0) both disarm
 * through 494520 before their generated victim streams. */
void tanya_weapon_reaction(Fighter *fighter) {
  require(retail_reaction_native(fighter) == 0x486710 || retail_reaction_native(fighter) == 0x4821a0, "Invalid Tanya weapon victim reaction");
  mk4_script_disarm_self(fighter);
  if (retail_reaction_native(fighter) == 0x486710) {
    fighter_start_move(fighter, S_HIT, &n497b60_victim_hit);
    fighter->flags |= FF_REACTING;
    retail_vm_enter(fighter, 0x4e9db8, mk4_mmove_thrown_finish);
  } else {
    fighter_start_move(fighter, S_KNOCKDOWN, &n497b60_victim_fall);
    fighter->flags |= FF_REACTING;
    retail_vm_enter(fighter, 0x4e8bb0, mk4_mmove_thrown_finish);
  }
}
