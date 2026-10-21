#include "../character.h"

static void teleport_land_gate(Fighter *fighter);
static void teleport_drop(Fighter *fighter);
static void teleport_pause(Fighter *fighter);
static void teleport_block_return(Fighter *fighter);
static void teleport_query(Fighter *fighter);
static void teleport_vanish(Fighter *fighter);
static void teleport_range(Fighter *fighter);
static const AttackRecord teleport_record = {0x128f5, -0x3ae1, 0xab85, 0x91eb, 90, 90, 0, ATTACK_MARK, 0x170a};
static const MoveDef s_teleport = {.clip = 312, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0209};
static const MoveDef s_teleport_block = {.clip = 301, .last = MOVE_END, .flags = SF_LOCK, .action = ACTION_RETAIL_1011};

void scorpion_teleport_land_script(Fighter *fighter) {
  native_begin(fighter, 1, 10, scorpion_special_ready);
  for (int frame = 1; frame <= 10; ++frame) {
    native_frame(fighter, frame, mini(27, 18 + frame), NULL);
  }
  native_play(fighter, false);
}

static void teleport_land_gate(Fighter *fighter) {
  fighter->mmove_hold = fighter_airborne(fighter);
}

static void teleport_drop(Fighter *fighter) {
  fighter->body.gravity = 0x1999;
  if (!fighter_airborne(fighter)) {
    fighter_finish_mmove(fighter, scorpion_special_ready);
    return;
  }
  native_begin(fighter, 0, 0, scorpion_special_ready);
  native_frame(fighter, 0, fighter_sample(fighter), teleport_land_gate);
  native_play(fighter, false);
}

static void teleport_pause(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter->flags &= ~(unsigned)FF_TELEPORTING;
  fighter->script_phase = 2;
  native_hold(fighter, 6, teleport_drop);
}

static void teleport_block_return(Fighter *fighter) {
  scorpion_entry_retail_script_004e8818(fighter);
}

static void teleport_query(Fighter *fighter) {
  if (!fighter_airborne(fighter)) {
    fighter_finish_mmove(fighter, scorpion_special_ready);
    return;
  }
  ContactResult result = battle_contact(fighter, &teleport_record);
  if (result == CONTACT_MISS) {
    return;
  }
  if (result == CONTACT_BLOCK) {
    fighter_start_move(fighter, S_JUMP, &s_teleport_block);
    fighter->sample_override = 8;
    fighter_set_speed(fighter, -0x28f);
    fighter->body.vel[AXIS_Y] = -0x2147;
    fighter->body.gravity = 0x1ca;
    native_hold(fighter, 21, teleport_block_return);
  } else {
    teleport_pause(fighter);
  }
}

static void teleport_vanish(Fighter *fighter) {
  Fighter *victim = fighter_peer(fighter);
  int first = fighter->mmove_frame + 1;
  /* 480140 reappears past the opponent along this fighter's pair axis, on
   * the opponent when that point leaves the arena, then travels back along
   * the pre-teleport axis. */
  q16 distance = (q16)(fighter_distance(fighter) * Q16_ONE);
  int direction = victim->body.pos[AXIS_X] >= fighter->body.pos[AXIS_X] ? 1 : -1;
  q16 offset = 0x26666 + maxi(0, 4 * Q16_ONE - distance);
  q16 x = victim->body.pos[AXIS_X] + q16_mul(fighter_axis(fighter, AXIS_X), offset);
  q16 z = victim->body.pos[AXIS_Z] + q16_mul(fighter_axis(fighter, AXIS_Z), offset);
  double radius = g_battle.arena_radius;
  bool outside = (double)x * x + (double)z * z > radius * radius;
  fighter->body.pos[AXIS_X] = outside ? victim->body.pos[AXIS_X] : x;
  fighter->body.pos[AXIS_Z] = outside ? victim->body.pos[AXIS_Z] : z;
  fighter->facing = -direction;
  fighter_set_speed(fighter, -0x2666);
  fighter->sample_override = 9;
  fighter->script_phase = 1;
  native_begin(fighter, first, maxi(first, 70), teleport_pause);
  for (int frame = first; frame <= maxi(first, 70); ++frame) {
    native_frame(fighter, frame, 9, teleport_query);
  }
  native_play(fighter, false);
}

static void teleport_range(Fighter *fighter) {
  scorpion_script_teleport_motion(fighter);
  if (fighter->native_signal) {
    teleport_vanish(fighter);
  }
}

void scorpion_teleport_script(Fighter *fighter) {
  int first = fighter->ticks + 1;
  native_begin(fighter, first, maxi(first, 10), teleport_vanish);
  for (int frame = first; frame <= maxi(first, 10); ++frame) {
    native_frame(fighter, frame, mini(9, frame), teleport_range);
  }
  native_play(fighter, false);
}

bool scorpion_can_teleport(const Fighter *fighter) {
  return g_battle.phase == PHASE_FIGHT && g_battle.ticks >= fighter->teleport_ready && fighter_opponent(fighter)->hits < 3;
}

void scorpion_teleport(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &s_teleport);
  fighter->sample_override = 0;
  fighter->teleport_ready = g_battle.ticks + 69;
  fighter->native_signal = false;
  fighter->flags |= FF_TELEPORTING;
  fighter->facing = fighter->body.pos[AXIS_X] <= fighter_opponent(fighter)->body.pos[AXIS_X] ? -1 : 1;
  scorpion_entry_retail_script_004e87b0(fighter);
}

void scorpion_script_teleport_motion(Fighter *fighter) {
  /* 480040 checks the portable arena/range limits before the native vanish
   * continuation. World physics owns integration of generated launch motion. */
  double x = fighter->body.pos[AXIS_X], z = fighter->body.pos[AXIS_Z], wall = g_battle.arena_radius - Q16_ONE;
  if (fighter->script_phase == 0 &&
      (fighter_distance(fighter) >= 5.9f || x * x + z * z >= wall * wall || (fighter->body.vel[AXIS_X] == 0 && fighter->body.vel[AXIS_Z] == 0))) {
    fighter->native_signal = true;
  }
}

void scorpion_script_teleport_victim_tail(Fighter *victim) {
  scorpion_entry_retail_script_004e9970(victim);
}
