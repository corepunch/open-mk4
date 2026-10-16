#ifndef MK4_GAME_H
#define MK4_GAME_H
/* One header for the match: input frames, the character roster, the two live
 * fighters, and the draw entry points. Asset and renderer headers stay in
 * common/ because the viewer uses them too.
 *
 * A character is a row in the roster, not a subclass. The row points at a
 * motion record, immutable moves, entry callbacks, and a command table.
 * Each character's C file owns its moves and any custom logic. The live
 * fighter keeps the current move; battle_step runs shared combat rules.
 *
 * MK4 fills the roster today. War Gods (Midway arcade, Eurocom PC port, its
 * own wgfsys archive) is another game id: append rows, point them at that
 * game's state and clip tables, and load a second asset bank. The step
 * function does not branch on the game id. */
#include "renderer.h"
#include <stdbool.h>
#include <stdint.h>

#define TICK_SECONDS (1.f / 60.f)
#define CLIP_ASSET_IDLE (-1)

typedef enum GameId {
  GAME_MK4 = 0,
  GAME_WARGODS = 1
} GameId;

typedef enum Button {
  BUTTON_LEFT,
  BUTTON_RIGHT,
  BUTTON_UP,
  BUTTON_DOWN,
  BUTTON_BLOCK,
  BUTTON_RUN,
  BUTTON_PUNCH,
  BUTTON_KICK,
  BUTTON_UPPERCUT,
  BUTTON_THROW,
  BUTTON_COUNT
} Button;

#define BUTTON_BIT(button) (1u << (button))

/* held is the buttons down during this tick. pressed is the subset that
 * went down since the previous sample. The match reads held for movement
 * and block, and pressed for attacks, so a held punch does not repeat. */
typedef struct Input {
  unsigned held;
  unsigned pressed;
} Input;

static inline bool input_held(Input in, Button button) {
  return (in.held & BUTTON_BIT(button)) != 0;
}

static inline bool input_pressed(Input in, Button button) {
  return (in.pressed & BUTTON_BIT(button)) != 0;
}

/* code[] holds keyboard scancodes. The numbers come from SDL; this header
 * does not include SDL. */
typedef struct InputDevice {
  int code[BUTTON_COUNT];
  unsigned held;
} InputDevice;

typedef enum ShellCommand {
  SHELL_NONE = 0,
  SHELL_QUIT,
  SHELL_PAUSE,
  SHELL_RESET,
  SHELL_REMATCH,
  SHELL_NEXT_P1,
  SHELL_NEXT_P2,
  SHELL_TOGGLE_CPU,
  SHELL_NEXT_ARENA
} ShellCommand;

typedef enum StateId {
  S_IDLE,
  S_WALK,
  S_CROUCH,
  S_JUMP,
  S_PUNCH,
  S_KICK,
  S_SWEEP,
  S_UPPERCUT,
  S_THROW,
  S_BLOCK,
  S_HIT,
  S_KNOCKDOWN,
  S_VICTORY,
  S_COUNT
} StateId;

enum {
  SF_ATTACK = 1u << 0,
  SF_LOW = 1u << 1,
  SF_LAUNCH = 1u << 2,
  SF_UNBLOCKABLE = 1u << 3,
  SF_LOCK = 1u << 4,
  SF_REACTION = 1u << 5,
  SF_WAIT_LAND = 1u << 6,
  SF_HOLD = 1u << 7,
  SF_SCRIPT_CONTACT = 1u << 8 /* sparse events request contact instead of an active interval */
};

/* MoveDef.last value for the clip's final frame. */
#define MOVE_END (-1)

enum {
  CMD_GROUNDED = 1u << 0,
  CMD_CROUCHING = 1u << 1,
  CMD_STANDING = 1u << 2
};

typedef struct MoveSequence MoveSequence;

/* Immutable move and combat rules. A recovered sequence overrides generic
 * first/last playback and ends through its callback. Other moves retain the
 * approximate playback policy until their scripts are recovered. */
typedef struct MoveDef {
  int clip;
  int first, last;
  int startup, active, recovery;
  int damage, stun;
  float reach, push;
  unsigned flags;
  const MoveSequence *sequence;
} MoveDef;

typedef struct Motion {
  float walk, run, jump_velocity, gravity;
} Motion;

typedef struct CommandDef {
  Button button;
  unsigned when;
  int state;
} CommandDef;

typedef struct Battle Battle;
typedef struct Fighter Fighter;
typedef void (*FighterFn)(Battle *battle, Fighter *fighter);

/* A playback segment starts at a move-relative tick. Sample progression is
 * first + step * floor((tick - at) / period), until the next segment.
 * Negative steps reverse; zero steps hold. These are sparse changes in
 * playback policy, not one record per displayed frame. */
typedef struct MoveSegment {
  int at, first, step, period;
} MoveSegment;

/* at is a move-relative simulation frame, not a stored pose sample. Keeping
 * the two clocks distinct allows an event during a hold or reverse segment. */
typedef struct MoveEvent {
  int at;
  FighterFn think;
} MoveEvent;

struct MoveSequence {
  const MoveSegment *segments;
  int segment_count;
  const MoveEvent *events;
  int event_count;
  int count;
  FighterFn end;
};

typedef struct CharacterDef {
  GameId game;
  int asset;
  const char *name;
  const Motion *motion;
  const MoveDef *const *moves; /* move_count pointers, indexed by StateId */
  int move_count;
  const CommandDef *commands;
  int command_count;
  const FighterFn *enter; /* optional move_count callbacks; NULL entries use the default move */
} CharacterDef;

typedef enum Phase {
  PHASE_READY,
  PHASE_FIGHT,
  PHASE_ROUND_OVER,
  PHASE_MATCH_OVER
} Phase;

typedef enum ContactResult {
  CONTACT_MISS = 0,
  CONTACT_BLOCK = 1,
  CONTACT_HIT = 2,
  CONTACT_AIR_HIT = 3
} ContactResult;

struct Fighter {
  int roster;
  int health, wins, facing, ticks;
  int state;
  float x, height, velocity;
  bool connected, crouching;
  const MoveDef *current_move;
  unsigned buffered; /* pressed buttons since the current move phase began */
  int move_loops;
  ContactResult contact;
  int contact_tick;
  bool contact_ready;
  FighterFn after_contact; /* one-shot continuation after simultaneous contact resolution */
};

typedef struct HitEffect {
  float x, height;
  int ticks;
  bool blocked;
} HitEffect;

struct Battle {
  Fighter fighters[2];
  HitEffect effects[2];
  Phase phase;
  int phase_ticks, time_ticks, round, winner;
  uint64_t ticks;
  float wall;
};

void input_bind_player(InputDevice *device, int player);
void input_sample(InputDevice *device, const unsigned char *keys, Input *out);
void input_sync(InputDevice *device, const unsigned char *keys);
void input_cpu(const Battle *battle, int player, Input *out);
ShellCommand input_shell_command(int keycode);

int roster_count(void);
const CharacterDef *roster_def(int index);
const MoveDef *fighter_move(const Fighter *fighter);
int roster_clip(const CharacterDef *def, int state, int asset_idle);
const Character *roster_character(const Assets *assets, int roster_index);
void fighter_start_move(Battle *battle, Fighter *fighter, int state, const MoveDef *move);
void fighter_enter_state(Battle *battle, Fighter *fighter, int state);
void fighter_step_sequence(Battle *battle, Fighter *fighter);
void fighter_note_contact(Fighter *fighter, ContactResult result);
double fighter_frame(const Fighter *fighter, int frame_count);

void battle_init(Battle *battle, int p1, int p2);
void battle_reset_match(Battle *battle);
void battle_next_round(Battle *battle);
void battle_step(Battle *battle, const Input input[2]);
const char *state_name(int state);

void draw_battle(
    Canvas *canvas, const Assets *assets, const Battle *battle, int arena, bool cpu, bool paused);
void draw_sprite(Canvas *canvas, const Geometry *geometry, int sprite, Rect dest, bool flip);
#endif
