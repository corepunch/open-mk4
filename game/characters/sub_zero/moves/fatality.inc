static void fatal_pose(Fighter *fighter, int clip, int sample, bool owner) {
  if (fighter_move(fighter)->clip != clip) {
    for (int i = 0; i < COUNT(fatal_moves); ++i) {
      if (fatal_moves[i].clip == clip) {
        fighter_start_move(fighter, owner ? S_VICTORY : S_KNOCKDOWN, &fatal_moves[i]);
        break;
      }
    }
  }
  fighter->sample_override = sample;
  fighter->flags |= FF_UNTOUCHABLE | FF_BUSY;
}

static bool can_close_fatality(const Fighter *fighter) {
  return battle_can_finish(fighter) && fighter_distance(fighter) <= 1.f;
}

static void finish_scene(Fighter *fighter) {
  (void)fighter;
  battle_end_finisher();
}

static void scene_blend_cleanup(Fighter *fighter) {
  fighter->scene.clips = NULL;
  fighter->blend_duration = 0;
  fighter->blend_weight = -1;
}

static void victory_finish(Fighter *fighter) {
  (void)fighter;
  fighter_cancel_all_mmoves();
  battle_end_finisher();
}

static void victory_watch(Fighter *fighter) {
  if (fighter_move(fighter)->clip == 118) {
    native_motion_begin(fighter, 50, victory_finish);
    native_motion_play(fighter);
    native_motion_step(fighter);
  }
}

static void script_victory(Fighter *fighter) {
  fighter->body.physics = victory_watch;
  retail_script_004e6a40(fighter);
}

static void start_fatality(Fighter *fighter, int id) {
  if (!battle_begin_finisher(fighter, id)) {
    sub_zero_ready(fighter);
    return;
  }
  Fighter *victim = opponent(fighter);
  fatal_pose(victim, 13, 0, false);
  fatal_pose(fighter, id == FINISH_SUB_ZERO_1 ? 385 : id == FINISH_SUB_ZERO_2 ? 552 : id == FINISH_FAN ? 415 : 293, 0, true);
  if (id == FINISH_SUB_ZERO_1) {
    /* 445100 -> 475530 aligns the scene pair at 1.45 units. */
    for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
      victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0x17333);
    }
  }
  if (id == FINISH_FAN) {
    fatal_pose(victim, 416, 0, false);
  }
  FighterFn start = id == FINISH_SUB_ZERO_1   ? first_fatality_script
                    : id == FINISH_SUB_ZERO_2 ? second_fatality_script
                    : id == FINISH_FAN        ? fan_start
                                              : spike_start;
  start(fighter);
}
