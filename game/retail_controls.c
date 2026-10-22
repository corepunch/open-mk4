#include "retail_layout.h"
#include "retail_native.h"
#include "retail_data.h"
#include "retail_vm.h"
#include "native.h"
#include "script.h"
#include "native_services.h"

/* Views contain addresses only; button targets remain in the loaded image. */
const ButtonTable retail_close_buttons[] = {
    {.retail_address = MK4_RETAIL_INPUT_TABLE_CLOSE_PRIMARY},
    {.retail_address = MK4_RETAIL_INPUT_TABLE_CLOSE_SECONDARY},
    {.retail_address = MK4_RETAIL_INPUT_TABLE_CLOSE_TERTIARY},
    {.retail_address = MK4_RETAIL_INPUT_TABLE_CLOSE_FOLLOWUP},
};

const ButtonTable retail_ground_buttons[] = {
    {.retail_address = MK4_RETAIL_INPUT_TABLE_STANDING},
    {.retail_address = MK4_RETAIL_INPUT_TABLE_CROUCHING},
};

const ButtonTable retail_air_buttons[] = {
    {.retail_address = MK4_RETAIL_INPUT_TABLE_STRAIGHT_AIR},
    {.retail_address = MK4_RETAIL_INPUT_TABLE_DIAGONAL_AIR},
    {.retail_address = MK4_RETAIL_INPUT_TABLE_AIR_COMBO},
    {.retail_address = MK4_RETAIL_INPUT_TABLE_TELEPORT_AIR},
};

const Controls retail_stances = {.stances = {
                                     [STANCE_STANDING] = &retail_ground_buttons[0],
                                     [STANCE_CROUCHING] = &retail_ground_buttons[1],
                                     [STANCE_JUMP] = &retail_air_buttons[0],
                                     [STANCE_DIAGONAL_AIR] = &retail_air_buttons[1],
                                 }};

/* Original thin controllers dispatch their0x80000..0x80004 action words. */
static bool press_air_controller(Fighter *fighter, uint32_t target) {
  unsigned action;
  switch (target) {
    case RETAIL_NATIVE_PRESS_STRAIGHT_AIR_HIGH_PUNCH:
    case RETAIL_NATIVE_PRESS_DIAGONAL_AIR_HIGH_PUNCH:
      action = 0;
      break;
    case RETAIL_NATIVE_PRESS_STRAIGHT_AIR_LOW_PUNCH:
    case RETAIL_NATIVE_PRESS_DIAGONAL_AIR_LOW_PUNCH:
      action = 1;
      break;
    case RETAIL_NATIVE_PRESS_DIAGONAL_AIR_HIGH_KICK:
      action = 2;
      break;
    case RETAIL_NATIVE_PRESS_DIAGONAL_AIR_LOW_KICK:
      action = 3;
      break;
    case RETAIL_NATIVE_PRESS_STRAIGHT_AIR_KICK:
      action = 4;
      break;
    default:
      return false;
  }
  uint32_t entry = retail_action_target(battle_data(), 8, action);
  fighter_interrupt(fighter, retail_native_entry(entry));
  return true;
}

static FighterFn action_entry(unsigned index) {
  uint32_t target = retail_action_target(battle_data(), 1, index);
  return retail_native_entry(target);
}

static void enter_action(Fighter *fighter, unsigned index) {
  action_entry(index)(fighter);
}

static FighterFn crouch_entry(unsigned index) {
  uint32_t target = mk4_image_u32(battle_data(), MK4_RETAIL_DATA_CROUCH_ENTRY + index * 4);
  return retail_native_entry(target);
}

uint32_t retail_button_target(const Mk4Image *image, uint32_t table, Button button) {
  require(button >= BUTTON_HIGH_PUNCH && button < BUTTON_COUNT, "Invalid retail button slot");
  require(table >= RETAIL_SCRIPT_BEGIN && table <= RETAIL_SCRIPT_END - 24 && table % 4 == 0, "Invalid retail button table");
  unsigned slot;
  switch (button) {
    case BUTTON_HIGH_PUNCH:
      slot = 0;
      break;
    case BUTTON_LOW_PUNCH:
      slot = 1;
      break;
    case BUTTON_BLOCK:
      slot = 2;
      break;
    case BUTTON_HIGH_KICK:
      slot = 3;
      break;
    case BUTTON_LOW_KICK:
      slot = 4;
      break;
    case BUTTON_RUN:
      slot = 5;
      break;
    default:
      fatal("Invalid retail button");
  }
  uint32_t target = mk4_image_u32(image, table + slot * 4);
  require(target == 0 || (target >= MK4_RETAIL_DATA_IMAGE_BASE && target < MK4_RETAIL_DATA_PACKED_POINTERS_BEGIN), "Invalid retail button target");
  return target;
}

void retail_combo_high_punch(Fighter *fighter) {
  unsigned slot = retail_native_character_slot((unsigned)fighter->roster);
  uint32_t entry = mk4_image_u32(battle_data(), MK4_RETAIL_DATA_COMBO_HIGH_PUNCH + slot * 4);
  uint32_t script;
  switch (entry) {
    case RETAIL_NATIVE_JAX_COMBO_HP_ENTRY:
      script = MK4_RETAIL_SCRIPT_JAX_COMBO_HP_ATTACKER;
      break;
    case RETAIL_NATIVE_COMBO_HP_ENTRY:
      script = MK4_RETAIL_SCRIPT_SUB_ZERO_COMBO_HP_ATTACKER;
      break;
    case RETAIL_NATIVE_KITANA_COMBO_HP_ENTRY:
      script = MK4_RETAIL_SCRIPT_KITANA_COMBO_HIGH_PUNCH_ATTACKER;
      break;
    default:
      fatal("Unimplemented native combo entry %08x", entry);
  }
  retail_native_begin(fighter, S_PUNCH, ACTION_RETAIL_0108, false);
  retail_vm_enter(fighter, script, fighter_ready);
}

static void normal_hp(Fighter *fighter) {
  const Fighter *peer = fighter_opponent(fighter);
  if (fighter_distance(fighter) <= q16_float(0xcccc) && !fighter_airborne(peer) && (peer->action & 0xf000) != 0x4000) {
    enter_action(fighter, 9);
  } else {
    enter_action(fighter, 11);
  }
}

/* Native46c3f0 can force a close punch even when the grounded peer is reacting. */
static void air_ground_hp(Fighter *fighter) {
  if (fighter_distance(fighter) <= q16_float(0xcccc) && !fighter_airborne(fighter_opponent(fighter))) {
    enter_action(fighter, 9);
  } else {
    normal_hp(fighter);
  }
}

static FighterFn normal_hk_entry(Fighter *fighter) {
  const Fighter *peer = fighter_opponent(fighter);
  if (fighter_holds_back(fighter)) {
    return action_entry(7);
  } else if (fighter_distance(fighter) <= q16_float(0xcccc) && !fighter_airborne(peer) && peer->action != ACTION_RETAIL_4005 &&
             peer->action != ACTION_RETAIL_4001) {
    return action_entry(12);
  } else {
    return action_entry(5);
  }
}

void retail_normal_low_punch(Fighter *fighter) {
  (mk4_can_ground_throw(fighter, false) ? retail_lp_throw : retail_jab)(fighter);
}

void retail_normal_low_kick(Fighter *fighter) {
  (mk4_can_ground_throw(fighter, true) ? retail_lk_throw : retail_low_kick)(fighter);
}

/* Native46b5d0 supplies only the literal script operand, not authored logic. */
static void follow_crouch_hk(Fighter *fighter) {
  int action = fighter->action;
  retail_native_begin(fighter, S_KICK, action, false);
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_FOLLOW_CROUCH_HK, fighter_ready);
}

/* Controllers46c210..46c740. Native weapon frontends remain transitional services. */
void retail_press_native_button(Fighter *fighter, uint32_t target) {
  if (target == 0 || press_air_controller(fighter, target)) {
    return;
  }
  if (target == RETAIL_NATIVE_TELEPORT_SLAM) {
    fighter_interrupt(fighter, retail_native_entry(target));
    return;
  }
  if (target == RETAIL_NATIVE_HANDSTAND_SPIN || target == RETAIL_NATIVE_HANDSTAND_EXIT || target == RETAIL_NATIVE_HANDSTAND_FRONT ||
      target == RETAIL_NATIVE_HANDSTAND_REAR) {
    fighter_interrupt(fighter, retail_native_entry(target));
    return;
  }
  const Fighter *peer = fighter_opponent(fighter);
  bool down = fighter_height(fighter) <= .5f && (fighter->held & BUTTON_BIT(BUTTON_DOWN));
  FighterFn entry = NULL;
  switch (target) {
    case RETAIL_NATIVE_PRESS_STANDING_HIGH_PUNCH:
      entry = fighter->armed && fighter->weapon != NULL ? (fighter_holds_back(fighter) ? retail_weapon_back_high_punch : retail_weapon_high_punch) : normal_hp;
      break;
    case RETAIL_NATIVE_PRESS_STANDING_LOW_PUNCH:
      entry =
          fighter->armed && fighter->weapon != NULL ? (fighter_holds_back(fighter) ? retail_weapon_back_low_punch : retail_weapon_low_punch) : action_entry(8);
      break;
    case RETAIL_NATIVE_PRESS_STANDING_LOW_KICK:
      entry = action_entry(fighter_holds_back(fighter) ? 6 : 4);
      break;
    case RETAIL_NATIVE_PRESS_STANDING_HIGH_KICK:
      entry = normal_hk_entry(fighter);
      break;
    case RETAIL_NATIVE_PRESS_CROUCHING_HIGH_PUNCH:
      /* The engine command matcher owns finish command gates before fallback. */
      {
        uint32_t target = retail_action_target(battle_data(), 1, 1);
        entry = target == RETAIL_NATIVE_UPPERCUT_ENTRY ? retail_uppercut : retail_native_entry(target);
      }
      break;
    case RETAIL_NATIVE_PRESS_CROUCHING_LOW_PUNCH:
      if (fighter->armed && fighter->weapon != NULL && fighter->weapon->type == 0x60) {
        entry = retail_sword_spin;
      } else {
        entry = crouch_entry(8);
      }
      break;
    case RETAIL_NATIVE_PRESS_CROUCHING_HIGH_KICK:
      entry = crouch_entry(7);
      break;
    case RETAIL_NATIVE_PRESS_CROUCHING_LOW_KICK:
      entry = crouch_entry(6);
      break;
    case RETAIL_NATIVE_PRESS_STANDING_BLOCK:
    case RETAIL_NATIVE_PRESS_CROUCHING_BLOCK:
      /* Held-block movement/state ownership is handled by the simulation pump. */
      return;
    case RETAIL_NATIVE_PRESS_CLOSE_PUNCH_COMBO_HIGH_PUNCH:
    case RETAIL_NATIVE_PRESS_AIR_CONTACT_HIGH_PUNCH:
    case RETAIL_NATIVE_PRESS_COMBO_PUNCH_HIGH_PUNCH:
    case RETAIL_NATIVE_PRESS_CLOSE_KICK_HIGH_PUNCH:
      if (target == RETAIL_NATIVE_PRESS_CLOSE_KICK_HIGH_PUNCH && down && (peer->action == ACTION_RETAIL_4004 || peer->hits > 1)) {
        entry = retail_uppercut;
      } else if (fighter->armed && fighter->weapon != NULL) {
        weapon_followup_victim(fighter);
        entry = fighter_holds_back(fighter) ? retail_weapon_back_high_punch : retail_weapon_high_punch;
      } else {
        entry = target == RETAIL_NATIVE_PRESS_CLOSE_PUNCH_COMBO_HIGH_PUNCH ? retail_combo_high_punch
                : target == RETAIL_NATIVE_PRESS_AIR_CONTACT_HIGH_PUNCH     ? air_ground_hp
                                                                           : normal_hp;
      }
      break;
    case RETAIL_NATIVE_PRESS_FOLLOWUP_LOW_PUNCH:
      if (fighter->armed && fighter->weapon != NULL) {
        weapon_followup_victim(fighter);
        entry = fighter_holds_back(fighter) ? retail_weapon_back_low_punch : retail_weapon_low_punch;
      } else {
        entry = retail_normal_low_punch;
      }
      break;
    case RETAIL_NATIVE_PRESS_COMBO_PUNCH_HIGH_KICK:
    case RETAIL_NATIVE_PRESS_FOLLOWUP_HIGH_KICK:
    case RETAIL_NATIVE_PRESS_CLOSE_KICK_HIGH_KICK:
      if (target != RETAIL_NATIVE_PRESS_FOLLOWUP_HIGH_KICK && down && peer->hits > 1) {
        entry = follow_crouch_hk;
      } else if (target == RETAIL_NATIVE_PRESS_CLOSE_KICK_HIGH_KICK) {
        entry = fighter_holds_back(fighter) ? retail_roundhouse : retail_high_kick;
      } else {
        entry = normal_hk_entry(fighter);
      }
      break;
    case RETAIL_NATIVE_PRESS_FOLLOWUP_LOW_KICK:
      entry = action_entry(fighter_holds_back(fighter) ? 6 : 4);
      break;
    default:
      fatal("Unimplemented retail button controller %08x", target);
  }
  fighter->body.pos[AXIS_Y] = fighter->body.ground;
  fighter_interrupt(fighter, entry);
}

static void open_followup(Fighter *fighter, unsigned table) {
  fighter->buttons = &retail_close_buttons[table];
  fighter->flags = (fighter->flags | FF_FOLLOW_UP) & ~(unsigned)FF_BUSY;
}

static void close_followup(Fighter *fighter) {
  fighter->buttons = NULL;
  fighter->flags &= ~(unsigned)FF_FOLLOW_UP;
}

static bool contact(Fighter *fighter, uint32_t table, unsigned selector, ContactResult *result) {
  uint32_t packed = mk4_image_u32(battle_data(), table + selector * 4);
  AttackRecord attack = retail_vm_attack_record(battle_data(), packed * 4);
  uint64_t continuation = fighter->continuation;
  *result = battle_contact(fighter, &attack);
  return fighter->continuation == continuation && fighter->pending == NULL;
}

static void hp_reverse(Fighter *fighter) {
  close_followup(fighter);
  require(fighter_move(fighter)->last >= 8, "Native close punch needs sample8");
  fighter->sample_override = 8;
  native_play_rows(fighter, 0, -1, fighter_ready);
}

static void hp_follow_wait(Fighter *fighter) {
  if (!fighter->human) {
    retail_high_kick(fighter);
  } else {
    native_hold(fighter, 6, hp_reverse);
  }
}

static void hp_contact_finished(Fighter *fighter) {
  native_hold(fighter, 2, hp_follow_wait);
}

static void hp_miss_finished(Fighter *fighter) {
  native_hold(fighter, 2, hp_reverse);
}

void retail_close_high_punch_tail(Fighter *fighter) {
  const Fighter *peer = fighter_opponent(fighter);
  ContactResult result = CONTACT_MISS;
  if (peer->action != ACTION_RETAIL_2001 && peer->action != ACTION_RETAIL_0106 && peer->action != ACTION_RETAIL_0107 &&
      !contact(fighter, MK4_RETAIL_DATA_CONTACT_QUERY_A_TABLE, 16, &result)) {
    return;
  }
  if (result == CONTACT_MISS) {
    battle_sound(SOUND_B, 111);
    native_finish_clip(fighter, hp_miss_finished);
  } else {
    open_followup(fighter, 0);
    native_finish_clip(fighter, hp_contact_finished);
  }
}

static void combo_finish(Fighter *fighter) {
  native_finish_clip(fighter, fighter_ready);
}

void retail_combo_high_punch_tail(Fighter *fighter) {
  ContactResult result;
  if (!contact(fighter, MK4_RETAIL_DATA_CONTACT_QUERY_A_TABLE, 16, &result)) {
    return;
  }
  if (result == CONTACT_MISS) {
    combo_finish(fighter);
  } else {
    open_followup(fighter, 1);
    native_hold(fighter, 3, combo_finish);
  }
}

static void knee_blend(Fighter *fighter) {
  close_followup(fighter);
  const Mk4Image *image = battle_data();
  int clip = fighter->armed ? 286 : retail_stance_clip(image, (unsigned)fighter->roster, false);
  int count = (int)mk4_image_u32(image, RETAIL_ANIMATION_TABLE + (uint32_t)clip * RETAIL_ANIMATION_DESCRIPTOR_SIZE + RETAIL_ANIMATION_FRAME_COUNT_OFFSET);
  mk4_mmove_blend_to(fighter, clip, count, 0, 0x1999, 0, fighter_ready);
}

static void knee_reverse(Fighter *fighter) {
  if (!fighter->human) {
    retail_high_kick(fighter);
    return;
  }
  close_followup(fighter);
  fighter->sample_override = fighter_move(fighter)->last;
  native_play_rows(fighter, 0, -1, knee_blend);
}

static void knee_contact_finished(Fighter *fighter) {
  native_hold(fighter, 2, knee_reverse);
}

static void knee_miss_finished(Fighter *fighter) {
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_KNEE_MISS_FINISHED, fighter_ready);
}

void retail_close_high_kick_tail(Fighter *fighter) {
  ContactResult result;
  if (!contact(fighter, MK4_RETAIL_DATA_CONTACT_QUERY_B_TABLE, 38, &result)) {
    return;
  }
  if (result == CONTACT_MISS) {
    native_finish_clip(fighter, knee_miss_finished);
  } else {
    fighter->run_energy = fighter->run_energy > 0x6666 ? fighter->run_energy - 0x6666 : 0;
    fighter->run_delay = 40;
    open_followup(fighter, 2);
    native_finish_clip(fighter, knee_contact_finished);
  }
}

static void knee_script(Fighter *fighter) {
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_KNEE_SCRIPT, fighter_ready);
}

void retail_close_high_kick(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter->body.pos[AXIS_Y] = fighter->body.ground;
  retail_native_begin(fighter, S_KICK, ACTION_RETAIL_0103, false);
  if ((fighter_opponent(fighter)->action & 0xf000) == 0x4000) {
    retail_vm_bind_animation(fighter, mk4_image_u32(battle_data(), MK4_RETAIL_ANIMATION_DESCRIPTOR_CLOSE_HIGH_KICK) * 4, 0);
    native_play_rows(fighter, 4, 1, knee_script);
  } else {
    knee_script(fighter);
  }
}

void retail_press_table(Fighter *fighter, Button button) {
  uint32_t target = retail_button_target(battle_data(), fighter->buttons->retail_address, button);
  retail_press_native_button(fighter, target);
}

/* Local45f2b0 consumes two four-word player records: required switch words,
 * then their masks. P2 uses the same switches shifted8 and4 bits. The engine
 * Button order preserves the two retail words (see input.h). */
bool retail_native_button_chord(const Fighter *fighter, uint32_t address) {
  enum {
    PLAYER_RECORD_BYTES = 16,
    ROW_BYTES = 32,
    P2_PRIMARY_SHIFT = 8,
    P2_SECONDARY_SHIFT = 4
  };

  require(address >= RETAIL_SCRIPT_BEGIN && address <= RETAIL_SCRIPT_END - ROW_BYTES && address % 4 == 0, "Invalid native button-chord row");
  bool second_player = fighter == &g_battle.fighters[1];
  uint32_t row = address + (second_player ? PLAYER_RECORD_BYTES : 0);
  uint32_t primary = fighter->held & (BUTTON_BIT(BUTTON_LOW_PUNCH) - 1);
  uint32_t secondary = fighter->held >> BUTTON_LOW_PUNCH;
  if (second_player) {
    primary <<= P2_PRIMARY_SHIFT;
    secondary <<= P2_SECONDARY_SHIFT;
  }
  const Mk4Image *image = battle_data();
  return (primary & mk4_image_u32(image, row + 8)) == mk4_image_u32(image, row) &&
         (secondary & mk4_image_u32(image, row + 12)) == mk4_image_u32(image, row + 4);
}
