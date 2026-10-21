#include "retail_native.h"
#include "retail_vm.h"
#include "script.h"

/* 470c60/470cb0: native contact rows, followed by a negative terminator
 * and native continuation. Poses and selectors stay in the retail image. */
static void kick_row(Fighter *fighter) {
  RetailVm *vm = &fighter->retail_vm;
  require(vm->active && vm->native_rows++ < 64, "Unbounded native contact rows");
  int32_t pose = (int32_t)mk4_image_u32(vm->image, vm->native_cursor);
  vm->native_cursor += 4;
  if (pose < 0) {
    uint32_t next = mk4_image_u32(vm->image, vm->native_cursor);
    uint32_t script;
    if (next == 0x46a770 || next == 0x46a6c0) {
      /* 470e80 marks the recovery action before these two continuations. */
      fighter->action = ACTION_RETAIL_1006;
      script = next == 0x46a770 ? 0x4e5c70 : 0x4e5d20;
    } else {
      require(next == 0x46a7a0, "Unimplemented native contact-row continuation");
      script = 0x4e6998;
    }
    retail_vm_enter(fighter, script, vm->finish);
    return;
  }
  require(pose <= fighter_move(fighter)->last, "Native contact pose outside clip");
  fighter->sample_override = pose;
  uint64_t continuation = fighter->continuation;
  if (!vm->native_contact && fighter_opponent(fighter)->action != ACTION_RETAIL_2001) {
    require(vm->native_query == 0x48d3f0, "Unimplemented native contact-row query");
    uint32_t record = mk4_image_u32(vm->image, 0x53d2b8 + vm->native_selector * 4);
    AttackRecord attack = retail_vm_attack_record(vm->image, record * 4);
    ContactResult result = battle_contact(fighter, &attack);
    if (!vm->active || fighter->continuation != continuation || fighter->pending != NULL) {
      return;
    }
    vm->native_contact = result != CONTACT_MISS;
  }
  retail_vm_wait_native(fighter, 1, kick_row);
}

void retail_kick_contact_rows(Fighter *fighter, uint32_t table) {
  RetailVm *vm = &fighter->retail_vm;
  require(vm->active && table >= RETAIL_SCRIPT_BEGIN && table <= RETAIL_SCRIPT_END - 12, "Native contact rows need a live retail task and data");
  vm->native_selector = mk4_image_u32(vm->image, table);
  vm->native_query = mk4_image_u32(vm->image, table + 4);
  vm->native_cursor = table + 8;
  vm->native_rows = 0;
  vm->native_contact = false;
  kick_row(fighter);
}

void retail_kick_contact_default(Fighter *fighter) {
  retail_kick_contact_rows(fighter, 0x4e5d00);
}

void retail_kick_contact_wide(Fighter *fighter) {
  retail_kick_contact_rows(fighter, 0x4e5d30);
}

void retail_kick_contact_narrow(Fighter *fighter) {
  retail_kick_contact_rows(fighter, 0x4e6978);
}

/* Native 46a680 and the post-gate block 46a81a set the action and enter the retail selector.
 * The script binds the clip before yielding; no fighter C clip seed is used. */
static void kick_enter(Fighter *fighter, int action, uint32_t script) {
  retail_native_begin(fighter, S_KICK, action, false);
  retail_vm_enter(fighter, script, fighter_ready);
}

void retail_high_kick(Fighter *fighter) {
  kick_enter(fighter, ACTION_RETAIL_0100, 0x4e5ca0);
}

void retail_low_kick(Fighter *fighter) {
  kick_enter(fighter, ACTION_RETAIL_0102, 0x4e5d90);
}

/* Playback after the original controller's LP throw gates (46dcbc). */
void retail_jab(Fighter *fighter) {
  retail_native_begin(fighter, S_PUNCH, ACTION_RETAIL_010B, true);
  retail_vm_enter(fighter, 0x4e62f0, fighter_ready);
}

static bool sweep_sound(BattleEvent *event, Fighter *owner, Fighter *victim) {
  (void)event;
  (void)owner;
  (void)victim;
  battle_sound(SOUND_B, 0x1c);
  return false;
}

void retail_sweep(Fighter *fighter) {
  retail_native_begin(fighter, S_SWEEP, ACTION_RETAIL_010C, false);
  /* 46ef90 spawns independent native 46ef30, which cues B[28] after14. */
  BattleEvent *event = battle_event_start(fighter, sweep_sound, false, false);
  if (event != NULL) {
    battle_event_delay(event, 14);
  }
  retail_vm_enter(fighter, 0x4e6798, fighter_ready);
}

void retail_sweep_state(Fighter *fighter) {
  fighter_enter_state(fighter, S_SWEEP);
}

static void sweep_recover(Fighter *fighter) {
  retail_vm_forward_return(fighter);
}

/* 46efd0/46f080: submit the next pose and query once, repeat only on a
 * first miss, then mark recovery, wait one visit and finish the bound clip. */
static void sweep_query(Fighter *fighter) {
  RetailVm *vm = &fighter->retail_vm;
  require(vm->active && vm->native_rows < 2, "Sweep query without a live native continuation");
  int pose = fighter_sample(fighter);
  if (pose < fighter_move(fighter)->last) {
    fighter->sample_override = pose + 1;
  }
  uint32_t packed = mk4_image_u32(vm->image, 0x53d2b8 + vm->native_selector * 4);
  AttackRecord attack = retail_vm_attack_record(vm->image, packed * 4);
  uint64_t continuation = fighter->continuation;
  ContactResult result = battle_contact(fighter, &attack);
  if (!vm->active || fighter->continuation != continuation || fighter->pending != NULL) {
    return;
  }
  if (result != CONTACT_MISS || ++vm->native_rows == 2) {
    fighter->action = ACTION_RETAIL_1006;
    retail_vm_wait_native(fighter, 1, sweep_recover);
  } else {
    retail_vm_wait_native(fighter, 1, sweep_query);
  }
}

static void sweep_contact_start(Fighter *fighter, unsigned selector) {
  require(fighter->retail_vm.active, "Sweep native needs a live retail task");
  fighter->retail_vm.native_rows = 0;
  fighter->retail_vm.native_selector = selector;
  sweep_query(fighter);
}

void retail_sweep_standard_tail(Fighter *fighter) {
  sweep_contact_start(fighter, 35);
}

void retail_sweep_advancing_tail(Fighter *fighter) {
  sweep_contact_start(fighter, 36);
}

void retail_press_sweep(Fighter *fighter) {
  fighter_interrupt(fighter, retail_sweep);
}

/* Native 46f1f0 only stores 010d and enters the retail roster selector. */
void retail_roundhouse(Fighter *fighter) {
  bool fast = (fighter->flags & FF_FOLLOW_UP) != 0;
  mk4_script_stop_body(fighter);
  retail_native_begin(fighter, S_KICK, ACTION_RETAIL_010D, false);
  fighter->native_followup = fast;
  retail_vm_enter(fighter, 0x4e6800, fighter_ready);
}

void retail_press_roundhouse(Fighter *fighter) {
  fighter_interrupt(fighter, retail_roundhouse);
}

void retail_roundhouse_start(Fighter *fighter) {
  retail_vm_enter(fighter, fighter->native_followup ? 0x4e6860 : 0x4e6878, fighter->retail_vm.finish);
}

void retail_roundhouse_wide_start(Fighter *fighter) {
  retail_vm_enter(fighter, fighter->native_followup ? 0x4e6890 : 0x4e68a8, fighter->retail_vm.finish);
}

static void roundhouse_return(Fighter *fighter) {
  FighterFn next = fighter->retail_vm.finish;
  retail_vm_cancel(fighter);
  if (next != NULL) {
    next(fighter);
  }
}

/* 46f500 saves the query result across forward playback. Results above one
 * (ground/air hit) install the native 28-visit recovery; miss/block return. */
static void roundhouse_recover(Fighter *fighter) {
  RetailVm *vm = &fighter->retail_vm;
  int pose = fighter_sample(fighter);
  if (pose < fighter_move(fighter)->last) {
    fighter->sample_override = pose + 1;
    retail_vm_wait_native(fighter, 1, roundhouse_recover);
  } else {
    retail_vm_wait_native(fighter, vm->native_contact ? 28 : 0, roundhouse_return);
  }
}

static void roundhouse_query(Fighter *fighter) {
  RetailVm *vm = &fighter->retail_vm;
  require(vm->active && vm->native_rows < 3, "Roundhouse query without a live native continuation");
  int pose = fighter_sample(fighter);
  if (pose < fighter_move(fighter)->last) {
    fighter->sample_override = pose + 1;
  }
  uint32_t packed = mk4_image_u32(vm->image, 0x53d2b8 + vm->native_selector * 4);
  AttackRecord attack = retail_vm_attack_record(vm->image, packed * 4);
  uint64_t continuation = fighter->continuation;
  ContactResult result = battle_contact(fighter, &attack);
  if (!vm->active || fighter->continuation != continuation || fighter->pending != NULL) {
    return;
  }
  if (result != CONTACT_MISS || ++vm->native_rows == 3) {
    vm->native_contact = result == CONTACT_HIT || result == CONTACT_AIR_HIT;
    fighter->action = ACTION_RETAIL_1006;
    retail_vm_wait_native(fighter, 1, roundhouse_recover);
  } else {
    retail_vm_wait_native(fighter, 1, roundhouse_query);
  }
}

static void roundhouse_contact_start(Fighter *fighter, unsigned selector) {
  require(fighter->retail_vm.active, "Roundhouse native needs a live retail task");
  fighter->retail_vm.native_rows = 0;
  fighter->retail_vm.native_selector = selector;
  roundhouse_query(fighter);
}

void retail_roundhouse_tail(Fighter *fighter) {
  roundhouse_contact_start(fighter, 33);
}

void retail_roundhouse_wide_tail(Fighter *fighter) {
  roundhouse_contact_start(fighter, 34);
}

/* 46cdb4 playback after the finish-window controller gates. */
void retail_uppercut(Fighter *fighter) {
  retail_native_begin(fighter, S_UPPERCUT, ACTION_RETAIL_0109, false);
  retail_vm_enter(fighter, 0x4e62a0, fighter_ready);
}

void retail_uppercut_state(Fighter *fighter) {
  fighter_enter_state(fighter, S_UPPERCUT);
}

void retail_press_uppercut(Fighter *fighter) {
  fighter_interrupt(fighter, retail_uppercut);
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
  uint32_t target = along >= 0 && distance2 < 0x280000 ? 0x4de6b8 : alternate;
  FighterFn finish = fighter->retail_vm.active ? fighter->retail_vm.finish : fighter_ready;
  retail_vm_enter(fighter, target, finish);
}

void retail_lp_select_short(Fighter *fighter) {
  lp_select(fighter, 0x4de610);
}

void retail_lp_select_long(Fighter *fighter) {
  lp_select(fighter, 0x4de640);
}

/* 45b380 invokes 490bd0; pair heading is maintained by the world pump. */
void retail_face_peer(Fighter *fighter) {
  fighter->facing = fighter->body.pos[AXIS_X] <= fighter_opponent(fighter)->body.pos[AXIS_X] ? 1 : -1;
}

/* 46b710/46b740: script startup, one B[23] probe, then native recovery wait. */
void retail_crouch_low_kick(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter->body.pos[AXIS_Y] = fighter->body.ground;
  retail_native_begin(fighter, S_KICK, ACTION_RETAIL_0106, false);
  retail_vm_enter(fighter, 0x4e6158, fighter_ready);
}

static void crouch_low_kick_recover(Fighter *fighter) {
  retail_vm_enter(fighter, 0x4e6180, fighter->retail_vm.finish);
}

void retail_crouch_low_kick_tail(Fighter *fighter) {
  RetailVm *vm = &fighter->retail_vm;
  require(vm->active, "Crouch LK native needs a live retail task");
  ContactResult result = CONTACT_MISS;
  if (fighter_opponent(fighter)->action != ACTION_RETAIL_2011) {
    uint32_t packed = mk4_image_u32(vm->image, 0x53d2b8 + 23 * 4);
    AttackRecord attack = retail_vm_attack_record(vm->image, packed * 4);
    uint64_t continuation = fighter->continuation;
    result = battle_contact(fighter, &attack);
    if (!vm->active || fighter->continuation != continuation || fighter->pending != NULL) {
      return;
    }
  }
  fighter->action = ACTION_RETAIL_1006;
  retail_vm_wait_native(fighter, result == CONTACT_MISS ? 1 : 4, crouch_low_kick_recover);
}

/* Native46b5a0 delegates all authored launcher differences to selector4e6020. */
void retail_crouch_high_kick(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter->body.pos[AXIS_Y] = fighter->body.ground;
  retail_native_begin(fighter, S_KICK, ACTION_RETAIL_0104, false);
  retail_vm_enter(fighter, 0x4e6020, fighter_ready);
}

void retail_press_crouch_high_kick(Fighter *fighter) {
  fighter_interrupt(fighter, retail_crouch_high_kick);
}

void retail_press_crouch_low_kick(Fighter *fighter) {
  fighter_interrupt(fighter, retail_crouch_low_kick);
}
