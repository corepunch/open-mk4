#include "../character.h"

static const MoveDef s_shadow_low = {.clip = 26, .last = 13, .flags = SF_LOCK, .action = 0x309};
static const MoveDef s_shadow_up = {.clip = 24, .last = 9, .flags = SF_LOCK, .action = 0x30a};
static const AttackRecord hp_record = {51118, -45809, 38666, 28180, 32, 11, 1, 0, 3276};
static const MoveDef s_hp_second = {.clip = 297, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_010A};
static const NativeHPProfile hp_profile = {&hp_record, &kitana_s_hp, &s_hp_second, kitana_ready};

/* Native controller return used by generated VM programs and native runners. */
void kitana_ready(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    fighter_start_move(fighter, S_JUMP, roster_def(fighter->roster)->moves[S_JUMP]);
  } else {
    fighter_enter_state(fighter, S_IDLE);
  }
}

int kitana_victim_action(const Fighter *fighter) {
  return fighter_opponent(fighter)->action;
}

void kitana_shadow_low(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &s_shadow_low);
  fighter_start_fiber(fighter, kitana_entry_retail_script_004ed268, NULL);
}

void kitana_shadow_up(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &s_shadow_up);
  fighter_start_fiber(fighter, kitana_entry_retail_script_004ed2c0, NULL);
}

bool kitana_shadow_up_enabled(const Fighter *fighter) {
  return fighter->body.gravity != 0;
}

Fighter *kitana_opponent(Fighter *fighter) {
  return &g_battle.fighters[fighter == &g_battle.fighters[0] ? 1 : 0];
}

void kitana_stop_body(Fighter *fighter) {
  memset(fighter->body.vel, 0, sizeof fighter->body.vel);
  fighter->body.gravity = 0;
  fighter->body.physics = NULL;
  fighter->body.on_land = NULL;
}

void kitana_hp_script(Fighter *fighter) {
  native_hp_start(fighter, &hp_profile);
}

void kitana_lk(Fighter *fighter) {
  kitana_ground_attack(fighter, G_LOW_KICK);
}

void kitana_hk(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  if (fighter_holds_back(fighter)) {
    kitana_roundhouse(fighter);
  } else if (fighter_distance(fighter) <= 0.8f && !fighter_airborne(victim) && victim->action != ACTION_RETAIL_4001 && victim->action != ACTION_RETAIL_4005) {
    kitana_ground_attack(fighter, G_CLOSE_HK);
  } else {
    kitana_kick(fighter);
  }
}

void kitana_script_reaction(Fighter *fighter, int reaction) {
  if (reaction == 184) {
    Fighter *victim = kitana_opponent(fighter);
    mk4_script_scene_pose(victim, 561, 79, 0);
    fighter_start_fiber(victim, kitana_entry_retail_script_004e59c8, NULL);
    return;
  }
  if (reaction == 175) {
    return;
  }
  require(reaction == 78 || reaction == 79 || reaction == 103, "Unexpected Kitana paired reaction");
  Fighter *victim = mk4_script_release_pair(fighter);
  victim->reaction = reaction;
  victim->flags |= FF_REACTING;
  fighter_start_fiber(victim,
      reaction == 78   ? kitana_lk_victim
      : reaction == 79 ? kitana_entry_retail_script_004e9a68
                       : kitana_entry_retail_script_004e9b88,
      NULL);
}

void kitana_script_lk_return(Fighter *fighter) {
  mk4_script_finish_clip(fighter, 1);
  native_delay(fighter, 20); /* Native stance blend, step 0xccc. */
  fighter_fiber_finish(kitana_ready);
}

void kitana_shadow_task_low(Fighter *fighter, int task_type) {
  require(task_type == 75, "Unexpected Kitana shadow task type");
  battle_shadow_actor_start(fighter, SHADOW_ACTOR_LOW);
}

void kitana_shadow_task_up(Fighter *fighter, int task_type) {
  require(task_type == 75, "Unexpected Kitana shadow task type");
  battle_shadow_actor_start(fighter, SHADOW_ACTOR_UP);
}

void kitana_shadow_install_physics(Fighter *fighter) {
  battle_shadow_actor_install_physics(fighter);
  fighter_fiber_finish(kitana_ready);
}

void kitana_special_cleanup(Fighter *fighter) {
  mk4_script_pair_cleanup(fighter);
  fighter->body.physics = NULL;
  fighter_set_speed(fighter, 0);
  if (fighter_airborne(fighter)) {
    fighter->body.gravity = 0x28f;
  }
}

void kitana_script_reaction_side_getup(Fighter *victim) {
  if (victim->health <= 0) {
    mk4_script_thrown_finish(victim);
  }
  mk4_script_bind(victim, 185, 16, 0);
  mk4_script_finish_clip(victim, 1);
  mk4_script_thrown_finish(victim);
}

void kitana_generated_reaction(Fighter *victim) {
  FighterFn script;
  switch (victim->reaction) {
    case 24:
      script = kitana_entry_retail_script_004e9110;
      break;
    case 55:
      script = kitana_entry_retail_script_004e8b38;
      break;
    case 56:
      script = kitana_entry_retail_script_004e8f50;
      break;
    case 68:
      script = kitana_entry_retail_script_004e9de8;
      break;
    case 86:
      script = kitana_entry_retail_script_004ea130;
      break;
    case 87:
      script = kitana_square_victim;
      break;
    case 121:
      script = kitana_entry_retail_script_004ea0b8;
      battle_wall_recoil(victim);
      break;
    default:
      require(false, "Unexpected Kitana record reaction");
      return;
  }
  mk4_script_disarm_self(victim);
  fighter_start_move(victim, S_KNOCKDOWN, &kitana_s_knockdown);
  victim->flags |= FF_REACTING;
  victim->sample_override = 0;
  if (fighter_airborne(victim) && victim->body.gravity == 0) {
    victim->body.gravity = 0x28f;
  }
  fighter_start_fiber(victim, script, NULL);
}
