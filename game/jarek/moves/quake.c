#include "../character.h"

/* B,D,B+HK -> 47b5d0: action 0x105, then generated 4e81a8 stamps clip 107
 * to pose 20 with indexed 33, bank C 68 and shake 3 before tail 47b600. */
static const MoveDef quake_move = {.clip = 107, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = 0x105};

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
    /* 4ea4e8[136] -> 4834d0 is roster-generic: the shared reaction table
     * runs Goro's port of it (disarm, 4ea8f8 voice, victim script 4e95c8). */
    battle_react(victim, 136);
  }
  jarek_entry_retail_script_004e81c8(fighter);
}
