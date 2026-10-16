#include "game.h"
#include <stddef.h>

/* Timings are one table. Human fighters read clip ids from these rows.
 * Goro points at the same rows and overrides the clip ids; his mesh uses
 * a separate 21-channel bank. Idle is CLIP_ASSET_IDLE because each mesh
 * already carries its own idle clip.
 *
 * Punch and uppercut come from the original move routines, which store an
 * action code at player +0x74 and start a move script. Action 0x10b plays
 * 294 (a complete jab; a repeated press continues into 295). Action 0x109
 * starts with 293. Clips 210..250 belong to the weapon routines and hold
 * a weapon grip, so they do not suit unarmed attacks. */
// clang-format off: Keep table entries on one row, including long rows.
static const StateDef mk4_states[S_COUNT] = {
  [S_IDLE] = {CLIP_ASSET_IDLE, 0, 0, 0, 0, 0, 0.f, 0.f, 0},
  [S_WALK] = {267, 0, 0, 0, 0, 0, 0.f, 0.f, 0},
  [S_CROUCH] = {219, 0, 0, 0, 0, 0, 0.f, 0.f, 0},
  [S_JUMP] = {301, 0, 0, 0, 0, 0, 0.f, 0.f, 0},
  [S_PUNCH] = {294, 5, 4, 12, 7, 19, 1.25f, 0.13f, SF_ATTACK | SF_LOCK},
  [S_KICK] = {65, 10, 5, 18, 12, 25, 1.85f, 0.32f, SF_ATTACK | SF_LOCK},
  [S_SWEEP] = {101, 11, 5, 23, 10, 45, 1.65f, 0.25f, SF_ATTACK | SF_LOCK | SF_LOW | SF_LAUNCH},
  [S_UPPERCUT] = {293, 9, 5, 30, 18, 60, 1.35f, 0.5f, SF_ATTACK | SF_LOCK | SF_LAUNCH},
  [S_THROW] = {23, 6, 2, 25, 14, 42, 0.95f, 0.45f, SF_ATTACK | SF_LOCK | SF_LAUNCH | SF_UNBLOCKABLE},
  [S_BLOCK] = {251, 0, 0, 0, 0, 0, 0.f, 0.f, 0},
  [S_HIT] = {247, 0, 0, 24, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION},
  [S_KNOCKDOWN] = {166, 0, 0, 54, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_WAIT_LAND},
  [S_VICTORY] = {293, 0, 0, 0, 0, 0, 0.f, 0.f, 0}
};
// clang-format on

// clang-format off: Keep table entries on one row, including long rows.
static const int goro_clips[S_COUNT] = {
  [S_IDLE] = CLIP_ASSET_IDLE,
  [S_WALK] = 590,
  [S_CROUCH] = 603,
  [S_JUMP] = 602,
  [S_PUNCH] = 596,
  [S_KICK] = 598,
  [S_SWEEP] = 600,
  [S_UPPERCUT] = 593,
  [S_THROW] = 597,
  [S_BLOCK] = 595,
  [S_HIT] = 605,
  [S_KNOCKDOWN] = 581,
  [S_VICTORY] = 608
};
// clang-format on

/* First match wins. Crouch punch is an uppercut and crouch kick is a sweep,
 * for every fighter who points at this table. A later character that breaks
 * the rule gets its own command rows. */
// clang-format off: Keep table entries on one row, including long rows.
static const CommandDef mk4_commands[] = {
  {BUTTON_THROW, CMD_GROUNDED | CMD_STANDING, S_THROW},
  {BUTTON_UPPERCUT, CMD_GROUNDED, S_UPPERCUT},
  {BUTTON_PUNCH, CMD_CROUCHING, S_UPPERCUT},
  {BUTTON_PUNCH, 0, S_PUNCH},
  {BUTTON_KICK, CMD_CROUCHING, S_SWEEP},
  {BUTTON_KICK, 0, S_KICK}
};
// clang-format on

static const Motion mk4_motion = {2.5f, 4.5f, 6.8f, 15.f};

enum {
  MK4_COMMAND_COUNT = (int)(sizeof mk4_commands / sizeof mk4_commands[0])
};

// clang-format off: Keep table entries on one row, including long rows.
static const CharacterDef roster[] = {
  {GAME_MK4, 0, "SCORPION", &mk4_motion, mk4_states, S_COUNT, NULL, mk4_commands, MK4_COMMAND_COUNT},
  {GAME_MK4, 1, "RAIDEN", &mk4_motion, mk4_states, S_COUNT, NULL, mk4_commands, MK4_COMMAND_COUNT},
  {GAME_MK4, 2, "SONYA", &mk4_motion, mk4_states, S_COUNT, NULL, mk4_commands, MK4_COMMAND_COUNT},
  {GAME_MK4, 3, "LIU KANG", &mk4_motion, mk4_states, S_COUNT, NULL, mk4_commands, MK4_COMMAND_COUNT},
  {GAME_MK4, 4, "SUB ZERO", &mk4_motion, mk4_states, S_COUNT, NULL, mk4_commands, MK4_COMMAND_COUNT},
  {GAME_MK4, 5, "FUJIN", &mk4_motion, mk4_states, S_COUNT, NULL, mk4_commands, MK4_COMMAND_COUNT},
  {GAME_MK4, 6, "SHINNOK", &mk4_motion, mk4_states, S_COUNT, NULL, mk4_commands, MK4_COMMAND_COUNT},
  {GAME_MK4, 7, "REIKO", &mk4_motion, mk4_states, S_COUNT, NULL, mk4_commands, MK4_COMMAND_COUNT},
  {GAME_MK4, 8, "QUAN CHI", &mk4_motion, mk4_states, S_COUNT, NULL, mk4_commands, MK4_COMMAND_COUNT},
  {GAME_MK4, 9, "TANYA", &mk4_motion, mk4_states, S_COUNT, NULL, mk4_commands, MK4_COMMAND_COUNT},
  {GAME_MK4, 10, "REPTILE", &mk4_motion, mk4_states, S_COUNT, NULL, mk4_commands, MK4_COMMAND_COUNT},
  {GAME_MK4, 11, "KAI", &mk4_motion, mk4_states, S_COUNT, NULL, mk4_commands, MK4_COMMAND_COUNT},
  {GAME_MK4, 12, "JAREK", &mk4_motion, mk4_states, S_COUNT, NULL, mk4_commands, MK4_COMMAND_COUNT},
  {GAME_MK4, 13, "JAX", &mk4_motion, mk4_states, S_COUNT, NULL, mk4_commands, MK4_COMMAND_COUNT},
  {GAME_MK4, 14, "CAGE", &mk4_motion, mk4_states, S_COUNT, NULL, mk4_commands, MK4_COMMAND_COUNT},
  {GAME_MK4, 15, "GORO", &mk4_motion, mk4_states, S_COUNT, goro_clips, mk4_commands, MK4_COMMAND_COUNT},
  {GAME_MK4, 16, "KITANA", &mk4_motion, mk4_states, S_COUNT, NULL, mk4_commands, MK4_COMMAND_COUNT},
  {GAME_MK4, 17, "NOOB SAIBOT", &mk4_motion, mk4_states, S_COUNT, NULL, mk4_commands, MK4_COMMAND_COUNT}
};
// clang-format on

int roster_count(void) {
  return (int)(sizeof roster / sizeof roster[0]);
}

const CharacterDef *roster_def(int index) {
  require(index >= 0 && index < roster_count(), "Roster index out of range");
  return &roster[index];
}

const StateDef *fighter_state(const Fighter *fighter) {
  const CharacterDef *def = roster_def(fighter->roster);
  require(fighter->state >= 0 && fighter->state < def->state_count, "Fighter state out of range");
  return &def->states[fighter->state];
}

int roster_clip(const CharacterDef *def, int state, int asset_idle) {
  int clip;
  require(state >= 0 && state < def->state_count, "State has no clip");
  clip = def->clips ? def->clips[state] : def->states[state].clip;
  return clip < 0 ? asset_idle : clip;
}

const Character *roster_character(const Assets *assets, int roster_index) {
  const CharacterDef *def = roster_def(roster_index);
  require(def->game == GAME_MK4, "Only the MK4 mesh bank is loaded");
  require(def->asset >= 0 && (size_t)def->asset < assets->characters.count,
      "Roster asset out of range");
  return &assets->characters.data[def->asset];
}

const char *state_name(int state) {
  static const char *names[S_COUNT] = {"READY", "MOVE", "CROUCH", "JUMP", "PUNCH", "KICK", "SWEEP",
      "UPPERCUT", "THROW", "BLOCK", "HIT", "KNOCKDOWN", "WINNER"};
  if (state < 0 || state >= S_COUNT)
    return "";
  return names[state];
}
