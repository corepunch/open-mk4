#include "native_services.h"

void retail_weapon_native_496d10(Fighter *fighter);
static void weapon_first_victim(Fighter *fighter);
static void weapon_second_victim(Fighter *fighter);
static const NativeAxeProfile axe_profile = {weapon_first_victim, weapon_second_victim};

void retail_weapon_native_496d10(Fighter *fighter) {
  weapon_axe_strike(fighter, 3, &axe_profile);
}

static void weapon_first_victim(Fighter *fighter) {
  weapon_axe_reaction(fighter, false, (RetailScriptContinuation){0x4ea040, mk4_script_thrown_ready, 7});
}

static void weapon_second_victim(Fighter *fighter) {
  weapon_axe_reaction(fighter, true, (RetailScriptContinuation){0x4ea0b8, mk4_script_thrown_ready, 7});
}
