#include "../character.h"

static bool teleport_below_ground(const Fighter *fighter);
static void teleport_done(Fighter *fighter);
static void teleport_land(Fighter *fighter);
static void teleport_drop(Fighter *fighter);
static void teleport_script(Fighter *fighter);
static const MoveDef s_teleport = {.clip = 304, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = 0x20c};

static bool teleport_below_ground(const Fighter *fighter) {
  return fighter->body.pos[AXIS_Y] >= fighter->body.ground + Q16_ONE;
}

static void teleport_done(Fighter *fighter) {
  fighter_finish_mmove(fighter, fighter_ready);
}

static void teleport_land(Fighter *fighter) {
  fighter->flags &= ~(unsigned)FF_UNTOUCHABLE;
  fighter->teleport_ready = g_battle.ticks + 32;
  native_hold(fighter, 4, teleport_done);
}

static void teleport_drop(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    fighter->body.pos[axis] = victim->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0x10000);
  }
  fighter->body.pos[AXIS_Y] = fighter->body.ground - 3 * Q16_ONE;
  fighter->body.vel[AXIS_Y] = -0x451e;
  fighter->body.gravity = 0x28f;
  fighter->flags |= FF_UNTOUCHABLE;
  native_await(fighter, native_grounded, teleport_land);
}

static void teleport_script(Fighter *fighter) {
  fighter->body.vel[AXIS_Y] = 0x4ccc;
  fighter->body.gravity = 0;
  native_await(fighter, teleport_below_ground, teleport_drop);
}

void raiden_teleport(Fighter *fighter) {
  if (g_battle.phase != PHASE_FIGHT) {
    return;
  }
  fighter_start_move(fighter, S_JUMP, &s_teleport);
  battle_sound(SOUND_A, 0x30);
  fighter->mmove_cleanup = raiden_special_cleanup;
  teleport_script(fighter);
}

bool raiden_can_teleport(const Fighter *fighter) {
  return g_battle.phase == PHASE_FIGHT && g_battle.ticks >= fighter->teleport_ready;
}
