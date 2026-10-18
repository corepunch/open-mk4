#include "../character.h"

static bool cage_fatality_scene_event(BattleEvent *event, Fighter *owner, Fighter *opponent);
static bool cage_fatality_2_event(BattleEvent *event, Fighter *owner, Fighter *opponent);
static void cage_script_fatality_1_continue(Fighter *fighter);
static void cage_fatality_1(Fighter *fighter);
static void cage_fatality_2(Fighter *fighter);

static bool cage_fatality_scene_event(BattleEvent *event, Fighter *owner, Fighter *opponent) {
  (void)opponent;
  if (g_battle.phase != PHASE_FATALITY) {
    return false;
  }
  if (event->phase == 0) {
    event->phase = 1;
    return true;
  }
  if (owner->sample_override >= 29) {
    battle_end_finisher();
    return false;
  }
  return true;
}

void cage_script_fatality_1_signal(Fighter *fighter) {
  (void)battle_event_start(fighter, cage_fatality_scene_event, false, true);
}

static bool cage_fatality_2_event(BattleEvent *event, Fighter *owner, Fighter *opponent) {
  (void)opponent;
  if (g_battle.phase != PHASE_FATALITY) {
    return false;
  }
  if (event->phase == 0) {
    event->phase = 1;
    fighter_start_fiber(owner, cage_entry_retail_script_004e12b0, NULL);
    return true;
  }
  if (owner->sample_override >= 29) {
    battle_end_finisher();
    return false;
  }
  return true;
}

void cage_script_fatality_2_continue(Fighter *fighter) {
  (void)battle_event_start(fighter, cage_fatality_2_event, false, true);
}

static void cage_script_fatality_1_continue(Fighter *fighter) {
  cage_entry_retail_script_004e12b0(fighter);
}

static void cage_fatality_1(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, CAGE_EVENT_FATALITY_1)) {
    return;
  }
  fighter_start_fiber(fighter, cage_entry_retail_script_004e12a0, cage_script_fatality_1_continue);
}

static void cage_fatality_2(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, CAGE_EVENT_FATALITY_2)) {
    return;
  }
  fighter_start_fiber(fighter, cage_entry_retail_script_004e5870, NULL);
}
