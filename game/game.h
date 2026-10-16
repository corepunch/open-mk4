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

/* Retail world values are signed Q16: 1.0 is 0x10000, one mesh unit. */
typedef int32_t q16;
#define Q16_ONE 0x10000

/* Signed product with retail rounding: (a * b) >> 16, toward minus infinity. */
static inline q16 q16_mul(q16 a, q16 b) {
  int64_t product = (int64_t)a * b;
  return (q16)(product >= 0 ? product / Q16_ONE : -((-product + Q16_ONE - 1) / Q16_ONE));
}

static inline float q16_float(q16 value) {
  return (float)value / (float)Q16_ONE;
}

typedef enum GameId {
  GAME_MK4 = 0,
  GAME_WARGODS = 1
} GameId;

/* Retail switches in the order the switch scan queues their press events:
 * word 0x4d00a4 holds directions, HP, BLK and HK; word 0x4d00a8 holds LP,
 * LK and RUN. Directions are absolute, not facing relative. See
 * docs/scorpion/input-and-commands.md. */
typedef enum Button {
  BUTTON_UP,
  BUTTON_DOWN,
  BUTTON_LEFT,
  BUTTON_RIGHT,
  BUTTON_HIGH_PUNCH,
  BUTTON_BLOCK,
  BUTTON_HIGH_KICK,
  BUTTON_LOW_PUNCH,
  BUTTON_LOW_KICK,
  BUTTON_RUN,
  BUTTON_COUNT
} Button;

#define BUTTON_BIT(button) (1u << (button))

/* held is the switches down during this tick. The simulation derives presses
 * with the retail two-sample rule: a switch must be up in both previous
 * samples, so a one-tick release between taps does not repeat a press. */
typedef struct Input {
  unsigned held;
} Input;

static inline bool input_held(Input in, Button button) {
  return (in.held & BUTTON_BIT(button)) != 0;
}

/* code[] holds keyboard scancodes. The numbers come from SDL; this header
 * does not include SDL. */
typedef struct InputDevice {
  int code[BUTTON_COUNT];
  unsigned suppressed; /* keys held across a shell action, ignored until released */
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
  SF_RECORD = 1u << 8 /* move events query retail attack records; the approximate resolver skips it */
};

/* MoveDef.last value for the clip's final frame. */
#define MOVE_END (-1)

typedef struct MoveSequence MoveSequence;

/* Immutable move and combat rules. A recovered sequence overrides generic
 * first/last playback and ends through its callback. Other moves retain the
 * approximate playback policy until their scripts are recovered. */
typedef struct MoveDef {
  int clip;
  int first, last;
  int startup, active, recovery;
  int damage, stun; /* approximate moves: damage in percent of full health */
  float reach, push;
  unsigned flags;
  const MoveSequence *sequence;
  int action; /* retail action code at controller +0x74; 0 uses the state default */
} MoveDef;

/* Per-tick Q16 movement values. */
typedef struct Motion {
  q16 walk, run, jump_velocity, gravity;
} Motion;

/* A retail attack record (6 words). The box is relative to the attacker's
 * origin along the attacker-to-victim axis: X spans [reach - length, reach]
 * when facing the victim, Y spans [top, top + height] (Y down). */
enum {
  ATTACK_LOW = 1,         /* blocked only with down held */
  ATTACK_UNBLOCKABLE = 4, /* our explicit flag for unblockable script contacts */
  ATTACK_MARK = 2,        /* sets the victim's FF_MARK */
};

typedef struct AttackRecord {
  q16 reach, top, length, height;
  uint8_t hit, air, blocked, flags; /* reaction ids for hit / air hit / block */
  q16 damage;                       /* fraction of full health */
} AttackRecord;
typedef struct Battle Battle;
typedef struct Fighter Fighter;
typedef void (*FighterFn)(Battle *battle, Fighter *fighter);

typedef struct WeaponDef {
  int type; /* retail descriptor type; travels with a dropped weapon */
  FighterFn hp, lp, back_hp, back_lp, crouch_lp, throw_move;
  void (*equip)(Fighter *fighter);
} WeaponDef;

/* 494a00's descriptor-driven attack phases, translated into game data. */
typedef struct WeaponStrike {
  MoveDef move;
  AttackRecord record;
  int first, windup, target, queries, recovery;
  int delay, travel;
  q16 speed;
  int sound;
} WeaponStrike;

typedef struct SceneClip {
  MoveDef move;
  int first, last, repeats;
  q16 blend_step, blend_first;
} SceneClip;

typedef struct ScenePlayback {
  const SceneClip *clips;
  int count, index, repeat, loop_from;
  bool done;
} ScenePlayback;

typedef bool (*CommandPredicate)(const Battle *battle, const Fighter *fighter);

/* Pattern tokens of a special command, earliest first. Back and forward are
 * resolved against the opponent's position when the button is pressed. */
typedef enum CommandToken {
  TOKEN_UP = 1,
  TOKEN_DOWN,
  TOKEN_BACK,
  TOKEN_FORWARD
} CommandToken;

enum {
  CMD_AIR = 0x10,     /* retail flag 0x10: allowed off the ground */
  CMD_BLOCK_OK = 0x40 /* retail flag 0x40: allowed while block is held */
};

#define COMMAND_TOKENS 4

/* One retail command record (gate<<24 | flags<<16 | window<<8 | action).
 * count 0 is an unconditional record. window is inclusive ticks from the
 * first direction press to the button press; 0 disables the check. The
 * action-table bits of the retail flags are replaced by the enter callback. */
typedef struct CommandDef {
  Button button;
  CommandToken tokens[COMMAND_TOKENS];
  int count;
  int window;
  unsigned flags;
  CommandPredicate predicate;
  FighterFn enter;
} CommandDef;

/* Press handlers for one stance, indexed by Button. Direction entries are
 * unused; NULL ignores the press. A move with SF_LOCK installs none. */
typedef struct ButtonTable {
  FighterFn press[BUTTON_COUNT];
} ButtonTable;

typedef enum Stance {
  STANCE_STANDING,
  STANCE_CROUCHING,
  STANCE_JUMP,
  STANCE_COUNT
} Stance;

typedef struct Controls {
  const CommandDef *commands;
  int command_count;
  const ButtonTable *stances[STANCE_COUNT];
} Controls;

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

typedef bool (*FighterTest)(const Battle *battle, const Fighter *fighter);

/* until, when set, holds the last sample after count ticks until it holds;
 * a retail script wait such as "until landed". */
struct MoveSequence {
  const MoveSegment *segments;
  int segment_count;
  const MoveEvent *events;
  int event_count;
  int count;
  FighterFn end;
  FighterTest until;
};

typedef struct CharacterDef {
  GameId game;
  int asset;
  const char *name;
  const Motion *motion;
  const MoveDef *const *moves; /* move_count pointers, indexed by StateId */
  int move_count;
  const Controls *controls;
  const FighterFn *enter; /* optional move_count callbacks; NULL entries use the default move */
  q16 ground;             /* retail standing group Y: the rig root's Y translation (soles at 0) */
  FighterFn walk;         /* game-owned ground locomotion; reads held direction/run */
  FighterFn jump;         /* grounded jump entry; owns takeoff, flight and landing */
} CharacterDef;

enum {
  FINISH_BURN = 1,
  FINISH_BEAST,
  FINISH_FAN,
  FINISH_SPIKE,
  FINISH_SUB_ZERO_1,
  FINISH_SUB_ZERO_2
};

enum {
  APPEARANCE_NORMAL,
  APPEARANCE_BEAST,
  APPEARANCE_BURNED,
  APPEARANCE_HIDDEN,
  APPEARANCE_FROZEN
};

typedef enum Phase {
  PHASE_READY,
  PHASE_FIGHT,
  PHASE_FINISH,
  PHASE_FATALITY,
  PHASE_ROUND_OVER,
  PHASE_MATCH_OVER
} Phase;

typedef enum ContactResult {
  CONTACT_MISS = 0,
  CONTACT_BLOCK = 1,
  CONTACT_HIT = 2,
  CONTACT_AIR_HIT = 3
} ContactResult;

#define INPUT_HISTORY 8

/* Newest first, as retail 0x524ed8 (tokens) and 0x534660 (tick stamps). */
typedef struct InputHistory {
  uint8_t token[INPUT_HISTORY]; /* direction Button + 1; 0 is empty */
  uint64_t tick[INPUT_HISTORY];
} InputHistory;

/* Retail group +0x40 bits. */
enum {
  FF_BUSY = 0x001,         /* special commands rejected; a human cannot block */
  FF_WEAPON_LOST = 0x002,  /* draw-weapon predicate fails (HYPOTHESIS) */
  FF_REACTING = 0x004,     /* in a hit reaction; commands rejected */
  FF_NO_GUARD = 0x008,     /* contacts skip the block test; back is not held */
  FF_MARK = 0x010,         /* copied from the last attack record's MARK bit */
  FF_UNTOUCHABLE = 0x020,  /* contacts are ignored */
  FF_TELEPORTING = 0x040,  /* camera ignores this fighter */
  FF_FOLLOW_UP = 0x200,    /* commands allowed off the ground */
  FF_PAIRED = 0x400,       /* bodies attached by a paired move; skip separation */
  FF_NEUTRAL_CLEAR = 0x6e8 /* bits cleared by the return to neutral 0x48e080 */
};

/* Retail group motion: Q16 position (+0x54, Y down), velocity (+0x6c),
 * per-tick gravity (+0x4c) and ground (+0x48). physics is the +0x44
 * callback run by the world pump; on_land is the controller's one-shot
 * landing continuation. */
typedef struct Body {
  q16 pos[3], vel[3];
  bool walking;          /* ground controller motion resolves before body separation */
  q16 angle[3], spin[3]; /* radians Q16, integrated by the world pump */
  q16 root_y;            /* explicit root offset, paired with group Y by 4702e0 */
  q16 gravity, ground;
  q16 last_inside[3]; /* last position inside the arena circle */
  FighterFn physics;
  FighterFn on_land;
} Body;

enum {
  AXIS_X,
  AXIS_Y,
  AXIS_Z
};

struct Fighter {
  const Assets *assets; /* borrowed decoded clips/rigs; owned by the host */
  q16 heading[2];       /* model's forward X/Z, independent of the camera */
  int roster;
  int wins, facing, ticks;
  int state;
  Body body;
  q16 health;       /* 0x10000 is full */
  int hits;         /* controller +0x7c: hits taken in the current combo */
  q16 damage_total; /* controller +0x80 */
  q16 damage_scale; /* 0x5347a4 / 0x534834 */
  q16 run_energy;
  int run_delay;
  int action; /* controller +0x74 */
  bool human; /* 0x5347c8 / 0x532248: block from switches, not action codes */
  bool connected, crouching;
  bool avoids_high; /* low-kick script's temporary high-punch avoidance gate */
  const MoveDef *current_move;
  unsigned buffered; /* pressed buttons since the current move phase began */
  int move_loops;
  ContactResult contact;
  int contact_tick;
  unsigned held, prev[2]; /* switch samples for the two-sample press rule */
  unsigned pressed;       /* presses detected this tick */
  InputHistory directions;
  uint64_t latch[BUTTON_COUNT]; /* tick of the last press; HK is never latched */
  const ButtonTable *buttons;   /* current stance table; NULL while locked */
  unsigned flags;
  FighterFn pending;   /* interrupt entry, started on the next tick */
  FighterFn script;    /* native C move continuation; cleared on interruption */
  int sample_override; /* -1 uses the move playback, otherwise an explicit script cursor */
  int script_phase, script_timer;
  const WeaponStrike *weapon_strike;
  bool weapon_travel;
  int blend_clip, blend_sample, blend_ticks, blend_duration;
  q16 blend_weight;
  ScenePlayback scene;
  uint64_t hidden_joints[2];
  int appearance;  /* character-script rendering variant; reset at round spawn */
  int frozen_clip; /* preserve the victim's current pose while frozen */
  int reaction;    /* last selected attack-record reaction, retained across its phases */
  uint64_t spear_ready, teleport_ready;
  uint64_t clone_ready;
  uint64_t slide_ready, sidestep_ready;
  bool armed, weapon_used;
  const WeaponDef *weapon;
  FighterFn disarm; /* character-owned weapon drop entry */
};

typedef struct HitEffect {
  float x, height;
  int ticks;
  bool blocked;
} HitEffect;

typedef struct Projectile Projectile;
typedef void (*ProjectileFn)(Battle *battle, Projectile *projectile);

enum {
  PROJECTILE_SPEAR = 1,
  PROJECTILE_SWORD = 2,
  PROJECTILE_ICE = 3,
  PROJECTILE_CLONE = 4
};

struct Projectile {
  int kind, owner, facing, ticks, lifetime, phase;
  const WeaponDef *weapon;
  int clip, sample; /* stationary clone's captured pose */
  q16 pos[3], vel[3];
  q16 heading[2];
  q16 angle[3], root_y; /* captured clone orientation/root offset */
  ProjectileFn step;
};

#define MAX_PROJECTILES 4

typedef struct ScenePiece {
  bool active, frozen;
  int roster, bone, attached_to, joint;
  q16 pos[3], vel[3], angle[3], spin[3];
  Mat3 orientation;
} ScenePiece;

typedef struct Particle {
  q16 pos[3], vel[3], gravity;
  int ticks;
  uint32_t color;
} Particle;

typedef struct SceneCamera {
  bool active;
  Vec3 eye, target, from_eye, from_target, to_eye, to_target;
  int ticks, duration;
} SceneCamera;

#define MAX_SCENE_PIECES 32
#define MAX_PARTICLES 128

typedef struct SoundEvent {
  int bank, selector;
} SoundEvent;

#define MAX_SOUND_EVENTS 16

struct Battle {
  const Assets *assets;
  Fighter fighters[2];
  HitEffect effects[2];
  Projectile projectiles[MAX_PROJECTILES];
  ScenePiece pieces[MAX_SCENE_PIECES];
  Particle particles[MAX_PARTICLES];
  SceneCamera camera;
  SoundEvent sounds[MAX_SOUND_EVENTS];
  int sound_count;
  Phase phase;
  int phase_ticks, time_ticks, round, winner;
  int arena, finisher; /* stage id and completed/active character finisher id */
  uint64_t ticks;
  q16 arena_radius;
  q16 axis[2];       /* 0x530238: unit X/Z from P1 toward P2, previous world pump */
  q16 distance;      /* 0x53019c */
  int ko_winner;     /* 0x532354: 1 or 2 after a KO */
  bool contacts_off; /* 0x527ae4 */
  bool contacted;    /* 0x532358: any contact this round */
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
float fighter_x(const Fighter *fighter);
float fighter_z(const Fighter *fighter);
float fighter_height(const Fighter *fighter);
bool fighter_airborne(const Fighter *fighter);
void fighter_neutral(Fighter *fighter);
void fighter_upright(Fighter *fighter);
q16 fighter_axis(const Battle *battle, const Fighter *fighter, int axis);
void fighter_set_speed(const Battle *battle, Fighter *fighter, q16 speed);
ContactResult battle_contact(Battle *battle, Fighter *attacker, const AttackRecord *record);
ContactResult battle_contact_from(Battle *battle, Fighter *attacker, const AttackRecord *record, const q16 origin[3], int facing);
ContactResult battle_projectile_contact(Battle *battle, Projectile *projectile, const AttackRecord *record);
Projectile *battle_spawn_projectile(Battle *battle, Fighter *owner, int kind, int lifetime, ProjectileFn step);
void battle_damage(Battle *battle, Fighter *victim, q16 damage);
void battle_react(Battle *battle, Fighter *victim, int reaction);
void battle_world(Battle *battle);
bool battle_can_finish(const Battle *battle, const Fighter *fighter);
bool battle_begin_finisher(Battle *battle, Fighter *fighter, int id);
void battle_end_finisher(Battle *battle);
void fighter_interrupt(Fighter *fighter, FighterFn entry);
void fighter_enter_stance(Fighter *fighter, Stance stance);
const Fighter *fighter_opponent(const Battle *battle, const Fighter *fighter);
float fighter_distance(const Battle *battle, const Fighter *fighter);
bool fighter_holds_back(const Battle *battle, const Fighter *fighter);
bool fighter_fast_start(const Battle *battle, const Fighter *fighter);
void fighter_press(Battle *battle, Fighter *fighter, Button button);
double fighter_frame(const Fighter *fighter, int frame_count);
int fighter_sample(const Fighter *fighter);
void battle_bind_assets(Battle *battle, const Assets *assets);
const Character *fighter_model(const Assets *assets, const Fighter *fighter);
const Clip *fighter_clip(const Assets *assets, const Fighter *fighter);
Mat3 fighter_turn(const Assets *assets, const Fighter *fighter);
bool fighter_joint_bounds(const Fighter *fighter, const q16 origin[3], const q16 axis[2], q16 bounds[4]);
int fighter_pose_sample(const Fighter *fighter);
void fighter_pose(const Assets *assets, const Fighter *fighter, Pose output[MK4_MAX_BONES]);
void fighter_ready(Battle *battle, Fighter *fighter);
void fighter_equip_weapon(Fighter *fighter, const WeaponDef *weapon);
void weapon_start_strike(Battle *battle, Fighter *fighter, const WeaponStrike *strike, bool travel);
void weapon_air_attack(Battle *battle, Fighter *fighter, bool high);
void weapon_draw(Battle *battle, Fighter *fighter, const WeaponDef *weapon);
void weapon_throw(Battle *battle, Fighter *fighter);
bool weapon_can_pickup(const Battle *battle, const Fighter *fighter);
void weapon_pickup(Battle *battle, Fighter *fighter);
void weapon_followup_victim(Battle *battle, Fighter *fighter);
void weapon_stance(Battle *battle, Fighter *fighter, int state);
void scene_begin(Fighter *fighter, const SceneClip *clips, int count, int loop_from);
bool scene_step(Fighter *fighter);
bool fighter_joint_position(const Fighter *fighter, int joint, Vec3 *position, Mat3 *orientation);
void battle_scene_camera(Battle *battle, const Fighter *owner, const Fighter *target, q16 along, q16 y, q16 across, int ticks);
void battle_particles(Battle *battle, const Fighter *fighter, int joint, uint32_t color, int count, bool ice);
void battle_attach_head(Battle *battle, Fighter *victim, Fighter *owner, int joint);
void battle_shatter(Battle *battle, Fighter *victim);
void battle_step_presentation(Battle *battle);
void battle_sound(Battle *battle, int bank, int selector);

void battle_init(Battle *battle, int p1, int p2);
void battle_reset_match(Battle *battle);
void battle_next_round(Battle *battle);
void battle_step(Battle *battle, const Input input[2]);
const char *state_name(int state);

void draw_battle(Canvas *canvas, const Assets *assets, const Battle *battle, int arena, bool cpu, bool paused);
void draw_sprite(Canvas *canvas, const Geometry *geometry, int sprite, Rect dest, bool flip);
#endif
