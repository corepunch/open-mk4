#include "game.h"
#include "mk4_characters.h"
#include "retail_data.h"
#include "retail_vm.h"
#include "retail_native.h"
#include <stddef.h>

/* Retail data supplies roster identity and script animation. The remaining
 * legacy rows provide native services while their families are consolidated;
 * shared script states never copy their authored attack seeds. */
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
  static FighterFn entries[MK4_ROSTER_COUNT][S_COUNT];
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
      bool retail_attack = state >= S_PUNCH && state <= S_UPPERCUT;
      states[index][state] = state == S_IDLE || state == S_WALK || retail_attack ? (MoveDef){.clip = -1, .last = MOVE_END} : *roster[index]->moves[state];
      entries[index][state] = roster[index]->enter != NULL ? roster[index]->enter[state] : NULL;
      if (retail_attack) {
        /* Unstarted engine views have no authored clip or balance. Playback
         * binds the original descriptor before its first rendered sample. */
        states[index][state].flags = SF_ATTACK | SF_LOCK | SF_RECORD;
      }
      state_views[index][state] = &states[index][state];
    }
    states[index][S_IDLE].clip = identities[index].idle;
    states[index][S_WALK].clip = identities[index].walk;
    definitions[index].moves = state_views[index];
    entries[index][S_PUNCH] = retail_jab;
    entries[index][S_KICK] = retail_high_kick;
    entries[index][S_SWEEP] = retail_sweep;
    entries[index][S_UPPERCUT] = retail_uppercut;
    definitions[index].enter = entries[index];
    controls[index] = *roster[index]->controls;
    retail_bind_commands(&controls[index], image, (unsigned)index);
    definitions[index].controls = &controls[index];
    cpu[index] = roster[index]->cpu != NULL ? *roster[index]->cpu : (CpuMoves){0};
    const CpuAction slots[4] = {CPU_SPECIAL_NEAR, CPU_SPECIAL_1, CPU_SPECIAL_2, CPU_PROJECTILE};
    for (unsigned table = 0; table < 4; ++table) {
      uint32_t target = retail_cpu_target(image, (unsigned)index, table);
      cpu[index].enter[slots[table]] = target ? retail_native_entry((unsigned)index, target) : NULL;
    }
    cpu[index].enter[CPU_FINISH_1] = retail_finish_target(image, (unsigned)index, false) ? retail_finish_first : NULL;
    cpu[index].enter[CPU_FINISH_2] = retail_finish_target(image, (unsigned)index, true) ? retail_finish_second : NULL;
    cpu[index].enter[CPU_FINISH_FAN] = retail_fan_finish;
    cpu[index].enter[CPU_FINISH_SPIKE] = retail_spike_finish;
    for (unsigned finish = 0; finish < 2; ++finish) {
      cpu[index].finish_distance[finish] = retail_cpu_finish_distance(image, (unsigned)index, finish != 0);
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
