#include "retail_native.h"
#include "../character.h"

/* HP record 4e457c (U,D,D,D, window 50, gate 45e530) targets 467cd0, which
 * selects 4dfe90[10] = 43bf40: shared setup 41aaf0, then VM 4dff08. */
static const MoveDef owner_pose = {.clip = 493, .last = 39, .flags = SF_LOCK | SF_RECORD, .action = 0x700};
static const MoveDef victim_pose = {.clip = 13, .last = 55, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = 0x700};
/* 4818f0 reads 4ea420 in this order, separating one joint every five visits. */
static const int torn_joints[] = {2, 10, 9, 8, 7, 6, 5, 3, 12, 11, 14, 13, 16, 15};
/* 41e490 droplets reuse the 4d16d0 shared meshes 340..344; 40f380 adds
 * 0x106 gravity each visit. */
static const ParticleVisual acid_visual = {
    .animation = {.first = 340, .count = 5, .period = 2, .scale = 0x51eb, .source = MESH_SHARED, .offset = {0x1100, 0xd80, 0}}};
static const ParticleSpray acid_spray = {
    .visual = &acid_visual, .count = 2, .lifetime = 30, .gravity = 0x106, .velocity = {0, 0x1708, 0}, .spread = {0x51e, 0x7ae, 0x51e}};

enum {
  HOVER_RISE,
  HOVER_EASE,
  HOVER_FALLING,
  HOVER_RISING
};

/* Task 38 (46a280) writes the owner's gravity: -0x28f, ten +0x41 steps,
 * then +0x83 until vertical speed is not upward (48f490), ten visits,
 * -0x83 until it rises (48f510), ten visits, and so on. */
static bool hover_step(BattleEvent *event, Fighter *owner, Fighter *victim) {
  (void)victim;
  if (g_battle.phase != PHASE_FATALITY && g_battle.phase != PHASE_MATCH_OVER) {
    return false;
  }
  switch (event->phase) {
    case HOVER_RISE:
      owner->body.gravity = -0x28f;
      event->phase = HOVER_EASE;
      event->data[0] = 0;
      return true;
    case HOVER_EASE:
      if (event->data[0]++ == 0) {
        return true;
      }
      owner->body.gravity += 0x41;
      if (event->data[0] > 10) {
        owner->body.gravity = 0x83;
        event->phase = HOVER_FALLING;
      }
      return true;
    case HOVER_FALLING:
      if (owner->body.vel[AXIS_Y] >= 0) {
        battle_event_delay(event, 10);
        owner->body.gravity = -0x83;
        event->phase = HOVER_RISING;
      }
      return true;
    default:
      if (owner->body.vel[AXIS_Y] < 0) {
        battle_event_delay(event, 10);
        owner->body.gravity = 0x83;
        event->phase = HOVER_FALLING;
      }
      return true;
  }
}

void reptile_second_hover(Fighter *fighter, int task_type) {
  require(task_type == 38, "Unexpected Reptile hover task");
  BattleEvent *event = battle_event_start(fighter, hover_step, false, false);
  require(event != NULL, "Reptile second fatality needs its bounded hover task");
  event->phase = HOVER_RISE;
}

static bool acid_drops(BattleEvent *event, Fighter *owner, Fighter *victim) {
  (void)victim;
  if (event->data[0]-- <= 0 || g_battle.phase == PHASE_ROUND_OVER) {
    return false;
  }
  Vec3 head;
  Mat3 basis;
  if (fighter_joint_position(owner, 2, &head, &basis)) {
    /* 41e490 transforms the head-local point (.01, -.05, -.1) via 4b8140. */
    Vec3 mouth = vec_add(head, mat3_apply(basis, (Vec3){q16_float(0x28f), q16_float(-0xccc), q16_float(-0x1999)}));
    Vec3 forward = {q16_float(fighter_axis(owner, AXIS_X)), 0, q16_float(fighter_axis(owner, AXIS_Z))};
    battle_spray(&acid_spray, mouth, forward, 0x1eb8, 0xa3d);
  }
  return true;
}

/* Task 0 (41e490): 121 visits, two droplets each, from Reptile's head
 * along the pair axis at 0x28f5 +/- 0xa3d. The 40f380 victim/floor
 * splash and smoke follow-ups are decorative and not reproduced. */
void reptile_second_acid(Fighter *fighter, int task_type) {
  require(task_type == 0, "Unexpected Reptile acid emitter task");
  BattleEvent *event = battle_event_start(fighter, acid_drops, false, false);
  if (event != NULL) {
    event->data[0] = 121;
  }
}

/* Reaction 180 (481530): hold 28 visits; 481670 clears the mirror bit
 * and turns the body perpendicular to the pair, then VM 4ea290. */
static void second_turn(Fighter *victim) {
  victim->body.angle[AXIS_Y] = -0x191eb;
  reptile_entry_retail_script_004ea290(victim);
}

void reptile_second_victim(Fighter *victim) {
  fighter_start_move(victim, S_KNOCKDOWN, &victim_pose);
  victim->flags |= FF_BUSY | FF_UNTOUCHABLE | FF_REACTING;
  native_hold(victim, 28, second_turn);
}

static void second_tear(Fighter *victim) {
  /* 481b80/481c10: closed share2 part (406ce0), ten times the saved
   * drift without facing, Z spin 0x1999, forty-five visits. */
  int index = victim->native_variant++;
  require(index >= 0 && (size_t)index < sizeof torn_joints / sizeof torn_joints[0], "Invalid Reptile tear phase");
  q16 velocity[3] = {victim->saved_motion[AXIS_X] * 10, 0, victim->saved_motion[AXIS_Z] * 10};
  ScenePiece *piece = battle_detach_piece(victim, torn_joints[index], velocity, (q16[3]){0, 0, 0x1999});
  if (piece != NULL && victim->assets != NULL) {
    piece->model = &victim->assets->scene_models.data[SCENE_MODEL_WIND_PIECES + index];
    piece->bone = 0;
    piece->lifetime = 45;
  }
}

static void second_flayed(Fighter *victim) {
  /* 481880: stop_motion and 41d770 install the share2 flesh rig 50c444,
   * then 4ea2d0 jumps to the shared fall 4ea2a8. */
  native_motion_stop(victim);
  mk4_script_stop_body(victim);
  memset(victim->hidden_joints, 0, sizeof victim->hidden_joints);
  if (victim->assets != NULL) {
    victim->scene_model = &victim->assets->scene_models.data[SCENE_MODEL_FLAYED_BODY];
  }
  reptile_entry_retail_script_004ea2a8(victim);
}

/* 481590: save the -0x28f drift, stop motion and run 4818f0, one joint
 * every five visits; the last tear is followed by five more visits. */
void reptile_second_melt(Fighter *victim) {
  victim->saved_motion[AXIS_X] = victim->body.vel[AXIS_X];
  victim->saved_motion[AXIS_Z] = victim->body.vel[AXIS_Z];
  mk4_script_stop_body(victim);
  int count = (int)(sizeof torn_joints / sizeof torn_joints[0]);
  int pose = fighter_sample(victim);
  victim->native_variant = 0;
  native_begin(victim, 1, count * 5, second_flayed);
  for (int visit = 1; visit <= count * 5; ++visit) {
    native_frame(victim, visit, pose, (visit - 1) % 5 == 0 ? second_tear : NULL);
  }
  native_play(victim, false);
}

void reptile_second_face(Fighter *fighter) {
  /* Opcode 64 -> 490bd0: face the peer again. */
  fighter->facing = fighter->body.pos[AXIS_X] <= fighter_opponent(fighter)->body.pos[AXIS_X] ? 1 : -1;
}

void reptile_second_event(Fighter *fighter, int selector) {
  require(g_battle.phase == PHASE_FATALITY && selector == 63, "Unexpected Reptile second scene event");
  /* 4315b0 queues scene orchestration; the finisher owns its input lock. */
  fighter->flags |= FF_BUSY | FF_UNTOUCHABLE;
}

static void second_complete(Fighter *victim) {
  mk4_script_stop_body(victim);
  battle_end_finisher();
}

/* 4817c0 lowers the body by .08 and waits 60 visits before 4314e0 ends
 * the scene; its raw audio controls are orchestration, not samples. */
void reptile_second_finish(Fighter *victim) {
  mk4_script_stop_body(victim);
  victim->body.pos[AXIS_Y] -= 0x147a;
  native_hold(victim, 60, second_complete);
}

void reptile_fatality_2(Fighter *fighter) {
  if (!retail_can_finish_range(fighter) || !battle_begin_finisher(fighter, REPTILE_EVENT_FATALITY_2)) {
    return;
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_move(victim, S_KNOCKDOWN, &victim_pose);
  victim->flags |= FF_BUSY | FF_UNTOUCHABLE;
  fighter_start_move(fighter, S_PUNCH, &owner_pose);
  fighter->flags |= FF_BUSY | FF_UNTOUCHABLE;
  fighter->sample_override = 0;
  reptile_entry_retail_script_004dff08(fighter);
}
