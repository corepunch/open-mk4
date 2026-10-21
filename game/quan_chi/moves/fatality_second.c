#include "../character.h"
#include "retail_data.h"
#include "retail_vm.h"

/* 45e530: the second-fatality range gate, 1.8..3 units. */
/* LP [U,U,D,D] record 4e4824 enters 467cd0, whose table 4dfe90[8] selects
 * 43be30: Quan Chi performs the victim roster's own second fatality
 * (4dfe90[victim], Kitana as Sonya and Noob as Reiko). Against another Quan
 * Chi, 43cbb0 runs his first fatality instead. The portable dispatch uses
 * the victim's original native table target, with the current actor as owner. */
void quan_chi_second_fatality(Fighter *fighter) {
  unsigned roster = retail_character_slot((unsigned)fighter_opponent(fighter)->roster);
  if (roster == 8) {
    retail_finish_first(fighter);
    return;
  }
  uint32_t target = retail_finish_target(retail_vm_image(), roster, true);
  if (target == 0) {
    fighter_ready(fighter);
    return;
  }
  retail_native_entry((unsigned)fighter->roster, target)(fighter);
}
