#include "game.h"
#include <stddef.h>

/* Timings are one table shared by every fighter. Each fighter owns a clip
 * table indexed by state, in StateId order. Idle is CLIP_ASSET_IDLE because
 * each mesh already carries its own idle clip. Goro's mesh uses a separate
 * 21-channel bank, so his table names clips from that bank.
 *
 * Punch and uppercut come from the original move routines, which store an
 * action code at player +0x74 and start a move script. Action 0x10b plays
 * 294 (a complete jab; a repeated press continues into 295). Action 0x109
 * starts with 293. Clips 210..250 belong to the weapon routines and hold
 * a weapon grip, so they do not suit unarmed attacks. */
// clang-format off: Keep table entries on one row, including long rows.
static const StateDef mk4_states[S_COUNT] = {
  [S_IDLE] = {0, 0, 0, 0, 0, 0.f, 0.f, 0},
  [S_WALK] = {0, 0, 0, 0, 0, 0.f, 0.f, 0},
  [S_CROUCH] = {0, 0, 0, 0, 0, 0.f, 0.f, 0},
  [S_JUMP] = {0, 0, 0, 0, 0, 0.f, 0.f, 0},
  [S_PUNCH] = {5, 4, 12, 7, 19, 1.25f, 0.13f, SF_ATTACK | SF_LOCK},
  [S_KICK] = {10, 5, 18, 12, 25, 1.85f, 0.32f, SF_ATTACK | SF_LOCK},
  [S_SWEEP] = {11, 5, 23, 10, 45, 1.65f, 0.25f, SF_ATTACK | SF_LOCK | SF_LOW | SF_LAUNCH},
  [S_UPPERCUT] = {9, 5, 30, 18, 60, 1.35f, 0.5f, SF_ATTACK | SF_LOCK | SF_LAUNCH},
  [S_THROW] = {6, 2, 25, 14, 42, 0.95f, 0.45f, SF_ATTACK | SF_LOCK | SF_LAUNCH | SF_UNBLOCKABLE},
  [S_BLOCK] = {0, 0, 0, 0, 0, 0.f, 0.f, 0},
  [S_HIT] = {0, 0, 24, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION},
  [S_KNOCKDOWN] = {0, 0, 54, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_WAIT_LAND},
  [S_VICTORY] = {0, 0, 0, 0, 0, 0.f, 0.f, 0}
};
// clang-format on

_Static_assert(S_COUNT == 13, "Clip table columns follow StateId order");

// clang-format off: Keep table entries on one row, including long rows.
//                                               IDLE             WALK  CROUCH  JUMP  PUNCH  KICK  SWEEP  UPPER  THROW  BLOCK   HIT  KNOCK  VICTORY
static const int scorpion_clips[S_COUNT] =      {CLIP_ASSET_IDLE,  267,    219,  301,   294,   65,   101,   293,    23,   251,  247,   166,     293};
static const int raiden_clips[S_COUNT] =        {CLIP_ASSET_IDLE,  267,    219,  301,   294,   65,   101,   293,    23,   251,  247,   166,     293};
static const int sonya_clips[S_COUNT] =         {CLIP_ASSET_IDLE,  267,    219,  301,   294,   65,   101,   293,    23,   251,  247,   166,     293};
static const int liu_kang_clips[S_COUNT] =      {CLIP_ASSET_IDLE,  267,    219,  301,   294,   65,   101,   293,    23,   251,  247,   166,     293};
static const int sub_zero_clips[S_COUNT] =      {CLIP_ASSET_IDLE,  267,    219,  301,   294,   65,   101,   293,    23,   251,  247,   166,     293};
static const int fujin_clips[S_COUNT] =         {CLIP_ASSET_IDLE,  267,    219,  301,   294,   65,   101,   293,    23,   251,  247,   166,     293};
static const int shinnok_clips[S_COUNT] =       {CLIP_ASSET_IDLE,  267,    219,  301,   294,   65,   101,   293,    23,   251,  247,   166,     293};
static const int reiko_clips[S_COUNT] =         {CLIP_ASSET_IDLE,  267,    219,  301,   294,   65,   101,   293,    23,   251,  247,   166,     293};
static const int quan_chi_clips[S_COUNT] =      {CLIP_ASSET_IDLE,  267,    219,  301,   294,   65,   101,   293,    23,   251,  247,   166,     293};
static const int tanya_clips[S_COUNT] =         {CLIP_ASSET_IDLE,  267,    219,  301,   294,   65,   101,   293,    23,   251,  247,   166,     293};
static const int reptile_clips[S_COUNT] =       {CLIP_ASSET_IDLE,  267,    219,  301,   294,   65,   101,   293,    23,   251,  247,   166,     293};
static const int kai_clips[S_COUNT] =           {CLIP_ASSET_IDLE,  267,    219,  301,   294,   65,   101,   293,    23,   251,  247,   166,     293};
static const int jarek_clips[S_COUNT] =         {CLIP_ASSET_IDLE,  267,    219,  301,   294,   65,   101,   293,    23,   251,  247,   166,     293};
static const int jax_clips[S_COUNT] =           {CLIP_ASSET_IDLE,  267,    219,  301,   294,   65,   101,   293,    23,   251,  247,   166,     293};
static const int cage_clips[S_COUNT] =          {CLIP_ASSET_IDLE,  267,    219,  301,   294,   65,   101,   293,    23,   251,  247,   166,     293};
static const int goro_clips[S_COUNT] =          {CLIP_ASSET_IDLE,  590,    603,  602,   596,  598,   600,   593,   597,   595,  605,   581,     608};
static const int kitana_clips[S_COUNT] =        {CLIP_ASSET_IDLE,  267,    219,  301,   294,   65,   101,   293,    23,   251,  247,   166,     293};
static const int noob_saibot_clips[S_COUNT] =   {CLIP_ASSET_IDLE,  267,    219,  301,   294,   65,   101,   293,    23,   251,  247,   166,     293};
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
  {GAME_MK4, 0, "SCORPION", &mk4_motion, mk4_states, S_COUNT, scorpion_clips, mk4_commands, MK4_COMMAND_COUNT},
  {GAME_MK4, 1, "RAIDEN", &mk4_motion, mk4_states, S_COUNT, raiden_clips, mk4_commands, MK4_COMMAND_COUNT},
  {GAME_MK4, 2, "SONYA", &mk4_motion, mk4_states, S_COUNT, sonya_clips, mk4_commands, MK4_COMMAND_COUNT},
  {GAME_MK4, 3, "LIU KANG", &mk4_motion, mk4_states, S_COUNT, liu_kang_clips, mk4_commands, MK4_COMMAND_COUNT},
  {GAME_MK4, 4, "SUB ZERO", &mk4_motion, mk4_states, S_COUNT, sub_zero_clips, mk4_commands, MK4_COMMAND_COUNT},
  {GAME_MK4, 5, "FUJIN", &mk4_motion, mk4_states, S_COUNT, fujin_clips, mk4_commands, MK4_COMMAND_COUNT},
  {GAME_MK4, 6, "SHINNOK", &mk4_motion, mk4_states, S_COUNT, shinnok_clips, mk4_commands, MK4_COMMAND_COUNT},
  {GAME_MK4, 7, "REIKO", &mk4_motion, mk4_states, S_COUNT, reiko_clips, mk4_commands, MK4_COMMAND_COUNT},
  {GAME_MK4, 8, "QUAN CHI", &mk4_motion, mk4_states, S_COUNT, quan_chi_clips, mk4_commands, MK4_COMMAND_COUNT},
  {GAME_MK4, 9, "TANYA", &mk4_motion, mk4_states, S_COUNT, tanya_clips, mk4_commands, MK4_COMMAND_COUNT},
  {GAME_MK4, 10, "REPTILE", &mk4_motion, mk4_states, S_COUNT, reptile_clips, mk4_commands, MK4_COMMAND_COUNT},
  {GAME_MK4, 11, "KAI", &mk4_motion, mk4_states, S_COUNT, kai_clips, mk4_commands, MK4_COMMAND_COUNT},
  {GAME_MK4, 12, "JAREK", &mk4_motion, mk4_states, S_COUNT, jarek_clips, mk4_commands, MK4_COMMAND_COUNT},
  {GAME_MK4, 13, "JAX", &mk4_motion, mk4_states, S_COUNT, jax_clips, mk4_commands, MK4_COMMAND_COUNT},
  {GAME_MK4, 14, "CAGE", &mk4_motion, mk4_states, S_COUNT, cage_clips, mk4_commands, MK4_COMMAND_COUNT},
  {GAME_MK4, 15, "GORO", &mk4_motion, mk4_states, S_COUNT, goro_clips, mk4_commands, MK4_COMMAND_COUNT},
  {GAME_MK4, 16, "KITANA", &mk4_motion, mk4_states, S_COUNT, kitana_clips, mk4_commands, MK4_COMMAND_COUNT},
  {GAME_MK4, 17, "NOOB SAIBOT", &mk4_motion, mk4_states, S_COUNT, noob_saibot_clips, mk4_commands, MK4_COMMAND_COUNT}
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
  clip = def->clips[state];
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
  // clang-format off: Keep table entries on one row, including long rows.
  static const char *names[S_COUNT] = {
    "READY",
    "MOVE",
    "CROUCH",
    "JUMP",
    "PUNCH",
    "KICK",
    "SWEEP",
    "UPPERCUT",
    "THROW",
    "BLOCK",
    "HIT",
    "KNOCKDOWN",
    "WINNER"
  };
  // clang-format on
  if (state < 0 || state >= S_COUNT)
    return "";
  return names[state];
}
