#include "retail_layout.h"
#include "game.h"
#include "native.h"
#include "native_services.h"
#include "retail_data.h"
#include "retail_vm.h"
#include "retail_native.h"
#include <stddef.h>

/* Engine state views are shared native boundaries. Original programs bind
 * attack/victory animation; these views contain no authored fighter attacks. */
static const MoveDef engine_states[S_COUNT] = {
    [S_IDLE] = {.clip = -1, .last = MOVE_END},
    [S_WALK] = {.clip = -1, .last = MOVE_END},
    [S_CROUCH] = {.clip = -1, .last = MOVE_END, .flags = SF_HOLD},
    [S_JUMP] = {.clip = -1, .last = MOVE_END},
    [S_PUNCH] = {.clip = -1, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_RECORD},
    [S_KICK] = {.clip = -1, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_RECORD},
    [S_SWEEP] = {.clip = -1, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_RECORD},
    [S_UPPERCUT] = {.clip = -1, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_RECORD},
    [S_THROW] = {.clip = -1, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_RECORD},
    [S_BLOCK] = {.clip = -1, .last = MOVE_END, .flags = SF_HOLD},
    [S_HIT] = {.clip = -1, .last = MOVE_END, .recovery = 24, .flags = SF_LOCK | SF_REACTION | SF_HOLD},
    [S_KNOCKDOWN] = {.clip = -1, .last = MOVE_END, .recovery = 54, .flags = SF_LOCK | SF_REACTION | SF_WAIT_LAND},
    [S_VICTORY] = {.clip = -1, .last = MOVE_END, .flags = SF_HOLD},
};

static void victory_finished(Fighter *fighter) {
  fighter_cancel_mmove(fighter);
}

void retail_victory_continue(Fighter *fighter) {
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_RAIDEN_SCRIPT_VICTORY_4E6A40, fighter->retail_vm.active ? fighter->retail_vm.finish : victory_finished);
}

void retail_victory(Fighter *fighter) {
  fighter_start_move(fighter, S_VICTORY, &engine_states[S_VICTORY]);
  retail_victory_continue(fighter);
}

static const FighterFn state_entries[S_COUNT] = {
    [S_IDLE] = weapon_idle,
    [S_WALK] = weapon_walk,
    [S_CROUCH] = weapon_crouch,
    [S_PUNCH] = retail_jab,
    [S_KICK] = retail_high_kick,
    [S_SWEEP] = retail_sweep,
    [S_UPPERCUT] = retail_uppercut,
    [S_THROW] = retail_lp_throw,
    [S_BLOCK] = weapon_block,
    [S_VICTORY] = retail_victory,
};

static int descriptor_clip(uint32_t packed) {
  require(packed >= RETAIL_ANIMATION_TABLE / 4 && packed < (RETAIL_ANIMATION_TABLE + MK4_CLIP_COUNT * RETAIL_ANIMATION_DESCRIPTOR_SIZE) / 4 &&
              (packed * 4 - RETAIL_ANIMATION_TABLE) % RETAIL_ANIMATION_DESCRIPTOR_SIZE == 0,
      "Invalid native state descriptor");
  return (int)((packed * 4 - RETAIL_ANIMATION_TABLE) / RETAIL_ANIMATION_DESCRIPTOR_SIZE);
}

int roster_count(void) {
  return MK4_ROSTER_COUNT;
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
    definitions[index] = (CharacterDef){.motion = &mk4_motion, .move_count = S_COUNT, .enter = state_entries, .walk = mk4_walk, .jump = mk4_jump};
    retail_roster_read(image, (unsigned)index, &identities[index]);
    definitions[index].game = GAME_MK4;
    definitions[index].asset = index;
    definitions[index].name = identities[index].name;
    definitions[index].ground = identities[index].ground;
    for (int state = 0; state < S_COUNT; ++state) {
      states[index][state] = engine_states[state];
      state_views[index][state] = &states[index][state];
    }
    /* Native46b300 guard: the boss descriptor is a native literal and the
     * ordinary descriptor is loaded. Other native entries bind on takeover;
     * unstarted views do not invent fighter-compatible animation seeds. */
    states[index][S_BLOCK].clip =
        descriptor_clip(index == 15 ? MK4_RETAIL_ANIMATION_DESCRIPTOR_CLIP_595_4FC960 / 4 : mk4_image_u32(image, MK4_RETAIL_DATA_ROSTER_DEF_53CE30));
    states[index][S_IDLE].clip = identities[index].idle;
    states[index][S_WALK].clip = identities[index].walk;
    definitions[index].moves = state_views[index];
    controls[index] = retail_stances;
    retail_bind_commands(&controls[index], image, (unsigned)index);
    definitions[index].controls = &controls[index];
    retail_bind_move_list(&definitions[index], image, (unsigned)index);
    cpu[index] = (CpuMoves){0};
    cpu[index].enter[CPU_JAB] = retail_jab;
    cpu[index].enter[CPU_LOW_KICK] = retail_low_kick;
    cpu[index].enter[CPU_THROW] = retail_lp_throw;
    cpu[index].enter[CPU_CLOSE_HP] = retail_native_entry(retail_action_target(image, 1, 9));
    cpu[index].enter[CPU_CLOSE_HK] = retail_native_entry(retail_action_target(image, 1, 12));
    /* Native weapon draw is available only when present in the loaded input
     * lists. This also respects boss slots without a draw action. */
    uint32_t draw = retail_action_target(image, 1, 41);
    for (Button button = BUTTON_UP; button < BUTTON_COUNT; ++button) {
      RetailCommandCursor cursor = retail_command_cursor(image, (unsigned)index, 0, button, false);
      RetailCommand command;
      while (retail_command_next(image, &cursor, &command)) {
        if (draw != 0 && command.target == draw) {
          cpu[index].enter[CPU_DRAW] = retail_native_entry(draw);
        }
      }
    }
    const CpuAction slots[4] = {CPU_SPECIAL_NEAR, CPU_SPECIAL_1, CPU_SPECIAL_2, CPU_PROJECTILE};
    for (unsigned table = 0; table < 4; ++table) {
      uint32_t target = retail_cpu_target(image, (unsigned)index, table);
      cpu[index].enter[slots[table]] = target ? retail_native_entry(target) : NULL;
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
