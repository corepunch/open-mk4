#include "retail_vm.h"
#include "../character.h"

static const MoveDef caster = {.clip = 98, .last = 39, .flags = SF_LOCK | SF_RECORD, .action = 0x403};
static const MoveDef slam = {.clip = 99, .last = 29, .flags = SF_LOCK | SF_RECORD, .action = 0x402};
static const MoveDef caught = {.clip = 8, .last = 28, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = 0x1001};
static void vortex_lift(Fighter *fighter);
static void vortex_next(Fighter *fighter);
static void wind_effect_step(Projectile *effect);
// clang-format off: Keep table entries on one row, including long rows.
static const ParticleVisual wind_visuals[] = {
  {.animation = {.first = 84, .count = 1, .period = 1, .scale = Q16_ONE, .source = MESH_SHARED}, .additive = true},
  {.animation = {.first = 85, .count = 1, .period = 1, .scale = Q16_ONE, .source = MESH_SHARED}, .additive = true},
  {.animation = {.first = 86, .count = 1, .period = 1, .scale = Q16_ONE, .source = MESH_SHARED}, .additive = true},
  {.animation = {.first = 87, .count = 1, .period = 1, .scale = Q16_ONE, .source = MESH_SHARED}, .additive = true},
  {.animation = {.first = 88, .count = 1, .period = 1, .scale = Q16_ONE, .source = MESH_SHARED}, .additive = true},
  {.animation = {.first = 89, .count = 1, .period = 1, .scale = Q16_ONE, .source = MESH_SHARED}, .additive = true},
  {.animation = {.first = 90, .count = 1, .period = 1, .scale = Q16_ONE, .source = MESH_SHARED}, .additive = true},
  {.animation = {.first = 91, .count = 1, .period = 1, .scale = Q16_ONE, .source = MESH_SHARED}, .additive = true},
};
// clang-format on
static const q16 wind_root_y[] = {0, -0x10a3, -0x2147, -0x3c28, -0x5ae1, -0x71eb, -0x8666, -0xa28f};

bool fujin_can_wind(const Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  return g_battle.phase == PHASE_FIGHT && victim->action != 0x100d && victim->action != 0x1001 && victim->action != 0x1002 && victim->hits <= 2;
}

bool fujin_can_wind_slam(const Fighter *fighter) {
  return g_battle.phase == PHASE_FIGHT && fighter_opponent(fighter)->action == 0x1001;
}

static void wind_cleanup(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  int slot = fighter == &g_battle.fighters[0] ? 0 : 1;
  for (int i = 0; i < MAX_PROJECTILES; ++i) {
    Projectile *effect = &g_battle.projectiles[i];
    if (effect->step == wind_effect_step && effect->owner == slot) {
      effect->kind = 0;
    }
  }
}

static void wind_effect_step(Projectile *effect) {
  const Fighter *victim = &g_battle.fighters[effect->owner];
  if (victim->action != 0x1001 || g_battle.contacts_off) {
    effect->kind = 0;
    return;
  }
  /* 414950 emits three additive wind meshes per visit from4d1a50.
   * Reuse the bounded particle pool for their random spread and lifetime. */
  Vec3 origin = {fighter_x(victim), 0, fighter_z(victim)};
  for (int i = 0; i < 3; ++i) {
    int mesh = (int)((g_battle.ticks * 3 + (unsigned)i) % 8);
    ParticleSpray spray = {.visual = &wind_visuals[mesh],
        .count = 1,
        .lifetime = 10,
        .velocity = {0, -0x23d7, 0},
        .spread = {0, 0x7ae, 0},
        .position_spread = {0xd999, 0, 0xd999},
        .offset = {0, wind_root_y[mesh], 0}};
    battle_spray(&spray, origin, (Vec3){0}, 0, 0);
  }
}

static void wind_motion_hold(Fighter *fighter) {
  fighter->mmove_hold = !fighter->native_condition(fighter);
}

static void wind_wait(Fighter *fighter, FighterTest done, FighterFn end) {
  fighter->native_condition = done;
  native_begin(fighter, 0, 0, end);
  native_frame(fighter, 0, fighter_sample(fighter), wind_motion_hold);
  native_play(fighter, false);
}

static bool at_apex(const Fighter *fighter) {
  return fighter->body.vel[AXIS_Y] >= 0;
}

static bool rising(const Fighter *fighter) {
  return fighter->body.vel[AXIS_Y] < 0;
}

static void vortex_up_complete(Fighter *fighter) {
  native_hold(fighter, 8, vortex_next);
}

static void vortex_reverse(Fighter *fighter) {
  battle_sound(SOUND_C, 0x40);
  fighter->body.gravity = -0x83;
  wind_wait(fighter, rising, vortex_up_complete);
}

static void vortex_apex(Fighter *fighter) {
  native_hold(fighter, 8, vortex_reverse);
}

static void vortex_lift(Fighter *fighter) {
  /* 4881b0/4881f0: apex, eight visits, reverse acceleration until
   * rising, then eight visits before returning to the next of four lifts. */
  fighter->body.gravity = 0x83;
  wind_wait(fighter, at_apex, vortex_apex);
}

static void vortex_landed(Fighter *fighter) {
  mk4_mmove_bind(fighter, 195, 29, 28);
  if (fighter->health > 0) {
    retail_vm_enter_owned(fighter, 0x4e9fe0, fighter_ready, 5);
  } else {
    fighter_finish_mmove(fighter, NULL);
  }
}

static void vortex_next(Fighter *fighter) {
  if (++fighter->native_variant < 4) {
    vortex_lift(fighter);
  } else {
    fighter->action = 0x1002;
    fighter->body.gravity = 0xccc;
    wind_wait(fighter, native_grounded, vortex_landed);
  }
}

static void vortex_lifts_begin(Fighter *fighter) {
  battle_sound(SOUND_C, 0x40);
  fighter->native_variant = 0;
  vortex_lift(fighter);
}

static void vortex_damp(Fighter *fighter) {
  fighter->body.vel[AXIS_Y] = q16_mul(fighter->body.vel[AXIS_Y], 0xe666);
}

static void vortex_freeze(Fighter *fighter) {
  fighter->body.gravity = 0;
  native_begin(fighter, 0, 19, vortex_lifts_begin);
  for (int frame = 0; frame < 20; ++frame) {
    native_frame(fighter, frame, 0, vortex_damp);
  }
  native_play(fighter, false);
}

void fujin_wind_victim(Fighter *fighter) {
  mk4_script_disarm_self(fighter);
  fighter_start_move(fighter, S_KNOCKDOWN, &caught);
  fighter->flags |= FF_REACTING;
  fighter->mmove_cleanup = wind_cleanup;
  fighter->sample_override = 0;
  battle_sound(SOUND_C, 5);
  battle_spawn_projectile(fighter, PROJECTILE_EFFECT, 400, wind_effect_step);
  fighter->body.gravity = -0x28f;
  native_hold(fighter, 10, vortex_freeze);
}

static void slam_landed(Fighter *fighter) {
  mk4_mmove_bind(fighter, 195, 29, 28);
  fighter->hits++;
  retail_vm_enter_owned(fighter, 0x4e9ff0, fighter_ready, 5);
}

static void slam_drop(Fighter *fighter) {
  fighter->body.gravity = 0xf5c;
  wind_wait(fighter, native_grounded, slam_landed);
}

void fujin_wind_slam_victim(Fighter *fighter) {
  /* Reaction97 changes gravity while retaining the captured vortex velocity.
   * battle_react preserves that velocity across the previous action cleanup. */
  fighter_start_move(fighter, S_KNOCKDOWN, &caught);
  fighter->action = 0x1002;
  fighter->flags |= FF_REACTING;
  fighter->mmove_cleanup = wind_cleanup;
  fighter->sample_override = 0;
  fighter->body.gravity = -0x11eb;
  native_hold(fighter, 4, slam_drop);
}

void fujin_wind_crouch_return(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter_finish_mmove(fighter, NULL);
  if (fighter->health > 0) {
    fighter_enter_state(fighter, S_CROUCH);
  }
}

void fujin_wind_recovery(Fighter *fighter) {
  fighter->action = 0x1015;
  retail_vm_enter_owned(fighter, 0x4e5c20, fighter_ready, 5);
}

void fujin_wind(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter_start_move(fighter, S_PUNCH, &caster);
  retail_vm_enter_owned(fighter, 0x4e5c50, fighter_ready, 5);
}

void fujin_wind_slam(Fighter *fighter) {
  if (!fujin_can_wind_slam(fighter)) {
    fighter_ready(fighter);
    return;
  }
  mk4_script_stop_body(fighter);
  fighter_start_move(fighter, S_KICK, &slam);
  retail_vm_enter_owned(fighter, 0x4e5c00, fighter_ready, 5);
}
