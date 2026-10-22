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
    retail_vm_bind_animation(fighter, 0x4fad7c + (uint32_t)clip * 12, sample);
  }
  mk4_script_touchable(fighter);
}

static void victim_485860(Fighter *fighter) {
  victim_begin(fighter);
  fighter->body.pos[AXIS_Y] = fighter->body.ground;
  fighter->facing = -fighter->facing;
  retail_vm_enter(fighter, 0x4e9a20, mk4_mmove_thrown_finish);
}

void retail_native_sound_list(uint32_t address) {
  const Mk4Image *image = retail_vm_image();
  unsigned count = mk4_image_u32(image, address);
  require(count > 0 && count <= 32, "Invalid native sound token list");
  battle_sound(SOUND_TOKEN, (int)mk4_image_u32(image, address + 4 + 4 * battle_random(count)));
}

void retail_impact_voice(Fighter *fighter) {
  (void)fighter;
  retail_native_sound_list(0x4ea8a0);
}

void retail_throw_grunt(void) {
  retail_native_sound_list(0x4ea828);
}

static void victim_4857b0(Fighter *fighter) {
  victim_begin(fighter);
  retail_vm_bind_animation(fighter, 0x4fb55c, 0);
  fighter->body.physics = fighter_friction;
  retail_impact_voice(fighter);
  retail_vm_enter(fighter, 0x4e99e0, mk4_mmove_thrown_finish);
}

static void victim_485810(Fighter *fighter) {
  victim_begin(fighter);
  retail_vm_bind_animation(fighter, 0x4fb5c8, 0);
  mk4_script_save_motion(fighter);
  retail_vm_enter(fighter, 0x4e99f0, mk4_mmove_thrown_finish);
}

static void victim_4854e0(Fighter *fighter) {
  mk4_script_disarm_self(fighter);
  victim_begin(fighter);
  retail_vm_enter(fighter, 0x4e99c0, mk4_mmove_thrown_finish);
}

static void victim_485480(Fighter *fighter) {
  mk4_script_disarm_self(fighter);
  victim_begin(fighter);
  fighter->body.pos[AXIS_Y] -= 0x4ccc;
  retail_vm_enter(fighter, 0x4ea398, mk4_mmove_thrown_finish);
}

static void flip_getup(Fighter *fighter) {
  fighter->facing = -fighter->facing;
  mk4_mmove_getup(fighter, 0x4ea210);
}

static void victim_pause_getup(Fighter *fighter) {
  native_hold(fighter, 10, flip_getup);
}

static void victim_4851d0(Fighter *fighter) {
  mk4_script_disarm_self(fighter);
  victim_begin(fighter);
  const Mk4Image *image = retail_vm_image();
  retail_vm_bind_animation(fighter, mk4_image_u32(image, 0x53cf7c) * 4, 0);
  native_finish_clip(fighter, victim_pause_getup);
}

static void roll_getup(Fighter *fighter) {
  mk4_mmove_getup(fighter, 0x4ea220);
}

static void victim_pause_roll(Fighter *fighter) {
  native_hold(fighter, 8, roll_getup);
}

static void victim_impact_finish(Fighter *fighter) {
  battle_scene_impact(fighter);
  native_finish_clip(fighter, victim_pause_roll);
}

static void victim_485530(Fighter *fighter) {
  victim_begin(fighter);
  const Mk4Image *image = retail_vm_image();
  retail_vm_bind_animation(fighter, mk4_image_u32(image, 0x53cf80) * 4, 0);
  native_play_rows(fighter, 45, 1, victim_impact_finish);
}

static void victim_slide(Fighter *fighter) {
  fighter_set_speed(fighter, -0x3333);
  fighter->body.physics = fighter_friction;
  native_finish_clip(fighter, mk4_mmove_thrown_finish);
}

static void victim_4856a0(Fighter *fighter) {
  victim_begin(fighter);
  native_hold(fighter, 2, victim_slide);
}

static void victim_stance_blend(Fighter *fighter) {
  const Mk4Image *image = retail_vm_image();
  int clip = retail_stance_clip(image, (unsigned)fighter->roster, false);
  unsigned count = mk4_image_u32(image, 0x4fad7c + (uint32_t)clip * 12 + 4);
  require(count > 0 && count <= 65535, "Invalid native stance animation length");
  mk4_mmove_blend_to(fighter, clip, (int)count, 0, 0x3333, 0, mk4_mmove_thrown_finish);
}

static void victim_slide_blend(Fighter *fighter) {
  fighter_set_speed(fighter, -0x3333);
  fighter->body.physics = fighter_friction;
  native_finish_clip(fighter, victim_stance_blend);
}

static void victim_4852e0(Fighter *fighter) {
  mk4_script_disarm_self(fighter);
  victim_begin(fighter);
  native_hold(fighter, 1, victim_slide_blend);
}

/* 485c60 is shared by the LP victim entries. Pair/world coordinates are
 * owned by the engine; 485d00's render-coordinate rebasing is unnecessary. */
void retail_lp_victim(Fighter *fighter) {
  uint32_t entry = retail_reaction_target(retail_vm_image(), (unsigned)fighter->reaction);
  if (entry == 0x485bf0 && fighter_peer(fighter)->roster == 15) {
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
    case 0x485b70:
      native_animation_table(fighter, 0x4e9bb8);
      break;
    case 0x485ba0:
      native_animation_table(fighter, 0x4e9bc0);
      break;
    case 0x485bf0:
      native_animation_table(fighter, 0x4e9bc8);
      break;
    case 0x485b30:
      retail_vm_enter(fighter, 0x4e9b88, mk4_mmove_thrown_finish);
      break;
    case 0x485af0:
      retail_vm_enter(fighter, 0x4e9b68, mk4_mmove_thrown_finish);
      break;
    case 0x485a90:
      retail_vm_enter(fighter, 0x4e9b40, mk4_mmove_thrown_finish);
      break;
    case 0x485a30:
      retail_vm_enter(fighter, 0x4e9b18, mk4_mmove_thrown_finish);
      break;
    default:
      require(false, "Unexpected native LP victim entry");
  }
}

void retail_script_release_reaction(Fighter *fighter, int reaction) {
  battle_react(mk4_script_release_pair(fighter), reaction);
}

static void victim_484f10(Fighter *fighter) {
  victim_begin(fighter);
  mk4_script_stop_body(fighter);
  retail_vm_bind_animation(fighter, 0x4fc8f4, 65);
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
  uint32_t entry = retail_reaction_target(retail_vm_image(), (unsigned)fighter->reaction);
  int sample = fighter_sample(fighter);
  victim_begin(fighter);
  switch (entry) {
    case 0x484d50:
      retail_vm_bind_animation(fighter, 0x4fb700, sample);
      native_play_rows(fighter, mini(fighter_move(fighter)->last, sample + 7), 1, lk_inherited_finish);
      break;
    case 0x485910:
      retail_vm_enter(fighter, 0x4e9a90, mk4_mmove_thrown_finish);
      break;
    case 0x4858a0:
      retail_vm_enter(fighter, 0x4e9a68, mk4_mmove_thrown_finish);
      break;
    case 0x485040:
      mk4_script_effect(fighter, 0);
      retail_vm_bind_animation(fighter, 0x4fb568, 0);
      retail_vm_bind_animation(fighter, mk4_image_u32(retail_vm_image(), 0x53cf78) * 4, 0);
      fighter_set_speed(fighter, (q16)mk4_image_u32(retail_vm_image(), 0x4e99bc));
      native_play_rows(fighter, mini(fighter_move(fighter)->last, 26), 1, lk_impact_finish);
      break;
    default:
      require(false, "Unexpected native LK victim entry");
  }
}

FighterFn retail_throw_reaction(const Mk4Image *image, unsigned reaction) {
  switch (retail_reaction_target(image, reaction)) {
    case 0x485910:
    case 0x484d50:
    case 0x4858a0:
    case 0x485040:
      return retail_lk_victim;
    case 0x485b70:
    case 0x485ba0:
    case 0x485bf0:
    case 0x485b30:
    case 0x485af0:
    case 0x485a90:
    case 0x485a30:
      return retail_lp_victim;
    case 0x484f10:
      return victim_484f10;
    case 0x484760:
      return retail_slam_reaction;
    case 0x4847a0:
      return retail_air_victim_flip;
    case 0x484b40:
      return retail_air_victim_default;
    case 0x484a10:
      return retail_air_victim_lift;

    case 0x4851d0:
      return victim_4851d0;
    case 0x485530:
      return victim_485530;
    case 0x4856a0:
      return victim_4856a0;
    case 0x4852e0:
      return victim_4852e0;
    case 0x485860:
      return victim_485860;
    case 0x4857b0:
      return victim_4857b0;
    case 0x485810:
      return victim_485810;
    case 0x4854e0:
      return victim_4854e0;
    case 0x485480:
      return victim_485480;
    default:
      return NULL;
  }
}

void retail_throw_victim_continue(Fighter *fighter) {
  retail_vm_enter(fighter, 0x4ea3c0, mk4_mmove_thrown_finish);
}

void retail_throw_victim_impact(Fighter *fighter) {
  retail_vm_enter(fighter, 0x4ea3d0, mk4_mmove_thrown_finish);
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
    case 0x42b700:
      return 0x4de3d0;
    case 0x42b770:
      return 0x4ddff8;
    case 0x42b7e0:
      return 0x4de038;
    case 0x42bad0:
      return 0x4de088;
    case 0x42bb50:
      return 0x4de0e8;
    case 0x42bbe0:
      return 0x4de138;
    case 0x42bc40:
      return 0x4de1b0;
    case 0x42bce0:
      return 0x4de218;
    case 0x42bd10:
      return 0x4de258;
    case 0x42bda0:
      return 0x4de2a0;
    case 0x42bdd0:
      return 0x4de2f8;
    case 0x42bed0:
      return 0x4de360;
    case 0x42c040:
      return 0x4de3b0;
    case 0x42c180:
      return 0x4de490;
    case 0x42c750:
      return 0x4de720;
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
  const Mk4Image *image = retail_vm_image();
  uint32_t native = retail_lk_target(image, (unsigned)fighter->roster);
  uint32_t script = lk_script(native);
  retail_throw_capture(fighter, ACTION_RETAIL_0501, native != 0x42c180);
  if (native == 0x42c180) {
    fighter_set_speed(fighter, -0x1999);
    mk4_script_save_motion(fighter);
  } else if (native == 0x42c040) {
    Fighter *peer = (Fighter *)fighter_opponent(fighter);
    peer->body.pos[AXIS_X] += q16_mul(fighter_axis(fighter, AXIS_X), 0xcccc);
    peer->body.pos[AXIS_Z] += q16_mul(fighter_axis(fighter, AXIS_Z), 0xcccc);
  } else if (native == 0x42bc40) {
    pair_edge_task(fighter);
  } else if (native == 0x42c750) {
    battle_sound(SOUND_TOKEN, 0x654);
  }
  retail_vm_enter(fighter, script, fighter_ready);
}

void retail_throw_pair_start(Fighter *fighter) {
  require((fighter->flags & FF_PAIRED) && (fighter_opponent(fighter)->flags & FF_PAIRED), "Native42b680 needs its acquired peer");
  fighter->action = ACTION_RETAIL_0501;
}

static void idle_pose(Fighter *fighter) {
  const Mk4Image *image = retail_vm_image();
  int clip = retail_stance_clip(image, (unsigned)fighter->roster, false);
  retail_vm_bind_animation(fighter, 0x4fad7c + (uint32_t)clip * 12, 0);
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
  retail_vm_bind_animation(fighter, 0x4fbb98, 0);
  mk4_jump_backward(fighter);
}

void retail_lk_recoil(Fighter *fighter) {
  idle_pose(fighter);
  retail_vm_wait_native(fighter, 1, mk4_mmove_thrown_finish);
}

void retail_lk_branch_continue(Fighter *fighter) {
  pair_edge_task(fighter);
  retail_vm_enter(fighter, 0x4de060, fighter_ready);
}

void retail_lk_victim_return(Fighter *fighter) {
  const Mk4Image *image = fighter->retail_vm.image;
  retail_vm_bind_animation(fighter, mk4_image_u32(image, 0x4de210) * 4, 0);
  retail_vm_forward_return(fighter);
}

void retail_lk_script_170(Fighter *fighter) {
  retail_vm_enter(fighter, 0x4de170, fighter_ready);
}

void retail_lk_script_180(Fighter *fighter) {
  retail_vm_enter(fighter, 0x4de180, fighter_ready);
}

void retail_lk_script_190(Fighter *fighter) {
  retail_vm_enter(fighter, 0x4de190, fighter_ready);
}

void retail_lk_continue_840(Fighter *fighter) {
  require(retail_native_vm(fighter, 0x48f9f0, RETAIL_CALL, 0), "Missing shared pair alignment native");
  retail_vm_enter(fighter, 0x4de840, fighter_ready);
}

/* Native42cbc0/42ce60/42ce80 return through the engine controller boundary.
 * Pair-space coordinates belong to simulation rather than retail render state. */
void retail_throw_ready(Fighter *fighter) {
  if (fighter->flags & FF_PAIRED) {
    fighter->flags &= ~(unsigned)FF_PAIRED;
    fighter_peer(fighter)->flags &= ~(unsigned)FF_PAIRED;
  }
  mk4_script_stop_body(fighter);
  fighter_finish_mmove(fighter, fighter->health > 0 ? fighter_ready : NULL);
}

/* 42cc40 runs the peer callback cell4de664, then the original owner words. */
void retail_lp_end(Fighter *fighter) {
  retail_throw_turn(fighter_peer(fighter));
  retail_vm_enter(fighter, 0x4de668, fighter_ready);
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
  fighter_finish_mmove(fighter, fighter->health > 0 ? fighter_ready : NULL);
}
