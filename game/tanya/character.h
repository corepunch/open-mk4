#include "retail_native.h"
#ifndef MK4_TANYA_PRIVATE_H
#define MK4_TANYA_PRIVATE_H

#include "game.h"
#include "native.h"
#include "mk4_characters.h"
#include "script.h"
#include "script.h"

/* Internal interfaces; helpers and tables stay local to their owning move. */
enum {
  TANYA_EVENT_FATALITY_1 = 35,
  TANYA_EVENT_FATALITY_2
};

void tanya_script_current_reaction(Fighter *fighter, int reaction);
void tanya_script_opponent_reaction(Fighter *fighter, int reaction);

extern const ButtonTable tanya_diagonal_air;
bool tanya_can_spin_kick(const Fighter *fighter);
void tanya_spin_kick(Fighter *fighter);
void tanya_blocked_recoil_land(Fighter *fighter);
void tanya_drill_kick(Fighter *fighter);
void tanya_drill_kick_contact(Fighter *fighter);
void tanya_boomerang_release(Fighter *fighter);
void tanya_fatality_2(Fighter *fighter);
void tanya_second_height(Fighter *fighter);
void tanya_second_owner_cue(Fighter *fighter, int task_type);
void tanya_second_victim_cue(Fighter *fighter, int task_type);
void tanya_second_event(Fighter *fighter, int selector);
void tanya_second_finish(Fighter *fighter);
void tanya_second_victim_pause(Fighter *fighter);
void tanya_second_reaction(Fighter *fighter);
void tanya_script_victory(Fighter *fighter);
bool tanya_can_first_fatality(const Fighter *fighter);
void tanya_fatality_1(Fighter *fighter);
void tanya_first_owner_53(Fighter *fighter);
void tanya_first_owner_drift(Fighter *fighter);
void tanya_first_owner_replays(Fighter *fighter);
void tanya_first_owner_loop(Fighter *fighter);
void tanya_first_victim_swell(Fighter *fighter);
void tanya_first_victim_repeat(Fighter *fighter);
void tanya_first_victim_burst(Fighter *fighter);
#endif
