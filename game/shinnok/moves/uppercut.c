#include "../character.h"

static void shinnok_uppercut_frame(Fighter *fighter);
static const AttackRecord shinnok_uppercut_record = {58327, -64880, 38010, 66840, 34, 34, 6, 0, 10485};
static const AttackRecord shinnok_uppercut_followup_record = {73990, -70057, 64159, 96927, 34, 34, 6, 0, 2621};

void shinnok_uppercut(Fighter *fighter) {
  fighter_start_move(fighter, S_UPPERCUT, &shinnok_s_uppercut);
  shinnok_entry_retail_script_004e62a0(fighter);
}

static void shinnok_uppercut_frame(Fighter *fighter) {
  const AttackRecord *record = (fighter->flags & FF_FOLLOW_UP) ? &shinnok_uppercut_followup_record : &shinnok_uppercut_record;
  if (!fighter->connected && fighter_opponent(fighter)->action != ACTION_RETAIL_2001 && battle_contact(fighter, record) != CONTACT_MISS) {
    fighter->action = ACTION_RETAIL_1006;
  }
}

void shinnok_script_uppercut_finish(Fighter *fighter) {
  native_begin(fighter, 1, 4, NULL);
  for (int frame = 1; frame <= 4; ++frame) {
    native_frame(fighter, frame, frame == 1 ? fighter_sample(fighter) : frame + 4, frame >= 2 ? shinnok_uppercut_frame : NULL);
  }
  native_wait(fighter);
  fighter_fiber_finish(shinnok_ready);
}

void shinnok_press_uppercut(Fighter *fighter) {
  fighter_interrupt(fighter, shinnok_uppercut);
}
