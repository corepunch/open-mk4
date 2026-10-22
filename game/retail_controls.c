#include "retail_layout.h"
#include "retail_native.h"
#include "retail_data.h"
#include "retail_vm.h"
#include "native.h"
#include "script.h"
#include "native_services.h"

/* Views contain addresses only; button targets remain in the loaded image. */
const ButtonTable retail_close_buttons[] = {
    {.retail_address = 0x4e7078},
    {.retail_address = 0x4e7090},
    {.retail_address = 0x4e70a8},
    {.retail_address = 0x4e70c0},
};

const ButtonTable retail_ground_buttons[] = {
    {.retail_address = 0x4e5058},
    {.retail_address = 0x4e5070},
};

const ButtonTable retail_air_buttons[] = {
    {.retail_address = 0x4e6ec8},
    {.retail_address = 0x4e70d8},
    {.retail_address = 0x4e6ee0},
    {.retail_address = 0x4e88f0},
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
    case 0x4712c0:
    case 0x46c1f0:
      action = 0;
      break;
    case 0x4712d0:
    case 0x46c200:
      action = 1;
      break;
    case 0x46c1d0:
      action = 2;
      break;
    case 0x46c1e0:
      action = 3;
      break;
    case 0x4712b0:
      action = 4;
      break;
    default:
      return false;
  }
  uint32_t entry = retail_action_target(retail_vm_image(), 8, action);
  fighter_interrupt(fighter, retail_native_entry(entry));
  return true;
}

static FighterFn action_entry(unsigned index) {
  uint32_t target = retail_action_target(retail_vm_image(), 1, index);
  return retail_native_entry(target);
}

static void enter_action(Fighter *fighter, unsigned index) {
  action_entry(index)(fighter);
}

static FighterFn crouch_entry(unsigned index) {
  uint32_t target = mk4_image_u32(retail_vm_image(), 0x4e7020 + index * 4);
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
  require(target == 0 || (target >= 0x400000 && target < 0x4d0000), "Invalid retail button target");
  return target;
}

void retail_combo_high_punch(Fighter *fighter) {
  unsigned slot = retail_native_character_slot((unsigned)fighter->roster);
  uint32_t entry = mk4_image_u32(retail_vm_image(), 0x4e6c50 + slot * 4);
  uint32_t script;
  switch (entry) {
    case 0x46c290:
      script = 0x4e69b0;
      break;
    case 0x46c2a0:
      script = 0x4e69c8;
      break;
    case 0x46c2b0:
      script = 0x4e6a28;
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
  retail_vm_enter(fighter, 0x4e5fd0, fighter_ready);
}

/* Controllers46c210..46c740. Native weapon frontends remain transitional services. */
void retail_press_native_button(Fighter *fighter, uint32_t target) {
  if (target == 0 || press_air_controller(fighter, target)) {
    return;
  }
  if (target == 0x47c2a0) {
    fighter_interrupt(fighter, retail_native_entry(target));
    return;
  }
  if (target == 0x47c850 || target == 0x47ccf0 || target == 0x47cc10 || target == 0x47cc80) {
    fighter_interrupt(fighter, retail_native_entry(target));
    return;
  }
  const Fighter *peer = fighter_opponent(fighter);
  bool down = fighter_height(fighter) <= .5f && (fighter->held & BUTTON_BIT(BUTTON_DOWN));
  FighterFn entry = NULL;
  switch (target) {
    case 0x460f40:
      entry = fighter->armed && fighter->weapon != NULL ? (fighter_holds_back(fighter) ? retail_weapon_back_high_punch : retail_weapon_high_punch) : normal_hp;
      break;
    case 0x460fb0:
      entry =
          fighter->armed && fighter->weapon != NULL ? (fighter_holds_back(fighter) ? retail_weapon_back_low_punch : retail_weapon_low_punch) : action_entry(8);
      break;
    case 0x460fc0:
      entry = action_entry(fighter_holds_back(fighter) ? 6 : 4);
      break;
    case 0x461030:
      entry = normal_hk_entry(fighter);
      break;
    case 0x4610c0:
      /* The engine command matcher owns finish command gates before fallback. */
      {
        uint32_t target = retail_action_target(retail_vm_image(), 1, 1);
        entry = target == 0x46ccd0 ? retail_uppercut : retail_native_entry(target);
      }
      break;
    case 0x4610d0:
      if (fighter->armed && fighter->weapon != NULL && fighter->weapon->type == 0x60) {
        entry = retail_weapon_native_480770;
      } else {
        entry = crouch_entry(8);
      }
      break;
    case 0x461190:
      entry = crouch_entry(7);
      break;
    case 0x461160:
      entry = crouch_entry(6);
      break;
    case 0x460200:
    case 0x461130:
      /* Held-block movement/state ownership is handled by the simulation pump. */
      return;
    case 0x46c210:
    case 0x46c3f0:
    case 0x46c450:
    case 0x46c480:
      if (target == 0x46c480 && down && (peer->action == ACTION_RETAIL_4004 || peer->hits > 1)) {
        entry = retail_uppercut;
      } else if (fighter->armed && fighter->weapon != NULL) {
        weapon_followup_victim(fighter);
        entry = fighter_holds_back(fighter) ? retail_weapon_back_high_punch : retail_weapon_high_punch;
      } else {
        entry = target == 0x46c210 ? retail_combo_high_punch : target == 0x46c3f0 ? air_ground_hp : normal_hp;
      }
      break;
    case 0x46c6f0:
      if (fighter->armed && fighter->weapon != NULL) {
        weapon_followup_victim(fighter);
        entry = fighter_holds_back(fighter) ? retail_weapon_back_low_punch : retail_weapon_low_punch;
      } else {
        entry = retail_normal_low_punch;
      }
      break;
    case 0x46c650:
    case 0x46c680:
    case 0x46c6a0:
      if (target != 0x46c680 && down && peer->hits > 1) {
        entry = follow_crouch_hk;
      } else if (target == 0x46c6a0) {
        entry = fighter_holds_back(fighter) ? retail_roundhouse : retail_high_kick;
      } else {
        entry = normal_hk_entry(fighter);
      }
      break;
    case 0x46c740:
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
  uint32_t packed = mk4_image_u32(retail_vm_image(), table + selector * 4);
  AttackRecord attack = retail_vm_attack_record(retail_vm_image(), packed * 4);
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
      !contact(fighter, 0x53d268, 16, &result)) {
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
  if (!contact(fighter, 0x53d268, 16, &result)) {
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
  const Mk4Image *image = retail_vm_image();
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
  retail_vm_enter(fighter, 0x4e61d0, fighter_ready);
}

void retail_close_high_kick_tail(Fighter *fighter) {
  ContactResult result;
  if (!contact(fighter, 0x53d2b8, 38, &result)) {
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
  retail_vm_enter(fighter, 0x4e6190, fighter_ready);
}

void retail_close_high_kick(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter->body.pos[AXIS_Y] = fighter->body.ground;
  retail_native_begin(fighter, S_KICK, ACTION_RETAIL_0103, false);
  if ((fighter_opponent(fighter)->action & 0xf000) == 0x4000) {
    retail_vm_bind_animation(fighter, mk4_image_u32(retail_vm_image(), 0x53ce60) * 4, 0);
    native_play_rows(fighter, 4, 1, knee_script);
  } else {
    knee_script(fighter);
  }
}

void retail_press_table(Fighter *fighter, Button button) {
  uint32_t target = retail_button_target(retail_vm_image(), fighter->buttons->retail_address, button);
  retail_press_native_button(fighter, target);
}
