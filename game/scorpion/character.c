#include "character.h"

static void script_spear_done(Fighter *fighter);
static void scorpion_air_throw(Fighter *fighter);
static void fan_start(Fighter *fighter);
static void spike_start(Fighter *fighter);
#include "generated/scripts.inc"
#include "generated/frames.inc"

/* Retail attack records (docs/scorpion/combat-core.md section 2). */
// clang-format off: Keep table entries on one row, including long rows.
static const AttackRecord uppercut_record = {0xe3d7, -0xfd70, 0x947a, 0x10518, 34, 34, 6, 0, 0x28f5};     /* 0x4ebd50, A selector 2 */
static const AttackRecord uppercut_alt_record = {0x12106, -0x111a9, 0xfa9f, 0x17a9f, 34, 34, 6, 0, 0xa3d}; /* 0x4ebd68, A selector 6 */
static const AttackRecord kick_record = {0x15e76, -0xd999, 0x151a9, 0x111eb, 16, 13, 6, 0, 0x2147};       /* 0x4ebae0, B selector 1 */
static const AttackRecord sweep_record = {0x175c2, 0x7a9f, 0x10ccc, 0x8a3d, 18, 18, 4, ATTACK_LOW, 0x23d7}; /* 0x4ebb58, B selector 35 */
// clang-format on

#define COUNT(array) ((int)(sizeof array / sizeof array[0]))
#define SEQUENCE(segments, count, end) {segments, COUNT(segments), NULL, 0, count, end, NULL}

/* Recovered moves query retail records from their events (SF_RECORD). The
 * remaining moves keep the approximate damage, reach and push columns. */
static const MoveDef s_idle = {CLIP_ASSET_IDLE, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_walk = {269, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_crouch = {259, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_jump = {304, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
#define RETAIL SF_ATTACK | SF_LOCK | SF_RECORD
static const MoveDef s_jab = {.clip = 294, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_010B};
static const MoveDef s_kick = {.clip = 66, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0100};
static const MoveDef s_sweep = {.clip = 63, .last = MOVE_END, .flags = RETAIL | SF_LOW, .action = ACTION_RETAIL_010C};
static const MoveDef s_uppercut = {.clip = 293, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0109};
/* The jab graph alternates an opening/recovery with a second clip. Contact
 * requests at phase boundaries execute before the next branch is committed. */

static const MoveDef s_throw = {.clip = 75, .last = 64, .flags = SF_LOCK | SF_RECORD | SF_UNBLOCKABLE, .action = ACTION_RETAIL_0500};
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

static void scorpion_idle(Fighter *fighter);
static void scorpion_walk(Fighter *fighter);
static void scorpion_crouch(Fighter *fighter);
static void scorpion_block(Fighter *fighter);
static const FighterFn enter[S_COUNT] = {
    [S_IDLE] = scorpion_idle,
    [S_WALK] = scorpion_walk,
    [S_CROUCH] = scorpion_crouch,
    [S_BLOCK] = scorpion_block,
    [S_PUNCH] = scorpion_jab,
    [S_KICK] = scorpion_kick,
    [S_SWEEP] = scorpion_sweep,
    [S_UPPERCUT] = scorpion_uppercut,
};

/* Specials translated from spear_entry/return, teleport_entry/landing and
 * breathe_fire in build/retail-disassembly/scripts. Native loops are ordinary
 * C continuations. The camera-dependent teleport exit uses a fixed ten-tick
 * vanish in our engine; it retains the distance/wall exits and contact branches. */
static const AttackRecord spear_record = {0x3333, -0x1eb8, 0x947a, 0x3ae1, 70, 70, 112, ATTACK_MARK, 0xccc};
static const AttackRecord teleport_record = {0x128f5, -0x3ae1, 0xab85, 0x91eb, 90, 90, 0, ATTACK_MARK, 0x170a};
static const AttackRecord fire_record = {0x24e56, -0xc978, 0x23cac, 0xb7ce, 173, 173, 0, ATTACK_UNBLOCKABLE, 0x1eb8};
static const MoveDef s_spear = {.clip = 14, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0300};
static const MoveDef s_snared = {.clip = 199, .last = MOVE_END, .flags = SF_LOCK, .action = ACTION_RETAIL_4000};
static const MoveDef s_spear_stun = {.clip = 13, .last = MOVE_END, .flags = SF_LOCK, .action = ACTION_RETAIL_1008};
static const MoveDef s_teleport = {.clip = 312, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0209};
static const MoveDef s_teleport_block = {.clip = 301, .last = MOVE_END, .flags = SF_LOCK, .action = ACTION_RETAIL_1011};
static const MoveDef s_teleport_land = {.clip = 304, .first = 18, .last = 27, .flags = SF_LOCK, .action = ACTION_RETAIL_2008};
static const MoveDef s_fire = {.clip = 448, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0115};

static void teleport_drop(Fighter *fighter);
static void teleport_pause(Fighter *fighter);

static void sword_hp(Fighter *fighter);
static void sword_lp(Fighter *fighter);
static void sword_back_hp(Fighter *fighter);
static void sword_back_lp(Fighter *fighter);
static void sword_spin(Fighter *fighter);
static void scorpion_weapon(Fighter *fighter);

static bool can_weapon(const Fighter *fighter);
static bool can_pickup(const Fighter *fighter);
static void scorpion_pickup(Fighter *fighter);

/* Ground scripts and their native tails: 4e6d08/46a8d0, 4e60b8,
 * 4e6158/46b740, 46b7f0, 4e61e0/46bf10, 4e69b0/46c2c0,
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
  {{.clip = 67, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0102}, {94371, -13762, 72024, 23592, 17, 10, 6, 0, 7208}, 10, 33, 36},
  {{.clip = 260, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0107}, {65536, -3932, 42532, 20971, 29, 10, 4, 0, 2621}, 8, 19, 22},
  {{.clip = 315, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0106}, {81264, 34734, 67502, 32768, 28, 10, 4, 0, 3932}, 9, 19, 22},
  {{.clip = 54, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0104}, {48496, -24903, 44564, 30146, 25, 10, 5, 0, 7864}, 9, 29, 29},
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
    ground_attacks, {&close_hp_followup, &combo_hp_followup, &close_hk_followup}, scorpion_ready, 5, 0, 28, 21, 22, 38, NULL};

static const NativeHPProfile hp_profile = {&hp_record, &s_hp, &s_hp_second, scorpion_ready};

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
static const ButtonTable scorpion_standing;

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

/* Paired ground throws: 0x4de6e4 (LP), 0x4de2f8/318/328 (LK).
 * Native opponent-root alignment is represented by a shared world origin. */
static const MoveDef s_lk_throw = {.clip = 37, .last = 65, .flags = SF_LOCK | SF_RECORD | SF_UNBLOCKABLE, .action = ACTION_RETAIL_0501};
static const MoveDef s_lp_thrown = {.clip = 152, .last = 41, .flags = SF_LOCK, .action = ACTION_RETAIL_4003};
static const MoveDef s_lk_thrown = {.clip = 157, .last = 130, .flags = SF_LOCK, .action = ACTION_RETAIL_4003};
static const MoveDef s_lk_thrown_end = {.clip = 586, .first = 65, .last = 130, .flags = SF_LOCK, .action = ACTION_RETAIL_4003};

/* Generated VM finishers use shared scene, blend and object helpers. */
#define FATAL_MOVE(c, last_sample) {.clip = c, .last = last_sample, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0700}
// clang-format off: Keep table entries on one row, including long rows.
static const MoveDef fatal_moves[] = {
  FATAL_MOVE(13, 55), FATAL_MOVE(390, 144), FATAL_MOVE(391, 169), FATAL_MOVE(206, 99),
  FATAL_MOVE(115, 18), FATAL_MOVE(116, 49), FATAL_MOVE(117, 18), FATAL_MOVE(118, 49),
  FATAL_MOVE(12, 57), FATAL_MOVE(578, 48), FATAL_MOVE(579, 34), FATAL_MOVE(580, 77), FATAL_MOVE(581, 37),
  FATAL_MOVE(584, 77), FATAL_MOVE(585, 47), FATAL_MOVE(415, 129), FATAL_MOVE(416, 107), FATAL_MOVE(412, 48),
  FATAL_MOVE(293, 19), FATAL_MOVE(128, 109),
};
// clang-format on
#undef FATAL_MOVE

// clang-format off: Keep table entries on one row, including long rows.
static const CommandDef commands[] = {
  {BUTTON_BLOCK, {TOKEN_BACK, TOKEN_FORWARD, TOKEN_FORWARD, TOKEN_BACK}, 4, 50, CMD_BLOCK_OK, can_burn_fatality, scorpion_burn_fatality},
  {BUTTON_HIGH_PUNCH, {TOKEN_BACK, TOKEN_FORWARD, TOKEN_DOWN, TOKEN_UP}, 4, 50, 0, can_beast_fatality, scorpion_beast_fatality},
  {BUTTON_LOW_KICK, {TOKEN_FORWARD, TOKEN_DOWN, TOKEN_DOWN}, 3, 40, 0, can_fan_fatality, scorpion_fan_fatality},
  {BUTTON_LOW_KICK, {TOKEN_BACK, TOKEN_FORWARD, TOKEN_FORWARD}, 3, 40, 0, can_spike_fatality, scorpion_spike_fatality},
  {BUTTON_BLOCK, {0}, 0, 0, CMD_AIR | CMD_BLOCK_OK, mk4_can_air_throw, scorpion_air_throw},
  {BUTTON_HIGH_PUNCH, {TOKEN_DOWN, TOKEN_BACK}, 2, 24, CMD_AIR, can_teleport, scorpion_teleport},
  {BUTTON_LOW_PUNCH, {TOKEN_BACK, TOKEN_BACK}, 2, 27, 0, can_spear, scorpion_spear},
  {BUTTON_LOW_PUNCH, {TOKEN_DOWN, TOKEN_FORWARD}, 2, 20, 0, fighting, scorpion_fire},
  {BUTTON_HIGH_KICK, {TOKEN_FORWARD, TOKEN_FORWARD}, 2, 24, 0, can_weapon, scorpion_weapon},
  {BUTTON_RUN, {0}, 0, 0, 0, mk4_can_sidestep, mk4_sidestep},
  {BUTTON_RUN, {0}, 0, 0, 0, can_pickup, scorpion_pickup},
};
static const ButtonTable scorpion_standing = {{[BUTTON_HIGH_PUNCH] = press_jab, [BUTTON_LOW_PUNCH] = press_lp, [BUTTON_HIGH_KICK] = press_kick, [BUTTON_LOW_KICK] = press_lk}};
static const ButtonTable scorpion_crouching = {{[BUTTON_HIGH_PUNCH] = press_uppercut, [BUTTON_LOW_PUNCH] = press_crouch_lp, [BUTTON_HIGH_KICK] = press_crouch_hk, [BUTTON_LOW_KICK] = press_crouch_lk}};
static const ButtonTable scorpion_jumping = {{[BUTTON_HIGH_PUNCH] = press_air_hp, [BUTTON_LOW_PUNCH] = press_air_lp, [BUTTON_HIGH_KICK] = press_air_hk, [BUTTON_LOW_KICK] = press_air_lk}};
static const Controls scorpion_controls = {commands, COUNT(commands), {&scorpion_standing, &scorpion_crouching, &scorpion_jumping}};
// clang-format on

/* Weapon scripts 0x4e62c0/0x4e62d8, 0x4ec8b0/0x4ec918 and 0x4e8850;
 * slash parameter blocks 0x4ec880/0x4ec898; native throw 0x498290. */
// clang-format off: Keep table entries on one row, including long rows.
static const AttackRecord sword_records[] = {
  {98304, -35389, 57016, 39321, 48, 12, 119, 0, 13107}, /* HP / LP */
  {124518, -34078, 83230, 51118, 49, 12, 119, 0, 13107}, /* back LP */
  {111411, -34078, 83230, 51118, 50, 12, 119, 0, 13107}, /* back HP */
  {78643, 1900, 193331, 24903, 99, 10, 118, 0, 3276}, /* spin */
  {110690, -60882, 68157, 62914, 156, 156, 4, 0, 11796}, /* air HP */
  {101580, -4587, 64225, 53739, 155, 155, 4, 0, 11796}, /* air LP */
  {57606, -37945, 73990, 35979, 154, 154, 0, ATTACK_MARK, 16384}, /* throw */
};
// clang-format on
#define SWORD_MOVE(name, clip_id, action_id) static const MoveDef name = {.clip = clip_id, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = action_id}
SWORD_MOVE(s_sword_hp, 235, ACTION_RETAIL_010E);
SWORD_MOVE(s_sword_lp, 236, ACTION_RETAIL_010E);
SWORD_MOVE(s_sword_back_lp, 237, ACTION_RETAIL_0112);
SWORD_MOVE(s_sword_back_hp, 238, ACTION_RETAIL_0112);
SWORD_MOVE(s_spin_start, 223, ACTION_RETAIL_0407);

static const WeaponDef sword;

static void sword_slash_build(Fighter *fighter, int first, bool hit);

static const WeaponDef sword = {0x60, sword_hp, sword_lp, sword_back_hp, sword_back_lp, sword_spin, weapon_throw, equip_sword, SCENE_MODEL_SWORD};

const CharacterDef mk4_scorpion = {GAME_MK4, 0, "SCORPION", &mk4_motion, moves, S_COUNT, &scorpion_controls, enter, -76873, mk4_walk, mk4_jump, NULL};

// clang-format off: Keep table entries on one row, including long rows.
/* Native-only blends; no VM timing tables. */
#define BLEND(clip_id, cursor) {{.clip = clip_id, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0700}, cursor, cursor, 1, 0x3333, 0x3333}
static const SceneClip burn_repeat_blend[] = {BLEND(390, 105)};
static const SceneClip burn_victory_blend[] = {BLEND(115, 0)};
static const SceneClip burn_victim_blend[] = {BLEND(206, 0)};
#undef BLEND
// clang-format on

/* Private move implementations share this character's data and declarations. */
#include "moves/air_attacks.inc"
#include "moves/common.inc"
#include "moves/controls.inc"
#include "moves/fan.inc"
#include "moves/fatality.inc"
#include "moves/fire.inc"
#include "moves/ground_attacks.inc"
#include "moves/kicks.inc"
#include "moves/punches.inc"
#include "moves/roundhouse.inc"
#include "moves/spear.inc"
#include "moves/spike.inc"
#include "moves/sweep.inc"
#include "moves/teleport.inc"
#include "moves/throws.inc"
#include "moves/uppercut.inc"
#include "moves/weapon.inc"
