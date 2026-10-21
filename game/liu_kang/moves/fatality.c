#include "../character.h"

static void dragon_stream(Fighter *fighter);
static void dragon_spawn(Fighter *fighter);
static void dragon_actor_start(Fighter *fighter);
static void dragon_victim_start(Fighter *fighter);
static void dragon_tear(Fighter *fighter);
static void dragon_bite_sound(Fighter *fighter);
static void dragon_delayed_sound(Fighter *fighter);
static void dragon_detach(Fighter *fighter);
static void dragon_fatality_script(Fighter *fighter);
static void liu_script_fatal_watch(Fighter *fighter);
static void liu_script_fatal_hit(Fighter *owner);
static void fatal_fireball_object(Projectile *p);
// clang-format off: Keep table entries on one row, including long rows.
static const SceneClip dragon_actor[] = {
  SCENE(418, 0, 159),
  SCENE(418, 160, 518),
  SCENE(418, 446, 518),
};
// clang-format on
// clang-format off: Keep table entries on one row, including long rows.
static const SceneClip dragon_target[] = {
  {{.clip = 389, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0700}, 0, 0, 1, 0x32f1, 0x3333},
  {{.clip = 419, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0700}, 0, 0, 1, 0x32f1, 0x3333},
  SCENE(419, 0, 288),
};
// clang-format on

void liu_kang_finish_scene(Fighter *fighter) {
  (void)fighter;
  battle_end_finisher();
}

static void dragon_stream(Fighter *fighter) {
  Fighter *victim = fighter_peer(fighter);
  if (fighter->scene.clips != NULL) {
    scene_step(fighter);
  }
  if (victim->scene.clips != NULL) {
    scene_step(victim);
  }
}

static void dragon_spawn(Fighter *fighter) {
  fighter->appearance = APPEARANCE_DRAGON;
  fighter->body.pos[AXIS_Y] = -0x12c49;
  mk4_mmove_bind(fighter, 418, 519, 0);
  fighter->flags |= FF_UNTOUCHABLE | FF_BUSY;
  battle_sound(SOUND_B, 36);
  float x = q16_float(fighter->heading[0]), z = q16_float(fighter->heading[1]);
  Vec3 center = {fighter_x(fighter) + x, -2.7f, fighter_z(fighter) + z};
  g_battle.camera = (SceneCamera){.active = true, .focal_scale = 0x9999, .eye = {center.x - z * 7.5f, -2.9f, center.z + x * 7.5f}, .target = center};
}

static void dragon_actor_start(Fighter *fighter) {
  battle_sound(SOUND_B, 46);
  scene_begin(fighter, dragon_actor, COUNT(dragon_actor), 2);
}

static void dragon_victim_start(Fighter *fighter) {
  scene_step(fighter);
  scene_begin(fighter_peer(fighter), dragon_target, COUNT(dragon_target), -1);
}

static void dragon_tear(Fighter *fighter) {
  dragon_stream(fighter);
  battle_particles(fighter_peer(fighter), 3, 3, false);
}

static void dragon_bite_sound(Fighter *fighter) {
  dragon_stream(fighter);
  battle_sound(SOUND_B, 37);
}

static void dragon_delayed_sound(Fighter *fighter) {
  dragon_stream(fighter);
  battle_sound(SOUND_B, 65);
}

static void dragon_detach(Fighter *fighter) {
  dragon_stream(fighter);
  battle_detach_piece(fighter_peer(fighter), 5, (q16[3]){0}, (q16[3]){0});
  battle_detach_piece(fighter_peer(fighter), 6, (q16[3]){0}, (q16[3]){0});
}

static void dragon_fatality_script(Fighter *fighter) {
  int first = g_battle.phase_ticks + 1;
  int actor_end = 121 + scene_frame_count(dragon_actor, 2);
  int victim_end = 269 + scene_frame_count(dragon_target, COUNT(dragon_target));
  int last = maxi(actor_end, victim_end);
  native_motion_begin(fighter, last - first + 1, liu_kang_finish_scene);
  for (int frame = first; frame <= last; ++frame) {
    FighterFn action = dragon_stream;
    if (frame == 101) {
      action = dragon_spawn;
    } else if (frame == 121) {
      action = dragon_actor_start;
    } else if (frame == 269) {
      action = dragon_victim_start;
    } else if (frame == 281) {
      action = dragon_bite_sound;
    } else if (frame == 418) {
      action = dragon_delayed_sound;
    } else if (frame == 541) {
      action = dragon_detach;
    } else {
      int tear = frame - 285;
      if ((tear >= 0 && tear < 92 && tear % 4 == 0) || (tear >= 92 && tear < 248 && (tear - 92) % 6 == 0)) {
        action = dragon_tear;
      }
    }
    native_motion_frame(fighter, frame - first, NULL, 0, action);
  }
  native_motion_play(fighter);
}

static void liu_script_fatal_watch(Fighter *fighter) {
  (void)fighter;
  fighter_cancel_all_mmoves();
  battle_end_finisher();
}

static void liu_script_fatal_hit(Fighter *owner) {
  Fighter *victim = fighter_peer(owner);
  victim->script_phase = 2;
  victim->reaction = 177;
  battle_shatter(victim);
  battle_particles(victim, 3, 20, false);
  native_motion_begin(owner, 160, liu_script_fatal_watch);
  native_motion_play(owner);
}

static void fatal_fireball_object(Projectile *p) {
  if (g_battle.phase != PHASE_FATALITY || g_battle.finisher != FINISH_LIU_KANG_2) {
    p->kind = 0;
    return;
  }
  Fighter *victim = fighter_peer(&g_battle.fighters[p->owner]);
  for (int axis = 0; axis < 3; ++axis) {
    p->pos[axis] += p->vel[axis];
  }
  q16 dx = victim->body.pos[AXIS_X] - p->pos[AXIS_X];
  q16 dz = victim->body.pos[AXIS_Z] - p->pos[AXIS_Z];
  if ((int64_t)dx * dx + (int64_t)dz * dz <= (int64_t)0x28000 * 0x28000 || p->ticks >= 47) {
    p->kind = 0;
    liu_script_fatal_hit(&g_battle.fighters[p->owner]);
  }
}

void liu_kang_liu_script_fatal_setup(Fighter *fighter) {
  retail_scene_pose(fighter, 537, 0, true);
  retail_scene_pose(fighter_peer(fighter), 539, 0, false);
  fighter->flags |= FF_PAIRED;
  fighter_peer(fighter)->flags |= FF_PAIRED;
  liu_kang_entry_retail_script_004e0020(fighter);
}

void liu_kang_liu_script_fatal_spawn(Fighter *fighter, int task_type) {
  require(task_type == 38, "Invalid fatality projectile task");
  Projectile *p = battle_spawn_projectile(fighter, PROJECTILE_FATAL_FIREBALL, 48, fatal_fireball_object);
  if (p != NULL) {
    p->mesh_animation = &liu_kang_fireball_mesh;
    p->initial_mesh = 50;
    p->pos[AXIS_Y] -= 0x7333;
    for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
      p->pos[axis] += q16_mul(fighter_axis(fighter, axis), 0x1999);
      p->vel[axis] = q16_mul(fighter_axis(fighter, axis), 0x3333);
    }
  } else {
    liu_script_fatal_hit(fighter);
  }
}

void liu_kang_liu_script_fatal_align(Fighter *fighter) {
  Fighter *victim = fighter_peer(fighter);
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0xb333);
  }
}

void liu_kang_start_fatality(Fighter *fighter, int id) {
  require(id == FINISH_LIU_KANG_1 || id == FINISH_LIU_KANG_2, "Invalid native scene entry");
  if (!battle_begin_finisher(fighter, id)) {
    return;
  }
  Fighter *victim = fighter_peer(fighter);
  retail_scene_pose(victim, 13, 0, false);
  retail_scene_pose(fighter, 119, 0, true);
  if (id == FINISH_LIU_KANG_1) {
    for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
      victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0x20ccc);
    }
  }
  FighterFn start = id == FINISH_LIU_KANG_1 ? dragon_fatality_script : liu_kang_entry_retail_script_004dfff8;
  start(fighter);
}

/* Typed native boundaries used by generated VM code. These contain native
 * contact/controller work; all VM playback and branching lives in the include. */

void liu_kang_liu_script_scene_event(Fighter *fighter, int selector) {
  require(g_battle.phase == PHASE_FATALITY && selector >= 42 && selector <= 44, "Invalid fatality orchestration event");
  /* battle_begin_finisher owns input lock, camera and scene activation. */
  fighter->flags |= FF_BUSY | FF_UNTOUCHABLE;
}
