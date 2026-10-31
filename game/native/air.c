#include "script/native_ids.h"
#include "native/actions.h"
#include "script/vm.h"
#include "runtime/playback.h"
#include "native/helpers.h"
#include "native/capabilities.h"
#include "math_util.h"

/* Original native entries supply literal script operands. Selectors and authored
 * animation/control flow and native descriptor rows come from the image. */
/* Native470980's initial phase binds the descriptor in its original row. */
void action_air_bind_row(Fighter *fighter, uint32_t row) {
  require(row >= MK4_DATA_IMAGE_BASE && row <= MK4_DATA_IMAGE_END - 12 && row % 4 == 0, "Invalid native air parameter row");
  const Mk4Image *image = fighter->execution.script.active ? fighter->execution.script.image : battle_data();
  script_vm_bind_animation(fighter, mk4_image_u32(image, row) * 4, 0);
}

void action_air_attack(Fighter *fighter, unsigned kind) {
  static const int actions[] = {0x603, 0x604, 0x600, 0x602, 0x601};
  static const uint32_t scripts[] = {
      fn_air_high_punch_startup, fn_air_low_punch_startup, fn_air_straight_kick_startup, fn_air_diagonal_high_kick_startup, fn_air_diagonal_low_kick_startup};
  require(kind < sizeof scripts / sizeof *scripts, "Invalid native air attack");
  bool combo = fighter_airborne(fighter) && fighter->body.gravity == 0;
  action_begin(fighter, S_JUMP, actions[kind], true);
  fighter_upright(fighter);
  fighter->script_phase = combo ? 1 : 0;
  fighter->native_variant = (int)kind;
  fighter->body.on_land = NULL;
  fighter->body.physics = NULL;
  action_face_peer(fighter);
  uint32_t script = scripts[kind];
  if (fighter->armed && kind < 2) {
    script = kind == 0 ? fn_jax_draw_weapon_air_hp : fn_jax_draw_weapon_air_lp;
  }
  script_vm_enter(fighter, script, fighter_ready);
}

void action_air_high_punch(Fighter *fighter) {
  action_air_attack(fighter, 0);
}

void action_air_low_punch(Fighter *fighter) {
  action_air_attack(fighter, 1);
}

void action_air_kick(Fighter *fighter) {
  action_air_attack(fighter, 2);
}

void action_air_diagonal_high_kick(Fighter *fighter) {
  action_air_attack(fighter, 3);
}

void action_air_diagonal_low_kick(Fighter *fighter) {
  action_air_attack(fighter, 4);
}

/* Native470980 and4704d0 consume original descriptor/contact rows. These are
 * shared native controllers; waits, reverse clips and recovery branches remain
 * original script words. See mk4decomp five_block_dispatch_variants.c. */
static void air_continue(Fighter *fighter, uint32_t target);
static void air_contact_begin(Fighter *fighter, uint32_t row);

void action_air_land(Fighter *fighter) {
  fighter->flags &= ~(unsigned)FF_FOLLOW_UP;
  fighter->buttons = NULL;
  fighter->body.on_land = NULL;
  mk4_land(fighter);
}

static void air_cleanup(Fighter *fighter) {
  if (fighter_airborne(fighter) && fighter->body.gravity == 0) {
    fighter->body.gravity = 0x51e;
  }
  fighter->buttons = NULL;
  if (fighter->body.on_land == action_air_land) {
    fighter->body.on_land = NULL;
  }
}

static void air_wait_land(Fighter *fighter) {
  fighter->buttons = NULL;
  if (fighter->body.gravity == 0) {
    fighter->body.gravity = 0x51e;
  }
  native_await(fighter, native_grounded, action_air_land);
}

void action_air_straight_land(Fighter *fighter) {
  air_wait_land(fighter);
}

static void air_script(Fighter *fighter, uint32_t address, bool monitor_landing) {
  fighter->body.on_land = monitor_landing ? action_air_land : NULL;
  script_vm_enter(fighter, address, fighter_ready);
}

static void air_low_contact_end(Fighter *fighter) {
  fighter->body.gravity = 0x28f;
  air_wait_land(fighter);
}

static void air_high_contact_end(Fighter *fighter) {
  fighter->buttons = NULL;
  air_script(fighter, fn_air_high_contact_end, false);
}

static void air_punch_contact(Fighter *fighter, bool high) {
  bool air_followup = fighter_height(fighter) > .8f || (high && fighter_airborne(fighter_opponent(fighter)));
  mk4_script_stop_body(fighter);
  fighter->action = ACTION_CODE_1006;
  if (air_followup) {
    if (!high) {
      air_script(fighter, fn_air_punch_contact, false);
      return;
    }
    fighter->buttons = &action_air_buttons[2];
    fighter->flags &= ~(unsigned)FF_BUSY;
    native_hold(fighter, 12, air_high_contact_end);
  } else {
    fighter->body.pos[AXIS_Y] = fighter->body.ground;
    fighter->buttons = &action_close_buttons[3];
    fighter->flags |= FF_FOLLOW_UP;
    fighter->flags &= ~(unsigned)FF_BUSY;
    native_hold(fighter, 12, air_low_contact_end);
  }
}

static void air_contact_miss(Fighter *fighter) {
  uint32_t row = fighter->action_air_row;
  air_continue(fighter, mk4_image_u32(battle_data(), row + 20));
}

static void air_contact_query(Fighter *fighter) {
  uint32_t row = fighter->action_air_row;
  const Mk4Image *image = battle_data();
  if (!fighter_airborne(fighter)) {
    fighter->action = ACTION_CODE_1006;
    air_continue(fighter, mk4_image_u32(image, row + 16));
    return;
  }
  uint32_t query = mk4_image_u32(image, row + 8);
  require(query == native_contact_query_a || query == native_contact_query_b, "Unsupported native air contact query");
  unsigned selector = mk4_image_u32(image, row + 4);
  require(selector < 256, "Invalid native air contact selector");
  uint32_t bank = query == native_contact_query_a ? dat_contact_query_a_table : dat_contact_query_b_table;
  AttackRecord record = script_vm_attack_record(image, mk4_image_u32(image, bank + selector * 4) * 4);
  if (!fighter->connected && battle_contact(fighter, &record) != CONTACT_MISS) {
    fighter->action = ACTION_CODE_1006;
    air_continue(fighter, mk4_image_u32(image, row + 12));
  }
}

static void air_contact_begin(Fighter *fighter, uint32_t row) {
  unsigned count = mk4_image_u32(battle_data(), row);
  require(count > 0 && count <= NATIVE_FRAME_CAPACITY, "Invalid native air contact budget");
  fighter->action_air_row = row;
  fighter->execution.cleanup = air_cleanup;
  native_begin(fighter, 1, (int)count, air_contact_miss);
  for (unsigned visit = 1; visit <= count; ++visit) {
    native_frame(fighter, (int)visit, fighter_sample(fighter), air_contact_query);
  }
  native_play(fighter, true);
}

static void air_opened(Fighter *fighter) {
  air_continue(fighter, mk4_image_u32(battle_data(), fighter->action_air_row + 8));
}

static void air_open_guard(Fighter *fighter) {
  if (!fighter_airborne(fighter)) {
    action_air_land(fighter);
  }
}

static void air_open(Fighter *fighter, uint32_t row) {
  action_air_bind_row(fighter, row);
  unsigned budget = mk4_image_u32(battle_data(), row + 4);
  int count = fighter_move(fighter)->last;
  if (budget != 0 && budget < (unsigned)count) {
    count = (int)budget;
  }
  require(count > 0 && count <= NATIVE_FRAME_CAPACITY, "Invalid native air opening budget");
  fighter->action_air_row = row;
  fighter->execution.cleanup = air_cleanup;
  native_begin(fighter, 1, count, air_opened);
  for (int visit = 1; visit <= count; ++visit) {
    native_frame(fighter, visit, visit, air_open_guard);
  }
  native_play(fighter, false);
}

static void air_continue(Fighter *fighter, uint32_t target) {
  switch (target) {
    case native_air_high_punch_contact_window:
      fighter->sample_override = mini(9, fighter_move(fighter)->last);
      air_contact_begin(fighter, dat_air_high_punch_contact_row);
      return;
    case native_air_contact_begin:
      air_contact_begin(fighter, dat_air_low_punch_contact_row);
      return;
    case native_air_diagonal_kick_query_30:
      fighter->flags &= ~(unsigned)FF_BUSY;
      air_contact_begin(fighter, dat_air_diagonal_kick_query_30_row);
      return;
    case native_air_diagonal_kick_query_31:
      fighter->flags &= ~(unsigned)FF_BUSY;
      air_contact_begin(fighter, dat_air_diagonal_kick_query_31_row);
      return;
    case native_air_diagonal_kick_query_32:
      fighter->flags &= ~(unsigned)FF_BUSY;
      air_contact_begin(fighter, dat_air_diagonal_kick_query_32_row);
      return;
    case native_goro_air_hp_recovery_callback:
      air_punch_contact(fighter, true);
      return;
    case native_goro_air_kick_recovery_callback:
      air_punch_contact(fighter, false);
      return;
    case native_goro_air_lp_recovery_callback:
      air_script(fighter, fn_air_low_punch_contact_hold, false);
      return;
    case native_air_low_kick_contact_hold:
      air_script(fighter, fn_air_low_kick_contact_hold, false);
      return;
    case native_air_diagonal_kick_contact_hold:
      air_script(fighter, fn_air_diagonal_kick_contact_hold, false);
      return;
    case native_air_high_punch_miss_reverse:
      air_script(fighter, fn_air_high_punch_miss_reverse, true);
      return;
    case native_air_low_punch_miss_reverse:
      air_script(fighter, fn_air_low_punch_miss_reverse, true);
      return;
    case native_air_low_kick_clip_58_reverse:
      fighter->flags &= ~(unsigned)FF_BUSY;
      air_script(fighter, fn_air_low_kick_clip_58_reverse, true);
      return;
    case native_air_low_kick_clip_59_reverse:
      fighter->flags &= ~(unsigned)FF_BUSY;
      air_script(fighter, fn_air_low_kick_clip_59_reverse, true);
      return;
    case native_air_low_kick_clip_60_reverse:
      fighter->flags &= ~(unsigned)FF_BUSY;
      air_script(fighter, fn_air_low_kick_clip_60_reverse, true);
      return;
    case native_air_diagonal_kick_clip_55_reverse:
      air_script(fighter, fn_air_diagonal_kick_clip_55_reverse, true);
      return;
    case native_air_diagonal_kick_clip_56_reverse:
      air_script(fighter, fn_air_diagonal_kick_clip_56_reverse, true);
      return;
    case native_air_diagonal_kick_clip_57_reverse:
      air_script(fighter, fn_air_diagonal_kick_clip_57_reverse, true);
      return;
    case native_air_straight_kick_miss_reverse:
      air_script(fighter, fn_cage_air_kick_narrow_miss_reversal, false);
      return;
    case native_air_attack_land_or_finish:
    case native_air_straight_kick_land:
      action_air_land(fighter);
      return;
    default:
      require(false, "Unsupported native air continuation");
  }
}

void action_air_hp_tail(Fighter *fighter) {
  air_open(fighter, dat_air_hp_tail);
}

void action_air_lp_tail(Fighter *fighter) {
  air_open(fighter, dat_air_lp_tail);
}

void action_air_diagonal_tail(Fighter *fighter) {
  air_open(fighter, dat_air_diagonal_tail);
}

void action_air_diagonal_alt_tail(Fighter *fighter) {
  air_open(fighter, dat_air_diagonal_alt_tail);
}

void action_air_diagonal_wide_tail(Fighter *fighter) {
  air_open(fighter, dat_air_diagonal_wide_tail);
}

void action_air_lk_tail(Fighter *fighter) {
  air_contact_begin(fighter, dat_air_lk_tail);
}

void action_air_lk_alt_tail(Fighter *fighter) {
  air_contact_begin(fighter, dat_air_lk_alt_tail);
}

void action_air_lk_wide_tail(Fighter *fighter) {
  air_contact_begin(fighter, dat_air_lk_wide_tail);
}

void action_air_straight_tail(Fighter *fighter) {
  air_contact_begin(fighter, dat_air_straight_tail);
}

static void air_wide_landed(Fighter *fighter) {
  fighter->sample_override = mini(23, fighter_move(fighter)->last);
  native_finish_clip(fighter, mk4_return_to_stance);
}

static void air_wide_miss(Fighter *fighter) {
  fighter->flags &= ~(unsigned)FF_BUSY;
  if (fighter->body.gravity == 0) {
    fighter->body.gravity = 0x51e;
  }
  native_await(fighter, native_grounded, air_wide_landed);
}

static void air_wide_query(Fighter *fighter) {
  if (!fighter_airborne(fighter)) {
    air_wide_landed(fighter);
    return;
  }
  AttackRecord record = script_vm_attack_record(battle_data(), mk4_image_u32(battle_data(), dat_contact_query_b_table + 39 * 4) * 4);
  if (!fighter->connected && battle_contact(fighter, &record) != CONTACT_MISS) {
    air_punch_contact(fighter, false);
  }
}

void action_air_straight_wide_tail(Fighter *fighter) {
  int first = fighter_sample(fighter);
  native_begin(fighter, 1, 3, air_wide_miss);
  for (int visit = 1; visit <= 3; ++visit) {
    native_frame(fighter, visit, mini(first + visit, fighter_move(fighter)->last), air_wide_query);
  }
  native_play(fighter, true);
}
