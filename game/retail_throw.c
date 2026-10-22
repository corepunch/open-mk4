#include "retail_layout.h"
#include "retail_native.h"
#include "retail_data.h"
#include "retail_vm.h"
#include "script.h"
#include "native.h"
#include "native_services.h"

/* Native throw-victim entries are selected by the original reaction table,
 * independently of either fighter's model or the originating script. */
static void victim_begin(Fighter *fighter) {
  static const MoveDef move = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = ACTION_RETAIL_4003};
  int clip = fighter_move(fighter)->clip;
  int sample = fighter_sample(fighter);
  fighter_start_move(fighter, S_KNOCKDOWN, &move);
  if (clip >= 0) {
    retail_vm_bind_animation(fighter, RETAIL_ANIMATION_TABLE + (uint32_t)clip * RETAIL_ANIMATION_DESCRIPTOR_SIZE, sample);
  }
  mk4_script_touchable(fighter);
}

static void victim_ground_turn(Fighter *fighter) {
  victim_begin(fighter);
  fighter->body.pos[AXIS_Y] = fighter->body.ground;
  fighter->facing = -fighter->facing;
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_VICTIM_GROUND_TURN, mk4_mmove_thrown_finish);
}

void retail_native_sound_list(uint32_t address) {
  const Mk4Image *image = battle_data();
  unsigned count = mk4_image_u32(image, address);
  require(count > 0 && count <= 32, "Invalid native sound token list");
  battle_sound(SOUND_TOKEN, (int)mk4_image_u32(image, address + 4 + 4 * battle_random(count)));
}

void retail_impact_voice(Fighter *fighter) {
  (void)fighter;
  retail_native_sound_list(RETAIL_SOUND_IMPACT_VOICE_LIST);
}

void retail_throw_grunt(void) {
  retail_native_sound_list(RETAIL_SOUND_THROW_GRUNT_LIST);
}

static void victim_friction_impact(Fighter *fighter) {
  victim_begin(fighter);
  retail_vm_bind_animation(fighter, MK4_RETAIL_ANIMATION_DESCRIPTOR_CLIP_168, 0);
  fighter->body.physics = fighter_friction;
  retail_impact_voice(fighter);
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_VICTIM_FRICTION_IMPACT, mk4_mmove_thrown_finish);
}

static void victim_saved_motion(Fighter *fighter) {
  victim_begin(fighter);
  retail_vm_bind_animation(fighter, MK4_RETAIL_ANIMATION_DESCRIPTOR_CLIP_177, 0);
  mk4_script_save_motion(fighter);
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_VICTIM_SAVED_MOTION, mk4_mmove_thrown_finish);
}

static void victim_disarm_fall(Fighter *fighter) {
  mk4_script_disarm_self(fighter);
  victim_begin(fighter);
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_VICTIM_DISARM_FALL, mk4_mmove_thrown_finish);
}

static void victim_disarm_raise(Fighter *fighter) {
  mk4_script_disarm_self(fighter);
  victim_begin(fighter);
  fighter->body.pos[AXIS_Y] -= 0x4ccc;
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_VICTIM_DISARM_RAISE, mk4_mmove_thrown_finish);
}

static void flip_getup(Fighter *fighter) {
  fighter->facing = -fighter->facing;
  mk4_mmove_getup(fighter, MK4_RETAIL_GETUP_DESCRIPTOR_FLIP_GETUP);
}

static void victim_pause_getup(Fighter *fighter) {
  native_hold(fighter, 10, flip_getup);
}

static void victim_finish_flip_getup(Fighter *fighter) {
  mk4_script_disarm_self(fighter);
  victim_begin(fighter);
  const Mk4Image *image = battle_data();
  retail_vm_bind_animation(fighter, mk4_image_u32(image, MK4_RETAIL_ANIMATION_DESCRIPTOR_VICTIM_FINISH_FLIP_GETUP) * 4, 0);
  native_finish_clip(fighter, victim_pause_getup);
}

static void roll_getup(Fighter *fighter) {
  mk4_mmove_getup(fighter, MK4_RETAIL_GETUP_DESCRIPTOR_ROLL_GETUP);
}

static void victim_pause_roll(Fighter *fighter) {
  native_hold(fighter, 8, roll_getup);
}

static void victim_impact_finish(Fighter *fighter) {
  battle_scene_impact(fighter);
  native_finish_clip(fighter, victim_pause_roll);
}

static void victim_impact_roll_getup(Fighter *fighter) {
  victim_begin(fighter);
  const Mk4Image *image = battle_data();
  retail_vm_bind_animation(fighter, mk4_image_u32(image, MK4_RETAIL_ANIMATION_DESCRIPTOR_VICTIM_IMPACT_ROLL_GETUP) * 4, 0);
  native_play_rows(fighter, 45, 1, victim_impact_finish);
}

static void victim_slide(Fighter *fighter) {
  fighter_set_speed(fighter, -0x3333);
  fighter->body.physics = fighter_friction;
  native_finish_clip(fighter, mk4_mmove_thrown_finish);
}

static void victim_pause_slide(Fighter *fighter) {
  victim_begin(fighter);
  native_hold(fighter, 2, victim_slide);
}

static void victim_stance_blend(Fighter *fighter) {
  const Mk4Image *image = battle_data();
  int clip = retail_stance_clip(image, (unsigned)fighter->roster, false);
  unsigned count = mk4_image_u32(image, RETAIL_ANIMATION_TABLE + (uint32_t)clip * RETAIL_ANIMATION_DESCRIPTOR_SIZE + RETAIL_ANIMATION_FRAME_COUNT_OFFSET);
  require(count > 0 && count <= 65535, "Invalid native stance animation length");
  mk4_mmove_blend_to(fighter, clip, (int)count, 0, 0x3333, 0, mk4_mmove_thrown_finish);
}

static void victim_slide_blend(Fighter *fighter) {
  fighter_set_speed(fighter, -0x3333);
  fighter->body.physics = fighter_friction;
  native_finish_clip(fighter, victim_stance_blend);
}

static void victim_slide_to_stance(Fighter *fighter) {
  mk4_script_disarm_self(fighter);
  victim_begin(fighter);
  native_hold(fighter, 1, victim_slide_blend);
}

/* 485c60 is shared by the LP victim entries. Pair/world coordinates are
 * owned by the engine; 485d00's render-coordinate rebasing is unnecessary. */
/* The three table releases currently share descriptor173 and callback485c40.
 * Their native entries remain distinct: the rebased variant runs485d00;
 * the scatter variant runs43a5c0 for a Goro thrower. Our world coordinates
 * already supply the rebased position. The other four releases are scripts
 * with distinct slide/impact paths; none of that playback is authored here. */
void retail_lp_victim(Fighter *fighter) {
  uint32_t entry = retail_reaction_target(battle_data(), (unsigned)fighter->reaction);
  if (entry == RETAIL_NATIVE_LP_VICTIM_WEAPON_SCATTER_RELEASE && fighter_peer(fighter)->roster == 15) {
    retail_disturb_resting_weapons();
  }
  mk4_script_disarm_self(fighter);
  victim_begin(fighter);
  retail_throw_grunt();
  mk4_script_shake(fighter, 0);
  mk4_script_roster_sound(fighter, 6);
  mk4_script_stop_body(fighter);
  fighter->body.pos[AXIS_Y] = fighter->body.ground;
  switch (entry) {
    case RETAIL_NATIVE_LP_VICTIM_TABLE_RELEASE:
      native_animation_table(fighter, MK4_RETAIL_ANIMATION_ROWS_LP_VICTIM_TABLE_RELEASE);
      break;
    case RETAIL_NATIVE_LP_VICTIM_REBASED_TABLE_RELEASE:
      native_animation_table(fighter, MK4_RETAIL_ANIMATION_ROWS_LP_VICTIM_REBASED_RELEASE);
      break;
    case RETAIL_NATIVE_LP_VICTIM_WEAPON_SCATTER_RELEASE:
      native_animation_table(fighter, MK4_RETAIL_ANIMATION_ROWS_LP_VICTIM_WEAPON_SCATTER_RELEASE);
      break;
    case RETAIL_NATIVE_LP_VICTIM_TWO_SPEED_SLIDE_RELEASE:
      retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_LP_VICTIM_TWO_SPEED_SLIDE_RELEASE, mk4_mmove_thrown_finish);
      break;
    case RETAIL_NATIVE_LP_VICTIM_IMPACT_RELEASE:
      retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_LP_VICTIM_IMPACT_RELEASE, mk4_mmove_thrown_finish);
      break;
    case RETAIL_NATIVE_LP_VICTIM_SLIDING_IMPACT_RELEASE:
      retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_LP_VICTIM_SLIDING_IMPACT_RELEASE, mk4_mmove_thrown_finish);
      break;
    case RETAIL_NATIVE_LP_VICTIM_SLIDE_RELEASE:
      retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_LP_VICTIM_SLIDE_RELEASE, mk4_mmove_thrown_finish);
      break;
    default:
      require(false, "Unexpected native LP victim entry");
  }
}

void retail_script_release_reaction(Fighter *fighter, int reaction) {
  battle_react(mk4_script_release_pair(fighter), reaction);
}

static void victim_finish_captured_clip(Fighter *fighter) {
  victim_begin(fighter);
  mk4_script_stop_body(fighter);
  retail_vm_bind_animation(fighter, MK4_RETAIL_ANIMATION_DESCRIPTOR_CLIP_586, 65);
  native_finish_clip(fighter, mk4_mmove_thrown_finish);
}

static void lk_impact_finish(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  battle_scene_impact(fighter);
  native_finish_clip(fighter, mk4_mmove_thrown_finish);
}

static void lk_inherited_finish(Fighter *fighter) {
  native_finish_clip(fighter, mk4_mmove_thrown_finish);
}

void retail_lk_victim(Fighter *fighter) {
  uint32_t entry = retail_reaction_target(battle_data(), (unsigned)fighter->reaction);
  int sample = fighter_sample(fighter);
  victim_begin(fighter);
  switch (entry) {
    case RETAIL_NATIVE_LK_VICTIM_CLIP_203_RECOVERY:
      retail_vm_bind_animation(fighter, MK4_RETAIL_ANIMATION_DESCRIPTOR_CLIP_203, sample);
      native_play_rows(fighter, mini(fighter_move(fighter)->last, sample + 7), 1, lk_inherited_finish);
      break;
    case RETAIL_NATIVE_LIU_KANG_LK_THROW_VICTIM_REACTION_80:
      retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_LIU_KANG_LK_THROW_VICTIM, mk4_mmove_thrown_finish);
      break;
    case RETAIL_NATIVE_SONYA_LEG_GRAB_VICTIM_RECOVERY:
      retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_SONYA_LEG_GRAB_VICTIM_RECOVERY, mk4_mmove_thrown_finish);
      break;
    case RETAIL_NATIVE_LK_VICTIM_EFFECT_SLIDE:
      mk4_script_effect(fighter, 0);
      retail_vm_bind_animation(fighter, MK4_RETAIL_ANIMATION_DESCRIPTOR_CLIP_169, 0);
      retail_vm_bind_animation(fighter, mk4_image_u32(battle_data(), MK4_RETAIL_ANIMATION_DESCRIPTOR_LK_VICTIM_SLIDE) * 4, 0);
      fighter_set_speed(fighter, (q16)mk4_image_u32(battle_data(), MK4_RETAIL_DATA_LK_VICTIM_SLIDE_SPEED));
      native_play_rows(fighter, mini(fighter_move(fighter)->last, 26), 1, lk_impact_finish);
      break;
    default:
      require(false, "Unexpected native LK victim entry");
  }
}

FighterFn retail_throw_reaction(const Mk4Image *image, unsigned reaction) {
  switch (retail_reaction_target(image, reaction)) {
    case RETAIL_NATIVE_LIU_KANG_LK_THROW_VICTIM_REACTION_80:
    case RETAIL_NATIVE_LK_VICTIM_CLIP_203_RECOVERY:
    case RETAIL_NATIVE_SONYA_LEG_GRAB_VICTIM_RECOVERY:
    case RETAIL_NATIVE_LK_VICTIM_EFFECT_SLIDE:
      return retail_lk_victim;
    case RETAIL_NATIVE_LP_VICTIM_TABLE_RELEASE:
    case RETAIL_NATIVE_LP_VICTIM_REBASED_TABLE_RELEASE:
    case RETAIL_NATIVE_LP_VICTIM_WEAPON_SCATTER_RELEASE:
    case RETAIL_NATIVE_LP_VICTIM_TWO_SPEED_SLIDE_RELEASE:
    case RETAIL_NATIVE_LP_VICTIM_IMPACT_RELEASE:
    case RETAIL_NATIVE_LP_VICTIM_SLIDING_IMPACT_RELEASE:
    case RETAIL_NATIVE_LP_VICTIM_SLIDE_RELEASE:
      return retail_lp_victim;
    case RETAIL_NATIVE_VICTIM_FINISH_CAPTURED_CLIP:
      return victim_finish_captured_clip;
    case RETAIL_NATIVE_SLAM_REACTION:
      return retail_slam_reaction;
    case RETAIL_NATIVE_AIR_VICTIM_FLIP:
      return retail_air_victim_flip;
    case RETAIL_NATIVE_AIR_VICTIM_DEFAULT:
      return retail_air_victim_default;
    case RETAIL_NATIVE_AIR_VICTIM_LIFT:
      return retail_air_victim_lift;

    case RETAIL_NATIVE_VICTIM_FINISH_FLIP_GETUP:
      return victim_finish_flip_getup;
    case RETAIL_NATIVE_VICTIM_IMPACT_ROLL_GETUP:
      return victim_impact_roll_getup;
    case RETAIL_NATIVE_VICTIM_PAUSE_SLIDE:
      return victim_pause_slide;
    case RETAIL_NATIVE_VICTIM_SLIDE_TO_STANCE:
      return victim_slide_to_stance;
    case RETAIL_NATIVE_VICTIM_GROUND_TURN:
      return victim_ground_turn;
    case RETAIL_NATIVE_VICTIM_FRICTION_IMPACT:
      return victim_friction_impact;
    case RETAIL_NATIVE_VICTIM_SAVED_MOTION:
      return victim_saved_motion;
    case RETAIL_NATIVE_VICTIM_DISARM_FALL:
      return victim_disarm_fall;
    case RETAIL_NATIVE_VICTIM_DISARM_RAISE:
      return victim_disarm_raise;
    default:
      return NULL;
  }
}

void retail_throw_victim_continue(Fighter *fighter) {
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_THROW_VICTIM_CONTINUE, mk4_mmove_thrown_finish);
}

void retail_throw_victim_impact(Fighter *fighter) {
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_THROW_VICTIM_IMPACT, mk4_mmove_thrown_finish);
}

/* 42b930 is an independent ten-visit native task. The portable arena uses
 * its circular boundary and current camera heading for the edge predicate. */
static bool pair_drift(BattleEvent *event, Fighter *owner, Fighter *peer) {
  if (event->phase++ == 0) {
    Fighter *first = &g_battle.fighters[0];
    event->data[0] = -q16_mul(first->body.pos[AXIS_X], 0x7ae);
    event->data[1] = -q16_mul(first->body.pos[AXIS_Z], 0x7ae);
  } else {
    event->data[0] = q16_mul(event->data[0], 0xe666);
    event->data[1] = q16_mul(event->data[1], 0xe666);
  }
  if (event->phase > 10) {
    mk4_script_stop_body(owner);
    mk4_script_stop_body(peer);
    return false;
  }
  owner->body.vel[AXIS_X] = peer->body.vel[AXIS_X] = event->data[0];
  owner->body.vel[AXIS_Z] = peer->body.vel[AXIS_Z] = event->data[1];
  battle_event_delay(event, 1);
  return true;
}

static void pair_edge_task(Fighter *fighter) {
  const Fighter *peer = fighter_opponent(fighter);
  double actor = fighter->body.pos[AXIS_X] * g_battle.view.z - fighter->body.pos[AXIS_Z] * g_battle.view.x;
  double other = peer->body.pos[AXIS_X] * g_battle.view.z - peer->body.pos[AXIS_Z] * g_battle.view.x;
  if (actor >= other && actor > g_battle.arena_radius - 10 * Q16_ONE) {
    BattleEvent *event = battle_event_start(fighter, pair_drift, false, false);
    if (event != NULL) {
      battle_event_delay(event, 1);
    }
  }
}

/* Each case is an original native routine's script-enter operand. Fighter
 * selection itself comes exclusively from the loaded4ddfb8 table. */
static uint32_t lk_script(uint32_t native) {
  switch (native) {
    case RETAIL_NATIVE_CAGE_LK_THROW_ENTRY:
      return MK4_RETAIL_SCRIPT_CAGE_LK_THROW_STARTUP;
    case RETAIL_NATIVE_JAREK_LK_THROW_ENTRY:
      return MK4_RETAIL_SCRIPT_JAREK_LK_THROW_STARTUP;
    case RETAIL_NATIVE_TANYA_LK_THROW_ENTRY:
      return MK4_RETAIL_SCRIPT_TANYA_LK_THROW_STARTUP;
    case RETAIL_NATIVE_SHINNOK_LK_THROW_ENTRY:
      return MK4_RETAIL_SCRIPT_SHINNOK_LK_THROW_STARTUP;
    case RETAIL_NATIVE_REPTILE_LK_THROW_ENTRY:
      return MK4_RETAIL_SCRIPT_REPTILE_LK_THROW_STARTUP;
    case RETAIL_NATIVE_REIKO_LK_THROW_ENTRY:
      return MK4_RETAIL_SCRIPT_REIKO_LK_THROW_STARTUP;
    case RETAIL_NATIVE_QUAN_CHI_LK_THROW_ENTRY:
      return MK4_RETAIL_SCRIPT_QUAN_CHI_LK_THROW_STARTUP;
    case RETAIL_NATIVE_RAIDEN_LK_THROW_ENTRY:
      return MK4_RETAIL_SCRIPT_RAIDEN_LK_THROW_STARTUP;
    case RETAIL_NATIVE_KAI_LK_THROW_ENTRY:
      return MK4_RETAIL_SCRIPT_KAI_LK_THROW_STARTUP;
    case RETAIL_NATIVE_FUJIN_LK_THROW_ENTRY:
      return MK4_RETAIL_SCRIPT_FUJIN_LK_THROW_STARTUP;
    case RETAIL_NATIVE_SCORPION_LK_THROW_ENTRY:
      return MK4_RETAIL_SCRIPT_SCORPION_LK_THROW_STARTUP;
    case RETAIL_NATIVE_SONYA_LK_THROW_ENTRY:
      return MK4_RETAIL_SCRIPT_SONYA_LK_THROW_STARTUP;
    case RETAIL_NATIVE_LIU_KANG_LK_THROW_ENTRY:
      return MK4_RETAIL_SCRIPT_LIU_KANG_LK_THROW_STARTUP;
    case RETAIL_NATIVE_SUB_ZERO_LK_THROW_ENTRY:
      return MK4_RETAIL_SCRIPT_SUB_ZERO_LK_THROW_STARTUP;
    case RETAIL_NATIVE_GORO_LK_THROW_ENTRY:
      return MK4_RETAIL_SCRIPT_GORO_LK_THROW_STARTUP;
    default:
      require(false, "Unimplemented native LK entry");
      return 0;
  }
}

void retail_lk_throw(Fighter *fighter) {
  if (!mk4_can_ground_throw(fighter, true)) {
    fighter_ready(fighter);
    return;
  }
  const Mk4Image *image = battle_data();
  uint32_t native = retail_lk_target(image, (unsigned)fighter->roster);
  uint32_t script = lk_script(native);
  retail_throw_capture(fighter, ACTION_RETAIL_0501, native != RETAIL_NATIVE_SUB_ZERO_LK_THROW_ENTRY);
  if (native == RETAIL_NATIVE_SUB_ZERO_LK_THROW_ENTRY) {
    fighter_set_speed(fighter, -0x1999);
    mk4_script_save_motion(fighter);
  } else if (native == RETAIL_NATIVE_LIU_KANG_LK_THROW_ENTRY) {
    Fighter *peer = (Fighter *)fighter_opponent(fighter);
    peer->body.pos[AXIS_X] += q16_mul(fighter_axis(fighter, AXIS_X), 0xcccc);
    peer->body.pos[AXIS_Z] += q16_mul(fighter_axis(fighter, AXIS_Z), 0xcccc);
  } else if (native == RETAIL_NATIVE_QUAN_CHI_LK_THROW_ENTRY) {
    pair_edge_task(fighter);
  } else if (native == RETAIL_NATIVE_GORO_LK_THROW_ENTRY) {
    battle_sound(SOUND_TOKEN, 0x654);
  }
  retail_vm_enter(fighter, script, fighter_ready);
}

void retail_throw_pair_start(Fighter *fighter) {
  require((fighter->flags & FF_PAIRED) && (fighter_opponent(fighter)->flags & FF_PAIRED), "Native42b680 needs its acquired peer");
  fighter->action = ACTION_RETAIL_0501;
}

static void idle_pose(Fighter *fighter) {
  const Mk4Image *image = battle_data();
  int clip = retail_stance_clip(image, (unsigned)fighter->roster, false);
  retail_vm_bind_animation(fighter, RETAIL_ANIMATION_TABLE + (uint32_t)clip * RETAIL_ANIMATION_DESCRIPTOR_SIZE, 0);
}

void retail_lk_return(Fighter *fighter) {
  idle_pose(fighter);
  mk4_mmove_thrown_finish(fighter);
}

void retail_lk_pause_long(Fighter *fighter) {
  idle_pose(fighter);
  retail_vm_wait_native(fighter, 80, mk4_mmove_thrown_finish);
}

void retail_lk_pause_short(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  idle_pose(fighter);
  retail_vm_wait_native(fighter, 40, mk4_mmove_thrown_finish);
}

void retail_lk_jump_back(Fighter *fighter) {
  retail_vm_bind_animation(fighter, MK4_RETAIL_ANIMATION_DESCRIPTOR_CLIP_301, 0);
  mk4_jump_backward(fighter);
}

void retail_lk_recoil(Fighter *fighter) {
  idle_pose(fighter);
  retail_vm_wait_native(fighter, 1, mk4_mmove_thrown_finish);
}

void retail_lk_branch_continue(Fighter *fighter) {
  pair_edge_task(fighter);
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_LK_BRANCH_CONTINUE, fighter_ready);
}

void retail_lk_victim_return(Fighter *fighter) {
  const Mk4Image *image = fighter->execution.script.image;
  retail_vm_bind_animation(fighter, mk4_image_u32(image, MK4_RETAIL_ANIMATION_DESCRIPTOR_LK_VICTIM_RETURN) * 4, 0);
  retail_vm_forward_return(fighter);
}

void retail_lk_rock_repeat_one(Fighter *fighter) {
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_LK_THROW_ROCK_REPEAT_ONE, fighter_ready);
}

void retail_lk_rock_repeat_two(Fighter *fighter) {
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_LK_THROW_ROCK_REPEAT_TWO, fighter_ready);
}

void retail_lk_rock_release(Fighter *fighter) {
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_LK_THROW_ROCK_RELEASE, fighter_ready);
}

void retail_lp_align_and_release(Fighter *fighter) {
  require(retail_native_vm(fighter, RETAIL_NATIVE_LIU_KANG_LIU_SCRIPT_FATAL_ALIGN, RETAIL_CALL, 0), "Missing shared pair alignment native");
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_LP_SHARED_RELEASE, fighter_ready);
}

/* Native42cbc0/42ce60/42ce80 return through the engine controller boundary.
 * Pair-space coordinates belong to simulation rather than retail render state. */
void retail_throw_ready(Fighter *fighter) {
  if (fighter->flags & FF_PAIRED) {
    fighter->flags &= ~(unsigned)FF_PAIRED;
    fighter_peer(fighter)->flags &= ~(unsigned)FF_PAIRED;
  }
  mk4_script_stop_body(fighter);
  fighter_execution_finish(fighter, fighter->health > 0 ? fighter_ready : NULL);
}

/* 42cc40 runs the peer callback cell4de664, then the original owner words. */
void retail_lp_end(Fighter *fighter) {
  retail_throw_turn(fighter_peer(fighter));
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_LP_END, fighter_ready);
}

/* 470420/48e080 releases native callbacks and restores ordinary control.
 * Airborne motion belongs to physics; controller return must preserve it. */
void retail_controller_return(Fighter *fighter) {
  if (fighter->flags & FF_PAIRED) {
    fighter->flags &= ~(unsigned)FF_PAIRED;
    fighter_peer(fighter)->flags &= ~(unsigned)FF_PAIRED;
  }
  native_motion_stop(fighter);
  fighter->body.physics = NULL;
  fighter->body.on_land = NULL;
  retail_face_peer(fighter);
  fighter_execution_finish(fighter, fighter->health > 0 ? fighter_ready : NULL);
}
