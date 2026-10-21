#include "native_services.h"

void raiden_script_weapon_victim_getup(Fighter *victim) {
  native_finish_clip(victim, retail_getup_flip);
}

void raiden_script_staff_victim_tail(Fighter *victim) {
  victim->body.physics = fighter_friction;
  native_hold(victim, 20, raiden_script_weapon_victim_getup);
}
