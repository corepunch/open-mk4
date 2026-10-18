#include "character.h"

static const MoveDef s_idle = {.clip = CLIP_ASSET_IDLE, .last = MOVE_END};
static const MoveDef s_walk = {.clip = 265, .last = MOVE_END};
static const MoveDef s_crouch = {.clip = 259, .last = MOVE_END, .flags = SF_HOLD};
static const MoveDef s_jump = {.clip = 304, .last = MOVE_END};
static const MoveDef s_jab = {.clip = 294, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK, .action = ACTION_RETAIL_010B};
static const MoveDef s_high_kick = {.clip = 66, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK, .action = ACTION_RETAIL_0100};
static const MoveDef s_low_kick = {.clip = 70, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_LOW, .action = ACTION_RETAIL_0106};
static const MoveDef s_sweep = {.clip = 62, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_LOW, .action = ACTION_RETAIL_010C};
static const MoveDef s_uppercut = {.clip = 293, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_LAUNCH, .action = ACTION_RETAIL_0109};
static const MoveDef s_roundhouse = {.clip = 64, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK, .action = ACTION_RETAIL_010D};
static const AttackRecord s_roundhouse_record = {0x10f5c, -0xab85, 0xf0a3, 0x6b85, 15, 15, 5, 0, 0x2e14};
static const MoveDef s_air_attack = {.clip = 0, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK, .action = ACTION_RETAIL_0601};
static const MoveDef s_weapon_draw = {.clip = 244, .last = MOVE_END, .flags = SF_LOCK};
static const MoveDef s_fatality_1_opening = {.clip = 384, .last = MOVE_END, .flags = SF_LOCK};
static const MoveDef s_fatality_1_victim = {.clip = 204, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION};
static const MoveDef s_fatality_1_victim_alt = {.clip = 400, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION};
static const MoveDef s_fatality_2_opening = {.clip = 284, .last = MOVE_END, .flags = SF_LOCK};
static const MoveDef s_fatality_2_victim = {.clip = 556, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION};
static const MoveDef s_throw = {.clip = 83, .last = MOVE_END, .flags = SF_LOCK | SF_UNBLOCKABLE, .action = ACTION_RETAIL_0500};
static const MoveDef s_thrown = {.clip = 145, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION | SF_HOLD};
static const MoveDef s_lk_throw = {.clip = 37, .last = MOVE_END, .flags = SF_LOCK | SF_UNBLOCKABLE, .action = ACTION_RETAIL_0500};
static const MoveDef s_lk_thrown = {.clip = 157, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION | SF_HOLD};
static const MoveDef s_block = {.clip = 322, .last = MOVE_END, .flags = SF_HOLD};
static const MoveDef s_hit = {.clip = 247, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION | SF_HOLD};
static const MoveDef s_knockdown = {.clip = 186, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION | SF_WAIT_LAND};
static const MoveDef s_victory = {.clip = 412, .last = MOVE_END, .flags = SF_HOLD};
static const WeaponDef s_jax_weapon;
static void jax_weapon_action(Fighter *fighter);
static void jax_script_current_reaction(Fighter *fighter, int reaction);
static void jax_script_opponent_reaction(Fighter *fighter, int reaction);
static void jax_script_roundhouse_tail(Fighter *fighter);
static void jax_script_fatality_1_scene_signal(Fighter *fighter);
static void jax_script_fatality_1_finish(Fighter *fighter);
static void jax_script_fatality_2_task(Fighter *fighter, int task_type);
static void jax_script_fatality_2_scene_finish(Fighter *fighter);
static void jax_script_air_throw_ground_release(Fighter *fighter);
static void jax_script_jab_contact(Fighter *fighter);
static void jax_script_jab_followup(Fighter *fighter);
static void jax_script_cpu_projectile_spawn(Fighter *fighter, int task_type);

#include "generated/scripts.inc"
#include "generated/frames.inc"

enum {
  JAX_EVENT_FATALITY_1_SIGNAL = 1,
  JAX_EVENT_FATALITY_1_FINISH,
  JAX_EVENT_FATALITY_2_TASK,
  JAX_EVENT_FATALITY_2_FINISH,
  JAX_EVENT_JAB_CONTACT,
  JAX_EVENT_JAB_RECOVERY
};

// clang-format off: Keep table entries on one row, including long rows.
static const AttackRecord s_jax_jab_record = {0xe872, -0x3f7c, 0xb84b, 0x56c8, 10, 10, 40, 0, 0x170a};
// clang-format on

static const GroundAttack jax_ground_attacks[8] = {
    [7] = {.active = 14, .end_miss = 39, .end_contact = 67},
};
static const NativeGroundProfile jax_ground_profile = {.attacks = jax_ground_attacks,
    .ready = jax_ready,
    .fast = 4,
    .round_pre = 19,
    .round_first = 20,
    .round_last = 44,
    .roundhouse_record = &s_roundhouse_record};

static const WeaponDef s_jax_weapon = {
    0x69, jax_weapon_hp, jax_weapon_lp, jax_weapon_back_hp, jax_weapon_back_lp, NULL, weapon_throw, jax_weapon_equip, SCENE_MODEL_SWORD};

static const ButtonTable s_standing = {
    {[BUTTON_HIGH_PUNCH] = jax_press_hp, [BUTTON_LOW_PUNCH] = jax_press_lp, [BUTTON_HIGH_KICK] = jax_press_hk, [BUTTON_LOW_KICK] = jax_press_lk}};
static const ButtonTable s_crouching = {{[BUTTON_HIGH_PUNCH] = jax_press_uppercut,
    [BUTTON_LOW_PUNCH] = jax_press_uppercut,
    [BUTTON_HIGH_KICK] = jax_press_sweep,
    [BUTTON_LOW_KICK] = jax_press_sweep}};
static const ButtonTable s_jumping = {{[BUTTON_HIGH_PUNCH] = jax_press_air_hp,
    [BUTTON_LOW_PUNCH] = jax_press_air_lp,
    [BUTTON_HIGH_KICK] = jax_press_air_kick,
    [BUTTON_LOW_KICK] = jax_press_air_kick}};
static const Controls s_controls = {NULL, 0, {&s_standing, &s_crouching, &s_jumping}};

static const FighterFn enter[S_COUNT] = {
    [S_IDLE] = jax_idle,
    [S_WALK] = jax_walk,
    [S_CROUCH] = jax_crouch,
    [S_JUMP] = mk4_jump,
    [S_PUNCH] = jax_jab,
    [S_KICK] = jax_high_kick,
    [S_SWEEP] = jax_sweep,
    [S_UPPERCUT] = jax_uppercut,
    [S_THROW] = jax_lp_throw,
    [S_BLOCK] = jax_block,
    [S_HIT] = NULL,
    [S_KNOCKDOWN] = NULL,
    [S_VICTORY] = NULL,
};

// clang-format off: Keep table entries on one row, including long rows.
static const MoveDef *const moves[S_COUNT] = {
  [S_IDLE] = &s_idle,
  [S_WALK] = &s_walk,
  [S_CROUCH] = &s_crouch,
  [S_JUMP] = &s_jump,
  [S_PUNCH] = &s_jab,
  [S_KICK] = &s_high_kick,
  [S_SWEEP] = &s_sweep,
  [S_UPPERCUT] = &s_uppercut,
  [S_THROW] = &s_throw,
  [S_BLOCK] = &s_block,
  [S_HIT] = &s_hit,
  [S_KNOCKDOWN] = &s_knockdown,
  [S_VICTORY] = &s_victory,
};
// clang-format on

const CharacterDef mk4_jax = {GAME_MK4, 13, "JAX", &mk4_motion, moves, S_COUNT, &s_controls, enter, -75169, mk4_walk, mk4_jump, NULL};

/* Private move implementations share this character's data and declarations. */
#include "moves/air_attacks.inc"
#include "moves/common.inc"
#include "moves/controls.inc"
#include "moves/fan.inc"
#include "moves/fatality.inc"
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
