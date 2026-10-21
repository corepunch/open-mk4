#include "retail_vm.h"
#include "retail_native.h"
#include "../character.h"

static const MoveDef first_owner = {.clip = 398, .last = 26, .flags = SF_LOCK | SF_RECORD, .action = 0x700};
static const MoveDef first_victim = {.clip = 13, .last = 55, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = 0x700};
// clang-format off: Keep table entries on one row, including long rows.
static const SceneClip first_intro[] = {
  {{.clip = 398, .last = 26, .flags = SF_LOCK | SF_RECORD}, 0, 0, 1, 0x32f1, 0x3333},
};
static const SceneClip first_bow[] = {
  {{.clip = 210, .last = 36, .flags = SF_LOCK | SF_RECORD}, 0, 0, 1, 0x32f1, 0x3333},
};
static const ParticleVisual tornado[] = {
  {.animation = {.first = 0, .count = 1, .period = 1, .scale = Q16_ONE, .source = MESH_FUJIN_WIND}, .billboard = true, .additive = true},
  {.animation = {.first = 1, .count = 1, .period = 1, .scale = Q16_ONE, .source = MESH_FUJIN_WIND}, .billboard = true, .additive = true},
  {.animation = {.first = 2, .count = 1, .period = 1, .scale = Q16_ONE, .source = MESH_FUJIN_WIND}, .billboard = true, .additive = true},
  {.animation = {.first = 3, .count = 1, .period = 1, .scale = Q16_ONE, .source = MESH_FUJIN_WIND}, .billboard = true, .additive = true},
  {.animation = {.first = 4, .count = 1, .period = 1, .scale = Q16_ONE, .source = MESH_FUJIN_WIND}, .billboard = true, .additive = true},
  {.animation = {.first = 5, .count = 1, .period = 1, .scale = Q16_ONE, .source = MESH_FUJIN_WIND}, .billboard = true, .additive = true},
};
// clang-format on
static const MeshAnimation first_bolt = {.first = 106, .count = 1, .period = 1, .scale = Q16_ONE, .source = MESH_SHARED};

bool fujin_can_first_fatality(const Fighter *fighter) {
  if (!retail_can_finish_range(fighter)) {
    return false;
  }
  unsigned pair = BUTTON_BIT(BUTTON_BLOCK) | BUTTON_BIT(BUTTON_RUN);
  unsigned forbidden =
      BUTTON_BIT(BUTTON_UP) | BUTTON_BIT(BUTTON_HIGH_PUNCH) | BUTTON_BIT(BUTTON_LOW_PUNCH) | BUTTON_BIT(BUTTON_HIGH_KICK) | BUTTON_BIT(BUTTON_LOW_KICK);
  if ((fighter->held & pair) != pair || (fighter->held & forbidden)) {
    return false;
  }
  /*45e3b0/4e4138 increments a native input-event counter, not a clock.
   *Both eligible press tables visit this gate while BL+RUN are held. */
  Fighter *owner = &g_battle.fighters[fighter == &g_battle.fighters[1]];
  return ++owner->fujin_finish_inputs == 14;
}

static void first_wind_start(Fighter *victim) {
  Fighter *owner = (Fighter *)fighter_opponent(victim);
  for (int part = 0; part < 6; ++part) {
    ParticleSpray spray = {.visual = &tornado[part], .count = 1, .lifetime = 600};
    Vec3 point = {fighter_x(victim), q16_float(victim->body.ground - part * 0x6666), fighter_z(victim)};
    battle_spray(&spray, point, (Vec3){0}, 0, 0);
  }
  /*450090/4501e0: rise for216 visits, approaching the point2.3 ahead
   *of the caster at.005 of the initial displacement for200 visits. */
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    q16 target = owner->body.pos[axis] + q16_mul(fighter_axis(owner, axis), 0x24ccc);
    victim->body.vel[axis] = q16_mul(target - victim->body.pos[axis], 0x147);
  }
}

static void first_spin_start(Fighter *victim) {
  victim->body.spin[AXIS_Y] = victim->facing > 0 ? -0x51e : 0x51e;
}

static void first_spin_accelerate(Fighter *victim) {
  victim->body.spin[AXIS_Y] = q16_mul(victim->body.spin[AXIS_Y], 0x110c4);
}

static void first_rise(Fighter *victim) {
  victim->body.pos[AXIS_Y] -= 0x28f;
}

static void first_rise_and_spin(Fighter *victim) {
  first_rise(victim);
  first_spin_accelerate(victim);
}

static void first_start_rise(Fighter *victim) {
  first_wind_start(victim);
  first_rise_and_spin(victim);
}

static void first_stop_drift(Fighter *victim) {
  victim->body.vel[AXIS_X] = 0;
  victim->body.vel[AXIS_Z] = 0;
  first_rise(victim);
}

static bool first_sound_loop(BattleEvent *event, Fighter *owner, Fighter *victim) {
  (void)victim;
  if (!owner->native_signal || g_battle.phase != PHASE_FATALITY) {
    return false;
  }
  battle_sound(SOUND_B, 0x26);
  battle_event_delay(event, 60);
  return true;
}

void fujin_first_loop_start(Fighter *fighter) {
  /*44f5e0 sets13 before the initial399 cycle:14 complete passes. */
  fighter->native_variant = 13;
  BattleEvent *sound = battle_event_start(fighter, first_sound_loop, false, false);
  if (sound != NULL) {
    battle_event_delay(sound, 25);
  }
  retail_vm_enter_owned(fighter, 0x4e1f68, fighter_ready, 5);
}

void fujin_first_cycle(Fighter *fighter) {
  if (fighter->native_variant-- > 0) {
    retail_vm_enter_owned(fighter, 0x4e1f68, fighter_ready, 5);
    return;
  }
  fujin_equip_crossbow(fighter);
  native_scene_blends(fighter, first_bow, 1, fujin_entry_retail_script_004e1f80);
}

static void first_explode(Fighter *victim, Fighter *owner) {
  /*44e090 hides the body eight visits after the bolt's ten-visit arrival.
   *44eb00's closed head/hand debris uses the2/9/10 list at4e1cd0. */
  Fighter source = *victim;
  source.appearance = APPEARANCE_NORMAL;
  memset(source.hidden_joints, 0, sizeof source.hidden_joints);
  if (source.assets != NULL) {
    source.scene_model = &source.assets->cut_characters.data[roster_def(source.roster)->asset];
  }
  static const int joints[] = {2, 9, 10};
  static const q16 scales[] = {0x1999, 0x147a, 0x147a};
  for (int part = 0; part < 3; ++part) {
    ScenePiece *piece = battle_detach_piece(&source, joints[part], (q16[3]){part == 1 ? -0x2147 : 0x2147, -0x2147, 0}, (q16[3]){0, 0x2147, 0});
    if (piece != NULL) {
      piece->scale = scales[part];
      piece->gravity = 0x189;
    }
  }
  battle_blood_hit(owner, victim, (Vec3){fighter_x(victim), q16_float(victim->body.pos[AXIS_Y]), fighter_z(victim)}, Q16_ONE);
  victim->appearance = APPEARANCE_HIDDEN;
  mk4_script_stop_body(victim);
  fighter_stop_mmove(victim);
}

static void first_bolt_step(Projectile *bolt) {
  if (g_battle.phase != PHASE_FATALITY) {
    bolt->kind = 0;
    return;
  }
  for (int axis = 0; axis < 3; ++axis) {
    bolt->pos[axis] += bolt->vel[axis];
  }
  if (bolt->ticks == 10) {
    battle_sound(SOUND_C, 0x43);
    bolt->mesh_animation = NULL;
  }
  if (bolt->ticks == 18) {
    first_explode(&g_battle.fighters[1 - bolt->owner], &g_battle.fighters[bolt->owner]);
    bolt->kind = 0;
  }
}

static bool first_shot(BattleEvent *event, Fighter *owner, Fighter *victim) {
  (void)event;
  (void)victim;
  ++owner->native_variant;
  owner->native_signal = false;
  for (int i = 0; i < MAX_PARTICLES; ++i) {
    Particle *particle = &g_battle.particles[i];
    if (particle->visual != NULL && particle->visual->animation.source == MESH_FUJIN_WIND) {
      particle->ticks = 0;
    }
  }
  Projectile *bolt = battle_spawn_projectile(owner, PROJECTILE_EFFECT, 19, first_bolt_step);
  if (bolt != NULL) {
    bolt->mesh_animation = &first_bolt;
    bolt->initial_mesh = 106;
    bolt->angle[1] = 0x3243f;
    bolt->angle[2] = -Q16_ONE;
    bolt->pos[AXIS_Y] = -0x18000;
    for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
      bolt->pos[axis] += q16_mul(fighter_axis(owner, axis), 0xccc);
      bolt->vel[axis] = q16_mul(fighter_axis(owner, axis), 0x3333);
    }
    bolt->vel[AXIS_Y] = -0x2e14;
  }
  battle_sound(SOUND_C, 0x29);
  retail_vm_enter_owned(owner, 0x4e1f98, fighter_ready, 5);
  return true;
}

static bool first_shots_done(BattleEvent *event, Fighter *owner, Fighter *victim) {
  (void)event;
  (void)owner;
  (void)victim;
  return false;
}

void fujin_first_shoot(Fighter *fighter) {
  fighter->native_variant = 0;
  BattleEvent *event = battle_event_start(fighter, first_shots_done, false, false);
  require(event != NULL, "Fujin first scene needs its bounded shot coordinator");
  battle_event_program(event, 86);
  battle_event_frame(event, 0, 0, first_shot);
  battle_event_frame(event, 40, 0, first_shot);
  battle_event_frame(event, 85, 0, first_shot);
}

void fujin_first_fire_end(Fighter *fighter) {
  fighter_stop_mmove(fighter);
  if (fighter->native_variant == 3) {
    battle_end_finisher();
  }
}

void fujin_first_fatality(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, FINISH_FUJIN_1)) {
    return;
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_move(victim, S_KNOCKDOWN, &first_victim);
  victim->flags |= FF_BUSY | FF_UNTOUCHABLE;
  native_begin(victim, 0, 305, fighter_stop_mmove);
  for (int frame = 0; frame <= 305; ++frame) {
    FighterFn action = frame == 80                 ? first_spin_start
                       : frame == 90               ? first_start_rise
                       : frame == 290              ? first_stop_drift
                       : frame > 90 && frame < 140 ? first_rise_and_spin
                       : frame >= 90               ? first_rise
                       : frame > 80 && frame < 140 ? first_spin_accelerate
                                                   : NULL;
    native_frame(victim, frame, 0, action);
  }
  native_play(victim, false);
  fighter_start_move(fighter, S_PUNCH, &first_owner);
  fighter->flags |= FF_BUSY | FF_UNTOUCHABLE;
  fighter->native_signal = true;
  g_battle.camera.follow_height = true;
  native_scene_blends(fighter, first_intro, 1, fujin_entry_retail_script_004e1f58);
}
