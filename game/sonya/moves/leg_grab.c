#include "../character.h"

static void leg_stop(Fighter *fighter);
static void leg_grab_done(Fighter *fighter);
static void leg_grab_contact(Fighter *fighter);
static void leg_grab_script(Fighter *fighter);
static const MoveDef s_leg_grab = {.clip = 102, .last = 64, .flags = SF_LOCK | SF_RECORD, .action = 0x203};
static const MoveDef s_leg_victim = {.clip = 201, .last = 34, .flags = SF_LOCK, .action = ACTION_RETAIL_4003};
static const AttackRecord leg_record = {92405, -29491, 85852, 76677, 88, 88, 8, 1, 11796};

void sonya_script_leg_victim_end(Fighter *victim) {
  mk4_mmove_bind(victim, 185, 16, 0);
  sonya_script_throw_getup(victim);
}

static void leg_grab_done(Fighter *fighter) {
  fighter_set_speed(fighter, 0);
  fighter_finish_mmove(fighter, sonya_ready);
}

void sonya_script_leg_grab_tail(Fighter *fighter) {
  native_finish_clip(fighter, leg_grab_done);
}

static void leg_stop(Fighter *fighter) {
  fighter_set_speed(fighter, 0);
}

static void leg_grab_contact(Fighter *fighter) {
  if (battle_contact(fighter, &leg_record) == CONTACT_MISS || fighter->contact == CONTACT_BLOCK) {
    fighter->action = 0x1012;
    sonya_entry_retail_script_004e85a8(fighter);
  } else {
    mk4_mmove_pair_victim(fighter, &s_leg_victim);
    sonya_entry_retail_script_004e8578(fighter);
  }
}

static void leg_grab_script(Fighter *fighter) {
  fighter_set_speed(fighter, 0x3333);
  native_begin(fighter, 1, 11, leg_grab_contact);
  for (int frame = 1; frame <= 11; ++frame) {
    native_frame(fighter, frame, frame, frame == 7 ? leg_stop : NULL);
  }
  native_play(fighter, false);
}

void sonya_leg_grab(Fighter *fighter) {
  if (g_battle.phase != PHASE_FIGHT || fighter_airborne(fighter)) {
    return;
  }
  fighter->leg_grab_ready = g_battle.ticks + 64;
  fighter_start_move(fighter, S_KICK, &s_leg_grab);
  battle_sound(SOUND_A, 9);
  battle_sound(SOUND_INDEXED, 15);
  fighter->mmove_cleanup = sonya_special_cleanup;
  leg_grab_script(fighter);
}

bool sonya_can_leg_grab(const Fighter *fighter) {
  unsigned chord = BUTTON_BIT(BUTTON_DOWN) | BUTTON_BIT(BUTTON_BLOCK) | BUTTON_BIT(BUTTON_LOW_PUNCH);
  unsigned mask =
      chord | BUTTON_BIT(BUTTON_UP) | BUTTON_BIT(BUTTON_HIGH_PUNCH) | BUTTON_BIT(BUTTON_HIGH_KICK) | BUTTON_BIT(BUTTON_LOW_KICK) | BUTTON_BIT(BUTTON_RUN);
  return g_battle.phase == PHASE_FIGHT && g_battle.ticks >= fighter->leg_grab_ready && (fighter->held & mask) == chord;
}
