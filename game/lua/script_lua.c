#include "lua/script_lua.h"
#include "script/vm.h"
#include "script/native_ids.h"
#include "script/bindings.h"
#include "data/layout.h"
#include "native/helpers.h"
#include "native/actions.h"
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include <stdio.h>
#include <string.h>

/* One host-owned Lua state. Registry tables:
 *   scripts: linked entry address -> Lua function
 *   names:   linked entry address -> label (diagnostics)
 *   threads: Fighter lightuserdata -> the actor's live coroutine
 * The main stack also anchors every coroutine while it runs, so a nested
 * script start from a native cannot let the collector reclaim the resumer. */
static lua_State *L;
static bool coverage;
static unsigned entries_started;
static Fighter *current;   /* actor whose coroutine is running */
static lua_State *running; /* that coroutine */
static int budget;
static const char scripts_key = 0, names_key = 0, threads_key = 0;

static const char *const roster_names[MK4_ROSTER_COUNT] = {"scorpion", "raiden", "sonya", "liu_kang", "sub_zero", "fujin", "shinnok", "reiko", "quan_chi",
    "tanya", "reptile", "kai", "jarek", "jax", "cage", "goro", "kitana", "noob_saibot"};

/* Entry starts by address, for coverage reports: every start passes through
 * the hook, whether or not a Lua function claims it. */
#define ENTRY_SLOTS 4096

typedef struct EntryCount {
  uint32_t address;
  unsigned count;
} EntryCount;

static EntryCount entry_counts[ENTRY_SLOTS];

static void count_entry(uint32_t address) {
  unsigned slot = (address / 4) % ENTRY_SLOTS;
  for (unsigned probe = 0; probe < ENTRY_SLOTS; ++probe) {
    EntryCount *entry = &entry_counts[(slot + probe) % ENTRY_SLOTS];
    if (entry->address == address || entry->count == 0) {
      entry->address = address;
      entry->count++;
      return;
    }
  }
  fatal("Lua entry table full");
}

static lua_State *thread_of(const Fighter *fighter) {
  lua_rawgetp(L, LUA_REGISTRYINDEX, &threads_key);
  lua_rawgetp(L, -1, fighter);
  lua_State *co = lua_tothread(L, -1);
  lua_pop(L, 2);
  return co;
}

static const char *script_name(uint32_t address) {
  lua_rawgetp(L, LUA_REGISTRYINDEX, &names_key);
  lua_rawgeti(L, -1, (lua_Integer)address);
  const char *name = lua_tostring(L, -1);
  lua_pop(L, 2);
  return name != NULL ? name : "?";
}

static bool has_script(uint32_t address) {
  lua_rawgetp(L, LUA_REGISTRYINDEX, &scripts_key);
  lua_rawgeti(L, -1, (lua_Integer)address);
  bool found = lua_isfunction(L, -1);
  lua_pop(L, 2);
  return found;
}

static void runaway(lua_State *co, lua_Debug *ar) {
  (void)ar;
  luaL_error(co, "script did not yield within the Lua instruction budget");
}

/* The shared step calls this after the engine-owned playback prologue. */
void script_lua_step(Fighter *fighter) {
  lua_State *co = thread_of(fighter);
  require(co != NULL, "Lua script resume without a coroutine");
  lua_pushthread(co);
  lua_xmove(co, L, 1); /* anchor on the main stack while it runs */
  Fighter *saved_fighter = current;
  lua_State *saved_thread = running;
  current = fighter;
  running = co;
  budget = 0;
  int results = 0;
  int status = lua_resume(co, saved_thread, 0, &results);
  current = saved_fighter;
  running = saved_thread;
  lua_pop(L, 1);
  if (status == LUA_YIELD) {
    lua_pop(co, results);
    return;
  }
  if (status != LUA_OK) {
    fatal("Lua script %s: %s", script_name(fighter->execution.script.entry), lua_tostring(co, -1));
  }
  ScriptVm *vm = &fighter->execution.script;
  if (vm->active && thread_of(fighter) == co) {
    fatal("Lua script %s ended without ready(), finish() or a native tail", script_name(vm->entry));
  }
}

void script_lua_start(Fighter *fighter, uint32_t entry) {
  require(L != NULL, "Lua runtime is not open: the host opens it before a match starts");
  count_entry(entry);
  lua_rawgetp(L, LUA_REGISTRYINDEX, &scripts_key);
  lua_rawgeti(L, -1, (lua_Integer)entry);
  if (!lua_isfunction(L, -1)) {
    fatal("No Lua script owns linked entry %08x (%s)", entry, script_name(entry));
  }
  lua_State *co = lua_newthread(L);
  lua_pushvalue(L, -2);
  lua_xmove(L, co, 1);
  lua_sethook(co, runaway, LUA_MASKCOUNT, 1000000);
  lua_rawgetp(L, LUA_REGISTRYINDEX, &threads_key);
  lua_pushvalue(L, -2);
  lua_rawsetp(L, -2, fighter);
  lua_pop(L, 4);
  entries_started++;
}

unsigned script_lua_entries(void) {
  return entries_started;
}

unsigned script_lua_memory_kb(void) {
  lua_gc(L, LUA_GCCOLLECT);
  return (unsigned)lua_gc(L, LUA_GCCOUNT);
}

void script_lua_set_coverage(bool value) {
  require(L == NULL, "Coverage instrumentation is chosen before the runtime opens");
  coverage = value;
}

void script_lua_visit_functions(void (*visit)(const char *name, unsigned count)) {
  lua_getglobal(L, "mk4");
  lua_getfield(L, -1, "coverage");
  if (lua_istable(L, -1)) {
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
      visit(lua_tostring(L, -2), (unsigned)lua_tointeger(L, -1));
      lua_pop(L, 1);
    }
  }
  lua_pop(L, 2);
}

const char *script_lua_entry_name(uint32_t address) {
  return L != NULL ? script_name(address) : "?";
}

void script_lua_visit_entries(void (*visit)(uint32_t address, const char *name, unsigned count, bool owned)) {
  for (unsigned slot = 0; slot < ENTRY_SLOTS; ++slot) {
    if (entry_counts[slot].count != 0) {
      uint32_t address = entry_counts[slot].address;
      visit(address, script_name(address), entry_counts[slot].count, has_script(address));
    }
  }
}

/* ---- script API: the same actor-owned semantics as the word handlers ---- */

static Fighter *actor(lua_State *co) {
  if (co != running || current == NULL) {
    luaL_error(co, "mk4 operations run only inside an actor script");
  }
  if (++budget > 1024) {
    luaL_error(co, "script did not yield within 1024 operations");
  }
  return current;
}

/* After each operation: suspend when the actor moved on (the coroutine is
 * then abandoned) or when playback, a wait, a park or a pending interrupt
 * owns the next ticks. Otherwise return `results` values to the script. */
static int settle_results(lua_State *co, Fighter *fighter, uint64_t generation, int results) {
  ScriptVm *vm = &fighter->execution.script;
  if (!vm->active || fighter->execution.generation != generation || thread_of(fighter) != co) {
    return lua_yield(co, 0);
  }
  if (vm->parked || vm->wait_ground || fighter->pending != NULL || vm->play_ticks > 0 || vm->blend_ticks > 0 || vm->interpolated_play ||
      g_battle.ticks < vm->wake) {
    return lua_yield(co, 0);
  }
  return results;
}

static int settle(lua_State *co, Fighter *fighter, uint64_t generation) {
  return settle_results(co, fighter, generation, 0);
}

static int small(lua_State *co, int index) {
  lua_Integer value = luaL_checkinteger(co, index);
  luaL_argcheck(co, value >= 0 && value <= 255, index, "expected an 8-bit operand");
  return (int)value;
}

static int field24(lua_State *co, int index) {
  lua_Integer value = luaL_checkinteger(co, index);
  luaL_argcheck(co, value >= 0 && value <= 0xffffff, index, "expected a 24-bit operand");
  return (int)value;
}

static int field12(lua_State *co, int index) {
  lua_Integer value = luaL_checkinteger(co, index);
  luaL_argcheck(co, value >= 0 && value <= 0xfff, index, "expected a 12-bit operand");
  return (int)value;
}

/* A 32-bit operand word; negative Lua integers encode negative Q16 values. */
static uint32_t word(lua_State *co, int index) {
  lua_Integer value = luaL_checkinteger(co, index);
  luaL_argcheck(co, value >= -(lua_Integer)0x80000000 && value <= (lua_Integer)0xffffffff, index, "expected a 32-bit operand");
  return (uint32_t)value;
}

static uint32_t descriptor(lua_State *co, int index) {
  lua_Integer clip = luaL_checkinteger(co, index);
  luaL_argcheck(co, clip >= 0 && clip < MK4_CLIP_COUNT, index, "expected an animation clip number");
  return MK4_ANIMATION_TABLE + (uint32_t)clip * MK4_ANIMATION_DESCRIPTOR_SIZE;
}

static void bind_clip(Fighter *fighter, lua_State *co, int index, int first) {
  script_vm_bind_animation(fighter, descriptor(co, index), first);
}

/* Operations with an optional clip prefix bind it before acting. */
static void optional_clip(Fighter *fighter, lua_State *co, int index) {
  if (!lua_isnoneornil(co, index)) {
    bind_clip(fighter, co, index, 0);
  }
}

static int last_sample(const Fighter *fighter) {
  return fighter_move(fighter)->last;
}

static int clamp_forward(const Fighter *fighter, int count) {
  int target = fighter_sample(fighter) + count;
  return target > last_sample(fighter) ? last_sample(fighter) : target;
}

static void dispatch(lua_State *co, Fighter *fighter, uint32_t address, ScriptNativeKind kind, unsigned argument) {
  if (!script_native_dispatch(fighter, address, kind, argument)) {
    luaL_error(co, "unreviewed native binding %08x", (unsigned)address);
  }
}

#define ACTOR                                                                                                                                                  \
  Fighter *fighter = actor(co);                                                                                                                                \
  uint64_t generation = fighter->execution.generation;                                                                                                         \
  ScriptVm *vm = &fighter->execution.script;                                                                                                                   \
  (void)vm;                                                                                                                                                    \
  (void)generation
#define SETTLE return settle(co, fighter, generation)
#define BOOLEAN(value)                                                                                                                                         \
  lua_pushboolean(co, (value));                                                                                                                                \
  return settle_results(co, fighter, generation, 1)

/* --- animation binding and playback --- */

static int op_clip(lua_State *co) {
  ACTOR;
  bind_clip(fighter, co, 1, 0);
  script_vm_play(fighter, last_sample(fighter), true);
  SETTLE;
}

static int op_play_clip(lua_State *co) {
  ACTOR;
  int target = small(co, 1);
  bind_clip(fighter, co, 2, 0);
  script_vm_play(fighter, target, false);
  SETTLE;
}

static int op_forward(lua_State *co) {
  ACTOR;
  script_vm_play(fighter, last_sample(fighter), true);
  SETTLE;
}

static int op_forward_count(lua_State *co) {
  ACTOR;
  int count = small(co, 1);
  script_vm_play(fighter, clamp_forward(fighter, count), false);
  vm->play_ticks = count;
  SETTLE;
}

/* 05 and 47: advance to a sample when it lies ahead; 47 may bind a clip first. */
static int op_forward_to(lua_State *co) {
  ACTOR;
  int target = field24(co, 1);
  optional_clip(fighter, co, 2);
  if (target > fighter_sample(fighter)) {
    script_vm_play(fighter, target, false);
  }
  SETTLE;
}

static int op_backward_count(lua_State *co) {
  ACTOR;
  int target = fighter_sample(fighter) - field24(co, 1);
  script_vm_play(fighter, target < 0 ? 0 : target, false);
  SETTLE;
}

static int op_backward(lua_State *co) {
  ACTOR;
  script_vm_play(fighter, 0, true);
  SETTLE;
}

static int op_clip_alt(lua_State *co) {
  ACTOR;
  int first = small(co, 1);
  bind_clip(fighter, co, 2, first);
  script_vm_play(fighter, last_sample(fighter), true);
  SETTLE;
}

/* 09: bind at the final sample and play back to the start. */
static int op_clip_operation(lua_State *co) {
  ACTOR;
  bind_clip(fighter, co, 1, -1);
  script_vm_play(fighter, 0, true);
  SETTLE;
}

static int op_cursor_advance(lua_State *co) {
  ACTOR;
  script_vm_sample(fighter, fighter_sample(fighter) + small(co, 1));
  SETTLE;
}

static int op_bind_submit(lua_State *co) {
  ACTOR;
  int first = small(co, 1);
  bind_clip(fighter, co, 2, first);
  int current_sample = fighter_sample(fighter);
  script_vm_play(fighter, current_sample < last_sample(fighter) ? current_sample + 1 : current_sample, false);
  SETTLE;
}

static int op_playback_native(lua_State *co) {
  ACTOR;
  script_vm_sample(fighter, fighter_sample(fighter) + 1);
  SETTLE;
}

static int op_open_peer_clip(lua_State *co) {
  ACTOR;
  script_vm_bind_animation((Fighter *)fighter_opponent(fighter), descriptor(co, 1), 0);
  SETTLE;
}

static int op_open_clip(lua_State *co) {
  ACTOR;
  int first = small(co, 1);
  bind_clip(fighter, co, 2, first);
  SETTLE;
}

static int op_play_pair(lua_State *co) {
  ACTOR;
  int count = small(co, 1);
  script_vm_play(fighter, clamp_forward(fighter, count), false);
  vm->paired = true;
  vm->play_ticks = count;
  SETTLE;
}

static int op_bind_cursor(lua_State *co) {
  ACTOR;
  int first = small(co, 1);
  bind_clip(fighter, co, 2, first);
  SETTLE;
}

/* 12, 1d and 49: submit one sample; 49 may bind a clip first. */
static int op_cursor_set(lua_State *co) {
  ACTOR;
  int pose = small(co, 1);
  optional_clip(fighter, co, 2);
  script_vm_sample(fighter, pose);
  SETTLE;
}

static int op_cursor_last(lua_State *co) {
  ACTOR;
  script_vm_sample(fighter, last_sample(fighter));
  SETTLE;
}

/* 17: bind, start at sample one and play to the end two samples per tick. */
static int op_play_double(lua_State *co) {
  ACTOR;
  bind_clip(fighter, co, 1, 0);
  int last = last_sample(fighter);
  script_vm_sample(fighter, last > 0 ? 1 : 0);
  script_vm_play(fighter, last, true);
  vm->stride = 2;
  vm->play_ticks = (last - fighter_sample(fighter) + 1) / 2 + 1;
  SETTLE;
}

static int op_double_count(lua_State *co) {
  ACTOR;
  int count = small(co, 1);
  script_vm_play(fighter, clamp_forward(fighter, count), false);
  vm->stride = 2;
  vm->play_ticks = (count + 1) / 2;
  SETTLE;
}

static int op_double_to(lua_State *co) {
  ACTOR;
  int target = small(co, 1);
  int current_sample = fighter_sample(fighter);
  script_vm_play(fighter, target, false);
  vm->stride = 2;
  vm->play_ticks = (target - current_sample + 1) / 2;
  SETTLE;
}

static int op_play_interleaved(lua_State *co) {
  ACTOR;
  bind_clip(fighter, co, 1, 0);
  script_vm_interpolated_play(fighter, 200);
  SETTLE;
}

static int op_inherited_interleaved(lua_State *co) {
  ACTOR;
  script_vm_interpolated_play(fighter, (unsigned)small(co, 1));
  SETTLE;
}

/* 46, 68, 69: half-speed playback; 46 counts from the cursor, 68 restarts. */
static int op_half_count(lua_State *co) {
  ACTOR;
  int count = small(co, 1);
  optional_clip(fighter, co, 2);
  int last = last_sample(fighter);
  int target = count != 0 ? fighter_sample(fighter) + count : last;
  script_vm_slow_play(fighter, target > last ? last : target, count == 0 || target > last);
  SETTLE;
}

static int op_half_restart(lua_State *co) {
  ACTOR;
  optional_clip(fighter, co, 1);
  script_vm_sample(fighter, 0);
  script_vm_slow_play(fighter, last_sample(fighter), true);
  SETTLE;
}

static int op_half_finish(lua_State *co) {
  ACTOR;
  script_vm_slow_play(fighter, last_sample(fighter), true);
  SETTLE;
}

/* 48: bind an optional clip and play to the end. */
static int op_prefixed_finish(lua_State *co) {
  ACTOR;
  optional_clip(fighter, co, 1);
  script_vm_play(fighter, last_sample(fighter), true);
  SETTLE;
}

/* 4a: rewind to a sample when it lies behind. */
static int op_playback_pair(lua_State *co) {
  ACTOR;
  int target = small(co, 1);
  if (target < fighter_sample(fighter)) {
    script_vm_play(fighter, target, false);
  }
  SETTLE;
}

/* 4b, 4c: paired playback to a sample, driving the captured peer. */
static int op_playback_single(lua_State *co) {
  ACTOR;
  script_vm_play(fighter, small(co, 1), true);
  vm->paired = true;
  SETTLE;
}

static int op_blend(lua_State *co) {
  ACTOR;
  script_vm_blend(fighter, descriptor(co, 1));
  SETTLE;
}

/* --- waits, returns and natives --- */

static int op_wait(lua_State *co) {
  ACTOR;
  vm->wake = g_battle.ticks + (uint64_t)field24(co, 1);
  SETTLE;
}

/* 37, 54: wait a miss count, or a hit count when the last contact landed. */
static int op_wait_branch(lua_State *co) {
  ACTOR;
  int miss = field12(co, 1), hit = field12(co, 2);
  vm->wake = g_battle.ticks + (uint64_t)((vm->result & 1u) ? hit : miss);
  SETTLE;
}

static int op_park(lua_State *co) {
  ACTOR;
  script_vm_wait_native(fighter, 0, NULL);
  SETTLE;
}

/* 30, 3f: return through the entry's typed continuation. */
static int op_ready(lua_State *co) {
  ACTOR;
  script_vm_return(fighter);
  SETTLE;
}

/* 1b: return with a playback limit and a word for the native caller. */
static int op_return_word(lua_State *co) {
  ACTOR;
  vm->playback_limit = (unsigned)small(co, 1);
  vm->returned_word = word(co, 2);
  script_vm_return(fighter);
  SETTLE;
}

static int op_call(lua_State *co) {
  ACTOR;
  dispatch(co, fighter, word(co, 1), MK4_CALL, 0);
  SETTLE;
}

static int op_tail(lua_State *co) {
  ACTOR;
  dispatch(co, fighter, word(co, 1), MK4_TAIL, 0);
  SETTLE;
}

static int op_spawn(lua_State *co) {
  ACTOR;
  unsigned type = (unsigned)word(co, 1);
  dispatch(co, fighter, word(co, 2), MK4_SPAWN, type);
  SETTLE;
}

static int op_handler(lua_State *co) {
  ACTOR;
  dispatch(co, fighter, word(co, 1), MK4_PHYSICS, 0);
  SETTLE;
}

static int op_clear_handler(lua_State *co) {
  ACTOR;
  fighter->body.physics = NULL;
  SETTLE;
}

static int handler_operation(lua_State *co, uint32_t address) {
  ACTOR;
  dispatch(co, fighter, address, MK4_HANDLER, (unsigned)small(co, 1));
  SETTLE;
}

static int op_scene_event(lua_State *co) {
  return handler_operation(co, MK4_NATIVE_CONTEXT);
}

static int op_face(lua_State *co) {
  return handler_operation(co, MK4_NATIVE_FACE_PEER_AND_RESUME_SCRIPT);
}

static int op_native_return(lua_State *co) {
  return handler_operation(co, MK4_NATIVE_SCRIPT_REACTION_DISPATCH);
}

static int op_native_step(lua_State *co) {
  return handler_operation(co, MK4_NATIVE_DISPATCH_OPPONENT_REACTION);
}

/* --- predicates: true when the wordcode branch would be taken --- */

static int predicate(lua_State *co, int native_index, unsigned argument, unsigned bit) {
  ACTOR;
  dispatch(co, fighter, word(co, native_index), MK4_PREDICATE, argument);
  BOOLEAN((vm->result & bit) != 0);
}

static int op_predicate_flags(lua_State *co) {
  return predicate(co, 1, 0, 4u);
}

static int op_predicate_bit(lua_State *co) {
  return predicate(co, 1, 0, 1u);
}

static int op_predicate_argument(lua_State *co) {
  return predicate(co, 2, (unsigned)small(co, 1), 1u);
}

static int op_airborne(lua_State *co) {
  ACTOR;
  BOOLEAN(fighter_airborne(fighter));
}

static int op_down(lua_State *co) {
  ACTOR;
  BOOLEAN((fighter->held & BUTTON_BIT(BUTTON_DOWN)) != 0);
}

static int op_fast_start(lua_State *co) {
  ACTOR;
  BOOLEAN(fighter_fast_start(fighter));
}

static int op_blocked(lua_State *co) {
  ACTOR;
  BOOLEAN(fighter->contact == CONTACT_BLOCK);
}

static int op_contact(lua_State *co) {
  ACTOR;
  BOOLEAN((vm->result & 1u) != 0);
}

static int op_edge(lua_State *co) {
  ACTOR;
  bool outer = action_outer_edge(fighter);
  vm->result = (vm->result & ~1u) | (outer ? 1u : 0u);
  BOOLEAN(outer);
}

static int op_query_branch(lua_State *co) {
  ACTOR;
  script_vm_contact(fighter, MK4_DATA_CONTACT_QUERY_B_TABLE, (unsigned)small(co, 1));
  BOOLEAN((vm->result & 1u) != 0);
}

/* --- contact queries --- */

static int query(lua_State *co, uint32_t table) {
  ACTOR;
  script_vm_contact(fighter, table, (unsigned)small(co, 1));
  SETTLE;
}

static int op_query_b(lua_State *co) {
  return query(co, MK4_DATA_CONTACT_QUERY_B_TABLE);
}

static int op_query_a(lua_State *co) {
  return query(co, MK4_DATA_CONTACT_QUERY_A_TABLE);
}

static int op_query_d(lua_State *co) {
  return query(co, MK4_DATA_CONTACT_QUERY_D_TABLE);
}

static int op_weapon_query(lua_State *co) {
  return query(co, MK4_WEAPON_ATTACK_TABLE);
}

/* 70: within a distance, dispatch a reaction and record contact. */
static int op_reaction_distance(lua_State *co) {
  ACTOR;
  unsigned argument = (unsigned)small(co, 1);
  q16 limit = (q16)word(co, 2);
  if (g_battle.distance <= limit) {
    dispatch(co, fighter, MK4_NATIVE_SCRIPT_REACTION_DISPATCH, MK4_HANDLER, argument);
    if (!vm->active || fighter->execution.generation != generation || fighter->pending != NULL) {
      return lua_yield(co, 0);
    }
    vm->result |= 1u;
  } else {
    vm->result &= ~1u;
  }
  SETTLE;
}

/* --- actor state, sound and effects --- */

static int op_attack_selector(lua_State *co) {
  ACTOR;
  vm->selector = (unsigned)word(co, 1);
  SETTLE;
}

static int sound(lua_State *co, int bank) {
  ACTOR;
  battle_sound(bank, small(co, 1));
  SETTLE;
}

static int op_sound_a(lua_State *co) {
  return sound(co, SOUND_A);
}

static int op_sound_b(lua_State *co) {
  return sound(co, SOUND_B);
}

static int op_sound_c(lua_State *co) {
  return sound(co, SOUND_C);
}

static int op_sound_indexed(lua_State *co) {
  ACTOR;
  mk4_script_indexed_sound(fighter, small(co, 1));
  SETTLE;
}

static int op_sound_context(lua_State *co) {
  ACTOR;
  mk4_script_context_sound(fighter, small(co, 1));
  SETTLE;
}

static int op_sound_roster(lua_State *co) {
  ACTOR;
  mk4_script_roster_sound(fighter, small(co, 1));
  SETTLE;
}

static int op_sound_special(lua_State *co) {
  ACTOR;
  mk4_script_impact(fighter);
  SETTLE;
}

static int op_sound_variant(lua_State *co) {
  ACTOR;
  mk4_script_draw_voice(fighter);
  SETTLE;
}

static int op_sound_random(lua_State *co) {
  ACTOR;
  battle_sound(SOUND_TOKEN, battle_random(0x10000) & 1 ? 0x66d : 0x64a);
  SETTLE;
}

static int op_sound_token(lua_State *co) {
  ACTOR;
  battle_sound(SOUND_TOKEN, 0x627);
  SETTLE;
}

static int op_block_sound(lua_State *co) {
  ACTOR;
  mk4_script_guard_block_sound_26(fighter);
  SETTLE;
}

static int op_block_sound_alt(lua_State *co) {
  ACTOR;
  mk4_script_guard_block_sound_28(fighter);
  SETTLE;
}

static int op_task_callback(lua_State *co) {
  ACTOR;
  mk4_script_impact_callback(fighter);
  SETTLE;
}

static int op_release_attack(lua_State *co) {
  ACTOR;
  mk4_script_attack_return(fighter);
  SETTLE;
}

static int op_action_recovery(lua_State *co) {
  ACTOR;
  fighter->action = ACTION_CODE_1006;
  SETTLE;
}

static int op_action_followup(lua_State *co) {
  ACTOR;
  fighter->action = ACTION_CODE_1007;
  SETTLE;
}

static int op_spawn_effect(lua_State *co) {
  ACTOR;
  mk4_script_shake(fighter, small(co, 1));
  SETTLE;
}

static int op_effect(lua_State *co) {
  ACTOR;
  mk4_script_effect(fighter, small(co, 1));
  SETTLE;
}

static int op_no_guard(lua_State *co) {
  ACTOR;
  fighter->flags |= FF_NO_GUARD;
  SETTLE;
}

static int op_allow_commands(lua_State *co) {
  ACTOR;
  fighter->flags &= ~(unsigned)FF_BUSY;
  SETTLE;
}

static int op_scene_active(lua_State *co) {
  ACTOR;
  mk4_script_scene_active(fighter);
  SETTLE;
}

static int op_native_release(lua_State *co) {
  ACTOR;
  mk4_script_context(fighter, small(co, 1));
  SETTLE;
}

static int op_damage(lua_State *co) {
  ACTOR;
  mk4_script_damage(fighter, (q16)word(co, 1));
  SETTLE;
}

static int op_native_word(lua_State *co) {
  ACTOR;
  mk4_script_pair_adjust(fighter, (q16)word(co, 1));
  SETTLE;
}

static int op_disarm_peer(lua_State *co) {
  ACTOR;
  mk4_script_disarm(fighter);
  SETTLE;
}

/* --- motion --- */

static int op_motion(lua_State *co) {
  ACTOR;
  fighter_set_speed(fighter, (q16)word(co, 1));
  fighter->body.vel[AXIS_Y] = (q16)word(co, 2);
  fighter->body.gravity = (q16)word(co, 3);
  SETTLE;
}

static int op_motion_block(lua_State *co) {
  ACTOR;
  int left = (int32_t)word(co, 1), right = (int32_t)word(co, 2), up = (int32_t)word(co, 3), gravity = (int32_t)word(co, 4);
  mk4_script_reaction_motion(fighter, left, right, up, gravity);
  SETTLE;
}

static int op_peer_motion(lua_State *co) {
  ACTOR;
  q16 speed = (q16)word(co, 1), up = (q16)word(co, 2), gravity = (q16)word(co, 3);
  action_directional_motion(fighter, speed, up, gravity);
  SETTLE;
}

static int op_speed(lua_State *co) {
  ACTOR;
  fighter_set_speed(fighter, (q16)word(co, 1));
  SETTLE;
}

static int op_speed_negative(lua_State *co) {
  ACTOR;
  fighter_set_speed(fighter, (q16)(0u - word(co, 1)));
  SETTLE;
}

static int op_gravity(lua_State *co) {
  ACTOR;
  fighter->body.gravity = (q16)word(co, 1);
  SETTLE;
}

static int op_position(lua_State *co) {
  ACTOR;
  q16 horizontal = (q16)word(co, 1), vertical = (q16)word(co, 2);
  fighter->body.pos[AXIS_X] += q16_mul(fighter_axis(fighter, AXIS_X), horizontal);
  fighter->body.pos[AXIS_Z] += q16_mul(fighter_axis(fighter, AXIS_Z), horizontal);
  fighter->body.pos[AXIS_Y] += vertical;
  SETTLE;
}

/* 6c: delay, duration, then the speed word. */
static int op_schedule_motion(lua_State *co) {
  ACTOR;
  int delay = field12(co, 1), duration = field12(co, 2);
  mk4_script_schedule_motion(fighter, (int32_t)word(co, 3), delay, duration);
  SETTLE;
}

static int op_landing(lua_State *co) {
  ACTOR;
  mk4_script_stop(fighter);
  SETTLE;
}

/* 60: wait until the actor is grounded. */
static int op_native_context(lua_State *co) {
  ACTOR;
  vm->wait_ground = fighter_airborne(fighter);
  SETTLE;
}

static int op_edge_push(lua_State *co) {
  ACTOR;
  action_edge_push(fighter);
  SETTLE;
}

/* --- queries for scripts and tests --- */

/* 3c selection: Kitana and Noob reuse the Sonya and Reiko slots. */
static int op_roster_name(lua_State *co) {
  Fighter *fighter = actor(co);
  int roster = fighter->roster;
  roster = roster == 16 ? 2 : roster == 17 ? 7 : roster;
  luaL_argcheck(co, roster >= 0 && roster < 16, 1, "actor outside the selectable roster");
  lua_pushstring(co, roster_names[roster]);
  return 1;
}

static int op_result(lua_State *co) {
  lua_pushinteger(co, (lua_Integer)actor(co)->execution.script.result);
  return 1;
}

static int op_current_sample(lua_State *co) {
  lua_pushinteger(co, fighter_sample(actor(co)));
  return 1;
}

// clang-format off: Keep table entries on one row, including long rows.
static const luaL_Reg operations[] = {
  {"clip", op_clip},
  {"play_clip", op_play_clip},
  {"forward", op_forward},
  {"forward_count", op_forward_count},
  {"forward_to", op_forward_to},
  {"prefixed_to", op_forward_to},
  {"backward_count", op_backward_count},
  {"backward", op_backward},
  {"clip_alt", op_clip_alt},
  {"clip_operation", op_clip_operation},
  {"cursor_advance", op_cursor_advance},
  {"bind_submit", op_bind_submit},
  {"playback_native", op_playback_native},
  {"open_peer_clip", op_open_peer_clip},
  {"open_clip", op_open_clip},
  {"play_pair", op_play_pair},
  {"bind_cursor", op_bind_cursor},
  {"cursor_set", op_cursor_set},
  {"playback_tail", op_cursor_set},
  {"prefixed_sample", op_cursor_set},
  {"cursor_last", op_cursor_last},
  {"submit_last", op_cursor_last},
  {"play_double", op_play_double},
  {"double_count", op_double_count},
  {"double_to", op_double_to},
  {"play_interleaved", op_play_interleaved},
  {"inherited_interleaved", op_inherited_interleaved},
  {"half_count", op_half_count},
  {"half_restart", op_half_restart},
  {"half_finish", op_half_finish},
  {"prefixed_finish", op_prefixed_finish},
  {"playback_pair", op_playback_pair},
  {"playback_single", op_playback_single},
  {"playback_pair_reverse", op_playback_single},
  {"native_sound", op_forward},
  {"blend", op_blend},
  {"wait", op_wait},
  {"wait_branch", op_wait_branch},
  {"wait_result", op_wait_branch},
  {"park", op_park},
  {"ready", op_ready},
  {"finish", op_ready},
  {"return_word", op_return_word},
  {"call", op_call},
  {"tail", op_tail},
  {"spawn", op_spawn},
  {"handler", op_handler},
  {"clear_handler", op_clear_handler},
  {"scene_event", op_scene_event},
  {"face", op_face},
  {"native_return", op_native_return},
  {"native_step", op_native_step},
  {"predicate_flags", op_predicate_flags},
  {"predicate_bit", op_predicate_bit},
  {"predicate_argument", op_predicate_argument},
  {"airborne", op_airborne},
  {"down", op_down},
  {"fast_start", op_fast_start},
  {"blocked", op_blocked},
  {"contact", op_contact},
  {"edge", op_edge},
  {"query_branch", op_query_branch},
  {"query_b", op_query_b},
  {"query_a", op_query_a},
  {"query_d", op_query_d},
  {"weapon_query", op_weapon_query},
  {"reaction_distance", op_reaction_distance},
  {"attack_selector", op_attack_selector},
  {"sound_a", op_sound_a},
  {"sound_b", op_sound_b},
  {"sound_c", op_sound_c},
  {"sound_indexed", op_sound_indexed},
  {"sound_context", op_sound_context},
  {"sound_roster", op_sound_roster},
  {"sound_special", op_sound_special},
  {"sound_variant", op_sound_variant},
  {"sound_random", op_sound_random},
  {"sound_token", op_sound_token},
  {"block_sound", op_block_sound},
  {"block_sound_alt", op_block_sound_alt},
  {"task_callback", op_task_callback},
  {"release_attack", op_release_attack},
  {"action_recovery", op_action_recovery},
  {"action_followup", op_action_followup},
  {"spawn_effect", op_spawn_effect},
  {"effect", op_effect},
  {"no_guard", op_no_guard},
  {"allow_commands", op_allow_commands},
  {"scene_active", op_scene_active},
  {"native_release", op_native_release},
  {"damage", op_damage},
  {"native_word", op_native_word},
  {"disarm_peer", op_disarm_peer},
  {"motion", op_motion},
  {"motion_block", op_motion_block},
  {"peer_motion", op_peer_motion},
  {"speed", op_speed},
  {"speed_negative", op_speed_negative},
  {"gravity", op_gravity},
  {"position", op_position},
  {"schedule_motion", op_schedule_motion},
  {"landing", op_landing},
  {"native_context", op_native_context},
  {"edge_push", op_edge_push},
  {"roster_name", op_roster_name},
  {"result", op_result},
  {"current_sample", op_current_sample},
  {NULL, NULL},
};
// clang-format on

#define OPERATION_COUNT (sizeof operations / sizeof *operations - 1)
static unsigned operation_counts[OPERATION_COUNT];

/* Each registered operation counts its calls, so tests can report which
 * wordcode semantics the gate exercised. A yield inside the operation
 * unwinds through this frame, which Lua permits for C functions. */
static int counted(lua_State *co) {
  lua_Integer index = lua_tointeger(co, lua_upvalueindex(1));
  operation_counts[index]++;
  return operations[index].func(co);
}

void script_lua_visit_operations(void (*visit)(const char *name, unsigned count)) {
  for (size_t i = 0; i < OPERATION_COUNT; ++i) {
    visit(operations[i].name, operation_counts[i]);
  }
}

/* ---- loading ---- */

static void run_file(const char *path, int results) {
  if (luaL_loadfile(L, path) != LUA_OK || lua_pcall(L, 0, results, 0) != LUA_OK) {
    fatal("Lua: %s", lua_tostring(L, -1));
  }
}

static void remove_field(const char *table, const char *field) {
  lua_getglobal(L, table);
  lua_pushnil(L);
  lua_setfield(L, -2, field);
  lua_pop(L, 1);
}

/* Register one module's label -> function table under the linked addresses. */
static void register_scripts(const char *path) {
  run_file(path, 1);
  if (!lua_istable(L, -1)) {
    fatal("Lua: %s must return a table of scripts", path);
  }
  lua_getglobal(L, "mk4");
  lua_getfield(L, -1, "scripts");                  /* mk4.scripts */
  lua_getfield(L, -2, "labels");                   /* mk4.labels: name -> address */
  lua_rawgetp(L, LUA_REGISTRYINDEX, &scripts_key); /* address -> function */
  lua_rawgetp(L, LUA_REGISTRYINDEX, &names_key);   /* address -> name */
  /* stack: file, mk4, scripts, labels, registry scripts, registry names */
  lua_pushnil(L);
  while (lua_next(L, -7) != 0) {
    if (lua_type(L, -2) != LUA_TSTRING || !lua_isfunction(L, -1)) {
      fatal("Lua: %s must map label names to functions", path);
    }
    const char *name = lua_tostring(L, -2);
    lua_getfield(L, -5, name);
    if (!lua_isinteger(L, -1)) {
      fatal("Lua: %s names no linked script label (%s)", name, path);
    }
    lua_Integer address = lua_tointeger(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, -6, name);
    if (!lua_isnil(L, -1)) {
      fatal("Lua: script %s is defined twice", name);
    }
    lua_pop(L, 1);
    if (coverage) {
      /* mk4.instrument(name, fn) returns a counting wrapper that tail-calls fn. */
      lua_getfield(L, -7, "instrument");
      lua_pushstring(L, name);
      lua_pushvalue(L, -3);
      lua_call(L, 2, 1);
      lua_replace(L, -2);
      lua_pushvalue(L, -1);
      lua_setfield(L, -9, name); /* the module's own references count too */
    }
    lua_pushvalue(L, -1);
    lua_setfield(L, -7, name); /* mk4.scripts[name] = fn */
    lua_pushvalue(L, -1);
    lua_rawseti(L, -5, address); /* scripts[address] = fn */
    lua_pushstring(L, name);
    lua_rawseti(L, -4, address); /* names[address] = name */
    lua_pop(L, 1);
  }
  lua_pop(L, 6);
}

static void directory_of(const char *path, char *buffer, size_t size) {
  const char *slash = strrchr(path, '/');
  size_t length = slash != NULL ? (size_t)(slash - path + 1) : 0;
  require(length < size, "Lua program path too long");
  memcpy(buffer, path, length);
  buffer[length] = '\0';
}

void script_lua_open(const char *symbols_path, const char *program_path) {
  require(L == NULL, "Lua runtime already open");
  L = luaL_newstate();
  require(L != NULL, "Lua state allocation failed");
  luaL_requiref(L, LUA_GNAME, luaopen_base, 1);
  luaL_requiref(L, LUA_TABLIBNAME, luaopen_table, 1);
  luaL_requiref(L, LUA_STRLIBNAME, luaopen_string, 1);
  luaL_requiref(L, LUA_MATHLIBNAME, luaopen_math, 1);
  lua_pop(L, 4);
  /* Scripts stay deterministic and sandboxed: no host randomness, files or code loading. */
  remove_field("math", "random");
  remove_field("math", "randomseed");
  remove_field("_G", "dofile");
  remove_field("_G", "loadfile");
  remove_field("_G", "load");
  lua_newtable(L);
  lua_rawsetp(L, LUA_REGISTRYINDEX, &scripts_key);
  lua_newtable(L);
  lua_rawsetp(L, LUA_REGISTRYINDEX, &names_key);
  lua_newtable(L);
  lua_rawsetp(L, LUA_REGISTRYINDEX, &threads_key);
  lua_createtable(L, 0, (int)OPERATION_COUNT + 4);
  for (size_t i = 0; i < OPERATION_COUNT; ++i) {
    lua_pushinteger(L, (lua_Integer)i);
    lua_pushcclosure(L, counted, 1);
    lua_setfield(L, -2, operations[i].name);
  }
  run_file(symbols_path, 1);
  if (!lua_istable(L, -1)) {
    fatal("Lua: %s must return the linked symbol table", symbols_path);
  }
  lua_getfield(L, -1, "layout");
  const char *layout = lua_tostring(L, -1);
  char expected[65];
  for (int i = 0; i < 32; ++i) {
    snprintf(expected + 2 * i, 3, "%02x", (unsigned char)MK4_LAYOUT_ID[i]);
  }
  if (layout == NULL || strcmp(layout, expected) != 0) {
    fatal("%s was generated for another data layout: rebuild the application with this source package", symbols_path);
  }
  lua_pop(L, 1);
  lua_getfield(L, -1, "labels");
  lua_setfield(L, -3, "labels");
  lua_getfield(L, -1, "natives");
  lua_setfield(L, -3, "natives");
  lua_pop(L, 1);
  lua_newtable(L);
  lua_setfield(L, -2, "scripts");
  lua_setglobal(L, "mk4");
  char directory[512], path[1024];
  directory_of(program_path, directory, sizeof directory);
  snprintf(path, sizeof path, "%smk4.lua", directory);
  run_file(path, 0);
  run_file(program_path, 1);
  if (!lua_istable(L, -1)) {
    fatal("Lua: %s must return a list of module files", program_path);
  }
  lua_Integer count = luaL_len(L, -1);
  for (lua_Integer i = 1; i <= count; ++i) {
    lua_rawgeti(L, -1, i);
    const char *module = lua_tostring(L, -1);
    if (module == NULL) {
      fatal("Lua: %s lists a non-string module", program_path);
    }
    snprintf(path, sizeof path, "%s%s", directory, module);
    lua_pop(L, 1);
    register_scripts(path);
  }
  lua_pop(L, 1);
  entries_started = 0;
  memset(entry_counts, 0, sizeof entry_counts);
  memset(operation_counts, 0, sizeof operation_counts);
}

void script_lua_open_beside(const char *data_path, const char *program_path) {
  char directory[512], symbols[1024];
  directory_of(data_path, directory, sizeof directory);
  snprintf(symbols, sizeof symbols, "%smk4_symbols.lua", directory);
  script_lua_open(symbols, program_path);
}

void script_lua_close(void) {
  if (L != NULL) {
    lua_close(L);
    L = NULL;
  }
  current = NULL;
  running = NULL;
}
