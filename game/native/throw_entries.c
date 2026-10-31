#include "script/bindings.h"
#include "script/native_ids.h"
#include "data/layout.h"
#include "native/actions.h"
#include "data/tables.h"
#include "script/vm.h"
#include "native/helpers.h"
#include "runtime/playback.h"
#include "native/capabilities.h"

/* Native throw-victim entries are selected by the original reaction table,
 * independently of either fighter's model or the originating script. */
static void victim_begin(Fighter *fighter) {
  static const MoveDef move = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = ACTION_CODE_4003};
  int clip = fighter_move(fighter)->clip;
  int sample = fighter_sample(fighter);
  fighter_start_move(fighter, S_KNOCKDOWN, &move);
  if (clip >= 0) {
    script_vm_bind_animation(fighter, dat_animation_table + (uint32_t)clip * MK4_ANIMATION_DESCRIPTOR_SIZE, sample);
  }
  mk4_script_touchable(fighter);
}

void victim_ground_turn(Fighter *fighter) {
  victim_begin(fighter);
  fighter->body.pos[AXIS_Y] = fighter->body.ground;
  fighter->facing = -fighter->facing;
  script_vm_enter(fighter, fn_victim_ground_turn, mk4_mmove_thrown_finish);
}

void action_native_sound_list(uint32_t address) {
  const Mk4Image *image = battle_data();
  unsigned count = mk4_image_u32(image, address);
  require(count > 0 && count <= 32, "Invalid native sound token list");
  battle_sound(SOUND_TOKEN, (int)mk4_image_u32(image, address + 4 + 4 * battle_random(count)));
}

void action_impact_voice(Fighter *fighter) {
  (void)fighter;
  action_native_sound_list(dat_sound_impact_voice_list);
}

void action_throw_grunt(void) {
  action_native_sound_list(dat_sound_throw_grunt_list);
}

void victim_friction_impact(Fighter *fighter) {
  victim_begin(fighter);
  script_vm_bind_animation(fighter, dat_animation_clip_168, 0);
  fighter->body.physics = fighter_friction;
  action_impact_voice(fighter);
  script_vm_enter(fighter, fn_victim_friction_impact, mk4_mmove_thrown_finish);
}

void victim_saved_motion(Fighter *fighter) {
  victim_begin(fighter);
  script_vm_bind_animation(fighter, dat_animation_clip_177, 0);
  mk4_script_save_motion(fighter);
  script_vm_enter(fighter, fn_victim_saved_motion, mk4_mmove_thrown_finish);
}

void victim_disarm_fall(Fighter *fighter) {
  mk4_script_disarm_self(fighter);
  victim_begin(fighter);
  script_vm_enter(fighter, fn_victim_disarm_fall, mk4_mmove_thrown_finish);
}

void victim_disarm_raise(Fighter *fighter) {
  mk4_script_disarm_self(fighter);
  victim_begin(fighter);
  fighter->body.pos[AXIS_Y] -= 0x4ccc;
  script_vm_enter(fighter, fn_victim_disarm_raise, mk4_mmove_thrown_finish);
}

static void flip_getup(Fighter *fighter) {
  fighter->facing = -fighter->facing;
  mk4_mmove_getup(fighter, dat_getup_descriptor_flip_getup);
}

static void victim_pause_getup(Fighter *fighter) {
  native_hold(fighter, 10, flip_getup);
}

void victim_finish_flip_getup(Fighter *fighter) {
  mk4_script_disarm_self(fighter);
  victim_begin(fighter);
  const Mk4Image *image = battle_data();
  script_vm_bind_animation(fighter, mk4_image_u32(image, dat_animation_descriptor_victim_finish_flip_getup) * 4, 0);
  native_finish_clip(fighter, victim_pause_getup);
}

static void roll_getup(Fighter *fighter) {
  mk4_mmove_getup(fighter, dat_getup_descriptor_roll_getup);
}

static void victim_pause_roll(Fighter *fighter) {
  native_hold(fighter, 8, roll_getup);
}

static void victim_impact_finish(Fighter *fighter) {
  battle_scene_impact(fighter);
  native_finish_clip(fighter, victim_pause_roll);
}

void victim_impact_roll_getup(Fighter *fighter) {
  victim_begin(fighter);
  const Mk4Image *image = battle_data();
  script_vm_bind_animation(fighter, mk4_image_u32(image, dat_animation_descriptor_victim_impact_roll_getup) * 4, 0);
  native_play_rows(fighter, 45, 1, victim_impact_finish);
}

static void victim_slide(Fighter *fighter) {
  fighter_set_speed(fighter, -0x3333);
  fighter->body.physics = fighter_friction;
  native_finish_clip(fighter, mk4_mmove_thrown_finish);
}

void victim_pause_slide(Fighter *fighter) {
  victim_begin(fighter);
  native_hold(fighter, 2, victim_slide);
}

static void victim_stance_blend(Fighter *fighter) {
  const Mk4Image *image = battle_data();
  int clip = mk4_stance_clip(image, (unsigned)fighter->roster, false);
  unsigned count = mk4_image_u32(image, dat_animation_table + (uint32_t)clip * MK4_ANIMATION_DESCRIPTOR_SIZE + MK4_ANIMATION_FRAME_COUNT_OFFSET);
  require(count > 0 && count <= 65535, "Invalid native stance animation length");
  mk4_mmove_blend_to(fighter, clip, (int)count, 0, 0x3333, 0, mk4_mmove_thrown_finish);
}

static void victim_slide_blend(Fighter *fighter) {
  fighter_set_speed(fighter, -0x3333);
  fighter->body.physics = fighter_friction;
  native_finish_clip(fighter, victim_stance_blend);
}

void victim_slide_to_stance(Fighter *fighter) {
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
void action_lp_victim(Fighter *fighter) {
  uint32_t entry = mk4_reaction_target(battle_data(), (unsigned)fighter->reaction);
  if (entry == native_lp_victim_weapon_scatter_release && fighter_peer(fighter)->roster == 15) {
    action_disturb_resting_weapons();
  }
  mk4_script_disarm_self(fighter);
  victim_begin(fighter);
  action_throw_grunt();
  mk4_script_shake(fighter, 0);
  mk4_script_roster_sound(fighter, 6);
  mk4_script_stop_body(fighter);
  fighter->body.pos[AXIS_Y] = fighter->body.ground;
  switch (entry) {
    case native_lp_victim_table_release:
      native_animation_table(fighter, dat_animation_rows_lp_victim_table_release);
      break;
    case native_lp_victim_rebased_table_release:
      native_animation_table(fighter, dat_animation_rows_lp_victim_rebased_release);
      break;
    case native_lp_victim_weapon_scatter_release:
      native_animation_table(fighter, dat_animation_rows_lp_victim_weapon_scatter_release);
      break;
    case native_lp_victim_two_speed_slide_release:
      script_vm_enter(fighter, fn_lp_victim_two_speed_slide_release, mk4_mmove_thrown_finish);
      break;
    case native_lp_victim_impact_release:
      script_vm_enter(fighter, fn_lp_victim_impact_release, mk4_mmove_thrown_finish);
      break;
    case native_lp_victim_sliding_impact_release:
      script_vm_enter(fighter, fn_lp_victim_sliding_impact_release, mk4_mmove_thrown_finish);
      break;
    case native_lp_victim_slide_release:
      script_vm_enter(fighter, fn_lp_victim_slide_release, mk4_mmove_thrown_finish);
      break;
    default:
      require(false, "Unexpected native LP victim entry");
  }
}

void action_script_release_reaction(Fighter *fighter, int reaction) {
  battle_react(mk4_script_release_pair(fighter), reaction);
}

void victim_finish_captured_clip(Fighter *fighter) {
  victim_begin(fighter);
  mk4_script_stop_body(fighter);
  script_vm_bind_animation(fighter, dat_animation_clip_586, 65);
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

void action_lk_victim(Fighter *fighter) {
  uint32_t entry = mk4_reaction_target(battle_data(), (unsigned)fighter->reaction);
  int sample = fighter_sample(fighter);
  victim_begin(fighter);
  switch (entry) {
    case native_lk_victim_clip_203_recovery:
      script_vm_bind_animation(fighter, dat_animation_clip_203, sample);
      native_play_rows(fighter, mini(fighter_move(fighter)->last, sample + 7), 1, lk_inherited_finish);
      break;
    case native_liu_kang_lk_throw_victim_reaction_80:
      script_vm_enter(fighter, fn_liu_kang_lk_throw_victim, mk4_mmove_thrown_finish);
      break;
    case native_sonya_leg_grab_victim_recovery:
      script_vm_enter(fighter, fn_sonya_leg_grab_victim_recovery, mk4_mmove_thrown_finish);
      break;
    case native_lk_victim_effect_slide:
      mk4_script_effect(fighter, 0);
      script_vm_bind_animation(fighter, dat_animation_clip_169, 0);
      script_vm_bind_animation(fighter, mk4_image_u32(battle_data(), dat_animation_descriptor_lk_victim_slide) * 4, 0);
      fighter_set_speed(fighter, (q16)mk4_image_u32(battle_data(), dat_lk_victim_slide_speed));
      native_play_rows(fighter, mini(fighter_move(fighter)->last, 26), 1, lk_impact_finish);
      break;
    default:
      require(false, "Unexpected native LK victim entry");
  }
}

FighterFn action_throw_reaction(const Mk4Image *image, unsigned reaction) {
  return script_native_throw_reaction(mk4_reaction_target(image, reaction));
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
    case native_cage_lk_throw_entry:
      return fn_cage_lk_throw_startup;
    case native_jarek_lk_throw_entry:
      return fn_jarek_lk_throw_startup;
    case native_tanya_lk_throw_entry:
      return fn_tanya_lk_throw_startup;
    case native_shinnok_lk_throw_entry:
      return fn_shinnok_lk_throw_startup;
    case native_reptile_lk_throw_entry:
      return fn_reptile_lk_throw_startup;
    case native_reiko_lk_throw_entry:
      return fn_reiko_lk_throw_startup;
    case native_quan_chi_lk_throw_entry:
      return fn_quan_chi_lk_throw_startup;
    case native_raiden_lk_throw_entry:
      return fn_raiden_lk_throw_startup;
    case native_kai_lk_throw_entry:
      return fn_kai_lk_throw_startup;
    case native_fujin_lk_throw_entry:
      return fn_fujin_lk_throw_startup;
    case native_scorpion_lk_throw_entry:
      return fn_scorpion_lk_throw_startup;
    case native_sonya_lk_throw_entry:
      return fn_sonya_lk_throw_startup;
    case native_liu_kang_lk_throw_entry:
      return fn_liu_kang_lk_throw_startup;
    case native_sub_zero_lk_throw_entry:
      return fn_sub_zero_lk_throw_startup;
    case native_goro_lk_throw_entry:
      return fn_goro_lk_throw_startup;
    default:
      require(false, "Unimplemented native LK entry");
      return 0;
  }
}

void action_lk_throw(Fighter *fighter) {
  if (!mk4_can_ground_throw(fighter, true)) {
    fighter_ready(fighter);
    return;
  }
  const Mk4Image *image = battle_data();
  uint32_t native = mk4_lk_target(image, (unsigned)fighter->roster);
  uint32_t script = lk_script(native);
  action_throw_capture(fighter, ACTION_CODE_0501, native != native_sub_zero_lk_throw_entry);
  if (native == native_sub_zero_lk_throw_entry) {
    fighter_set_speed(fighter, -0x1999);
    mk4_script_save_motion(fighter);
  } else if (native == native_liu_kang_lk_throw_entry) {
    Fighter *peer = (Fighter *)fighter_opponent(fighter);
    peer->body.pos[AXIS_X] += q16_mul(fighter_axis(fighter, AXIS_X), 0xcccc);
    peer->body.pos[AXIS_Z] += q16_mul(fighter_axis(fighter, AXIS_Z), 0xcccc);
  } else if (native == native_quan_chi_lk_throw_entry) {
    pair_edge_task(fighter);
  } else if (native == native_goro_lk_throw_entry) {
    battle_sound(SOUND_TOKEN, 0x654);
  }
  script_vm_enter(fighter, script, fighter_ready);
}

void action_throw_pair_start(Fighter *fighter) {
  require((fighter->flags & FF_PAIRED) && (fighter_opponent(fighter)->flags & FF_PAIRED), "Native42b680 needs its acquired peer");
  fighter->action = ACTION_CODE_0501;
}

static void idle_pose(Fighter *fighter) {
  const Mk4Image *image = battle_data();
  int clip = mk4_stance_clip(image, (unsigned)fighter->roster, false);
  script_vm_bind_animation(fighter, dat_animation_table + (uint32_t)clip * MK4_ANIMATION_DESCRIPTOR_SIZE, 0);
}

void action_lk_return(Fighter *fighter) {
  idle_pose(fighter);
  mk4_mmove_thrown_finish(fighter);
}

void action_lk_pause_long(Fighter *fighter) {
  idle_pose(fighter);
  script_vm_wait_native(fighter, 80, mk4_mmove_thrown_finish);
}

void action_lk_pause_short(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  idle_pose(fighter);
  script_vm_wait_native(fighter, 40, mk4_mmove_thrown_finish);
}

void action_lk_jump_back(Fighter *fighter) {
  script_vm_bind_animation(fighter, dat_animation_clip_301, 0);
  mk4_jump_backward(fighter);
}

void action_lk_recoil(Fighter *fighter) {
  idle_pose(fighter);
  script_vm_wait_native(fighter, 1, mk4_mmove_thrown_finish);
}

void action_lk_branch_continue(Fighter *fighter) {
  pair_edge_task(fighter);
  script_vm_enter(fighter, fn_lk_branch_continue, fighter_ready);
}

void action_lk_victim_return(Fighter *fighter) {
  const Mk4Image *image = fighter->execution.script.image;
  script_vm_bind_animation(fighter, mk4_image_u32(image, dat_animation_descriptor_lk_victim_return) * 4, 0);
  script_vm_forward_return(fighter);
}

void action_lp_align_and_release(Fighter *fighter) {
  require(script_native_dispatch(fighter, native_liu_kang_liu_script_fatal_align, MK4_CALL, 0), "Missing shared pair alignment native");
  script_vm_enter(fighter, fn_lp_shared_release, fighter_ready);
}

/* Native42cbc0/42ce60/42ce80 return through the engine controller boundary.
 * Pair-space coordinates belong to simulation rather than retail render state. */
void action_throw_ready(Fighter *fighter) {
  if (fighter->flags & FF_PAIRED) {
    fighter->flags &= ~(unsigned)FF_PAIRED;
    fighter_peer(fighter)->flags &= ~(unsigned)FF_PAIRED;
  }
  mk4_script_stop_body(fighter);
  fighter_execution_finish(fighter, fighter->health > 0 ? fighter_ready : NULL);
}

/* 42cc40 runs the peer callback cell4de664, then the original owner words. */
void action_lp_end(Fighter *fighter) {
  action_throw_turn(fighter_peer(fighter));
  script_vm_enter(fighter, fn_lp_end, fighter_ready);
}

/* 470420/48e080 releases native callbacks and restores ordinary control.
 * Airborne motion belongs to physics; controller return must preserve it. */
void action_controller_return(Fighter *fighter) {
  if (fighter->flags & FF_PAIRED) {
    fighter->flags &= ~(unsigned)FF_PAIRED;
    fighter_peer(fighter)->flags &= ~(unsigned)FF_PAIRED;
  }
  native_motion_stop(fighter);
  fighter->body.physics = NULL;
  fighter->body.on_land = NULL;
  action_face_peer(fighter);
  fighter_execution_finish(fighter, fighter->health > 0 ? fighter_ready : NULL);
}
