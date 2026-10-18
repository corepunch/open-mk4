static void spear_stun_script(Fighter *fighter) {
  native_hold(fighter, 64, special_ready);
}

static bool spear_not_pulling(const Fighter *fighter) {
  for (int i = 0; i < MAX_PROJECTILES; ++i) {
    const Projectile *p = &g_battle.projectiles[i];
    if (p->kind == PROJECTILE_SPEAR && &g_battle.fighters[p->owner] == fighter && p->phase == 1) {
      return false;
    }
  }
  return true;
}

/* Runs on the scheduler stack, after the old fiber has been retired. Pull
 * arrival installs stun before replacing the owner's move, so that victim
 * is already released from s_snared and must retain its follow-up opening. */
static void spear_cleanup(Fighter *fighter) {
  for (int i = 0; i < MAX_PROJECTILES; ++i) {
    Projectile *p = &g_battle.projectiles[i];
    if (p->kind != PROJECTILE_SPEAR || &g_battle.fighters[p->owner] != fighter) {
      continue;
    }
    bool pulling = p->phase == 1;
    p->kind = 0;
    Fighter *victim = opponent(fighter);
    if (pulling && fighter_move(victim)->clip == 199) {
      special_ready(victim);
    }
  }
}

static void spear_projectile(Projectile *p) {
  Fighter *owner = &g_battle.fighters[p->owner], *victim = opponent(owner);
  bool interrupted = fighter_move(owner)->clip != 14 || (!fighter_fiber_active(owner) && owner->current_mmove == NULL) || owner->pending != NULL;
  if (interrupted || g_battle.contacts_off) {
    if (p->phase == 1 && fighter_move(victim)->clip == 199) {
      special_ready(victim);
    }
    p->kind = 0;
    return;
  }
  if (p->phase == 1) {
    if (fighter_move(victim)->clip != 199 || victim->health <= 0) {
      p->kind = 0;
      return;
    }
    q16 dx = owner->body.pos[AXIS_X] - victim->body.pos[AXIS_X];
    if (abs(dx) <= 0xcccc) {
      stop_body(victim);
      fighter_start_move(victim, S_HIT, &s_spear_stun);
      victim->sample_override = 0;
      victim->flags |= FF_REACTING;
      fighter_start_fiber(victim, retail_script_004e9f20, NULL);
      fighter_start_move(owner, S_PUNCH, &s_spear);
      owner->sample_override = 31;
      fighter_start_fiber(owner, retail_script_004ed4a8, spear_cleanup);
      p->kind = 0;
    } else {
      victim->body.pos[AXIS_X] += dx > 0 ? 0x3333 : -0x3333;
      victim->body.pos[AXIS_Y] = victim->body.ground;
      memcpy(p->pos, victim->body.pos, sizeof p->pos);
    }
    return;
  }
  if (p->phase == 2) {
    q16 dx = owner->body.pos[AXIS_X] - p->pos[AXIS_X];
    if (abs(dx) <= 0x3333) {
      p->kind = 0;
    } else {
      p->pos[AXIS_X] += dx > 0 ? 0x3333 : -0x3333;
    }
    return;
  }
  for (int k = 0; k < 3; ++k) {
    p->pos[k] += p->vel[k];
  }
  ContactResult result = battle_contact_from(owner, &spear_record, p->pos, p->facing);
  if (result == CONTACT_HIT || result == CONTACT_AIR_HIT) {
    stop_body(victim);
    fighter_start_move(victim, S_HIT, &s_snared);
    victim->sample_override = 0;
    victim->flags |= FF_REACTING;
    fighter_start_fiber(victim, retail_script_004e9d00, NULL);
    victim->damage_scale = 0x9999; /* 0x485fb0: subsequent combo damage scales to 60%. */
    p->phase = 1;
    p->lifetime = 100;
  } else if (result == CONTACT_BLOCK || p->ticks >= 30 || abs(p->pos[AXIS_X]) > g_battle.arena_radius) {
    p->phase = 2;
    p->lifetime = 45;
  }
}

static void spear_spawn(Fighter *fighter) {
  Projectile *p = battle_spawn_projectile(fighter, PROJECTILE_SPEAR, 90, spear_projectile);
  if (p != NULL) {
    p->pos[AXIS_X] += fighter->facing * 0xc000;
    p->pos[AXIS_Y] = -0x16666;
    p->vel[AXIS_X] = fighter->facing * 0x3333;
  }
}

/* 4ed448/4ed47c -> 4ed470/4ed498. Normal/fast launch is at tick 10/8;
 * sample 30 follows at 15/11. Keep the existing paired-pull policy: arrival
 * replaces this fiber with spear_return_script, while cancellation cleans up. */

static bool can_spear(const Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  return g_battle.phase == PHASE_FIGHT && g_battle.ticks >= fighter->spear_ready && victim->hits <= 3 && victim->action != ACTION_RETAIL_1008;
}

static void scorpion_spear(Fighter *fighter) {
  bool followup = (fighter->flags & FF_FOLLOW_UP) != 0;
  stop_body(fighter);
  fighter_start_move(fighter, S_PUNCH, &s_spear);
  fighter->script_phase = followup;
  fighter->sample_override = followup ? 9 : 0;
  fighter->spear_ready = g_battle.ticks + 90;
  fighter_start_fiber(fighter, retail_script_004ed448, spear_cleanup);
}

static void spear_wait_native(Fighter *fighter) {
  native_until(fighter, spear_not_pulling);
  fighter_fiber_finish(special_ready);
}

static void script_spear_done(Fighter *fighter) {
  if (spear_not_pulling(fighter)) {
    special_ready(fighter);
  } else {
    fighter_start_fiber(fighter, spear_wait_native, spear_cleanup);
  }
}

static void script_spear_return(Fighter *fighter) {
  fighter->spear_ready = g_battle.ticks + 90;
  fighter_fiber_finish(special_ready);
}

static void script_spawn_spear(Fighter *fighter, int type) {
  require(type == 67, "Unexpected spear task");
  spear_spawn(fighter);
}

static void script_spear_pull(Fighter *fighter) {
  fighter->damage_scale = 0x9999;
  native_begin(fighter, 0, 0, NULL);
  fighter->native.frames[0].thinkfunc = mk4_mmove_hold;
  native_play(fighter, false);
}
