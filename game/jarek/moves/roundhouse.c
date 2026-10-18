#include "../character.h"

static const AttackRecord jarek_roundhouse_record;
static void jarek_roundhouse(Fighter *fighter);
static const MoveDef s_roundhouse = {.clip = 313, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK, .action = ACTION_RETAIL_010D};
static const AttackRecord jarek_roundhouse_record = {69468, -43909, 61603, 27525, 20, 20, 5, 0, 11796};

static void jarek_roundhouse(Fighter *fighter) {
  jarek_start(fighter, S_KICK, &s_roundhouse, jarek_entry_retail_script_004e6800);
}

void jarek_script_roundhouse_start(Fighter *fighter) {
  if (fighter_fast_start(fighter)) {
    jarek_entry_retail_script_004e6860(fighter);
  } else {
    jarek_entry_retail_script_004e6878(fighter);
  }
}

void jarek_script_roundhouse_finish(Fighter *fighter) {
  if (!fighter->connected) {
    battle_contact(fighter, &jarek_roundhouse_record);
  }
  fighter_fiber_finish(jarek_ready);
}
