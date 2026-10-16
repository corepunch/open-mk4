#include "game.h"
#include <stddef.h>

/* Each fighter owns a move table indexed by StateId. Columns: clip, first
 * and last frame, startup, active, recovery, damage, stun, reach, push,
 * flags. Idle is CLIP_ASSET_IDLE because each mesh
 * already carries its own idle clip. Goro's mesh uses a separate 21-channel
 * bank, so his table names clips from that bank.
 *
 * Clips come from the original move routines, which store an action code
 * at player +0x74 and run a move script; op 0x3c in a script branches on a
 * 16-entry character table (Kitana reads Sonya's entry, Noob Reiko's).
 *   walk    forward-walk table 0x53d388 (0x53d3c8 holds the slower walk back)
 *   crouch  0x2001 plays 259 into the crouch, then loops 258
 *   jump    0x2007 plays 304: takeoff, air, landing
 *   punch   0x10b plays the jab 294 (295 follows a repeated press)
 *   kick    0x100 high kick, character table at 0x4e5ca4
 *   sweep   0x10c, character table at 0x4e679c
 *   upper   0x109 starts with 293
 *   throw   the character table at 0x4de7a0 sets the victim's clip (op
 *           0x0e) and the thrower's (op 0x0f); Jax's entry calls 0x42c7f0,
 *           which switches to script 0x4de4f0 (83, victim 145)
 *   block   0x2005 sets slot 0x53ce30 (322), or 595 for Goro
 *   hit     0x2005 reaction script starts with 247
 *   knock   0x4003 staggers with 311, then falls with 186
 *   victory the round-end scripts call 0x4712e0, whose character table at
 *           0x4e6a44 opens each win pose and then waits on its last frame
 * Clips 210..250 belong to the weapon routines and hold a weapon grip;
 * the 0x4e5efc table (44..51) holds AI katas, not win poses.
 * Goro's moves come from his AI handlers: 597 punch (0x43ae70), 598 kick
 * (0x43b170), 600 uppercut (slot 0x53c6bc at 0x43b75f), 601 leap and
 * stomp (action 0x409), 602 jump (0x43b7e0), and 603, a rise from the
 * crouch, played backwards as his duck. He has no sweep; the stomp is his
 * low knockdown. His hit (593) and knockdown (581) are visual choices. */
// clang-format off: Keep table entries on one row, including long rows.
static const MoveDef scorpion_moves[S_COUNT] = {
  [S_IDLE] =      {CLIP_ASSET_IDLE, 0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   0},
  [S_WALK] =      {269,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   0},
  [S_CROUCH] =    {259,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   SF_HOLD},
  [S_JUMP] =      {304,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   0},
  [S_PUNCH] =     {294,             0, MOVE_END, 5,  4, 12, 7,  19, 1.25f, 0.13f, SF_ATTACK | SF_LOCK},
  [S_KICK] =      {66,              0, MOVE_END, 10, 5, 18, 12, 25, 1.85f, 0.32f, SF_ATTACK | SF_LOCK},
  [S_SWEEP] =     {63,              0, MOVE_END, 11, 5, 23, 10, 45, 1.65f, 0.25f, SF_ATTACK | SF_LOCK | SF_LOW | SF_LAUNCH},
  [S_UPPERCUT] =  {293,             0, MOVE_END, 9,  5, 30, 18, 60, 1.35f, 0.5f,  SF_ATTACK | SF_LOCK | SF_LAUNCH},
  [S_THROW] =     {75,              0, MOVE_END, 6,  2, 25, 14, 42, 0.95f, 0.45f, SF_ATTACK | SF_LOCK | SF_LAUNCH | SF_UNBLOCKABLE},
  [S_BLOCK] =     {322,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   SF_HOLD},
  [S_HIT] =       {247,             0, MOVE_END, 0,  0, 24, 0,  0,  0.f,   0.f,   SF_LOCK | SF_REACTION | SF_HOLD},
  [S_KNOCKDOWN] = {186,             0, MOVE_END, 0,  0, 54, 0,  0,  0.f,   0.f,   SF_LOCK | SF_REACTION | SF_WAIT_LAND},
  [S_VICTORY] =   {115,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   SF_HOLD}
};

static const MoveDef raiden_moves[S_COUNT] = {
  [S_IDLE] =      {CLIP_ASSET_IDLE, 0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   0},
  [S_WALK] =      {267,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   0},
  [S_CROUCH] =    {259,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   SF_HOLD},
  [S_JUMP] =      {304,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   0},
  [S_PUNCH] =     {294,             0, MOVE_END, 5,  4, 12, 7,  19, 1.25f, 0.13f, SF_ATTACK | SF_LOCK},
  [S_KICK] =      {71,              0, MOVE_END, 10, 5, 18, 12, 25, 1.85f, 0.32f, SF_ATTACK | SF_LOCK},
  [S_SWEEP] =     {62,              0, MOVE_END, 11, 5, 23, 10, 45, 1.65f, 0.25f, SF_ATTACK | SF_LOCK | SF_LOW | SF_LAUNCH},
  [S_UPPERCUT] =  {293,             0, MOVE_END, 9,  5, 30, 18, 60, 1.35f, 0.5f,  SF_ATTACK | SF_LOCK | SF_LAUNCH},
  [S_THROW] =     {78,              0, MOVE_END, 6,  2, 25, 14, 42, 0.95f, 0.45f, SF_ATTACK | SF_LOCK | SF_LAUNCH | SF_UNBLOCKABLE},
  [S_BLOCK] =     {322,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   SF_HOLD},
  [S_HIT] =       {247,             0, MOVE_END, 0,  0, 24, 0,  0,  0.f,   0.f,   SF_LOCK | SF_REACTION | SF_HOLD},
  [S_KNOCKDOWN] = {186,             0, MOVE_END, 0,  0, 54, 0,  0,  0.f,   0.f,   SF_LOCK | SF_REACTION | SF_WAIT_LAND},
  [S_VICTORY] =   {123,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   SF_HOLD}
};

static const MoveDef sonya_moves[S_COUNT] = {
  [S_IDLE] =      {CLIP_ASSET_IDLE, 0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   0},
  [S_WALK] =      {267,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   0},
  [S_CROUCH] =    {259,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   SF_HOLD},
  [S_JUMP] =      {304,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   0},
  [S_PUNCH] =     {294,             0, MOVE_END, 5,  4, 12, 7,  19, 1.25f, 0.13f, SF_ATTACK | SF_LOCK},
  [S_KICK] =      {65,              0, MOVE_END, 10, 5, 18, 12, 25, 1.85f, 0.32f, SF_ATTACK | SF_LOCK},
  [S_SWEEP] =     {63,              0, MOVE_END, 11, 5, 23, 10, 45, 1.65f, 0.25f, SF_ATTACK | SF_LOCK | SF_LOW | SF_LAUNCH},
  [S_UPPERCUT] =  {293,             0, MOVE_END, 9,  5, 30, 18, 60, 1.35f, 0.5f,  SF_ATTACK | SF_LOCK | SF_LAUNCH},
  [S_THROW] =     {78,              0, MOVE_END, 6,  2, 25, 14, 42, 0.95f, 0.45f, SF_ATTACK | SF_LOCK | SF_LAUNCH | SF_UNBLOCKABLE},
  [S_BLOCK] =     {322,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   SF_HOLD},
  [S_HIT] =       {247,             0, MOVE_END, 0,  0, 24, 0,  0,  0.f,   0.f,   SF_LOCK | SF_REACTION | SF_HOLD},
  [S_KNOCKDOWN] = {186,             0, MOVE_END, 0,  0, 54, 0,  0,  0.f,   0.f,   SF_LOCK | SF_REACTION | SF_WAIT_LAND},
  [S_VICTORY] =   {114,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   SF_HOLD}
};

static const MoveDef liu_kang_moves[S_COUNT] = {
  [S_IDLE] =      {CLIP_ASSET_IDLE, 0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   0},
  [S_WALK] =      {266,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   0},
  [S_CROUCH] =    {259,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   SF_HOLD},
  [S_JUMP] =      {304,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   0},
  [S_PUNCH] =     {294,             0, MOVE_END, 5,  4, 12, 7,  19, 1.25f, 0.13f, SF_ATTACK | SF_LOCK},
  [S_KICK] =      {66,              0, MOVE_END, 10, 5, 18, 12, 25, 1.85f, 0.32f, SF_ATTACK | SF_LOCK},
  [S_SWEEP] =     {62,              0, MOVE_END, 11, 5, 23, 10, 45, 1.65f, 0.25f, SF_ATTACK | SF_LOCK | SF_LOW | SF_LAUNCH},
  [S_UPPERCUT] =  {293,             0, MOVE_END, 9,  5, 30, 18, 60, 1.35f, 0.5f,  SF_ATTACK | SF_LOCK | SF_LAUNCH},
  [S_THROW] =     {79,              0, MOVE_END, 6,  2, 25, 14, 42, 0.95f, 0.45f, SF_ATTACK | SF_LOCK | SF_LAUNCH | SF_UNBLOCKABLE},
  [S_BLOCK] =     {322,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   SF_HOLD},
  [S_HIT] =       {247,             0, MOVE_END, 0,  0, 24, 0,  0,  0.f,   0.f,   SF_LOCK | SF_REACTION | SF_HOLD},
  [S_KNOCKDOWN] = {186,             0, MOVE_END, 0,  0, 54, 0,  0,  0.f,   0.f,   SF_LOCK | SF_REACTION | SF_WAIT_LAND},
  [S_VICTORY] =   {119,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   SF_HOLD}
};

static const MoveDef sub_zero_moves[S_COUNT] = {
  [S_IDLE] =      {CLIP_ASSET_IDLE, 0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   0},
  [S_WALK] =      {267,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   0},
  [S_CROUCH] =    {259,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   SF_HOLD},
  [S_JUMP] =      {304,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   0},
  [S_PUNCH] =     {294,             0, MOVE_END, 5,  4, 12, 7,  19, 1.25f, 0.13f, SF_ATTACK | SF_LOCK},
  [S_KICK] =      {71,              0, MOVE_END, 10, 5, 18, 12, 25, 1.85f, 0.32f, SF_ATTACK | SF_LOCK},
  [S_SWEEP] =     {63,              0, MOVE_END, 11, 5, 23, 10, 45, 1.65f, 0.25f, SF_ATTACK | SF_LOCK | SF_LOW | SF_LAUNCH},
  [S_UPPERCUT] =  {293,             0, MOVE_END, 9,  5, 30, 18, 60, 1.35f, 0.5f,  SF_ATTACK | SF_LOCK | SF_LAUNCH},
  [S_THROW] =     {76,              0, MOVE_END, 6,  2, 25, 14, 42, 0.95f, 0.45f, SF_ATTACK | SF_LOCK | SF_LAUNCH | SF_UNBLOCKABLE},
  [S_BLOCK] =     {322,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   SF_HOLD},
  [S_HIT] =       {247,             0, MOVE_END, 0,  0, 24, 0,  0,  0.f,   0.f,   SF_LOCK | SF_REACTION | SF_HOLD},
  [S_KNOCKDOWN] = {186,             0, MOVE_END, 0,  0, 54, 0,  0,  0.f,   0.f,   SF_LOCK | SF_REACTION | SF_WAIT_LAND},
  [S_VICTORY] =   {115,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   SF_HOLD}
};

static const MoveDef fujin_moves[S_COUNT] = {
  [S_IDLE] =      {CLIP_ASSET_IDLE, 0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   0},
  [S_WALK] =      {267,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   0},
  [S_CROUCH] =    {259,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   SF_HOLD},
  [S_JUMP] =      {304,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   0},
  [S_PUNCH] =     {294,             0, MOVE_END, 5,  4, 12, 7,  19, 1.25f, 0.13f, SF_ATTACK | SF_LOCK},
  [S_KICK] =      {65,              0, MOVE_END, 10, 5, 18, 12, 25, 1.85f, 0.32f, SF_ATTACK | SF_LOCK},
  [S_SWEEP] =     {62,              0, MOVE_END, 11, 5, 23, 10, 45, 1.65f, 0.25f, SF_ATTACK | SF_LOCK | SF_LOW | SF_LAUNCH},
  [S_UPPERCUT] =  {293,             0, MOVE_END, 9,  5, 30, 18, 60, 1.35f, 0.5f,  SF_ATTACK | SF_LOCK | SF_LAUNCH},
  [S_THROW] =     {81,              0, MOVE_END, 6,  2, 25, 14, 42, 0.95f, 0.45f, SF_ATTACK | SF_LOCK | SF_LAUNCH | SF_UNBLOCKABLE},
  [S_BLOCK] =     {322,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   SF_HOLD},
  [S_HIT] =       {247,             0, MOVE_END, 0,  0, 24, 0,  0,  0.f,   0.f,   SF_LOCK | SF_REACTION | SF_HOLD},
  [S_KNOCKDOWN] = {186,             0, MOVE_END, 0,  0, 54, 0,  0,  0.f,   0.f,   SF_LOCK | SF_REACTION | SF_WAIT_LAND},
  [S_VICTORY] =   {120,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   SF_HOLD}
};

static const MoveDef shinnok_moves[S_COUNT] = {
  [S_IDLE] =      {CLIP_ASSET_IDLE, 0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   0},
  [S_WALK] =      {267,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   0},
  [S_CROUCH] =    {259,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   SF_HOLD},
  [S_JUMP] =      {304,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   0},
  [S_PUNCH] =     {294,             0, MOVE_END, 5,  4, 12, 7,  19, 1.25f, 0.13f, SF_ATTACK | SF_LOCK},
  [S_KICK] =      {66,              0, MOVE_END, 10, 5, 18, 12, 25, 1.85f, 0.32f, SF_ATTACK | SF_LOCK},
  [S_SWEEP] =     {63,              0, MOVE_END, 11, 5, 23, 10, 45, 1.65f, 0.25f, SF_ATTACK | SF_LOCK | SF_LOW | SF_LAUNCH},
  [S_UPPERCUT] =  {293,             0, MOVE_END, 9,  5, 30, 18, 60, 1.35f, 0.5f,  SF_ATTACK | SF_LOCK | SF_LAUNCH},
  [S_THROW] =     {75,              0, MOVE_END, 6,  2, 25, 14, 42, 0.95f, 0.45f, SF_ATTACK | SF_LOCK | SF_LAUNCH | SF_UNBLOCKABLE},
  [S_BLOCK] =     {322,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   SF_HOLD},
  [S_HIT] =       {247,             0, MOVE_END, 0,  0, 24, 0,  0,  0.f,   0.f,   SF_LOCK | SF_REACTION | SF_HOLD},
  [S_KNOCKDOWN] = {186,             0, MOVE_END, 0,  0, 54, 0,  0,  0.f,   0.f,   SF_LOCK | SF_REACTION | SF_WAIT_LAND},
  [S_VICTORY] =   {111,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   SF_HOLD}
};

static const MoveDef reiko_moves[S_COUNT] = {
  [S_IDLE] =      {CLIP_ASSET_IDLE, 0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   0},
  [S_WALK] =      {267,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   0},
  [S_CROUCH] =    {259,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   SF_HOLD},
  [S_JUMP] =      {304,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   0},
  [S_PUNCH] =     {294,             0, MOVE_END, 5,  4, 12, 7,  19, 1.25f, 0.13f, SF_ATTACK | SF_LOCK},
  [S_KICK] =      {66,              0, MOVE_END, 10, 5, 18, 12, 25, 1.85f, 0.32f, SF_ATTACK | SF_LOCK},
  [S_SWEEP] =     {62,              0, MOVE_END, 11, 5, 23, 10, 45, 1.65f, 0.25f, SF_ATTACK | SF_LOCK | SF_LOW | SF_LAUNCH},
  [S_UPPERCUT] =  {293,             0, MOVE_END, 9,  5, 30, 18, 60, 1.35f, 0.5f,  SF_ATTACK | SF_LOCK | SF_LAUNCH},
  [S_THROW] =     {78,              0, MOVE_END, 6,  2, 25, 14, 42, 0.95f, 0.45f, SF_ATTACK | SF_LOCK | SF_LAUNCH | SF_UNBLOCKABLE},
  [S_BLOCK] =     {322,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   SF_HOLD},
  [S_HIT] =       {247,             0, MOVE_END, 0,  0, 24, 0,  0,  0.f,   0.f,   SF_LOCK | SF_REACTION | SF_HOLD},
  [S_KNOCKDOWN] = {186,             0, MOVE_END, 0,  0, 54, 0,  0,  0.f,   0.f,   SF_LOCK | SF_REACTION | SF_WAIT_LAND},
  [S_VICTORY] =   {113,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   SF_HOLD}
};

static const MoveDef quan_chi_moves[S_COUNT] = {
  [S_IDLE] =      {CLIP_ASSET_IDLE, 0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   0},
  [S_WALK] =      {267,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   0},
  [S_CROUCH] =    {259,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   SF_HOLD},
  [S_JUMP] =      {304,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   0},
  [S_PUNCH] =     {294,             0, MOVE_END, 5,  4, 12, 7,  19, 1.25f, 0.13f, SF_ATTACK | SF_LOCK},
  [S_KICK] =      {71,              0, MOVE_END, 10, 5, 18, 12, 25, 1.85f, 0.32f, SF_ATTACK | SF_LOCK},
  [S_SWEEP] =     {63,              0, MOVE_END, 11, 5, 23, 10, 45, 1.65f, 0.25f, SF_ATTACK | SF_LOCK | SF_LOW | SF_LAUNCH},
  [S_UPPERCUT] =  {293,             0, MOVE_END, 9,  5, 30, 18, 60, 1.35f, 0.5f,  SF_ATTACK | SF_LOCK | SF_LAUNCH},
  [S_THROW] =     {78,              0, MOVE_END, 6,  2, 25, 14, 42, 0.95f, 0.45f, SF_ATTACK | SF_LOCK | SF_LAUNCH | SF_UNBLOCKABLE},
  [S_BLOCK] =     {322,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   SF_HOLD},
  [S_HIT] =       {247,             0, MOVE_END, 0,  0, 24, 0,  0,  0.f,   0.f,   SF_LOCK | SF_REACTION | SF_HOLD},
  [S_KNOCKDOWN] = {186,             0, MOVE_END, 0,  0, 54, 0,  0,  0.f,   0.f,   SF_LOCK | SF_REACTION | SF_WAIT_LAND},
  [S_VICTORY] =   {125,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   SF_HOLD}
};

static const MoveDef tanya_moves[S_COUNT] = {
  [S_IDLE] =      {CLIP_ASSET_IDLE, 0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   0},
  [S_WALK] =      {267,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   0},
  [S_CROUCH] =    {259,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   SF_HOLD},
  [S_JUMP] =      {304,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   0},
  [S_PUNCH] =     {294,             0, MOVE_END, 5,  4, 12, 7,  19, 1.25f, 0.13f, SF_ATTACK | SF_LOCK},
  [S_KICK] =      {66,              0, MOVE_END, 10, 5, 18, 12, 25, 1.85f, 0.32f, SF_ATTACK | SF_LOCK},
  [S_SWEEP] =     {62,              0, MOVE_END, 11, 5, 23, 10, 45, 1.65f, 0.25f, SF_ATTACK | SF_LOCK | SF_LOW | SF_LAUNCH},
  [S_UPPERCUT] =  {293,             0, MOVE_END, 9,  5, 30, 18, 60, 1.35f, 0.5f,  SF_ATTACK | SF_LOCK | SF_LAUNCH},
  [S_THROW] =     {79,              0, MOVE_END, 6,  2, 25, 14, 42, 0.95f, 0.45f, SF_ATTACK | SF_LOCK | SF_LAUNCH | SF_UNBLOCKABLE},
  [S_BLOCK] =     {322,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   SF_HOLD},
  [S_HIT] =       {247,             0, MOVE_END, 0,  0, 24, 0,  0,  0.f,   0.f,   SF_LOCK | SF_REACTION | SF_HOLD},
  [S_KNOCKDOWN] = {186,             0, MOVE_END, 0,  0, 54, 0,  0,  0.f,   0.f,   SF_LOCK | SF_REACTION | SF_WAIT_LAND},
  [S_VICTORY] =   {124,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   SF_HOLD}
};

static const MoveDef reptile_moves[S_COUNT] = {
  [S_IDLE] =      {CLIP_ASSET_IDLE, 0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   0},
  [S_WALK] =      {268,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   0},
  [S_CROUCH] =    {259,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   SF_HOLD},
  [S_JUMP] =      {304,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   0},
  [S_PUNCH] =     {294,             0, MOVE_END, 5,  4, 12, 7,  19, 1.25f, 0.13f, SF_ATTACK | SF_LOCK},
  [S_KICK] =      {66,              0, MOVE_END, 10, 5, 18, 12, 25, 1.85f, 0.32f, SF_ATTACK | SF_LOCK},
  [S_SWEEP] =     {63,              0, MOVE_END, 11, 5, 23, 10, 45, 1.65f, 0.25f, SF_ATTACK | SF_LOCK | SF_LOW | SF_LAUNCH},
  [S_UPPERCUT] =  {293,             0, MOVE_END, 9,  5, 30, 18, 60, 1.35f, 0.5f,  SF_ATTACK | SF_LOCK | SF_LAUNCH},
  [S_THROW] =     {76,              0, MOVE_END, 6,  2, 25, 14, 42, 0.95f, 0.45f, SF_ATTACK | SF_LOCK | SF_LAUNCH | SF_UNBLOCKABLE},
  [S_BLOCK] =     {322,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   SF_HOLD},
  [S_HIT] =       {247,             0, MOVE_END, 0,  0, 24, 0,  0,  0.f,   0.f,   SF_LOCK | SF_REACTION | SF_HOLD},
  [S_KNOCKDOWN] = {186,             0, MOVE_END, 0,  0, 54, 0,  0,  0.f,   0.f,   SF_LOCK | SF_REACTION | SF_WAIT_LAND},
  [S_VICTORY] =   {121,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   SF_HOLD}
};

static const MoveDef kai_moves[S_COUNT] = {
  [S_IDLE] =      {CLIP_ASSET_IDLE, 0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   0},
  [S_WALK] =      {265,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   0},
  [S_CROUCH] =    {259,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   SF_HOLD},
  [S_JUMP] =      {304,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   0},
  [S_PUNCH] =     {294,             0, MOVE_END, 5,  4, 12, 7,  19, 1.25f, 0.13f, SF_ATTACK | SF_LOCK},
  [S_KICK] =      {71,              0, MOVE_END, 10, 5, 18, 12, 25, 1.85f, 0.32f, SF_ATTACK | SF_LOCK},
  [S_SWEEP] =     {62,              0, MOVE_END, 11, 5, 23, 10, 45, 1.65f, 0.25f, SF_ATTACK | SF_LOCK | SF_LOW | SF_LAUNCH},
  [S_UPPERCUT] =  {293,             0, MOVE_END, 9,  5, 30, 18, 60, 1.35f, 0.5f,  SF_ATTACK | SF_LOCK | SF_LAUNCH},
  [S_THROW] =     {81,              0, MOVE_END, 6,  2, 25, 14, 42, 0.95f, 0.45f, SF_ATTACK | SF_LOCK | SF_LAUNCH | SF_UNBLOCKABLE},
  [S_BLOCK] =     {322,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   SF_HOLD},
  [S_HIT] =       {247,             0, MOVE_END, 0,  0, 24, 0,  0,  0.f,   0.f,   SF_LOCK | SF_REACTION | SF_HOLD},
  [S_KNOCKDOWN] = {186,             0, MOVE_END, 0,  0, 54, 0,  0,  0.f,   0.f,   SF_LOCK | SF_REACTION | SF_WAIT_LAND},
  [S_VICTORY] =   {122,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   SF_HOLD}
};

static const MoveDef jarek_moves[S_COUNT] = {
  [S_IDLE] =      {CLIP_ASSET_IDLE, 0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   0},
  [S_WALK] =      {267,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   0},
  [S_CROUCH] =    {259,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   SF_HOLD},
  [S_JUMP] =      {304,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   0},
  [S_PUNCH] =     {294,             0, MOVE_END, 5,  4, 12, 7,  19, 1.25f, 0.13f, SF_ATTACK | SF_LOCK},
  [S_KICK] =      {65,              0, MOVE_END, 10, 5, 18, 12, 25, 1.85f, 0.32f, SF_ATTACK | SF_LOCK},
  [S_SWEEP] =     {63,              0, MOVE_END, 11, 5, 23, 10, 45, 1.65f, 0.25f, SF_ATTACK | SF_LOCK | SF_LOW | SF_LAUNCH},
  [S_UPPERCUT] =  {293,             0, MOVE_END, 9,  5, 30, 18, 60, 1.35f, 0.5f,  SF_ATTACK | SF_LOCK | SF_LAUNCH},
  [S_THROW] =     {78,              0, MOVE_END, 6,  2, 25, 14, 42, 0.95f, 0.45f, SF_ATTACK | SF_LOCK | SF_LAUNCH | SF_UNBLOCKABLE},
  [S_BLOCK] =     {322,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   SF_HOLD},
  [S_HIT] =       {247,             0, MOVE_END, 0,  0, 24, 0,  0,  0.f,   0.f,   SF_LOCK | SF_REACTION | SF_HOLD},
  [S_KNOCKDOWN] = {186,             0, MOVE_END, 0,  0, 54, 0,  0,  0.f,   0.f,   SF_LOCK | SF_REACTION | SF_WAIT_LAND},
  [S_VICTORY] =   {125,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   SF_HOLD}
};

static const MoveDef jax_moves[S_COUNT] = {
  [S_IDLE] =      {CLIP_ASSET_IDLE, 0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   0},
  [S_WALK] =      {265,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   0},
  [S_CROUCH] =    {259,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   SF_HOLD},
  [S_JUMP] =      {304,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   0},
  [S_PUNCH] =     {294,             0, MOVE_END, 5,  4, 12, 7,  19, 1.25f, 0.13f, SF_ATTACK | SF_LOCK},
  [S_KICK] =      {66,              0, MOVE_END, 10, 5, 18, 12, 25, 1.85f, 0.32f, SF_ATTACK | SF_LOCK},
  [S_SWEEP] =     {62,              0, MOVE_END, 11, 5, 23, 10, 45, 1.65f, 0.25f, SF_ATTACK | SF_LOCK | SF_LOW | SF_LAUNCH},
  [S_UPPERCUT] =  {293,             0, MOVE_END, 9,  5, 30, 18, 60, 1.35f, 0.5f,  SF_ATTACK | SF_LOCK | SF_LAUNCH},
  [S_THROW] =     {83,              0, MOVE_END, 6,  2, 25, 14, 42, 0.95f, 0.45f, SF_ATTACK | SF_LOCK | SF_LAUNCH | SF_UNBLOCKABLE},
  [S_BLOCK] =     {322,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   SF_HOLD},
  [S_HIT] =       {247,             0, MOVE_END, 0,  0, 24, 0,  0,  0.f,   0.f,   SF_LOCK | SF_REACTION | SF_HOLD},
  [S_KNOCKDOWN] = {186,             0, MOVE_END, 0,  0, 54, 0,  0,  0.f,   0.f,   SF_LOCK | SF_REACTION | SF_WAIT_LAND},
  [S_VICTORY] =   {114,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   SF_HOLD}
};

static const MoveDef cage_moves[S_COUNT] = {
  [S_IDLE] =      {CLIP_ASSET_IDLE, 0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   0},
  [S_WALK] =      {267,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   0},
  [S_CROUCH] =    {259,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   SF_HOLD},
  [S_JUMP] =      {304,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   0},
  [S_PUNCH] =     {294,             0, MOVE_END, 5,  4, 12, 7,  19, 1.25f, 0.13f, SF_ATTACK | SF_LOCK},
  [S_KICK] =      {71,              0, MOVE_END, 10, 5, 18, 12, 25, 1.85f, 0.32f, SF_ATTACK | SF_LOCK},
  [S_SWEEP] =     {63,              0, MOVE_END, 11, 5, 23, 10, 45, 1.65f, 0.25f, SF_ATTACK | SF_LOCK | SF_LOW | SF_LAUNCH},
  [S_UPPERCUT] =  {293,             0, MOVE_END, 9,  5, 30, 18, 60, 1.35f, 0.5f,  SF_ATTACK | SF_LOCK | SF_LAUNCH},
  [S_THROW] =     {75,              0, MOVE_END, 6,  2, 25, 14, 42, 0.95f, 0.45f, SF_ATTACK | SF_LOCK | SF_LAUNCH | SF_UNBLOCKABLE},
  [S_BLOCK] =     {322,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   SF_HOLD},
  [S_HIT] =       {247,             0, MOVE_END, 0,  0, 24, 0,  0,  0.f,   0.f,   SF_LOCK | SF_REACTION | SF_HOLD},
  [S_KNOCKDOWN] = {186,             0, MOVE_END, 0,  0, 54, 0,  0,  0.f,   0.f,   SF_LOCK | SF_REACTION | SF_WAIT_LAND},
  [S_VICTORY] =   {111,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   SF_HOLD}
};

static const MoveDef goro_moves[S_COUNT] = {
  [S_IDLE] =      {CLIP_ASSET_IDLE, 0,  MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   0},
  [S_WALK] =      {590,             0,  MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   0},
  [S_CROUCH] =    {603,             18, 0,        0,  0, 0,  0,  0,  0.f,   0.f,   SF_HOLD},
  [S_JUMP] =      {602,             0,  MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   0},
  [S_PUNCH] =     {597,             0,  MOVE_END, 5,  4, 12, 7,  19, 1.25f, 0.13f, SF_ATTACK | SF_LOCK},
  [S_KICK] =      {598,             0,  MOVE_END, 10, 5, 18, 12, 25, 1.85f, 0.32f, SF_ATTACK | SF_LOCK},
  [S_SWEEP] =     {601,             0,  MOVE_END, 11, 5, 23, 10, 45, 1.65f, 0.25f, SF_ATTACK | SF_LOCK | SF_LOW | SF_LAUNCH},
  [S_UPPERCUT] =  {600,             0,  MOVE_END, 9,  5, 30, 18, 60, 1.35f, 0.5f,  SF_ATTACK | SF_LOCK | SF_LAUNCH},
  [S_THROW] =     {604,             0,  MOVE_END, 6,  2, 25, 14, 42, 0.95f, 0.45f, SF_ATTACK | SF_LOCK | SF_LAUNCH | SF_UNBLOCKABLE},
  [S_BLOCK] =     {595,             0,  MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   SF_HOLD},
  [S_HIT] =       {593,             0,  MOVE_END, 0,  0, 24, 0,  0,  0.f,   0.f,   SF_LOCK | SF_REACTION | SF_HOLD},
  [S_KNOCKDOWN] = {581,             0,  MOVE_END, 0,  0, 54, 0,  0,  0.f,   0.f,   SF_LOCK | SF_REACTION | SF_WAIT_LAND},
  [S_VICTORY] =   {594,             0,  MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   SF_HOLD}
};

static const MoveDef kitana_moves[S_COUNT] = {
  [S_IDLE] =      {CLIP_ASSET_IDLE, 0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   0},
  [S_WALK] =      {267,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   0},
  [S_CROUCH] =    {259,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   SF_HOLD},
  [S_JUMP] =      {304,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   0},
  [S_PUNCH] =     {294,             0, MOVE_END, 5,  4, 12, 7,  19, 1.25f, 0.13f, SF_ATTACK | SF_LOCK},
  [S_KICK] =      {65,              0, MOVE_END, 10, 5, 18, 12, 25, 1.85f, 0.32f, SF_ATTACK | SF_LOCK},
  [S_SWEEP] =     {63,              0, MOVE_END, 11, 5, 23, 10, 45, 1.65f, 0.25f, SF_ATTACK | SF_LOCK | SF_LOW | SF_LAUNCH},
  [S_UPPERCUT] =  {293,             0, MOVE_END, 9,  5, 30, 18, 60, 1.35f, 0.5f,  SF_ATTACK | SF_LOCK | SF_LAUNCH},
  [S_THROW] =     {78,              0, MOVE_END, 6,  2, 25, 14, 42, 0.95f, 0.45f, SF_ATTACK | SF_LOCK | SF_LAUNCH | SF_UNBLOCKABLE},
  [S_BLOCK] =     {322,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   SF_HOLD},
  [S_HIT] =       {247,             0, MOVE_END, 0,  0, 24, 0,  0,  0.f,   0.f,   SF_LOCK | SF_REACTION | SF_HOLD},
  [S_KNOCKDOWN] = {186,             0, MOVE_END, 0,  0, 54, 0,  0,  0.f,   0.f,   SF_LOCK | SF_REACTION | SF_WAIT_LAND},
  [S_VICTORY] =   {114,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   SF_HOLD}
};

static const MoveDef noob_saibot_moves[S_COUNT] = {
  [S_IDLE] =      {CLIP_ASSET_IDLE, 0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   0},
  [S_WALK] =      {267,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   0},
  [S_CROUCH] =    {259,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   SF_HOLD},
  [S_JUMP] =      {304,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   0},
  [S_PUNCH] =     {294,             0, MOVE_END, 5,  4, 12, 7,  19, 1.25f, 0.13f, SF_ATTACK | SF_LOCK},
  [S_KICK] =      {66,              0, MOVE_END, 10, 5, 18, 12, 25, 1.85f, 0.32f, SF_ATTACK | SF_LOCK},
  [S_SWEEP] =     {62,              0, MOVE_END, 11, 5, 23, 10, 45, 1.65f, 0.25f, SF_ATTACK | SF_LOCK | SF_LOW | SF_LAUNCH},
  [S_UPPERCUT] =  {293,             0, MOVE_END, 9,  5, 30, 18, 60, 1.35f, 0.5f,  SF_ATTACK | SF_LOCK | SF_LAUNCH},
  [S_THROW] =     {78,              0, MOVE_END, 6,  2, 25, 14, 42, 0.95f, 0.45f, SF_ATTACK | SF_LOCK | SF_LAUNCH | SF_UNBLOCKABLE},
  [S_BLOCK] =     {322,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   SF_HOLD},
  [S_HIT] =       {247,             0, MOVE_END, 0,  0, 24, 0,  0,  0.f,   0.f,   SF_LOCK | SF_REACTION | SF_HOLD},
  [S_KNOCKDOWN] = {186,             0, MOVE_END, 0,  0, 54, 0,  0,  0.f,   0.f,   SF_LOCK | SF_REACTION | SF_WAIT_LAND},
  [S_VICTORY] =   {113,             0, MOVE_END, 0,  0, 0,  0,  0,  0.f,   0.f,   SF_HOLD}
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
  {GAME_MK4, 0, "SCORPION", &mk4_motion, scorpion_moves, S_COUNT, mk4_commands, MK4_COMMAND_COUNT},
  {GAME_MK4, 1, "RAIDEN", &mk4_motion, raiden_moves, S_COUNT, mk4_commands, MK4_COMMAND_COUNT},
  {GAME_MK4, 2, "SONYA", &mk4_motion, sonya_moves, S_COUNT, mk4_commands, MK4_COMMAND_COUNT},
  {GAME_MK4, 3, "LIU KANG", &mk4_motion, liu_kang_moves, S_COUNT, mk4_commands, MK4_COMMAND_COUNT},
  {GAME_MK4, 4, "SUB ZERO", &mk4_motion, sub_zero_moves, S_COUNT, mk4_commands, MK4_COMMAND_COUNT},
  {GAME_MK4, 5, "FUJIN", &mk4_motion, fujin_moves, S_COUNT, mk4_commands, MK4_COMMAND_COUNT},
  {GAME_MK4, 6, "SHINNOK", &mk4_motion, shinnok_moves, S_COUNT, mk4_commands, MK4_COMMAND_COUNT},
  {GAME_MK4, 7, "REIKO", &mk4_motion, reiko_moves, S_COUNT, mk4_commands, MK4_COMMAND_COUNT},
  {GAME_MK4, 8, "QUAN CHI", &mk4_motion, quan_chi_moves, S_COUNT, mk4_commands, MK4_COMMAND_COUNT},
  {GAME_MK4, 9, "TANYA", &mk4_motion, tanya_moves, S_COUNT, mk4_commands, MK4_COMMAND_COUNT},
  {GAME_MK4, 10, "REPTILE", &mk4_motion, reptile_moves, S_COUNT, mk4_commands, MK4_COMMAND_COUNT},
  {GAME_MK4, 11, "KAI", &mk4_motion, kai_moves, S_COUNT, mk4_commands, MK4_COMMAND_COUNT},
  {GAME_MK4, 12, "JAREK", &mk4_motion, jarek_moves, S_COUNT, mk4_commands, MK4_COMMAND_COUNT},
  {GAME_MK4, 13, "JAX", &mk4_motion, jax_moves, S_COUNT, mk4_commands, MK4_COMMAND_COUNT},
  {GAME_MK4, 14, "CAGE", &mk4_motion, cage_moves, S_COUNT, mk4_commands, MK4_COMMAND_COUNT},
  {GAME_MK4, 15, "GORO", &mk4_motion, goro_moves, S_COUNT, mk4_commands, MK4_COMMAND_COUNT},
  {GAME_MK4, 16, "KITANA", &mk4_motion, kitana_moves, S_COUNT, mk4_commands, MK4_COMMAND_COUNT},
  {GAME_MK4, 17, "NOOB SAIBOT", &mk4_motion, noob_saibot_moves, S_COUNT, mk4_commands, MK4_COMMAND_COUNT}
};
// clang-format on

int roster_count(void) {
  return (int)(sizeof roster / sizeof roster[0]);
}

const CharacterDef *roster_def(int index) {
  require(index >= 0 && index < roster_count(), "Roster index out of range");
  return &roster[index];
}

const MoveDef *fighter_move(const Fighter *fighter) {
  const CharacterDef *def = roster_def(fighter->roster);
  require(fighter->state >= 0 && fighter->state < def->move_count, "Fighter state out of range");
  return &def->moves[fighter->state];
}

int roster_clip(const CharacterDef *def, int state, int asset_idle) {
  int clip;
  require(state >= 0 && state < def->move_count, "State has no clip");
  clip = def->moves[state].clip;
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
