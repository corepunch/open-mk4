#include "retail_vm.h"
#include "../character.h"

static void torpedo_pose(Fighter *fighter, float pose);
static void torpedo_cycle(Fighter *fighter);
static void torpedo_land(Fighter *fighter);
static void torpedo_miss_motion(Fighter *fighter);
static void torpedo_recoil(Fighter *fighter);
static void torpedo_release(Fighter *fighter);
static void torpedo_pair(Fighter *fighter);

static const MoveDef s_torpedo = {.clip = 94, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = 0x202};
static const MoveDef s_torpedo_victim = {.clip = 135, .last = MOVE_END, .flags = SF_LOCK, .action = ACTION_RETAIL_4003};
static const AttackRecord torpedo_record = {85852, -3932, 62914, 20971, 82, 82, 0, 0, 9830};

void raiden_script_reaction(Fighter *fighter, int reaction) {
  if (reaction == 187) {
    Fighter *victim = fighter_peer(fighter);
    mk4_script_scene_pose(victim, 574, 0);
    fighter_cancel_mmove(victim);
    retail_vm_enter_owned(victim, 0x4e5770, fighter_ready, 1);
    return;
  }
  if (reaction == 175) {
    return; /* Arena spike coordinator owns the victim. */
  }
  require(reaction == 102 || reaction == 107 || reaction == 111, "Unexpected Raiden paired reaction");
  Fighter *victim = mk4_script_release_pair(fighter);
  victim->reaction = reaction;
  victim->flags |= FF_REACTING;
  fighter_cancel_mmove(victim);
  if (reaction == 111) {
    raiden_lk_victim(victim);
  } else {
    RetailScriptContinuation script =
        reaction == 107 ? (RetailScriptContinuation){0x4e9b40, mk4_script_thrown_ready, 1} : (RetailScriptContinuation){0x4e9bd0, mk4_script_thrown_ready, 1};
    retail_vm_enter_owned(victim, script.entry, script.finish, script.native_owner);
  }
}

void raiden_special_cleanup(Fighter *fighter) {
  mk4_script_pair_cleanup(fighter);
  fighter->body.physics = NULL;
  fighter_set_speed(fighter, 0);
  if (fighter_airborne(fighter)) {
    fighter->body.gravity = 0x28f;
  }
}

static void torpedo_pose(Fighter *fighter, float pose) {
  fighter->sample_override = (int)pose;
}

static void torpedo_cycle(Fighter *fighter) {
  native_motion_begin(fighter, 22, NULL);
  for (int frame = 0; frame < 22; ++frame) {
    native_motion_frame(fighter, frame, torpedo_pose, (float)(20 + frame / 2), NULL);
  }
  fighter->native_motion.loop = true;
  native_motion_play(fighter);
}

void raiden_script_torpedo_motion(Fighter *fighter) {
  int pose = fighter_sample(fighter);
  int delay = fighter->ticks & 1;
  int count = delay + (30 - pose) * 2;
  if (count <= 0) {
    torpedo_cycle(fighter);
  } else {
    native_motion_begin(fighter, count, torpedo_cycle);
    for (int frame = 0; frame < count; ++frame) {
      int sample = pose + (frame >= delay ? 1 + (frame - delay) / 2 : 0);
      native_motion_frame(fighter, frame, torpedo_pose, (float)sample, NULL);
    }
    native_motion_play(fighter);
  }
  native_motion_step(fighter);
}

unsigned raiden_script_torpedo_query(Fighter *fighter) {
  return battle_contact(fighter, &torpedo_record) == CONTACT_MISS ? 0u : 1u;
}

static void torpedo_land(Fighter *fighter) {
  fighter_finish_mmove(fighter, fighter_ready);
}

void raiden_script_torpedo_miss(Fighter *fighter) {
  fighter->body.physics = NULL;
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    fighter->body.vel[axis] = q16_mul(fighter->body.vel[axis], 0x4000);
  }
  fighter->body.gravity = 0x28f;
  native_await(fighter, native_grounded, torpedo_land);
}

/* The step runs inside a generated frame row that installs its successor
 * afterwards. The world pump runs this physics later in the same tick,
 * before gravity and integration, so the miss keeps the 47d720 timing. */
static void torpedo_miss_motion(Fighter *fighter) {
  raiden_script_torpedo_miss(fighter);
}

void raiden_script_torpedo_step(Fighter *fighter) {
  int64_t radius2 = 0, next2 = 0;
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    int64_t pos = fighter->body.pos[axis];
    int64_t next = pos + fighter->body.vel[axis];
    radius2 += pos * pos;
    next2 += next * next;
  }
  if (radius2 >= (int64_t)10 * Q16_ONE * 10 * Q16_ONE && next2 >= radius2) {
    fighter->body.physics = torpedo_miss_motion;
  }
}

static void torpedo_recoil(Fighter *fighter) {
  fighter->action = 0x1010;
  fighter_set_speed(fighter, -0xccc);
  fighter->body.vel[AXIS_Y] = -0x23d7;
  fighter->body.gravity = 0x168;
  native_await(fighter, native_grounded, torpedo_land);
}

static void torpedo_release(Fighter *fighter) {
  Fighter *victim = fighter_peer(fighter);
  if (victim->pending == NULL && (fighter->flags & FF_PAIRED) && (victim->flags & FF_PAIRED)) {
    mk4_script_release_pair(fighter);
    battle_react(victim, 83);
  } else {
    fighter->flags &= ~(unsigned)FF_PAIRED;
  }
  torpedo_recoil(fighter);
}

static void torpedo_pair(Fighter *fighter) {
  Fighter *victim = fighter_peer(fighter);
  if (victim->pending != NULL || !(fighter->flags & FF_PAIRED) || !(victim->flags & FF_PAIRED)) {
    torpedo_release(fighter);
    return;
  }
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0x18000);
  }
  victim->body.pos[AXIS_Y] = fighter->body.pos[AXIS_Y];
  if (fighter_distance(fighter) > 10.f) {
    torpedo_release(fighter);
  }
}

void raiden_script_torpedo_contact(Fighter *fighter) {
  fighter->body.physics = NULL;
  if (fighter->contact == CONTACT_BLOCK) {
    torpedo_recoil(fighter);
    return;
  }
  Fighter *victim = fighter_peer(fighter);
  mk4_mmove_carry_victim(fighter, &s_torpedo_victim);
  fighter_cancel_mmove(victim);
  retail_vm_enter_owned(victim, 0x4ea108, fighter_ready, 1);
  int pose = fighter_sample(fighter);
  native_begin(fighter, 1, 50, torpedo_release);
  for (int frame = 1; frame <= 50; ++frame) {
    native_frame(fighter, frame, pose, torpedo_pair);
  }
  native_play(fighter, false);
}

void raiden_torpedo(Fighter *fighter) {
  if (g_battle.phase != PHASE_FIGHT) {
    return;
  }
  bool fast = fighter_fast_start(fighter);
  fighter_start_move(fighter, S_KICK, &s_torpedo);
  fighter_set_speed(fighter, 0x2666);
  mk4_script_save_motion(fighter);
  fighter->mmove_cleanup = raiden_special_cleanup;
  RetailScriptContinuation script = fast ? (RetailScriptContinuation){0x4e8440, fighter_ready, 1} : (RetailScriptContinuation){0x4e8458, fighter_ready, 1};
  retail_vm_enter_owned(fighter, script.entry, script.finish, script.native_owner);
}

void raiden_script_torpedo_victim_motion(Fighter *victim) {
  const Fighter *owner = fighter_opponent(victim);
  if (!(victim->flags & FF_PAIRED)) {
    victim->body.physics = NULL;
    return;
  }
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    victim->body.pos[axis] = owner->body.pos[axis] + q16_mul(fighter_axis(owner, axis), 0x18000);
  }
  victim->body.pos[AXIS_Y] = owner->body.pos[AXIS_Y];
}

void raiden_generated_reaction(Fighter *victim) {
  RetailScriptContinuation script = {0};
  switch (victim->reaction) {
    case 47:
      script = (RetailScriptContinuation){0x4ea068, mk4_script_thrown_ready, 1};
      battle_wall_recoil(victim);
      break;
    case 53:
      script = (RetailScriptContinuation){0x4e8e18, mk4_script_thrown_ready, 1};
      break;
    case 66:
      script = (RetailScriptContinuation){0x4e9d50, mk4_script_thrown_ready, 1};
      break;
    case 82:
      script = (RetailScriptContinuation){0x4ea108, fighter_ready, 1};
      break;
    case 83:
      script = fighter_airborne(victim) ? (RetailScriptContinuation){0x4e9ae8, mk4_script_thrown_ready, 1}
                                        : (RetailScriptContinuation){0x4e9ad0, mk4_script_thrown_ready, 1};
      break;
    default:
      require(false, "Unexpected Raiden record reaction");
      return;
  }
  mk4_script_disarm_self(victim);
  fighter_start_move(victim, S_KNOCKDOWN, roster_def(victim->roster)->moves[S_KNOCKDOWN]);
  victim->flags |= FF_REACTING;
  victim->sample_override = 0;
  if (fighter_airborne(victim) && victim->body.gravity == 0) {
    victim->body.gravity = 0x28f;
  }
  retail_vm_enter_owned(victim, script.entry, script.finish, script.native_owner);
}
