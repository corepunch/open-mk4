#include "../character.h"

static void clone_object(Projectile *p);
static void clone_cleanup(Fighter *fighter);
static void clone_done(Fighter *fighter);
static void clone_land_frames_frame_0(Fighter *fighter);
static void clone_land(Fighter *fighter);
static void clone_turn(Fighter *fighter);
static bool clone_falling(const Fighter *fighter);
static void clone_apex_frames_frame_0(Fighter *fighter);
static void clone_apex_wait(Fighter *fighter);
static void clone_spin(Fighter *fighter);
static const AttackRecord clone_record = {0x4ccc, -0xa3d, 0xbae1, 0x9e76, 67, 67, 0, ATTACK_MARK, 0};
static const MoveDef s_clone = {.clip = 301, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0405};
static const MoveDef s_clone_near = {.clip = 301, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0406};
static const OrbitLightDef clone_light = {25 * Q16_ONE, -25 * Q16_ONE, 0x18000, 0xccc};
static const mframe_t clone_land_frames[1] = {
    [0] = {.thinkfunc = clone_land_frames_frame_0},
};
static const mmove_t clone_land_phase = {.firstframe = 0, .lastframe = 0, .frame = clone_land_frames, .endfunc = clone_done};
// clang-format off: Keep table entries on one row, including long rows.
static const int clone_reverse_frames_samples[] = {
  8,
  7,
  6,
  5,
  4,
  3,
  2,
  1,
};
// clang-format on
static const mframe_t clone_reverse_frames[8] = {{0}};
static const mmove_t clone_reverse = {
    .firstframe = 0, .lastframe = 7, .frame = clone_reverse_frames, .endfunc = clone_land, .samples = clone_reverse_frames_samples};
static const mframe_t clone_apex_frames[1] = {
    [0] = {.thinkfunc = clone_apex_frames_frame_0},
};
static const mmove_t clone_apex = {.firstframe = 8, .lastframe = 8, .frame = clone_apex_frames, .endfunc = clone_turn};
static const mframe_t clone_extend_frames[5] = {{0}};
static const mmove_t clone_extend = {.firstframe = 3, .lastframe = 7, .frame = clone_extend_frames, .endfunc = clone_apex_wait};
static const mframe_t clone_start_frames[3] = {{0}};
static const mmove_t clone_start = {.firstframe = 0, .lastframe = 2, .frame = clone_start_frames, .endfunc = clone_spin};

static void clone_object(Projectile *p) {
  Fighter *owner = &g_battle.fighters[p->owner];
  if (g_battle.contacts_off || battle_contact_from(owner, &clone_record, p->pos, p->facing) != CONTACT_MISS) {
    p->kind = 0;
  }
}

static void clone_cleanup(Fighter *fighter) {
  fighter_upright(fighter);
}

static void clone_done(Fighter *fighter) {
  fighter_finish_mmove(fighter, sub_zero_air_land);
}

static void clone_land_frames_frame_0(Fighter *fighter) {
  fighter->mmove_hold = fighter_airborne(fighter);
}

static void clone_land(Fighter *fighter) {
  fighter->sample_override = 0;
  if (fighter_airborne(fighter)) {
    fighter_mmove_phase(fighter, &clone_land_phase);
  } else {
    clone_done(fighter);
  }
}

static void clone_turn(Fighter *fighter) {
  memset(fighter->body.angle, 0, sizeof fighter->body.angle);
  memset(fighter->body.spin, 0, sizeof fighter->body.spin);
  fighter->script_phase = 1;
  fighter_mmove_phase(fighter, &clone_reverse);
}

static bool clone_falling(const Fighter *fighter) {
  return fighter->body.vel[AXIS_Y] >= 0;
}

static void clone_apex_frames_frame_0(Fighter *fighter) {
  fighter->mmove_hold = !clone_falling(fighter);
}

static void clone_apex_wait(Fighter *fighter) {
  fighter_mmove_phase(fighter, &clone_apex);
}

static void clone_spin(Fighter *fighter) {
  fighter->body.root_y += 0x3333;
  fighter->body.pos[AXIS_Y] -= 0x3333;
  fighter->body.spin[AXIS_X] = -0x6147;
  fighter_mmove_phase(fighter, &clone_extend);
}

bool sub_zero_can_clone(const Fighter *fighter) {
  return g_battle.phase == PHASE_FIGHT && g_battle.ticks >= fighter->clone_ready && fighter_opponent(fighter)->action != ACTION_RETAIL_1003 &&
         !sub_zero_owned_object(fighter, PROJECTILE_CLONE);
}

void sub_zero_clone(Fighter *fighter) {
  bool near = fighter_distance(fighter) < 1.f;
  if (!near) {
    Projectile *p = battle_spawn_projectile(fighter, PROJECTILE_CLONE, fighter_opponent(fighter)->human ? 74 : 53, clone_object);
    if (p != NULL) {
      p->clip = fighter_move(fighter)->clip;
      p->sample = fighter_pose_sample(fighter);
      battle_object_light(p, &clone_light);
    }
  }
  mk4_script_stop_body(fighter);
  fighter_start_move(fighter, S_JUMP, near ? &s_clone_near : &s_clone);
  fighter->sample_override = 0;
  fighter_set_speed(fighter, -0x170a);
  fighter->body.vel[AXIS_Y] = -0x28f5;
  fighter->body.gravity = 0x28f;
  fighter->clone_ready = g_battle.ticks + 180;
  battle_sound(SOUND_C, 0xa);
  fighter->mmove_cleanup = clone_cleanup;
  fighter_mmove_phase(fighter, &clone_start);
}
