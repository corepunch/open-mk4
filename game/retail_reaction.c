#include "native_services.h"
#include "retail_data.h"

/* Native482aa0: the impact voice depends on the captured peer, independently
 * of the selected script/model aliases. Script words supply subsequent sound. */
static void stagger_voice(Fighter *fighter) {
  if (fighter_peer(fighter)->roster == 13) {
    retail_native_sound_list(0x4ea8c8);
    retail_native_sound_list(0x4ea870);
  } else {
    mk4_script_indexed_sound(fighter, 1);
  }
}

void retail_stagger_reaction(Fighter *fighter) {
  uint32_t entry = retail_reaction_target(retail_vm_image(), (unsigned)fighter->reaction);
  int previous_action = fighter->action;
  bool crouch = previous_action == 0x2001 || previous_action == 0x2002;
  if (entry == 0x482e00) {
    crouch |= previous_action == 0x2010 || previous_action == 0x106 || previous_action == 0x107;
  }
  if (entry == 0x482e00 || entry == 0x482830 || entry == 0x482870 || entry == 0x4828d0) {
    battle_wall_recoil(fighter);
  }
  static const MoveDef move = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = ACTION_RETAIL_4001};
  fighter_start_move(fighter, S_HIT, &move);
  uint32_t script;
  switch (entry) {
    case 0x482e00:
      mk4_script_disarm_self(fighter);
      if (crouch) {
        uint32_t physics = mk4_image_u32(retail_vm_image(), 0x4e9358);
        require(retail_native_vm(fighter, physics, RETAIL_PHYSICS, 0), "Unknown crouch reaction motion callback");
      }
      script = crouch ? 0x4e9360 : 0x4e9308;
      break;
    case 0x482830:
      script = 0x4e90e0;
      break;
    case 0x482870:
    case 0x4828d0:
      mk4_script_disarm_self(fighter);
      retail_native_sound_list(0x4ea938);
      script = crouch ? 0x4e9170 : entry == 0x482870 ? 0x4e9130 : 0x4e9150;
      break;
    case 0x482be0:
      fighter->damage_scale = 0x9999;
      script = 0x4e9260;
      break;
    case 0x482e80:
    case 0x482e90:
      fighter->damage_scale = 0x8000;
      mk4_script_disarm_self(fighter);
      stagger_voice(fighter);
      fighter->action = ACTION_RETAIL_4003;
      script = 0x4e9388;
      break;
    case 0x482b10:
      fighter->action = 0x4002;
      stagger_voice(fighter);
      fighter->damage_scale = 0x9999;
      script = 0x4e9238;
      break;
    default:
      require(false, "Unexpected shared stagger native");
      return;
  }
  retail_vm_enter(fighter, script, mk4_script_thrown_ready);
}

void retail_launch_reaction(Fighter *fighter) {
  uint32_t entry = retail_reaction_target(retail_vm_image(), (unsigned)fighter->reaction);
  if (entry != 0x4835e0 && entry != 0x481350) {
    mk4_script_disarm_self(fighter);
  }
  if (entry == 0x481320) {
    battle_wall_recoil(fighter);
  }
  static const MoveDef move = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = ACTION_RETAIL_4003};
  fighter_start_move(fighter, S_KNOCKDOWN, &move);
  uint32_t script;
  switch (entry) {
    case 0x481320:
    case 0x481350:
      script = 0x4e89a0;
      break;
    case 0x482560:
      script = 0x4e8ea8;
      break;
    case 0x4825c0:
      script = 0x4e8f10;
      break;
    case 0x4835e0:
      script = 0x4e96c8;
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
  mk4_mmove_finish_getup(fighter, 0x4ea210);
}

void retail_reaction_finish(Fighter *fighter) {
  native_finish_clip(fighter, mk4_mmove_thrown_finish);
}

void retail_reaction_fall(Fighter *fighter) {
  mk4_script_disarm_self(fighter);
  retail_vm_enter(fighter, 0x4e96e0, mk4_script_thrown_ready);
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
    retail_native_sound_list(0x4ea880);
  }
}

void retail_tumble_reaction(Fighter *fighter) {
  uint32_t entry = retail_reaction_target(retail_vm_image(), (unsigned)fighter->reaction);
  if (entry == 0x482730) {
    mk4_script_stop_body(fighter);
    fighter->body.pos[AXIS_Y] = fighter->body.ground;
  } else {
    mk4_script_disarm_self(fighter);
  }
  if (entry == 0x483450 || entry == 0x483490) {
    battle_wall_recoil(fighter);
  }
  if (entry == 0x4826a0) {
    weapon_impact_voice(fighter);
  }
  static const MoveDef move = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = ACTION_RETAIL_4003};
  fighter_start_move(fighter, S_KNOCKDOWN, &move);
  uint32_t script;
  FighterFn finish = mk4_script_thrown_ready;
  switch (entry) {
    case 0x483450:
      script = 0x4e9508;
      finish = retail_getup_back;
      break;
    case 0x483490:
      script = 0x4e9568;
      finish = retail_getup_back;
      break;
    case 0x482750:
      script = 0x4e9048;
      break;
    case 0x482730:
      script = 0x4e8fe8;
      break;
    case 0x4826a0:
      script = 0x4e8f80;
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
  if (entry == 0x482420) {
    battle_wall_recoil(fighter);
  }
  if (entry == 0x482420 || entry == 0x482580 || entry == 0x482770 || entry == 0x4831e0 || entry == 0x4866d0 || entry == 0x486730) {
    mk4_script_disarm_self(fighter);
  }
  if (entry == 0x4831e0) {
    retail_native_sound_list(0x4ea8f8);
    mk4_script_stop_body(fighter);
    fighter->body.pos[AXIS_Y] = fighter->body.ground;
  } else if (entry == 0x486730) {
    battle_sound(SOUND_C, 0x5c);
  }
  static const MoveDef move = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = ACTION_RETAIL_4003};
  fighter_start_move(fighter, S_HIT, &move);
  uint32_t script;
  switch (entry) {
    case 0x482260:
      script = 0x4e8c78;
      break;
    case 0x482410:
      script = 0x4e8da8;
      break;
    case 0x482420:
      script = 0x4e8dd0;
      break;
    case 0x482580: {
      uint32_t physics = mk4_image_u32(retail_vm_image(), 0x4e8edc);
      require(retail_native_vm(fighter, physics, RETAIL_PHYSICS, 0), "Unknown native reaction motion callback");
      script = 0x4e8ee0;
      break;
    }
    case 0x482770:
      script = 0x4e90b0;
      break;
    case 0x4831e0:
      script = 0x4e94b0;
      break;
    case 0x4866d0: {
      uint32_t physics = mk4_image_u32(retail_vm_image(), 0x4e9d80);
      require(retail_native_vm(fighter, physics, RETAIL_PHYSICS, 0), "Unknown native recoil motion callback");
      script = 0x4e9d88;
      break;
    }
    case 0x486730:
      script = 0x4e9de8;
      break;
    default:
      require(false, "Unexpected script reaction native");
      return;
  }
  retail_vm_enter(fighter, script, mk4_script_thrown_ready);
}

void retail_scaled_reaction(Fighter *fighter) {
  uint32_t entry = retail_reaction_target(retail_vm_image(), (unsigned)fighter->reaction);
  if (entry == 0x482480) {
    mk4_script_shake(fighter, 6);
  }
  if (entry == 0x482480 || entry == 0x4824a0 || entry == 0x4823c0) {
    battle_wall_recoil(fighter);
  }
  mk4_script_disarm_self(fighter);
  static const MoveDef move = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = ACTION_RETAIL_4003};
  fighter_start_move(fighter, S_KNOCKDOWN, &move);
  uint32_t script;
  FighterFn finish = mk4_script_thrown_ready;
  switch (entry) {
    case 0x482480:
    case 0x4824a0: {
      fighter->damage_scale = 0x6666;
      Fighter *attacker = fighter_peer(fighter);
      attacker->run_energy = 0;
      attacker->run_delay = 60;
      script = 0x4e8e50;
      break;
    }
    case 0x4823c0:
      fighter->damage_scale = 0x8000;
      script = 0x4e8d70;
      finish = retail_getup_roll;
      break;
    case 0x483500:
      fighter->damage_scale = 0x9999;
      retail_native_sound_list(0x4ea8f8);
      script = 0x4e95f0;
      break;
    case 0x482f60:
      weapon_impact_voice(fighter);
      script = 0x4e93d8;
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
  if (entry == 0x482ef0 || entry == 0x482f00) {
    mk4_script_context_sound(fighter, 16);
  }
  mk4_script_disarm_self(fighter);
  static const MoveDef move = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = ACTION_RETAIL_4003};
  fighter_start_move(fighter, S_KNOCKDOWN, &move);
  uint32_t script;
  FighterFn finish = mk4_script_thrown_ready;
  switch (entry) {
    case 0x482ef0:
    case 0x482f00:
      script = airborne ? 0x4e9430 : 0x4e93a8;
      finish = retail_getup_front;
      break;
    case 0x485f10:
      script = airborne ? 0x4e9430 : 0x4e9c80;
      if (airborne) {
        finish = retail_getup_front;
      }
      break;
    case 0x485f50:
      fighter->action = 0x100a;
      script = 0x4e9ca8;
      break;
    case 0x484bc0:
      script = airborne ? 0x4e9980 : 0x4e9938;
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
  if (entry == 0x482110 || entry == 0x4825e0) {
    mk4_script_context_sound(fighter, 6);
  } else if (entry == 0x482130) {
    battle_sound(SOUND_C, 8);
  }
  if (entry != 0x482270) {
    mk4_script_disarm_self(fighter);
  }
  if (entry == 0x482110 || entry == 0x482130 || entry == 0x482300 || entry == 0x482350 || entry == 0x482380 || entry == 0x4825e0) {
    battle_wall_recoil(fighter);
  }
  static const MoveDef move = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = ACTION_RETAIL_4003};
  fighter_start_move(fighter, S_KNOCKDOWN, &move);
  uint32_t script;
  FighterFn finish = mk4_script_thrown_ready;
  switch (entry) {
    case 0x482110:
    case 0x482130:
      reaction_position(fighter, 0x1cccc, -0x8000);
      script = 0x4e8b78;
      break;
    case 0x4821c0:
      script = 0x4e8bc0;
      break;
    case 0x4821e0:
      reaction_position(fighter, 0x1b333, 0);
      mk4_script_stop_body(fighter);
      fighter->body.pos[AXIS_Y] = fighter->body.ground;
      script = 0x4e8c08;
      break;
    case 0x482270:
      script = 0x4e8c98;
      break;
    case 0x482300:
      fighter->damage_scale = 0x8000;
      script = 0x4e8cc0;
      break;
    case 0x482350:
      script = 0x4e8d08;
      break;
    case 0x482380:
      script = 0x4e8d38;
      finish = retail_getup_roll;
      break;
    case 0x4825e0:
      if (airborne) {
        fighter->body.gravity = 0x28f;
        fighter->body.vel[AXIS_Y] = 0x1999;
      }
      script = 0x4e8f50;
      break;
    case 0x483050:
      retail_native_sound_list(0x4ea8f8);
      mk4_script_stop_body(fighter);
      fighter->body.pos[AXIS_Y] = fighter->body.ground;
      script = 0x4e9460;
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
  retail_native_sound_list(0x4ea960);
  return false;
}

void retail_impact_reaction(Fighter *fighter) {
  uint32_t entry = retail_reaction_target(retail_vm_image(), (unsigned)fighter->reaction);
  bool stagger = false;
  if (entry == 0x4838e0) {
    /*426ad0/426af0 is an independent30-visit delayed audio task. */
    BattleEvent *voice = battle_event_start(fighter, uppercut_voice, false, false);
    require(voice != NULL, "Uppercut needs its native audio task");
    battle_event_delay(voice, 30);
    fighter->damage_scale = 0x6666;
    battle_wall_recoil(fighter);
    mk4_script_disarm_self(fighter);
  } else if (entry == 0x481fc0) {
    fighter->damage_scale = 0x9999;
    int64_t x = fighter->body.pos[AXIS_X], z = fighter->body.pos[AXIS_Z], radius = g_battle.arena_radius;
    if (fighter->hits > 2 && x * x + z * z >= radius * radius - 10LL * Q16_ONE * Q16_ONE) {
      battle_react(fighter_peer(fighter), 14);
    }
    mk4_script_disarm_self(fighter);
  } else {
    require(entry == 0x485e30, "Unexpected impact reaction native");
    fighter->hits = maxi(0, fighter->hits - 1);
    stagger = fighter->damage_total >= 0x3333 || fighter->roster == 15;
    if (stagger || fighter->hits >= 3) {
      battle_wall_recoil(fighter);
    }
  }
  static const MoveDef move = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = ACTION_RETAIL_4003};
  fighter_start_move(fighter, S_KNOCKDOWN, &move);
  uint32_t script = entry == 0x481fc0 ? 0x4e8ab8 : entry == 0x4838e0 ? 0x4e9758 : stagger ? 0x4e97d0 : 0x4e9c28;
  retail_vm_enter(fighter, script, mk4_script_thrown_ready);
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
  if (entry == 0x483c50) {
    BattleEvent *particles = battle_event_start(fighter, hit_particles, false, false);
    require(particles != NULL, "Hit reaction needs its native particle task");
    particles->data[0] = 16;
  }
  if (!airborne || entry != 0x483c50) {
    mk4_script_disarm_self(fighter);
  }
  if (airborne && entry != 0x483c50) {
    battle_wall_recoil(fighter);
  }
  static const MoveDef move = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD | SF_REACTION};
  fighter_start_move(fighter, airborne ? S_KNOCKDOWN : S_HIT, &move);
  fighter->action = entry == 0x483d40 ? 0x2013 : entry == 0x483d60 ? 0x1013 : 0;
  if (airborne) {
    if (entry == 0x483c50) {
      mk4_script_indexed_sound(fighter, 1);
      retail_vm_enter(fighter, 0x4e8a78, retail_controller_return);
    } else {
      retail_vm_enter(fighter, 0x4e89a0, mk4_script_thrown_ready);
    }
    return;
  }
  mk4_script_stop_body(fighter);
  fighter->body.pos[AXIS_Y] = fighter->body.ground;
  uint32_t physics = mk4_image_u32(retail_vm_image(), 0x4e97f0);
  require(retail_native_vm(fighter, physics, RETAIL_PHYSICS, 0), "Unknown native stagger callback");
  retail_vm_enter(fighter, 0x4e97f8, mk4_script_thrown_ready);
}

FighterFn retail_reaction_entry(const Mk4Image *image, unsigned reaction) {
  switch (retail_reaction_target(image, reaction)) {
    case 0x4836e0:
      return retail_stage_victim;
    case 0x464e30:
    case 0x465200:
    case 0x466110:
      return retail_scene_reaction;
    case 0x4835f0:
      return retail_reaction_fall;
    case 0x483c50:
    case 0x483d40:
    case 0x483d60:
      return retail_stagger_entry;
    case 0x481fc0:
    case 0x4838e0:
    case 0x485e30:
      return retail_impact_reaction;
    case 0x482110:
    case 0x482130:
    case 0x4821c0:
    case 0x4821e0:
    case 0x482270:
    case 0x482300:
    case 0x482350:
    case 0x482380:
    case 0x4825e0:
    case 0x483050:
      return retail_weapon_reaction;
    case 0x482ef0:
    case 0x482f00:
    case 0x485f10:
    case 0x485f50:
    case 0x484bc0:
      return retail_branch_reaction;
    case 0x482480:
    case 0x4824a0:
    case 0x4823c0:
    case 0x483500:
    case 0x482f60:
      return retail_scaled_reaction;
    case 0x4831e0:
    case 0x4866d0:
    case 0x486730:
    case 0x482260:
    case 0x482410:
    case 0x482420:
    case 0x482580:
    case 0x482770:
      return retail_script_reaction;
    case 0x483450:
    case 0x483490:
    case 0x482750:
    case 0x482730:
    case 0x4826a0:
      return retail_tumble_reaction;
    case 0x481320:
    case 0x481350:
    case 0x482560:
    case 0x4825c0:
    case 0x4835e0:
      return retail_launch_reaction;
    case 0x482e00:
    case 0x482830:
    case 0x482870:
    case 0x4828d0:
    case 0x482be0:
    case 0x482e80:
    case 0x482e90:
    case 0x482b10:
      return retail_stagger_reaction;
    default:
      return retail_throw_reaction(image, reaction);
  }
}
