static void fatal_pose(Fighter *fighter, int clip, int sample, bool owner) {
  if (fighter_move(fighter)->clip != clip) {
    for (int i = 0; i < COUNT(fatal_moves); ++i) {
      if (fatal_moves[i].clip == clip) {
        if (owner && fighter_fiber_running(fighter)) {
          fighter_fiber_move(fighter, S_VICTORY, &fatal_moves[i]);
        } else {
          fighter_start_move(fighter, owner ? S_VICTORY : S_KNOCKDOWN, &fatal_moves[i]);
        }
        break;
      }
    }
  }
  fighter->sample_override = sample;
  fighter->flags |= FF_UNTOUCHABLE | FF_BUSY;
}

static bool can_burn_fatality(const Fighter *fighter) {
  float distance = fighter_distance(fighter);
  return battle_can_finish(fighter) && distance >= q16_float(0x1cccc) && distance <= 3.f;
}

static bool can_beast_fatality(const Fighter *fighter) {
  return battle_can_finish(fighter) && fighter_distance(fighter) <= 1.f;
}

static void finish_scene(Fighter *fighter) {
  (void)fighter;
  battle_end_finisher();
}

static void burn_start(Fighter *fighter) {
  native_delay(fighter, maxi(0, 8 - g_battle.phase_ticks));
  retail_script_004e0278(fighter);
}

static void beast_start(Fighter *fighter) {
  retail_script_004d2e58(fighter);
}

static void start_fatality(Fighter *fighter, int id) {
  if (!battle_begin_finisher(fighter, id)) {
    scorpion_ready(fighter);
    return;
  }
  Fighter *victim = opponent(fighter);
  fatal_pose(victim, 13, 0, false);
  fatal_pose(fighter, id == FINISH_BEAST ? 12 : id == FINISH_FAN ? 415 : id == FINISH_SPIKE ? 293 : 390, 0, true);
  if (id == FINISH_FAN) {
    fatal_pose(victim, 416, 0, false);
  }
  fighter_start_fiber(fighter, id == FINISH_BURN ? burn_start : id == FINISH_BEAST ? beast_start : id == FINISH_FAN ? fan_start : spike_start, NULL);
}

static void scorpion_burn_fatality(Fighter *fighter) {
  start_fatality(fighter, FINISH_BURN);
}

static void scorpion_beast_fatality(Fighter *fighter) {
  start_fatality(fighter, FINISH_BEAST);
}

static void script_burn_tail(Fighter *victim) {
  victim->body.physics = fighter_friction;
  mk4_script_finish_clip(victim, 1);
  fighter_fiber_finish(mk4_script_thrown_ready);
}

static void script_burn_begin(Fighter *fighter) {
  battle_sound(SOUND_B, 14);
  battle_sound(SOUND_B, 46);
  retail_script_004e0288(fighter);
}

static void script_burn_victim_start(Fighter *fighter) {
  Fighter *victim = opponent(fighter);
  victim->appearance = APPEARANCE_BURNED;
  fighter_start_fiber(victim, retail_script_004e0300, NULL);
  battle_sound(SOUND_B, 15);
  retail_script_004e0298(fighter);
}

static void script_burn_repeat1(Fighter *fighter) {
  native_blend(fighter, burn_repeat_blend);
  retail_script_004e02a8(fighter);
}

static void script_burn_repeat2(Fighter *fighter) {
  native_blend(fighter, burn_repeat_blend);
  retail_script_004e02b8(fighter);
}

static void script_burn_victory(Fighter *fighter) {
  native_blend(fighter, burn_victory_blend);
  retail_script_004e02d0(fighter);
}

static void script_burn_victim_cue(Fighter *victim) {
  battle_sound(SOUND_B, 16);
  retail_script_004e0310(victim);
}

static void script_burn_victim_repeat(Fighter *victim) {
  retail_script_004e0320(victim);
}

static void script_burn_victim_blend(Fighter *victim) {
  native_blend(victim, burn_victim_blend);
  retail_script_004e0338(victim);
}

static void script_burn_victim_impact(Fighter *victim) {
  mk4_script_shake(victim, 0);
  battle_particles(victim, 0, 4, true);
  retail_script_004e0348(victim);
}

static void script_burn_victim_settle(Fighter *victim) {
  mk4_script_shake(victim, 0);
  retail_script_004e0358(victim);
}

static void script_burn_victim_end(Fighter *victim) {
  fighter_fiber_finish(finish_scene);
  (void)victim;
}

static void beast_watch(Fighter *fighter) {
  if (fighter->script_phase == 9 && opponent(fighter)->script_phase == 9) {
    fighter_cancel_all_fibers();
    battle_end_finisher();
  }
}

static void script_beast_begin(Fighter *fighter) {
  fighter->appearance = APPEARANCE_BEAST;
  fighter->body.physics = beast_watch;
  retail_script_004d2e88(fighter);
}

static void script_beast_attack(Fighter *fighter) {
  fighter_start_fiber(opponent(fighter), retail_script_004d2ef8, NULL);
  retail_script_004d2eb0(fighter);
}

static void script_beast_victim_pieces(Fighter *victim) {
  battle_particles(victim, 0, 12, true);
  retail_script_004d2f10(victim);
}

static void script_beast_voice(Fighter *fighter) {
  battle_sound(SOUND_IMPACT_VOICE, roster_def(fighter->roster)->asset);
}

static void script_beast_park(Fighter *fighter) {
  fighter->script_phase = 9;
  native_begin(fighter, 0, 0, NULL);
  fighter->native.frames[0].thinkfunc = mk4_mmove_hold;
  native_play(fighter, false);
}

static void beast_gore_object(Projectile *object) {
  Fighter *victim = &g_battle.fighters[object->owner];
  if (g_battle.phase != PHASE_FATALITY) {
    object->kind = 0;
  } else if (object->ticks % 4 == 0) {
    battle_particles(victim, 0, 2, true);
  }
}

static void script_beast_gore(Fighter *victim, int type) {
  require(type == 0, "Unexpected beast gore task");
  battle_spawn_projectile(victim, PROJECTILE_EFFECT, 60, beast_gore_object);
}
