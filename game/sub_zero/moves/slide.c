#include "../character.h"

static void slide_cleanup(Fighter *fighter);
static void slide_contact(Fighter *fighter);
static const AttackRecord slide_record = {0xf0a3, 0x947a, 0x111eb, 0x8f5c, 122, 122, 165, 0, 0x147a};
static const MoveDef s_slide = {.clip = 95, .last = 10, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0204};
// clang-format off: Keep table entries on one row, including long rows.
static const int slide_contact_frames_samples[] = {
  0,
  1,
  2,
  3,
  4,
  5,
  6,
  7,
  8,
  9,
  9,
  9,
  9,
  9,
  9,
  9,
  9,
  9,
  9,
  9,
  9,
  9,
  9,
  9,
  9,
};
// clang-format on
// clang-format off: Keep table entries on one row, including long rows.
static const mframe_t slide_contact_frames[25] = {
  [8] = {.thinkfunc = slide_contact},
  [9] = {.thinkfunc = slide_contact},
  [10] = {.thinkfunc = slide_contact},
  [11] = {.thinkfunc = slide_contact},
  [12] = {.thinkfunc = slide_contact},
  [13] = {.thinkfunc = slide_contact},
  [14] = {.thinkfunc = slide_contact},
  [15] = {.thinkfunc = slide_contact},
  [16] = {.thinkfunc = slide_contact},
  [17] = {.thinkfunc = slide_contact},
  [18] = {.thinkfunc = slide_contact},
  [19] = {.thinkfunc = slide_contact},
  [20] = {.thinkfunc = slide_contact},
  [21] = {.thinkfunc = slide_contact},
  [22] = {.thinkfunc = slide_contact},
  [23] = {.thinkfunc = slide_contact},
  [24] = {.thinkfunc = slide_contact},
};
// clang-format on
static const mmove_t slide_contact_phase = {
    .firstframe = 0, .lastframe = 24, .frame = slide_contact_frames, .endfunc = sub_zero_script_slide_return, .samples = slide_contact_frames_samples};
static const mframe_t breath_slide_phase_frames[9] = {{0}};
static const mmove_t breath_slide_phase = {.firstframe = 0, .lastframe = 8, .frame = breath_slide_phase_frames, .endfunc = sub_zero_breath_travel};
// clang-format off: Keep table entries on one row, including long rows.
static const int slide_return_frames_samples[] = {
  10,
  9,
  8,
  7,
  6,
  5,
  4,
  3,
  2,
  1,
  0,
};
// clang-format on
static const mframe_t slide_return_frames[11] = {{0}};
static const mmove_t slide_return_phase = {
    .firstframe = 0, .lastframe = 10, .frame = slide_return_frames, .endfunc = sub_zero_ready, .samples = slide_return_frames_samples};

static void slide_cleanup(Fighter *fighter) {
  fighter->flags &= ~(unsigned)FF_NO_GUARD;
  sub_zero_stop_body(fighter);
}

static void slide_contact(Fighter *fighter) {
  fighter->sample_override = 9;
  ContactResult contact = battle_contact(fighter, &slide_record);
  if (contact == CONTACT_MISS) {
    return;
  }
  fighter->script_phase = 1;
  if (contact == CONTACT_BLOCK) {
    sub_zero_entry_retail_script_004e8530(fighter);
  } else {
    fighter->slide_ready = g_battle.ticks + 64;
    sub_zero_entry_retail_script_004e84f8(fighter);
  }
}

void sub_zero_slide_script(Fighter *fighter) {
  fighter_mmove_phase(fighter, &slide_contact_phase);
}

bool sub_zero_can_slide(const Fighter *fighter) {
  unsigned chord = BUTTON_BIT(BUTTON_BLOCK) | BUTTON_BIT(BUTTON_LOW_PUNCH) | BUTTON_BIT(BUTTON_LOW_KICK);
  unsigned mask = chord | BUTTON_BIT(BUTTON_UP) | BUTTON_BIT(BUTTON_HIGH_PUNCH) | BUTTON_BIT(BUTTON_HIGH_KICK) | BUTTON_BIT(BUTTON_RUN);
  return g_battle.phase == PHASE_FIGHT && g_battle.ticks >= fighter->slide_ready && (fighter->held & mask) == chord;
}

void sub_zero_slide(Fighter *fighter) {
  sub_zero_stop_body(fighter);
  fighter_start_move(fighter, S_KICK, &s_slide);
  fighter->sample_override = 0;
  fighter->mmove_cleanup = slide_cleanup;
  sub_zero_entry_retail_script_004e84d8(fighter);
}

void sub_zero_script_breath_slide(Fighter *fighter) {
  fighter->script_phase = 1;
  fighter_mmove_phase(fighter, &breath_slide_phase);
}

void sub_zero_script_slide_return(Fighter *fighter) {
  sub_zero_stop_body(fighter);
  fighter->script_phase = 2;
  fighter_mmove_phase(fighter, &slide_return_phase);
}
