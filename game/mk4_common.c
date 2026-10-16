#include "game.h"
#include "mk4_characters.h"
#include "math_util.h"

/* Approximate stance tables for fighters whose retail moves are not yet
 * ported. They follow the retail buttons from the shared tables in
 * docs/scorpion/input-and-commands.md section 6: back+LK sweeps, crouching
 * HP uppercuts and a close LP throws. Other selections use the generic move
 * for the button. A character with recovered moves owns its tables. */
#define ENTER_STATE(name, state)                                                                                                                               \
  static void enter_##name(Battle *battle, Fighter *fighter) {                                                                                                 \
    fighter_enter_state(battle, fighter, state);                                                                                                               \
  }
ENTER_STATE(punch, S_PUNCH)
ENTER_STATE(kick, S_KICK)
ENTER_STATE(sweep, S_SWEEP)
ENTER_STATE(uppercut, S_UPPERCUT)
ENTER_STATE(throw, S_THROW)

/* 0xb333 Q16: retail close-LP throw distance. */
#define MK4_THROW_RANGE 0.7f

static void press_punch(Battle *battle, Fighter *fighter) {
  (void)battle;
  fighter_interrupt(fighter, enter_punch);
}

static void press_low_punch(Battle *battle, Fighter *fighter) {
  fighter_interrupt(fighter, fighter_distance(battle, fighter) <= MK4_THROW_RANGE ? enter_throw : enter_punch);
}

static void press_kick(Battle *battle, Fighter *fighter) {
  (void)battle;
  fighter_interrupt(fighter, enter_kick);
}

static void press_low_kick(Battle *battle, Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(battle, fighter) ? enter_sweep : enter_kick);
}

static void press_uppercut(Battle *battle, Fighter *fighter) {
  (void)battle;
  fighter_interrupt(fighter, enter_uppercut);
}

static void press_sweep(Battle *battle, Fighter *fighter) {
  (void)battle;
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

void mk4_walk(Battle *battle, Fighter *fighter) {
  int direction = !!(fighter->held & BUTTON_BIT(BUTTON_RIGHT)) - !!(fighter->held & BUTTON_BIT(BUTTON_LEFT));
  bool forward = direction * fighter->facing > 0;
  bool run = forward && (fighter->held & BUTTON_BIT(BUTTON_RUN)) && fighter->run_energy > 0 && fighter->roster != 15;
  int character = roster_def(fighter->roster)->asset;
  character = character == 16 ? 2 : character == 17 ? 7 : character;
  if (run) {
    bool continuing = fighter->state == S_WALK && (fighter->current_move == &run_open || fighter->current_move == &run_cycle);
    const MoveDef *move = continuing ? &run_cycle : &run_open;
    if (fighter->current_move != move) {
      fighter_start_move(battle, fighter, S_WALK, move);
    }
    fighter->sample_override = continuing ? 11 + fighter->ticks % 13 : 0;
    fighter->run_energy = maxi(0, fighter->run_energy - 0x51e);
    fighter->run_delay = 40;
    fighter_set_speed(battle, fighter, mk4_motion.run);
    fighter->body.walking = true;
    return;
  }
  if (fighter->armed) {
    weapon_stance(battle, fighter, S_WALK);
  } else {
    const MoveDef *move = &walk_gaits[character][forward ? 0 : 1];
    if (fighter->current_move != move || fighter->state != S_WALK) {
      fighter_start_move(battle, fighter, S_WALK, move);
    }
  }
  int count = fighter->assets != NULL ? (int)fighter_clip(fighter->assets, fighter)->frames.count : 30;
  fighter->sample_override = fighter->ticks % count;
  q16 speed = forward ? mk4_motion.walk : -q16_mul(mk4_motion.walk, 0xb333);
  fighter_set_speed(battle, fighter, speed);
  fighter->body.walking = true;
}

/* 46f7e0 / 46fdf0 / 46fe80: human standing and directional jumps.
 * 46ff20 opens two tuck samples, then four more with diagonal buttons;
 * 470080 spins to the apex; 470140/170 retract at Y >= -2.3. */
static const MoveDef jump_straight = {.clip = 304, .last = MOVE_END, .action = 0x2007};
static const MoveDef jump_forward = {.clip = 301, .last = MOVE_END, .action = 0x2009};
static const MoveDef jump_backward = {.clip = 301, .last = MOVE_END, .action = 0x200a};
static const MoveDef jump_landing = {.clip = 304, .first = 18, .last = 27, .flags = SF_LOCK, .action = 0x2008};

static void jump_tick(Battle *battle, Fighter *fighter);

static void jump_land_tick(Battle *battle, Fighter *fighter) {
  fighter->sample_override = mini(27, 18 + fighter->ticks);
  if (fighter->ticks >= 10) {
    fighter_enter_state(battle, fighter, S_IDLE);
  }
}

static void jump_land(Battle *battle, Fighter *fighter) {
  bool straight = fighter->current_move == &jump_straight;
  fighter_neutral(fighter);
  if (straight) {
    fighter_start_move(battle, fighter, S_JUMP, &jump_landing);
    fighter->script = jump_land_tick;
    fighter->sample_override = 18;
  } else {
    fighter_enter_state(battle, fighter, S_IDLE);
  }
}

static void start_flip(Battle *battle, Fighter *fighter, int direction, bool from_straight) {
  bool forward = direction * fighter->facing > 0;
  fighter_start_move(battle, fighter, S_JUMP, forward ? &jump_forward : &jump_backward);
  fighter->script = jump_tick;
  fighter->sample_override = 0;
  fighter->buttons = NULL;
  fighter_set_speed(battle, fighter, forward ? 0xe14 : -0xe14);
  if (!from_straight) {
    fighter->body.vel[AXIS_Y] = forward ? -0x30a3 : -0x2e14;
    battle_sound(battle, SOUND_B, 4);
  }
  fighter->body.gravity = 0x24d;
  fighter->body.on_land = jump_land;
}

static void jump_tick(Battle *battle, Fighter *fighter) {
  Body *body = &fighter->body;
  int tick = fighter->ticks;
  if (fighter->current_move == &jump_straight) {
    int direction = !!(fighter->held & BUTTON_BIT(BUTTON_RIGHT)) - !!(fighter->held & BUTTON_BIT(BUTTON_LEFT));
    if (direction != 0 && fighter->human) {
      start_flip(battle, fighter, direction, true);
      return;
    }
    fighter->sample_override = tick < 3 ? tick : 17;
    return;
  }
  if (fighter->script_phase == 0) {
    fighter->sample_override = mini(5, tick);
    if (tick == 2) {
      fighter_enter_stance(fighter, STANCE_JUMP);
    }
    if (tick >= 6) {
      body->spin[AXIS_X] = fighter->action == 0x2009 ? 0x4ccc : -0x4ccc;
      body->root_y += 0x3333;
      body->pos[AXIS_Y] -= 0x3333;
      fighter->script_phase = 1;
    }
  } else if (fighter->script_phase == 1 && body->vel[AXIS_Y] >= 0 && body->pos[AXIS_Y] >= -0x24ccc) {
    body->angle[AXIS_X] = body->spin[AXIS_X] = 0;
    fighter_set_speed(battle, fighter, 0);
    fighter->script_phase = 2;
    fighter->script_timer = 7;
    fighter->sample_override = 7;
  } else if (fighter->script_phase == 2 && --fighter->script_timer > 0) {
    fighter->sample_override--;
  }
}

void mk4_jump(Battle *battle, Fighter *fighter) {
  int direction = !!(fighter->held & BUTTON_BIT(BUTTON_RIGHT)) - !!(fighter->held & BUTTON_BIT(BUTTON_LEFT));
  if (direction != 0) {
    start_flip(battle, fighter, direction, false);
    return;
  }
  fighter_start_move(battle, fighter, S_JUMP, &jump_straight);
  fighter_set_speed(battle, fighter, 0);
  fighter->sample_override = 0;
  fighter->body.vel[AXIS_Y] = -0x30a3;
  fighter->body.gravity = 0x28f;
  fighter->body.on_land = jump_land;
  fighter->script = jump_tick;
  battle_sound(battle, SOUND_B, 4);
}
