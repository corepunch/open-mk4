#include "../character.h"

/* BLOCK record 4e45f0 and HK record 4e45b0 both enter 4698a0 -> VM 4e5bb0:
 * clip 12 (58 samples) advances 24 samples, cues C[75]/C[74], then native
 * 4698b0 toggles Reptile's visibility halfway through the clip. */
static const MoveDef invisibility = {.clip = 12, .last = 57, .flags = SF_LOCK | SF_RECORD};

bool reptile_can_invisibility(const Fighter *fighter) {
  /* 45f260 -> 45f2b0 with record 4e4228: P1 word 0x4d00a4 & 0x71 == 0x60
   * and word 0x4d00a8 & 7 == 0 (P2 uses the same bits shifted by eight).
   * BLOCK and HK are held; UP, HP, LP, LK and RUN are not; directions are
   * outside the mask. The 53c880 override is a non-gameplay switch. */
  unsigned chord = BUTTON_BIT(BUTTON_BLOCK) | BUTTON_BIT(BUTTON_HIGH_KICK);
  unsigned mask =
      chord | BUTTON_BIT(BUTTON_UP) | BUTTON_BIT(BUTTON_HIGH_PUNCH) | BUTTON_BIT(BUTTON_LOW_PUNCH) | BUTTON_BIT(BUTTON_LOW_KICK) | BUTTON_BIT(BUTTON_RUN);
  return (fighter->held & mask) == chord;
}

static void toggle(Fighter *fighter) {
  /* 4698b0 state 1: fighter render node +0x34 ^= 4, the culled bit. */
  fighter->invisible = !fighter->invisible;
}

/* 4698b0: 428d70 plays to sample 40 and submits it for one more visit,
 * the toggle runs, 428cf0 resumes at 41 and plays out the clip, then
 * 470420 returns to the controller. The 404cc0/404df0 green light pulse
 * and its 40fa20 effect task are presentation without an engine light. */
void reptile_script_invisibility(Fighter *fighter) {
  require(fighter_move(fighter)->clip == 12 && fighter_sample(fighter) == 24, "Invisibility needs its generated clip-12 startup");
  native_begin(fighter, 1, 35, mk4_mmove_thrown_finish);
  for (int frame = 1; frame <= 16; ++frame) {
    native_frame(fighter, frame, 24 + frame, NULL);
  }
  native_frame(fighter, 17, 40, NULL);
  native_frame(fighter, 18, 41, toggle);
  for (int frame = 19; frame <= 34; ++frame) {
    native_frame(fighter, frame, 23 + frame, NULL);
  }
  native_frame(fighter, 35, 57, NULL);
  native_play(fighter, false);
}

void reptile_invisibility(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &invisibility);
  reptile_entry_retail_script_004e5bb0(fighter);
}
