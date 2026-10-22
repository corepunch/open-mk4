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
  const Mk4Image *image = fighter->retail_vm.active ? fighter->retail_vm.image : retail_vm_image();
  retail_vm_bind_animation(fighter, mk4_image_u32(image, row) * 4, 0);
}

void retail_air_attack(Fighter *fighter, unsigned kind) {
  static const int actions[] = {0x603, 0x604, 0x600, 0x602, 0x601};
  static const uint32_t scripts[] = {MK4_RETAIL_SCRIPT_AIR_HIGH_PUNCH_STARTUP_4E6658, MK4_RETAIL_SCRIPT_AIR_LOW_PUNCH_STARTUP_4E6710,
      MK4_RETAIL_SCRIPT_AIR_STRAIGHT_KICK_STARTUP_4E61F8, MK4_RETAIL_SCRIPT_AIR_DIAGONAL_HIGH_KICK_STARTUP_4E6500,
      MK4_RETAIL_SCRIPT_AIR_DIAGONAL_LOW_KICK_STARTUP_4E63D8};
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
    script = kind == 0 ? MK4_RETAIL_SCRIPT_AIR_ATTACK_4E66D0 : MK4_RETAIL_SCRIPT_AIR_ATTACK_4E66F0;
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
  air_script(fighter, MK4_RETAIL_SCRIPT_AIR_HIGH_CONTACT_END_4E66B8, false);
}

static void air_punch_contact(Fighter *fighter, bool high) {
  bool air_followup = fighter_height(fighter) > .8f || (high && fighter_airborne(fighter_opponent(fighter)));
  mk4_script_stop_body(fighter);
  fighter->action = ACTION_RETAIL_1006;
  if (air_followup) {
    if (!high) {
      air_script(fighter, MK4_RETAIL_SCRIPT_AIR_PUNCH_CONTACT_4E6280, false);
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
  air_continue(fighter, mk4_image_u32(retail_vm_image(), row + 20));
}

static void air_contact_query(Fighter *fighter) {
  uint32_t row = fighter->retail_air_row;
  const Mk4Image *image = retail_vm_image();
  if (!fighter_airborne(fighter)) {
    fighter->action = ACTION_RETAIL_1006;
    air_continue(fighter, mk4_image_u32(image, row + 16));
    return;
  }
  uint32_t query = mk4_image_u32(image, row + 8);
  require(query == RETAIL_NATIVE_CONTACT_QUERY_A_48D3D0 || query == RETAIL_NATIVE_CONTACT_QUERY_B_48D3F0, "Unsupported native air contact query");
  unsigned selector = mk4_image_u32(image, row + 4);
  require(selector < 256, "Invalid native air contact selector");
  uint32_t bank = query == RETAIL_NATIVE_CONTACT_QUERY_A_48D3D0 ? MK4_RETAIL_DATA_CONTACT_QUERY_A_TABLE_53D268 : MK4_RETAIL_DATA_CONTACT_QUERY_B_TABLE_53D2B8;
  AttackRecord record = retail_vm_attack_record(image, mk4_image_u32(image, bank + selector * 4) * 4);
  if (!fighter->connected && battle_contact(fighter, &record) != CONTACT_MISS) {
    fighter->action = ACTION_RETAIL_1006;
    air_continue(fighter, mk4_image_u32(image, row + 12));
  }
}

static void air_contact_begin(Fighter *fighter, uint32_t row) {
  unsigned count = mk4_image_u32(retail_vm_image(), row);
  require(count > 0 && count <= NATIVE_FRAME_CAPACITY, "Invalid native air contact budget");
  fighter->retail_air_row = row;
  fighter->mmove_cleanup = air_cleanup;
  native_begin(fighter, 1, (int)count, air_contact_miss);
  for (unsigned visit = 1; visit <= count; ++visit) {
    native_frame(fighter, (int)visit, fighter_sample(fighter), air_contact_query);
  }
  native_play(fighter, true);
}

static void air_opened(Fighter *fighter) {
  air_continue(fighter, mk4_image_u32(retail_vm_image(), fighter->retail_air_row + 8));
}

static void air_open_guard(Fighter *fighter) {
  if (!fighter_airborne(fighter)) {
    retail_air_land(fighter);
  }
}

static void air_open(Fighter *fighter, uint32_t row) {
  retail_air_bind_row(fighter, row);
  unsigned budget = mk4_image_u32(retail_vm_image(), row + 4);
  int count = fighter_move(fighter)->last;
  if (budget != 0 && budget < (unsigned)count) {
    count = (int)budget;
  }
  require(count > 0 && count <= NATIVE_FRAME_CAPACITY, "Invalid native air opening budget");
  fighter->retail_air_row = row;
  fighter->mmove_cleanup = air_cleanup;
  native_begin(fighter, 1, count, air_opened);
  for (int visit = 1; visit <= count; ++visit) {
    native_frame(fighter, visit, visit, air_open_guard);
  }
  native_play(fighter, false);
}

static void air_continue(Fighter *fighter, uint32_t target) {
  switch (target) {
    case RETAIL_NATIVE_AIR_CONTINUE_46E750:
      fighter->sample_override = mini(9, fighter_move(fighter)->last);
      air_contact_begin(fighter, MK4_RETAIL_DATA_AIR_CONTINUE_4E6680);
      return;
    case RETAIL_NATIVE_AIR_CONTACT_BEGIN_46EEC0:
      air_contact_begin(fighter, MK4_RETAIL_DATA_AIR_CONTINUE_4E6738);
      return;
    case RETAIL_NATIVE_AIR_CONTINUE_46E3F0:
      fighter->flags &= ~(unsigned)FF_BUSY;
      air_contact_begin(fighter, MK4_RETAIL_DATA_AIR_CONTINUE_4E6568);
      return;
    case RETAIL_NATIVE_AIR_CONTINUE_46E4F0:
      fighter->flags &= ~(unsigned)FF_BUSY;
      air_contact_begin(fighter, MK4_RETAIL_DATA_AIR_CONTINUE_4E65B0);
      return;
    case RETAIL_NATIVE_AIR_CONTINUE_46E5E0:
      fighter->flags &= ~(unsigned)FF_BUSY;
      air_contact_begin(fighter, MK4_RETAIL_DATA_AIR_CONTINUE_4E65F8);
      return;
    case RETAIL_NATIVE_AIR_PUNCH_CONTACT_46E7C0:
      air_punch_contact(fighter, true);
      return;
    case RETAIL_NATIVE_AIR_PUNCH_CONTACT_46CB10:
      air_punch_contact(fighter, false);
      return;
    case RETAIL_NATIVE_AIR_SCRIPT_46EF10:
      air_script(fighter, MK4_RETAIL_SCRIPT_AIR_CONTINUE_4E6770, false);
      return;
    case RETAIL_NATIVE_AIR_SCRIPT_46E230:
      air_script(fighter, MK4_RETAIL_SCRIPT_AIR_CONTINUE_4E64E0, false);
      return;
    case RETAIL_NATIVE_AIR_SCRIPT_46E6B0:
      air_script(fighter, MK4_RETAIL_SCRIPT_AIR_CONTINUE_4E6630, false);
      return;
    case RETAIL_NATIVE_AIR_SCRIPT_46E790:
      air_script(fighter, MK4_RETAIL_SCRIPT_AIR_CONTINUE_4E66A0, true);
      return;
    case RETAIL_NATIVE_AIR_SCRIPT_46EEE0:
      air_script(fighter, MK4_RETAIL_SCRIPT_AIR_CONTINUE_4E6758, true);
      return;
    case RETAIL_NATIVE_AIR_CONTINUE_46DFB0:
      fighter->flags &= ~(unsigned)FF_BUSY;
      air_script(fighter, MK4_RETAIL_SCRIPT_AIR_CONTINUE_4E6458, true);
      return;
    case RETAIL_NATIVE_AIR_CONTINUE_46E090:
      fighter->flags &= ~(unsigned)FF_BUSY;
      air_script(fighter, MK4_RETAIL_SCRIPT_AIR_CONTINUE_4E6490, true);
      return;
    case RETAIL_NATIVE_AIR_CONTINUE_46E170:
      fighter->flags &= ~(unsigned)FF_BUSY;
      air_script(fighter, MK4_RETAIL_SCRIPT_AIR_CONTINUE_4E64C8, true);
      return;
    case RETAIL_NATIVE_AIR_SCRIPT_46E420:
      air_script(fighter, MK4_RETAIL_SCRIPT_AIR_CONTINUE_4E6588, true);
      return;
    case RETAIL_NATIVE_AIR_SCRIPT_46E510:
      air_script(fighter, MK4_RETAIL_SCRIPT_AIR_CONTINUE_4E65D0, true);
      return;
    case RETAIL_NATIVE_AIR_SCRIPT_46E600:
      air_script(fighter, MK4_RETAIL_SCRIPT_AIR_CONTINUE_4E6618, true);
      return;
    case RETAIL_NATIVE_AIR_SCRIPT_46C810:
      air_script(fighter, MK4_RETAIL_SCRIPT_AIR_CONTINUE_4E6270, false);
      return;
    case RETAIL_NATIVE_AIR_LAND_46E240:
    case RETAIL_NATIVE_AIR_LAND_46C8F0:
      retail_air_land(fighter);
      return;
    default:
      require(false, "Unsupported native air continuation");
  }
}

void retail_air_hp_tail(Fighter *fighter) {
  air_open(fighter, MK4_RETAIL_DATA_AIR_HP_TAIL_4E6670);
}

void retail_air_lp_tail(Fighter *fighter) {
  air_open(fighter, MK4_RETAIL_DATA_AIR_LP_TAIL_4E6728);
}

void retail_air_diagonal_tail(Fighter *fighter) {
  air_open(fighter, MK4_RETAIL_DATA_AIR_DIAGONAL_TAIL_4E6558);
}

void retail_air_diagonal_alt_tail(Fighter *fighter) {
  air_open(fighter, MK4_RETAIL_DATA_AIR_DIAGONAL_ALT_TAIL_4E65A0);
}

void retail_air_diagonal_wide_tail(Fighter *fighter) {
  air_open(fighter, MK4_RETAIL_DATA_AIR_DIAGONAL_WIDE_TAIL_4E65E8);
}

void retail_air_lk_tail(Fighter *fighter) {
  air_contact_begin(fighter, MK4_RETAIL_DATA_AIR_LK_TAIL_4E6438);
}

void retail_air_lk_alt_tail(Fighter *fighter) {
  air_contact_begin(fighter, MK4_RETAIL_DATA_AIR_LK_ALT_TAIL_4E6470);
}

void retail_air_lk_wide_tail(Fighter *fighter) {
  air_contact_begin(fighter, MK4_RETAIL_DATA_AIR_LK_WIDE_TAIL_4E64A8);
}

void retail_air_straight_tail(Fighter *fighter) {
  air_contact_begin(fighter, MK4_RETAIL_DATA_AIR_STRAIGHT_TAIL_4E6258);
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
  AttackRecord record = retail_vm_attack_record(retail_vm_image(), mk4_image_u32(retail_vm_image(), MK4_RETAIL_DATA_CONTACT_QUERY_B_TABLE_53D2B8 + 39 * 4) * 4);
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
