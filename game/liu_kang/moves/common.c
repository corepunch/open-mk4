#include "../character.h"

static const AttackRecord hp_record = {51118, -45809, 38666, 28180, 32, 11, 1, 0, 3276};
static const MoveDef s_hp_second = {.clip = 297, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_010A};
static const NativeHPProfile hp_profile = {&hp_record, &liu_kang_s_hp, &s_hp_second, liu_kang_ready};
static const MoveDef s_lp_release = {.clip = 174, .last = 26, .flags = SF_LOCK, .action = ACTION_RETAIL_4003};
static const MoveDef s_lp_getup = {.clip = 191, .last = 15, .flags = SF_LOCK, .action = ACTION_RETAIL_200E};

/* VM control flow is generated; native adapters reuse C fibers. Playback need not
 * match stored clip length. See docs/liu-kang-scripts.md for the evidence. */
void liu_kang_ready(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    fighter_start_move(fighter, S_JUMP, roster_def(fighter->roster)->moves[S_JUMP]);
  } else {
    fighter_enter_state(fighter, S_IDLE);
  }
}

int liu_kang_victim_action(const Fighter *fighter) {
  return fighter_opponent(fighter)->action;
}

Fighter *liu_kang_opponent(Fighter *fighter) {
  return &g_battle.fighters[fighter == &g_battle.fighters[0] ? 1 : 0];
}

void liu_kang_hp_script(Fighter *fighter) {
  native_hp_start(fighter, &hp_profile);
}

void liu_kang_lk(Fighter *fighter) {
  liu_kang_ground_attack(fighter, G_LOW_KICK);
}

void liu_kang_hk(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  if (fighter_holds_back(fighter)) {
    liu_kang_roundhouse(fighter);
  } else if (fighter_distance(fighter) <= 0.8f && !fighter_airborne(victim) && victim->action != ACTION_RETAIL_4001 && victim->action != ACTION_RETAIL_4005) {
    liu_kang_ground_attack(fighter, G_CLOSE_HK);
  } else {
    liu_kang_kick(fighter);
  }
}

void liu_kang_liu_script_lp_getup(Fighter *victim) {
  if (victim->health <= 0) {
    liu_kang_thrown_finish(victim);
  }
  victim->facing = -victim->facing;
  fighter_fiber_move(victim, S_KNOCKDOWN, &s_lp_getup);
  victim->sample_override = 0;
  fighter_set_speed(victim, -0x147a);
  liu_kang_thrown_play_to(victim, 15, 1);
  native_delay(victim, 1);
  liu_kang_thrown_finish(victim);
}

void liu_kang_liu_script_lk_continue(Fighter *fighter) {
  liu_kang_entry_retail_script_004de410(fighter);
}

void liu_kang_liu_script_lk_damage(Fighter *fighter) {
  Fighter *victim = liu_kang_opponent(fighter);
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0xb333);
  }
  victim->script_phase = 1;
  victim->flags |= FF_PAIR_RELEASE;
  fighter_start_fiber(victim, liu_kang_lk_thrown_script, NULL);
  battle_damage(victim, 0x2e14);
  liu_kang_entry_retail_script_004de428(fighter);
  liu_kang_entry_retail_script_004de468(fighter);
}

void liu_kang_liu_script_spawn_low(Fighter *fighter, int task_type) {
  require(task_type == 64, "Wrong Liu Kang low-fireball task type");
  liu_kang_fireball_spawn(fighter, 1);
}

void liu_kang_liu_script_spawn_high(Fighter *fighter, int task_type) {
  require(task_type == 64, "Wrong Liu Kang high-fireball task type");
  liu_kang_fireball_spawn(fighter, 0);
}

void liu_kang_liu_script_reaction(Fighter *fighter, int reaction) {
  Fighter *victim = liu_kang_opponent(fighter);
  victim->reaction = reaction;
  if (reaction == 104) {
    liu_kang_throw_disarm(victim);
    fighter->flags &= ~(unsigned)FF_PAIRED;
    victim->flags &= ~(unsigned)FF_PAIRED;
    mk4_script_stop_body(victim);
    fighter_start_move(victim, S_KNOCKDOWN, &s_lp_release);
    victim->sample_override = 0;
    victim->flags |= FF_REACTING;
    fighter_start_fiber(victim, liu_kang_entry_retail_script_004e9b68, NULL);
  } else if (reaction == 80) {
    fighter->flags &= ~(unsigned)FF_PAIRED;
    victim->flags &= ~(unsigned)FF_PAIRED;
  } else if (reaction == 176) {
    victim->facing = -victim->facing;
  }
}

void liu_kang_liu_script_motion(Fighter *fighter, int speed, int delay, int duration) {
  fighter->saved_motion[0] = speed;
  native_schedule_speed(fighter, speed, delay, duration);
}

void liu_kang_liu_script_back_lp_tail(Fighter *fighter) {
  const WeaponStrike *strike = &liu_kang_sword_attacks[3];
  weapon_script_contact_tail(fighter, &strike->record, strike->queries);
}
