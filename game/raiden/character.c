#include "character.h"

#include "generated/scripts.inc"
#include "generated/frames.inc"

/* Retail attack records (docs/scorpion/combat-core.md section 2). */
// clang-format off: Keep table entries on one row, including long rows.
static const AttackRecord uppercut_record = {0xe3d7, -0xfd70, 0x947a, 0x10518, 34, 34, 6, 0, 0x28f5};     /* 0x4ebd50, A selector 2 */
static const AttackRecord uppercut_alt_record = {0x12106, -0x111a9, 0xfa9f, 0x17a9f, 34, 34, 6, 0, 0xa3d}; /* 0x4ebd68, A selector 6 */
static const AttackRecord kick_record = {71368, -56950, 83820, 74645, 16, 13, 6, 0, 8519};       /* 0x4ebaf8, B selector 2 */
static const AttackRecord sweep_record = {94371, 34072, 57016, 32768, 19, 19, 4, 1, 9175}; /* 0x4ebb70, B selector 36 */
// clang-format on

#define COUNT(array) ((int)(sizeof array / sizeof array[0]))
#define SEQUENCE(segments, count, end) {segments, COUNT(segments), NULL, 0, count, end, NULL}

/* Attack entries query source contact records through SF_RECORD. Locomotion
 * and generic reaction states retain the shared portable controllers. */
static const MoveDef s_idle = {CLIP_ASSET_IDLE, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_walk = {267, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_crouch = {259, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_jump = {304, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
#define RETAIL SF_ATTACK | SF_LOCK | SF_RECORD
static const MoveDef s_jab = {.clip = 294, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_010B};
static const MoveDef s_kick = {.clip = 71, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0100};
static const MoveDef s_sweep = {.clip = 62, .last = MOVE_END, .flags = RETAIL | SF_LOW, .action = ACTION_RETAIL_010C};
static const MoveDef s_uppercut = {.clip = 293, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0109};
/* Native uppercut recovery uses contact-selected pose sequences. */

static const MoveDef s_throw = {.clip = 76, .last = 48, .flags = SF_LOCK | SF_RECORD | SF_UNBLOCKABLE, .action = ACTION_RETAIL_0500};
static const MoveDef s_block = {322, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_hit = {247, 0, MOVE_END, 0, 0, 24, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_HOLD, NULL, 0};
static const MoveDef s_knockdown = {186, 0, MOVE_END, 0, 0, 54, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_WAIT_LAND, NULL, 0};
static const MoveDef s_victory = {123, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};

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

static void enter_victory(Fighter *fighter);
static void enter_throw(Fighter *fighter);

static const FighterFn enter[S_COUNT] = {
    [S_THROW] = enter_throw,
    [S_VICTORY] = enter_victory,
    [S_IDLE] = armed_idle,
    [S_WALK] = armed_walk,
    [S_CROUCH] = armed_crouch,
    [S_BLOCK] = armed_block,
    [S_PUNCH] = raiden_jab,
    [S_KICK] = raiden_kick,
    [S_SWEEP] = raiden_sweep,
    [S_UPPERCUT] = raiden_uppercut,
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
    ground_attacks, {&close_hp_followup, &combo_hp_followup, &close_hk_followup}, raiden_ready, 5, 2, 27, 19, 20, 44, NULL};

static const NativeHPProfile hp_profile = {&hp_record, &s_hp, &s_hp_second, raiden_ready};

static void enter_throw(Fighter *fighter);
static void enter_lk_throw(Fighter *fighter);

/* Air scripts 0x4e6658, 0x4e6710, 0x4e61f8, 0x4e6500 and 0x4e63d8.
 * Each extends, queries while holding its strike pose, then retracts. The
 * native contact continuations freeze the body and optionally open a cancel. */

// clang-format off: Keep table entries on one row, including long rows.
static const AirAttack air_attacks[] = {
  {{.clip = 97, .last = MOVE_END, .flags = RETAIL, .action = 0x603}, {67502, -39976, 56360, 43253, 38, 10, 143, 0, 5898}, 6, 10, 9},
  {{.clip = 96, .last = MOVE_END, .flags = RETAIL, .action = 0x604}, {76677, -20316, 68157, 62914, 39, 39, 4, 0, 7864}, 10, 10, 11},
  {{.clip = 61, .last = MOVE_END, .flags = RETAIL, .action = 0x600}, {69468, -26214, 64225, 30146, 21, 10, 8, 0, 8519}, 6, 3, 20},
  {{.clip = 56, .last = MOVE_END, .flags = RETAIL, .action = 0x602}, {71434, -4587, 66846, 24903, 23, 23, 4, 2, 6553}, 6, 10, 6},
  {{.clip = 59, .last = MOVE_END, .flags = RETAIL, .action = 0x601}, {55705, -2621, 51773, 38010, 22, 22, 4, 2, 9830}, 6, 10, 10},
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

/* 4ddfb8 roster dispatch and 4de798 LP selector. Owner and victim have
 * independent fibers after release; replacement cancels only a live pair. */
static const MoveDef s_lk_throw = {.clip = 33, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0501};
static const MoveDef s_lk_thrown = {.clip = 149, .last = MOVE_END, .flags = SF_LOCK, .action = ACTION_RETAIL_4003};
static const MoveDef s_lp_thrown = {.clip = 154, .last = MOVE_END, .flags = SF_LOCK, .action = ACTION_RETAIL_4003};

/* 49c300 spawns independently of the owner's generated recovery. */
static const MoveDef s_projectile = {.clip = 17, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = 0x305};
static const AttackRecord projectile_record = {22282, -7864, 41287, 15073, 66, 10, 2, 2, 9175};
static const MeshAnimation projectile_mesh = {.first = 41, .count = 1, .period = 1, .scale = Q16_ONE};

static const MoveDef s_torpedo = {.clip = 94, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = 0x202};
static const MoveDef s_torpedo_victim = {.clip = 135, .last = MOVE_END, .flags = SF_LOCK, .action = ACTION_RETAIL_4003};
static const AttackRecord torpedo_record = {85852, -3932, 62914, 20971, 82, 82, 0, 0, 9830};

static const MoveDef s_teleport = {.clip = 304, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = 0x20c};

/* Native 494a00 parameter blocks; byte fields are decoded at authoring. */
// clang-format off: Keep table entries on one row, including long rows.
static const WeaponStrike weapon_attacks[] = {
  {{.clip = 233, .last = 44, .flags = RETAIL, .action = 0x112}, {104857, -27525, 70778, 20971, 51, 51, 119, 0, 9830}, 0, 8, 16, 5, 0, 0, 0, 0, 34}, /* 4ec810 */
  {{.clip = 234, .last = 44, .flags = RETAIL, .action = 0x112}, {94371, -24903, 74711, 22282, 47, 47, 119, 0, 9830}, 0, 8, 16, 4, 0, 0, 0, 0, 34}, /* 4ec848 */
  {{.clip = 238, .last = 36, .flags = RETAIL, .action = 0x112}, {114688, -24903, 71434, 35389, 53, 10, 119, 0, 9830}, 0, 8, 25, 3, 0, 8, 17, 4587, 34}, /* 4ec838 */
  {{.clip = 237, .last = 44, .flags = RETAIL, .action = 0x112}, {99614, -49152, 64880, 55050, 52, 52, 119, 0, 9830}, 0, 9, 16, 4, 0, 9, 11, 5242, 34}, /* 4ec870 */
};
// clang-format on

static const WeaponDef native_weapon;

static const WeaponDef native_weapon = {0x61, weapon_hp, weapon_lp, weapon_back_hp, weapon_back_lp, NULL, weapon_throw, equip_weapon, SCENE_MODEL_RAIDEN_STAFF};

// clang-format off: Keep table entries on one row, including long rows.
static const CommandDef commands[] = {
  {BUTTON_UP, {TOKEN_DOWN}, 1, 32, 0x0, can_teleport, raiden_teleport},
  {BUTTON_HIGH_PUNCH, {TOKEN_FORWARD, TOKEN_BACK}, 2, 26, 0x0, can_weapon, raiden_weapon},
  {BUTTON_HIGH_PUNCH, {TOKEN_DOWN, TOKEN_UP, TOKEN_UP, TOKEN_UP}, 4, 50, 0x0, can_close_fatality, second_fatality},
  {BUTTON_LOW_PUNCH, {TOKEN_DOWN, TOKEN_BACK}, 2, 16, 0x0, NULL, raiden_lightning},
  {BUTTON_LOW_PUNCH, {TOKEN_FORWARD, TOKEN_FORWARD, TOKEN_DOWN}, 3, 40, 0x0, can_spike_fatality, raiden_spike_fatality},
  {BUTTON_BLOCK, {TOKEN_DOWN, TOKEN_FORWARD, TOKEN_BACK}, 3, 40, 0x40, can_fan_fatality, raiden_fan_fatality},
  {BUTTON_HIGH_KICK, {TOKEN_FORWARD, TOKEN_BACK, TOKEN_UP, TOKEN_UP}, 4, 50, 0x40, can_close_fatality, first_fatality},
  {BUTTON_LOW_KICK, {TOKEN_FORWARD, TOKEN_FORWARD}, 2, 26, 0x10, NULL, raiden_torpedo},
  {BUTTON_RUN, {0}, 0, 0, 0x0, mk4_can_sidestep, raiden_sidestep},
  {BUTTON_RUN, {0}, 0, 0, 0x0, weapon_can_pickup, weapon_pickup},
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
            [CPU_JAB] = raiden_jab,
            [CPU_LOW_KICK] = raiden_lk,
            [CPU_HIGH_KICK] = raiden_hk,
            [CPU_SWEEP] = enter_sweep,
            [CPU_UPPERCUT] = enter_uppercut,
            [CPU_THROW] = enter_throw,
            [CPU_CLOSE_HP] = cpu_close_hp,
            [CPU_CLOSE_HK] = cpu_close_hk,
            [CPU_CROUCH_LP] = raiden_crouch_lp,
            [CPU_AIR_HP] = air_hp,
            [CPU_AIR_HK] = cpu_air_hk,
            [CPU_AIR_LK] = cpu_air_lk,
            [CPU_DRAW] = raiden_weapon,
            [CPU_FINISH_1] = first_fatality,
            [CPU_FINISH_2] = second_fatality,
            [CPU_FINISH_FAN] = raiden_fan_fatality,
            [CPU_FINISH_SPIKE] = raiden_spike_fatality,
            [CPU_PROJECTILE] = raiden_lightning,
            [CPU_SPECIAL_NEAR] = raiden_torpedo,
            [CPU_SPECIAL_1] = raiden_teleport,
            [CPU_SPECIAL_2] = raiden_torpedo,
        },
    .finish_min = {0, 0},
    .finish_distance = {0x10000, 0x10000},
};

const CharacterDef mk4_raiden = {GAME_MK4, 1, "RAIDEN", &mk4_motion, moves, S_COUNT, &controls, enter, -75497, mk4_walk, mk4_jump, &cpu_moves};

/* Private move implementations share this character's data and declarations. */
#include "moves/air_attacks.inc"
#include "moves/common.inc"
#include "moves/controls.inc"
#include "moves/fan.inc"
#include "moves/fatality.inc"
#include "moves/fatality_first.inc"
#include "moves/fatality_second.inc"
#include "moves/ground_attacks.inc"
#include "moves/kicks.inc"
#include "moves/projectile.inc"
#include "moves/punches.inc"
#include "moves/roundhouse.inc"
#include "moves/sidestep.inc"
#include "moves/spike.inc"
#include "moves/sweep.inc"
#include "moves/teleport.inc"
#include "moves/throws.inc"
#include "moves/uppercut.inc"
#include "moves/weapon.inc"
