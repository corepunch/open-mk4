#include "character.h"

#include "generated/scripts.inc"
#include "generated/frames.inc"

/* Retail attack records (docs/scorpion/combat-core.md section 2). */
// clang-format off: Keep table entries on one row, including long rows.
static const AttackRecord uppercut_record = {0xe3d7, -0xfd70, 0x947a, 0x10518, 34, 34, 6, 0, 0x28f5};     /* 0x4ebd50, A selector 2 */
static const AttackRecord uppercut_alt_record = {0x12106, -0x111a9, 0xfa9f, 0x17a9f, 34, 34, 6, 0, 0xa3d}; /* 0x4ebd68, A selector 6 */
static const AttackRecord kick_record = {83230, -51118, 65536, 51118, 16, 13, 6, 0, 8519};       /* 0x4ebac8, B selector 0 */
static const AttackRecord sweep_record = {0x175c2, 0x7a9f, 0x10ccc, 0x8a3d, 18, 18, 4, ATTACK_LOW, 0x23d7}; /* 0x4ebb58, B selector 35 */
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
static const MoveDef s_kick = {.clip = 65, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0100};
static const MoveDef s_sweep = {.clip = 63, .last = MOVE_END, .flags = RETAIL | SF_LOW, .action = ACTION_RETAIL_010C};
static const MoveDef s_uppercut = {.clip = 293, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0109};
/* Native uppercut recovery uses contact-selected pose sequences. */
static const MoveDef s_shadow_low = {.clip = 26, .last = 13, .flags = SF_LOCK, .action = 0x309};
static const MoveDef s_shadow_up = {.clip = 24, .last = 9, .flags = SF_LOCK, .action = 0x30a};

static const MoveDef s_throw = {.clip = 76, .last = 48, .flags = SF_LOCK | SF_RECORD | SF_UNBLOCKABLE, .action = ACTION_RETAIL_0500};
static const MoveDef s_block = {322, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_hit = {247, 0, MOVE_END, 0, 0, 24, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_HOLD, NULL, 0};
static const MoveDef s_knockdown = {186, 0, MOVE_END, 0, 0, 54, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_WAIT_LAND, NULL, 0};
static const MoveDef s_victory = {114, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};

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
    [S_PUNCH] = kitana_jab,
    [S_KICK] = kitana_kick,
    [S_SWEEP] = kitana_sweep,
    [S_UPPERCUT] = kitana_uppercut,
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
  {{.clip = 308, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0102}, {0x15c28, -0x35c2, 0x104df, 0x5c28, 17, 10, 6, 0, 0x1c28}, 11, 30, 32},
  {{.clip = 260, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0107}, {65536, -3932, 42532, 20971, 29, 10, 4, 0, 2621}, 8, 19, 22},
  {{.clip = 315, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0106}, {81264, 34734, 67502, 32768, 28, 10, 4, 0, 3932}, 9, 19, 22},
  {{.clip = 53, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0104}, {0xa8f5, -0xd70a, 0xa3d7, 0xb851, 26, 26, 7, 0, 0x1999}, 7, 31, 31},
  {{.clip = 262, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0108}, {65536, -37945, 57016, 51707, 43, 10, 3, 0, 2621}, 7, 21, 27},
  {{.clip = 261, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0108}, {65536, -37945, 57016, 51707, 43, 10, 3, 0, 2621}, 9, 26, 29},
  {{.clip = 314, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0103}, {60293, -11796, 43909, 72024, 27, 10, 3, 0, 6553}, 4, 25, 28},
  {{.clip = 313, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_010D}, {69468, -43909, 61603, 27525, 20, 20, 5, 0, 11796}, 16, 33, 61},
};
// clang-format on
static const ButtonTable close_hp_followup, combo_hp_followup, close_hk_followup;
static const AttackRecord hp_record = {51118, -45809, 38666, 28180, 32, 11, 1, 0, 3276};
static const MoveDef s_hp = {.clip = 296, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_010A};
static const MoveDef s_hp_second = {.clip = 297, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_010A};

static const NativeGroundProfile ground_profile = {
    ground_attacks, {&close_hp_followup, &combo_hp_followup, &close_hk_followup}, kitana_ready, 5, 2, 27, 19, 20, 44, NULL};

static const NativeHPProfile hp_profile = {&hp_record, &s_hp, &s_hp_second, kitana_ready};

static void enter_throw(Fighter *fighter);
static void enter_lk_throw(Fighter *fighter);

/* Air scripts 0x4e6658, 0x4e6710, 0x4e61f8, 0x4e6500 and 0x4e63d8.
 * Each extends, queries while holding its strike pose, then retracts. The
 * native contact continuations freeze the body and optionally open a cancel. */

// clang-format off: Keep table entries on one row, including long rows.
static const AirAttack air_attacks[] = {
  {{.clip = 97, .last = MOVE_END, .flags = RETAIL, .action = 0x603}, {67502, -39976, 56360, 43253, 38, 10, 143, 0, 5898}, 6, 10, 9},
  {{.clip = 96, .last = MOVE_END, .flags = RETAIL, .action = 0x604}, {76677, -20316, 68157, 62914, 39, 39, 4, 0, 7864}, 10, 10, 11},
  {{.clip = 327, .last = MOVE_END, .flags = RETAIL, .action = 0x600}, {67502, -18350, 58982, 43253, 21, 10, 8, 0, 8519}, 8, 4, 8},
  {{.clip = 57, .last = MOVE_END, .flags = RETAIL, .action = 0x602}, {74055, 1310, 39321, 19660, 23, 23, 4, 2, 6553}, 6, 10, 6},
  {{.clip = 60, .last = MOVE_END, .flags = RETAIL, .action = 0x601}, {55705, 1966, 56950, 30146, 22, 22, 4, 2, 9830}, 6, 10, 10},
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
static const MoveDef s_lk_throw = {.clip = 43, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0501};
static const MoveDef s_lk_thrown = {.clip = 163, .last = MOVE_END, .flags = SF_LOCK, .action = ACTION_RETAIL_4003};
static const MoveDef s_lp_thrown = {.clip = 156, .last = MOVE_END, .flags = SF_LOCK, .action = ACTION_RETAIL_4003};

/* 49c300 spawns independently of the owner's generated recovery. */
static const MoveDef s_projectile = {.clip = 15, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = 0x313};
static const AttackRecord projectile_record = {22282, -7864, 41287, 15073, 68, 10, 2, 2, 7864};
static const MeshAnimation projectile_mesh = {.first = 42, .count = 2, .period = 1, .scale = Q16_ONE};

static const MoveDef s_leg_grab = {.clip = 102, .last = 64, .flags = SF_LOCK | SF_RECORD, .action = 0x203};
static const MoveDef s_leg_victim = {.clip = 201, .last = 34, .flags = SF_LOCK, .action = ACTION_RETAIL_4003};
static const MoveDef s_square_wave = {.clip = 72, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = 0x201};
static const MoveDef s_cartwheel = {.clip = 104, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = 0x213};
static const MoveDef s_bicycle = {.clip = 310, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = 0x205};
static const AttackRecord leg_record = {92405, -29491, 85852, 76677, 88, 88, 8, 1, 11796};
static const AttackRecord square_record = {19005, -9830, 39321, 57671, 87, 87, 0, 0, 13107};
static const AttackRecord cartwheel_record = {75300, -45088, 72083, 89063, 121, 121, 9, 0, 4587};
static const AttackRecord bicycle_record = {80609, -1310, 62259, 20971, 86, 86, 0, 0, 9830};

/* Native 494a00 parameter blocks; byte fields are decoded at authoring. */
// clang-format off: Keep table entries on one row, including long rows.
static const WeaponStrike weapon_attacks[] = {
  {{.clip = 230, .last = 44, .flags = RETAIL, .action = 0x111}, {124518, -34078, 83230, 51118, 54, 54, 119, 0, 11796}, 0, 8, 11, 3, 1, 0, 0, 0, 34}, /* 4ec760 */
  {{.clip = 229, .last = 44, .flags = RETAIL, .action = 0x111}, {126484, -32768, 64880, 57016, 56, 56, 119, 0, 9830}, 0, 4, 7, 3, 1, 0, 0, 0, 32}, /* 4ec778 */
};
// clang-format on

static const WeaponDef native_weapon;

static const MoveDef s_weapon_back_hp = {.clip = 228, .last = 39, .flags = RETAIL, .action = 0x112};
static const MoveDef s_weapon_back_lp = {.clip = 227, .last = 59, .flags = RETAIL, .action = 0x112};
static const AttackRecord weapon_back_hp_record = {98959, -28835, 67502, 36700, 55, 55, 119, 0, 13107};
static const AttackRecord weapon_back_lp_record = {98959, -28835, 67502, 36700, 57, 57, 119, 0, 9175};

static const WeaponDef native_weapon = {
    /* Kitana's roster slot selects the same descriptor 4ece38 and rig as
     * Sonya, so both use the shared retail weapon model. */
    0x62, weapon_hp, weapon_lp, weapon_back_hp, weapon_back_lp, NULL, weapon_throw, equip_weapon, SCENE_MODEL_SONYA_WEAPON};

static const MoveDef s_air_owner = {.clip = 77, .last = 39, .flags = SF_LOCK | SF_RECORD, .action = 0x502};
static const MoveDef s_air_victim = {.clip = 161, .last = 39, .flags = SF_LOCK, .action = ACTION_RETAIL_4003};

// clang-format off: Keep table entries on one row, including long rows.
static const CommandDef commands[] = {
  {BUTTON_HIGH_PUNCH, {TOKEN_FORWARD, TOKEN_BACK}, 2, 24, 0x0, NULL, kitana_bicycle},
  {BUTTON_HIGH_PUNCH, {TOKEN_FORWARD, TOKEN_DOWN, TOKEN_FORWARD}, 3, 40, 0x0, can_spike_fatality, kitana_spike_fatality},
  {BUTTON_LOW_PUNCH, {TOKEN_DOWN, TOKEN_FORWARD}, 2, 16, 0x0, NULL, kitana_ring},
  {BUTTON_LOW_PUNCH, {TOKEN_DOWN, TOKEN_FORWARD}, 2, 26, 0x0, NULL, kitana_command_shadow_low},
  {BUTTON_LOW_PUNCH, {TOKEN_DOWN, TOKEN_FORWARD}, 2, 26, CMD_AIR, kitana_shadow_up_enabled, kitana_command_shadow_up},
  {BUTTON_LOW_PUNCH, {0}, 0, 0, 0x40, can_leg_grab, kitana_leg_grab},
  {BUTTON_BLOCK, {0}, 0, 0, 0x10, mk4_can_air_throw, kitana_air_throw},
  {BUTTON_BLOCK, {0}, 0, 0, 0x40, can_leg_grab, kitana_leg_grab},
  {BUTTON_HIGH_KICK, {TOKEN_BACK, TOKEN_BACK, TOKEN_DOWN}, 3, 40, 0x0, NULL, kitana_square_wave},
  {BUTTON_HIGH_KICK, {TOKEN_DOWN, TOKEN_BACK, TOKEN_BACK}, 3, 40, 0x0, can_fan_fatality, kitana_fan_fatality},
  {BUTTON_HIGH_KICK, {TOKEN_UP, TOKEN_DOWN, TOKEN_DOWN, TOKEN_UP}, 4, 50, 0x0, can_second_fatality, second_fatality},
  {BUTTON_LOW_KICK, {TOKEN_BACK, TOKEN_DOWN, TOKEN_FORWARD}, 3, 29, 0x0, can_cartwheel, kitana_cartwheel},
  {BUTTON_LOW_KICK, {TOKEN_FORWARD, TOKEN_FORWARD}, 2, 24, 0x0, can_weapon, kitana_weapon},
  {BUTTON_RUN, {TOKEN_DOWN, TOKEN_DOWN, TOKEN_DOWN, TOKEN_UP}, 4, 50, 0x40, can_first_fatality, first_fatality},
  {BUTTON_RUN, {0}, 0, 0, 0x0, mk4_can_sidestep, kitana_sidestep},
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
            [CPU_JAB] = kitana_jab,
            [CPU_LOW_KICK] = kitana_lk,
            [CPU_HIGH_KICK] = kitana_hk,
            [CPU_SWEEP] = enter_sweep,
            [CPU_UPPERCUT] = enter_uppercut,
            [CPU_THROW] = enter_throw,
            [CPU_CLOSE_HP] = cpu_close_hp,
            [CPU_CLOSE_HK] = cpu_close_hk,
            [CPU_CROUCH_LP] = kitana_crouch_lp,
            [CPU_AIR_HP] = air_hp,
            [CPU_AIR_HK] = cpu_air_hk,
            [CPU_AIR_LK] = cpu_air_lk,
            [CPU_DRAW] = kitana_weapon,
            [CPU_FINISH_1] = first_fatality,
            [CPU_FINISH_2] = second_fatality,
            [CPU_FINISH_FAN] = kitana_fan_fatality,
            [CPU_FINISH_SPIKE] = kitana_spike_fatality,
            [CPU_PROJECTILE] = kitana_ring,
            [CPU_SPECIAL_NEAR] = kitana_leg_grab,
            [CPU_SPECIAL_1] = kitana_bicycle,
            [CPU_SPECIAL_2] = kitana_cartwheel,
        },
    .finish_min = {0x10000, 0x1cccc},
    .finish_distance = {0x30000, 0x30000},
};

const CharacterDef mk4_kitana = {GAME_MK4, 16, "KITANA", &mk4_motion, moves, S_COUNT, &controls, enter, -73629, mk4_walk, mk4_jump, &cpu_moves};

/* Private move implementations share this character's data and declarations. */
#include "moves/air_attacks.inc"
#include "moves/bicycle.inc"
#include "moves/cartwheel.inc"
#include "moves/common.inc"
#include "moves/controls.inc"
#include "moves/fan.inc"
#include "moves/fatality.inc"
#include "moves/fatality_first.inc"
#include "moves/fatality_second.inc"
#include "moves/ground_attacks.inc"
#include "moves/kicks.inc"
#include "moves/leg_grab.inc"
#include "moves/projectile.inc"
#include "moves/punches.inc"
#include "moves/roundhouse.inc"
#include "moves/sidestep.inc"
#include "moves/spike.inc"
#include "moves/square_wave.inc"
#include "moves/sweep.inc"
#include "moves/throws.inc"
#include "moves/uppercut.inc"
#include "moves/weapon.inc"
