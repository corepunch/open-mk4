#include "../character.h"

static void jarek_command_467cd0(Fighter *fighter);
static void jarek_command_467ce0(Fighter *fighter);
static void jarek_command_47b310(Fighter *fighter);
static void jarek_command_47b5d0(Fighter *fighter);
static void jarek_command_497540(Fighter *fighter);

static void jarek_command_467cd0(Fighter *fighter) {
  jarek_fan_fatality(fighter);
}

static void jarek_command_467ce0(Fighter *fighter) {
  jarek_spike_fatality(fighter);
}

static void jarek_command_47b310(Fighter *fighter) {
  jarek_low_kick(fighter);
}

static void jarek_command_47b5d0(Fighter *fighter) {
  jarek_start(fighter, S_KICK, &jarek_s_high_kick, jarek_entry_retail_script_004e81a8);
}

static void jarek_command_497540(Fighter *fighter) {
  fighter_start_fiber(fighter, jarek_entry_retail_script_004ed118, NULL);
}
