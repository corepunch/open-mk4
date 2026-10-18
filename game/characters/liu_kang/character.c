#include "character.h"

#include "generated/scripts.inc"
#include "generated/frames.inc"

/* Retail attack records; source addresses identify the supplied EXE. */
// clang-format off: Keep table entries on one row, including long rows.
static const AttackRecord uppercut_record = {0xe3d7, -0xfd70, 0x947a, 0x10518, 34, 34, 6, 0, 0x28f5};     /* 0x4ebd50, A selector 2 */
static const AttackRecord uppercut_alt_record = {0x12106, -0x111a9, 0xfa9f, 0x17a9f, 34, 34, 6, 0, 0xa3d}; /* 0x4ebd68, A selector 6 */
static const AttackRecord kick_record = {0x15e76, -0xd999, 0x151a9, 0x111eb, 16, 13, 6, 0, 0x2147};       /* 0x4ebae0, B selector 1 */
static const AttackRecord sweep_record = {0x170a3, 0x8518, 0xdeb8, 0x8000, 19, 19, 4, ATTACK_LOW, 0x23d7}; /* 0x4ebb70, B selector 36 */
// clang-format on

/* 0x46ddd0: one query per strike visit, skipped against a crouching victim
 * (0x2001), actions 0x106/0x107, or an early sweep (0x10c before sample 5).
 * The 0x102 gate is the low-kick crouching interval (native 0x46a970). */

#define COUNT(array) ((int)(sizeof array / sizeof array[0]))
#define SEQUENCE(segments, count, end) {segments, COUNT(segments), NULL, 0, count, end, NULL}

/* Attacks query retail records from their events (SF_RECORD). Generic
 * S_HIT/S_KNOCKDOWN entries remain shared recovery fallbacks. */
static const MoveDef s_idle = {CLIP_ASSET_IDLE, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_walk = {266, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_crouch = {259, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_jump = {304, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
#define RETAIL SF_ATTACK | SF_LOCK | SF_RECORD
static const MoveDef s_jab = {.clip = 294, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_010B};
static const MoveDef s_kick = {.clip = 66, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0100};
static const MoveDef s_sweep = {.clip = 62, .last = MOVE_END, .flags = RETAIL | SF_LOW, .action = ACTION_RETAIL_010C};
static const MoveDef s_uppercut = {.clip = 293, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0109};

static const MoveDef s_throw = {.clip = 79, .last = 60, .flags = SF_LOCK | SF_RECORD | SF_UNBLOCKABLE, .action = ACTION_RETAIL_0500};
static const MoveDef s_block = {322, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_hit = {247, 0, MOVE_END, 0, 0, 24, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_HOLD, NULL, 0};
static const MoveDef s_knockdown = {186, 0, MOVE_END, 0, 0, 54, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_WAIT_LAND, NULL, 0};
static const MoveDef s_victory = {119, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};

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

static void armed_idle(Fighter *fighter);
static void armed_walk(Fighter *fighter);
static void armed_crouch(Fighter *fighter);
static void armed_block(Fighter *fighter);

static const FighterFn enter[S_COUNT] = {
    [S_IDLE] = armed_idle,
    [S_WALK] = armed_walk,
    [S_CROUCH] = armed_crouch,
    [S_BLOCK] = armed_block,
    [S_PUNCH] = liu_kang_jab,
    [S_KICK] = liu_kang_kick,
    [S_SWEEP] = liu_kang_sweep,
    [S_UPPERCUT] = liu_kang_uppercut,
};

/* Selected Liu Kang ground scripts and native tails: 4e6d08, 4e60b8,
 * 4e6158/46b740, 46b7f0, 4e61e0/46bf10, 4e69b0/46c2c0,
 * 4e6190/46bb60 and 4e6e90/46f390. Times are our script ticks. */
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
  {.move = {.clip = 67, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0102}}, /* generated 4e6d08 */
  {{.clip = 260, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0107}, {65536, -3932, 42532, 20971, 29, 10, 4, 0, 2621}, 8, 19, 22},
  {{.clip = 315, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0106}, {81264, 34734, 67502, 32768, 28, 10, 4, 0, 3932}, 9, 19, 22},
  {.move = {.clip = 54, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0104}}, /* generated 4e60b8 */
  {{.clip = 262, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0108}, {65536, -37945, 57016, 51707, 43, 10, 3, 0, 2621}, 7, 21, 27},
  {{.clip = 261, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0108}, {65536, -37945, 57016, 51707, 43, 10, 3, 0, 2621}, 9, 26, 29},
  {{.clip = 314, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0103}, {60293, -11796, 43909, 72024, 27, 10, 3, 0, 6553}, 4, 25, 28},
  {{.clip = 64, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_010D}, {0x10f5c, -0xab85, 0xf0a3, 0x6b85, 15, 15, 5, 0, 0x2e14}, 14, 39, 67},
};
// clang-format on
static const ButtonTable close_hp_followup, combo_hp_followup, close_hk_followup;
static const AttackRecord hp_record = {51118, -45809, 38666, 28180, 32, 11, 1, 0, 3276};
static const MoveDef s_hp = {.clip = 296, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_010A};
static const MoveDef s_hp_second = {.clip = 297, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_010A};

static const NativeGroundProfile ground_profile = {
    ground_attacks, {&close_hp_followup, &combo_hp_followup, &close_hk_followup}, liu_kang_ready, 4, 2, 27, 19, 20, 44, NULL};

static const NativeHPProfile hp_profile = {&hp_record, &s_hp, &s_hp_second, liu_kang_ready};

static void enter_throw(Fighter *fighter);
static void enter_lk_throw(Fighter *fighter);

/* Air scripts 0x4e6658, 0x4e6710, 0x4e61f8, 0x4e6500 and 0x4e63d8.
 * Each extends, queries while holding its strike pose, then retracts. The
 * native contact continuations freeze the body and optionally open a cancel. */

// clang-format off: Keep table entries on one row, including long rows.
static const AirAttack air_attacks[] = {
  {{.clip = 97, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0603}, {67502, -39976, 56360, 43253, 38, 10, 143, 0, 5898}, 6, 10, 9},
  {{.clip = 96, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0604}, {76677, -20316, 68157, 62914, 39, 39, 4, 0, 7864}, 10, 10, 11},
  {{.clip = 61, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0600}, {69468, -26214, 64225, 30146, 21, 10, 8, 0, 8519}, 6, 3, 20},
  {{.clip = 55, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0602}, {74055, -7864, 60293, 24903, 23, 23, 4, ATTACK_MARK, 6553}, 6, 10, 6},
  {{.clip = 58, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0601}, {75366, -2621, 67502, 34078, 22, 22, 4, ATTACK_MARK, 9830}, 10, 10, 10},
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

/* Slot 3: LP 4de678, LK 42c040/4de3b0/410/428/468/478.
 * The victim owns its released recovery, even after owner interruption/KO. */
static const MoveDef s_lk_throw = {.clip = 35, .last = 43, .flags = SF_LOCK | SF_RECORD | SF_UNBLOCKABLE, .action = ACTION_RETAIL_0501};
static const MoveDef s_lp_thrown = {.clip = 151, .last = 45, .flags = SF_LOCK, .action = ACTION_RETAIL_4003};
static const MoveDef s_lk_thrown = {.clip = 133, .last = 35, .flags = SF_LOCK, .action = ACTION_RETAIL_4003};
static const MoveDef s_lp_release = {.clip = 174, .last = 26, .flags = SF_LOCK, .action = ACTION_RETAIL_4003};
static const MoveDef s_lp_getup = {.clip = 191, .last = 15, .flags = SF_LOCK, .action = ACTION_RETAIL_200E};

/* LK release 4e49a8 -> 45eab0/47eaf0, scripts 4e8610/678/688.
 * Native 47eb50 holds a hit pair for 58 ticks; damage is D[6] once. */
static const MoveDef s_bicycle = {.clip = 68, .last = 22, .flags = RETAIL, .action = ACTION_RETAIL_0206};
static const MoveDef s_bicycle_victim = {.clip = 136, .last = 23, .flags = SF_LOCK, .action = ACTION_RETAIL_4003};
static const MoveDef s_bicycle_release = {.clip = 182, .last = 64, .flags = SF_LOCK, .action = ACTION_RETAIL_100B};
static const MoveDef s_bicycle_air_release = {.clip = 183, .last = 33, .flags = SF_LOCK, .action = ACTION_RETAIL_100B};
static const MoveDef s_bicycle_getup = {.clip = 185, .last = 15, .flags = SF_LOCK, .action = ACTION_RETAIL_200E};
static const AttackRecord bicycle_record = {0xf851, -0x547a, 0xf5c2, 0x70a3, 84, 84, 0, 0, 0x2666};

/* Fireballs: 49bf50/90/49bfe0 -> 4ed678/6c0/6f8.
 * Projectile task 64 has a 48-tick lifetime independent of its owner. */
static const MoveDef s_fireball_high = {.clip = 18, .last = 44, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_030C};
static const MoveDef s_fireball_low = {.clip = 19, .last = 46, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_030D};
static const MoveDef s_fireball_air = {.clip = 20, .last = 5, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_030E};
static const AttackRecord fireball_high_record = {0x3d70, -0x1eb8, 0x7d70, 0x3ae1, 64, 10, 2, ATTACK_MARK, 0x1999};
static const AttackRecord fireball_low_record = {0x3d70, -0x1eb8, 0x7d70, 0x3ae1, 65, 10, 2, ATTACK_MARK, 0x1999};
static const MeshAnimation fireball_mesh = {.first = 50, .count = 1, .period = 1, .scale = Q16_ONE};

/* 47edc0/4e86a8 -> 47ee00/30; D[12], six opening visits.
 * Hit and blocked exits are 4e86e8 and 4e8718 respectively. */
static const MoveDef s_flying_kick = {.clip = 92, .last = 5, .flags = RETAIL, .action = ACTION_RETAIL_0207};
static const AttackRecord flying_record = {0x14f5c, -0xf5c, 0xf5c2, 0x51eb, 31, 31, 0, 0, 0x23d7};

/* Default weapon descriptor 53d5c0[3] -> 4ece58: type 66,
 * rig 506cb0. The four weapon entries select distinct native programs. */
static const WeaponDef dragon_sword;
static void sword_hp(Fighter *fighter);
static void sword_lp(Fighter *fighter);
static void sword_back_hp(Fighter *fighter);
static void sword_back_lp(Fighter *fighter);

// clang-format off: Keep table entries on one row, including long rows.
static const WeaponStrike sword_attacks[] = {
  {{.clip = 240, .last = 44, .flags = RETAIL, .action = ACTION_RETAIL_010E}, {106168, -54394, 77987, 80609, 59, 59, 119, 0, 10485}, 0, 8, 13, 3, 4, 0, 0, 0, 34},
  {{.clip = 237, .last = 44, .flags = RETAIL, .action = ACTION_RETAIL_010E}, {99614, -49152, 64880, 55050, 61, 61, 119, 0, 9830}, 0, 9, 16, 4, 0, 9, 11, 0x147a, 34},
  {{.clip = 242, .last = 30, .flags = RETAIL, .action = ACTION_RETAIL_0212}, {111411, -25552, 89784, 11796, 62, 62, 118, 0, 9830}, 8, 9, 11, 3, 8, 2, 18, 0x28f5, 34},
  {{.clip = 227, .last = 59, .flags = RETAIL, .action = ACTION_RETAIL_0112}, {98959, -28835, 67502, 36700, 57, 57, 119, 0, 9175}, 0, 16, 29, 2, 0, 8, 13, 0x1999, 31},
};
static const MoveDef s_sword_hp = {.clip = 233, .last = 44, .flags = RETAIL, .action = ACTION_RETAIL_021C};
// clang-format on

static const WeaponDef dragon_sword = {
    0x66, sword_hp, sword_lp, sword_back_hp, sword_back_lp, NULL, weapon_throw, equip_dragon_sword, SCENE_MODEL_DRAGON_SWORD};

/* First scene: 4e0498[3] -> 450930. Second: 4dfe90[3] ->
 * 43c3d0/4dfff8/4e0020, victim reactions 176/177. */
#define FATAL_MOVE(c, last_sample) {.clip = c, .last = last_sample, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0700}
// clang-format off: Keep table entries on one row, including long rows.
static const MoveDef fatal_moves[] = {
  FATAL_MOVE(13, 55), FATAL_MOVE(119, 59), FATAL_MOVE(418, 518), FATAL_MOVE(419, 288),
  FATAL_MOVE(537, 59), FATAL_MOVE(538, 16), FATAL_MOVE(539, 37),
  FATAL_MOVE(415, 129), FATAL_MOVE(416, 107), FATAL_MOVE(412, 48), FATAL_MOVE(293, 19), FATAL_MOVE(128, 109),
};
// clang-format on
#undef FATAL_MOVE

// clang-format off: Keep table entries on one row, including long rows.
#define SCENE(c, a, b) {{.clip = c, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0700}, a, b, 1, 0, 0}
static const SceneClip dragon_actor[] = {SCENE(418, 0, 159), SCENE(418, 160, 518), SCENE(418, 446, 518)};
static const SceneClip dragon_target[] = {
  {{.clip = 389, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0700}, 0, 0, 1, 0x32f1, 0x3333},
  {{.clip = 419, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0700}, 0, 0, 1, 0x32f1, 0x3333},
  SCENE(419, 0, 288),
};
#undef SCENE
// clang-format on

// clang-format off: Keep table entries on one row, including long rows.
static const CommandDef commands[] = {
  {BUTTON_HIGH_PUNCH, {TOKEN_FORWARD, TOKEN_FORWARD}, 2, 24, CMD_AIR, NULL, liu_kang_high_fireball},
  {BUTTON_HIGH_PUNCH, {TOKEN_FORWARD, TOKEN_DOWN, TOKEN_DOWN, TOKEN_UP}, 4, 50, 0, can_close_fatality, liu_kang_second_fatality},
  {BUTTON_LOW_PUNCH, {TOKEN_FORWARD, TOKEN_FORWARD}, 2, 24, 0, NULL, liu_kang_low_fireball},
  {BUTTON_LOW_PUNCH, {TOKEN_FORWARD, TOKEN_FORWARD, TOKEN_BACK}, 3, 40, 0, can_fan_fatality, liu_kang_fan_fatality},
  {BUTTON_BLOCK, {TOKEN_FORWARD, TOKEN_FORWARD, TOKEN_FORWARD, TOKEN_DOWN}, 4, 60, CMD_BLOCK_OK, can_first_fatality, liu_kang_first_fatality},
  {BUTTON_HIGH_KICK, {TOKEN_FORWARD, TOKEN_FORWARD}, 2, 24, 0, NULL, liu_kang_flying_kick},
  {BUTTON_HIGH_KICK, {TOKEN_FORWARD, TOKEN_FORWARD, TOKEN_BACK}, 3, 40, 0, can_spike_fatality, liu_kang_spike_fatality},
  {BUTTON_LOW_KICK, {TOKEN_BACK, TOKEN_FORWARD}, 2, 24, 0, can_weapon, liu_kang_weapon},
  {BUTTON_LOW_KICK, {0}, 0, 0, CMD_RELEASE, can_bicycle, liu_kang_bicycle_kick},
  {BUTTON_RUN, {0}, 0, 0, 0, can_sidestep, liu_kang_sidestep},
  {BUTTON_RUN, {0}, 0, 0, 0, can_pickup, liu_kang_pickup},
};
// clang-format on

static const ButtonTable standing = {
    {[BUTTON_HIGH_PUNCH] = press_jab, [BUTTON_LOW_PUNCH] = press_lp, [BUTTON_HIGH_KICK] = press_kick, [BUTTON_LOW_KICK] = press_lk}};
static const ButtonTable crouching = {
    {[BUTTON_HIGH_PUNCH] = press_uppercut, [BUTTON_LOW_PUNCH] = press_crouch_lp, [BUTTON_HIGH_KICK] = press_crouch_hk, [BUTTON_LOW_KICK] = press_crouch_lk}};
static const ButtonTable jumping = {
    {[BUTTON_HIGH_PUNCH] = press_air_hp, [BUTTON_LOW_PUNCH] = press_air_lp, [BUTTON_HIGH_KICK] = press_air_hk, [BUTTON_LOW_KICK] = press_air_lk}};
static const Controls controls = {commands, COUNT(commands), {&standing, &crouching, &jumping}};

static const CpuMoves cpu_moves = {
    .enter =
        {
            [CPU_JAB] = liu_kang_jab,
            [CPU_LOW_KICK] = liu_kang_lk,
            [CPU_HIGH_KICK] = liu_kang_hk,
            [CPU_SWEEP] = enter_sweep,
            [CPU_UPPERCUT] = enter_uppercut,
            [CPU_THROW] = enter_throw,
            [CPU_CLOSE_HP] = cpu_close_hp,
            [CPU_CLOSE_HK] = cpu_close_hk,
            [CPU_CROUCH_LP] = liu_kang_crouch_lp,
            [CPU_AIR_HP] = cpu_air_hp,
            [CPU_AIR_HK] = cpu_air_hk,
            [CPU_AIR_LK] = cpu_air_lk,
            [CPU_PROJECTILE] = liu_kang_low_fireball,
            [CPU_SPECIAL_1] = liu_kang_low_fireball,
            [CPU_SPECIAL_2] = liu_kang_flying_kick,
            [CPU_SPECIAL_NEAR] = liu_kang_bicycle_kick,
            [CPU_DRAW] = liu_kang_weapon,
            [CPU_FINISH_1] = liu_kang_first_fatality,
            [CPU_FINISH_2] = liu_kang_second_fatality,
            [CPU_FINISH_FAN] = liu_kang_fan_fatality,
            [CPU_FINISH_SPIKE] = liu_kang_spike_fatality,
        },
    .finish_min = {0x1cccc, 0},
    .finish_distance = {0x20000, 0x10000}, /* 4dfe10/4dfe50[3]. */
};

const CharacterDef mk4_liu_kang = {GAME_MK4, 3, "LIU KANG", &mk4_motion, moves, S_COUNT, &controls, enter, -70320, mk4_walk, mk4_jump, &cpu_moves};

/* Private move implementations share this character's data and declarations. */
#include "moves/air_attacks.inc"
#include "moves/bicycle.inc"
#include "moves/common.inc"
#include "moves/controls.inc"
#include "moves/fan.inc"
#include "moves/fatality.inc"
#include "moves/fatality_first.inc"
#include "moves/fatality_second.inc"
#include "moves/flying_kick.inc"
#include "moves/ground_attacks.inc"
#include "moves/kicks.inc"
#include "moves/projectile.inc"
#include "moves/punches.inc"
#include "moves/roundhouse.inc"
#include "moves/sidestep.inc"
#include "moves/spike.inc"
#include "moves/sweep.inc"
#include "moves/throws.inc"
#include "moves/uppercut.inc"
#include "moves/weapon.inc"
