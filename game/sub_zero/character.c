#include "character.h"

static void script_thrown_return(Fighter *fighter);
static void script_lp_getup(Fighter *fighter);
static void script_ready(Fighter *fighter);
#include "generated/scripts.inc"
#include "generated/frames.inc"

/* Retail attack records (docs/scorpion/combat-core.md section 2). */
// clang-format off: Keep table entries on one row, including long rows.
static const AttackRecord uppercut_record = {0xe3d7, -0xfd70, 0x947a, 0x10518, 34, 34, 6, 0, 0x28f5};     /* 0x4ebd50, A selector 2 */
static const AttackRecord uppercut_alt_record = {0x12106, -0x111a9, 0xfa9f, 0x17a9f, 34, 34, 6, 0, 0xa3d}; /* 0x4ebd68, A selector 6 */
static const AttackRecord kick_record = {0x116c8, -0xde76, 0x1476c, 0x12395, 16, 13, 6, 0, 0x2147};       /* 0x4ebaf8, B selector 2 */
static const AttackRecord sweep_record = {0x175c2, 0x7a9f, 0x10ccc, 0x8a3d, 18, 18, 4, ATTACK_LOW, 0x23d7}; /* 0x4ebb58, B selector 35 */
// clang-format on

#define COUNT(array) ((int)(sizeof array / sizeof array[0]))

/* Recovered moves query retail records from their events (SF_RECORD). The
 * remaining moves keep the approximate damage, reach and push columns. */
static const MoveDef s_idle = {CLIP_ASSET_IDLE, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_walk = {267, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_crouch = {259, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_jump = {304, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
#define RETAIL SF_ATTACK | SF_LOCK | SF_RECORD
static const MoveDef s_jab = {.clip = 294, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_010B};
static const MoveDef s_kick = {.clip = 71, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0100};
static const MoveDef s_sweep = {.clip = 63, .last = MOVE_END, .flags = RETAIL | SF_LOW, .action = ACTION_RETAIL_010C};
static const MoveDef s_uppercut = {.clip = 293, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0109};
static const MoveDef s_throw = {.clip = 76, .last = 48, .flags = SF_LOCK | SF_RECORD | SF_UNBLOCKABLE, .action = ACTION_RETAIL_0500};
static const MoveDef s_block = {322, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_hit = {247, 0, MOVE_END, 0, 0, 24, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_HOLD, NULL, 0};
static const MoveDef s_knockdown = {186, 0, MOVE_END, 0, 0, 54, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_WAIT_LAND, NULL, 0};
static const MoveDef s_victory = {115, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};

// clang-format off: Keep table entries on one row, including long rows.
static const MoveDef *const moves[S_COUNT] = {
  [S_IDLE] = &s_idle,
  [S_WALK] = &s_walk,
  [S_CROUCH] = &s_crouch,
  [S_JUMP] = &s_jump,
  [S_PUNCH] = &s_jab,
  [S_KICK] = &s_kick,
  [S_SWEEP] = &s_sweep,
  [S_UPPERCUT] = &s_uppercut,
  [S_THROW] = &s_throw,
  [S_BLOCK] = &s_block,
  [S_HIT] = &s_hit,
  [S_KNOCKDOWN] = &s_knockdown,
  [S_VICTORY] = &s_victory,
};
// clang-format on

static const int uppercut_contact_frames_samples[] = {5, 5, 6, 7, 8};
// clang-format off: Keep table entries on one row, including long rows.
static const mframe_t uppercut_contact_frames[5] = {
  [1] = {.thinkfunc = uppercut_contact},
  [2] = {.thinkfunc = uppercut_contact},
  [3] = {.thinkfunc = uppercut_contact},
};
// clang-format on
static const mmove_t uppercut_contact_phase = {
    .firstframe = 0, .lastframe = 4, .frame = uppercut_contact_frames, .endfunc = uppercut_branch, .samples = uppercut_contact_frames_samples};

static void armed_idle(Fighter *fighter);
static void armed_walk(Fighter *fighter);
static void armed_crouch(Fighter *fighter);
static void armed_block(Fighter *fighter);

static const FighterFn enter[S_COUNT] = {
    [S_IDLE] = armed_idle,
    [S_WALK] = armed_walk,
    [S_CROUCH] = armed_crouch,
    [S_BLOCK] = armed_block,
    [S_PUNCH] = sub_zero_jab,
    [S_KICK] = sub_zero_kick,
    [S_SWEEP] = sub_zero_sweep,
    [S_UPPERCUT] = sub_zero_uppercut,
};

/* Ground scripts and their native tails: 4e6da0, 4e5fd8,
 * 4e6158/46b740, 46b7f0, 4e61e0/46bf10, 4e69c8/46c2c0,
 * 4e6190/46bb60 and 4e6844/46f220. Times are our script ticks. */
enum {
  G_LOW_KICK,
  G_CROUCH_LP,
  G_CROUCH_LK,
  G_CROUCH_HK,
  G_CLOSE_HP,
  G_COMBO_HP,
  G_CLOSE_HK,
  G_ROUNDHOUSE
};

// clang-format off: Keep table entries on one row, including long rows.
static const GroundAttack ground_attacks[] = {
  {{.clip = 70, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0102}, {0x15c28, -0x35c2, 0x104df, 0x5c28, 17, 10, 6, 0, 0x1c28}, 11, 30, 32},
  {{.clip = 260, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0107}, {65536, -3932, 42532, 20971, 29, 10, 4, 0, 2621}, 8, 19, 22},
  {{.clip = 315, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0106}, {81264, 34734, 67502, 32768, 28, 10, 4, 0, 3932}, 9, 19, 22},
  {{.clip = 52, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0104}, {0xa8f5, -0xd70a, 0xa3d7, 0xb851, 26, 26, 7, 0, 0x1999}, 7, 31, 31},
  {{.clip = 262, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0108}, {65536, -37945, 57016, 51707, 43, 10, 3, 0, 2621}, 7, 21, 27},
  {{.clip = 532, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0108}, {65536, -37945, 57016, 51707, 43, 10, 3, 0, 2621}, 11, 29, 32},
  {{.clip = 314, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0103}, {60293, -11796, 43909, 72024, 27, 10, 3, 0, 6553}, 4, 25, 28},
  {{.clip = 313, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_010D}, {69468, -43909, 61603, 27525, 20, 20, 5, 0, 11796}, 16, 33, 61},
};
// clang-format on
static const ButtonTable close_hp_followup, combo_hp_followup, close_hk_followup;
static const AttackRecord hp_record = {51118, -45809, 38666, 28180, 32, 11, 1, 0, 3276};
static const MoveDef s_hp = {.clip = 296, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_010A};
static const MoveDef s_hp_second = {.clip = 297, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_010A};

static const NativeGroundProfile ground_profile = {
    ground_attacks, {&close_hp_followup, &combo_hp_followup, &close_hk_followup}, sub_zero_ready, 5, 0, 28, 21, 22, 38, NULL};

static const NativeHPProfile hp_profile = {&hp_record, &s_hp, &s_hp_second, sub_zero_ready};

static void enter_throw(Fighter *fighter);
static void enter_lk_throw(Fighter *fighter);

/* Air scripts 0x4e6658, 0x4e6710, 0x4e61f8, 0x4e6500 and 0x4e63d8.
 * Each extends, queries while holding its strike pose, then retracts. The
 * native contact continuations freeze the body and optionally open a cancel. */

// clang-format off: Keep table entries on one row, including long rows.
static const AirAttack air_attacks[] = {
  {{.clip = 97, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0603}, {67502, -39976, 56360, 43253, 38, 10, 143, 0, 5898}, 6, 10, 9},
  {{.clip = 96, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0604}, {76677, -20316, 68157, 62914, 39, 39, 4, 0, 7864}, 10, 10, 11},
  {{.clip = 327, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0600}, {67502, -18350, 58982, 43253, 21, 10, 8, 0, 8519}, 8, 4, 8},
  {{.clip = 56, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0602}, {71434, -4587, 66846, 24903, 23, 23, 4, ATTACK_MARK, 6553}, 6, 10, 6},
  {{.clip = 59, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0601}, {55705, -2621, 51773, 38010, 22, 22, 4, ATTACK_MARK, 9830}, 6, 10, 10},
};
// clang-format on

enum {
  AIR_FLIGHT,
  AIR_COMBO,
  AIR_FREEZE,
  AIR_COMBO_FREEZE,
  AIR_FALL
};

static const ButtonTable air_followup;
static const ButtonTable air_ground_followup;

static const NativeAirProfile air_profile = {air_attacks, &air_followup, &air_ground_followup, air_land};

static const ButtonTable air_followup = {
    {[BUTTON_LOW_PUNCH] = press_air_lp, [BUTTON_HIGH_KICK] = press_air_followup_kick, [BUTTON_LOW_KICK] = press_air_followup_kick}};

static const ButtonTable air_ground_followup = {
    {[BUTTON_HIGH_PUNCH] = press_air_ground_hp, [BUTTON_LOW_PUNCH] = press_lp, [BUTTON_HIGH_KICK] = press_kick, [BUTTON_LOW_KICK] = press_lk}};

static const ButtonTable close_hp_followup = {
    {[BUTTON_HIGH_PUNCH] = press_combo_hp, [BUTTON_LOW_PUNCH] = press_lp, [BUTTON_HIGH_KICK] = press_kick, [BUTTON_LOW_KICK] = press_lk}};
static const ButtonTable combo_hp_followup = {
    {[BUTTON_HIGH_PUNCH] = press_jab, [BUTTON_LOW_PUNCH] = press_lp, [BUTTON_HIGH_KICK] = press_kick, [BUTTON_LOW_KICK] = press_lk}};
static const ButtonTable close_hk_followup = {
    {[BUTTON_HIGH_PUNCH] = press_knee_hp, [BUTTON_LOW_PUNCH] = press_lp, [BUTTON_HIGH_KICK] = press_combo_hk, [BUTTON_LOW_KICK] = press_lk}};

/* LP: roster script 4de798[4] -> 4de6b8/4de898; reaction 102 is
 * 485b70 -> 4e9bd0, then reverse getup 488b80/4fbcdc.
 * LK: 4ddfb8[4] -> 42c180 -> 4de490/4de4b8/4de4e0;
 * 42c300 releases victim through 4871e0/4e9ee0/487220.
 * Pair alignment uses our shared world origin, not retail task/group pointers. */
static const MoveDef s_lk_throw = {.clip = 41, .last = 74, .flags = SF_LOCK | SF_RECORD | SF_UNBLOCKABLE, .action = ACTION_RETAIL_0501};
static const MoveDef s_lp_thrown = {.clip = 153, .last = 25, .flags = SF_LOCK, .action = ACTION_RETAIL_4003};
static const MoveDef s_lk_thrown = {.clip = 165, .last = 46, .flags = SF_LOCK, .action = ACTION_RETAIL_4003};
static const MoveDef s_lk_throw_idle = {.clip = CLIP_ASSET_IDLE, .last = MOVE_END, .flags = SF_LOCK, .action = ACTION_RETAIL_0501};
static const MoveDef s_lk_thrown_rise = {.clip = 200, .last = 40, .flags = SF_LOCK, .action = ACTION_RETAIL_4003};
static const MoveDef s_lp_thrown_getup = {.clip = 328, .last = 14, .flags = SF_LOCK, .action = ACTION_RETAIL_200E};

enum {
  LP_THROW_RELEASE = 25,
  LK_THROW_DISARM = 39, /* Opening 10 + paired 29. */
  LK_THROW_VOICE = 44,  /* Another paired 5. */
  LK_THROW_DAMAGE = 52, /* Another paired 8. */
  LK_THROW_RELEASE = 57 /* Final paired 5 before the independent victim tail. */
};

/* Native 488b80: reversed getup clip 328, final sample held for one tick. */
// clang-format off: Keep table entries on one row, including long rows.
static const mframe_t lp_getup_frames[15] = {{0}};
// clang-format on
static const mmove_t lp_getup_phase = {.firstframe = 0, .lastframe = 14, .frame = lp_getup_frames, .endfunc = lp_getup_done};

// clang-format off: Keep table entries on one row, including long rows.
static const mframe_t lk_thrown_release_phase_frames[5] = {{0}};
// clang-format on
static const mmove_t lk_thrown_release_phase = {.firstframe = 42, .lastframe = 46, .frame = lk_thrown_release_phase_frames, .endfunc = lk_thrown_release};

static const int lk_owner_idle_frames_samples[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static const mframe_t lk_owner_idle_frames[60] = {{0}};
static const mmove_t lk_owner_idle_phase = {
    .firstframe = 0, .lastframe = 59, .frame = lk_owner_idle_frames, .endfunc = sub_zero_ready, .samples = lk_owner_idle_frames_samples};

// clang-format off: Keep table entries on one row, including long rows.
static const mframe_t lk_rise_frames[41] = {{0}};
// clang-format on
static const mmove_t lk_rise = {.firstframe = 0, .lastframe = 40, .frame = lk_rise_frames, .endfunc = thrown_finish};

// clang-format off: Keep table entries on one row, including long rows.
static const mframe_t lp_capture_frames[1] = {
  [0] = {.thinkfunc = lp_capture_frames_frame_0},
};
// clang-format on
static const mmove_t lp_capture_phase = {.firstframe = 0, .lastframe = 0, .frame = lp_capture_frames, .endfunc = lp_capture_done};

/* Freeze: 49bc60 -> 4ed588/4ed818 -> 49bcb0. Flight is an independent
 * task, so it survives owner interruption; bounds and contact own teardown. */
static const AttackRecord ice_record = {0x570a, -0x1eb8, 0xa147, 0x3ae1, 67, 67, 0, ATTACK_MARK, 0};
static const MoveDef s_ice = {.clip = 16, .last = 35, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0304};
/* 49c300 creates rig 506a48 (mesh 74); 49bcfe installs 4ed898:
 * shared meshes 75..82, one tick each, looping without the spawn rig.
 * The high bit invokes 40d720 for the separately managed ice trail. */
static const MeshAnimation ice_meshes = {.first = 75, .count = 8, .period = 1, .scale = Q16_ONE};
/* 40d720/40d920, 4d1b18/28/34: ten independent trail nodes,
 * ten-tick track, quarter scale shrinking by .02 each tick.
 * The supplied EXE's 405430 gate always succeeds. */
static const ParticleVisual ice_trail_visual = {.animation = {339, 1, 1, 0x4000}, .billboard = true, .additive = true, .shrink = 0x51e};
static const ParticleSpray ice_trail = {.visual = &ice_trail_visual,
    .count = 10,
    .lifetime = 10,
    .velocity = {0, -0x7ae, 0},
    .spread = {0, 0x28f, 0},
    .offset = {0x1000, 0xc00, 0},
    .position_spread = {0x6666, 0x6666, 0x6666}};

/* 47f4d0 copies the current pose before the retreat. Both ice predicates
 * inspect clone task types 45/46, not the projectile task type 42. */
static const AttackRecord clone_record = {0x4ccc, -0xa3d, 0xbae1, 0x9e76, 67, 67, 0, ATTACK_MARK, 0};
static const MoveDef s_clone = {.clip = 301, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0405};
static const MoveDef s_clone_near = {.clip = 301, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0406};
static const OrbitLightDef clone_light = {25 * Q16_ONE, -25 * Q16_ONE, 0x18000, 0xccc};

// clang-format off: Keep table entries on one row, including long rows.
static const mframe_t clone_land_frames[1] = {
  [0] = {.thinkfunc = clone_land_frames_frame_0},
};
// clang-format on
static const mmove_t clone_land_phase = {.firstframe = 0, .lastframe = 0, .frame = clone_land_frames, .endfunc = clone_done};

// clang-format off: Keep table entries on one row, including long rows.
static const int clone_reverse_frames_samples[] = {8, 7, 6, 5, 4, 3, 2, 1};
static const mframe_t clone_reverse_frames[8] = {{0}};
// clang-format on
static const mmove_t clone_reverse = {
    .firstframe = 0, .lastframe = 7, .frame = clone_reverse_frames, .endfunc = clone_land, .samples = clone_reverse_frames_samples};

// clang-format off: Keep table entries on one row, including long rows.
static const mframe_t clone_apex_frames[1] = {
  [0] = {.thinkfunc = clone_apex_frames_frame_0},
};
// clang-format on
static const mmove_t clone_apex = {.firstframe = 8, .lastframe = 8, .frame = clone_apex_frames, .endfunc = clone_turn};

// clang-format off: Keep table entries on one row, including long rows.
static const mframe_t clone_extend_frames[5] = {{0}};
// clang-format on
static const mmove_t clone_extend = {.firstframe = 3, .lastframe = 7, .frame = clone_extend_frames, .endfunc = clone_apex_wait};

// clang-format off: Keep table entries on one row, including long rows.
static const mframe_t clone_start_frames[3] = {{0}};
// clang-format on
static const mmove_t clone_start = {.firstframe = 0, .lastframe = 2, .frame = clone_start_frames, .endfunc = clone_spin};

/* Slide: 47dc00/4e84d8 -> 47dc40; hit 4e84f8, block 4e8530.
 * Only an unblocked contact stamps cooldown slot 2 (47de8e). */
static const AttackRecord slide_record = {0xf0a3, 0x947a, 0x111eb, 0x8f5c, 122, 122, 165, 0, 0x147a};
static const MoveDef s_slide = {.clip = 95, .last = 10, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0204};

// clang-format off: Keep table entries on one row, including long rows.
static const int slide_contact_frames_samples[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9};
/* Native 47dc40 queries contact on simulation frames 9 through 25. */
static const mframe_t slide_contact_frames[25] = {
  [8] = {.thinkfunc = slide_contact},
  [9] = {.thinkfunc = slide_contact},
  [10] = {.thinkfunc = slide_contact},
  [11] = {.thinkfunc = slide_contact},
  [12] = {.thinkfunc = slide_contact},
  [13] = {.thinkfunc = slide_contact},
  [14] = {.thinkfunc = slide_contact},
  [15] = {.thinkfunc = slide_contact},
  [16] = {.thinkfunc = slide_contact},
  [17] = {.thinkfunc = slide_contact},
  [18] = {.thinkfunc = slide_contact},
  [19] = {.thinkfunc = slide_contact},
  [20] = {.thinkfunc = slide_contact},
  [21] = {.thinkfunc = slide_contact},
  [22] = {.thinkfunc = slide_contact},
  [23] = {.thinkfunc = slide_contact},
  [24] = {.thinkfunc = slide_contact},
};
// clang-format on
static const mmove_t slide_contact_phase = {
    .firstframe = 0, .lastframe = 24, .frame = slide_contact_frames, .endfunc = script_slide_return, .samples = slide_contact_frames_samples};

/* Shared weapon draw 4e62c0/4e62d8, air 4e66d0/4e66f0 and
 * throw 498290. Scepter ground strikes use character-4 descriptor 4ecdb8. */
/* Scepter descriptor 4ecdb8. Parameter blocks 4ec458/490/480/4b8
 * select the shared native attack runner and its recovery continuations. */
// clang-format off: Keep table entries on one row, including long rows.
static const WeaponStrike scepter_attacks[] = {
  {{.clip = 231, .last = 44, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0111}, {128385, -35323, 92274, 22872, 51, 51, 119, 0, 11796}, 6, 10, 16, 4, 1, 0, 0, 0, 0x20},
  {{.clip = 250, .last = 31, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0111}, {115277, 20316, 73400, 51052, 150, 150, 118, ATTACK_LOW, 9830}, 0, 4, 12, 2, 9, 0, 0, 0, 0x22},
  {{.clip = 237, .last = 44, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0112}, {99614, -49152, 64880, 55050, 52, 52, 119, 0, 9830}, 0, 9, 16, 4, 0, 9, 8, 0x147a, 0x22},
  {{.clip = 228, .last = 39, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0113}, {123076, -26804, 91684, 14352, 151, 151, 118, 0, 0}, 0, 10, 16, 5, 5, 1, 14, 0x2666, 0x22},
};
// clang-format on

static const WeaponDef scepter;

static const WeaponDef scepter = {0x6b, scepter_hp, scepter_lp, scepter_back_hp, scepter_back_lp, NULL, weapon_throw, equip_scepter, SCENE_MODEL_SCEPTER};

/* Generated VM finishers use shared scene, blend and object helpers. */
#define FATAL_MOVE(c, last_sample) {.clip = c, .last = last_sample, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0700}
// clang-format off: Keep table entries on one row, including long rows.
static const MoveDef fatal_moves[] = {
  FATAL_MOVE(13, 55), FATAL_MOVE(385, 140), FATAL_MOVE(387, 73), FATAL_MOVE(552, 104),
  FATAL_MOVE(95, 10), FATAL_MOVE(258, 38), FATAL_MOVE(293, 19), FATAL_MOVE(128, 109),
  FATAL_MOVE(115, 18), FATAL_MOVE(116, 49), FATAL_MOVE(117, 18), FATAL_MOVE(118, 49),
  FATAL_MOVE(415, 129), FATAL_MOVE(416, 107), FATAL_MOVE(412, 48),
};
// clang-format on
#undef FATAL_MOVE

// clang-format off: Keep table entries on one row, including long rows.
/* These records are native blend operations. VM playback is generated. */
#define BLEND(c, sample, step, first) {{.clip = c, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0700}, sample, sample, 1, step, first}
static const SceneClip first_actor_blend[] = {BLEND(385, 0, 0x32f1, 0x3333)};
static const SceneClip first_victim_setup[] = {BLEND(389, 0, 0x32f1, 0x3333), BLEND(387, 0, 0x32f1, 0x3333)};
static const SceneClip first_victim_blends[] = {BLEND(387, 46, 0x666, 0x666), BLEND(387, 49, 0x43f, 0x43f), BLEND(387, 50, 0x43f, 0x886)};
#undef BLEND
// clang-format on

/* 445920: camera records 4e0f00/4e0f58, reordered from X,Z,Y. */
// clang-format off: Keep table entries on one row, including long rows.
static const q16 first_camera[][3] = {
  {131072, -131072, -196608},
  {131072, -131072, -196608},
  {131072, -131072, -196608},
  {131072, -131072, -196608},
  {131072, -131072, -196608},
  {131072, -131072, -196608},
  {131072, -131072, -196608},
};
static const q16 orbit_camera[][3] = {
  {-327680, -163840, -458752},
  {-393216, -183500, -393216},
  {-393216, -255590, 393216},
  {393216, -216268, 393216},
  {393216, -190054, -393216},
  {-458752, -170393, -458752},
  {-524288, -157286, 524288},
  {589824, -144179, 589824},
  {655360, -117964, -655360},
  {-655360, -104857, -655360},
  {-655360, -104857, 655360},
  {655360, -104857, 655360},
  {655360, -117964, -655360},
  {-655360, -131072, -655360},
  {-655360, -144179, 655360},
  {655360, -150732, 655360},
  {655360, -157286, -655360},
  {-655360, -157286, -655360},
  {-655360, -157286, 655360},
  {655360, -157286, 655360},
  {655360, -157286, -655360},
  {-655360, -157286, -655360},
  {-655360, -157286, 655360},
  {655360, -157286, 655360},
  {655360, -157286, -655360},
};
static const CameraShot first_camera_shots[] = {
  {first_camera, COUNT(first_camera), 50, 0xccc, 0, 0, 0},
  {orbit_camera, COUNT(orbit_camera), 0, 0x45a, 3, 80, -0x4000},
};
/* 445740/446620 and 4e0440: independent head/spine playback. */
static const AttachmentDef first_spine = {.model = SCENE_MODEL_SPINE, .clip = 386, .first = 0, .last = 96, .loop_from = 52,
    .group_offset = {0, -0xccc / 65536.f, 0}, .piece_offset = {0, 0x2b43 / 65536.f, 0}, .piece_kind = 0, .scene_space = true};
// clang-format on

/* 445c40: eleven double sprays, eleven singles, thirteen 2/1-tick
 * pairs, then seventy-one singles two ticks apart. 445e20 aims from
 * joint 2 away from joint 3; 474330 selects the shared blood mesh. */
static const ParticleVisual neck_blood_visual = {.animation = {0, 1, 1, Q16_ONE}};
static const ParticleSpray neck_blood = {
    .visual = &neck_blood_visual, .count = 1, .lifetime = 180, .gravity = 0x51e, .spread = {0x7ae, 0x7ae, 0x7ae}, .splash = true};
// clang-format off: Keep table entries on one row, including long rows.
static const EmissionPhase neck_blood_phases[] = {
  {11, 1, -1, 2},
  {11, 1, -1, 1},
  {39, 3, 2, 1},
  {142, 2, -1, 1},
};
// clang-format on
static const JointSprayDef first_blood = {.spray = &neck_blood,
    .phases = neck_blood_phases,
    .count = COUNT(neck_blood_phases),
    .origin_joint = 2,
    .base_joint = 3,
    .speed = 0x23d7,
    .variation = 0x7ae};

// clang-format off: Keep table entries on one row, including long rows.
static const mframe_t breath_slide_phase_frames[9] = {{0}};
// clang-format on
static const mmove_t breath_slide_phase = {.firstframe = 0, .lastframe = 8, .frame = breath_slide_phase_frames, .endfunc = breath_travel};

// clang-format off: Keep table entries on one row, including long rows.
static const mframe_t kick_contact_frames[3] = {
  [0] = {.thinkfunc = kick_contact},
  [1] = {.thinkfunc = kick_contact},
  [2] = {.thinkfunc = kick_contact},
};
// clang-format on
static const mmove_t kick_contact_phase = {.firstframe = 10, .lastframe = 12, .frame = kick_contact_frames, .endfunc = retail_script_004e6998};

/* 46f080/46f130: two native contact visits, then remaining clip playback.
 * Keep the established portable timing; source-script startup is generated. */
// clang-format off: Keep table entries on one row, including long rows.
static const mframe_t sweep_recovery_frames[24] = {{0}};
// clang-format on
static const mmove_t sweep_recovery = {.firstframe = 22, .lastframe = 45, .frame = sweep_recovery_frames, .endfunc = sub_zero_ready};

static const int sweep_contact_frames_samples[] = {22, 22};
// clang-format off: Keep table entries on one row, including long rows.
static const mframe_t sweep_contact_frames[2] = {
  [0] = {.thinkfunc = sub_zero_sweep_contact},
  [1] = {.thinkfunc = sub_zero_sweep_contact},
};
// clang-format on
static const mmove_t sweep_contact = {
    .firstframe = 0, .lastframe = 1, .frame = sweep_contact_frames, .endfunc = sweep_recover, .samples = sweep_contact_frames_samples};

// clang-format off: Keep table entries on one row, including long rows.
static const int slide_return_frames_samples[] = {10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
static const mframe_t slide_return_frames[11] = {{0}};
// clang-format on
static const mmove_t slide_return_phase = {
    .firstframe = 0, .lastframe = 10, .frame = slide_return_frames, .endfunc = sub_zero_ready, .samples = slide_return_frames_samples};

static const MoveDef s_air_getup = {.clip = 131, .first = 46, .last = 79, .flags = SF_LOCK, .action = ACTION_RETAIL_200E};
// clang-format off: Keep table entries on one row, including long rows.
static const mframe_t air_getup_frames[34] = {{0}};
// clang-format on
static const mmove_t air_getup = {.firstframe = 46, .lastframe = 79, .frame = air_getup_frames, .endfunc = fighter_ready};

// clang-format off: Keep table entries on one row, including long rows.
static const CommandDef commands[] = {
  {BUTTON_HIGH_PUNCH, {TOKEN_FORWARD, TOKEN_BACK, TOKEN_FORWARD, TOKEN_DOWN}, 4, 60, CMD_BLOCK_OK, can_first_fatality, sub_zero_first_fatality},
  {BUTTON_HIGH_PUNCH, {TOKEN_BACK, TOKEN_BACK, TOKEN_DOWN, TOKEN_BACK}, 4, 50, 0, can_second_fatality, sub_zero_second_fatality},
  {BUTTON_LOW_PUNCH, {0}, 0, 0, 0, can_slide, sub_zero_slide},
  {BUTTON_LOW_PUNCH, {TOKEN_DOWN, TOKEN_FORWARD}, 2, 26, 0, can_ice, sub_zero_ice},
  {BUTTON_LOW_PUNCH, {TOKEN_DOWN, TOKEN_BACK}, 2, 18, CMD_AIR, can_clone, sub_zero_clone},
  {BUTTON_BLOCK, {0}, 0, 0, 0, can_slide, sub_zero_slide},
  {BUTTON_BLOCK, {TOKEN_FORWARD, TOKEN_BACK, TOKEN_FORWARD, TOKEN_DOWN}, 4, 60, CMD_BLOCK_OK, can_first_fatality, sub_zero_first_fatality},
  /* Requested extension: retail Sub-Zero has no 4e4a04 air-throw record.
   * Keep his slide/fatality priority, then reuse the shared retail throw. */
  {BUTTON_BLOCK, {0}, 0, 0, CMD_AIR | CMD_BLOCK_OK, mk4_can_air_throw, sub_zero_air_throw},
  {BUTTON_HIGH_KICK, {TOKEN_DOWN, TOKEN_UP, TOKEN_UP, TOKEN_UP}, 4, 48, 0, can_fan_fatality, sub_zero_fan_fatality},
  {BUTTON_LOW_KICK, {0}, 0, 0, 0, can_slide, sub_zero_slide},
  {BUTTON_LOW_KICK, {TOKEN_DOWN, TOKEN_DOWN, TOKEN_DOWN}, 3, 40, 0, can_spike_fatality, sub_zero_spike_fatality},
  {BUTTON_HIGH_KICK, {TOKEN_DOWN, TOKEN_FORWARD}, 2, 24, 0, can_weapon, sub_zero_weapon},
  {BUTTON_RUN, {0}, 0, 0, 0, can_sidestep, sub_zero_sidestep},
  {BUTTON_RUN, {0}, 0, 0, 0, can_pickup, sub_zero_pickup},
  {BUTTON_RUN, {TOKEN_FORWARD, TOKEN_BACK, TOKEN_FORWARD, TOKEN_DOWN}, 4, 60, CMD_BLOCK_OK, can_first_fatality, sub_zero_first_fatality},
};
// clang-format on

static const ButtonTable standing = {
    {[BUTTON_HIGH_PUNCH] = press_jab, [BUTTON_LOW_PUNCH] = press_lp, [BUTTON_HIGH_KICK] = press_kick, [BUTTON_LOW_KICK] = press_lk}};
static const ButtonTable crouching = {
    {[BUTTON_HIGH_PUNCH] = press_uppercut, [BUTTON_LOW_PUNCH] = press_crouch_lp, [BUTTON_HIGH_KICK] = press_crouch_hk, [BUTTON_LOW_KICK] = press_crouch_lk}};
static const ButtonTable jumping = {
    {[BUTTON_HIGH_PUNCH] = press_air_hp, [BUTTON_LOW_PUNCH] = press_air_lp, [BUTTON_HIGH_KICK] = press_air_hk, [BUTTON_LOW_KICK] = press_air_lk}};
static const Controls controls = {commands, COUNT(commands), {&standing, &crouching, &jumping}};

static const CpuMoves cpu_moves = {.enter = {
                                       [CPU_JAB] = sub_zero_jab,
                                       [CPU_LOW_KICK] = sub_zero_lk,
                                       [CPU_HIGH_KICK] = sub_zero_hk,
                                       [CPU_SWEEP] = enter_sweep,
                                       [CPU_UPPERCUT] = enter_uppercut,
                                       [CPU_THROW] = enter_throw,
                                       [CPU_CLOSE_HP] = cpu_close_hp,
                                       [CPU_CLOSE_HK] = cpu_close_hk,
                                       [CPU_CROUCH_LP] = sub_zero_crouch_lp,
                                       [CPU_AIR_HP] = air_hp,
                                       [CPU_AIR_HK] = air_hk_diagonal,
                                       [CPU_AIR_LK] = air_lk_diagonal,
                                       [CPU_PROJECTILE] = sub_zero_ice,
                                       [CPU_SPECIAL_1] = sub_zero_clone,
                                       [CPU_SPECIAL_2] = sub_zero_slide,
                                       [CPU_DRAW] = sub_zero_weapon,
                                       [CPU_FINISH_1] = sub_zero_first_fatality,
                                       [CPU_FINISH_2] = sub_zero_second_fatality,
                                       [CPU_FINISH_FAN] = sub_zero_fan_fatality,
                                       [CPU_FINISH_SPIKE] = sub_zero_spike_fatality,
                                   }};

const CharacterDef mk4_sub_zero = {GAME_MK4, 4, "SUB ZERO", &mk4_motion, moves, S_COUNT, &controls, enter, -77660, mk4_walk, mk4_jump, &cpu_moves};

/* Private move implementations share this character's data and declarations. */
#include "moves/air_attacks.inc"
#include "moves/clone.inc"
#include "moves/common.inc"
#include "moves/controls.inc"
#include "moves/fan.inc"
#include "moves/fatality.inc"
#include "moves/fatality_breath.inc"
#include "moves/fatality_first.inc"
#include "moves/fatality_second.inc"
#include "moves/ground_attacks.inc"
#include "moves/kicks.inc"
#include "moves/projectile.inc"
#include "moves/punches.inc"
#include "moves/roundhouse.inc"
#include "moves/sidestep.inc"
#include "moves/slide.inc"
#include "moves/spike.inc"
#include "moves/sweep.inc"
#include "moves/throws.inc"
#include "moves/uppercut.inc"
#include "moves/weapon.inc"
