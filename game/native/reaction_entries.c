#include "script/bindings.h"
#include "script/native_ids.h"
#include "data/layout.h"
#include "native/capabilities.h"
#include "data/tables.h"

uint32_t action_reaction_native(const Fighter *fighter) {
  return mk4_reaction_target(battle_data(), (unsigned)fighter->reaction);
}

/* Native482aa0: the impact voice depends on the captured peer, independently
 * of the selected script/model aliases. Script words supply subsequent sound. */
static void stagger_voice(Fighter *fighter) {
  if (fighter_peer(fighter)->roster == 13) {
    action_native_sound_list(dat_sound_unarmed_impact_list);
    action_native_sound_list(dat_sound_knockdown_voice_list);
  } else {
    mk4_script_indexed_sound(fighter, 1);
  }
}

bool action_prepare_stagger_stance_select_disarm(Fighter *fighter, bool alternate) {
  mk4_script_disarm_self(fighter);
  if (alternate) {
    uint32_t physics = mk4_image_u32(battle_data(), dat_crouch_reaction_physics_word);
    require(script_native_dispatch(fighter, physics, MK4_PHYSICS, 0), "Unknown alternate reaction motion callback");
  }
  return true;
}

bool action_prepare_stagger_clip_134(Fighter *fighter, bool alternate) {
  (void)alternate;
  mk4_script_disarm_self(fighter);
  action_native_sound_list(dat_sound_list_stagger_reaction);
  return true;
}

bool action_prepare_stagger_shake_60_percent_damage(Fighter *fighter, bool alternate) {
  (void)alternate;
  fighter->damage_scale = MK4_DAMAGE_SCALE_60_PERCENT;
  return true;
}

bool action_prepare_knockdown_stagger_half_damage_alias(Fighter *fighter, bool alternate) {
  (void)alternate;
  fighter->damage_scale = MK4_DAMAGE_SCALE_50_PERCENT;
  mk4_script_disarm_self(fighter);
  stagger_voice(fighter);
  fighter->action = ACTION_CODE_4003;
  return true;
}

bool action_prepare_kick_stagger_60_percent_damage(Fighter *fighter, bool alternate) {
  (void)alternate;
  fighter->action = 0x4002;
  stagger_voice(fighter);
  fighter->damage_scale = MK4_DAMAGE_SCALE_60_PERCENT;
  return true;
}

void action_stagger_reaction(Fighter *fighter) {
  uint32_t entry = mk4_reaction_target(battle_data(), (unsigned)fighter->reaction);
  int previous_action = fighter->action;
  bool crouch = previous_action == ACTION_CODE_2001 || previous_action == ACTION_CODE_2002;
  if (entry == native_stagger_stance_select_disarm) {
    crouch |= previous_action == 0x2010 || previous_action == ACTION_CODE_0106 || previous_action == ACTION_CODE_0107;
  }
  if (entry == native_stagger_stance_select_disarm || entry == native_stagger_clip_311_keep_weapon || entry == native_stagger_clip_134 ||
      entry == native_stagger_clip_134_impact) {
    battle_wall_recoil(fighter);
  }
  static const MoveDef move = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = ACTION_CODE_4001};
  fighter_start_move(fighter, S_HIT, &move);
  script_native_continue(fighter, entry, crouch, mk4_script_thrown_ready);
}

void action_launch_reaction(Fighter *fighter) {
  uint32_t entry = mk4_reaction_target(battle_data(), (unsigned)fighter->reaction);
  if (entry != native_fall_reaction_with_effects && entry != native_fall_without_disarm) {
    mk4_script_disarm_self(fighter);
  }
  if (entry == native_disarm_wall_recoil_fall) {
    battle_wall_recoil(fighter);
  }
  static const MoveDef move = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = ACTION_CODE_4003};
  fighter_start_move(fighter, S_KNOCKDOWN, &move);
  script_native_continue(fighter, entry, false, mk4_script_thrown_ready);
}

/* 482790,482f90 and483610 finish the inherited descriptor; their native
 * completion differs.482f90 is the same finish/back-getup as482790.
 * Engine continuations replace the task84 resume flag. */
void action_reaction_getup_back(Fighter *fighter) {
  mk4_mmove_finish_getup(fighter, dat_getup_descriptor_flip_getup);
}

void action_reaction_finish(Fighter *fighter) {
  native_finish_clip(fighter, mk4_mmove_thrown_finish);
}

void action_reaction_fall(Fighter *fighter) {
  mk4_script_disarm_self(fighter);
  script_vm_enter(fighter, fn_reaction_fall, mk4_script_thrown_ready);
}

/* Native4826e0 selects impact audio from the captured attacker weapon. */
static void weapon_impact_voice(Fighter *fighter) {
  const Fighter *peer = fighter_peer(fighter);
  Mk4WeaponInventory inventory;
  mk4_weapon_read(battle_data(), (unsigned)peer->roster, &inventory);
  int type = peer->weapon != NULL ? peer->weapon->type : (int)inventory.type;
  if (type == 0x61) {
    mk4_script_indexed_sound(fighter, 2);
  } else if (type != 0x69) {
    action_native_sound_list(dat_sound_list_weapon_impact_voice);
  }
}

void action_tumble_reaction(Fighter *fighter) {
  uint32_t entry = mk4_reaction_target(battle_data(), (unsigned)fighter->reaction);
  if (entry == native_grounded_tumble_hold_return) {
    mk4_script_stop_body(fighter);
    fighter->body.pos[AXIS_Y] = fighter->body.ground;
  } else {
    mk4_script_disarm_self(fighter);
  }
  if (entry == native_tumble_camera_side_positive || entry == native_tumble_camera_side_negative) {
    battle_wall_recoil(fighter);
  }
  if (entry == native_weapon_tumble_back_getup) {
    weapon_impact_voice(fighter);
  }
  static const MoveDef move = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = ACTION_CODE_4003};
  fighter_start_move(fighter, S_KNOCKDOWN, &move);
  script_native_continue(fighter, entry, false, mk4_script_thrown_ready);
}

static void stagger_tail_finish(Fighter *fighter) {
  /*483fa0 resets the controller context without rebinding its descriptor. */
  fighter_neutral(fighter);
  native_finish_clip(fighter, mk4_mmove_thrown_finish);
}

/*483df0/483ed0 run20 then ten4-sample blocks through429490. Its
 * 4297a0 cursor saturates at the inherited descriptor end, never a fixed
 * fighter clip/count. The CPU controller owns the intervening input hook. */
void action_stagger_tail(Fighter *fighter) {
  int sample = fighter_sample(fighter);
  int last = fighter_move(fighter)->last;
  require(last != MOVE_END && sample >= 0 && sample <= last, "Stagger tail needs a bounded inherited descriptor");
  native_play_rows(fighter, mini(last, sample + 60), 1, stagger_tail_finish);
}

/* These native entries contain only a prelude and a script handoff. All
 * clip selection, motion, waits and recovery remain the original words. */
bool action_prepare_reaction_clip_180_slide_hold(Fighter *fighter, bool alternate) {
  (void)alternate;
  uint32_t physics = mk4_image_u32(battle_data(), dat_reaction_clip_180_friction_callback);
  require(script_native_dispatch(fighter, physics, MK4_PHYSICS, 0), "Unknown native reaction motion callback");
  return true;
}

bool action_prepare_reaction_clip_127_friction(Fighter *fighter, bool alternate) {
  (void)alternate;
  uint32_t physics = mk4_image_u32(battle_data(), dat_reaction_clip_127_friction_row);
  require(script_native_dispatch(fighter, physics, MK4_PHYSICS, 0), "Unknown native recoil motion callback");
  return true;
}

void action_script_reaction(Fighter *fighter) {
  uint32_t entry = mk4_reaction_target(battle_data(), (unsigned)fighter->reaction);
  if (entry == native_goro_special_punch_189_entry) {
    battle_wall_recoil(fighter);
  }
  if (entry == native_goro_special_punch_189_entry || entry == native_reaction_clip_180_slide_hold || entry == native_reaction_clip_132_native_finish ||
      entry == native_sweep_knockdown_victim || entry == native_reaction_clip_127_friction || entry == native_liu_kang_fireball_reaction_entry) {
    mk4_script_disarm_self(fighter);
  }
  if (entry == native_sweep_knockdown_victim) {
    action_native_sound_list(dat_sound_random_grunt_list);
    mk4_script_stop_body(fighter);
    fighter->body.pos[AXIS_Y] = fighter->body.ground;
  } else if (entry == native_liu_kang_fireball_reaction_entry) {
    battle_sound(SOUND_C, 0x5c);
  }
  static const MoveDef move = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = ACTION_CODE_4003};
  fighter_start_move(fighter, S_HIT, &move);
  script_native_continue(fighter, entry, false, mk4_script_thrown_ready);
}

bool action_prepare_goro_special_uppercut_192_entry(Fighter *fighter, bool alternate) {
  (void)alternate;
  fighter->damage_scale = MK4_DAMAGE_SCALE_40_PERCENT;
  Fighter *attacker = fighter_peer(fighter);
  attacker->run_energy = 0;
  attacker->run_delay = 60;
  return true;
}

bool action_prepare_weapon_clip_189_launch_half_damage(Fighter *fighter, bool alternate) {
  (void)alternate;
  fighter->damage_scale = MK4_DAMAGE_SCALE_50_PERCENT;
  return true;
}

bool action_prepare_reptile_dash_victim_scaled(Fighter *fighter, bool alternate) {
  (void)alternate;
  fighter->damage_scale = MK4_DAMAGE_SCALE_60_PERCENT;
  action_native_sound_list(dat_sound_random_grunt_list);
  return true;
}

bool action_prepare_weapon_clip_132_directional_fall(Fighter *fighter, bool alternate) {
  (void)alternate;
  weapon_impact_voice(fighter);
  return true;
}

void action_scaled_reaction(Fighter *fighter) {
  uint32_t entry = mk4_reaction_target(battle_data(), (unsigned)fighter->reaction);
  if (entry == native_goro_special_uppercut_192_entry) {
    mk4_script_shake(fighter, 6);
  }
  if (entry == native_goro_special_uppercut_192_entry || entry == native_uppercut_40_percent_damage || entry == native_weapon_clip_189_launch_half_damage) {
    battle_wall_recoil(fighter);
  }
  mk4_script_disarm_self(fighter);
  static const MoveDef move = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = ACTION_CODE_4003};
  fighter_start_move(fighter, S_KNOCKDOWN, &move);
  script_native_continue(fighter, entry, false, mk4_script_thrown_ready);
}

bool action_prepare_launch_clip_179_action_100a(Fighter *fighter, bool alternate) {
  (void)alternate;
  fighter->action = 0x100a;
  return true;
}

void action_branch_reaction(Fighter *fighter) {
  uint32_t entry = mk4_reaction_target(battle_data(), (unsigned)fighter->reaction);
  bool airborne = fighter_airborne(fighter);
  if (entry == native_forward_fall_context_sound_alias || entry == native_forward_fall_context_sound) {
    mk4_script_context_sound(fighter, 16);
  }
  mk4_script_disarm_self(fighter);
  static const MoveDef move = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = ACTION_CODE_4003};
  fighter_start_move(fighter, S_KNOCKDOWN, &move);
  script_native_continue(fighter, entry, airborne, mk4_script_thrown_ready);
}

/* Native48fed0 places the victim relative to its attacking peer. */
static void reaction_position(Fighter *fighter, q16 forward, q16 y) {
  Fighter *peer = fighter_peer(fighter);
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    fighter->body.pos[axis] = peer->body.pos[axis] + q16_mul(fighter_axis(peer, axis), forward);
  }
  fighter->body.pos[AXIS_Y] = peer->body.pos[AXIS_Y] + y;
}

bool action_prepare_kai_weapon_high_punch_victim_context_sound(Fighter *fighter, bool alternate) {
  (void)alternate;
  reaction_position(fighter, 0x1cccc, -0x8000);
  return true;
}

bool action_prepare_jarek_weapon_reaction_entry(Fighter *fighter, bool alternate) {
  (void)alternate;
  reaction_position(fighter, 0x1b333, 0);
  mk4_script_stop_body(fighter);
  fighter->body.pos[AXIS_Y] = fighter->body.ground;
  return true;
}

bool action_prepare_sonya_weapon_low_punch_victim_context_sound(Fighter *fighter, bool alternate) {
  if (alternate) {
    fighter->body.gravity = 0x28f;
    fighter->body.vel[AXIS_Y] = 0x1999;
  }
  return true;
}

bool action_prepare_weapon_sweep_grounded_victim(Fighter *fighter, bool alternate) {
  (void)alternate;
  action_native_sound_list(dat_sound_random_grunt_list);
  mk4_script_stop_body(fighter);
  fighter->body.pos[AXIS_Y] = fighter->body.ground;
  return true;
}

void action_weapon_reaction(Fighter *fighter) {
  uint32_t entry = mk4_reaction_target(battle_data(), (unsigned)fighter->reaction);
  bool airborne = fighter_airborne(fighter);
  if (entry == native_kai_weapon_high_punch_victim_context_sound || entry == native_sonya_weapon_low_punch_victim_context_sound) {
    mk4_script_context_sound(fighter, 6);
  } else if (entry == native_kai_weapon_high_punch_victim_impact_sound) {
    battle_sound(SOUND_C, 8);
  }
  if (entry != native_reptile_weapon_victim_keep_weapon) {
    mk4_script_disarm_self(fighter);
  }
  if (entry == native_kai_weapon_high_punch_victim_context_sound || entry == native_kai_weapon_high_punch_victim_impact_sound ||
      entry == native_cage_weapon_reaction_entry || entry == native_shinnok_weapon_reaction_entry || entry == native_weapon_low_punch_victim_roll_getup ||
      entry == native_sonya_weapon_low_punch_victim_context_sound) {
    battle_wall_recoil(fighter);
  }
  static const MoveDef move = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = ACTION_CODE_4003};
  fighter_start_move(fighter, S_KNOCKDOWN, &move);
  script_native_continue(fighter, entry, airborne, mk4_script_thrown_ready);
}

static bool uppercut_voice(BattleEvent *event, Fighter *owner, Fighter *peer) {
  (void)event;
  (void)owner;
  (void)peer;
  action_native_sound_list(dat_sound_uppercut_voice_list);
  return false;
}

void action_impact_reaction(Fighter *fighter) {
  uint32_t entry = mk4_reaction_target(battle_data(), (unsigned)fighter->reaction);
  bool stagger = false;
  if (entry == native_uppercut_delayed_voice_40_percent_damage) {
    /*426ad0/426af0 is an independent30-visit delayed audio task. */
    BattleEvent *voice = battle_event_start(fighter, uppercut_voice, false, false);
    require(voice != NULL, "Uppercut needs its native audio task");
    battle_event_delay(voice, 30);
    fighter->damage_scale = MK4_DAMAGE_SCALE_40_PERCENT;
    battle_wall_recoil(fighter);
    mk4_script_disarm_self(fighter);
  } else if (entry == native_impact_wall_counter_60_percent_damage) {
    fighter->damage_scale = MK4_DAMAGE_SCALE_60_PERCENT;
    int64_t x = fighter->body.pos[AXIS_X], z = fighter->body.pos[AXIS_Z], radius = g_battle.arena_radius;
    if (fighter->hits > 2 && x * x + z * z >= radius * radius - 10LL * Q16_ONE * Q16_ONE) {
      battle_react(fighter_peer(fighter), 14);
    }
    mk4_script_disarm_self(fighter);
  } else {
    require(entry == native_hit_clip_324_with_boss_stagger || entry == native_hit_clip_325_with_damage_stagger, "Unexpected impact reaction native");
    fighter->hits = maxi(0, fighter->hits - 1);
    stagger = fighter->damage_total >= 0x3333 || (entry == native_hit_clip_324_with_boss_stagger && fighter->roster == 15);
    if (stagger || fighter->hits >= 3) {
      battle_wall_recoil(fighter);
    }
  }
  static const MoveDef move = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = ACTION_CODE_4003};
  fighter_start_move(fighter, S_KNOCKDOWN, &move);
  uint32_t script = entry == native_impact_wall_counter_60_percent_damage      ? fn_impact_reaction
                    : entry == native_uppercut_delayed_voice_40_percent_damage ? fn_uppercut_victim_recovery
                    : stagger                                                  ? fn_jab_hit_damage_stagger
                    : entry == native_hit_clip_325_with_damage_stagger         ? fn_hit_clip_325_recovery
                                                                               : fn_hit_clip_324_recovery;
  script_vm_enter(fighter, script, mk4_script_thrown_ready);
}

/*483930/483970: native double-step recovery, nine locked visits, then
 * input may select crouch before the descriptor ends. The engine controller
 * owns input dispatch; KO keeps the final native pose. */
static void uppercut_recovery_input(Fighter *fighter) {
  if (fighter->health > 0 && (fighter->held & BUTTON_BIT(BUTTON_DOWN))) {
    fighter_execution_finish(fighter, action_controller_return);
  }
}

void action_uppercut_recover(Fighter *fighter) {
  int first = fighter_sample(fighter), last = fighter_move(fighter)->last;
  require(first >= 0 && last != MOVE_END && first <= last, "Native uppercut recovery needs a bounded descriptor");
  int frames = maxi(9, (last - first + 1) / 2);
  native_begin(fighter, 0, frames - 1, action_getup_back);
  for (int frame = 0; frame < frames; ++frame) {
    native_frame(fighter, frame, mini(last, first + 2 * (frame + 1)), frame >= 9 ? uppercut_recovery_input : NULL);
  }
  native_play(fighter, false);
}

/*416e20/416e50 spawns an independent bounded hit-particle task. The
 * engine presentation service supplies its portable particle artwork. */
static bool hit_particles(BattleEvent *event, Fighter *owner, Fighter *peer) {
  (void)peer;
  battle_particles(owner, 2, 1, true);
  return --event->data[0] > 0;
}

void action_stagger_entry(Fighter *fighter) {
  uint32_t entry = mk4_reaction_target(battle_data(), (unsigned)fighter->reaction);
  bool airborne = fighter_airborne(fighter);
  if (entry == native_hit_stagger_with_particles) {
    BattleEvent *particles = battle_event_start(fighter, hit_particles, false, false);
    require(particles != NULL, "Hit reaction needs its native particle task");
    particles->data[0] = 16;
  }
  if (!airborne || entry != native_hit_stagger_with_particles) {
    mk4_script_disarm_self(fighter);
  }
  if (airborne && entry != native_hit_stagger_with_particles) {
    battle_wall_recoil(fighter);
  }
  static const MoveDef move = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD | SF_REACTION};
  fighter_start_move(fighter, airborne ? S_KNOCKDOWN : S_HIT, &move);
  fighter->action = entry == native_reiko_star_reaction_45 ? 0x2013 : entry == native_hit_stagger_action_1013 ? 0x1013 : 0;
  if (airborne) {
    if (entry == native_hit_stagger_with_particles) {
      mk4_script_indexed_sound(fighter, 1);
      script_vm_enter(fighter, fn_reaction_fall_return, action_controller_return);
    } else {
      script_vm_enter(fighter, fn_guard_airborne_fall, mk4_script_thrown_ready);
    }
    return;
  }
  mk4_script_stop_body(fighter);
  fighter->body.pos[AXIS_Y] = fighter->body.ground;
  uint32_t physics = mk4_image_u32(battle_data(), dat_stagger_entry);
  require(script_native_dispatch(fighter, physics, MK4_PHYSICS, 0), "Unknown native stagger callback");
  script_vm_enter(fighter, fn_stagger_entry_2, mk4_script_thrown_ready);
}

void action_snare_reaction(Fighter *fighter) {
  static const MoveDef move = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = ACTION_CODE_4000};
  fighter_start_move(fighter, S_HIT, &move);
  fighter->flags |= FF_NO_GUARD;
  script_vm_enter(fighter, fn_snare_reaction, mk4_script_thrown_ready);
}

/*45b1f0/480f80 installs the loaded reaction on the captured peer.
 * Already-released spear arrival owns its independent stun continuation. */
void action_script_reaction_dispatch(Fighter *fighter, int reaction) {
  require(reaction >= 0 && reaction < 256, "Invalid script reaction selector");
  Fighter *victim = fighter_peer(fighter);
  uint32_t entry = mk4_reaction_target(battle_data(), (unsigned)reaction);
  if (entry == native_snare_reaction && victim->action == ACTION_CODE_1008) {
    victim->reaction = reaction;
    return;
  }
  if (action_throw_reaction(battle_data(), (unsigned)reaction) != NULL) {
    victim = mk4_script_release_pair(fighter);
  }
  battle_react(victim, reaction);
}

void action_bicycle_release(Fighter *fighter) {
  bool air = fighter->body.ground - fighter->body.pos[AXIS_Y] > 0x3333;
  static const MoveDef move = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = ACTION_CODE_100B};
  fighter_start_move(fighter, S_KNOCKDOWN, &move);
  if (air) {
    script_vm_enter(fighter, fn_raiden_torpedo_air_release, mk4_script_thrown_ready);
  } else {
    mk4_script_stop_body(fighter);
    fighter->body.pos[AXIS_Y] = fighter->body.ground;
    uint32_t physics = mk4_image_u32(battle_data(), dat_bicycle_release);
    require(script_native_dispatch(fighter, physics, MK4_PHYSICS, 0), "Unknown native release callback");
    script_vm_enter(fighter, fn_raiden_torpedo_ground_release, mk4_script_thrown_ready);
  }
}

/*481dd0/481fa0: original fall stream returns through tagged481fb0. */
void reaction_fall_return(Fighter *fighter) {
  static const MoveDef move = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD | SF_REACTION};
  fighter_start_move(fighter, S_KNOCKDOWN, &move);
  script_vm_enter(fighter, fn_reaction_fall_return, mk4_mmove_thrown_finish);
}

void action_spear_arrival(Fighter *fighter) {
  static const MoveDef move = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = ACTION_CODE_1008};
  fighter_start_move(fighter, S_HIT, &move);
  fighter->flags |= FF_NO_GUARD | FF_REACTING;
  mk4_script_stop_body(fighter);
  fighter->body.pos[AXIS_Y] = fighter->body.ground;
  script_vm_enter(fighter, fn_spear_arrival, fighter_ready);
}

static bool spear_arrived(const Fighter *fighter) {
  return (int64_t)fighter->body.vel[AXIS_X] + fighter->body.vel[AXIS_Z] == 0 || g_battle.distance <= 0xcccc;
}

void spear_wait(Fighter *fighter) {
  native_await(fighter, spear_arrived, action_spear_arrival);
}

/*487980: clear the native pair before choosing the loaded fall stream. */
void action_pair_release(Fighter *fighter) {
  fighter->flags &= ~(unsigned)FF_PAIRED;
  fighter_peer(fighter)->flags &= ~(unsigned)FF_PAIRED;
  fighter->paired_release = NULL;
  fighter->capture_action = 0;
  if (fighter_airborne(fighter)) {
    script_vm_enter(fighter, fn_pair_release, mk4_mmove_thrown_finish);
  } else {
    action_controller_return(fighter);
  }
}

static bool pair_owner_released(const Fighter *fighter) {
  const Fighter *owner = fighter_opponent(fighter);
  return fighter->capture_action == 0 || owner->action != fighter->capture_action || owner->pending != NULL || !(fighter->flags & FF_PAIRED);
}

void pair_wait(Fighter *fighter) {
  fighter->capture_action = fighter_peer(fighter)->action;
  fighter->paired_release = action_pair_release;
  fighter->flags |= FF_PAIRED;
  native_await(fighter, pair_owner_released, action_pair_release);
}

/*488890 loads the descriptor anew and resumes from sample4 while captured. */
void action_bicycle_cycle(Fighter *fighter) {
  if (pair_owner_released(fighter) || fighter->script_phase != 0) {
    action_pair_release(fighter);
    return;
  }
  mk4_script_effect(fighter, 14);
  script_vm_bind_animation(fighter, mk4_image_u32(battle_data(), dat_animation_descriptor_bicycle_cycle) * 4, 4);
  native_finish_clip(fighter, action_bicycle_cycle);
}

void action_bicycle_capture(Fighter *fighter) {
  mk4_script_disarm_self(fighter);
  static const MoveDef move = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = ACTION_CODE_4003};
  fighter_start_move(fighter, S_KNOCKDOWN, &move);
  fighter->capture_action = ACTION_CODE_0206;
  fighter->paired_release = action_pair_release;
  fighter->script_phase = 0;
  fighter->flags |= FF_PAIRED | FF_REACTING;
  script_vm_enter(fighter, fn_bicycle_capture, action_pair_release);
}

static void torpedo_return_hold(Fighter *fighter) {
  native_hold(fighter, 64, action_controller_return);
}

void action_torpedo_return(Fighter *fighter) {
  native_finish_clip(fighter, torpedo_return_hold);
}

bool action_prepare_battle_react(Fighter *fighter, bool alternate) {
  (void)alternate;
  battle_react(fighter_peer(fighter), 161);
  return true;
}

bool action_prepare_lightning_victim_friction(Fighter *fighter, bool alternate) {
  (void)alternate;
  uint32_t physics = mk4_image_u32(battle_data(), dat_record_reaction);
  require(script_native_dispatch(fighter, physics, MK4_PHYSICS, 0), "Unknown native record callback");
  return true;
}

bool action_prepare_torpedo_victim_face_peer(Fighter *fighter, bool alternate) {
  (void)alternate;
  action_face_peer(fighter);
  return true;
}

bool action_prepare_torpedo_victim_release_sound_and_shake(Fighter *fighter, bool alternate) {
  (void)alternate;
  battle_sound(SOUND_A, 0x2a);
  mk4_script_shake(fighter, 0);
  action_bicycle_release(fighter);
  return false;
}

bool action_prepare_sonya_weapon_high_punch_victim_position(Fighter *fighter, bool alternate) {
  (void)alternate;
  reaction_position(fighter, 0x1b333, -0x1999);
  return true;
}

bool action_prepare_sonya_weapon_low_punch_victim_impact_sound(Fighter *fighter, bool alternate) {
  battle_sound(SOUND_C, 8);
  if (alternate) {
    fighter->body.gravity = 0x28f;
    fighter->body.vel[AXIS_Y] = 0x1999;
  }
  return true;
}

bool action_prepare_square_wave_victim_face_peer(Fighter *fighter, bool alternate) {
  (void)alternate;
  action_face_peer(fighter);
  action_square_victim(fighter);
  return false;
}

bool action_prepare_cartwheel_victim_scaled_impact(Fighter *fighter, bool alternate) {
  (void)alternate;
  mk4_script_effect(fighter, 9);
  fighter->damage_scale = MK4_DAMAGE_SCALE_40_PERCENT;
  return true;
}

bool action_prepare_cartwheel_victim_impact(Fighter *fighter, bool alternate) {
  (void)alternate;
  mk4_script_effect(fighter, 0);
  return true;
}

void action_record_reaction(Fighter *fighter) {
  uint32_t entry = mk4_reaction_target(battle_data(), (unsigned)fighter->reaction);
  bool airborne = fighter_airborne(fighter);
  if (entry == native_battle_react || entry == native_staff_high_punch_weapon_victim || entry == native_sonya_crouch_high_kick_victim_keep_weapon ||
      entry == native_sonya_weapon_low_punch_victim_impact_sound || entry == native_cartwheel_victim_scaled_impact || entry == native_cartwheel_victim_impact ||
      entry == native_cartwheel_victim_recoil) {
    battle_wall_recoil(fighter);
  }
  if (entry != native_torpedo_victim_face_peer && entry != native_sonya_crouch_high_kick_victim_keep_weapon) {
    mk4_script_disarm_self(fighter);
  }
  static const MoveDef move = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = ACTION_CODE_4003};
  fighter_start_move(fighter, S_KNOCKDOWN, &move);
  script_native_continue(fighter, entry, airborne, mk4_script_thrown_ready);
}

/* Additional original reaction-table entries use the same typed native
 * prelude/script boundary; none selects by fighter or copied reaction ID. */
bool action_prepare_rear_hit_turn_and_launch(Fighter *fighter, bool alternate) {
  (void)alternate;
  action_face_peer(fighter);
  fighter->body.angle[AXIS_Y] += 0x32492;
  return true;
}

bool action_prepare_spear_hit_victim_60_percent_damage(Fighter *fighter, bool alternate) {
  (void)alternate;
  fighter->damage_scale = MK4_DAMAGE_SCALE_60_PERCENT;
  mk4_script_stop_body(fighter);
  fighter->body.pos[AXIS_Y] = fighter->body.ground;
  return true;
}

bool action_prepare_cage_groin_punch_victim_reaction(Fighter *fighter, bool alternate) {
  (void)alternate;
  fighter->action = 0x1014;
  mk4_script_stop_body(fighter);
  fighter->body.pos[AXIS_Y] = fighter->body.ground;
  uint32_t physics = mk4_image_u32(battle_data(), dat_aux_reaction);
  require(script_native_dispatch(fighter, physics, MK4_PHYSICS, 0), "Unknown native impact friction callback");
  return true;
}

void action_aux_reaction(Fighter *fighter) {
  uint32_t entry = action_reaction_native(fighter);
  if (entry == native_weapon_clip_189_launch_back_getup) {
    battle_wall_recoil(fighter);
  }
  if (entry == native_weapon_clip_189_launch_back_getup || entry == native_goro_kick_victim_disarm || entry == native_launch_clip_131_shake_impact ||
      entry == native_spear_hit_victim_60_percent_damage || entry == native_cage_groin_punch_victim_reaction) {
    mk4_script_disarm_self(fighter);
  }
  static const MoveDef move = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = ACTION_CODE_4003};
  fighter_start_move(fighter, S_KNOCKDOWN, &move);
  script_native_continue(fighter, entry, false, mk4_script_thrown_ready);
}

static void stun_shift(Fighter *fighter) {
  fighter->visual_offset[AXIS_Y] = -0x147a;
}

static void stun_reset(Fighter *fighter) {
  fighter->visual_offset[AXIS_Y] = 0;
}

/*486000/4860d0: ten native3+3 shake cycles and15-visit settle.
 * Use presentation offset so render jitter does not alter combat geometry. */
void action_shake_stun(Fighter *fighter) {
  int sample = fighter_sample(fighter);
  native_begin(fighter, 0, 74, action_controller_return);
  fighter->execution.cleanup = stun_reset;
  for (int frame = 0; frame < 75; ++frame) {
    FighterFn action = frame < 60 && frame % 6 == 0 ? stun_shift : frame < 60 && frame % 6 == 3 ? stun_reset : NULL;
    native_frame(fighter, frame, sample, action);
  }
  native_play(fighter, false);
}

void action_guard_fall(Fighter *fighter) {
  bool airborne = fighter_airborne(fighter);
  action_native_sound_list(fighter->armed ? dat_sound_armed_impact_list : dat_sound_unarmed_impact_list);
  if (airborne) {
    mk4_script_disarm_self(fighter);
    battle_wall_recoil(fighter);
  }
  static const MoveDef move = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = ACTION_CODE_4003};
  fighter_start_move(fighter, S_KNOCKDOWN, &move);
  script_vm_enter(fighter, airborne ? fn_guard_airborne_fall : fn_guard_grounded_stagger, mk4_script_thrown_ready);
}

FighterFn action_reaction_entry(const Mk4Image *image, unsigned reaction) {
  return script_native_reaction(mk4_reaction_target(image, reaction));
}
