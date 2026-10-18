#include "character.h"

/* Combat values remain the native approximation until measured against retail. */
static const MoveDef s_idle = {CLIP_ASSET_IDLE, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_walk = {267, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_crouch = {259, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_jump = {304, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_jab = {294, 0, MOVE_END, 5, 4, 12, 7, 19, 1.25f, 0.13f, SF_ATTACK | SF_LOCK, NULL, 0};
static const MoveDef s_kick = {71, 0, MOVE_END, 10, 5, 18, 12, 25, 1.85f, 0.32f, SF_ATTACK | SF_LOCK, NULL, 0};
static const MoveDef s_sweep = {63, 0, MOVE_END, 11, 5, 23, 10, 45, 1.65f, 0.25f, SF_ATTACK | SF_LOCK | SF_LOW | SF_LAUNCH, NULL, 0};
static const MoveDef s_uppercut = {293, 0, MOVE_END, 9, 5, 30, 18, 60, 1.35f, 0.5f, SF_ATTACK | SF_LOCK | SF_LAUNCH, NULL, 0};
static const MoveDef s_throw = {75, 0, MOVE_END, 6, 2, 25, 14, 42, 0.95f, 0.45f, SF_ATTACK | SF_LOCK | SF_LAUNCH | SF_UNBLOCKABLE, NULL, 0};
static const MoveDef s_block = {322, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_hit = {247, 0, MOVE_END, 0, 0, 24, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_HOLD, NULL, 0};
static const MoveDef s_knockdown = {186, 0, MOVE_END, 0, 0, 54, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_WAIT_LAND, NULL, 0};
static const MoveDef s_victory = {111, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_air_throw = {82, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_ATTACK | SF_LOCK | SF_UNBLOCKABLE, NULL, 0};
static const MoveDef s_air_thrown = {202, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION, NULL, 0};
static const MoveDef s_cpu_kick_script = {308, 0, 24, 0, 0, 18, 11, 0, 0.f, 0.f, SF_LOCK, NULL, 0};
static const MoveDef s_powered_punch_script = {85, 0, 54, 0, 0, 18, 9, 0, 0.f, 0.f, SF_LOCK, NULL, 0};
static const MoveDef s_powered_punch_finish = {105, 0, 29, 0, 0, 19, 12, 0, 0.f, 0.f, SF_LOCK, NULL, 0};

static void cage_script_cpu_kick_tail(Fighter *fighter);
static void cage_script_powered_punch_tail(Fighter *fighter);
static void cage_script_powered_punch_finish_tail(Fighter *fighter);
static void cage_script_fatality_1_signal(Fighter *fighter);
static void cage_script_fatality_2_continue(Fighter *fighter);

#include "generated/scripts.inc"
#include "generated/frames.inc"

/* Contact records from Cage's 53d2b8/53d268 tables, selected by the
 * generated controller callbacks 479b70/479f00/47a500. */
static const AttackRecord s_cage_cpu_kick_contact = {0x15c28, -0x35c2, 0x104df, 0x5c28, 17, 10, 6, 0, 0x1c28};
static const AttackRecord s_cage_powered_punch_contact = {0x107ae, -0x9c28, 0xdc28, 0xa8f5, 38, 10, 143, 0, 0x170a};
static const AttackRecord s_cage_powered_finish_contact = {0x12b85, -0x4f5c, 0x10a3d, 0xf5c2, 39, 39, 4, 0, 0x1eb8};

enum {
  CAGE_EVENT_CPU_KICK_RECOVERY = 1,
  CAGE_EVENT_POWERED_RECOVERY,
  CAGE_EVENT_FINISH_RECOVERY,
  CAGE_EVENT_SCENE_LOOP,
  CAGE_EVENT_FATALITY_1 = 17,
  CAGE_EVENT_FATALITY_2
};

const uint32_t cage_weapon_descriptor[] = {0x6e, 0x1430cb, 0x144359, 0x13b318, 0x13eeb9, 0x13ee9e, 0x13eea1, 0xffffb334};

// clang-format off: Keep table entries on one row, including long rows.
static const ButtonTable cage_standing = {{[BUTTON_HIGH_PUNCH] = cage_press_high_punch, [BUTTON_LOW_PUNCH] = cage_press_low_punch, [BUTTON_HIGH_KICK] = cage_press_high_kick, [BUTTON_LOW_KICK] = cage_press_low_kick}};
static const ButtonTable cage_crouching = {{[BUTTON_HIGH_PUNCH] = cage_press_crouching_hp, [BUTTON_LOW_PUNCH] = cage_press_crouching_hp, [BUTTON_HIGH_KICK] = cage_press_crouching_kick, [BUTTON_LOW_KICK] = cage_press_crouching_kick}};
static const ButtonTable cage_straight_air = {{[BUTTON_HIGH_PUNCH] = cage_press_air_hp, [BUTTON_LOW_PUNCH] = cage_press_air_lp, [BUTTON_HIGH_KICK] = cage_press_air_hk, [BUTTON_LOW_KICK] = cage_press_air_lk}};
static const ButtonTable cage_air_punch_followup = {{[BUTTON_LOW_PUNCH] = cage_press_air_lp}};
static const ButtonTable cage_diagonal_air = {{[BUTTON_HIGH_PUNCH] = cage_press_air_hp, [BUTTON_LOW_PUNCH] = cage_press_air_lp, [BUTTON_HIGH_KICK] = cage_press_air_hk, [BUTTON_LOW_KICK] = cage_press_air_lk}};
static const ButtonTable cage_close_hp_followup = {{[BUTTON_HIGH_PUNCH] = cage_press_high_punch}};
static const ButtonTable cage_combo_hp_followup = {{[BUTTON_HIGH_PUNCH] = cage_press_high_punch}};
static const ButtonTable cage_close_hk_followup = {{[BUTTON_HIGH_KICK] = cage_press_high_kick}};
static const ButtonTable cage_air_followup = {{[BUTTON_HIGH_KICK] = cage_press_air_hk, [BUTTON_LOW_KICK] = cage_press_air_lk}};
// clang-format on

static const Controls cage_controls = {NULL, 0,
    {
        [STANCE_STANDING] = &cage_standing,
        [STANCE_CROUCHING] = &cage_crouching,
        [STANCE_JUMP] = &cage_straight_air,
    }};

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

const CharacterDef mk4_cage = {GAME_MK4, 14, "CAGE", &mk4_motion, moves, S_COUNT, &cage_controls, NULL, -74645, mk4_walk, mk4_jump, NULL};

/* Private move implementations share this character's data and declarations. */
#include "moves/air_attacks.inc"
#include "moves/common.inc"
#include "moves/fan.inc"
#include "moves/fatality.inc"
#include "moves/kicks.inc"
#include "moves/projectile.inc"
#include "moves/punches.inc"
#include "moves/roundhouse.inc"
#include "moves/spike.inc"
#include "moves/sweep.inc"
#include "moves/throws.inc"
#include "moves/uppercut.inc"
