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

typedef enum RetailAction {
#define RETAIL_ACTION(name, value, confidence, description) name = value,
#include "retail_actions.inc"
#undef RETAIL_ACTION
} RetailAction;

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
typedef void (*FighterFn)(Fighter *fighter);

typedef enum CpuAction {
  CPU_JAB,
  CPU_LOW_KICK,
  CPU_HIGH_KICK,
  CPU_SWEEP,
  CPU_UPPERCUT,
  CPU_THROW,
  CPU_CLOSE_HP,
  CPU_CLOSE_HK,
  CPU_CROUCH_LP,
  CPU_AIR_HP,
  CPU_AIR_HK,
  CPU_AIR_LK,
  CPU_PROJECTILE,
  CPU_SPECIAL_1,
  CPU_SPECIAL_2,
  CPU_DRAW,
  CPU_WEAPON_THROW,
  CPU_FINISH_1,
  CPU_FINISH_2,
  CPU_FINISH_FAN,
  CPU_FINISH_SPIKE,
  CPU_ACTION_COUNT
} CpuAction;

typedef struct CpuMoves {
  FighterFn enter[CPU_ACTION_COUNT];
} CpuMoves;

typedef struct CpuState {
  int plan, timer, seen_action, rest, phase;
  q16 target;
  uint64_t weapon_tick;
  bool finish_chosen;
} CpuState;

typedef struct WeaponDef {
  int type; /* retail descriptor type; travels with a dropped weapon */
  FighterFn hp, lp, back_hp, back_lp, crouch_lp, throw_move;
  void (*equip)(Fighter *fighter);
  int model;
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

typedef bool (*CommandPredicate)(const Fighter *fighter);

/* Pattern tokens of a special command, earliest first. Back and forward are
 * resolved against the opponent's position when the button is pressed. */
typedef enum CommandToken {
  TOKEN_UP = 1,
  TOKEN_DOWN,
  TOKEN_BACK,
  TOKEN_FORWARD
} CommandToken;

enum {
  CMD_RELEASE = 0x100, /* match release instead of press; portable event tag */
  CMD_AIR = 0x10,      /* retail flag 0x10: allowed off the ground */
  CMD_BLOCK_OK = 0x40  /* retail flag 0x40: allowed while block is held */
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

typedef bool (*FighterTest)(const Fighter *fighter);

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
  const CpuMoves *cpu;    /* translated action entries, selected by the shared CPU controller */
} CharacterDef;

enum {
  FINISH_BURN = 1,
  FINISH_BEAST,
  FINISH_FAN,
  FINISH_SPIKE,
  FINISH_SUB_ZERO_1,
  FINISH_SUB_ZERO_2,
  FINISH_LIU_KANG_1,
  FINISH_LIU_KANG_2
};

enum {
  APPEARANCE_NORMAL,
  APPEARANCE_BEAST,
  APPEARANCE_BURNED,
  APPEARANCE_HIDDEN,
  APPEARANCE_FROZEN,
  APPEARANCE_DRAGON
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
  CpuState cpu;
  bool connected, crouching;
  bool body_contact; /* reaction source was the other fighter, rather than an independent object */
  bool avoids_high;  /* low-kick script's temporary high-punch avoidance gate */
  const MoveDef *current_move;
  const MoveSequence *sequence; /* active phase; may replace the move's initial sequence */
  uint64_t continuation;        /* changes on move/phase replacement, even when re-entering the same phase */
  int sequence_event_tick;
  bool sequence_finished;
  unsigned buffered; /* pressed buttons since the current move phase began */
  int move_loops;
  ContactResult contact;
  int contact_tick;
  unsigned held, prev[2]; /* switch samples for the two-sample press rule */
  unsigned pressed;       /* presses detected this tick */
  unsigned released;      /* releases after retail word-group press suppression */
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
  int appearance;       /* character-script rendering variant; reset at round spawn */
  int frozen_clip;      /* preserve the victim's current pose while frozen */
  q16 visual_offset[3]; /* presentation translation; never integrated as combat velocity */
  int reaction;         /* last selected attack-record reaction, retained across its phases */
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
typedef void (*ProjectileFn)(Projectile *projectile);

enum {
  PROJECTILE_SPEAR = 1,
  PROJECTILE_SWORD = 2,
  PROJECTILE_ICE = 3,
  PROJECTILE_CLONE = 4,
  PROJECTILE_FIREBALL = 5,
  PROJECTILE_FATAL_FIREBALL = 6
};

typedef struct OrbitLightDef {
  q16 radius, height, speed, fade;
} OrbitLightDef;

typedef struct OrbitLight {
  const OrbitLightDef *def;
  q16 phase, strength, pos[3];
} OrbitLight;

typedef struct MeshAnimation {
  int first, count, period;
  q16 scale;
} MeshAnimation;

struct Projectile {
  int kind, owner, facing, ticks, lifetime, phase;
  const WeaponDef *weapon;
  int clip, sample; /* stationary clone's captured pose */
  q16 pos[3], vel[3];
  q16 heading[2];
  q16 angle[3], root_y; /* captured clone orientation/root offset */
  ProjectileFn step;
  OrbitLight light;
  q16 spin[3];
  int bounce;
  const MeshAnimation *mesh_animation;
  int initial_mesh; /* Spawn rig before the first mesh animation tick. */
};

#define MAX_PROJECTILES 4

typedef struct ScenePiece {
  bool active, frozen, animated;
  int roster, bone, attached_to, joint;
  q16 pos[3], vel[3], angle[3], spin[3];
  Mat3 orientation;
} ScenePiece;

typedef struct AttachmentDef {
  int model, clip, first, last, loop_from;
  Vec3 group_offset, piece_offset;
  int piece_kind;
  bool scene_space;
} AttachmentDef;

typedef struct SceneAttachment {
  const AttachmentDef *def;
  int piece, sample;
  Vec3 origin;
  Mat3 orientation;
} SceneAttachment;

typedef struct ParticleVisual {
  MeshAnimation animation;
  bool billboard, additive;
  q16 shrink;
} ParticleVisual;

typedef struct ParticleSpray {
  const ParticleVisual *visual;
  int count, lifetime;
  q16 gravity;
  q16 velocity[3], spread[3], offset[3], position_spread[3];
  bool splash;
} ParticleSpray;

typedef struct EmissionPhase {
  int duration, period, extra, count; /* emit at period residue zero and optionally extra (-1 disables it) */
} EmissionPhase;

typedef struct JointSprayDef {
  const ParticleSpray *spray;
  const EmissionPhase *phases;
  int count, origin_joint, base_joint; /* spray from origin away from base */
  q16 speed, variation;
} JointSprayDef;

typedef struct JointSpray {
  const JointSprayDef *def;
  int fighter, ticks;
} JointSpray;

typedef struct Particle {
  q16 pos[3], vel[3], gravity;
  int ticks;
  const ParticleVisual *visual;
  int age;
  q16 scale;
  bool splash;
} Particle;

typedef struct CameraShot {
  const q16 (*points)[3];
  int count, hold;
  q16 step;
  int aim_joint, aim_ticks;
  q16 aim_y;
} CameraShot;

typedef struct SceneCamera {
  bool active;
  q16 focal_scale; /* zero keeps the standard focal length */
  Vec3 eye, target, from_target, to_target;
  int ticks, duration;
  const CameraShot *shots;
  int shot_count, shot, hold, target_player;
  q16 parameter;
  Vec3 origin, axis, path_start;
  q16 shake_y, shake_amount;
  int shake_pulses, shake_interval, shake_ticks;
} SceneCamera;

#define MAX_SCENE_PIECES 32
#define MAX_SCENE_ATTACHMENTS 8
#define MAX_PARTICLES 128
#define MAX_JOINT_SPRAYS 4

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
  SceneAttachment attachments[MAX_SCENE_ATTACHMENTS];
  Particle particles[MAX_PARTICLES];
  JointSpray sprays[MAX_JOINT_SPRAYS];
  SceneCamera camera;
  SoundEvent sounds[MAX_SOUND_EVENTS];
  int sound_count;
  Phase phase;
  int phase_ticks, time_ticks, round, winner;
  bool finish_waiting; /* final KO recovery must reach the dizzy controller before entry */
  uint32_t random[2];
  int cpu_level, cpu_rank, cpu_progress; /* 534780, 5345b0, 53c370 */
  bool cpu_adaptive;
  int arena, finisher; /* stage id and completed/active character finisher id */
  uint64_t ticks;
  q16 arena_radius;
  q16 axis[2];       /* 0x530238: unit X/Z from P1 toward P2, previous world pump */
  q16 distance;      /* 0x53019c */
  int ko_winner;     /* 0x532354: 1 or 2 after a KO */
  bool contacts_off; /* 0x527ae4 */
  bool contacted;    /* 0x532358: any contact this round */
};

/* One active simulation, defined in battle.c and initialized by battle_init.
 * Assets remain borrowed from the host; rendering only reads this state. */
extern Battle g_battle;

void input_bind_player(InputDevice *device, int player);
void input_sample(InputDevice *device, const unsigned char *keys, Input *out);
void input_sync(InputDevice *device, const unsigned char *keys);
void input_cpu(int player, Input *out);
uint32_t battle_random(uint32_t range);
bool battle_chance(int per_thousand);
void battle_cpu_step(Fighter *fighter);
bool battle_cpu_action(Fighter *fighter, CpuAction action);
ShellCommand input_shell_command(int keycode);

int roster_count(void);
const CharacterDef *roster_def(int index);
const MoveDef *fighter_move(const Fighter *fighter);
int roster_clip(const CharacterDef *def, int state, int asset_idle);
const Character *roster_character(const Assets *assets, int roster_index);
void fighter_start_move(Fighter *fighter, int state, const MoveDef *move);
void fighter_enter_state(Fighter *fighter, int state);
void fighter_step_sequence(Fighter *fighter);
void fighter_start_sequence(Fighter *fighter, const MoveSequence *sequence);
/* Stackful move prototype: one scheduler-owned fiber per active fighter. */
void fighter_start_fiber(Fighter *fighter, FighterFn entry, FighterFn cleanup);
bool fighter_fiber_active(const Fighter *fighter);
bool fighter_fiber_running(const Fighter *fighter);
void fighter_fiber_pump_begin(Fighter *fighter);
void fighter_fiber_move(Fighter *fighter, int state, const MoveDef *move);
void fighter_fiber_sequence(Fighter *fighter, const MoveSequence *sequence);
void fighter_fiber_play_to(Fighter *fighter, int sample, int step);
void fighter_step_fiber(Fighter *fighter);
void fighter_cancel_fiber(Fighter *fighter);
void fighter_cancel_all_fibers(void);
void fighter_wait_ticks(int ticks);
_Noreturn void fighter_fiber_finish(FighterFn next);
void fighter_note_contact(Fighter *fighter, ContactResult result);
float fighter_x(const Fighter *fighter);
float fighter_z(const Fighter *fighter);
float fighter_height(const Fighter *fighter);
bool fighter_airborne(const Fighter *fighter);
void fighter_neutral(Fighter *fighter);
void fighter_upright(Fighter *fighter);
q16 fighter_axis(const Fighter *fighter, int axis);
void fighter_set_speed(Fighter *fighter, q16 speed);
void fighter_friction(Fighter *fighter);
ContactResult battle_contact(Fighter *attacker, const AttackRecord *record);
ContactResult battle_contact_from(Fighter *attacker, const AttackRecord *record, const q16 origin[3], int facing);
ContactResult battle_projectile_contact(Projectile *projectile, const AttackRecord *record);
Projectile *battle_spawn_projectile(Fighter *owner, int kind, int lifetime, ProjectileFn step);
void battle_damage(Fighter *victim, q16 damage);
void battle_react(Fighter *victim, int reaction);
void battle_wall_recoil(Fighter *victim);
void battle_world(void);
void battle_set_arena(int arena);
bool battle_can_finish(const Fighter *fighter);
bool battle_begin_finisher(Fighter *fighter, int id);
void battle_end_finisher(void);
void fighter_interrupt(Fighter *fighter, FighterFn entry);
void fighter_enter_stance(Fighter *fighter, Stance stance);
const Fighter *fighter_opponent(const Fighter *fighter);
float fighter_distance(const Fighter *fighter);
bool fighter_holds_back(const Fighter *fighter);
bool fighter_fast_start(const Fighter *fighter);
void fighter_press(Fighter *fighter, Button button);
void fighter_release(Fighter *fighter, Button button);
double fighter_frame(const Fighter *fighter, int frame_count);
int fighter_sample(const Fighter *fighter);
void battle_bind_assets(const Assets *assets);
const Character *fighter_model(const Assets *assets, const Fighter *fighter);
const Clip *fighter_clip(const Assets *assets, const Fighter *fighter);
Mat3 fighter_turn(const Assets *assets, const Fighter *fighter);
bool fighter_joint_bounds(const Fighter *fighter, const q16 origin[3], const q16 axis[2], q16 bounds[4]);
int fighter_pose_sample(const Fighter *fighter);
void fighter_pose(const Assets *assets, const Fighter *fighter, Pose output[MK4_MAX_BONES]);
void fighter_ready(Fighter *fighter);
void fighter_equip_weapon(Fighter *fighter, const WeaponDef *weapon);
void weapon_start_strike(Fighter *fighter, const WeaponStrike *strike, bool travel);
void weapon_air_attack(Fighter *fighter, bool high);
void weapon_draw(Fighter *fighter, const WeaponDef *weapon);
void weapon_throw(Fighter *fighter);
bool weapon_can_pickup(const Fighter *fighter);
void weapon_pickup(Fighter *fighter);
void weapon_followup_victim(Fighter *fighter);
void weapon_stance(Fighter *fighter, int state);
void scene_begin(Fighter *fighter, const SceneClip *clips, int count, int loop_from);
bool scene_step(Fighter *fighter);
bool fighter_joint_position(const Fighter *fighter, int joint, Vec3 *position, Mat3 *orientation);
void battle_camera_sequence(const Fighter *owner, const Fighter *target, const CameraShot *shots, int count);
void battle_camera_shake(int pulses, int interval, q16 amount);
void fighter_visual_offset(Fighter *fighter, q16 along, q16 y);
void battle_object_light(Projectile *object, const OrbitLightDef *def);
int projectile_mesh_frame(const Projectile *object);
void battle_particles(const Fighter *fighter, int joint, int count, bool ice);
void battle_spray(const ParticleSpray *spray, Vec3 origin, Vec3 direction, q16 speed, q16 variation);
void battle_joint_spray(const Fighter *fighter, const JointSprayDef *def);
ScenePiece *battle_attach_head(Fighter *victim, Fighter *owner, int joint);
void battle_attach_piece_model(ScenePiece *piece, const AttachmentDef *def, const Fighter *actor);
ScenePiece *battle_detach_piece(Fighter *victim, int joint, const q16 velocity[3], const q16 spin[3]);
void battle_shatter(Fighter *victim);
void battle_step_presentation(void);
void battle_sound(int bank, int selector);
void battle_scene_impact(const Fighter *fighter);

void battle_init(int p1, int p2);
void battle_reset_match(void);
void battle_next_round(void);
void battle_step(const Input input[2]);
const char *state_name(int state);

void draw_battle(Canvas *canvas, const Assets *assets, int arena, bool cpu, bool paused);
void draw_sprite(Canvas *canvas, const Geometry *geometry, int sprite, Rect dest, bool flip);
#endif
