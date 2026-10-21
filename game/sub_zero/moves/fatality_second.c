#include "../character.h"

void sub_zero_second_fatality_script(Fighter *fighter) {
  sub_zero_entry_retail_script_004d2bf0(sub_zero_opponent(fighter));
  sub_zero_entry_retail_script_004d2c10(fighter);
}

void sub_zero_second_fatality(Fighter *fighter) {
  sub_zero_start_fatality(fighter, FINISH_SUB_ZERO_2);
}
