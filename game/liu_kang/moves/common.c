#include "retail_vm.h"
#include "../character.h"

static const MoveDef s_lp_release = {.clip = 174, .last = 26, .flags = SF_LOCK, .action = ACTION_RETAIL_4003};
static const MoveDef s_lp_getup = {.clip = 191, .last = 15, .flags = SF_LOCK, .action = ACTION_RETAIL_200E};

/* VM control flow is generated; native adapters build frame rows. Playback need
 * not match stored clip length. See docs/liu-kang-scripts.md for the evidence. */

void liu_kang_liu_script_lp_getup(Fighter *victim) {
  if (victim->health <= 0) {
    liu_kang_thrown_finish(victim);
    return;
  }
  victim->facing = -victim->facing;
  fighter_start_move(victim, S_KNOCKDOWN, &s_lp_getup);
  victim->sample_override = 0;
  fighter_set_speed(victim, -0x147a);
  native_finish_clip(victim, liu_kang_thrown_finish);
}

void liu_kang_liu_script_lk_continue(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4de410, fighter_ready, 3);
}

void liu_kang_liu_script_lk_damage(Fighter *fighter) {
  Fighter *victim = fighter_peer(fighter);
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0xb333);
  }
  victim->script_phase = 1;
  victim->flags |= FF_PAIR_RELEASE;
  fighter_cancel_mmove(victim);
  liu_kang_lk_thrown_script(victim);
  battle_damage(victim, 0x2e14);
  /* 4de428 returns to this native caller, which then enters 4de468. */
  retail_vm_enter_then_owned(fighter, 0x4de428, 0x4de468, fighter_ready, 3);
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
  Fighter *victim = fighter_peer(fighter);
  victim->reaction = reaction;
  if (reaction == 104) {
    liu_kang_throw_disarm(victim);
    fighter->flags &= ~(unsigned)FF_PAIRED;
    victim->flags &= ~(unsigned)FF_PAIRED;
    mk4_script_stop_body(victim);
    fighter_start_move(victim, S_KNOCKDOWN, &s_lp_release);
    victim->sample_override = 0;
    victim->flags |= FF_REACTING;
    retail_vm_enter_owned(victim, 0x4e9b68, fighter_ready, 3);
  } else if (reaction == 80) {
    fighter->flags &= ~(unsigned)FF_PAIRED;
    victim->flags &= ~(unsigned)FF_PAIRED;
  } else if (reaction == 176) {
    victim->facing = -victim->facing;
  }
}

void liu_kang_liu_script_back_lp_tail(Fighter *fighter) {
  const WeaponStrike *strike = &liu_kang_sword_attacks[3];
  weapon_script_contact_tail(fighter, &strike->record, strike->queries);
}
