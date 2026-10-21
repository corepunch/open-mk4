#include "retail_vm.h"
#include "../character.h"

static const MoveDef owner_pose = {.clip = 293, .last = 19, .flags = SF_LOCK | SF_RECORD, .action = 0x700};
static const MoveDef victim_pose = {.clip = 13, .last = 55, .flags = SF_LOCK | SF_RECORD, .action = 0x700};
/* 413ce0 uses4d17b8: the additive variants of the same ten share2
 * roots as Cage's orb, attached to joint10 at.7 scale. */
static const MeshAnimation charge_mesh = {
    .first = 271, .count = 10, .period = 1, .scale = 0xb333, .source = MESH_SHARED2, .offset = {7040, 6272, 0}, .additive = true};

static void charge_step(Projectile *object) {
  if (g_battle.phase != PHASE_FATALITY) {
    object->kind = 0;
    return;
  }
  Vec3 hand;
  Mat3 unused;
  fighter_joint_position(&g_battle.fighters[object->owner], 10, &hand, &unused);
  object->pos[AXIS_X] = (q16)(hand.x * Q16_ONE);
  object->pos[AXIS_Y] = (q16)(hand.y * Q16_ONE);
  object->pos[AXIS_Z] = (q16)(hand.z * Q16_ONE);
}

static void charge_spawn(Fighter *fighter, int task_type, int lifetime) {
  require(task_type == 619, "Unexpected Cage charge task");
  Projectile *object = battle_spawn_projectile(fighter, PROJECTILE_EFFECT, lifetime, charge_step);
  if (object != NULL) {
    object->mesh_animation = &charge_mesh;
    object->initial_mesh = charge_mesh.first;
    charge_step(object);
  }
}

void cage_second_charge(Fighter *fighter, int task_type) {
  charge_spawn(fighter, task_type, 19);
}

void cage_second_flash(Fighter *fighter, int task_type) {
  charge_spawn(fighter, task_type, 8);
}

void cage_script_fatality_2_continue(Fighter *fighter) {
  /* 4658a0 -> standing dispatch ->4658d0 ->4e5880. Both actors
   * already have locked scene poses; retain this actor's frame ownership. */
  retail_vm_enter_owned(fighter, 0x4e5880, fighter_ready, 14);
}

void cage_second_repeated_uppercuts(Fighter *fighter) {
  int sample = fighter_sample(fighter), frame = 1;
  require(fighter_move(fighter)->clip == 293 && sample == 6, "Cage repeated uppercuts require their generated six-pose startup");
  native_begin(fighter, 1, 45, cage_entry_retail_script_004e58d8);
  /* 4658f0 repeats rewind-to6/advance8 three times, then finishes293. */
  for (int repeat = 0; repeat < 3; ++repeat) {
    while (sample > 6) {
      native_frame(fighter, frame++, --sample, NULL);
    }
    for (int advance = 0; advance < 8; ++advance) {
      native_frame(fighter, frame++, ++sample, NULL);
    }
  }
  while (sample < 19) {
    native_frame(fighter, frame++, ++sample, NULL);
  }
  require(frame == 46, "Cage repeated uppercut frame count differs from the native loop");
  native_play(fighter, false);
}

void cage_second_victim_pause(Fighter *fighter) {
  /* 466df0 finishes only the auxiliary victim task; the body remains
   * at its selected pose until the uppercut installs reaction185. */
  fighter_finish_mmove(fighter, NULL);
}

static void second_scene_end(Fighter *victim) {
  fighter_finish_mmove(victim, NULL);
  battle_end_finisher();
}

void cage_second_victim_finish(Fighter *fighter) {
  /* 465b10 leaves joint2 hidden, holds the fallen victim80 visits,
   * then returns to the common completed-finisher controller. */
  fighter->hidden_joints[0] |= UINT64_C(1) << 2;
  native_hold(fighter, 80, second_scene_end);
}

void cage_second_victim_hit(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter_start_move(fighter, S_KNOCKDOWN, &victim_pose);
  fighter->flags |= FF_BUSY | FF_UNTOUCHABLE | FF_REACTING;
  /* 465a80/466e10 extract joint2;465ba0 launches its independent body
   * away from the attacker at.425, up.15 with gravity.03. */
  ScenePiece *head = battle_detach_piece(fighter, 2, (q16[3]){-0x6ccc, -0x2666, 0}, (q16[3]){0});
  if (head != NULL) {
    const Fighter *owner = fighter_opponent(fighter);
    head->vel[AXIS_X] = q16_mul(fighter_axis(owner, AXIS_X), 0x6ccc);
    head->vel[AXIS_Z] = q16_mul(fighter_axis(owner, AXIS_Z), 0x6ccc);
    head->gravity = 0x7ae;
  }
  Vec3 neck;
  Mat3 unused;
  fighter_joint_position(fighter, 2, &neck, &unused);
  battle_blood_hit(fighter_opponent(fighter), fighter, neck, 0x3333);
  retail_vm_enter_owned(fighter, 0x4e5900, fighter_stop_mmove, 14);
}

void cage_fatality_2(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, CAGE_EVENT_FATALITY_2)) {
    return;
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_move(victim, S_KNOCKDOWN, &victim_pose);
  victim->flags |= FF_BUSY | FF_UNTOUCHABLE;
  retail_vm_enter_owned(victim, 0x4e5b38, fighter_ready, 14);
  fighter_start_move(fighter, S_PUNCH, &owner_pose);
  fighter->flags |= FF_BUSY | FF_UNTOUCHABLE;
  retail_vm_enter_owned(fighter, 0x4e5870, fighter_ready, 14);
}
