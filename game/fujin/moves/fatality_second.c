#include "../character.h"

static const MoveDef owner_pose = {.clip = 291, .last = 72, .flags = SF_LOCK | SF_RECORD, .action = 0x700};
static const MoveDef victim_pose = {.clip = 13, .last = 55, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = 0x700};
/*4818f0 reads4ea420 in this order, separating one joint every five visits. */
static const int torn_joints[] = {2, 10, 9, 8, 7, 6, 5, 3, 12, 11, 14, 13, 16, 15};

bool fujin_can_second_fatality(const Fighter *fighter) {
  float distance = fighter_distance(fighter);
  return battle_can_finish(fighter) && distance >= q16_float(0x1cccc) && distance <= 3.f;
}

static void second_complete(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  battle_end_finisher();
}

void fujin_second_finish(Fighter *victim) {
  /*4817c0 lowers the body by.08 and waits60. Its raw audio control61
   *is orchestration rather than an indexed sample selector. */
  mk4_script_stop_body(victim);
  victim->body.pos[AXIS_Y] -= 0x147a;
  native_hold(victim, 60, second_complete);
}

void fujin_second_face(Fighter *fighter) {
  /*64 ->490bd0: correct the actor's facing relative to its peer. */
  fighter->facing = fighter->body.pos[AXIS_X] <= fighter_opponent(fighter)->body.pos[AXIS_X] ? 1 : -1;
}

void fujin_second_event(Fighter *fighter, int selector) {
  require(g_battle.phase == PHASE_FATALITY && selector == 63, "Unexpected Fujin second scene event");
  /*4315b0 queues scene orchestration; our finisher owns its input lock. */
  fighter->flags |= FF_BUSY | FF_UNTOUCHABLE;
}

void fujin_second_blood(Fighter *fighter, int task_type) {
  require(task_type == 38, "Unexpected wind fatality decoration task");
  const Fighter *victim = fighter_opponent(fighter);
  Vec3 point = {fighter_x(victim), q16_float(victim->body.pos[AXIS_Y]), fighter_z(victim)};
  battle_blood_hit(fighter, victim, point, 0x1eb8);
}

static void second_voice(Fighter *victim) {
  (void)victim;
  battle_sound(SOUND_C, 0x3f);
}

static void second_tear_joint(Fighter *victim) {
  int index = victim->native_variant++;
  require(index >= 0 && (size_t)index < sizeof torn_joints / sizeof torn_joints[0], "Invalid wind tear phase");
  q16 velocity[3] = {victim->saved_motion[AXIS_X] * 10 * victim->facing, 0, victim->saved_motion[AXIS_Z] * 10};
  ScenePiece *piece = battle_detach_piece(victim, torn_joints[index], velocity, (q16[3]){0});
  if (piece != NULL) {
    /*406ce0 exchanges the detached joint for its closed share2 part
     *from4ea420, retaining the original joint transform. */
    piece->model = &victim->assets->scene_models.data[SCENE_MODEL_WIND_PIECES + index];
    piece->bone = 0;
    piece->gravity = 0x1999;
    piece->lifetime = 45;
  }
}

static void second_flayed(Fighter *victim) {
  /*481880 ->41d770 swaps the now-torn skin for the complete flesh body,
   *then enters550. Detached pieces retain their authored closed part geometry. */
  native_motion_stop(victim);
  mk4_script_stop_body(victim);
  memset(victim->hidden_joints, 0, sizeof victim->hidden_joints);
  if (victim->assets != NULL) {
    victim->scene_model = &victim->assets->scene_models.data[SCENE_MODEL_FLAYED_BODY];
  }
  fujin_entry_retail_script_004ea2a8(victim);
}

void fujin_second_burst(Fighter *victim) {
  /*4816f0 saves drift;4818f0 schedules14 skin joints five visits apart.
   *The independent wind oscillation remains active until replacement. */
  victim->saved_motion[AXIS_X] = victim->body.vel[AXIS_X];
  victim->saved_motion[AXIS_Z] = victim->body.vel[AXIS_Z];
  victim->native_variant = 0;
  int visits = (int)(sizeof torn_joints / sizeof torn_joints[0]) * 5;
  native_begin(victim, 1, visits, second_flayed);
  for (int visit = 1; visit <= visits; ++visit) {
    native_frame(victim, visit, 0, (visit - 1) % 5 == 0 ? second_tear_joint : NULL);
  }
  native_play(victim, false);
}

static void second_vibrate(Fighter *victim) {
  victim->body.vel[AXIS_Y] = -victim->body.vel[AXIS_Y];
}

void fujin_second_victim(Fighter *victim) {
  fighter_start_move(victim, S_KNOCKDOWN, &victim_pose);
  victim->flags |= FF_BUSY | FF_UNTOUCHABLE | FF_REACTING;
  /*481670 turns the actor perpendicular to the pair;4819b0 alternates
   *vertical speed.03 every five visits during the90-visit wind hold. */
  victim->body.angle[AXIS_Y] = -0x191eb;
  victim->body.vel[AXIS_Y] = 0x7ae;
  second_voice(victim);
  native_motion_begin(victim, 160, NULL);
  for (int visit = 0; visit < 160; ++visit) {
    FighterFn action = visit % 5 == 0 ? second_vibrate : (visit == 59 || visit == 118) ? second_voice : NULL;
    native_motion_frame(victim, visit, NULL, 0, action);
  }
  native_motion_play(victim);
  fujin_entry_retail_script_004ea2d8(victim);
}

void fujin_second_fatality(Fighter *fighter) {
  if (!fujin_can_second_fatality(fighter) || !battle_begin_finisher(fighter, FINISH_FUJIN_2)) {
    return;
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_move(victim, S_KNOCKDOWN, &victim_pose);
  victim->flags |= FF_BUSY | FF_UNTOUCHABLE;
  fighter_start_move(fighter, S_PUNCH, &owner_pose);
  fighter->flags |= FF_BUSY | FF_UNTOUCHABLE;
  fujin_entry_retail_script_004dfed0(fighter);
}
