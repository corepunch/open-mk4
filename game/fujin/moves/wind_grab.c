#include "retail_vm.h"
#include "../character.h"
#include <math.h>

static const MoveDef wind_grab = {.clip = 10, .last = 2, .flags = SF_LOCK | SF_RECORD, .action = 0x20a};
static const MoveDef orbit_victim = {.clip = 8, .last = 28, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = 0x100d};
static const MeshAnimation grab_meshes[] = {
    {.first = 356, .count = 1, .period = 1, .scale = Q16_ONE, .source = MESH_SHARED, .additive = true},
    {.first = 350, .count = 1, .period = 1, .scale = Q16_ONE, .source = MESH_SHARED, .additive = true},
};
static void grab_loop(Fighter *fighter);

bool fujin_can_wind_grab(const Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  return g_battle.phase == PHASE_FIGHT && g_battle.ticks >= fighter->wind_grab_ready && victim->hits <= 2 && victim->action != 0x1001 &&
         victim->action != 0x100d;
}

static void grab_cleanup(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter->flags &= ~(unsigned)FF_NO_GUARD;
}

static void orbit_cleanup(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter->flags &= ~(unsigned)(FF_PAIRED | FF_UNTOUCHABLE);
}

static void grab_return(Fighter *fighter) {
  fighter_finish_mmove(fighter, mk4_mmove_thrown_finish);
}

static void grab_stamp_and_return(Fighter *fighter) {
  fighter->wind_grab_ready = g_battle.ticks + 90;
  grab_return(fighter);
}

static void grab_effect_step(Projectile *effect) {
  const Fighter *owner = &g_battle.fighters[effect->owner];
  effect->pos[AXIS_X] = owner->body.pos[AXIS_X];
  effect->pos[AXIS_Y] = 0;
  effect->pos[AXIS_Z] = owner->body.pos[AXIS_Z];
  if (!effect->phase && owner->action != 0x20a && owner->action != 0x1000) {
    /* 414810 retains the two authored wind surfaces for30 exit visits. */
    effect->phase = 1;
    effect->lifetime = effect->ticks + 30;
  }
}

static void grab_effect_start(Fighter *fighter) {
  /* 4146d0 creates two wind groups at chain offsets0/6 from4d1a18.
   * Keep their original shared surfaces in the existing bounded object pool. */
  for (int i = 0; i < 2; ++i) {
    Projectile *effect = battle_spawn_projectile(fighter, PROJECTILE_EFFECT, 6000, grab_effect_step);
    if (effect != NULL) {
      effect->mesh_animation = &grab_meshes[i];
      effect->initial_mesh = grab_meshes[i].first;
      memset(effect->angle, 0, sizeof effect->angle);
    }
  }
}

static void grab_damage(Fighter *fighter) {
  fighter->action = 0;
  mk4_script_damage(fighter, 0x1eb8);
  fighter->wind_grab_ready = g_battle.ticks + 90;
  native_hold(fighter, 14, grab_return);
}

static void grab_close(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  mk4_script_stop_body(fighter);
  if (battle_blocks_attack(victim, 0)) {
    fighter->action = 0x1015;
    battle_react(victim, 7);
    fighter->flags |= FF_NO_GUARD;
    mk4_mmove_bind(fighter, 13, 56, 0);
    native_hold(fighter, 60, grab_stamp_and_return);
  } else {
    mk4_script_context(fighter, 5);
    fighter->action = 0x1000;
    battle_react(victim, 98);
    battle_sound(SOUND_C, 0x3e);
    native_hold(fighter, 51, grab_damage);
  }
}

static void grab_control(Fighter *fighter) {
  if (fighter_distance(fighter) < 1.f) {
    grab_close(fighter);
    return;
  }
  /* Native496d20/491050 resolves screen-space steering, rather than
   * multiplying a fixed facing sign into world X. LP is word-B bit1/16. */
  q16 direction = fighter->held & BUTTON_BIT(BUTTON_LEFT) ? -0x1999 : 0;
  if (fighter->held & BUTTON_BIT(BUTTON_RIGHT)) {
    direction = 0x1999;
  }
  fighter->body.vel[AXIS_X] = q16_mul(fighter->body.vel[AXIS_X], 0x9999);
  fighter->body.vel[AXIS_Z] = q16_mul(fighter->body.vel[AXIS_Z], 0x9999);
  fighter_set_screen_speed(fighter, direction);
  if (!(fighter->held & BUTTON_BIT(BUTTON_LOW_PUNCH))) {
    mk4_script_stop_body(fighter);
    fighter->flags |= FF_NO_GUARD;
    native_hold(fighter, 12, grab_stamp_and_return);
  }
}

static void grab_sound_and_control(Fighter *fighter) {
  battle_sound(SOUND_C, 0x3e);
  grab_control(fighter);
}

static void grab_loop(Fighter *fighter) {
  /* Native480730 first sounds after20 visits, then every70. The loop
   * clock is encoded in authored rows; holding LP supplies its exit gate. */
  native_begin(fighter, 0, 69, grab_loop);
  for (int frame = 0; frame < 70; ++frame) {
    native_frame(fighter, frame, 0, frame == 19 ? grab_sound_and_control : grab_control);
  }
  native_play(fighter, false);
}

void fujin_grab_control_start(Fighter *fighter) {
  require(fighter_move(fighter)->clip == 11 && fighter_sample(fighter) == 0, "Wind-grab control needs its generated11-pose entry");
  grab_loop(fighter);
}

static q16 angle_wrap(q16 angle) {
  angle %= 0x6487e;
  return angle < 0 ? angle + 0x6487e : angle;
}

static q16 direction_angle(q16 x, q16 z) {
  return angle_wrap((q16)llround(atan2((double)z, (double)x) * Q16_ONE));
}

static void orbit_place(Fighter *victim, const Fighter *owner, q16 angle, q16 radius) {
  double radians = q16_float(angle);
  victim->body.pos[AXIS_X] = owner->body.pos[AXIS_X] + (q16)llround(cos(radians) * radius);
  victim->body.pos[AXIS_Z] = owner->body.pos[AXIS_Z] + (q16)llround(sin(radians) * radius);
}

static void orbit_release(Fighter *victim, const Fighter *owner, q16 angle) {
  orbit_place(victim, owner, angle, victim->saved_motion[1]);
  victim->flags &= ~(unsigned)(FF_PAIRED | FF_UNTOUCHABLE);
  q16 x = owner->body.pos[AXIS_X] - victim->body.pos[AXIS_X];
  q16 z = owner->body.pos[AXIS_Z] - victim->body.pos[AXIS_Z];
  double length = hypot((double)x, (double)z);
  victim->facing = x >= 0 ? 1 : -1;
  if (length > 0) {
    victim->heading[0] = (q16)llround(x * Q16_ONE / length);
    victim->heading[1] = (q16)llround(z * Q16_ONE / length);
  }
  retail_vm_enter_owned(victim, 0x4e9fb0, fighter_ready, 5);
}

static void orbit_step(Fighter *victim) {
  const Fighter *owner = fighter_opponent(victim);
  q16 angle = victim->saved_motion[0], radius = victim->saved_motion[1];
  orbit_place(victim, owner, angle, radius);
  victim->saved_motion[0] = angle_wrap(angle + 0x4ccc);
  radius += 0x28f;
  victim->saved_motion[1] = radius > 3 * Q16_ONE ? 5 * Q16_ONE : radius;
  victim->mmove_hold = true;
  if (victim->native_variant == 0) {
    q16 y = victim->body.pos[AXIS_Y] - 0x28f;
    victim->body.pos[AXIS_Y] = y < -0x2cccc ? -0x2cccc : y;
    if (owner->action != 0x1000 || owner->pending != NULL) {
      victim->native_variant = 1;
    }
  } else {
    /* Native487a69 releases toward the arena origin once the orbit comes
     * within.31 radians of that heading, then9fb0 supplies the flight. */
    q16 target = direction_angle(-owner->body.pos[AXIS_X], -owner->body.pos[AXIS_Z]);
    q16 difference = angle_wrap(angle - target);
    if (difference > 0x3243f) {
      difference = 0x6487e - difference;
    }
    if (difference <= 0x4f5c) {
      orbit_release(victim, owner, target);
    }
  }
}

void fujin_grab_victim(Fighter *fighter) {
  mk4_script_disarm_self(fighter);
  fighter_start_move(fighter, S_KNOCKDOWN, &orbit_victim);
  fighter->flags |= FF_REACTING | FF_PAIRED | FF_UNTOUCHABLE;
  fighter->mmove_cleanup = orbit_cleanup;
  fighter->native_variant = 0;
  const Fighter *owner = fighter_opponent(fighter);
  fighter->saved_motion[0] = direction_angle(fighter->body.pos[AXIS_X] - owner->body.pos[AXIS_X], fighter->body.pos[AXIS_Z] - owner->body.pos[AXIS_Z]);
  fighter->saved_motion[1] = 0x8000;
  mk4_script_indexed_sound(fighter, 5);
  native_begin(fighter, 0, 0, NULL);
  native_frame(fighter, 0, 0, orbit_step);
  native_play(fighter, false);
}

void fujin_wind_grab(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter_start_move(fighter, S_PUNCH, &wind_grab);
  fighter->mmove_cleanup = grab_cleanup;
  grab_effect_start(fighter);
  retail_vm_enter_owned(fighter, 0x4e8830, fighter_ready, 5);
}
