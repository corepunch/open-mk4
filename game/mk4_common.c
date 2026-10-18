#include "game.h"
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
    fighter->sample_override = continuing ? 11 + fighter->ticks % 13 : 0;
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
  fighter->sample_override = fighter->ticks % count;
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

static void jump_tick(Fighter *fighter);

static void stance_blend_tick(Fighter *fighter) {
  fighter->blend_weight += 0x3333;
  if (fighter->blend_weight > Q16_ONE) {
    fighter->blend_duration = 0;
    fighter->blend_weight = -1;
    fighter->sample_override = -1;
    fighter->script = NULL;
    fighter->ticks = 0;
  }
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
  fighter->script = stance_blend_tick;
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
  if (fighter->action != 0x2007 && --fighter->move_loops <= 0) {
    fighter->move_loops = 12;
    battle_sound(SOUND_C, 0x58);
  }
}

static void jump_land_tick(Fighter *fighter) {
  unsigned directions = fighter->held & (BUTTON_BIT(BUTTON_UP) | BUTTON_BIT(BUTTON_DOWN) | BUTTON_BIT(BUTTON_LEFT) | BUTTON_BIT(BUTTON_RIGHT));
  if (directions != 0) {
    fighter_enter_state(fighter, S_IDLE);
    return;
  }
  fighter->sample_override = mini(27, 18 + fighter->ticks);
  if (fighter->ticks >= 10) {
    mk4_return_to_stance(fighter);
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
  fighter->script = jump_land_tick;
  fighter->sample_override = 18;
  fighter_enter_stance(fighter, STANCE_STANDING);
}

static void start_flip(Fighter *fighter, int direction, bool from_straight) {
  bool forward = direction * fighter->facing > 0;
  fighter_start_move(fighter, S_JUMP, forward ? &jump_forward : &jump_backward);
  fighter->script = jump_tick;
  fighter->sample_override = 0;
  fighter->buttons = NULL;
  fighter->script_timer = forward || from_straight ? 0x4ccc : -0x4ccc;
  fighter_set_speed(fighter, forward ? 0xe14 : -0xe14);
  if (!from_straight) {
    fighter->body.vel[AXIS_Y] = forward ? -0x30a3 : -0x2e14;
    battle_sound(SOUND_B, 4);
  }
  fighter->body.gravity = 0x24d;
  fighter->body.on_land = jump_land;
  fighter->body.physics = jump_physics;
  fighter->move_loops = 1;
}

static void jump_tick(Fighter *fighter) {
  Body *body = &fighter->body;
  int tick = fighter->ticks;
  if (fighter->current_move == &jump_straight) {
    int direction = !!(fighter->held & BUTTON_BIT(BUTTON_RIGHT)) - !!(fighter->held & BUTTON_BIT(BUTTON_LEFT));
    if (direction != 0 && fighter->human && tick <= 3) {
      start_flip(fighter, direction, true);
      return;
    }
    fighter->sample_override = tick < 3 ? tick : mini(17, tick - 1);
    return;
  }
  if (fighter->script_phase == 0) {
    fighter->sample_override = mini(8, tick);
    if (tick == 2) {
      fighter_enter_stance(fighter, STANCE_JUMP);
    }
    if (tick >= 6) {
      body->spin[AXIS_X] = fighter->script_timer;
      body->root_y += 0x3333;
      body->pos[AXIS_Y] -= 0x3333;
      fighter->script_phase = 1;
    }
  } else if (fighter->script_phase == 1) {
    fighter->sample_override = mini(8, tick);
    if (body->vel[AXIS_Y] >= 0 && body->pos[AXIS_Y] >= -0x24ccc) {
      body->angle[AXIS_X] = body->spin[AXIS_X] = 0;
      fighter->script_phase = 2;
      fighter->script_timer = 7;
      fighter->sample_override = 7;
      fighter->buttons = NULL;
    }
  } else if (fighter->script_phase == 2) {
    if (fighter->script_timer > 0 && --fighter->script_timer > 0) {
      fighter->sample_override--;
    }
    if (fighter->script_timer == 0 && body->gravity == 0) {
      mk4_return_to_stance(fighter);
    }
  }
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
  fighter->script = jump_tick;
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

static bool air_throw_start(Fighter *fighter, const MoveDef *owner_move, const MoveDef *victim_move) {
  if (!mk4_can_air_throw(fighter)) {
    fighter_ready(fighter);
    return false;
  }
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
  return true;
}

void mk4_script_air_getup(Fighter *victim) {
  if (victim->health <= 0) {
    fighter_fiber_finish(NULL);
  }
  victim->facing = -victim->facing;
  fighter_fiber_move(victim, S_KNOCKDOWN, &s_throw_getup);
  victim->sample_override = 46;
  fighter_fiber_play_to(victim, 79, 1);
  fighter_wait_ticks(1);
  fighter_fiber_finish(fighter_ready);
}

static void air_generated_victim(Fighter *victim) {
  Fighter *owner = (Fighter *)fighter_opponent(victim);
  while (victim->flags & FF_PAIRED) {
    fighter_wait_ticks(1);
    if (owner->pending != NULL || owner->action != 0x502) {
      air_throw_drop(victim);
      victim->sample_override = air_throw_release_sample(victim);
    }
    memcpy(victim->body.pos, owner->body.pos, sizeof victim->body.pos);
  }
  victim->sample_override = air_throw_release_sample(victim);
  victim->paired_release(victim);
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

void mk4_air_throw_generated(Fighter *fighter, FighterFn owner_script, FighterFn victim_script) {
  if (!air_throw_start(fighter, &s_air_throw, &s_air_thrown)) {
    return;
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  victim->paired_release = victim_script;
  fighter_start_fiber(fighter, owner_script, air_generated_cleanup);
  fighter_start_fiber(victim, air_generated_victim, NULL);
}

void mk4_air_throw_generated_with_moves(
    Fighter *fighter, const MoveDef *owner_move, const MoveDef *victim_move, FighterFn owner_script, FighterFn victim_script) {
  require(owner_move != NULL && victim_move != NULL && owner_script != NULL && victim_script != NULL, "Paired air throw needs moves and both scripts");
  if (!air_throw_start(fighter, owner_move, victim_move)) {
    return;
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  victim->paired_release = victim_script;
  fighter_start_fiber(fighter, owner_script, air_generated_cleanup);
  fighter_start_fiber(victim, air_generated_victim, NULL);
}

static const MoveDef s_sidestep = {.clip = 300, .last = 17, .flags = SF_LOCK};

bool mk4_can_sidestep(const Fighter *fighter) {
  uint64_t previous = fighter->latch[BUTTON_RUN];
  return g_battle.phase == PHASE_FIGHT && !fighter_airborne(fighter) && previous != 0 && g_battle.ticks - previous <= 8 &&
         g_battle.ticks >= fighter->sidestep_ready;
}

static void sidestep_script(Fighter *fighter) {
  for (;;) {
    fighter_wait_ticks(1);
    fighter->sample_override = fighter->script_phase ? mini(17, fighter->ticks) : maxi(0, 17 - fighter->ticks);
    if (fighter->ticks >= 18) {
      mk4_script_stop_body(fighter);
      fighter_fiber_finish(fighter_ready);
    }
  }
}

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
  fighter_start_fiber(fighter, sidestep_script, NULL);
}
