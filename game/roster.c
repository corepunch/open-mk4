#include "game.h"
#include "mk4_characters.h"
#include "retail_data.h"
#include "retail_vm.h"
#include "retail_native.h"
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
 * low knockdown. His hit (593) and knockdown (581) are visual choices.
 * Clip 581 keeps a root Y near +0.87, which plants its last frame; the
 * first frame is still about 0.49 below the floor. His bank has no flatter
 * grounded fall, so that opening stays. */
static const CharacterDef *const roster[] = {
    &mk4_scorpion,
    &mk4_raiden,
    &mk4_sonya,
    &mk4_liu_kang,
    &mk4_sub_zero,
    &mk4_fujin,
    &mk4_shinnok,
    &mk4_reiko,
    &mk4_quan_chi,
    &mk4_tanya,
    &mk4_reptile,
    &mk4_kai,
    &mk4_jarek,
    &mk4_jax,
    &mk4_cage,
    &mk4_goro,
    &mk4_kitana,
    &mk4_noob_saibot,
};

int roster_count(void) {
  return (int)(sizeof roster / sizeof roster[0]);
}

const CharacterDef *roster_def(int index) {
  require(index >= 0 && index < roster_count(), "Roster index out of range");
  static CharacterDef definitions[MK4_ROSTER_COUNT];
  static Controls controls[MK4_ROSTER_COUNT];
  static CpuMoves cpu[MK4_ROSTER_COUNT];
  static RetailRoster identities[MK4_ROSTER_COUNT];
  static MoveDef states[MK4_ROSTER_COUNT][S_COUNT];
  static const MoveDef *state_views[MK4_ROSTER_COUNT][S_COUNT];
  static const Mk4Image *images[MK4_ROSTER_COUNT];
  const Mk4Image *image = retail_vm_image();
  if (images[index] != image) {
    definitions[index] = *roster[index];
    retail_roster_read(image, (unsigned)index, &identities[index]);
    definitions[index].game = GAME_MK4;
    definitions[index].asset = index;
    definitions[index].name = identities[index].name;
    definitions[index].ground = identities[index].ground;
    require(definitions[index].move_count == S_COUNT, "Native state view has an invalid shape");
    for (int state = 0; state < S_COUNT; ++state) {
      states[index][state] = state == S_IDLE || state == S_WALK ? (MoveDef){.last = MOVE_END} : *roster[index]->moves[state];
      state_views[index][state] = &states[index][state];
    }
    states[index][S_IDLE].clip = identities[index].idle;
    states[index][S_WALK].clip = identities[index].walk;
    definitions[index].moves = state_views[index];
    controls[index] = *roster[index]->controls;
    retail_bind_commands(&controls[index], image, (unsigned)index);
    definitions[index].controls = &controls[index];
    cpu[index] = roster[index]->cpu != NULL ? *roster[index]->cpu : (CpuMoves){0};
    const CpuAction slots[4] = {CPU_SPECIAL_NEAR, CPU_SPECIAL_1, CPU_SPECIAL_2, CPU_PROJECTILE};
    for (unsigned table = 0; table < 4; ++table) {
      uint32_t target = retail_cpu_target(image, (unsigned)index, table);
      cpu[index].enter[slots[table]] = target ? retail_native_entry((unsigned)index, target) : NULL;
    }
    unsigned slot = retail_character_slot((unsigned)index);
    for (unsigned finish = 0; finish < 2; ++finish) {
      cpu[index].finish_distance[finish] = (q16)mk4_image_u32(image, 0x4dfe10 + finish * 0x40 + slot * 4);
    }
    definitions[index].cpu = &cpu[index];
    images[index] = image;
  }
  return &definitions[index];
}

const MoveDef *fighter_move(const Fighter *fighter) {
  const CharacterDef *def = roster_def(fighter->roster);
  require(fighter->state >= 0 && fighter->state < def->move_count, "Fighter state out of range");
  return fighter->current_move ? fighter->current_move : def->moves[fighter->state];
}

int roster_clip(const CharacterDef *def, int state, int asset_idle) {
  int clip;
  require(state >= 0 && state < def->move_count, "State has no clip");
  clip = def->moves[state]->clip;
  return clip < 0 ? asset_idle : clip;
}

const Character *roster_character(const Assets *assets, int roster_index) {
  const CharacterDef *def = roster_def(roster_index);
  require(def->game == GAME_MK4, "Only the MK4 mesh bank is loaded");
  require(def->asset >= 0 && (size_t)def->asset < assets->characters.count, "Roster asset out of range");
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
