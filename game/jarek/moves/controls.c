#include "../character.h"

static void jarek_command_467cd0(Fighter *fighter);
static void jarek_command_467ce0(Fighter *fighter);
static void jarek_command_497540(Fighter *fighter);

static void jarek_command_467cd0(Fighter *fighter) {
  jarek_fan_fatality(fighter);
}

static void jarek_command_467ce0(Fighter *fighter) {
  jarek_spike_fatality(fighter);
}

static void jarek_command_497540(Fighter *fighter) {
  fighter_start_fiber(fighter, jarek_entry_retail_script_004ed118, NULL);
}
