#include "script/native_ids.h"
#include "native/actions.h"
#include "script/vm.h"
#include "data/tables.h"
#include "native/helpers.h"
#include "runtime/playback.h"

/* 470c60/470cb0: native contact rows, followed by a negative terminator
 * and native continuation. Poses and selectors stay in the retail image. */
static void kick_row(Fighter *fighter) {
  ScriptVm *vm = &fighter->execution.script;
  require(vm->active && vm->native_rows++ < 64, "Unbounded native contact rows");
  int32_t pose = (int32_t)mk4_image_u32(vm->image, vm->native_cursor);
  vm->native_cursor += 4;
  if (pose < 0) {
    uint32_t next = mk4_image_u32(vm->image, vm->native_cursor);
    uint32_t script;
    if (next == MK4_NATIVE_JAX_HIGH_KICK_CONTACT_RECOVERY || next == MK4_NATIVE_FUJIN_HIGH_KICK_CONTACT_RECOVERY) {
      /* 470e80 marks the recovery action before these two continuations. */
      fighter->action = ACTION_CODE_1006;
      script = next == MK4_NATIVE_JAX_HIGH_KICK_CONTACT_RECOVERY ? MK4_SCRIPT_JAX_HIGH_KICK_RECOVERY : MK4_SCRIPT_FUJIN_HIGH_KICK_RECOVERY;
    } else {
      require(next == MK4_NATIVE_HIGH_KICK_NARROW_CONTACT_RECOVERY, "Unimplemented native contact-row continuation");
      script = MK4_SCRIPT_KAI_HIGH_KICK_RECOVERY;
    }
    script_vm_enter(fighter, script, vm->finish);
    return;
  }
  require(pose <= fighter_move(fighter)->last, "Native contact pose outside clip");
  fighter->sample_override = pose;
  uint64_t continuation = fighter->execution.generation;
  if (!vm->native_contact && fighter_opponent(fighter)->action != ACTION_CODE_2001) {
    require(vm->native_query == MK4_NATIVE_CONTACT_QUERY_B, "Unimplemented native contact-row query");
    uint32_t record = mk4_image_u32(vm->image, MK4_DATA_CONTACT_QUERY_B_TABLE + vm->native_selector * 4);
    AttackRecord attack = script_vm_attack_record(vm->image, record * 4);
    ContactResult result = battle_contact(fighter, &attack);
    if (!vm->active || fighter->execution.generation != continuation || fighter->pending != NULL) {
      return;
    }
    vm->native_contact = result != CONTACT_MISS;
  }
  script_vm_wait_native(fighter, 1, kick_row);
}

void action_kick_contact_rows(Fighter *fighter, uint32_t table) {
  ScriptVm *vm = &fighter->execution.script;
  require(vm->active && table >= MK4_DATA_IMAGE_BASE && table <= MK4_DATA_IMAGE_END - 12, "Native contact rows need a live retail task and data");
  vm->native_selector = mk4_image_u32(vm->image, table);
  vm->native_query = mk4_image_u32(vm->image, table + 4);
  vm->native_cursor = table + 8;
  vm->native_rows = 0;
  vm->native_contact = false;
  kick_row(fighter);
}

void action_kick_contact_default(Fighter *fighter) {
  action_kick_contact_rows(fighter, MK4_DATA_KICK_CONTACT_DEFAULT);
}

void action_kick_contact_wide(Fighter *fighter) {
  action_kick_contact_rows(fighter, MK4_DATA_KICK_CONTACT_WIDE);
}

void action_kick_contact_narrow(Fighter *fighter) {
  action_kick_contact_rows(fighter, MK4_DATA_KICK_CONTACT_NARROW);
}

/* Native 46a680 and the post-gate block 46a81a set the action and enter the retail selector.
 * The script binds the clip before yielding; no fighter C clip seed is used. */
static void kick_enter(Fighter *fighter, int action, uint32_t script) {
  action_begin(fighter, S_KICK, action, false);
  script_vm_enter(fighter, script, fighter_ready);
}

void action_high_kick(Fighter *fighter) {
  kick_enter(fighter, ACTION_CODE_0100, MK4_DATA_HIGH_KICK);
}

void action_low_kick(Fighter *fighter) {
  kick_enter(fighter, ACTION_CODE_0102, MK4_DATA_LOW_KICK);
}

/* Playback after the original controller's LP throw gates (46dcbc). */
void action_jab(Fighter *fighter) {
  action_begin(fighter, S_PUNCH, ACTION_CODE_010B, true);
  script_vm_enter(fighter, MK4_SCRIPT_JAB, fighter_ready);
}

static bool sweep_sound(BattleEvent *event, Fighter *owner, Fighter *victim) {
  (void)event;
  (void)owner;
  (void)victim;
  battle_sound(SOUND_B, 0x1c);
  return false;
}

void action_sweep(Fighter *fighter) {
  action_begin(fighter, S_SWEEP, ACTION_CODE_010C, false);
  /* 46ef90 spawns independent native 46ef30, which cues B[28] after14. */
  BattleEvent *event = battle_event_start(fighter, sweep_sound, false, false);
  if (event != NULL) {
    battle_event_delay(event, 14);
  }
  script_vm_enter(fighter, MK4_SCRIPT_SWEEP, fighter_ready);
}

static void sweep_recover(Fighter *fighter) {
  script_vm_forward_return(fighter);
}

/* 46efd0/46f080: submit the next pose and query once, repeat only on a
 * first miss, then mark recovery, wait one visit and finish the bound clip. */
static void sweep_query(Fighter *fighter) {
  ScriptVm *vm = &fighter->execution.script;
  require(vm->active && vm->native_rows < 2, "Sweep query without a live native continuation");
  int pose = fighter_sample(fighter);
  if (pose < fighter_move(fighter)->last) {
    fighter->sample_override = pose + 1;
  }
  uint32_t packed = mk4_image_u32(vm->image, MK4_DATA_CONTACT_QUERY_B_TABLE + vm->native_selector * 4);
  AttackRecord attack = script_vm_attack_record(vm->image, packed * 4);
  uint64_t continuation = fighter->execution.generation;
  ContactResult result = battle_contact(fighter, &attack);
  if (!vm->active || fighter->execution.generation != continuation || fighter->pending != NULL) {
    return;
  }
  if (result != CONTACT_MISS || ++vm->native_rows == 2) {
    fighter->action = ACTION_CODE_1006;
    script_vm_wait_native(fighter, 1, sweep_recover);
  } else {
    script_vm_wait_native(fighter, 1, sweep_query);
  }
}

static void sweep_contact_start(Fighter *fighter, unsigned selector) {
  require(fighter->execution.script.active, "Sweep native needs a live retail task");
  fighter->execution.script.native_rows = 0;
  fighter->execution.script.native_selector = selector;
  sweep_query(fighter);
}

void action_sweep_standard_tail(Fighter *fighter) {
  sweep_contact_start(fighter, 35);
}

void action_sweep_advancing_tail(Fighter *fighter) {
  sweep_contact_start(fighter, 36);
}

/* Native 46f1f0 only stores 010d and enters the retail roster selector. */
void action_roundhouse(Fighter *fighter) {
  bool fast = (fighter->flags & FF_FOLLOW_UP) != 0;
  mk4_script_stop_body(fighter);
  action_begin(fighter, S_KICK, ACTION_CODE_010D, false);
  fighter->native_followup = fast;
  script_vm_enter(fighter, MK4_SCRIPT_ROUNDHOUSE, fighter_ready);
}

void action_roundhouse_start(Fighter *fighter) {
  script_vm_enter(
      fighter, fighter->native_followup ? MK4_SCRIPT_CAGE_ROUNDHOUSE_FAST_START : MK4_SCRIPT_CAGE_ROUNDHOUSE_NORMAL_START, fighter->execution.script.finish);
}

void action_roundhouse_wide_start(Fighter *fighter) {
  script_vm_enter(
      fighter, fighter->native_followup ? MK4_SCRIPT_JAX_ROUNDHOUSE_FAST_START : MK4_SCRIPT_JAX_ROUNDHOUSE_NORMAL_START, fighter->execution.script.finish);
}

static void roundhouse_return(Fighter *fighter) {
  FighterFn next = fighter->execution.script.finish;
  fighter_execution_cancel(fighter);
  if (next != NULL) {
    next(fighter);
  }
}

/* 46f500 saves the query result across forward playback. Results above one
 * (ground/air hit) install the native 28-visit recovery; miss/block return. */
static void roundhouse_recover(Fighter *fighter) {
  ScriptVm *vm = &fighter->execution.script;
  int pose = fighter_sample(fighter);
  if (pose < fighter_move(fighter)->last) {
    fighter->sample_override = pose + 1;
    script_vm_wait_native(fighter, 1, roundhouse_recover);
  } else {
    script_vm_wait_native(fighter, vm->native_contact ? 28 : 0, roundhouse_return);
  }
}

static void roundhouse_query(Fighter *fighter) {
  ScriptVm *vm = &fighter->execution.script;
  require(vm->active && vm->native_rows < 3, "Roundhouse query without a live native continuation");
  int pose = fighter_sample(fighter);
  if (pose < fighter_move(fighter)->last) {
    fighter->sample_override = pose + 1;
  }
  uint32_t packed = mk4_image_u32(vm->image, MK4_DATA_CONTACT_QUERY_B_TABLE + vm->native_selector * 4);
  AttackRecord attack = script_vm_attack_record(vm->image, packed * 4);
  uint64_t continuation = fighter->execution.generation;
  ContactResult result = battle_contact(fighter, &attack);
  if (!vm->active || fighter->execution.generation != continuation || fighter->pending != NULL) {
    return;
  }
  if (result != CONTACT_MISS || ++vm->native_rows == 3) {
    vm->native_contact = result == CONTACT_HIT || result == CONTACT_AIR_HIT;
    fighter->action = ACTION_CODE_1006;
    script_vm_wait_native(fighter, 1, roundhouse_recover);
  } else {
    script_vm_wait_native(fighter, 1, roundhouse_query);
  }
}

static void roundhouse_contact_start(Fighter *fighter, unsigned selector) {
  require(fighter->execution.script.active, "Roundhouse native needs a live retail task");
  fighter->execution.script.native_rows = 0;
  fighter->execution.script.native_selector = selector;
  roundhouse_query(fighter);
}

void action_roundhouse_tail(Fighter *fighter) {
  roundhouse_contact_start(fighter, 33);
}

void action_roundhouse_wide_tail(Fighter *fighter) {
  roundhouse_contact_start(fighter, 34);
}

/* 46cdb4 playback after the finish-window controller gates. */
void action_uppercut(Fighter *fighter) {
  action_begin(fighter, S_UPPERCUT, ACTION_CODE_0109, false);
  script_vm_enter(fighter, MK4_SCRIPT_UPPERCUT, fighter_ready);
}

/* Native42d1b0's pair-range/camera-side branch uses the engine fighter axis
 * for camera-side ordering. Both original native selectors share this test. */
static void lp_select(Fighter *fighter, uint32_t alternate) {
  q16 along = 0;
  int64_t distance2 = 0;
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    q16 delta = fighter->saved_motion[axis];
    along += q16_mul(delta, fighter_axis(fighter, axis));
    distance2 += ((int64_t)delta * delta) >> 16;
  }
  uint32_t target = along >= 0 && distance2 < 0x280000 ? MK4_DATA_LP_SELECT : alternate;
  FighterFn finish = fighter->execution.script.active ? fighter->execution.script.finish : fighter_ready;
  script_vm_enter(fighter, target, finish);
}

void action_lp_select_short(Fighter *fighter) {
  lp_select(fighter, MK4_DATA_LP_SELECT_SHORT);
}

void action_lp_select_long(Fighter *fighter) {
  lp_select(fighter, MK4_DATA_LP_SELECT_LONG);
}

/* 45b380 invokes 490bd0; pair heading is maintained by the world pump. */
void action_face_peer(Fighter *fighter) {
  fighter->facing = fighter->body.pos[AXIS_X] <= fighter_opponent(fighter)->body.pos[AXIS_X] ? 1 : -1;
}

/* 46b710/46b740: script startup, one B[23] probe, then native recovery wait. */
void action_crouch_low_kick(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter->body.pos[AXIS_Y] = fighter->body.ground;
  action_begin(fighter, S_KICK, ACTION_CODE_0106, false);
  script_vm_enter(fighter, MK4_SCRIPT_CROUCH_LOW_KICK, fighter_ready);
}

static void crouch_low_kick_recover(Fighter *fighter) {
  script_vm_enter(fighter, MK4_SCRIPT_CROUCH_LOW_KICK_RECOVER, fighter->execution.script.finish);
}

void action_crouch_low_kick_tail(Fighter *fighter) {
  ScriptVm *vm = &fighter->execution.script;
  require(vm->active, "Crouch LK native needs a live retail task");
  ContactResult result = CONTACT_MISS;
  if (fighter_opponent(fighter)->action != ACTION_CODE_2011) {
    uint32_t packed = mk4_image_u32(vm->image, MK4_DATA_CONTACT_QUERY_B_TABLE + 23 * 4);
    AttackRecord attack = script_vm_attack_record(vm->image, packed * 4);
    uint64_t continuation = fighter->execution.generation;
    result = battle_contact(fighter, &attack);
    if (!vm->active || fighter->execution.generation != continuation || fighter->pending != NULL) {
      return;
    }
  }
  fighter->action = ACTION_CODE_1006;
  script_vm_wait_native(fighter, result == CONTACT_MISS ? 1 : 4, crouch_low_kick_recover);
}

/* Native46b5a0 delegates all authored launcher differences to selector4e6020. */
void action_crouch_high_kick(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter->body.pos[AXIS_Y] = fighter->body.ground;
  action_begin(fighter, S_KICK, ACTION_CODE_0104, false);
  script_vm_enter(fighter, MK4_SCRIPT_CROUCH_HIGH_KICK, fighter_ready);
}

static void crouch_punch_reverse(Fighter *fighter) {
  int last = fighter_move(fighter)->last;
  native_begin(fighter, 0, last, fighter_ready);
  for (int row = 0; row <= last; ++row) {
    native_frame(fighter, row, last - row, NULL);
  }
  native_play(fighter, false);
}

static void crouch_punch_contact(Fighter *fighter) {
  ContactResult result = CONTACT_MISS;
  if (fighter_opponent(fighter)->action != ACTION_CODE_2011) {
    const Mk4Image *image = battle_data();
    uint32_t packed = mk4_image_u32(image, MK4_DATA_CONTACT_QUERY_A_TABLE + 3 * 4);
    AttackRecord attack = script_vm_attack_record(image, packed * 4);
    uint64_t continuation = fighter->execution.generation;
    result = battle_contact(fighter, &attack);
    if (fighter->execution.generation != continuation || fighter->pending != NULL) {
      return;
    }
  }
  fighter->action = ACTION_CODE_1006;
  native_hold(fighter, result == CONTACT_MISS ? 1 : 4, crouch_punch_reverse);
}

static void crouch_punch_extend(Fighter *fighter) {
  battle_sound(SOUND_B, 27);
  native_finish_clip(fighter, crouch_punch_contact);
}

/* Native46b7f0:4940e0 selects the armed/unarmed descriptor from53ce58.
 * This routine is native; no script control flow or fighter frame table is copied. */
void action_crouch_low_punch(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter->body.pos[AXIS_Y] = fighter->body.ground;
  action_begin(fighter, S_PUNCH, ACTION_CODE_0107, false);
  const Mk4Image *image = battle_data();
  uint32_t packed = mk4_image_u32(image, MK4_DATA_CROUCH_LOW_PUNCH + (fighter->armed ? 4 : 0));
  script_vm_bind_animation(fighter, packed * 4, 2);
  require(fighter_move(fighter)->last >= 5, "Native crouch punch needs three opening samples");
  native_play_rows(fighter, 5, 1, crouch_punch_extend);
}

/* Native46be40 binds its descriptor before entering inherited-animation script4e61e0.
 * 48e6e0 tests the peer's action class, independently of the actor's weapon. */
void action_close_high_punch(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter->body.pos[AXIS_Y] = fighter->body.ground;
  action_begin(fighter, S_PUNCH, ACTION_CODE_0108, false);
  const Mk4Image *image = battle_data();
  unsigned slot = mk4_native_character_slot((unsigned)fighter->roster);
  uint32_t packed = mk4_image_u32(image, MK4_DATA_CLOSE_HIGH_PUNCH + slot * 4);
  bool reacting = (fighter_opponent(fighter)->action & 0xf000) == 0x4000;
  script_vm_bind_animation(fighter, packed * 4, reacting ? 0 : 4);
  script_vm_enter(fighter, MK4_SCRIPT_CLOSE_HIGH_PUNCH, fighter_ready);
}

/* Native490460 uses the captured peer's facing and relative order before
 * assigning motion. The engine's pair/world coordinates own that ordering. */
void action_directional_motion(Fighter *fighter, q16 speed, q16 up, q16 gravity) {
  const Fighter *peer = fighter_opponent(fighter);
  bool peer_right = peer->body.pos[AXIS_X] >= fighter->body.pos[AXIS_X];
  bool reverse = peer->facing < 0 ? !peer_right : peer_right;
  if (reverse) {
    speed = (q16)(-(int64_t)speed);
    action_face_peer(fighter);
    action_flip_facing(fighter);
  }
  fighter_set_speed(fighter, speed);
  fighter->body.vel[AXIS_Y] = up;
  fighter->body.gravity = gravity;
}

static int64_t native_radius_squared(const Fighter *fighter) {
  int64_t x = fighter->body.pos[AXIS_X], z = fighter->body.pos[AXIS_Z];
  return x * x + z * z;
}

/* Native48f100: current body is farther out and within the outer ten square
 * world units. These are arena geometry values, not fighter configuration. */
bool action_outer_edge(const Fighter *fighter) {
  int64_t radius = g_battle.arena_radius;
  int64_t own = native_radius_squared(fighter);
  return own >= native_radius_squared(fighter_opponent(fighter)) && own > radius * radius - 10LL * Q16_ONE * Q16_ONE;
}

static bool native_pair_recoil_frame(BattleEvent *event, Fighter *owner, Fighter *other) {
  (void)event;
  Fighter *actors[] = {owner, other};
  for (unsigned i = 0; i < 2; ++i) {
    actors[i]->body.vel[AXIS_X] = q16_mul(actors[i]->body.vel[AXIS_X], 0xe666);
    actors[i]->body.vel[AXIS_Z] = q16_mul(actors[i]->body.vel[AXIS_Z], 0xe666);
  }
  return true;
}

static bool native_pair_recoil_done(BattleEvent *event, Fighter *owner, Fighter *other) {
  (void)event;
  mk4_script_stop_body(owner);
  mk4_script_stop_body(other);
  return false;
}

/* Native45c050 ->42b8c0/42b930: geometric edge push, independently owned
 * by the world event pool. The original native task has ten decay visits. */
void action_edge_push(Fighter *fighter) {
  const Fighter *peer = fighter_opponent(fighter);
  int64_t outer = native_radius_squared(peer);
  if (outer < native_radius_squared(fighter)) {
    return;
  }
  int64_t radius = g_battle.arena_radius;
  int64_t threshold = radius - 2LL * Q16_ONE;
  /* 4ab290 takes the Q16 square root. Compare squared values in the
   * engine's world coordinates. */
  if (threshold >= 0 && outer <= threshold * threshold) {
    return;
  }
  Fighter *first = &g_battle.fighters[0], *second = &g_battle.fighters[1];
  q16 x = -q16_mul(first->body.pos[AXIS_X], 0x7ae);
  q16 z = -q16_mul(first->body.pos[AXIS_Z], 0x7ae);
  first->body.vel[AXIS_X] = second->body.vel[AXIS_X] = x;
  first->body.vel[AXIS_Z] = second->body.vel[AXIS_Z] = z;
  BattleEvent *event = battle_event_start(fighter, native_pair_recoil_done, false, false);
  battle_event_program(event, 10);
  for (int visit = 0; visit < 10; ++visit) {
    battle_event_frame(event, visit, 0, native_pair_recoil_frame);
  }
}
