#include "retail_vm.h"
#include "../character.h"

/* U,U,F,F+BLK -> 467cd0 -> 43bd70 -> 4dfe90[12] = 41df20. After the shared
 * 41aaf0 preamble the owner runs generated 4d2db0 on clip 577. */
static const MoveDef owner_pose = {.clip = 577, .last = 114, .flags = SF_LOCK | SF_RECORD, .action = 0x700};
static const MoveDef victim_pose = {.clip = 13, .last = 55, .flags = SF_LOCK | SF_RECORD, .action = 0x700};
static const MoveDef victim_scene = {.clip = 182, .last = 64, .flags = SF_LOCK | SF_RECORD, .action = 0x700};
/* 41de30 -> 406dd0(50c604): share2 mesh 0, .113 forward of the node. */
static const MeshAnimation glow_mesh = {.first = 0, .count = 1, .period = 1, .scale = Q16_ONE, .source = MESH_SHARED2, .offset = {0, 0, 0x1ced}};

static void glow_step(Projectile *object) {
  if (g_battle.phase != PHASE_FATALITY) {
    object->kind = 0;
    return;
  }
  const Fighter *owner = &g_battle.fighters[object->owner];
  Vec3 head;
  Mat3 unused;
  fighter_joint_position(owner, 2, &head, &unused);
  object->pos[AXIS_X] = (q16)(head.x * Q16_ONE);
  object->pos[AXIS_Y] = (q16)(head.y * Q16_ONE);
  object->pos[AXIS_Z] = (q16)(head.z * Q16_ONE);
  object->heading[0] = owner->heading[0];
  object->heading[1] = owner->heading[1];
  object->lifetime = 2;
}

/* 41de30: bank C 11, then the 50c604 model rides the owner's joint 2. */
void jarek_second_glow(Fighter *fighter) {
  battle_sound(SOUND_C, 11);
  Projectile *glow = battle_spawn_projectile(fighter, PROJECTILE_EFFECT, 2, glow_step);
  if (glow != NULL) {
    glow->mesh_animation = &glow_mesh;
    glow->initial_mesh = glow_mesh.first;
    glow_step(glow);
  }
}

static void neck_blood(Fighter *victim) {
  Vec3 neck;
  Mat3 unused;
  fighter_joint_position(victim, 2, &neck, &unused);
  battle_blood_hit(fighter_opponent(victim), victim, neck, 0x3333);
}

static ScenePiece *victim_head(const Fighter *victim) {
  int index = victim == &g_battle.fighters[0] ? 0 : 1;
  for (int i = 0; i < MAX_SCENE_PIECES; ++i) {
    ScenePiece *piece = &g_battle.pieces[i];
    if (piece->active && piece->attached_to == index && piece->joint == 2) {
      return piece;
    }
  }
  return NULL;
}

/* 41dff0 stage 0: context sound 0x12, then 41ad60 sprays 40a470 gore 0/8
 * and separates joint 2 as its own spinning node. Stage 1 enters 4d2e10. */
static void victim_start(Fighter *victim) {
  battle_sound(SOUND_CONTEXT, 0x12);
  neck_blood(victim);
  /* The separated head stays on the neck until 41e0e0 drops it. */
  battle_attach_head(victim, victim, 2);
  fighter_start_move(victim, S_KNOCKDOWN, &victim_scene);
  victim->flags |= FF_BUSY | FF_UNTOUCHABLE | FF_REACTING;
  retail_vm_enter_owned(victim, 0x4d2e10, fighter_ready, 12);
}

/* 41dfc0: 48f880 replaces the victim task with 41dff0; the owner enters
 * 4d2de0 and keeps lasing on clip 577. */
void jarek_second_handoff(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_cancel_mmove(victim);
  victim_start(victim);
  retail_vm_enter_owned(fighter, 0x4d2de0, fighter_ready, 12);
}

/* 41e0e0: the separated head falls under gravity 0x20c, spinning about Z
 * by the victim's side (41ad60 0x147a), with gore 8/4/3 at release. */
void jarek_second_head_fall(Fighter *fighter, int task_type) {
  require(task_type == 0, "Unexpected Jarek head task");
  battle_sound(SOUND_CONTEXT, 0x12);
  battle_sound(SOUND_C, 11);
  neck_blood(fighter);
  ScenePiece *head = victim_head(fighter);
  if (head != NULL) {
    head->attached_to = -1;
    head->gravity = 0x20c;
    head->spin[2] = fighter == &g_battle.fighters[0] ? 0x147a : -0x147a;
  }
}

static void second_end(Fighter *victim) {
  fighter_finish_mmove(victim, NULL);
  if (g_battle.phase == PHASE_FATALITY) {
    battle_end_finisher();
  }
}

/* 41e0a0 -> 41f1b0 parks the fallen victim in 60-visit waits; the portable
 * scene completes after the first one. */
void jarek_second_victim_hold(Fighter *fighter) {
  native_hold(fighter, 60, second_end);
}

void jarek_fatality_2(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, JAREK_EVENT_FATALITY_2)) {
    return;
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_move(victim, S_KNOCKDOWN, &victim_pose);
  victim->flags |= FF_BUSY | FF_UNTOUCHABLE;
  /* The victim keeps its defeated stance until 41dfc0 replaces its task. */
  native_begin(victim, 0, victim_pose.last, NULL);
  for (int frame = 0; frame <= victim_pose.last; ++frame) {
    native_frame(victim, frame, frame, NULL);
  }
  native_play(victim, false);
  fighter_start_move(fighter, S_PUNCH, &owner_pose);
  fighter->flags |= FF_BUSY | FF_UNTOUCHABLE;
  retail_vm_enter_owned(fighter, 0x4d2db0, fighter_ready, 12);
}
