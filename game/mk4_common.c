#include "game.h"
#include "native.h"
#include "script.h"
#include "mk4_characters.h"
#include "math_util.h"
#include <string.h>

/* Approximate stance tables for fighters whose retail moves are not yet
 * ported. They follow the retail buttons from the shared tables in
 * docs/scorpion/input-and-commands.md section 6: back+LK sweeps, crouching
 * HP uppercuts and a close LP throws. Other selections use the generic move
 * for the button. A character with recovered moves owns its tables. */
#define ENTER_STATE(name, state)                                                                                                                               \
  static void enter_##name(Fighter *fighter) {                                                                                                                 \
    fighter_enter_state(fighter, state);                                                                                                                       \
  }
ENTER_STATE(punch, S_PUNCH)
ENTER_STATE(kick, S_KICK)
ENTER_STATE(sweep, S_SWEEP)
ENTER_STATE(uppercut, S_UPPERCUT)
ENTER_STATE(throw, S_THROW)

/* 0xb333 Q16: retail close-LP throw distance. */
#define MK4_THROW_RANGE 0.7f

static void press_punch(Fighter *fighter) {
  fighter_interrupt(fighter, enter_punch);
}

static void press_low_punch(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_distance(fighter) <= MK4_THROW_RANGE ? enter_throw : enter_punch);
}

static void press_kick(Fighter *fighter) {
  fighter_interrupt(fighter, enter_kick);
}

static void press_low_kick(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? enter_sweep : enter_kick);
}

static void press_uppercut(Fighter *fighter) {
  fighter_interrupt(fighter, enter_uppercut);
}

static void press_sweep(Fighter *fighter) {
  fighter_interrupt(fighter, enter_sweep);
}

// clang-format off: Keep table entries on one row, including long rows.
static const ButtonTable standing = {{[BUTTON_HIGH_PUNCH] = press_punch, [BUTTON_LOW_PUNCH] = press_low_punch, [BUTTON_HIGH_KICK] = press_kick, [BUTTON_LOW_KICK] = press_low_kick}};
static const ButtonTable crouching = {{[BUTTON_HIGH_PUNCH] = press_uppercut, [BUTTON_LOW_PUNCH] = press_uppercut, [BUTTON_HIGH_KICK] = press_sweep, [BUTTON_LOW_KICK] = press_sweep}};
static const ButtonTable jumping = {{[BUTTON_HIGH_PUNCH] = press_punch, [BUTTON_LOW_PUNCH] = press_punch, [BUTTON_HIGH_KICK] = press_kick, [BUTTON_LOW_KICK] = press_kick}};
// clang-format on

const Controls mk4_controls = {NULL, 0, {[STANCE_STANDING] = &standing, [STANCE_CROUCHING] = &crouching, [STANCE_JUMP] = &jumping}};

/* 460570/460710, 491780: forward walking .06, backward .7 scale.
 * 460980/4609f0: .13 running, 2% energy per tick, 40-tick recovery delay.
 * 422800 restores 5% after that delay. Straight jump: 46f7e0. */
const Motion mk4_motion = {0xf5c, 0x2147, 0x30a3, 0x28f};

#define WALK(c) {.clip = c, .last = MOVE_END}
// clang-format off: Keep table entries on one row, including long rows.
static const MoveDef walk_gaits[][2] = {
  {WALK(269), WALK(274)},
  {WALK(267), WALK(272)},
  {WALK(267), WALK(272)},
  {WALK(266), WALK(271)},
  {WALK(267), WALK(272)},
  {WALK(267), WALK(272)},
  {WALK(267), WALK(272)},
  {WALK(267), WALK(272)},
  {WALK(267), WALK(272)},
  {WALK(267), WALK(272)},
  {WALK(268), WALK(273)},
  {WALK(265), WALK(270)},
  {WALK(267), WALK(272)},
  {WALK(265), WALK(270)},
  {WALK(267), WALK(272)},
  {WALK(590), WALK(589)},
};
// clang-format on
static const MoveDef run_open = WALK(317);
static const MoveDef run_cycle = WALK(307);
#undef WALK

void mk4_walk(Fighter *fighter) {
  int direction = !!(fighter->held & BUTTON_BIT(BUTTON_RIGHT)) - !!(fighter->held & BUTTON_BIT(BUTTON_LEFT));
  bool forward = direction * fighter->facing > 0;
  bool run = forward && (fighter->held & BUTTON_BIT(BUTTON_RUN)) && fighter->run_energy > 0 && fighter->roster != 15;
  int character = roster_def(fighter->roster)->asset;
  character = character == 16 ? 2 : character == 17 ? 7 : character;
  if (run) {
    bool continuing = fighter->state == S_WALK && (fighter->current_move == &run_open || fighter->current_move == &run_cycle);
    const MoveDef *move = continuing ? &run_cycle : &run_open;
    if (fighter->current_move != move) {
      fighter_start_move(fighter, S_WALK, move);
    }
    if (fighter->current_mmove == NULL) {
      if (continuing) {
        native_begin(fighter, 1, 13, NULL);
        for (int frame = 1; frame <= 13; ++frame) {
          native_frame(fighter, frame, 11 + frame % 13, NULL);
        }
        native_play(fighter, false);
      }
      fighter->sample_override = continuing ? 11 : 0;
    }
    if (fighter->human) {
      fighter->run_energy = maxi(0, fighter->run_energy - 0x51e);
      fighter->run_delay = 40;
    }
    fighter_set_speed(fighter, mk4_motion.run);
    fighter->body.walking = true;
    return;
  }
  if (fighter->armed) {
    weapon_stance(fighter, S_WALK);
  } else {
    const MoveDef *move = &walk_gaits[character][forward ? 0 : 1];
    if (fighter->current_move != move || fighter->state != S_WALK) {
      fighter_start_move(fighter, S_WALK, move);
    }
  }
  int count = fighter->assets != NULL ? (int)fighter_clip(fighter->assets, fighter)->frames.count : 30;
  if (fighter->current_mmove == NULL) {
    native_begin(fighter, 1, count, NULL);
    for (int frame = 1; frame <= count; ++frame) {
      native_frame(fighter, frame, frame % count, NULL);
    }
    native_play(fighter, false);
    fighter->sample_override = 0;
  }
  q16 speed = forward ? mk4_motion.walk : -q16_mul(mk4_motion.walk, 0xb333);
  fighter_set_speed(fighter, speed);
  fighter->body.walking = true;
}

/* 46f7e0 / 46fdf0 / 46fe80: human standing and directional jumps.
 * 46ff20 opens two tuck samples, then four more with diagonal buttons;
 * 470080 spins to the apex; 470140/170 retract at Y >= -2.3. */
static const MoveDef jump_straight = {.clip = 304, .last = MOVE_END, .action = 0x2007};
static const MoveDef jump_forward = {.clip = 301, .last = MOVE_END, .flags = SF_LOCK, .action = 0x2009};
static const MoveDef jump_backward = {.clip = 301, .last = MOVE_END, .flags = SF_LOCK, .action = 0x200a};
static const MoveDef jump_landing = {.clip = 304, .first = 18, .last = 27, .flags = SF_LOCK, .action = 0x2008};

static void start_flip(Fighter *fighter, int direction, bool from_straight);
static void jump_physics(Fighter *fighter);

static void stance_blend_add(Fighter *fighter) {
  fighter->blend_weight += 0x3333;
}

static void stance_blend_done(Fighter *fighter) {
  fighter_stop_mmove(fighter);
  fighter->blend_duration = 0;
  fighter->blend_weight = -1;
  fighter->sample_override = -1;
  fighter->ticks = 0;
}

/* 46f650/6e0/740: recover the current pose into the character's
 * standing descriptor with weights .2, .4, .6, .8, 1.0. Stance
 * controls remain available throughout this interruptible blend. */
void mk4_return_to_stance(Fighter *fighter) {
  int clip = fighter->assets != NULL ? fighter_clip(fighter->assets, fighter)->id : fighter_move(fighter)->clip;
  int sample = fighter_pose_sample(fighter);
  fighter_neutral(fighter);
  fighter_enter_state(fighter, S_IDLE);
  fighter->blend_clip = clip;
  fighter->blend_sample = sample;
  fighter->blend_duration = 5;
  fighter->blend_weight = 0x3333;
  fighter->sample_override = 0;
  native_begin(fighter, 1, 5, stance_blend_done);
  for (int frame = 1; frame <= 5; ++frame) {
    native_frame(fighter, frame, 0, stance_blend_add);
  }
  native_play(fighter, false);
}

/* 46fcd0/470650/4706f0 and shared tail 4707e0. CPU air attacks
 * select their entries directly; humans only run the whoosh countdown. */
static void jump_physics(Fighter *fighter) {
  const Fighter *other = fighter_opponent(fighter);
  if (!fighter->human && g_battle.cpu_level > 1 && fighter->pending == NULL) {
    q16 height = fighter->body.ground - fighter->body.pos[AXIS_Y];
    float distance = fighter_distance(fighter);
    bool straight = fighter->action == 0x2007;
    bool forward = fighter->action == 0x2009;
    bool target_air = fighter_airborne(other);
    bool attack = straight ? height >= 0xcccc && distance <= 1.5f
                           : height >= 0x8000 && (forward ? distance <= 2.f && (target_air || fighter->body.vel[AXIS_Y] >= 0) : target_air && distance <= 1.f);
    if (attack) {
      CpuAction action = distance < q16_float(0xcccc) || other->body.pos[AXIS_Y] - fighter->body.pos[AXIS_Y] > 0x6666 ? CPU_AIR_LK : CPU_AIR_HK;
      battle_cpu_action(fighter, action);
    }
  }
  if (fighter->action != 0x2007) {
    native_motion_step(fighter);
  }
}

static void jump_land_cancel(Fighter *fighter) {
  unsigned directions = fighter->held & (BUTTON_BIT(BUTTON_UP) | BUTTON_BIT(BUTTON_DOWN) | BUTTON_BIT(BUTTON_LEFT) | BUTTON_BIT(BUTTON_RIGHT));
  if (directions != 0) {
    fighter_enter_state(fighter, S_IDLE);
  }
}

static void jump_land(Fighter *fighter) {
  if (fighter->current_move == &jump_straight) {
    mk4_land(fighter);
  }
}

/* 46fd90: the same landing is reached by a straight jump, air strike
 * or clone retreat. Keep direction cancels and standing buttons shared. */
void mk4_land(Fighter *fighter) {
  fighter->body.physics = NULL;
  fighter_start_move(fighter, S_JUMP, &jump_landing);
  fighter->sample_override = 18;
  native_begin(fighter, 1, 10, mk4_return_to_stance);
  for (int frame = 1; frame <= 10; ++frame) {
    native_frame(fighter, frame, mini(27, 18 + frame), jump_land_cancel);
  }
  native_play(fighter, false);
  fighter_enter_stance(fighter, STANCE_STANDING);
}

static void flip_retract(Fighter *fighter);
static void flip_wait(Fighter *fighter);

static void jump_whoosh(Fighter *fighter) {
  (void)fighter;
  battle_sound(SOUND_C, 0x58);
}

static const mframe_t jump_sound_frames[12] = {{.thinkfunc = jump_whoosh}};

static void flip_ground(Fighter *fighter) {
  fighter->mmove_hold = fighter->body.gravity != 0;
}

static void flip_finish(Fighter *fighter) {
  if (fighter->body.gravity == 0) {
    mk4_return_to_stance(fighter);
    return;
  }
  native_begin(fighter, 0, 0, mk4_return_to_stance);
  native_frame(fighter, 0, 1, flip_ground);
  native_play(fighter, false);
}

static void flip_apex(Fighter *fighter) {
  Body *body = &fighter->body;
  if (body->vel[AXIS_Y] >= 0 && body->pos[AXIS_Y] >= -0x24ccc) {
    flip_retract(fighter);
  }
}

static void flip_wait(Fighter *fighter) {
  native_begin(fighter, 0, 0, NULL);
  native_frame(fighter, 0, 8, flip_apex);
  native_play(fighter, false);
}

/* 470080 is also entered directly by native specials after binding clip 301
 * at its tuck sample: spin to the apex, retract and land through 46f650. */
void mk4_flip_apex_return(Fighter *fighter) {
  flip_wait(fighter);
}

static void flip_retract(Fighter *fighter) {
  fighter->body.angle[AXIS_X] = fighter->body.spin[AXIS_X] = 0;
  fighter->script_phase = 2;
  fighter->sample_override = 7;
  fighter->buttons = NULL;
  native_begin(fighter, 1, 7, flip_finish);
  for (int frame = 1; frame <= 7; ++frame) {
    native_frame(fighter, frame, maxi(1, 7 - frame), NULL);
  }
  native_play(fighter, false);
}

/* 46acc0 state 1 -> 470080: after a slam hop, 428720 binds clip 301 at
 * its last sample; spin 0x4ccc and the 0.2 root offset (4702e0) precede
 * the shared apex wait, 470140 retraction and landing. The action stays. */
void mk4_throw_flip_tail(Fighter *fighter) {
  mk4_mmove_bind(fighter, 301, 9, 8);
  fighter->buttons = NULL;
  fighter->body.spin[AXIS_X] = 0x4ccc;
  fighter->body.root_y += 0x3333;
  fighter->body.pos[AXIS_Y] -= 0x3333;
  fighter->script_phase = 1;
  flip_wait(fighter);
}

static void flip_buttons(Fighter *fighter) {
  fighter_enter_stance(fighter, STANCE_DIAGONAL_AIR);
}

static void flip_spin(Fighter *fighter) {
  fighter->body.spin[AXIS_X] = fighter->saved_motion[2];
  fighter->body.root_y += 0x3333;
  fighter->body.pos[AXIS_Y] -= 0x3333;
  fighter->script_phase = 1;
}

static void start_flip(Fighter *fighter, int direction, bool from_straight) {
  bool forward = direction * fighter->facing > 0;
  fighter_start_move(fighter, S_JUMP, forward ? &jump_forward : &jump_backward);
  fighter->sample_override = 0;
  fighter->buttons = NULL;
  fighter->saved_motion[2] = forward || from_straight ? 0x4ccc : -0x4ccc;
  fighter_set_speed(fighter, forward ? 0xe14 : -0xe14);
  if (!from_straight) {
    fighter->body.vel[AXIS_Y] = forward ? -0x30a3 : -0x2e14;
    battle_sound(SOUND_B, 4);
  }
  fighter->body.gravity = 0x24d;
  fighter->body.on_land = jump_land;
  native_motion_bind(fighter, jump_physics, jump_sound_frames, 12);
  fighter->body.physics = jump_physics;
  native_begin(fighter, 1, 8, flip_wait);
  for (int frame = 1; frame <= 8; ++frame) {
    native_frame(fighter, frame, frame, frame == 2 ? flip_buttons : frame == 6 ? flip_spin : frame > 6 ? flip_apex : NULL);
  }
  native_play(fighter, false);
}

static void straight_direction(Fighter *fighter) {
  int direction = !!(fighter->held & BUTTON_BIT(BUTTON_RIGHT)) - !!(fighter->held & BUTTON_BIT(BUTTON_LEFT));
  if (direction != 0 && fighter->human) {
    start_flip(fighter, direction, true);
  }
}

static void straight_hold(Fighter *fighter) {
  fighter_stop_mmove(fighter);
}

void mk4_jump(Fighter *fighter) {
  int direction = !!(fighter->held & BUTTON_BIT(BUTTON_RIGHT)) - !!(fighter->held & BUTTON_BIT(BUTTON_LEFT));
  if (direction != 0) {
    start_flip(fighter, direction, false);
    return;
  }
  fighter_start_move(fighter, S_JUMP, &jump_straight);
  fighter_set_speed(fighter, 0);
  fighter->sample_override = 0;
  fighter->body.vel[AXIS_Y] = -0x30a3;
  fighter->body.gravity = 0x28f;
  fighter->body.on_land = jump_land;
  fighter->body.physics = fighter->human || g_battle.cpu_level <= 1 ? NULL : jump_physics;
  native_begin(fighter, 1, 18, straight_hold);
  for (int frame = 1; frame <= 18; ++frame) {
    native_frame(fighter, frame, frame < 3 ? frame : mini(17, frame - 1), frame <= 3 ? straight_direction : NULL);
  }
  native_play(fighter, false);
  battle_sound(SOUND_B, 4);
}

/* Air throw: scripts 0x4e5e28/0x4e5e50, native 0x46aae0.
 * The owner releases the pair; the victim owns its fall/getup, so interrupting
 * either participant cannot suspend a body. Shared getup replaces task parking. */
static const MoveDef s_air_throw = {.clip = 82, .last = 44, .flags = SF_LOCK | SF_RECORD, .action = 0x502};
static const MoveDef s_air_thrown = {.clip = 202, .last = 44, .flags = SF_LOCK, .action = 0x4003};
static const MoveDef s_throw_getup = {.clip = 131, .first = 46, .last = 79, .flags = SF_LOCK, .action = 0x200e};

static int air_throw_release_sample(const Fighter *victim) {
  int last = fighter_move(victim)->last;
  return last < 28 ? last : 28;
}

/* 480f50[89] -> 484b40 / 4e9910: release is owned by the attacker,
 * before world physics, so P1/P2 update order cannot delay the drop. */
static void air_throw_drop(Fighter *victim) {
  victim->flags &= ~(unsigned)FF_PAIRED;
  victim->script_phase = 1;
  victim->sample_override = 27;
  memset(victim->body.vel, 0, sizeof victim->body.vel);
  victim->body.vel[AXIS_Y] = 0x3333;
  victim->body.gravity = 0x28f;
}

/* 48e3a0 stamps a per-player special slot; 48e3f0 reports the ticks
 * since that stamp to the reuse-delay predicates. */
void mk4_special_stamp(Fighter *fighter, int slot) {
  require(slot >= 0 && slot < (int)(sizeof fighter->special_stamp / sizeof fighter->special_stamp[0]), "Invalid special stamp slot");
  fighter->special_stamp[slot] = g_battle.ticks + 1;
}

bool mk4_special_elapsed(const Fighter *fighter, int slot, uint64_t ticks) {
  require(slot >= 0 && slot < (int)(sizeof fighter->special_stamp / sizeof fighter->special_stamp[0]), "Invalid special stamp slot");
  uint64_t stamp = fighter->special_stamp[slot];
  return stamp == 0 || g_battle.ticks - (stamp - 1) >= ticks;
}

bool mk4_can_fan_finish(const Fighter *fighter) {
  return g_battle.arena == 8 && battle_can_finish(fighter) && fighter_distance(fighter) <= 1.f;
}

bool mk4_can_spike_finish(const Fighter *fighter) {
  return g_battle.arena == 0 && battle_can_finish(fighter) && fighter_distance(fighter) <= 1.f;
}

/* 467e00/467e70 enter the stage coordinator after the winner/range gate.
 * Native scene ownership must be established before either generated actor
 * starts, since the child may immediately bind or release its peer. */
bool mk4_begin_stage_finish(Fighter *fighter, int id) {
  if (!battle_begin_finisher(fighter, id)) {
    fighter_ready(fighter);
    return false;
  }
  mk4_script_scene_pose(fighter, id == FINISH_FAN ? 415 : 293, id == FINISH_FAN ? 130 : 20, 0);
  mk4_script_scene_pose((Fighter *)fighter_opponent(fighter), id == FINISH_FAN ? 416 : 13, id == FINISH_FAN ? 108 : 56, 0);
  return true;
}

/* Shared 46c6f0/46c740 ground throw gates, also used by the fully ported
 * normal entries: LK's close comparison is strict, LP's is inclusive. */
bool mk4_can_ground_throw(const Fighter *fighter, bool low_kick) {
  const Fighter *victim = fighter_opponent(fighter);
  int action = victim->action;
  float distance = fighter_distance(fighter);
  return (low_kick ? distance < .7f : distance <= .7f) && !(fighter->flags & FF_FOLLOW_UP) && !fighter_airborne(victim) && !(victim->flags & FF_REACTING) &&
         action != ACTION_RETAIL_010A && action != ACTION_RETAIL_010B && action != ACTION_RETAIL_0100 && action != ACTION_RETAIL_0102 &&
         action != ACTION_RETAIL_010E && !(action >= ACTION_RETAIL_0111 && action <= ACTION_RETAIL_0113);
}

bool mk4_can_air_throw(const Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  int64_t distance2 = 0;
  for (int i = 0; i < 3; ++i) {
    int64_t d = (int64_t)victim->body.pos[i] - fighter->body.pos[i];
    distance2 += (d * d) >> 16; /* 45ed80 sums three Q16 products. */
  }
  return g_battle.phase == PHASE_FIGHT && fighter->body.pos[AXIS_Y] <= -0x23333 && victim->body.pos[AXIS_Y] <= -0x23333 && distance2 <= 0x20000 &&
         !(victim->flags & (FF_REACTING | FF_UNTOUCHABLE | FF_PAIRED));
}

static void air_pair_capture(Fighter *fighter, const MoveDef *owner_move, const MoveDef *victim_move) {
  Fighter *victim = &g_battle.fighters[fighter == &g_battle.fighters[0] ? 1 : 0];
  if (victim->armed && victim->disarm != NULL) {
    victim->disarm(victim);
  }
  for (int i = 0; i < 2; ++i) {
    Fighter *actor = &g_battle.fighters[i];
    fighter_upright(actor); /* 48f9f0 / 490750: cancel the jump rotation. */
    memset(actor->body.vel, 0, sizeof actor->body.vel);
    actor->body.gravity = 0;
    actor->body.physics = NULL;
    actor->body.on_land = NULL;
  }
  fighter_start_move(fighter, S_THROW, owner_move);
  fighter_start_move(victim, S_KNOCKDOWN, victim_move);
  fighter->sample_override = victim->sample_override = 0;
  fighter->flags |= FF_PAIRED;
  victim->flags |= FF_PAIRED | FF_REACTING;
  memcpy(victim->body.pos, fighter->body.pos, sizeof victim->body.pos);
  victim->facing = fighter->facing;
  memcpy(victim->heading, fighter->heading, sizeof victim->heading);
  victim->capture_action = fighter->action;
}

/* 484b60: KO stays down; otherwise turn and finish clip 131 from pose 46. */
void mk4_mmove_air_getup(Fighter *victim) {
  if (victim->health <= 0) {
    fighter_finish_mmove(victim, NULL);
    return;
  }
  victim->facing = -victim->facing;
  fighter_start_move(victim, S_KNOCKDOWN, &s_throw_getup);
  victim->sample_override = 46;
  native_finish_clip(victim, fighter_ready);
}

void mk4_script_air_throw_release(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter->flags &= ~(unsigned)FF_PAIRED;
  air_throw_drop(victim);
  victim->sample_override = air_throw_release_sample(victim);
  victim->reaction = 89;
  battle_damage(victim, 0x2147);
  if (victim->health <= 0) {
    fighter->body.gravity = 0x28f;
  }
}

static void air_generated_cleanup(Fighter *fighter) {
  if ((fighter->flags & FF_PAIRED) && fighter_airborne(fighter)) {
    fighter->body.gravity = 0x28f;
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter->flags &= ~(unsigned)FF_PAIRED;
  if (victim->flags & FF_PAIRED) {
    air_throw_drop(victim);
    victim->sample_override = air_throw_release_sample(victim);
  }
}

static void air_frame_victim_tick(Fighter *victim) {
  const Fighter *owner = fighter_opponent(victim);
  if (owner->pending != NULL || owner->action != victim->capture_action || !(owner->flags & FF_PAIRED)) {
    air_throw_drop(victim);
    victim->sample_override = air_throw_release_sample(victim);
    victim->paired_release(victim);
    return;
  }
  memcpy(victim->body.pos, owner->body.pos, sizeof victim->body.pos);
}

static bool air_frame_captured(const Fighter *victim) {
  (void)victim;
  return false;
}

static void air_frame_victim_frames_frame_0(Fighter *fighter) {
  uint64_t continuation = fighter->continuation;
  air_frame_victim_tick(fighter);
  if (fighter->continuation != continuation || fighter->pending != NULL) {
    return;
  }
  fighter->mmove_hold = !air_frame_captured(fighter);
}

// clang-format off: Keep table entries on one row, including long rows.
static const mframe_t air_frame_victim_frames[1] = {
  [0] = {.thinkfunc = air_frame_victim_frames_frame_0},
};
// clang-format on
static const mmove_t air_frame_victim_phase = {.firstframe = 0, .lastframe = 0, .frame = air_frame_victim_frames, .endfunc = fighter_stop_mmove};

static void air_frame_cleanup(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  bool captured = (victim->flags & FF_PAIRED) != 0;
  air_generated_cleanup(fighter);
  if (captured && victim->pending == NULL) {
    victim->paired_release(victim);
  }
}

void mk4_mmove_air_throw_release(Fighter *fighter) {
  mk4_script_air_throw_release(fighter);
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  victim->paired_release(victim);
}

void mk4_air_throw_frames(Fighter *fighter, FighterFn owner_script, FighterFn victim_script) {
  mk4_air_throw_frames_with_moves(fighter, &s_air_throw, &s_air_thrown, owner_script, victim_script);
}

/* An interrupted captured victim must not stay suspended without gravity. */
static void air_frame_victim_cleanup(Fighter *victim) {
  if (fighter_airborne(victim) && victim->body.gravity == 0) {
    victim->body.gravity = 0x28f;
  }
}

/* Roster-specific air throws keep the shared 48f9f0 pair acquisition and
 * captured-victim hold while selecting their own owner and victim clips. */
void mk4_air_throw_frames_with_moves(Fighter *fighter, const MoveDef *owner_move, const MoveDef *victim_move, FighterFn owner_script, FighterFn victim_script) {
  if (!mk4_can_air_throw(fighter)) {
    fighter_ready(fighter);
    return;
  }
  mk4_air_pair_frames(fighter, owner_move, victim_move, owner_script, victim_script);
}

/* The same capture and held victim after a caller-owned contact decision,
 * such as a teleport slam that can take a grounded peer. */
void mk4_air_pair_frames(Fighter *fighter, const MoveDef *owner_move, const MoveDef *victim_move, FighterFn owner_script, FighterFn victim_script) {
  require(owner_move != NULL && victim_move != NULL && owner_script != NULL && victim_script != NULL, "Paired air throw needs moves and both scripts");
  air_pair_capture(fighter, owner_move, victim_move);
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  victim->paired_release = victim_script;
  fighter_mmove_phase(victim, &air_frame_victim_phase);
  victim->mmove_cleanup = air_frame_victim_cleanup;
  fighter->mmove_cleanup = air_frame_cleanup;
  owner_script(fighter);
}

static const MoveDef s_sidestep = {.clip = 300, .last = 17, .flags = SF_LOCK};

bool mk4_can_sidestep(const Fighter *fighter) {
  uint64_t previous = fighter->latch[BUTTON_RUN];
  return g_battle.phase == PHASE_FIGHT && !fighter_airborne(fighter) && previous != 0 && g_battle.ticks - previous <= 8 &&
         g_battle.ticks >= fighter->sidestep_ready;
}

static void sidestep_done(Fighter *fighter) {
  fighter_finish_mmove(fighter, fighter_ready);
}

// clang-format off: Keep table entries on one row, including long rows.
static const mframe_t sidestep_forward_frames[18] = {{0}};
// clang-format on
static const mmove_t sidestep_forward = {.firstframe = 0, .lastframe = 17, .frame = sidestep_forward_frames, .endfunc = sidestep_done};
// clang-format off: Keep table entries on one row, including long rows.
static const int sidestep_backward_frames_samples[] = {17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
static const mframe_t sidestep_backward_frames[18] = {{0}};
// clang-format on
static const mmove_t sidestep_backward = {
    .firstframe = 0, .lastframe = 17, .frame = sidestep_backward_frames, .endfunc = sidestep_done, .samples = sidestep_backward_frames_samples};

void mk4_sidestep(Fighter *fighter) {
  bool down = (fighter->held & BUTTON_BIT(BUTTON_DOWN)) != 0;
  q16 along = 0x443, across = down ? -0xccc : 0xccc;
  if (fighter_airborne(fighter_opponent(fighter))) {
    along = q16_mul(along, 0x4ccc);
    across = q16_mul(across, 0x4ccc);
  }
  q16 x = fighter_axis(fighter, AXIS_X), z = fighter_axis(fighter, AXIS_Z);
  mk4_script_stop_body(fighter);
  fighter_start_move(fighter, S_WALK, &s_sidestep);
  fighter->script_phase = down;
  fighter->sample_override = down ? 0 : 17;
  fighter->body.vel[AXIS_X] = q16_mul(x, along) - q16_mul(z, across);
  fighter->body.vel[AXIS_Z] = q16_mul(z, along) + q16_mul(x, across);
  fighter->sidestep_ready = g_battle.ticks + 80;
  fighter->mmove_cleanup = mk4_script_stop_body;
  fighter_mmove_phase(fighter, down ? &sidestep_forward : &sidestep_backward);
}
