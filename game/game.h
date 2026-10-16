#ifndef MK4_GAME_H
#define MK4_GAME_H
/* One header for the match: input frames, the character roster, the two live
 * fighters, and the draw entry points. Asset and renderer headers stay in
 * common/ because the viewer uses them too.
 *
 * A character is a row in the roster, not a subclass. The row points at a
 * motion record, a state table, an optional clip override, and a command
 * table. Spawn writes the roster index onto a fighter. battle_step is the
 * shared think function and only reads those tables.
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
    STATE_IDLE,
    STATE_WALK,
    STATE_CROUCH,
    STATE_JUMP,
    STATE_PUNCH,
    STATE_KICK,
    STATE_SWEEP,
    STATE_UPPERCUT,
    STATE_THROW,
    STATE_BLOCK,
    STATE_HIT,
    STATE_KNOCKDOWN,
    STATE_VICTORY,
    STATE_COUNT
} StateId;

enum {
    STATE_F_ATTACK = 1u << 0,
    STATE_F_LOW = 1u << 1,
    STATE_F_LAUNCH = 1u << 2,
    STATE_F_UNBLOCKABLE = 1u << 3,
    STATE_F_LOCK = 1u << 4,
    STATE_F_REACTION = 1u << 5,
    STATE_F_WAIT_LAND = 1u << 6
};

enum {
    CMD_GROUNDED = 1u << 0,
    CMD_CROUCHING = 1u << 1,
    CMD_STANDING = 1u << 2
};

typedef struct StateDef {
    int clip;
    int startup, active, recovery;
    int damage, stun;
    float reach, push;
    unsigned flags;
} StateDef;

typedef struct Motion {
    float walk, run, jump_velocity, gravity;
} Motion;

typedef struct CommandDef {
    Button button;
    unsigned when;
    int state;
} CommandDef;

typedef struct CharacterDef {
    GameId game;
    int asset;
    const char *name;
    const Motion *motion;
    const StateDef *states;
    int state_count;
    const int *clips;
    const CommandDef *commands;
    int command_count;
} CharacterDef;

typedef enum Phase {
    PHASE_READY,
    PHASE_FIGHT,
    PHASE_ROUND_OVER,
    PHASE_MATCH_OVER
} Phase;

typedef struct Fighter {
    int roster;
    int health, wins, facing, ticks;
    int state;
    float x, height, velocity;
    bool connected, crouching;
} Fighter;

typedef struct HitEffect {
    float x, height;
    int ticks;
    bool blocked;
} HitEffect;

typedef struct Battle {
    Fighter fighters[2];
    HitEffect effects[2];
    Phase phase;
    int phase_ticks, time_ticks, round, winner;
    uint64_t ticks;
    float wall;
} Battle;

void input_bind_player(InputDevice *device, int player);
void input_sample(InputDevice *device, const unsigned char *keys, Input *out);
void input_sync(InputDevice *device, const unsigned char *keys);
void input_cpu(const Battle *battle, int player, Input *out);
ShellCommand input_shell_command(int keycode);

int roster_count(void);
const CharacterDef *roster_def(int index);
const StateDef *fighter_state(const Fighter *fighter);
int roster_clip(const CharacterDef *def, int state, int asset_idle);
const Character *roster_character(const Assets *assets, int roster_index);

void battle_init(Battle *battle, int p1, int p2);
void battle_reset_match(Battle *battle);
void battle_next_round(Battle *battle);
void battle_step(Battle *battle, const Input input[2]);
const char *state_name(int state);

void draw_battle(Canvas *canvas, const Assets *assets, const Battle *battle, int arena, bool cpu, bool paused);
void draw_sprite(Canvas *canvas, const Geometry *geometry, int sprite, Rect dest, bool flip);
#endif
