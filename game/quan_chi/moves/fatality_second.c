#include "../character.h"

/* 45e530: the second-fatality range gate, 1.8..3 units. */
bool quan_chi_can_second_fatality(const Fighter *fighter) {
  float distance = fighter_distance(fighter);
  return battle_can_finish(fighter) && distance >= q16_float(0x1cccc) && distance <= 3.f;
}

/* LP [U,U,D,D] record 4e4824 enters 467cd0, whose table 4dfe90[8] selects
 * 43be30: Quan Chi performs the victim roster's own second fatality
 * (4dfe90[victim], Kitana as Sonya and Noob as Reiko). Against another Quan
 * Chi, 43cbb0 runs his first fatality instead. The portable dispatch uses
 * each roster's registered second-fatality entry, with Quan Chi as owner. */
void quan_chi_second_fatality(Fighter *fighter) {
  int roster = fighter_opponent(fighter)->roster;
  roster = roster == 16 ? 2 : roster == 17 ? 7 : roster;
  if (roster == 8) {
    quan_chi_first_fatality(fighter);
    return;
  }
  const CpuMoves *moves = roster_def(roster)->cpu;
  FighterFn entry = moves != NULL ? moves->enter[CPU_FINISH_2] : NULL;
  if (entry == NULL) {
    fighter_ready(fighter);
    return;
  }
  entry(fighter);
}
