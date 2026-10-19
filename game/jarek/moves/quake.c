#include "../character.h"

/* B,D,B+HK -> 47b5d0: action 0x105, then generated 4e81a8 stamps clip 107
 * to pose 20 with indexed 33, bank C 68 and shake 3 before tail 47b600. */
static const MoveDef quake_move = {.clip = 107, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = 0x105};
static const MoveDef stumble_move = {.clip = 182, .last = 64, .flags = SF_LOCK | SF_REACTION, .action = ACTION_RETAIL_4001};

void jarek_quake(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &quake_move);
  fighter->sample_override = 0;
  jarek_entry_retail_script_004e81a8(fighter);
}

/* 47b600: action 0x200f, 415180 ground-shock decoration, then 48ee60/48ef40
 * test the peer: an airborne peer is unaffected, a grounded one takes the
 * unblockable 48de80 damage and reaction 136. 47b680 enters 4e81c8. */
void jarek_quake_tail(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter->action = 0x200f;
  if (!fighter_airborne(victim)) {
    battle_damage(victim, 0x23d7);
    battle_react(victim, 136);
  }
  jarek_entry_retail_script_004e81c8(fighter);
}

/* 4ea4e8[136] -> 4834d0: 494520 drops the weapon, 48a320 -> 489e80 picks
 * one of tokens 0x78/0x7d/0x82 from 4ea8f8, then generated 4e95c8. */
void jarek_quake_victim(Fighter *fighter) {
  /* The same tokens are bank A15, A13 and B60. */
  static const int banks[3] = {SOUND_A, SOUND_A, SOUND_B};
  static const int selectors[3] = {0x0f, 0x0d, 0x3c};
  uint32_t pick = battle_random(3);
  mk4_script_disarm_self(fighter);
  battle_sound(banks[pick], selectors[pick]);
  fighter_start_move(fighter, S_HIT, &stumble_move);
  fighter->flags |= FF_REACTING;
  jarek_entry_retail_script_004e95c8(fighter);
}
