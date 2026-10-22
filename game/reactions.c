#include "retail_layout.h"
#include "game.h"
#include "native.h"
#include "retail_native.h"
#include "retail_data.h"
#include "retail_vm.h"
#include "native_services.h"

void reptile_reaction_40(Fighter *fighter);
void reptile_second_victim(Fighter *fighter);
void fujin_bolt_victim(Fighter *fighter);
void fujin_wind_victim(Fighter *fighter);
void fujin_wind_slam_victim(Fighter *fighter);
void fujin_triple_victim(Fighter *fighter);
void fujin_grab_victim(Fighter *fighter);
void fujin_second_victim(Fighter *fighter);
void goro_quake_victim(Fighter *fighter);
void goro_kick_victim(Fighter *fighter);
void goro_stomp_victim(Fighter *fighter);
void kai_second_victim(Fighter *fighter);
#include "script.h"
#include "math_util.h"

/* Shared reaction phases used by script attack records. Source addresses
 * are in docs/scorpion/combat-core.md. These are our C continuations, not
 * an emulation of the retail task scheduler. Native entry selection is loaded. */

enum {
  FRICTION = 0xe666,   /* 0x4913a0: velocity *= 0.9 per world pump */
  FRICTION_STOP = 0x41 /* stop once vx*vx + vz*vz <= this */
};

static void neutral(Fighter *fighter) {
  if (fighter->health > 0) {
    fighter_enter_state(fighter, S_IDLE);
  } else {
    fighter_stop_mmove(fighter);
  }
}

/* 0x4913a0 group callback. */
void fighter_friction(Fighter *fighter) {
  q16 *v = fighter->body.vel;
  v[AXIS_X] = q16_mul(v[AXIS_X], FRICTION);
  v[AXIS_Z] = q16_mul(v[AXIS_Z], FRICTION);
  if (q16_mul(v[AXIS_X], v[AXIS_X]) + q16_mul(v[AXIS_Z], v[AXIS_Z]) <= FRICTION_STOP) {
    v[AXIS_X] = 0;
    v[AXIS_Z] = 0;
    fighter->body.physics = NULL;
  }
}

static void slide(Fighter *fighter, q16 speed) {
  fighter_set_speed(fighter, -speed);
  fighter->body.physics = fighter_friction;
}

/* 488ea0 -> 480fc0(2) -> 487530/570: a grounded attacker recoils
 * when the victim is in the outer ten square units of the arena. Keep
 * the attacking pose, slide back .3 with friction, wait 8+5 ticks, then
 * wait for an airborne reacting opponent before returning control. */
static void wall_recoil_buttons(Fighter *fighter) {
  if (fighter->script_phase == 0x10a || fighter->script_phase == 0x10b) {
    fighter_enter_stance(fighter, STANCE_STANDING);
  }
}

static void wall_recoil_wait(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  fighter->mmove_hold = (victim->flags & FF_REACTING) && fighter_airborne(victim);
  if (!fighter->mmove_hold) {
    fighter_neutral(fighter);
    fighter_enter_state(fighter, S_IDLE);
  }
}

static void wall_recoil(Fighter *fighter) {
  int sample = fighter_pose_sample(fighter);
  int action = fighter->action;
  fighter_upright(fighter);
  fighter->pending = NULL;
  fighter->state = S_HIT;
  fighter->ticks = 0;
  fighter->action = 0;
  fighter->script_phase = action;
  fighter->sample_override = sample;
  fighter->buttons = NULL;
  fighter->flags |= FF_REACTING | FF_BUSY;
  native_begin(fighter, 1, 13, NULL);
  for (int frame = 1; frame <= 13; ++frame) {
    native_frame(fighter, frame, sample, frame == 8 ? wall_recoil_buttons : frame == 13 ? wall_recoil_wait : NULL);
  }
  native_play(fighter, false);
  slide(fighter, 0x4ccc);
}

void battle_wall_recoil(Fighter *victim) {
  Fighter *attacker = (Fighter *)fighter_opponent(victim);
  int64_t x = victim->body.pos[AXIS_X], z = victim->body.pos[AXIS_Z];
  int64_t radius = g_battle.arena_radius;
  if (victim->body_contact && x * x + z * z >= radius * radius - 10LL * Q16_ONE * Q16_ONE && !(victim->flags & FF_MARK) && !fighter_airborne(attacker)) {
    fighter_interrupt(attacker, wall_recoil);
  }
}

/* Native preludes select original streams; script words own all playback. */
static void hit_script(Fighter *fighter, uint32_t script, FighterFn finish) {
  static const MoveDef move = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD | SF_REACTION};
  fighter_start_move(fighter, S_HIT, &move);
  retail_vm_enter(fighter, script, finish);
}

static void react_jab_air(Fighter *fighter) {
  fighter->damage_scale = RETAIL_DAMAGE_SCALE_60_PERCENT;
  if (fighter->hits >= 2) {
    battle_wall_recoil(fighter);
  }
  mk4_script_disarm_self(fighter);
  hit_script(fighter, 0x4e8b00, retail_controller_return);
}

static void react_kick_air(Fighter *fighter) {
  fighter->damage_scale = RETAIL_DAMAGE_SCALE_60_PERCENT;
  battle_wall_recoil(fighter);
  mk4_script_disarm_self(fighter);
  retail_native_sound_list(RETAIL_SOUND_KNOCKDOWN_VOICE_LIST);
  hit_script(fighter, 0x4e8a18, retail_getup_roll);
}

static void react_kick_air_impact(Fighter *fighter) {
  mk4_script_shake(fighter, 0);
  mk4_script_disarm_self(fighter);
  hit_script(fighter, 0x4e89d8, retail_getup_roll);
}

static void react_kick_hit(Fighter *fighter) {
  battle_wall_recoil(fighter);
  mk4_script_disarm_self(fighter);
  if (fighter->crouching) {
    mk4_script_indexed_sound(fighter, 1);
    fighter->damage_scale = RETAIL_DAMAGE_SCALE_60_PERCENT;
    hit_script(fighter, 0x4e9238, retail_controller_return);
    fighter->action = ACTION_RETAIL_4005;
  } else {
    hit_script(fighter, 0x4e9208, retail_controller_return);
  }
}

static void react_jab_hit(Fighter *fighter) {
  retail_native_sound_list(RETAIL_SOUND_RANDOM_GRUNT_LIST);
  bool crouch = fighter->action == ACTION_RETAIL_2001 || fighter->action == ACTION_RETAIL_2002;
  if (crouch) {
    uint32_t physics = mk4_image_u32(retail_vm_image(), RETAIL_CROUCH_REACTION_PHYSICS_WORD);
    require(retail_native_vm(fighter, physics, RETAIL_PHYSICS, 0), "Unknown crouch reaction callback");
    hit_script(fighter, RETAIL_CROUCH_REACTION_SCRIPT, retail_controller_return);
    return;
  }
  fighter->hits = maxi(0, fighter->hits - 1);
  fighter->flags &= ~(unsigned)FF_NO_GUARD;
  bool staggers = fighter->damage_total >= (fighter->human ? 0x3333 : 0x2666) || fighter->roster == 15;
  if (staggers) {
    battle_wall_recoil(fighter);
  }
  hit_script(fighter, staggers ? 0x4e97d0 : 0x4e9798, retail_controller_return);
}

static void reaction_play(Fighter *fighter, int first, int last, int final, FighterFn end) {
  native_begin(fighter, first, final, end);
  for (int frame = first; frame <= final; ++frame) {
    native_frame(fighter, frame, mini(last, frame), NULL);
  }
  native_play(fighter, false);
}

/*486f90 uses native descriptor playback, rather than a script stream. */
static void block_crouch_finish(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  retail_controller_return(fighter);
}

static void block_crouch_reverse(Fighter *fighter) {
  int first = fighter_sample(fighter);
  native_begin(fighter, 0, first, block_crouch_finish);
  for (int frame = 0; frame <= first; ++frame) {
    native_frame(fighter, frame, first - frame, NULL);
  }
  native_play(fighter, false);
}

/*486f30: repeat the original seven-pose stream, then return to input. */
void retail_block_cycle(Fighter *fighter) {
  if (fighter->native_variant > 0) {
    fighter->native_variant--;
  }
  if (fighter->native_variant > 0) {
    retail_vm_enter(fighter, 0x4e9e68, retail_controller_return);
  } else {
    mk4_script_stop_body(fighter);
    retail_controller_return(fighter);
  }
}

static void react_block(Fighter *fighter) {
  uint32_t entry = retail_reaction_native(fighter);
  q16 speed = 0;
  unsigned repeat = 2, variant = 0;
  bool recoil = true, stance = false;
  switch (entry) {
    case 0x486970:
      speed = 0xf5c;
      variant = 3;
      stance = true;
      recoil = false;
      break;
    case 0x4869c0:
      repeat = 1;
      variant = 0x10;
      break;
    case 0x4869f0:
      speed = 0x2e14;
      variant = 0x15;
      break;
    case 0x486a30:
      speed = 0x3333;
      variant = 0x16;
      stance = true;
      break;
    case 0x486a70:
      speed = 0x4000;
      repeat = 3;
      variant = 0x17;
      stance = true;
      break;
    case 0x486ab0:
      speed = 0x4000;
      repeat = 3;
      variant = 7;
      stance = true;
      break;
    case 0x486af0:
      speed = 0x3333;
      variant = 6;
      stance = true;
      break;
    case 0x486b30:
      speed = 0x3333;
      variant = 6;
      stance = true;
      recoil = false;
      break;
    case 0x486b70:
      speed = 0x2e14;
      variant = 5;
      break;
    case 0x486bb0:
      speed = 0x2666;
      variant = 4;
      break;
    case 0x486bf0:
      speed = 0xf5c;
      variant = 3;
      stance = true;
      break;
    case 0x486c40:
      speed = 0x3d70;
      repeat = 1;
      variant = 1;
      break;
    case 0x486c70:
      repeat = 1;
      recoil = false;
      break;
    case 0x486ca0:
      speed = 0x2147;
      variant = 5;
      recoil = false;
      break;
    case 0x486cd0:
      repeat = 1;
      break;
    case 0x486d00:
      recoil = false;
      break;
    default:
      fatal("Unknown block native %08x", entry);
  }
  bool crouch = fighter->crouching;
  if (stance) {
    fighter_enter_stance(fighter, STANCE_STANDING);
  }
  if (recoil) {
    battle_wall_recoil(fighter);
  }
  static const MoveDef move = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = ACTION_RETAIL_2005};
  fighter_start_move(fighter, S_BLOCK, &move);
  if (entry == 0x486970 || entry == 0x486bf0) {
    fighter->action = ACTION_RETAIL_4004;
  }
  if (entry != 0x486d00) {
    fighter_set_speed(fighter, -speed);
    uint32_t physics = mk4_image_u32(retail_vm_image(), 0x4e9e60);
    require(retail_native_vm(fighter, physics, RETAIL_PHYSICS, 0), "Unknown native block callback");
  }
  if (variant >= 16 && fighter->armed) {
    retail_native_sound_list(RETAIL_SOUND_ARMED_IMPACT_LIST);
  } else {
    if (fighter->armed || variant >= 16) {
      retail_native_sound_list(RETAIL_SOUND_UNARMED_IMPACT_LIST);
    }
    retail_native_sound_list(variant % 16 < 5 ? 0x4ea918 : 0x4ea928);
  }
  fighter->native_variant = repeat;
  if (crouch) {
    uint32_t descriptor = mk4_image_u32(retail_vm_image(), 0x53cf88 + (fighter->armed ? 4 : 0)) * 4;
    retail_vm_bind_animation(fighter, descriptor, 0);
    native_finish_clip(fighter, block_crouch_reverse);
  } else {
    retail_vm_enter(fighter, fighter->armed ? 0x4e9ec8 : 0x4e9e68, retail_controller_return);
  }
}

/* 486230/486310/486340/4863b0: six 6-tick shakes, 88-tick hold,
 * three 6-tick shakes. Pose and body stay fixed; cosmetic shake is omitted.
 * Air thaw: 4e9d38 sets gravity 0x147 and waits for landing, then 486530. */
enum {
  AIRBORNE_THAW_SCRIPT = 0x4e9d38
};

static const MoveDef frozen = {.clip = 13, .last = MOVE_END, .flags = SF_LOCK | SF_HOLD, .action = ACTION_RETAIL_1003};
static const MoveDef thaw_hop = {.clip = 310, .first = 7, .last = 18, .flags = SF_LOCK, .action = ACTION_RETAIL_1005};

/*486530 runs the shared46fd90 landing native after4e9d38. */
void retail_thaw_land(Fighter *fighter) {
  fighter->flags &= ~(unsigned)FF_NO_GUARD;
  retail_recoil_land(fighter);
}

static void frozen_thaw(Fighter *fighter) {
  fighter->appearance = APPEARANCE_NORMAL;
  if (fighter_airborne(fighter)) {
    retail_vm_enter(fighter, AIRBORNE_THAW_SCRIPT, retail_controller_return);
  } else {
    fighter->flags &= ~(unsigned)FF_NO_GUARD;
    neutral(fighter);
  }
}

static void frozen_offset(Fighter *fighter, float offset) {
  fighter_visual_offset(fighter, (q16)offset, (q16)offset);
}

static void frozen_play(Fighter *fighter, int sample) {
  native_begin(fighter, 1, 142, frozen_thaw);
  for (int frame = 1; frame <= 142; ++frame) {
    bool shake = (frame < 36 && frame % 6 < 3) || (frame >= 124 && frame < 142 && (frame - 124) % 6 < 3);
    fighter->native.poses[frame - 1] = sample;
    fighter->native.frames[frame - 1] = (mframe_t){frozen_offset, shake ? 0xccc : 0, NULL};
  }
  native_play(fighter, false);
}

static void thaw_hop_land(Fighter *fighter) {
  if (!fighter_airborne(fighter)) {
    retail_thaw_land(fighter);
  }
}

static void thaw_hop_hold(Fighter *fighter) {
  native_begin(fighter, 0, 0, NULL);
  native_frame(fighter, 0, 18, thaw_hop_land);
  native_play(fighter, false);
}

static void thaw_hop_play(Fighter *fighter) {
  native_begin(fighter, 1, 11, thaw_hop_hold);
  for (int frame = 1; frame <= 11; ++frame) {
    native_frame(fighter, frame, 7 + frame, thaw_hop_land);
  }
  native_play(fighter, false);
}

static void react_freeze(Fighter *fighter) {
  if (fighter->action == ACTION_RETAIL_1003) {
    /* 486430 calls 480f80, which switches to the opponent: double freeze
     * backfires. It does not recursively refresh this victim's timer. */
    Fighter *attacker = &g_battle.fighters[fighter == &g_battle.fighters[0] ? 1 : 0];
    battle_react(attacker, 67);
    if (fighter_airborne(fighter)) {
      fighter_start_move(fighter, S_JUMP, &thaw_hop);
      fighter->sample_override = 7;
      thaw_hop_play(fighter);
      fighter->body.vel[AXIS_Y] = -0x51e;
      fighter->body.gravity = 0x20c;
    } else {
      neutral(fighter);
    }
    return;
  }
  int clip = fighter->appearance == APPEARANCE_FROZEN ? fighter->frozen_clip : fighter_move(fighter)->clip;
  int sample = fighter_pose_sample(fighter);
  fighter_start_move(fighter, S_HIT, &frozen);
  fighter->sample_override = sample;
  frozen_play(fighter, sample);
  fighter->frozen_clip = clip;
  fighter->appearance = APPEARANCE_FROZEN;
  fighter->damage_scale = fighter->hits > 0 ? RETAIL_DAMAGE_SCALE_30_PERCENT : RETAIL_DAMAGE_SCALE_50_PERCENT;
  fighter->flags |= FF_REACTING | FF_NO_GUARD;
  fighter_visual_offset(fighter, 0xccc, 0xccc);
}

static void react_scepter_freeze(Fighter *fighter) {
  if (fighter->hits > 3) {
    /* 486210 -> 4835f0 uses the shared weapon-throw tumble. */
    retail_reaction_fall(fighter);
  } else {
    react_freeze(fighter);
  }
}

/* 482c20, reaction 120: restart one of the current block poses, otherwise
 * use 180. The old no-guard flag survives a follow-up weapon draw. */
static const MoveDef weapon_followup[] = {
    {.clip = 255, .last = 4, .flags = SF_LOCK},
    {.clip = 222, .last = 4, .flags = SF_LOCK},
    {.clip = 247, .last = 4, .flags = SF_LOCK},
    {.clip = 321, .last = 32, .flags = SF_LOCK},
    {.clip = 180, .last = 19, .flags = SF_LOCK},
};

static void weapon_followup_play(Fighter *fighter) {
  int last = fighter->assets != NULL ? (int)fighter_clip(fighter->assets, fighter)->frames.count - 1 : fighter_move(fighter)->last;
  reaction_play(fighter, 1, last, last + 1, neutral);
}

static void react_weapon_followup(Fighter *fighter) {
  int selected = 4, clip = fighter_move(fighter)->clip;
  unsigned no_guard = fighter->flags & FF_NO_GUARD;
  int action = fighter->action;
  for (int i = 0; i < 4; ++i) {
    if (weapon_followup[i].clip == clip) {
      selected = i;
    }
  }
  fighter_start_move(fighter, S_BLOCK, &weapon_followup[selected]);
  fighter->sample_override = 0;
  weapon_followup_play(fighter);
  fighter->flags |= no_guard;
  fighter->action = action;
}

/* Native entry registry. Reaction IDs and roster dispatch stay in retail data. */
FighterFn retail_engine_reaction(uint32_t entry) {
  switch (entry) {
    case 0x465a80:
      return cage_second_victim_hit;
    case 0x466b40:
      return jax_fatality_2_head;
    case 0x4814c0:
      return kai_second_victim;
    case 0x481510:
      return shinnok_second_victim;
    case 0x481530:
      return reptile_second_victim;
    case 0x4816b0:
      return fujin_second_victim;
    case 0x481ce0:
      return react_kick_air_impact;
    case 0x481d30:
      return react_kick_air;
    case 0x481d90:
      return quan_chi_flip_reaction;
    case 0x482070:
      return react_jab_air;
    case 0x4821a0:
      return tanya_weapon_reaction;
    case 0x482a10:
      return react_kick_hit;
    case 0x482c20:
      return react_weapon_followup;
    case 0x482de0:
      return kai_handstand_reaction;
    case 0x4834d0:
      return goro_quake_victim;
    case 0x483550:
      return reptile_reaction_40;
    case 0x483570:
      return goro_kick_victim;
    case 0x4835a0:
      return jarek_ball_victim;
    case 0x4836b0:
      return cage_shadow_victim;
    case 0x483b40:
      return react_jab_hit;
    case 0x484060:
      return quan_chi_steal_reaction;
    case 0x4842d0:
      return goro_stomp_victim;
    case 0x4843b0:
      return quan_chi_stomp_reaction;
    case 0x485f90:
      return quan_chi_skull_reaction;
    case 0x486210:
      return react_scepter_freeze;
    case 0x486230:
      return react_freeze;
    case 0x486710:
      return tanya_weapon_reaction;
    case 0x486770:
      return fujin_bolt_victim;
    case 0x4867b0:
      return jax_dash_victim_throw;
    case 0x486970:
      return react_block;
    case 0x4869c0:
      return react_block;
    case 0x4869f0:
      return react_block;
    case 0x486a30:
      return react_block;
    case 0x486a70:
      return react_block;
    case 0x486ab0:
      return react_block;
    case 0x486af0:
      return react_block;
    case 0x486b30:
      return react_block;
    case 0x486b70:
      return react_block;
    case 0x486bb0:
      return react_block;
    case 0x486bf0:
      return react_block;
    case 0x486c40:
      return react_block;
    case 0x486c70:
      return react_block;
    case 0x486ca0:
      return react_block;
    case 0x486cd0:
      return react_block;
    case 0x486d00:
      return react_block;
    case 0x487a10:
      return fujin_grab_victim;
    case 0x487ed0:
      return fujin_wind_victim;
    case 0x488310:
      return fujin_wind_slam_victim;
    case 0x4883f0:
      return jax_dash_victim_hit;
    case 0x488400:
      return cage_power_victim;
    case 0x488420:
      return fujin_triple_victim;
    case 0x488450:
      return fujin_triple_victim;
    case 0x488a50:
      return kai_handstand_reaction;
    case 0x488a80:
      return kai_handstand_reaction;
    default:
      return NULL;
  }
}

/* 0x48f880: the victim's move is replaced at once; velocities, gravity and
 * its physics callback are cleared and it is marked as reacting. The
 * reaction starts on the victim's next update. */
void battle_react(Fighter *victim, int reaction) {
  require(reaction >= 0, "Invalid reaction id");
  uint32_t target = retail_reaction_target(retail_vm_image(), (unsigned)reaction);
  FighterFn native = retail_reaction_entry(retail_vm_image(), (unsigned)reaction);
  q16 retained_velocity[3];
  if (target == 0x488310 || target == 0x487350) {
    memcpy(retained_velocity, victim->body.vel, sizeof retained_velocity);
  }
  victim->body_contact = false;
  victim->reaction = reaction;
  if (target != 0x486230 && target != 0x486210) {
    for (int i = 0; i < 3; ++i) {
      victim->body.angle[i] = 0;
    }
    victim->body.pos[AXIS_Y] += victim->body.root_y;
    victim->body.root_y = 0;
  }
  require(native != NULL, "Unimplemented loaded reaction native");

  for (int i = 0; i < 3; ++i) {
    victim->body.vel[i] = 0;
    victim->body.spin[i] = 0;
  }
  victim->body.gravity = 0;
  victim->body.physics = NULL;
  victim->body.on_land = NULL;
  fighter_interrupt(victim, native);
  /* Native488310 changes gravity on the existing vortex motion. Preserve
   * that motion across cancellation of the previous portable frame action. */
  if (target == 0x488310 || target == 0x487350) {
    memcpy(victim->body.vel, retained_velocity, sizeof retained_velocity);
  }
  victim->flags |= FF_REACTING;
}
