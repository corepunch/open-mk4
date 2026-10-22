#include "retail_native.h"
#include "retail_vm.h"
#include "native.h"
#include "script.h"
#include "native_services.h"
#include "math_util.h"

/* Original native entries supply literal script operands. Selectors and authored
 * animation/control flow and native descriptor rows come from the image. */
/* Native470980's initial phase binds the descriptor in its original row. */
void retail_air_bind_row(Fighter *fighter, uint32_t row) {
  require(row >= RETAIL_SCRIPT_BEGIN && row <= RETAIL_SCRIPT_END - 12 && row % 4 == 0, "Invalid native air parameter row");
  const Mk4Image *image = fighter->execution.script.active ? fighter->execution.script.image : battle_data();
  retail_vm_bind_animation(fighter, mk4_image_u32(image, row) * 4, 0);
}

void retail_air_attack(Fighter *fighter, unsigned kind) {
  static const int actions[] = {0x603, 0x604, 0x600, 0x602, 0x601};
  static const uint32_t scripts[] = {MK4_RETAIL_SCRIPT_AIR_HIGH_PUNCH_STARTUP, MK4_RETAIL_SCRIPT_AIR_LOW_PUNCH_STARTUP,
      MK4_RETAIL_SCRIPT_AIR_STRAIGHT_KICK_STARTUP, MK4_RETAIL_SCRIPT_AIR_DIAGONAL_HIGH_KICK_STARTUP, MK4_RETAIL_SCRIPT_AIR_DIAGONAL_LOW_KICK_STARTUP};
  require(kind < sizeof scripts / sizeof *scripts, "Invalid native air attack");
  bool combo = fighter_airborne(fighter) && fighter->body.gravity == 0;
  retail_native_begin(fighter, S_JUMP, actions[kind], true);
  fighter_upright(fighter);
  fighter->script_phase = combo ? 1 : 0;
  fighter->native_variant = (int)kind;
  fighter->body.on_land = NULL;
  fighter->body.physics = NULL;
  retail_face_peer(fighter);
  uint32_t script = scripts[kind];
  if (fighter->armed && kind < 2) {
    script = kind == 0 ? MK4_RETAIL_SCRIPT_JAX_DRAW_WEAPON_AIR_HP : MK4_RETAIL_SCRIPT_JAX_DRAW_WEAPON_AIR_LP;
  }
  retail_vm_enter(fighter, script, fighter_ready);
}

void retail_air_high_punch(Fighter *fighter) {
  retail_air_attack(fighter, 0);
}

void retail_air_low_punch(Fighter *fighter) {
  retail_air_attack(fighter, 1);
}

void retail_air_kick(Fighter *fighter) {
  retail_air_attack(fighter, 2);
}

void retail_air_diagonal_high_kick(Fighter *fighter) {
  retail_air_attack(fighter, 3);
}

void retail_air_diagonal_low_kick(Fighter *fighter) {
  retail_air_attack(fighter, 4);
}

/* Native470980 and4704d0 consume original descriptor/contact rows. These are
 * shared native controllers; waits, reverse clips and recovery branches remain
 * original script words. See mk4decomp five_block_dispatch_variants.c. */
static void air_continue(Fighter *fighter, uint32_t target);
static void air_contact_begin(Fighter *fighter, uint32_t row);

void retail_air_land(Fighter *fighter) {
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
  if (fighter->body.on_land == retail_air_land) {
    fighter->body.on_land = NULL;
  }
}

static void air_wait_land(Fighter *fighter) {
  fighter->buttons = NULL;
  if (fighter->body.gravity == 0) {
    fighter->body.gravity = 0x51e;
  }
  native_await(fighter, native_grounded, retail_air_land);
}

void retail_air_straight_land(Fighter *fighter) {
  air_wait_land(fighter);
}

static void air_script(Fighter *fighter, uint32_t address, bool monitor_landing) {
  fighter->body.on_land = monitor_landing ? retail_air_land : NULL;
  retail_vm_enter(fighter, address, fighter_ready);
}

static void air_low_contact_end(Fighter *fighter) {
  fighter->body.gravity = 0x28f;
  air_wait_land(fighter);
}

static void air_high_contact_end(Fighter *fighter) {
  fighter->buttons = NULL;
  air_script(fighter, MK4_RETAIL_SCRIPT_AIR_HIGH_CONTACT_END, false);
}

static void air_punch_contact(Fighter *fighter, bool high) {
  bool air_followup = fighter_height(fighter) > .8f || (high && fighter_airborne(fighter_opponent(fighter)));
  mk4_script_stop_body(fighter);
  fighter->action = ACTION_RETAIL_1006;
  if (air_followup) {
    if (!high) {
      air_script(fighter, MK4_RETAIL_SCRIPT_AIR_PUNCH_CONTACT, false);
      return;
    }
    fighter->buttons = &retail_air_buttons[2];
    fighter->flags &= ~(unsigned)FF_BUSY;
    native_hold(fighter, 12, air_high_contact_end);
  } else {
    fighter->body.pos[AXIS_Y] = fighter->body.ground;
    fighter->buttons = &retail_close_buttons[3];
    fighter->flags |= FF_FOLLOW_UP;
    fighter->flags &= ~(unsigned)FF_BUSY;
    native_hold(fighter, 12, air_low_contact_end);
  }
}

static void air_contact_miss(Fighter *fighter) {
  uint32_t row = fighter->retail_air_row;
  air_continue(fighter, mk4_image_u32(battle_data(), row + 20));
}

static void air_contact_query(Fighter *fighter) {
  uint32_t row = fighter->retail_air_row;
  const Mk4Image *image = battle_data();
  if (!fighter_airborne(fighter)) {
    fighter->action = ACTION_RETAIL_1006;
    air_continue(fighter, mk4_image_u32(image, row + 16));
    return;
  }
  uint32_t query = mk4_image_u32(image, row + 8);
  require(query == RETAIL_NATIVE_CONTACT_QUERY_A || query == RETAIL_NATIVE_CONTACT_QUERY_B, "Unsupported native air contact query");
  unsigned selector = mk4_image_u32(image, row + 4);
  require(selector < 256, "Invalid native air contact selector");
  uint32_t bank = query == RETAIL_NATIVE_CONTACT_QUERY_A ? MK4_RETAIL_DATA_CONTACT_QUERY_A_TABLE : MK4_RETAIL_DATA_CONTACT_QUERY_B_TABLE;
  AttackRecord record = retail_vm_attack_record(image, mk4_image_u32(image, bank + selector * 4) * 4);
  if (!fighter->connected && battle_contact(fighter, &record) != CONTACT_MISS) {
    fighter->action = ACTION_RETAIL_1006;
    air_continue(fighter, mk4_image_u32(image, row + 12));
  }
}

static void air_contact_begin(Fighter *fighter, uint32_t row) {
  unsigned count = mk4_image_u32(battle_data(), row);
  require(count > 0 && count <= NATIVE_FRAME_CAPACITY, "Invalid native air contact budget");
  fighter->retail_air_row = row;
  fighter->execution.cleanup = air_cleanup;
  native_begin(fighter, 1, (int)count, air_contact_miss);
  for (unsigned visit = 1; visit <= count; ++visit) {
    native_frame(fighter, (int)visit, fighter_sample(fighter), air_contact_query);
  }
  native_play(fighter, true);
}

static void air_opened(Fighter *fighter) {
  air_continue(fighter, mk4_image_u32(battle_data(), fighter->retail_air_row + 8));
}

static void air_open_guard(Fighter *fighter) {
  if (!fighter_airborne(fighter)) {
    retail_air_land(fighter);
  }
}

static void air_open(Fighter *fighter, uint32_t row) {
  retail_air_bind_row(fighter, row);
  unsigned budget = mk4_image_u32(battle_data(), row + 4);
  int count = fighter_move(fighter)->last;
  if (budget != 0 && budget < (unsigned)count) {
    count = (int)budget;
  }
  require(count > 0 && count <= NATIVE_FRAME_CAPACITY, "Invalid native air opening budget");
  fighter->retail_air_row = row;
  fighter->execution.cleanup = air_cleanup;
  native_begin(fighter, 1, count, air_opened);
  for (int visit = 1; visit <= count; ++visit) {
    native_frame(fighter, visit, visit, air_open_guard);
  }
  native_play(fighter, false);
}

static void air_continue(Fighter *fighter, uint32_t target) {
  switch (target) {
    case RETAIL_NATIVE_AIR_HIGH_PUNCH_CONTACT_WINDOW:
      fighter->sample_override = mini(9, fighter_move(fighter)->last);
      air_contact_begin(fighter, MK4_RETAIL_DATA_AIR_HIGH_PUNCH_CONTACT_ROW);
      return;
    case RETAIL_NATIVE_AIR_CONTACT_BEGIN:
      air_contact_begin(fighter, MK4_RETAIL_DATA_AIR_LOW_PUNCH_CONTACT_ROW);
      return;
    case RETAIL_NATIVE_AIR_DIAGONAL_KICK_QUERY_30:
      fighter->flags &= ~(unsigned)FF_BUSY;
      air_contact_begin(fighter, MK4_RETAIL_DATA_AIR_DIAGONAL_KICK_QUERY_30_ROW);
      return;
    case RETAIL_NATIVE_AIR_DIAGONAL_KICK_QUERY_31:
      fighter->flags &= ~(unsigned)FF_BUSY;
      air_contact_begin(fighter, MK4_RETAIL_DATA_AIR_DIAGONAL_KICK_QUERY_31_ROW);
      return;
    case RETAIL_NATIVE_AIR_DIAGONAL_KICK_QUERY_32:
      fighter->flags &= ~(unsigned)FF_BUSY;
      air_contact_begin(fighter, MK4_RETAIL_DATA_AIR_DIAGONAL_KICK_QUERY_32_ROW);
      return;
    case RETAIL_NATIVE_GORO_AIR_HP_RECOVERY_CALLBACK:
      air_punch_contact(fighter, true);
      return;
    case RETAIL_NATIVE_GORO_AIR_KICK_RECOVERY_CALLBACK:
      air_punch_contact(fighter, false);
      return;
    case RETAIL_NATIVE_GORO_AIR_LP_RECOVERY_CALLBACK:
      air_script(fighter, MK4_RETAIL_SCRIPT_AIR_LOW_PUNCH_CONTACT_HOLD, false);
      return;
    case RETAIL_NATIVE_AIR_LOW_KICK_CONTACT_HOLD:
      air_script(fighter, MK4_RETAIL_SCRIPT_AIR_LOW_KICK_CONTACT_HOLD, false);
      return;
    case RETAIL_NATIVE_AIR_DIAGONAL_KICK_CONTACT_HOLD:
      air_script(fighter, MK4_RETAIL_SCRIPT_AIR_DIAGONAL_KICK_CONTACT_HOLD, false);
      return;
    case RETAIL_NATIVE_AIR_HIGH_PUNCH_MISS_REVERSE:
      air_script(fighter, MK4_RETAIL_SCRIPT_AIR_HIGH_PUNCH_MISS_REVERSE, true);
      return;
    case RETAIL_NATIVE_AIR_LOW_PUNCH_MISS_REVERSE:
      air_script(fighter, MK4_RETAIL_SCRIPT_AIR_LOW_PUNCH_MISS_REVERSE, true);
      return;
    case RETAIL_NATIVE_AIR_LOW_KICK_CLIP_58_REVERSE:
      fighter->flags &= ~(unsigned)FF_BUSY;
      air_script(fighter, MK4_RETAIL_SCRIPT_AIR_LOW_KICK_CLIP_58_REVERSE, true);
      return;
    case RETAIL_NATIVE_AIR_LOW_KICK_CLIP_59_REVERSE:
      fighter->flags &= ~(unsigned)FF_BUSY;
      air_script(fighter, MK4_RETAIL_SCRIPT_AIR_LOW_KICK_CLIP_59_REVERSE, true);
      return;
    case RETAIL_NATIVE_AIR_LOW_KICK_CLIP_60_REVERSE:
      fighter->flags &= ~(unsigned)FF_BUSY;
      air_script(fighter, MK4_RETAIL_SCRIPT_AIR_LOW_KICK_CLIP_60_REVERSE, true);
      return;
    case RETAIL_NATIVE_AIR_DIAGONAL_KICK_CLIP_55_REVERSE:
      air_script(fighter, MK4_RETAIL_SCRIPT_AIR_DIAGONAL_KICK_CLIP_55_REVERSE, true);
      return;
    case RETAIL_NATIVE_AIR_DIAGONAL_KICK_CLIP_56_REVERSE:
      air_script(fighter, MK4_RETAIL_SCRIPT_AIR_DIAGONAL_KICK_CLIP_56_REVERSE, true);
      return;
    case RETAIL_NATIVE_AIR_DIAGONAL_KICK_CLIP_57_REVERSE:
      air_script(fighter, MK4_RETAIL_SCRIPT_AIR_DIAGONAL_KICK_CLIP_57_REVERSE, true);
      return;
    case RETAIL_NATIVE_AIR_STRAIGHT_KICK_MISS_REVERSE:
      air_script(fighter, MK4_RETAIL_SCRIPT_CAGE_AIR_KICK_NARROW_MISS_REVERSAL, false);
      return;
    case RETAIL_NATIVE_AIR_ATTACK_LAND_OR_FINISH:
    case RETAIL_NATIVE_AIR_STRAIGHT_KICK_LAND:
      retail_air_land(fighter);
      return;
    default:
      require(false, "Unsupported native air continuation");
  }
}

void retail_air_hp_tail(Fighter *fighter) {
  air_open(fighter, MK4_RETAIL_DATA_AIR_HP_TAIL);
}

void retail_air_lp_tail(Fighter *fighter) {
  air_open(fighter, MK4_RETAIL_DATA_AIR_LP_TAIL);
}

void retail_air_diagonal_tail(Fighter *fighter) {
  air_open(fighter, MK4_RETAIL_DATA_AIR_DIAGONAL_TAIL);
}

void retail_air_diagonal_alt_tail(Fighter *fighter) {
  air_open(fighter, MK4_RETAIL_DATA_AIR_DIAGONAL_ALT_TAIL);
}

void retail_air_diagonal_wide_tail(Fighter *fighter) {
  air_open(fighter, MK4_RETAIL_DATA_AIR_DIAGONAL_WIDE_TAIL);
}

void retail_air_lk_tail(Fighter *fighter) {
  air_contact_begin(fighter, MK4_RETAIL_DATA_AIR_LK_TAIL);
}

void retail_air_lk_alt_tail(Fighter *fighter) {
  air_contact_begin(fighter, MK4_RETAIL_DATA_AIR_LK_ALT_TAIL);
}

void retail_air_lk_wide_tail(Fighter *fighter) {
  air_contact_begin(fighter, MK4_RETAIL_DATA_AIR_LK_WIDE_TAIL);
}

void retail_air_straight_tail(Fighter *fighter) {
  air_contact_begin(fighter, MK4_RETAIL_DATA_AIR_STRAIGHT_TAIL);
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
  AttackRecord record = retail_vm_attack_record(battle_data(), mk4_image_u32(battle_data(), MK4_RETAIL_DATA_CONTACT_QUERY_B_TABLE + 39 * 4) * 4);
  if (!fighter->connected && battle_contact(fighter, &record) != CONTACT_MISS) {
    air_punch_contact(fighter, false);
  }
}

void retail_air_straight_wide_tail(Fighter *fighter) {
  int first = fighter_sample(fighter);
  native_begin(fighter, 1, 3, air_wide_miss);
  for (int visit = 1; visit <= 3; ++visit) {
    native_frame(fighter, visit, mini(first + visit, fighter_move(fighter)->last), air_wide_query);
  }
  native_play(fighter, true);
}
