#include "retail_layout.h"
#include "native_services.h"
#include "retail_data.h"

uint32_t retail_reaction_native(const Fighter *fighter) {
  return retail_reaction_target(retail_vm_image(), (unsigned)fighter->reaction);
}

/* Native482aa0: the impact voice depends on the captured peer, independently
 * of the selected script/model aliases. Script words supply subsequent sound. */
static void stagger_voice(Fighter *fighter) {
  if (fighter_peer(fighter)->roster == 13) {
    retail_native_sound_list(RETAIL_SOUND_UNARMED_IMPACT_LIST);
    retail_native_sound_list(RETAIL_SOUND_KNOCKDOWN_VOICE_LIST);
  } else {
    mk4_script_indexed_sound(fighter, 1);
  }
}

void retail_stagger_reaction(Fighter *fighter) {
  uint32_t entry = retail_reaction_target(retail_vm_image(), (unsigned)fighter->reaction);
  int previous_action = fighter->action;
  bool crouch = previous_action == ACTION_RETAIL_2001 || previous_action == ACTION_RETAIL_2002;
  if (entry == RETAIL_NATIVE_STAGGER_REACTION_482E00) {
    crouch |= previous_action == 0x2010 || previous_action == ACTION_RETAIL_0106 || previous_action == ACTION_RETAIL_0107;
  }
  if (entry == RETAIL_NATIVE_STAGGER_REACTION_482E00 || entry == RETAIL_NATIVE_STAGGER_REACTION_482830 || entry == RETAIL_NATIVE_STAGGER_REACTION_482870 ||
      entry == RETAIL_NATIVE_STAGGER_REACTION_4828D0) {
    battle_wall_recoil(fighter);
  }
  static const MoveDef move = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = ACTION_RETAIL_4001};
  fighter_start_move(fighter, S_HIT, &move);
  uint32_t script;
  switch (entry) {
    case RETAIL_NATIVE_STAGGER_REACTION_482E00:
      mk4_script_disarm_self(fighter);
      if (crouch) {
        uint32_t physics = mk4_image_u32(retail_vm_image(), RETAIL_CROUCH_REACTION_PHYSICS_WORD);
        require(retail_native_vm(fighter, physics, RETAIL_PHYSICS, 0), "Unknown crouch reaction motion callback");
      }
      script = crouch ? RETAIL_CROUCH_REACTION_SCRIPT : MK4_RETAIL_SCRIPT_KAI_HANDSTAND_REACTION_STANDING_STAGGER;
      break;
    case RETAIL_NATIVE_STAGGER_REACTION_482830:
      script = MK4_RETAIL_SCRIPT_STAGGER_REACTION_4E90E0;
      break;
    case RETAIL_NATIVE_STAGGER_REACTION_482870:
    case RETAIL_NATIVE_STAGGER_REACTION_4828D0:
      mk4_script_disarm_self(fighter);
      retail_native_sound_list(MK4_RETAIL_SOUND_LIST_STAGGER_REACTION);
      script = crouch                                           ? MK4_RETAIL_SCRIPT_STAGGER_REACTION_4E9170
               : entry == RETAIL_NATIVE_STAGGER_REACTION_482870 ? MK4_RETAIL_SCRIPT_STAGGER_REACTION_4E9130
                                                                : MK4_RETAIL_SCRIPT_STAGGER_REACTION_4E9150;
      break;
    case RETAIL_NATIVE_STAGGER_REACTION_482BE0:
      fighter->damage_scale = RETAIL_DAMAGE_SCALE_60_PERCENT;
      script = MK4_RETAIL_SCRIPT_STAGGER_REACTION_4E9260;
      break;
    case RETAIL_NATIVE_STAGGER_REACTION_482E80:
    case RETAIL_NATIVE_STAGGER_REACTION_482E90:
      fighter->damage_scale = RETAIL_DAMAGE_SCALE_50_PERCENT;
      mk4_script_disarm_self(fighter);
      stagger_voice(fighter);
      fighter->action = ACTION_RETAIL_4003;
      script = MK4_RETAIL_SCRIPT_STAGGER_REACTION_4E9388;
      break;
    case RETAIL_NATIVE_STAGGER_REACTION_482B10:
      fighter->action = 0x4002;
      stagger_voice(fighter);
      fighter->damage_scale = RETAIL_DAMAGE_SCALE_60_PERCENT;
      script = MK4_RETAIL_SCRIPT_REACT_KICK_HIT_4E9238;
      break;
    default:
      require(false, "Unexpected shared stagger native");
      return;
  }
  retail_vm_enter(fighter, script, mk4_script_thrown_ready);
}

void retail_launch_reaction(Fighter *fighter) {
  uint32_t entry = retail_reaction_target(retail_vm_image(), (unsigned)fighter->reaction);
  if (entry != RETAIL_NATIVE_LAUNCH_REACTION_4835E0 && entry != RETAIL_NATIVE_LAUNCH_REACTION_481350) {
    mk4_script_disarm_self(fighter);
  }
  if (entry == RETAIL_NATIVE_LAUNCH_REACTION_481320) {
    battle_wall_recoil(fighter);
  }
  static const MoveDef move = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = ACTION_RETAIL_4003};
  fighter_start_move(fighter, S_KNOCKDOWN, &move);
  uint32_t script;
  switch (entry) {
    case RETAIL_NATIVE_LAUNCH_REACTION_481320:
    case RETAIL_NATIVE_LAUNCH_REACTION_481350:
      script = MK4_RETAIL_SCRIPT_GUARD_FALL_4E89A0;
      break;
    case RETAIL_NATIVE_LAUNCH_REACTION_482560:
      script = MK4_RETAIL_SCRIPT_LAUNCH_REACTION_4E8EA8;
      break;
    case RETAIL_NATIVE_LAUNCH_REACTION_4825C0:
      script = MK4_RETAIL_SCRIPT_LAUNCH_REACTION_4E8F10;
      break;
    case RETAIL_NATIVE_LAUNCH_REACTION_4835E0:
      script = MK4_RETAIL_SCRIPT_LAUNCH_REACTION_4E96C8;
      break;
    default:
      require(false, "Unexpected shared launch native");
      return;
  }
  retail_vm_enter(fighter, script, mk4_script_thrown_ready);
}

/* 482790 and483610 both finish the inherited descriptor; their native
 * completion differs. Engine continuations replace the task84 resume flag. */
void retail_reaction_getup_back(Fighter *fighter) {
  mk4_mmove_finish_getup(fighter, MK4_RETAIL_GETUP_DESCRIPTOR_FLIP_GETUP);
}

void retail_reaction_finish(Fighter *fighter) {
  native_finish_clip(fighter, mk4_mmove_thrown_finish);
}

void retail_reaction_fall(Fighter *fighter) {
  mk4_script_disarm_self(fighter);
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_REACTION_FALL, mk4_script_thrown_ready);
}

/* Native4826e0 selects impact audio from the captured attacker weapon. */
static void weapon_impact_voice(Fighter *fighter) {
  const Fighter *peer = fighter_peer(fighter);
  RetailWeaponInventory inventory;
  retail_weapon_read(retail_vm_image(), (unsigned)peer->roster, &inventory);
  int type = peer->weapon != NULL ? peer->weapon->type : (int)inventory.type;
  if (type == 0x61) {
    mk4_script_indexed_sound(fighter, 2);
  } else if (type != 0x69) {
    retail_native_sound_list(MK4_RETAIL_SOUND_LIST_WEAPON_IMPACT_VOICE);
  }
}

void retail_tumble_reaction(Fighter *fighter) {
  uint32_t entry = retail_reaction_target(retail_vm_image(), (unsigned)fighter->reaction);
  if (entry == RETAIL_NATIVE_TUMBLE_REACTION_482730) {
    mk4_script_stop_body(fighter);
    fighter->body.pos[AXIS_Y] = fighter->body.ground;
  } else {
    mk4_script_disarm_self(fighter);
  }
  if (entry == RETAIL_NATIVE_TUMBLE_REACTION_483450 || entry == RETAIL_NATIVE_TUMBLE_REACTION_483490) {
    battle_wall_recoil(fighter);
  }
  if (entry == RETAIL_NATIVE_TUMBLE_REACTION_4826A0) {
    weapon_impact_voice(fighter);
  }
  static const MoveDef move = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = ACTION_RETAIL_4003};
  fighter_start_move(fighter, S_KNOCKDOWN, &move);
  uint32_t script;
  FighterFn finish = mk4_script_thrown_ready;
  switch (entry) {
    case RETAIL_NATIVE_TUMBLE_REACTION_483450:
      script = MK4_RETAIL_SCRIPT_TUMBLE_REACTION_4E9508;
      finish = retail_getup_back;
      break;
    case RETAIL_NATIVE_TUMBLE_REACTION_483490:
      script = MK4_RETAIL_SCRIPT_TUMBLE_REACTION_4E9568;
      finish = retail_getup_back;
      break;
    case RETAIL_NATIVE_TUMBLE_REACTION_482750:
      script = MK4_RETAIL_SCRIPT_TUMBLE_REACTION_4E9048;
      break;
    case RETAIL_NATIVE_TUMBLE_REACTION_482730:
      script = MK4_RETAIL_SCRIPT_TUMBLE_REACTION_4E8FE8;
      break;
    case RETAIL_NATIVE_TUMBLE_REACTION_4826A0:
      script = MK4_RETAIL_SCRIPT_TUMBLE_REACTION_4E8F80;
      finish = retail_getup_back;
      break;
    default:
      require(false, "Unexpected shared tumble native");
      return;
  }
  retail_vm_enter(fighter, script, finish);
}

static void stagger_tail_finish(Fighter *fighter) {
  /*483fa0 resets the controller context without rebinding its descriptor. */
  fighter_neutral(fighter);
  native_finish_clip(fighter, mk4_mmove_thrown_finish);
}

/*483df0/483ed0 run20 then ten4-sample blocks through429490. Its
 * 4297a0 cursor saturates at the inherited descriptor end, never a fixed
 * fighter clip/count. The CPU controller owns the intervening input hook. */
void retail_stagger_tail(Fighter *fighter) {
  int sample = fighter_sample(fighter);
  int last = fighter_move(fighter)->last;
  require(last != MOVE_END && sample >= 0 && sample <= last, "Stagger tail needs a bounded inherited descriptor");
  native_play_rows(fighter, mini(last, sample + 60), 1, stagger_tail_finish);
}

/* These native entries contain only a prelude and a script handoff. All
 * clip selection, motion, waits and recovery remain the original words. */
void retail_script_reaction(Fighter *fighter) {
  uint32_t entry = retail_reaction_target(retail_vm_image(), (unsigned)fighter->reaction);
  if (entry == RETAIL_NATIVE_GORO_SPECIAL_PUNCH_189_ENTRY) {
    battle_wall_recoil(fighter);
  }
  if (entry == RETAIL_NATIVE_GORO_SPECIAL_PUNCH_189_ENTRY || entry == RETAIL_NATIVE_SCRIPT_REACTION_482580 || entry == RETAIL_NATIVE_SCRIPT_REACTION_482770 ||
      entry == RETAIL_NATIVE_SCRIPT_REACTION_4831E0 || entry == RETAIL_NATIVE_SCRIPT_REACTION_4866D0 ||
      entry == RETAIL_NATIVE_LIU_KANG_FIREBALL_REACTION_ENTRY) {
    mk4_script_disarm_self(fighter);
  }
  if (entry == RETAIL_NATIVE_SCRIPT_REACTION_4831E0) {
    retail_native_sound_list(RETAIL_SOUND_RANDOM_GRUNT_LIST);
    mk4_script_stop_body(fighter);
    fighter->body.pos[AXIS_Y] = fighter->body.ground;
  } else if (entry == RETAIL_NATIVE_LIU_KANG_FIREBALL_REACTION_ENTRY) {
    battle_sound(SOUND_C, 0x5c);
  }
  static const MoveDef move = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = ACTION_RETAIL_4003};
  fighter_start_move(fighter, S_HIT, &move);
  uint32_t script;
  switch (entry) {
    case RETAIL_NATIVE_SCRIPT_REACTION_482260:
      script = MK4_RETAIL_SCRIPT_SCRIPT_REACTION_4E8C78;
      break;
    case RETAIL_NATIVE_SCRIPT_REACTION_482410:
      script = MK4_RETAIL_SCRIPT_SCRIPT_REACTION_4E8DA8;
      break;
    case RETAIL_NATIVE_GORO_SPECIAL_PUNCH_189_ENTRY:
      script = MK4_RETAIL_SCRIPT_GORO_SPECIAL_PUNCH_189_VICTIM;
      break;
    case RETAIL_NATIVE_SCRIPT_REACTION_482580: {
      uint32_t physics = mk4_image_u32(retail_vm_image(), MK4_RETAIL_DATA_SCRIPT_REACTION_4E8EDC);
      require(retail_native_vm(fighter, physics, RETAIL_PHYSICS, 0), "Unknown native reaction motion callback");
      script = MK4_RETAIL_SCRIPT_SCRIPT_REACTION_4E8EE0;
      break;
    }
    case RETAIL_NATIVE_SCRIPT_REACTION_482770:
      script = MK4_RETAIL_SCRIPT_SCRIPT_REACTION_4E90B0;
      break;
    case RETAIL_NATIVE_SCRIPT_REACTION_4831E0:
      script = MK4_RETAIL_SCRIPT_SCRIPT_REACTION_4E94B0;
      break;
    case RETAIL_NATIVE_SCRIPT_REACTION_4866D0: {
      uint32_t physics = mk4_image_u32(retail_vm_image(), MK4_RETAIL_DATA_SCRIPT_REACTION_4E9D80);
      require(retail_native_vm(fighter, physics, RETAIL_PHYSICS, 0), "Unknown native recoil motion callback");
      script = MK4_RETAIL_SCRIPT_SCRIPT_REACTION_4E9D88;
      break;
    }
    case RETAIL_NATIVE_LIU_KANG_FIREBALL_REACTION_ENTRY:
      script = MK4_RETAIL_SCRIPT_SONYA_RING_VICTIM;
      break;
    default:
      require(false, "Unexpected script reaction native");
      return;
  }
  retail_vm_enter(fighter, script, mk4_script_thrown_ready);
}

void retail_scaled_reaction(Fighter *fighter) {
  uint32_t entry = retail_reaction_target(retail_vm_image(), (unsigned)fighter->reaction);
  if (entry == RETAIL_NATIVE_GORO_SPECIAL_UPPERCUT_192_ENTRY) {
    mk4_script_shake(fighter, 6);
  }
  if (entry == RETAIL_NATIVE_GORO_SPECIAL_UPPERCUT_192_ENTRY || entry == RETAIL_NATIVE_SCALED_REACTION_4824A0 ||
      entry == RETAIL_NATIVE_SCALED_REACTION_4823C0) {
    battle_wall_recoil(fighter);
  }
  mk4_script_disarm_self(fighter);
  static const MoveDef move = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = ACTION_RETAIL_4003};
  fighter_start_move(fighter, S_KNOCKDOWN, &move);
  uint32_t script;
  FighterFn finish = mk4_script_thrown_ready;
  switch (entry) {
    case RETAIL_NATIVE_GORO_SPECIAL_UPPERCUT_192_ENTRY:
    case RETAIL_NATIVE_SCALED_REACTION_4824A0: {
      fighter->damage_scale = RETAIL_DAMAGE_SCALE_40_PERCENT;
      Fighter *attacker = fighter_peer(fighter);
      attacker->run_energy = 0;
      attacker->run_delay = 60;
      script = MK4_RETAIL_SCRIPT_GORO_SPECIAL_UPPERCUT_192_VICTIM;
      break;
    }
    case RETAIL_NATIVE_SCALED_REACTION_4823C0:
      fighter->damage_scale = RETAIL_DAMAGE_SCALE_50_PERCENT;
      script = MK4_RETAIL_SCRIPT_SCALED_REACTION_4E8D70;
      finish = retail_getup_roll;
      break;
    case RETAIL_NATIVE_SCALED_REACTION_483500:
      fighter->damage_scale = RETAIL_DAMAGE_SCALE_60_PERCENT;
      retail_native_sound_list(RETAIL_SOUND_RANDOM_GRUNT_LIST);
      script = MK4_RETAIL_SCRIPT_REPTILE_DASH_VICTIM;
      break;
    case RETAIL_NATIVE_SCALED_REACTION_482F60:
      weapon_impact_voice(fighter);
      script = MK4_RETAIL_SCRIPT_SCALED_REACTION_4E93D8;
      break;
    default:
      require(false, "Unexpected scaled reaction native");
      return;
  }
  retail_vm_enter(fighter, script, finish);
}

void retail_branch_reaction(Fighter *fighter) {
  uint32_t entry = retail_reaction_target(retail_vm_image(), (unsigned)fighter->reaction);
  bool airborne = fighter_airborne(fighter);
  if (entry == RETAIL_NATIVE_BRANCH_REACTION_482EF0 || entry == RETAIL_NATIVE_BRANCH_REACTION_482F00) {
    mk4_script_context_sound(fighter, 16);
  }
  mk4_script_disarm_self(fighter);
  static const MoveDef move = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = ACTION_RETAIL_4003};
  fighter_start_move(fighter, S_KNOCKDOWN, &move);
  uint32_t script;
  FighterFn finish = mk4_script_thrown_ready;
  switch (entry) {
    case RETAIL_NATIVE_BRANCH_REACTION_482EF0:
    case RETAIL_NATIVE_BRANCH_REACTION_482F00:
      script = airborne ? MK4_RETAIL_SCRIPT_BRANCH_REACTION_4E9430 : MK4_RETAIL_SCRIPT_BRANCH_REACTION_4E93A8;
      finish = retail_getup_front;
      break;
    case RETAIL_NATIVE_BRANCH_REACTION_485F10:
      script = airborne ? MK4_RETAIL_SCRIPT_BRANCH_REACTION_4E9430 : MK4_RETAIL_SCRIPT_BRANCH_REACTION_4E9C80;
      if (airborne) {
        finish = retail_getup_front;
      }
      break;
    case RETAIL_NATIVE_BRANCH_REACTION_485F50:
      fighter->action = 0x100a;
      script = MK4_RETAIL_SCRIPT_BRANCH_REACTION_4E9CA8;
      break;
    case RETAIL_NATIVE_BRANCH_REACTION_484BC0:
      script = airborne ? MK4_RETAIL_SCRIPT_BRANCH_REACTION_4E9980 : MK4_RETAIL_SCRIPT_SCORPION_TELEPORT_PUNCH_GROUNDED_VICTIM;
      break;
    default:
      require(false, "Unexpected branching reaction native");
      return;
  }
  retail_vm_enter(fighter, script, finish);
}

/* Native48fed0 places the victim relative to its attacking peer. */
static void reaction_position(Fighter *fighter, q16 forward, q16 y) {
  Fighter *peer = fighter_peer(fighter);
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    fighter->body.pos[axis] = peer->body.pos[axis] + q16_mul(fighter_axis(peer, axis), forward);
  }
  fighter->body.pos[AXIS_Y] = peer->body.pos[AXIS_Y] + y;
}

void retail_weapon_reaction(Fighter *fighter) {
  uint32_t entry = retail_reaction_target(retail_vm_image(), (unsigned)fighter->reaction);
  bool airborne = fighter_airborne(fighter);
  if (entry == RETAIL_NATIVE_REACTION_POSITION_482110 || entry == RETAIL_NATIVE_WEAPON_REACTION_4825E0) {
    mk4_script_context_sound(fighter, 6);
  } else if (entry == RETAIL_NATIVE_REACTION_POSITION_482130) {
    battle_sound(SOUND_C, 8);
  }
  if (entry != RETAIL_NATIVE_WEAPON_REACTION_482270) {
    mk4_script_disarm_self(fighter);
  }
  if (entry == RETAIL_NATIVE_REACTION_POSITION_482110 || entry == RETAIL_NATIVE_REACTION_POSITION_482130 || entry == RETAIL_NATIVE_CAGE_WEAPON_REACTION_ENTRY ||
      entry == RETAIL_NATIVE_SHINNOK_WEAPON_REACTION_ENTRY || entry == RETAIL_NATIVE_WEAPON_REACTION_482380 || entry == RETAIL_NATIVE_WEAPON_REACTION_4825E0) {
    battle_wall_recoil(fighter);
  }
  static const MoveDef move = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = ACTION_RETAIL_4003};
  fighter_start_move(fighter, S_KNOCKDOWN, &move);
  uint32_t script;
  FighterFn finish = mk4_script_thrown_ready;
  switch (entry) {
    case RETAIL_NATIVE_REACTION_POSITION_482110:
    case RETAIL_NATIVE_REACTION_POSITION_482130:
      reaction_position(fighter, 0x1cccc, -0x8000);
      script = MK4_RETAIL_SCRIPT_KAI_DRAW_WEAPON_HP_VICTIM;
      break;
    case RETAIL_NATIVE_REPTILE_WEAPON_REACTION_ENTRY:
      script = MK4_RETAIL_SCRIPT_KAI_DRAW_WEAPON_BACK_VICTIM;
      break;
    case RETAIL_NATIVE_JAREK_WEAPON_REACTION_ENTRY:
      reaction_position(fighter, 0x1b333, 0);
      mk4_script_stop_body(fighter);
      fighter->body.pos[AXIS_Y] = fighter->body.ground;
      script = MK4_RETAIL_SCRIPT_JAREK_WEAPON_REACTION_KNIFE_VICTIM;
      break;
    case RETAIL_NATIVE_WEAPON_REACTION_482270:
      script = MK4_RETAIL_SCRIPT_REPTILE_WEAPON_REACTION_REACTION_137;
      break;
    case RETAIL_NATIVE_CAGE_WEAPON_REACTION_ENTRY:
      fighter->damage_scale = RETAIL_DAMAGE_SCALE_50_PERCENT;
      script = MK4_RETAIL_SCRIPT_REPTILE_WEAPON_REACTION_REACTION_144;
      break;
    case RETAIL_NATIVE_SHINNOK_WEAPON_REACTION_ENTRY:
      script = MK4_RETAIL_SCRIPT_SHINNOK_WEAPON_REACTION_LP_VICTIM;
      break;
    case RETAIL_NATIVE_WEAPON_REACTION_482380:
      script = MK4_RETAIL_SCRIPT_CAGE_WEAPON_REACTION_LP_VICTIM;
      finish = retail_getup_roll;
      break;
    case RETAIL_NATIVE_WEAPON_REACTION_4825E0:
      if (airborne) {
        fighter->body.gravity = 0x28f;
        fighter->body.vel[AXIS_Y] = 0x1999;
      }
      script = MK4_RETAIL_SCRIPT_SONYA_WEAPON_LP_VICTIM;
      break;
    case RETAIL_NATIVE_WEAPON_REACTION_483050:
      retail_native_sound_list(RETAIL_SOUND_RANDOM_GRUNT_LIST);
      mk4_script_stop_body(fighter);
      fighter->body.pos[AXIS_Y] = fighter->body.ground;
      script = MK4_RETAIL_SCRIPT_SHINNOK_WEAPON_REACTION_SWEEP_VICTIM;
      break;
    default:
      require(false, "Unexpected weapon reaction native");
      return;
  }
  retail_vm_enter(fighter, script, finish);
}

static bool uppercut_voice(BattleEvent *event, Fighter *owner, Fighter *peer) {
  (void)event;
  (void)owner;
  (void)peer;
  retail_native_sound_list(RETAIL_SOUND_UPPERCUT_VOICE_LIST);
  return false;
}

void retail_impact_reaction(Fighter *fighter) {
  uint32_t entry = retail_reaction_target(retail_vm_image(), (unsigned)fighter->reaction);
  bool stagger = false;
  if (entry == RETAIL_NATIVE_IMPACT_REACTION_4838E0) {
    /*426ad0/426af0 is an independent30-visit delayed audio task. */
    BattleEvent *voice = battle_event_start(fighter, uppercut_voice, false, false);
    require(voice != NULL, "Uppercut needs its native audio task");
    battle_event_delay(voice, 30);
    fighter->damage_scale = RETAIL_DAMAGE_SCALE_40_PERCENT;
    battle_wall_recoil(fighter);
    mk4_script_disarm_self(fighter);
  } else if (entry == RETAIL_NATIVE_IMPACT_REACTION_481FC0) {
    fighter->damage_scale = RETAIL_DAMAGE_SCALE_60_PERCENT;
    int64_t x = fighter->body.pos[AXIS_X], z = fighter->body.pos[AXIS_Z], radius = g_battle.arena_radius;
    if (fighter->hits > 2 && x * x + z * z >= radius * radius - 10LL * Q16_ONE * Q16_ONE) {
      battle_react(fighter_peer(fighter), 14);
    }
    mk4_script_disarm_self(fighter);
  } else {
    require(entry == RETAIL_NATIVE_IMPACT_REACTION_485E30 || entry == RETAIL_NATIVE_IMPACT_REACTION_485EA0, "Unexpected impact reaction native");
    fighter->hits = maxi(0, fighter->hits - 1);
    stagger = fighter->damage_total >= 0x3333 || (entry == RETAIL_NATIVE_IMPACT_REACTION_485E30 && fighter->roster == 15);
    if (stagger || fighter->hits >= 3) {
      battle_wall_recoil(fighter);
    }
  }
  static const MoveDef move = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = ACTION_RETAIL_4003};
  fighter_start_move(fighter, S_KNOCKDOWN, &move);
  uint32_t script = entry == RETAIL_NATIVE_IMPACT_REACTION_481FC0   ? MK4_RETAIL_SCRIPT_IMPACT_REACTION
                    : entry == RETAIL_NATIVE_IMPACT_REACTION_4838E0 ? MK4_RETAIL_DATA_IMPACT_REACTION_4E9758
                    : stagger                                       ? MK4_RETAIL_SCRIPT_REACT_JAB_HIT_4E97D0
                    : entry == RETAIL_NATIVE_IMPACT_REACTION_485EA0 ? MK4_RETAIL_DATA_IMPACT_REACTION_4E9C40
                                                                    : MK4_RETAIL_DATA_IMPACT_REACTION_4E9C28;
  retail_vm_enter(fighter, script, mk4_script_thrown_ready);
}

/*483930/483970: native double-step recovery, nine locked visits, then
 * input may select crouch before the descriptor ends. The engine controller
 * owns input dispatch; KO keeps the final native pose. */
static void uppercut_recovery_input(Fighter *fighter) {
  if (fighter->health > 0 && (fighter->held & BUTTON_BIT(BUTTON_DOWN))) {
    fighter_finish_mmove(fighter, retail_controller_return);
  }
}

void retail_uppercut_recover(Fighter *fighter) {
  int first = fighter_sample(fighter), last = fighter_move(fighter)->last;
  require(first >= 0 && last != MOVE_END && first <= last, "Native uppercut recovery needs a bounded descriptor");
  int frames = maxi(9, (last - first + 1) / 2);
  native_begin(fighter, 0, frames - 1, retail_getup_back);
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

void retail_stagger_entry(Fighter *fighter) {
  uint32_t entry = retail_reaction_target(retail_vm_image(), (unsigned)fighter->reaction);
  bool airborne = fighter_airborne(fighter);
  if (entry == RETAIL_NATIVE_STAGGER_ENTRY_483C50) {
    BattleEvent *particles = battle_event_start(fighter, hit_particles, false, false);
    require(particles != NULL, "Hit reaction needs its native particle task");
    particles->data[0] = 16;
  }
  if (!airborne || entry != RETAIL_NATIVE_STAGGER_ENTRY_483C50) {
    mk4_script_disarm_self(fighter);
  }
  if (airborne && entry != RETAIL_NATIVE_STAGGER_ENTRY_483C50) {
    battle_wall_recoil(fighter);
  }
  static const MoveDef move = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD | SF_REACTION};
  fighter_start_move(fighter, airborne ? S_KNOCKDOWN : S_HIT, &move);
  fighter->action = entry == RETAIL_NATIVE_REIKO_STAR_REACTION_45 ? 0x2013 : entry == RETAIL_NATIVE_STAGGER_ENTRY_483D60 ? 0x1013 : 0;
  if (airborne) {
    if (entry == RETAIL_NATIVE_STAGGER_ENTRY_483C50) {
      mk4_script_indexed_sound(fighter, 1);
      retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_REACTION_FALL_RETURN, retail_controller_return);
    } else {
      retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_GUARD_FALL_4E89A0, mk4_script_thrown_ready);
    }
    return;
  }
  mk4_script_stop_body(fighter);
  fighter->body.pos[AXIS_Y] = fighter->body.ground;
  uint32_t physics = mk4_image_u32(retail_vm_image(), MK4_RETAIL_DATA_STAGGER_ENTRY);
  require(retail_native_vm(fighter, physics, RETAIL_PHYSICS, 0), "Unknown native stagger callback");
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_STAGGER_ENTRY, mk4_script_thrown_ready);
}

void retail_snare_reaction(Fighter *fighter) {
  static const MoveDef move = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = ACTION_RETAIL_4000};
  fighter_start_move(fighter, S_HIT, &move);
  fighter->flags |= FF_NO_GUARD;
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_SNARE_REACTION, mk4_script_thrown_ready);
}

/*45b1f0/480f80 installs the loaded reaction on the captured peer.
 * Already-released spear arrival owns its independent stun continuation. */
void retail_script_reaction_dispatch(Fighter *fighter, int reaction) {
  require(reaction >= 0 && reaction < 256, "Invalid script reaction selector");
  Fighter *victim = fighter_peer(fighter);
  uint32_t entry = retail_reaction_target(retail_vm_image(), (unsigned)reaction);
  if (entry == RETAIL_NATIVE_SNARE_REACTION && victim->action == ACTION_RETAIL_1008) {
    victim->reaction = reaction;
    return;
  }
  if (retail_throw_reaction(retail_vm_image(), (unsigned)reaction) != NULL) {
    victim = mk4_script_release_pair(fighter);
  }
  battle_react(victim, reaction);
}

void retail_bicycle_release(Fighter *fighter) {
  bool air = fighter->body.ground - fighter->body.pos[AXIS_Y] > 0x3333;
  static const MoveDef move = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = ACTION_RETAIL_100B};
  fighter_start_move(fighter, S_KNOCKDOWN, &move);
  if (air) {
    retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_RAIDEN_TORPEDO_AIR_RELEASE, mk4_script_thrown_ready);
  } else {
    mk4_script_stop_body(fighter);
    fighter->body.pos[AXIS_Y] = fighter->body.ground;
    uint32_t physics = mk4_image_u32(retail_vm_image(), MK4_RETAIL_DATA_BICYCLE_RELEASE);
    require(retail_native_vm(fighter, physics, RETAIL_PHYSICS, 0), "Unknown native release callback");
    retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_RAIDEN_TORPEDO_GROUND_RELEASE, mk4_script_thrown_ready);
  }
}

/*481dd0/481fa0: original fall stream returns through tagged481fb0. */
static void reaction_fall_return(Fighter *fighter) {
  static const MoveDef move = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD | SF_REACTION};
  fighter_start_move(fighter, S_KNOCKDOWN, &move);
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_REACTION_FALL_RETURN, mk4_mmove_thrown_finish);
}

void retail_spear_arrival(Fighter *fighter) {
  static const MoveDef move = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = ACTION_RETAIL_1008};
  fighter_start_move(fighter, S_HIT, &move);
  fighter->flags |= FF_NO_GUARD | FF_REACTING;
  mk4_script_stop_body(fighter);
  fighter->body.pos[AXIS_Y] = fighter->body.ground;
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_SPEAR_ARRIVAL, fighter_ready);
}

static bool spear_arrived(const Fighter *fighter) {
  return (int64_t)fighter->body.vel[AXIS_X] + fighter->body.vel[AXIS_Z] == 0 || g_battle.distance <= 0xcccc;
}

static void spear_wait(Fighter *fighter) {
  native_await(fighter, spear_arrived, retail_spear_arrival);
}

/*487980: clear the native pair before choosing the loaded fall stream. */
void retail_pair_release(Fighter *fighter) {
  fighter->flags &= ~(unsigned)FF_PAIRED;
  fighter_peer(fighter)->flags &= ~(unsigned)FF_PAIRED;
  fighter->paired_release = NULL;
  fighter->capture_action = 0;
  if (fighter_airborne(fighter)) {
    retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_PAIR_RELEASE, mk4_mmove_thrown_finish);
  } else {
    retail_controller_return(fighter);
  }
}

static bool pair_owner_released(const Fighter *fighter) {
  const Fighter *owner = fighter_opponent(fighter);
  return fighter->capture_action == 0 || owner->action != fighter->capture_action || owner->pending != NULL || !(fighter->flags & FF_PAIRED);
}

static void pair_wait(Fighter *fighter) {
  fighter->capture_action = fighter_peer(fighter)->action;
  fighter->paired_release = retail_pair_release;
  fighter->flags |= FF_PAIRED;
  native_await(fighter, pair_owner_released, retail_pair_release);
}

/*488890 loads the descriptor anew and resumes from sample4 while captured. */
void retail_bicycle_cycle(Fighter *fighter) {
  if (pair_owner_released(fighter) || fighter->script_phase != 0) {
    retail_pair_release(fighter);
    return;
  }
  mk4_script_effect(fighter, 14);
  retail_vm_bind_animation(fighter, mk4_image_u32(retail_vm_image(), MK4_RETAIL_ANIMATION_DESCRIPTOR_BICYCLE_CYCLE) * 4, 4);
  native_finish_clip(fighter, retail_bicycle_cycle);
}

void retail_bicycle_capture(Fighter *fighter) {
  mk4_script_disarm_self(fighter);
  static const MoveDef move = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = ACTION_RETAIL_4003};
  fighter_start_move(fighter, S_KNOCKDOWN, &move);
  fighter->capture_action = ACTION_RETAIL_0206;
  fighter->paired_release = retail_pair_release;
  fighter->script_phase = 0;
  fighter->flags |= FF_PAIRED | FF_REACTING;
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_BICYCLE_CAPTURE, retail_pair_release);
}

static void torpedo_return_hold(Fighter *fighter) {
  native_hold(fighter, 64, retail_controller_return);
}

void retail_torpedo_return(Fighter *fighter) {
  native_finish_clip(fighter, torpedo_return_hold);
}

void retail_record_reaction(Fighter *fighter) {
  uint32_t entry = retail_reaction_target(retail_vm_image(), (unsigned)fighter->reaction);
  bool airborne = fighter_airborne(fighter);
  if (entry == RETAIL_NATIVE_BATTLE_REACT || entry == RETAIL_NATIVE_RECORD_REACTION_482450 || entry == RETAIL_NATIVE_RECORD_REACTION_482850 ||
      entry == RETAIL_NATIVE_BATTLE_SOUND_482600 || entry == RETAIL_NATIVE_EFFECT_4887C0 || entry == RETAIL_NATIVE_EFFECT_488810 ||
      entry == RETAIL_NATIVE_RECORD_REACTION_488830) {
    battle_wall_recoil(fighter);
  }
  if (entry != RETAIL_NATIVE_FACE_PEER_488990 && entry != RETAIL_NATIVE_RECORD_REACTION_482850) {
    mk4_script_disarm_self(fighter);
  }
  static const MoveDef move = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = ACTION_RETAIL_4003};
  fighter_start_move(fighter, S_KNOCKDOWN, &move);
  uint32_t script;
  FighterFn finish = mk4_script_thrown_ready;
  switch (entry) {
    case RETAIL_NATIVE_BATTLE_REACT:
      battle_react(fighter_peer(fighter), 161);
      script = MK4_RETAIL_SCRIPT_RAIDEN_STAFF_HP_STAFF_VICTIM;
      break;
    case RETAIL_NATIVE_RECORD_REACTION_482450:
      script = MK4_RETAIL_SCRIPT_RAIDEN_STAFF_HP_WEAPON_VICTIM;
      break;
    case RETAIL_NATIVE_RECORD_REACTION_486690: {
      uint32_t physics = mk4_image_u32(retail_vm_image(), MK4_RETAIL_DATA_RECORD_REACTION);
      require(retail_native_vm(fighter, physics, RETAIL_PHYSICS, 0), "Unknown native record callback");
      script = MK4_RETAIL_SCRIPT_RAIDEN_LIGHTNING_VICTIM;
      break;
    }
    case RETAIL_NATIVE_FACE_PEER_488990:
      retail_face_peer(fighter);
      script = MK4_RETAIL_SCRIPT_RAIDEN_TORPEDO_CONTACT;
      break;
    case RETAIL_NATIVE_BATTLE_SOUND_485960:
      battle_sound(SOUND_A, 0x2a);
      mk4_script_shake(fighter, 0);
      retail_bicycle_release(fighter);
      return;
    case RETAIL_NATIVE_RECORD_REACTION_482850:
      script = MK4_RETAIL_SCRIPT_SONYA_CROUCH_HK_VICTIM;
      break;
    case RETAIL_NATIVE_REACTION_POSITION_4820D0:
      reaction_position(fighter, 0x1b333, -0x1999);
      script = MK4_RETAIL_SCRIPT_SONYA_WEAPON_HP_VICTIM;
      break;
    case RETAIL_NATIVE_BATTLE_SOUND_482600:
      battle_sound(SOUND_C, 8);
      if (airborne) {
        fighter->body.gravity = 0x28f;
        fighter->body.vel[AXIS_Y] = 0x1999;
      }
      script = MK4_RETAIL_SCRIPT_SONYA_WEAPON_LP_VICTIM;
      break;
    case RETAIL_NATIVE_RECORD_REACTION_486750:
      script = MK4_RETAIL_SCRIPT_SONYA_RING_VICTIM;
      break;
    case RETAIL_NATIVE_RECORD_REACTION_488A20:
      script = MK4_RETAIL_SCRIPT_SONYA_BICYCLE_VICTIM;
      finish = retail_getup_front;
      break;
    case RETAIL_NATIVE_FACE_PEER_487750:
      retail_face_peer(fighter);
      retail_square_victim(fighter);
      return;
    case RETAIL_NATIVE_EFFECT_4887C0:
      mk4_script_effect(fighter, 9);
      fighter->damage_scale = RETAIL_DAMAGE_SCALE_40_PERCENT;
      script = MK4_RETAIL_SCRIPT_SONYA_CARTWHEEL_VICTIM;
      break;
    case RETAIL_NATIVE_EFFECT_488810:
      mk4_script_effect(fighter, 0);
      script = MK4_RETAIL_SCRIPT_SONYA_CARTWHEEL_VICTIM;
      break;
    case RETAIL_NATIVE_RECORD_REACTION_488830:
      script = MK4_RETAIL_SCRIPT_SONYA_CARTWHEEL_VICTIM;
      break;
    default:
      require(false, "Unexpected record reaction native");
      return;
  }
  retail_vm_enter(fighter, script, finish);
}

/* Additional original reaction-table entries use the same typed native
 * prelude/script boundary; none selects by fighter or copied reaction ID. */
void retail_aux_reaction(Fighter *fighter) {
  uint32_t entry = retail_reaction_native(fighter);
  if (entry == RETAIL_NATIVE_AUX_REACTION_482230) {
    battle_wall_recoil(fighter);
  }
  if (entry == RETAIL_NATIVE_AUX_REACTION_482230 || entry == RETAIL_NATIVE_AUX_REACTION_483580 || entry == RETAIL_NATIVE_AUX_REACTION_4835C0 ||
      entry == RETAIL_NATIVE_AUX_REACTION_485FB0 || entry == RETAIL_NATIVE_CAGE_GROIN_PUNCH_VICTIM_REACTION) {
    mk4_script_disarm_self(fighter);
  }
  static const MoveDef move = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = ACTION_RETAIL_4003};
  fighter_start_move(fighter, S_KNOCKDOWN, &move);
  uint32_t script;
  switch (entry) {
    case RETAIL_NATIVE_AUX_REACTION_482230:
      script = MK4_RETAIL_SCRIPT_AUX_REACTION_4E8C30;
      break;
    case RETAIL_NATIVE_AUX_REACTION_4829A0:
      retail_face_peer(fighter);
      fighter->body.angle[AXIS_Y] += 0x32492;
      script = MK4_RETAIL_SCRIPT_AUX_REACTION_4E91A0;
      break;
    case RETAIL_NATIVE_AUX_REACTION_483580:
      script = MK4_RETAIL_SCRIPT_GORO_KICK_VICTIM;
      break;
    case RETAIL_NATIVE_AUX_REACTION_4835C0:
      script = MK4_RETAIL_SCRIPT_AUX_REACTION_4E9690;
      break;
    case RETAIL_NATIVE_AUX_REACTION_4836D0:
      script = MK4_RETAIL_SCRIPT_AUX_REACTION_4EA2F8;
      break;
    case RETAIL_NATIVE_AUX_REACTION_485F00:
      script = MK4_RETAIL_SCRIPT_AUX_REACTION_4E9C58;
      break;
    case RETAIL_NATIVE_AUX_REACTION_485FB0:
      fighter->damage_scale = RETAIL_DAMAGE_SCALE_60_PERCENT;
      mk4_script_stop_body(fighter);
      fighter->body.pos[AXIS_Y] = fighter->body.ground;
      script = MK4_RETAIL_SCRIPT_SCORPION_SPEAR_VICTIM_HIT;
      break;
    case RETAIL_NATIVE_CAGE_GROIN_PUNCH_VICTIM_REACTION: {
      fighter->action = 0x1014;
      mk4_script_stop_body(fighter);
      fighter->body.pos[AXIS_Y] = fighter->body.ground;
      uint32_t physics = mk4_image_u32(retail_vm_image(), MK4_RETAIL_DATA_AUX_REACTION);
      require(retail_native_vm(fighter, physics, RETAIL_PHYSICS, 0), "Unknown native impact friction callback");
      script = MK4_RETAIL_SCRIPT_GROIN_PUNCH_GROIN_VICTIM;
      break;
    }
    default:
      require(false, "Unexpected auxiliary reaction native");
      return;
  }
  retail_vm_enter(fighter, script, mk4_script_thrown_ready);
}

static void stun_shift(Fighter *fighter) {
  fighter->visual_offset[AXIS_Y] = -0x147a;
}

static void stun_reset(Fighter *fighter) {
  fighter->visual_offset[AXIS_Y] = 0;
}

/*486000/4860d0: ten native3+3 shake cycles and15-visit settle.
 * Use presentation offset so render jitter does not alter combat geometry. */
void retail_shake_stun(Fighter *fighter) {
  int sample = fighter_sample(fighter);
  native_begin(fighter, 0, 74, retail_controller_return);
  fighter->mmove_cleanup = stun_reset;
  for (int frame = 0; frame < 75; ++frame) {
    FighterFn action = frame < 60 && frame % 6 == 0 ? stun_shift : frame < 60 && frame % 6 == 3 ? stun_reset : NULL;
    native_frame(fighter, frame, sample, action);
  }
  native_play(fighter, false);
}

void retail_guard_fall(Fighter *fighter) {
  bool airborne = fighter_airborne(fighter);
  retail_native_sound_list(fighter->armed ? RETAIL_SOUND_ARMED_IMPACT_LIST : RETAIL_SOUND_UNARMED_IMPACT_LIST);
  if (airborne) {
    mk4_script_disarm_self(fighter);
    battle_wall_recoil(fighter);
  }
  static const MoveDef move = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = ACTION_RETAIL_4003};
  fighter_start_move(fighter, S_KNOCKDOWN, &move);
  retail_vm_enter(fighter, airborne ? MK4_RETAIL_SCRIPT_GUARD_FALL_4E89A0 : MK4_RETAIL_SCRIPT_GUARD_FALL_4E9D20, mk4_script_thrown_ready);
}

FighterFn retail_reaction_entry(const Mk4Image *image, unsigned reaction) {
  switch (retail_reaction_target(image, reaction)) {
    case RETAIL_NATIVE_REACTION_FALL_RETURN:
      return reaction_fall_return;
    case RETAIL_NATIVE_SPEAR_WAIT:
      return spear_wait;
    case RETAIL_NATIVE_PAIR_WAIT:
      return pair_wait;
    case RETAIL_NATIVE_BICYCLE_CAPTURE:
      return retail_bicycle_capture;
    case RETAIL_NATIVE_SCENE_EXPLODE:
      return retail_scene_explode;
    case RETAIL_NATIVE_SCENE_PARK:
      return retail_scene_park;
    case RETAIL_NATIVE_GUARD_FALL:
      return retail_guard_fall;
    case RETAIL_NATIVE_IMPACT_REACTION_485EA0:
      return retail_impact_reaction;
    case RETAIL_NATIVE_EFFECT_488810:
    case RETAIL_NATIVE_RECORD_REACTION_488830:
      return retail_record_reaction;
    case RETAIL_NATIVE_AUX_REACTION_482230:
    case RETAIL_NATIVE_AUX_REACTION_4829A0:
    case RETAIL_NATIVE_AUX_REACTION_483580:
    case RETAIL_NATIVE_AUX_REACTION_4835C0:
    case RETAIL_NATIVE_AUX_REACTION_4836D0:
    case RETAIL_NATIVE_AUX_REACTION_485F00:
    case RETAIL_NATIVE_AUX_REACTION_485FB0:
    case RETAIL_NATIVE_CAGE_GROIN_PUNCH_VICTIM_REACTION:
      return retail_aux_reaction;
    case RETAIL_NATIVE_TASK_PAUSE_4314E0:
      return retail_task_pause;
    case RETAIL_NATIVE_BATTLE_REACT:
    case RETAIL_NATIVE_RECORD_REACTION_482450:
    case RETAIL_NATIVE_RECORD_REACTION_486690:
    case RETAIL_NATIVE_FACE_PEER_488990:
    case RETAIL_NATIVE_BATTLE_SOUND_485960:
    case RETAIL_NATIVE_RECORD_REACTION_482850:
    case RETAIL_NATIVE_REACTION_POSITION_4820D0:
    case RETAIL_NATIVE_BATTLE_SOUND_482600:
    case RETAIL_NATIVE_RECORD_REACTION_486750:
    case RETAIL_NATIVE_RECORD_REACTION_488A20:
    case RETAIL_NATIVE_FACE_PEER_487750:
    case RETAIL_NATIVE_EFFECT_4887C0:
      return retail_record_reaction;
    case RETAIL_NATIVE_BICYCLE_RELEASE:
      return retail_bicycle_release;
    case RETAIL_NATIVE_TORPEDO_RETURN:
      return retail_torpedo_return;
    case RETAIL_NATIVE_SNARE_REACTION:
      return retail_snare_reaction;
    case RETAIL_NATIVE_SCENE_VICTIM_HOLD:
      return retail_scene_victim_hold;
    case RETAIL_NATIVE_STAGE_VICTIM:
      return retail_stage_victim;
    case RETAIL_NATIVE_SCENE_REACTION_464E30:
    case RETAIL_NATIVE_TANYA_FATALITY_2_VICTIM_REACTION_186:
    case RETAIL_NATIVE_SCENE_REACTION_466110:
      return retail_scene_reaction;
    case RETAIL_NATIVE_REACTION_FALL:
      return retail_reaction_fall;
    case RETAIL_NATIVE_STAGGER_ENTRY_483C50:
    case RETAIL_NATIVE_REIKO_STAR_REACTION_45:
    case RETAIL_NATIVE_STAGGER_ENTRY_483D60:
      return retail_stagger_entry;
    case RETAIL_NATIVE_IMPACT_REACTION_481FC0:
    case RETAIL_NATIVE_IMPACT_REACTION_4838E0:
    case RETAIL_NATIVE_IMPACT_REACTION_485E30:
      return retail_impact_reaction;
    case RETAIL_NATIVE_REACTION_POSITION_482110:
    case RETAIL_NATIVE_REACTION_POSITION_482130:
    case RETAIL_NATIVE_REPTILE_WEAPON_REACTION_ENTRY:
    case RETAIL_NATIVE_JAREK_WEAPON_REACTION_ENTRY:
    case RETAIL_NATIVE_WEAPON_REACTION_482270:
    case RETAIL_NATIVE_CAGE_WEAPON_REACTION_ENTRY:
    case RETAIL_NATIVE_SHINNOK_WEAPON_REACTION_ENTRY:
    case RETAIL_NATIVE_WEAPON_REACTION_482380:
    case RETAIL_NATIVE_WEAPON_REACTION_4825E0:
    case RETAIL_NATIVE_WEAPON_REACTION_483050:
      return retail_weapon_reaction;
    case RETAIL_NATIVE_BRANCH_REACTION_482EF0:
    case RETAIL_NATIVE_BRANCH_REACTION_482F00:
    case RETAIL_NATIVE_BRANCH_REACTION_485F10:
    case RETAIL_NATIVE_BRANCH_REACTION_485F50:
    case RETAIL_NATIVE_BRANCH_REACTION_484BC0:
      return retail_branch_reaction;
    case RETAIL_NATIVE_GORO_SPECIAL_UPPERCUT_192_ENTRY:
    case RETAIL_NATIVE_SCALED_REACTION_4824A0:
    case RETAIL_NATIVE_SCALED_REACTION_4823C0:
    case RETAIL_NATIVE_SCALED_REACTION_483500:
    case RETAIL_NATIVE_SCALED_REACTION_482F60:
      return retail_scaled_reaction;
    case RETAIL_NATIVE_SCRIPT_REACTION_4831E0:
    case RETAIL_NATIVE_SCRIPT_REACTION_4866D0:
    case RETAIL_NATIVE_LIU_KANG_FIREBALL_REACTION_ENTRY:
    case RETAIL_NATIVE_SCRIPT_REACTION_482260:
    case RETAIL_NATIVE_SCRIPT_REACTION_482410:
    case RETAIL_NATIVE_GORO_SPECIAL_PUNCH_189_ENTRY:
    case RETAIL_NATIVE_SCRIPT_REACTION_482580:
    case RETAIL_NATIVE_SCRIPT_REACTION_482770:
      return retail_script_reaction;
    case RETAIL_NATIVE_TUMBLE_REACTION_483450:
    case RETAIL_NATIVE_TUMBLE_REACTION_483490:
    case RETAIL_NATIVE_TUMBLE_REACTION_482750:
    case RETAIL_NATIVE_TUMBLE_REACTION_482730:
    case RETAIL_NATIVE_TUMBLE_REACTION_4826A0:
      return retail_tumble_reaction;
    case RETAIL_NATIVE_LAUNCH_REACTION_481320:
    case RETAIL_NATIVE_LAUNCH_REACTION_481350:
    case RETAIL_NATIVE_LAUNCH_REACTION_482560:
    case RETAIL_NATIVE_LAUNCH_REACTION_4825C0:
    case RETAIL_NATIVE_LAUNCH_REACTION_4835E0:
      return retail_launch_reaction;
    case RETAIL_NATIVE_STAGGER_REACTION_482E00:
    case RETAIL_NATIVE_STAGGER_REACTION_482830:
    case RETAIL_NATIVE_STAGGER_REACTION_482870:
    case RETAIL_NATIVE_STAGGER_REACTION_4828D0:
    case RETAIL_NATIVE_STAGGER_REACTION_482BE0:
    case RETAIL_NATIVE_STAGGER_REACTION_482E80:
    case RETAIL_NATIVE_STAGGER_REACTION_482E90:
    case RETAIL_NATIVE_STAGGER_REACTION_482B10:
      return retail_stagger_reaction;
    default: {
      FighterFn entry = retail_throw_reaction(image, reaction);
      return entry != NULL ? entry : retail_engine_reaction(retail_reaction_target(image, reaction));
    }
  }
}
