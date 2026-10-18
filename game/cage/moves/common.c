#include "../character.h"

static void cage_start_event_recovery(BattleEvent *event, int frames, int kind);
static const MoveDef s_powered_punch_finish = {105, 0, 29, 0, 0, 19, 12, 0, 0.f, 0.f, SF_LOCK, NULL, 0};
static const AttackRecord s_cage_powered_punch_contact = {0x107ae, -0x9c28, 0xdc28, 0xa8f5, 38, 10, 143, 0, 0x170a};
static const AttackRecord s_cage_powered_finish_contact = {0x12b85, -0x4f5c, 0x10a3d, 0xf5c2, 39, 39, 4, 0, 0x1eb8};

void cage_ready(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    fighter_start_move(fighter, S_JUMP, roster_def(fighter->roster)->moves[S_JUMP]);
  } else {
    fighter_enter_state(fighter, S_IDLE);
  }
}

static void cage_start_event_recovery(BattleEvent *event, int frames, int kind) {
  (void)kind;
  Fighter *owner = &g_battle.fighters[event->owner];
  native_hold(owner, frames, cage_ready);
}

bool cage_combo_event(BattleEvent *event, Fighter *owner, Fighter *opponent) {
  ContactResult contact;
  (void)opponent;

  if (event->phase == 0) {
    if (event->data[0] == 1) {
      contact = battle_contact(owner, &s_cage_powered_punch_contact);
      if (contact != CONTACT_MISS) {
        fighter_start_move(owner, S_KICK, &s_powered_punch_finish);
        fighter_start_fiber(owner, cage_entry_retail_script_004e80f4, NULL);
        return false;
      }
      cage_start_event_recovery(event, 6, CAGE_EVENT_POWERED_RECOVERY);
      return false;
    }

    contact = battle_contact(owner, &s_cage_powered_finish_contact);
    cage_start_event_recovery(event, contact == CONTACT_MISS ? 5 : s_powered_punch_finish.recovery, CAGE_EVENT_FINISH_RECOVERY);
    return false;
  }

  return false;
}

void cage_start_script(Fighter *fighter, StateId state, const MoveDef *move, FighterFn script) {
  fighter_start_move(fighter, state, move);
  if (state == S_SWEEP) {
    script(fighter);
  } else {
    fighter_start_fiber(fighter, script, NULL);
  }
}

void cage_script_lp_return(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter->flags &= ~(unsigned)FF_PAIRED;
  victim->flags &= ~(unsigned)FF_PAIRED;
  mk4_script_stop_body(fighter);
  fighter_finish_mmove(fighter, cage_ready);
}

void cage_script_finish(Fighter *fighter) {
  native_standard_sweep(fighter);
}

void cage_script_opponent_reaction(Fighter *fighter, int reaction) {
  if (reaction == 175) {
    return;
  }
  if (reaction == 77) {
    mk4_mmove_lk_release(fighter, reaction);
    return;
  }
  require(false, "Unexpected Cage opponent reaction");
}

void cage_script_current_reaction(Fighter *fighter, int reaction) {
  if (reaction == 185) {
    battle_react((Fighter *)fighter_opponent(fighter), reaction);
    return;
  }
  require(reaction == 100, "Unexpected Cage LP victim reaction");
  Fighter *victim = mk4_script_release_pair(fighter);
  battle_react(victim, reaction);
}
