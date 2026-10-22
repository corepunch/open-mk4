#include "data/layout.h"
#include "game.h"
#include "runtime/playback.h"
#include "native/capabilities.h"
#include "data/tables.h"
#include "script/vm.h"
#include "native/actions.h"
#include <stddef.h>
#include <stdio.h>

static void bind_commands(Controls *controls, const Mk4Image *image, unsigned character) {
  static CommandDef records[MK4_ROSTER_COUNT][64];
  require(character < MK4_ROSTER_COUNT, "Invalid retail controls owner");
  unsigned count = 0;
  for (Button button = BUTTON_UP; button < BUTTON_COUNT; button++) {
    for (unsigned released = 0; released < 2; ++released) {
      Mk4CommandCursor cursor = mk4_command_cursor(image, character, 0, button, released != 0);
      Mk4Command source;
      while (mk4_command_next(image, &cursor, &source)) {
        require(count < 64, "Retail command list exceeds controls view capacity");
        CommandDef *record = &records[character][count++];
        *record = (CommandDef){.button = button,
            .window = (int)source.window,
            .flags = (source.flags & (CMD_AIR | CMD_BLOCK_OK)) | (released ? CMD_RELEASE : 0),
            .predicate = script_native_predicate(source.predicate),
            .enter = script_native_entry(source.target)};
        for (uint32_t pattern = source.pattern == 0xffffffffu ? 0 : source.pattern; pattern; pattern >>= 8) {
          require(record->count < COMMAND_TOKENS, "Retail pattern exceeds controls view capacity");
          CommandToken token;
          switch (pattern & 255u) {
            case 1:
              token = TOKEN_UP;
              break;
            case 2:
              token = TOKEN_DOWN;
              break;
            case 0xaa:
              token = TOKEN_BACK;
              break;
            case 0xdd:
              token = TOKEN_FORWARD;
              break;
            default:
              fatal("Invalid retail command direction %02x", pattern & 255u);
          }
          record->tokens[record->count++] = token;
        }
      }
    }
  }
  controls->commands = records[character];
  controls->command_count = (int)count;
}

static void bind_move_list(CharacterDef *definition, const Mk4Image *image, unsigned character) {
  enum {
    CAPACITY = 64,
    INPUT_SIZE = 64
  };

  static MoveListEntry entries[MK4_ROSTER_COUNT][CAPACITY];
  static char inputs[MK4_ROSTER_COUNT][CAPACITY][INPUT_SIZE];
  static const char *buttons[BUTTON_COUNT] = {"U", "D", "L", "R", "HP", "LP", "BL", "HK", "LK", "RUN"};
  require(character < MK4_ROSTER_COUNT, "Invalid command display owner");
  unsigned count = 0;
  for (unsigned finish = 0; finish < 2; ++finish) {
    for (Button button = BUTTON_UP; button < BUTTON_COUNT; ++button) {
      for (unsigned released = 0; released < 2; ++released) {
        Mk4CommandCursor cursor = mk4_command_cursor(image, character, 0, button, released != 0);
        Mk4Command command;
        while (mk4_command_next(image, &cursor, &command)) {
          unsigned action = command.word & 255;
          bool finisher = (command.flags & 1) && action >= 64 && action <= 67;
          if (finisher != (finish != 0) || (command.pattern == 0xffffffffu && !released)) {
            continue;
          }
          require(count < CAPACITY, "Retail command display capacity exceeded");
          char *input = inputs[character][count];
          size_t used = 0;
          if (released) {
            used = (size_t)snprintf(input, INPUT_SIZE, "RELEASE ");
          }
          uint32_t pattern = command.pattern == 0xffffffffu ? 0 : command.pattern;
          for (; pattern; pattern >>= 8) {
            const char *direction = (pattern & 255) == 1 ? "U" : (pattern & 255) == 2 ? "D" : (pattern & 255) == 0xaa ? "B" : "F";
            int written = snprintf(input + used, INPUT_SIZE - used, "%s%s", direction, pattern >> 8 ? "," : "+");
            require(written >= 0 && (size_t)written < INPUT_SIZE - used, "Retail command display input too long");
            used += (size_t)written;
          }
          int written = snprintf(input + used, INPUT_SIZE - used, "%s", buttons[button]);
          require(written >= 0 && (size_t)written < INPUT_SIZE - used, "Retail command display button too long");
          const char *name = finisher                              ? (action == 64      ? "FATALITY 1"
                                                                         : action == 67 ? "FATALITY 2"
                                                                                        : "STAGE FINISH")
                             : (command.flags & 1) && action == 41 ? "WEAPON"
                                                                   : "SPECIAL";
          entries[character][count] = (MoveListEntry){finisher ? MOVE_LIST_FINISHER : MOVE_LIST_SPECIAL, name, input};
          count++;
        }
      }
    }
  }
  definition->move_list = entries[character];
  definition->move_list_count = (int)count;
}

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
  fighter_execution_cancel(fighter);
}

void action_victory_continue(Fighter *fighter) {
  script_vm_enter(fighter, MK4_SCRIPT_RAIDEN_VICTORY, fighter->execution.script.active ? fighter->execution.script.finish : victory_finished);
}

void action_victory(Fighter *fighter) {
  fighter_start_move(fighter, S_VICTORY, &engine_states[S_VICTORY]);
  action_victory_continue(fighter);
}

static const FighterFn state_entries[S_COUNT] = {
    [S_IDLE] = weapon_idle,
    [S_WALK] = weapon_walk,
    [S_CROUCH] = weapon_crouch,
    [S_PUNCH] = action_jab,
    [S_KICK] = action_high_kick,
    [S_SWEEP] = action_sweep,
    [S_UPPERCUT] = action_uppercut,
    [S_THROW] = action_lp_throw,
    [S_BLOCK] = weapon_block,
    [S_VICTORY] = action_victory,
};

static int descriptor_clip(uint32_t packed) {
  require(packed >= MK4_ANIMATION_TABLE / 4 && packed < (MK4_ANIMATION_TABLE + MK4_CLIP_COUNT * MK4_ANIMATION_DESCRIPTOR_SIZE) / 4 &&
              (packed * 4 - MK4_ANIMATION_TABLE) % MK4_ANIMATION_DESCRIPTOR_SIZE == 0,
      "Invalid native state descriptor");
  return (int)((packed * 4 - MK4_ANIMATION_TABLE) / MK4_ANIMATION_DESCRIPTOR_SIZE);
}

int roster_count(void) {
  return MK4_ROSTER_COUNT;
}

const CharacterDef *roster_def(int index) {
  require(index >= 0 && index < roster_count(), "Roster index out of range");
  static CharacterDef definitions[MK4_ROSTER_COUNT];
  static Controls controls[MK4_ROSTER_COUNT];
  static CpuMoves cpu[MK4_ROSTER_COUNT];
  static Mk4Roster identities[MK4_ROSTER_COUNT];
  static MoveDef states[MK4_ROSTER_COUNT][S_COUNT];
  static const MoveDef *state_views[MK4_ROSTER_COUNT][S_COUNT];
  static const Mk4Image *images[MK4_ROSTER_COUNT];
  const Mk4Image *image = battle_data();
  if (images[index] != image) {
    definitions[index] = (CharacterDef){.motion = &mk4_motion, .move_count = S_COUNT, .enter = state_entries, .walk = mk4_walk, .jump = mk4_jump};
    mk4_roster_read(image, (unsigned)index, &identities[index]);
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
    states[index][S_BLOCK].clip = descriptor_clip(index == 15 ? MK4_ANIMATION_DESCRIPTOR_CLIP_595 / 4 : mk4_image_u32(image, MK4_DATA_ROSTER_DEF));
    states[index][S_IDLE].clip = identities[index].idle;
    states[index][S_WALK].clip = identities[index].walk;
    definitions[index].moves = state_views[index];
    controls[index] = action_stances;
    bind_commands(&controls[index], image, (unsigned)index);
    definitions[index].controls = &controls[index];
    bind_move_list(&definitions[index], image, (unsigned)index);
    cpu[index] = (CpuMoves){0};
    cpu[index].enter[CPU_JAB] = action_jab;
    cpu[index].enter[CPU_LOW_KICK] = action_low_kick;
    cpu[index].enter[CPU_THROW] = action_lp_throw;
    cpu[index].enter[CPU_CLOSE_HP] = script_native_entry(mk4_action_target(image, 1, 9));
    cpu[index].enter[CPU_CLOSE_HK] = script_native_entry(mk4_action_target(image, 1, 12));
    /* Native weapon draw is available only when present in the loaded input
     * lists. This also respects boss slots without a draw action. */
    uint32_t draw = mk4_action_target(image, 1, 41);
    for (Button button = BUTTON_UP; button < BUTTON_COUNT; ++button) {
      Mk4CommandCursor cursor = mk4_command_cursor(image, (unsigned)index, 0, button, false);
      Mk4Command command;
      while (mk4_command_next(image, &cursor, &command)) {
        if (draw != 0 && command.target == draw) {
          cpu[index].enter[CPU_DRAW] = script_native_entry(draw);
        }
      }
    }
    const CpuAction slots[4] = {CPU_SPECIAL_NEAR, CPU_SPECIAL_1, CPU_SPECIAL_2, CPU_PROJECTILE};
    for (unsigned table = 0; table < 4; ++table) {
      uint32_t target = mk4_cpu_target(image, (unsigned)index, table);
      cpu[index].enter[slots[table]] = target ? script_native_entry(target) : NULL;
    }
    cpu[index].enter[CPU_FINISH_1] = mk4_finish_target(image, (unsigned)index, false) ? action_finish_first : NULL;
    cpu[index].enter[CPU_FINISH_2] = mk4_finish_target(image, (unsigned)index, true) ? action_finish_second : NULL;
    cpu[index].enter[CPU_FINISH_FAN] = action_fan_finish;
    cpu[index].enter[CPU_FINISH_SPIKE] = action_spike_finish;
    for (unsigned finish = 0; finish < 2; ++finish) {
      cpu[index].finish_distance[finish] = mk4_cpu_finish_distance(image, (unsigned)index, finish != 0);
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
