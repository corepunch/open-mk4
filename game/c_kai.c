#include "game.h"
#include "native.h"
#include "math_util.h"
#include "mk4_characters.h"
#include "script.h"
#include <string.h>

/* Kai's move clips and contact timing come from the slot-11 retail tables.
 * Script bodies are emitted from tools/ports/kai.json. */
static const MoveDef s_idle = {CLIP_ASSET_IDLE, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_walk = {265, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_crouch = {259, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_jump = {304, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_jab = {294, 0, MOVE_END, 5, 4, 12, 7, 19, 1.25f, 0.13f, SF_ATTACK | SF_LOCK, NULL, 0};
static const MoveDef s_high_kick = {71, 0, MOVE_END, 10, 5, 18, 12, 28, 1.85f, 0.32f, SF_ATTACK | SF_LOCK, NULL, 0};
static const MoveDef s_low_kick = {70, 0, MOVE_END, 9, 5, 18, 12, 28, 1.55f, 0.27f, SF_ATTACK | SF_LOCK | SF_LOW, NULL, 0};
static const MoveDef s_sweep = {62, 0, MOVE_END, 11, 5, 23, 10, 44, 1.65f, 0.25f, SF_ATTACK | SF_LOCK | SF_LOW | SF_LAUNCH, NULL, 0};
static const MoveDef s_uppercut = {293, 0, MOVE_END, 9, 5, 30, 18, 20, 1.35f, 0.5f, SF_ATTACK | SF_LOCK | SF_LAUNCH, NULL, 0};
static const MoveDef s_throw = {37, 0, MOVE_END, 6, 2, 25, 14, 66, 0.95f, 0.45f, SF_ATTACK | SF_LOCK | SF_LAUNCH | SF_UNBLOCKABLE, NULL, 0};
static const MoveDef s_thrown = {157, 0, MOVE_END, 0, 0, 54, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_HOLD, NULL, 0};
static const MoveDef s_lk_thrown = {.clip = 158, .last = 47, .flags = SF_LOCK | SF_REACTION};
static const MoveDef s_lp_thrown = {.clip = 153, .last = 26, .flags = SF_LOCK | SF_REACTION};
static const MoveDef s_air_attack = {61, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_ATTACK | SF_LOCK, NULL, 0};
static const MoveDef s_air_throw = {82, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_ATTACK | SF_LOCK | SF_UNBLOCKABLE, NULL, 0};
static const MoveDef s_air_thrown = {202, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION, NULL, 0};
static const MoveDef s_block = {322, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_hit = {247, 0, MOVE_END, 0, 0, 24, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_HOLD, NULL, 0};
static const MoveDef s_knockdown = {186, 0, MOVE_END, 0, 0, 54, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_WAIT_LAND, NULL, 0};
static const MoveDef s_victory = {122, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_stage_event_clip = {.clip = 0, .last = 0, .flags = SF_LOCK};
static const AttackRecord s_kai_attack_b9 = {0x18000, -0xa8b4, 0xe147, 0x13ced, 127, 127, 5, 0, 0x23d7};
static const AttackRecord s_kai_attack_b10 = {0x1851e, -0xbd2f, 0x10000, 0x9958, 127, 127, 5, 0, 0x1c28};

static void kai_ready(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    fighter_start_move(fighter, S_JUMP, roster_def(fighter->roster)->moves[S_JUMP]);
  } else {
    fighter_enter_state(fighter, S_IDLE);
  }
}

#include "generated/kai.inc"
#include "generated/frames_kai.inc"

static void kai_start_script(Fighter *fighter, StateId state, const MoveDef *move, FighterFn script) {
  fighter_start_move(fighter, state, move);
  fighter_start_fiber(fighter, script, NULL);
}

static void kai_idle(Fighter *fighter) {
  fighter_start_move(fighter, S_IDLE, &s_idle);
}

static void kai_walk(Fighter *fighter) {
  fighter_start_move(fighter, S_WALK, &s_walk);
}

static void kai_crouch(Fighter *fighter) {
  fighter_start_move(fighter, S_CROUCH, &s_crouch);
}

static void kai_block(Fighter *fighter) {
  fighter_start_move(fighter, S_BLOCK, &s_block);
}

static void kai_jab(Fighter *fighter) {
  kai_start_script(fighter, S_PUNCH, &s_jab, retail_script_004e62f0);
}

static void kai_high_punch(Fighter *fighter) {
  kai_start_script(fighter, S_PUNCH, &s_air_attack, retail_script_004e8210);
}

static void kai_low_punch(Fighter *fighter) {
  kai_start_script(fighter, S_PUNCH, &s_jab, retail_script_004e62f0);
}

static void kai_draw_weapon(Fighter *fighter) {
  fighter_start_fiber(fighter, retail_script_004e62c0, NULL);
}

static void kai_input_478df0(Fighter *fighter) {
  kai_start_script(fighter, S_KICK, &s_low_kick, retail_script_004e8870);
}

static void kai_input_47bd10(Fighter *fighter) {
  kai_high_punch(fighter);
}

static void kai_input_47bd40(Fighter *fighter) {
  fighter_start_fiber(fighter, retail_script_004e8238, NULL);
}

static void kai_input_47c520(Fighter *fighter) {
  kai_start_script(fighter, S_KICK, &s_low_kick, retail_script_004e8320);
}

static void kai_input_47cbf0(Fighter *fighter) {
  fighter_start_fiber(fighter, retail_script_004e88c8, NULL);
}

static void kai_input_47cc10(Fighter *fighter) {
  fighter_start_fiber(fighter, retail_script_004e8358, NULL);
}

static void kai_high_kick(Fighter *fighter);
static void kai_uppercut(Fighter *fighter);
static void kai_air_hp(Fighter *fighter);
static void kai_air_lp(Fighter *fighter);
static void kai_air_kick(Fighter *fighter);
static void kai_air_diagonal_lk(Fighter *fighter);

static void kai_press_high_punch(Fighter *fighter) {
  fighter_interrupt(fighter, kai_input_47bd10);
}

static void kai_press_low_punch(Fighter *fighter) {
  fighter_interrupt(fighter, kai_low_punch);
}

static void kai_press_high_kick(Fighter *fighter) {
  fighter_interrupt(fighter, kai_high_kick);
}

static void kai_press_low_kick(Fighter *fighter) {
  fighter_interrupt(fighter, kai_input_47c520);
}

static void kai_press_uppercut(Fighter *fighter) {
  fighter_interrupt(fighter, kai_uppercut);
}

static void kai_press_sweep(Fighter *fighter) {
  fighter_interrupt(fighter, kai_input_478df0);
}

static void kai_press_air_hp(Fighter *fighter) {
  fighter_interrupt(fighter, kai_air_hp);
}

static void kai_press_air_lp(Fighter *fighter) {
  fighter_interrupt(fighter, kai_air_lp);
}

static void kai_press_air_hk(Fighter *fighter) {
  fighter_interrupt(fighter, kai_air_kick);
}

static void kai_press_air_lk(Fighter *fighter) {
  fighter_interrupt(fighter, kai_air_diagonal_lk);
}

// clang-format off: Keep table entries on one row, including long rows.
static const ButtonTable kai_standing = {{[BUTTON_HIGH_PUNCH] = kai_press_high_punch, [BUTTON_LOW_PUNCH] = kai_press_low_punch, [BUTTON_HIGH_KICK] = kai_press_high_kick, [BUTTON_LOW_KICK] = kai_press_low_kick}};
static const ButtonTable kai_crouching = {{[BUTTON_HIGH_PUNCH] = kai_press_uppercut, [BUTTON_LOW_PUNCH] = kai_press_uppercut, [BUTTON_HIGH_KICK] = kai_press_sweep, [BUTTON_LOW_KICK] = kai_press_sweep}};
static const ButtonTable kai_jumping = {{[BUTTON_HIGH_PUNCH] = kai_press_air_hp, [BUTTON_LOW_PUNCH] = kai_press_air_lp, [BUTTON_HIGH_KICK] = kai_press_air_hk, [BUTTON_LOW_KICK] = kai_press_air_lk}};
// clang-format on

static const Controls kai_controls = {NULL, 0, {[STANCE_STANDING] = &kai_standing, [STANCE_CROUCHING] = &kai_crouching, [STANCE_JUMP] = &kai_jumping}};

static void kai_high_kick(Fighter *fighter) {
  kai_start_script(fighter, S_KICK, &s_high_kick, retail_script_004e5ca0);
}

static void kai_low_kick(Fighter *fighter) {
  kai_start_script(fighter, S_KICK, &s_low_kick, retail_script_004e5d90);
}

static void kai_sweep(Fighter *fighter) {
  kai_start_script(fighter, S_SWEEP, &s_sweep, retail_script_004e6798);
}

static void kai_uppercut(Fighter *fighter) {
  kai_start_script(fighter, S_UPPERCUT, &s_uppercut, retail_script_004e62a0);
}

static void kai_roundhouse(Fighter *fighter) {
  kai_start_script(fighter, S_KICK, &s_high_kick, retail_script_004e6800);
}

static void kai_air_hp(Fighter *fighter) {
  kai_start_script(fighter, S_JUMP, &s_air_attack, retail_script_004e6658);
}

static void kai_air_lp(Fighter *fighter) {
  kai_start_script(fighter, S_JUMP, &s_air_attack, retail_script_004e6710);
}

static void kai_air_kick(Fighter *fighter) {
  kai_start_script(fighter, S_JUMP, &s_air_attack, retail_script_004e61f8);
}

static void kai_air_diagonal_hk(Fighter *fighter) {
  kai_start_script(fighter, S_JUMP, &s_air_attack, retail_script_004e6500);
}

static void kai_air_diagonal_lk(Fighter *fighter) {
  kai_start_script(fighter, S_JUMP, &s_air_attack, retail_script_004e63d8);
}

static void kai_lp_throw(Fighter *fighter) {
  mk4_script_throw_start(fighter, &s_throw, &s_thrown, retail_script_004de798);
}

static void kai_lk_throw(Fighter *fighter) {
  mk4_script_throw_start(fighter, &s_throw, &s_thrown, retail_script_004de2f8);
}

static void kai_air_throw(Fighter *fighter) {
  mk4_air_throw_generated(fighter, retail_script_004e5e28, retail_script_004e9910);
}

static void kai_fan_start(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_fiber(victim, retail_script_004e0188, NULL);
  retail_script_004e0118(fighter);
}

static void kai_fan_fatality(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, FINISH_FAN)) {
    kai_ready(fighter);
    return;
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  mk4_script_scene_pose(fighter, 415, 130, 0);
  mk4_script_scene_pose(victim, 416, 108, 0);
  fighter_start_fiber(fighter, kai_fan_start, NULL);
}

static void kai_spike_fatality(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, FINISH_SPIKE)) {
    kai_ready(fighter);
    return;
  }
  fighter_start_fiber(fighter, retail_script_004e6950, NULL);
}

static void kai_script_lk_return(Fighter *fighter) {
  /* Retail 42be10 enters the follow-up VM root after its global task checks. */
  retail_script_004de358(fighter);
}

static void kai_script_lk_finish(Fighter *fighter) {
  /* Retail 42bf00 coordinates global task callbacks. The portable fiber
   * scheduler owns that queue; close the actor pair at this boundary. */
  mk4_script_pair_cleanup(fighter);
  fighter_fiber_finish(kai_ready);
}

static void kai_script_aerial_transition(Fighter *fighter) {
  /* 47bd40 resumes Kai's high-punch move at its generated follow-up stream. */
  retail_script_004e8238(fighter);
}

static void kai_script_aerial_recovery(Fighter *fighter) {
  /* 47bf90 selects the 4e8280 recovery suffix after its aerial clip. */
  memset(fighter->body.vel, 0, sizeof fighter->body.vel);
  memset(fighter->body.spin, 0, sizeof fighter->body.spin);
  fighter->body.gravity = 0;
  native_delay(fighter, 8);
  fighter->flags &= ~(unsigned)FF_BUSY;
  fighter->body.gravity = 0x51e;
  mk4_script_wait_land(fighter);
  fighter_fiber_finish(kai_ready);
}

static void kai_contact_frame(Fighter *fighter) {
  if (!fighter->connected) {
    battle_contact(fighter, fighter->native_record);
  }
}

static void kai_script_contact_b9_continuation(Fighter *fighter) {
  native_play_to(fighter, 29, 1);
  fighter->native_record = &s_kai_attack_b9;
  native_calls(fighter, 5, 1, kai_contact_frame);
  retail_script_004e88c8(fighter);
}

static void kai_script_contact_b10_continuation(Fighter *fighter) {
  native_play_to(fighter, 9, 1);
  fighter->native_record = &s_kai_attack_b10;
  native_begin(fighter, 1, 3, NULL);
  for (int frame = 1; frame <= 3; ++frame) {
    native_frame(fighter, frame, frame == 1 ? 10 : 11, kai_contact_frame);
  }
  native_wait(fighter);
  retail_script_004e8390(fighter);
}

enum {
  KAI_STAGE_EVENT_PLAY_CLIP = 1,
  KAI_STAGE_EVENT_SETTLE
};

static bool kai_stage_event_step(BattleEvent *event, Fighter *owner, Fighter *opponent) {
  (void)opponent;
  int clip = event->data[0], sample = event->data[1], last = event->data[2];
  fighter_start_move(owner, S_KICK, &s_stage_event_clip);
  owner->playback_move = *fighter_move(owner);
  owner->playback_move.clip = clip;
  owner->playback_move.first = 0;
  owner->playback_move.last = last;
  owner->playback_move.sequence = NULL;
  owner->playback_move.flags = SF_LOCK;
  owner->current_move = &owner->playback_move;
  owner->sample_override = sample;
  owner->action = 0x200d;
  native_begin(owner, 1, last - sample + 2, kai_ready);
  for (int frame = 1; frame <= last - sample + 2; ++frame) {
    native_frame(owner, frame, mini(last, sample + frame), NULL);
  }
  native_play(owner, false);
  return false;
}

static void kai_script_stage_event(Fighter *fighter) {
  const MoveDef *move = fighter_move(fighter);
  require((move->clip == 1 && move->last == 19) || (move->clip == 9 && move->last == 65), "Kai stage event needs clip 1 or clip 9 context");
  require(fighter->sample_override >= 0 && fighter->sample_override <= move->last, "Kai stage event needs a valid clip sample");
  BattleEvent *event = battle_event_start(fighter, kai_stage_event_step, false, true);
  event->data[0] = move->clip;
  event->data[1] = fighter->sample_override;
  event->data[2] = move->last;
}

static void kai_script_lp_select(Fighter *fighter) {
  q16 along = 0;
  int64_t distance2 = 0;
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    q16 delta = fighter->saved_motion[axis];
    along += q16_mul(delta, fighter_axis(fighter, axis));
    distance2 += ((int64_t)delta * delta) >> 16;
  }
  if (along >= 0 && distance2 < 0x280000) {
    retail_script_004de6b8(fighter);
  } else {
    retail_script_004de610(fighter);
  }
}

static void kai_script_current_reaction(Fighter *fighter, int reaction) {
  battle_react(fighter, reaction);
}

static void kai_script_lp_extended_finish(Fighter *fighter) {
  Fighter *victim = mk4_script_release_pair(fighter);
  victim->reaction = 109;
  victim->flags |= FF_REACTING;
  fighter_start_move(victim, S_KNOCKDOWN, &s_lk_thrown);
  battle_react(victim, 109);
  fighter_fiber_finish(kai_ready);
}

static void kai_script_lp_short_finish(Fighter *fighter) {
  Fighter *victim = mk4_script_release_pair(fighter);
  victim->reaction = 102;
  victim->flags |= FF_REACTING;
  fighter_start_move(victim, S_KNOCKDOWN, &s_lp_thrown);
  battle_react(victim, 102);
  fighter_fiber_finish(kai_ready);
}

static void kai_script_opponent_reaction(Fighter *fighter, int reaction) {
  if (reaction == 175) {
    return;
  }
  if (reaction == 77) {
    Fighter *victim = mk4_script_release_pair(fighter);
    victim->reaction = reaction;
    victim->flags |= FF_REACTING;
    fighter_fiber_finish(kai_ready);
    return;
  }
  require(false, "Unexpected Kai paired reaction");
}

static void kai_script_draw_equip(Fighter *fighter) {
  /* 46d260's actor-local continuation is exactly the generated 4e62d8 body.
   * Its global task queue bookkeeping is owned by the portable fiber scheduler. */
  retail_script_004e62d8(fighter);
}

static void kai_script_air_release(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_fiber(victim, retail_script_004e9910, NULL);
  mk4_script_air_throw_release(fighter);
  retail_script_004e5e50(fighter);
}

static void kai_script_fan_tail(Fighter *fighter) {
  mk4_script_fan_tail(fighter, &retail_script_004e0138_repeat_phase, retail_script_004e0148, retail_script_004e01a0);
}

static void kai_script_fan_cycle(Fighter *fighter) {
  mk4_script_fan_cycle(fighter);
}

static void kai_script_fan_victory(Fighter *fighter) {
  retail_script_004e0158(fighter);
}

static void kai_script_fan_victim_end(Fighter *fighter) {
  mk4_script_fan_victim_end(fighter);
}

static void kai_script_kick_finish(Fighter *fighter) {
  retail_script_004e5c70(fighter);
}

static void kai_script_air_finish(Fighter *fighter) {
  mk4_script_finish_clip(fighter, 1);
  fighter_fiber_finish(mk4_land);
}

static void kai_script_finish(Fighter *fighter) {
  mk4_script_finish_clip(fighter, 1);
  fighter_fiber_finish(kai_ready);
}

static void kai_script_roundhouse_start(Fighter *fighter) {
  if (fighter_fast_start(fighter)) {
    retail_script_004e6860(fighter);
  } else {
    retail_script_004e6878(fighter);
  }
}

static void kai_script_spike_tail(Fighter *fighter) {
  mk4_script_spike_tail(fighter);
}

static void kai_script_thrown_return(Fighter *fighter) {
  mk4_script_thrown_finish(fighter);
}

static const FighterFn enter[S_COUNT] = {
    [S_IDLE] = kai_idle,
    [S_WALK] = kai_walk,
    [S_CROUCH] = kai_crouch,
    [S_JUMP] = NULL,
    [S_PUNCH] = kai_jab,
    [S_KICK] = kai_high_kick,
    [S_SWEEP] = kai_sweep,
    [S_UPPERCUT] = kai_uppercut,
    [S_THROW] = kai_lp_throw,
    [S_BLOCK] = kai_block,
    [S_HIT] = NULL,
    [S_KNOCKDOWN] = NULL,
    [S_VICTORY] = kai_ready,
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

/* Encoded Kai weapon descriptor 0x4ecef8; attack moves remain routed through
 * the selected weapon's native inventory entry until their strike records
 * are ported. */
const uint32_t kai_weapon_descriptor[] = {0x63, 0x141b33, 0x142daf, 0x13b330, 0x13eeb6, 0x13ee98, 0x13ee9b, 0xffffc000};

const CharacterDef mk4_kai = {GAME_MK4, 11, "KAI", &mk4_motion, moves, S_COUNT, &kai_controls, enter, -74829, mk4_walk, mk4_jump, NULL};
