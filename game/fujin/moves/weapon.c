#include "../character.h"

static void weapon_hp(Fighter *fighter);
static void weapon_lp(Fighter *fighter);
static void weapon_low(Fighter *fighter);
void fujin_equip_crossbow(Fighter *fighter);
static void bow_fire(Fighter *fighter);
static void bow_followup_begin(Fighter *fighter);
static const WeaponDef crossbow = {0x68, weapon_hp, weapon_lp, weapon_hp, weapon_low, NULL, weapon_throw, fujin_equip_crossbow, SCENE_MODEL_FUJIN_WEAPON};
static const MoveDef bow_move = {.clip = 211, .last = 28, .flags = SF_LOCK | SF_RECORD, .action = 0x303};
static const MoveDef bolt_victim = {.clip = 311, .last = 18, .flags = SF_LOCK | SF_REACTION, .action = 0x4001};
static const AttackRecord bolt_record = {0x3d70, -0x1eb8, 0x7d70, 0x3ae1, 101, 10, 2, ATTACK_MARK, 0xccc};
/* Native4ed438 selects506dc8, whose sole mesh is shared106. */
static const MeshAnimation bolt_mesh = {.first = 106, .count = 1, .period = 1, .scale = Q16_ONE, .source = MESH_SHARED};

void fujin_equip_crossbow(Fighter *fighter) {
  require(fujin_weapon_descriptor[0] == crossbow.type, "Fujin crossbow must match its reviewed retail descriptor");
  fighter_equip_weapon(fighter, &crossbow);
}

static void bow_cleanup(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter->body.on_land = NULL;
}

static void bolt_step(Projectile *bolt) {
  if (g_battle.contacts_off) {
    bolt->kind = 0;
    return;
  }
  for (int axis = 0; axis < 3; ++axis) {
    bolt->pos[axis] += bolt->vel[axis];
  }
  /* Native49a110 reflects angled bolts at absolute Y=-3 and Y=0. */
  if (bolt->phase != 0 && (bolt->pos[AXIS_Y] < -3 * Q16_ONE || bolt->pos[AXIS_Y] >= 0)) {
    bolt->pos[AXIS_Y] = bolt->pos[AXIS_Y] < -3 * Q16_ONE ? -3 * Q16_ONE : 0;
    bolt->vel[AXIS_Y] = -bolt->vel[AXIS_Y];
    bolt->angle[2] = -bolt->angle[2];
  }
  int64_t x = bolt->pos[AXIS_X], z = bolt->pos[AXIS_Z], radius = g_battle.arena_radius;
  if (x * x + z * z > radius * radius + (int64_t)12 * Q16_ONE * Q16_ONE) {
    bolt->kind = 0;
    return;
  }
  if (battle_projectile_contact(bolt, &bolt_record) != CONTACT_MISS) {
    battle_sound(SOUND_C, 0x43);
    bolt->kind = 0;
  }
}

static void bow_spawn(Fighter *fighter) {
  Projectile *bolt = battle_spawn_projectile(fighter, PROJECTILE_FIREBALL, 180, bolt_step);
  if (bolt == NULL) {
    return;
  }
  int variant = fighter->native_variant;
  bolt->phase = variant == 1 || variant == 2 ? variant : 0;
  bolt->mesh_animation = &bolt_mesh;
  bolt->initial_mesh = bolt_mesh.first;
  bolt->heading[0] = fighter->saved_motion[0];
  bolt->heading[1] = fighter->saved_motion[1];
  bolt->pos[AXIS_Y] += -0x3333;
  bolt->vel[AXIS_Y] = variant == 1 ? -0x3333 : variant == 2 ? 0x3333 : 0;
  /* 49c370 adds a quarter turn to the native actor heading. In the
   * portable +X projectile basis this points shared106's -X nose forward.
   * Its pitch changes sign after that local-to-heading axis conversion. */
  bolt->angle[1] = 0x3243f;
  bolt->angle[2] = variant == 1 ? -Q16_ONE : variant == 2 ? Q16_ONE : 0;
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    int component = axis == AXIS_X ? 0 : 1;
    bolt->pos[axis] += q16_mul(bolt->heading[component], 0xccc);
    bolt->vel[axis] = q16_mul(bolt->heading[component], bolt->phase == 0 ? 0x3333 : 0x2666);
  }
}

/* Native429220 advances two poses per visit;428d30 advances to the
 * requested cursor from zero. These are native bodies, not VM transcriptions. */
static void bow_native_to(Fighter *fighter, int target, int step, FighterFn end) {
  int first = fighter_sample(fighter), distance = target - first;
  require(distance >= 0 && target <= fighter_move(fighter)->last && (step == 1 || step == 2), "Invalid Fujin crossbow playback range");
  int visits = (distance + step - 1) / step;
  native_begin(fighter, 0, visits, end);
  for (int row = 0; row <= visits; ++row) {
    int sample = first + row * step;
    native_frame(fighter, row, sample < target ? sample : target, NULL);
  }
  native_play(fighter, false);
}

static void bow_return(Fighter *fighter) {
  fighter_finish_mmove(fighter, mk4_mmove_thrown_finish);
}

void fujin_bow_finish(Fighter *fighter) {
  fighter->action = 0x1016;
  bow_native_to(fighter, fighter_move(fighter)->last, 1, bow_return);
}

static void bow_fire(Fighter *fighter) {
  bow_spawn(fighter);
  if (fighter->native_variant >= 3) {
    fujin_entry_retail_script_004ed788(fighter);
  } else if (fighter->native_variant == 0) {
    fighter->action = 0x1016;
    fujin_entry_retail_script_004ed408(fighter);
  } else {
    fujin_bow_finish(fighter);
  }
}

static void bow_curved_clip(Fighter *fighter) {
  int clip = fighter->native_variant == 1 ? 213 : 215;
  mk4_mmove_bind(fighter, clip, 30, 0);
  bow_native_to(fighter, 4, 1, bow_fire);
}

static void bow_start(Fighter *fighter, int variant) {
  mk4_script_stop_body(fighter);
  fighter_start_move(fighter, S_PUNCH, &bow_move);
  fighter->body.physics = fighter->body.on_land = NULL;
  fighter->mmove_cleanup = bow_cleanup;
  fighter->native_variant = variant;
  fighter->saved_motion[0] = fighter_axis(fighter, AXIS_X);
  fighter->saved_motion[1] = fighter_axis(fighter, AXIS_Z);
  fighter->action = variant == 1 ? 0x301 : variant == 2 ? 0x302 : 0x303;
  battle_sound(SOUND_C, 0x29);
  if (variant == 1 || variant == 2) {
    mk4_mmove_bind(fighter, variant == 1 ? 212 : 214, 10, 0);
    bow_native_to(fighter, 9, 2, bow_curved_clip);
  } else {
    mk4_mmove_bind(fighter, 211, 29, 0);
    bow_native_to(fighter, 4, 1, bow_fire);
  }
}

static void weapon_hp(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    weapon_air_frames(fighter, true, fujin_entry_retail_script_004e66d0);
  } else {
    bow_start(fighter, 1);
  }
}

static void weapon_low(Fighter *fighter) {
  bow_start(fighter, 2);
}

static void weapon_lp(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    weapon_air_frames(fighter, false, fujin_entry_retail_script_004e66f0);
  } else if (!(fighter->flags & FF_FOLLOW_UP) && mk4_can_ground_throw(fighter, false)) {
    fujin_enter_throw(fighter);
  } else {
    bow_start(fighter, fighter_holds_back(fighter) ? 2 : 0);
  }
}

/* Native4ecb40[type68-60] ->495cf0 ->4ed828[25] ->49a190:
 * three short shots, then49a3b0's full ordinary LP shot and recovery. */
void fujin_bow_followup_next(Fighter *fighter) {
  int next = fighter->native_variant + 1;
  if (next < 6) {
    bow_start(fighter, next);
  } else {
    bow_start(fighter, 0);
  }
}

static void bow_followup_begin(Fighter *fighter) {
  fighter_upright(fighter);
  fighter->body.pos[AXIS_Y] = fighter->body.ground;
  bow_start(fighter, 3);
}

void fujin_draw_equip(Fighter *fighter) {
  fujin_equip_crossbow(fighter);
  fujin_entry_retail_script_004e62d8(fighter);
}

void fujin_weapon_action(Fighter *fighter) {
  if (fighter->armed) {
    fighter->weapon->throw_move(fighter);
  } else if (fighter->flags & FF_FOLLOW_UP) {
    fujin_equip_crossbow(fighter);
    weapon_followup_victim(fighter);
    bow_followup_begin(fighter);
  } else {
    weapon_draw(fighter, &crossbow);
    fujin_entry_retail_script_004e62c0(fighter);
  }
}

void fujin_bolt_victim(Fighter *fighter) {
  mk4_script_disarm_self(fighter);
  fighter_start_move(fighter, S_HIT, &bolt_victim);
  fighter->flags |= FF_REACTING;
  fighter->mmove_cleanup = bow_cleanup;
  fujin_entry_retail_script_004e9e10(fighter);
}

void fujin_armed_idle(Fighter *fighter) {
  weapon_stance(fighter, S_IDLE);
}

void fujin_armed_walk(Fighter *fighter) {
  weapon_stance(fighter, S_WALK);
}

void fujin_armed_crouch(Fighter *fighter) {
  weapon_stance(fighter, S_CROUCH);
}

void fujin_armed_block(Fighter *fighter) {
  weapon_stance(fighter, S_BLOCK);
}
