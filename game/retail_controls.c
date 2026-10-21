#include "retail_native.h"
#include "retail_data.h"
#include "retail_vm.h"
#include "native.h"
#include "script.h"
#include "mk4_characters.h"

/* Views contain addresses only; button targets remain in the loaded image. */
const ButtonTable retail_close_buttons[] = {
    {.retail_address = 0x4e7078},
    {.retail_address = 0x4e7090},
    {.retail_address = 0x4e70a8},
};

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
    retail_close_high_punch(fighter);
  } else {
    retail_high_punch(fighter);
  }
}

static void normal_hk(Fighter *fighter) {
  const Fighter *peer = fighter_opponent(fighter);
  if (fighter_holds_back(fighter)) {
    retail_roundhouse(fighter);
  } else if (fighter_distance(fighter) <= q16_float(0xcccc) && !fighter_airborne(peer) && peer->action != ACTION_RETAIL_4005 &&
             peer->action != ACTION_RETAIL_4001) {
    retail_close_high_kick(fighter);
  } else {
    retail_high_kick(fighter);
  }
}

static void normal_lp(Fighter *fighter) {
  (mk4_can_ground_throw(fighter, false) ? retail_lp_throw : retail_jab)(fighter);
}

static void normal_lk(Fighter *fighter) {
  if (fighter_holds_back(fighter)) {
    retail_sweep(fighter);
  } else {
    (mk4_can_ground_throw(fighter, true) ? retail_lk_throw : retail_low_kick)(fighter);
  }
}

/* Native46b5d0 supplies only the literal script operand, not authored logic. */
static void follow_crouch_hk(Fighter *fighter) {
  int action = fighter->action;
  retail_native_begin(fighter, S_KICK, action, false);
  retail_vm_enter(fighter, 0x4e5fd0, fighter_ready);
}

/* Controllers46c210..46c740. Native weapon frontends remain transitional services. */
void retail_press_table(Fighter *fighter, Button button) {
  uint32_t target = retail_button_target(retail_vm_image(), fighter->buttons->retail_address, button);
  if (target == 0) {
    return;
  }
  const Fighter *peer = fighter_opponent(fighter);
  bool down = fighter_height(fighter) <= .5f && (fighter->held & BUTTON_BIT(BUTTON_DOWN));
  FighterFn entry = NULL;
  switch (target) {
    case 0x46c210:
    case 0x46c450:
    case 0x46c480:
      if (target == 0x46c480 && down && (peer->action == ACTION_RETAIL_4004 || peer->hits > 1)) {
        entry = retail_uppercut;
      } else if (fighter->armed && fighter->weapon != NULL) {
        weapon_followup_victim(fighter);
        entry = fighter_holds_back(fighter) ? fighter->weapon->back_hp : fighter->weapon->hp;
      } else {
        entry = target == 0x46c210 ? retail_combo_high_punch : normal_hp;
      }
      break;
    case 0x46c6f0:
      if (fighter->armed && fighter->weapon != NULL) {
        weapon_followup_victim(fighter);
        entry = fighter_holds_back(fighter) ? fighter->weapon->back_lp : fighter->weapon->lp;
      } else {
        entry = normal_lp;
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
        entry = normal_hk;
      }
      break;
    case 0x46c740:
      entry = normal_lk;
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
  int count = (int)mk4_image_u32(image, 0x4fad7c + (uint32_t)clip * 12 + 4);
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
