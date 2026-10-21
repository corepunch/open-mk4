#include "native_services.h"

#define RETAIL SF_ATTACK | SF_LOCK | SF_RECORD
static void bicycle_release(Fighter *victim);
static void bicycle_victim_guard(Fighter *victim);
static void bicycle_victim_cycle(Fighter *victim);
static void bicycle_victim_script(Fighter *victim);
static void bicycle_cleanup(Fighter *fighter);
static void bicycle_query(Fighter *fighter);
static void bicycle_pair(Fighter *fighter);
static bool bicycle_grounded(const Fighter *fighter);
static void bicycle_land(Fighter *fighter);
static void bicycle_descend(Fighter *fighter);
static void bicycle_carry_end(Fighter *fighter);
static void bicycle_contact_end(Fighter *fighter);
static void bicycle_motion_pose(Fighter *fighter, float pose);
static const MoveDef s_bicycle = {.clip = 68, .last = 22, .flags = RETAIL, .action = ACTION_RETAIL_0206};
static const MoveDef s_bicycle_victim = {.clip = 136, .last = 23, .flags = SF_LOCK, .action = ACTION_RETAIL_4003};
static const MoveDef s_bicycle_release = {.clip = 182, .last = 64, .flags = SF_LOCK, .action = ACTION_RETAIL_100B};
static const MoveDef s_bicycle_air_release = {.clip = 183, .last = 33, .flags = SF_LOCK, .action = ACTION_RETAIL_100B};
static const AttackRecord bicycle_record = {0xf851, -0x547a, 0xf5c2, 0x70a3, 84, 84, 0, 0, 0x2666};

static void bicycle_release(Fighter *victim) {
  victim->flags &= ~(unsigned)FF_PAIRED;
  bool air = victim->body.ground - victim->body.pos[AXIS_Y] > 0x3333;
  fighter_start_move(victim, S_KNOCKDOWN, air ? &s_bicycle_air_release : &s_bicycle_release);
  victim->reaction = 81;
  victim->sample_override = 0;
  if (air) {
    retail_vm_enter_owned(victim, 0x4e9ae8, fighter_ready, 3);
  } else {
    victim->body.physics = fighter_friction;
    retail_vm_enter_owned(victim, 0x4e9ad0, mk4_script_thrown_ready, 3);
  }
}

static void bicycle_victim_guard(Fighter *victim) {
  const Fighter *owner = fighter_opponent(victim);
  if (victim->script_phase != 0 || owner->action != ACTION_RETAIL_0206 || owner->pending != NULL || !(owner->flags & FF_PAIRED)) {
    fighter_finish_mmove(victim, bicycle_release);
  }
}

static void bicycle_victim_cycle(Fighter *victim) {
  native_begin(victim, 0, 19, NULL);
  for (int frame = 0; frame < 20; ++frame) {
    native_frame(victim, frame, 4 + (frame + 1) % 20, bicycle_victim_guard);
  }
  native_play(victim, false);
}

static void bicycle_victim_script(Fighter *victim) {
  native_begin(victim, 1, 24, bicycle_victim_cycle);
  for (int frame = 1; frame <= 24; ++frame) {
    native_frame(victim, frame, frame < 24 ? frame : 4, bicycle_victim_guard);
  }
  native_play(victim, false);
}

static void bicycle_cleanup(Fighter *fighter) {
  Fighter *victim = fighter_peer(fighter);
  if (victim->current_move == &s_bicycle_victim && (victim->flags & FF_PAIRED)) {
    victim->flags &= ~(unsigned)FF_PAIRED;
    victim->script_phase = 1;
  }
}

static void bicycle_query(Fighter *fighter) {
  if (!fighter_airborne(fighter) || battle_contact(fighter, &bicycle_record) != CONTACT_MISS) {
    bicycle_contact_end(fighter);
  }
}

static void bicycle_pair(Fighter *fighter) {
  Fighter *victim = fighter_peer(fighter);
  if (victim->current_move != &s_bicycle_victim || victim->pending != NULL || victim->current_mmove == NULL || !(fighter->flags & FF_PAIRED) ||
      !(victim->flags & FF_PAIRED)) {
    bicycle_carry_end(fighter);
    return;
  }
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0xe666);
  }
}

static bool bicycle_grounded(const Fighter *fighter) {
  return !fighter_airborne(fighter);
}

static void bicycle_land(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8678, fighter_ready, 3);
}

static void bicycle_descend(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    fighter->body.gravity = 0xc4;
    native_await(fighter, bicycle_grounded, bicycle_land);
  } else {
    bicycle_land(fighter);
  }
}

static void bicycle_carry_end(Fighter *fighter) {
  Fighter *victim = fighter_peer(fighter);
  if (victim->current_move == &s_bicycle_victim && victim->pending == NULL && (victim->flags & FF_PAIRED)) {
    victim->flags &= ~(unsigned)FF_PAIRED;
    victim->script_phase = 1;
  }
  fighter->flags &= ~(unsigned)FF_PAIRED;
  fighter->body.pos[AXIS_Y] = fighter->body.ground;
  mk4_script_stop_body(fighter);
  bicycle_descend(fighter);
}

/* The query rows end on landing, contact or their last visit. */
static void bicycle_contact_end(Fighter *fighter) {
  if (fighter->contact == CONTACT_BLOCK) {
    retail_vm_enter_owned(fighter, 0x4e8688, fighter_ready, 3);
    return;
  }
  if (fighter->contact != CONTACT_HIT) {
    bicycle_descend(fighter);
    return;
  }
  Fighter *victim = fighter_peer(fighter);
  liu_kang_throw_disarm(victim);
  fighter_start_move(victim, S_KNOCKDOWN, &s_bicycle_victim);
  victim->sample_override = 0;
  victim->reaction = 84;
  victim->flags |= FF_PAIRED | FF_REACTING;
  fighter->flags |= FF_PAIRED;
  fighter->body.pos[AXIS_Y] = mini(fighter->body.pos[AXIS_Y], -0x15999);
  fighter_set_speed(fighter, 0x10a3);
  fighter->body.vel[AXIS_Y] = fighter->body.gravity = 0;
  bicycle_victim_script(victim);
  native_begin(fighter, 0, 57, bicycle_carry_end);
  for (int frame = 0; frame < 58; ++frame) {
    native_frame(fighter, frame, 7 + frame % 16, bicycle_pair);
  }
  native_play(fighter, false);
}

void liu_kang_bicycle_script(Fighter *fighter) {
  native_begin(fighter, 0, 59, bicycle_contact_end);
  for (int frame = 0; frame < 60; ++frame) {
    native_frame(fighter, frame, 7 + frame % 16, bicycle_query);
  }
  native_play(fighter, false);
}

bool liu_kang_can_bicycle(const Fighter *fighter) {
  return g_battle.phase == PHASE_FIGHT && fighter_opponent(fighter)->action != ACTION_RETAIL_100B && fighter->latch[BUTTON_LOW_KICK] != 0 &&
         g_battle.ticks - fighter->latch[BUTTON_LOW_KICK] >= 120;
}

void liu_kang_bicycle_kick(Fighter *fighter) {
  if (g_battle.phase != PHASE_FIGHT || fighter_opponent(fighter)->action == ACTION_RETAIL_100B) {
    return;
  }
  mk4_script_stop_body(fighter);
  fighter_start_move(fighter, S_KICK, &s_bicycle);
  fighter->sample_override = 0;
  fighter->mmove_cleanup = bicycle_cleanup;
  retail_vm_enter_owned(fighter, 0x4e8610, fighter_ready, 3);
}

static void bicycle_motion_pose(Fighter *fighter, float pose) {
  fighter->sample_override = (int)pose;
}

void liu_kang_liu_script_bicycle_motion(Fighter *fighter) {
  if (fighter->native_motion.binding != liu_kang_liu_script_bicycle_motion) {
    native_motion_begin(fighter, 16, NULL);
    for (int frame = 0; frame < 16; ++frame) {
      native_motion_frame(fighter, frame, bicycle_motion_pose, (float)(7 + (frame + 1) % 16), NULL);
    }
    fighter->native_motion.binding = liu_kang_liu_script_bicycle_motion;
    fighter->native_motion.loop = true;
    native_motion_play(fighter);
    fighter->body.physics = liu_kang_liu_script_bicycle_motion;
  }
  native_motion_step(fighter);
}
