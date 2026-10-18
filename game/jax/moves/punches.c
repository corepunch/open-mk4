#include "../character.h"

static void jax_high_punch(Fighter *fighter);

void jax_script_jab_contact(Fighter *fighter) {
  BattleEvent *event = battle_event_start(fighter, jax_contact_event, false, true);
  event->phase = JAX_EVENT_JAB_CONTACT;
}

void jax_script_jab_followup(Fighter *fighter) {
  (void)battle_event_start(fighter, jax_contact_event, false, true);
}

void jax_jab(Fighter *fighter) {
  jax_start(fighter, S_PUNCH, &jax_s_jab, jax_entry_retail_script_004e62f0);
}

static void jax_high_punch(Fighter *fighter) {
  jax_jab(fighter);
}

void jax_press_hp(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed ? fighter->weapon->hp : jax_weapon_action);
}
