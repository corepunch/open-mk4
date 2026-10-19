#include "../character.h"

static void fatal_pose(Fighter *fighter, int clip, int sample, bool owner);
static void finish_scene(Fighter *fighter);
static void burn_start(Fighter *fighter);
static void burn_tail_done(Fighter *victim);
static void beast_start(Fighter *fighter);
static void beast_watch(Fighter *fighter);
static void beast_gore_object(Projectile *object);
// clang-format off: Keep table entries on one row, including long rows.
static const MoveDef fatal_moves[] = {
  FATAL_MOVE(13, 55),
  FATAL_MOVE(390, 144),
  FATAL_MOVE(391, 169),
  FATAL_MOVE(206, 99),
  FATAL_MOVE(115, 18),
  FATAL_MOVE(116, 49),
  FATAL_MOVE(117, 18),
  FATAL_MOVE(118, 49),
  FATAL_MOVE(12, 57),
  FATAL_MOVE(578, 48),
  FATAL_MOVE(579, 34),
  FATAL_MOVE(580, 77),
  FATAL_MOVE(581, 37),
  FATAL_MOVE(584, 77),
  FATAL_MOVE(585, 47),
  FATAL_MOVE(415, 129),
  FATAL_MOVE(416, 107),
  FATAL_MOVE(412, 48),
  FATAL_MOVE(293, 19),
  FATAL_MOVE(128, 109),
};
// clang-format on
static const SceneClip burn_repeat_blend[] = {BLEND(390, 105)};
static const SceneClip burn_victory_blend[] = {BLEND(115, 0)};
static const SceneClip burn_victim_blend[] = {BLEND(206, 0)};

static void fatal_pose(Fighter *fighter, int clip, int sample, bool owner) {
  if (fighter_move(fighter)->clip != clip) {
    for (int i = 0; i < COUNT(fatal_moves); ++i) {
      if (fatal_moves[i].clip == clip) {
        fighter_start_move(fighter, owner ? S_VICTORY : S_KNOCKDOWN, &fatal_moves[i]);
        break;
      }
    }
  }
  fighter->sample_override = sample;
  fighter->flags |= FF_UNTOUCHABLE | FF_BUSY;
}

bool scorpion_can_burn_fatality(const Fighter *fighter) {
  float distance = fighter_distance(fighter);
  return battle_can_finish(fighter) && distance >= q16_float(0x1cccc) && distance <= 3.f;
}

bool scorpion_can_beast_fatality(const Fighter *fighter) {
  return battle_can_finish(fighter) && fighter_distance(fighter) <= 1.f;
}

static void finish_scene(Fighter *fighter) {
  (void)fighter;
  battle_end_finisher();
}

static void burn_start(Fighter *fighter) {
  int frames = 8 - g_battle.phase_ticks;
  if (frames > 0) {
    native_hold(fighter, frames, scorpion_entry_retail_script_004e0278);
  } else {
    scorpion_entry_retail_script_004e0278(fighter);
  }
}

static void beast_start(Fighter *fighter) {
  scorpion_entry_retail_script_004d2e58(fighter);
}

void scorpion_start_fatality(Fighter *fighter, int id) {
  if (!battle_begin_finisher(fighter, id)) {
    scorpion_ready(fighter);
    return;
  }
  Fighter *victim = scorpion_opponent(fighter);
  fatal_pose(victim, 13, 0, false);
  fatal_pose(fighter, id == FINISH_BEAST ? 12 : id == FINISH_FAN ? 415 : id == FINISH_SPIKE ? 293 : 390, 0, true);
  if (id == FINISH_FAN) {
    fatal_pose(victim, 416, 0, false);
  }
  FighterFn start = id == FINISH_BURN ? burn_start : id == FINISH_BEAST ? beast_start : id == FINISH_FAN ? scorpion_fan_start : scorpion_spike_start;
  start(fighter);
}

void scorpion_burn_fatality(Fighter *fighter) {
  scorpion_start_fatality(fighter, FINISH_BURN);
}

void scorpion_beast_fatality(Fighter *fighter) {
  scorpion_start_fatality(fighter, FINISH_BEAST);
}

static void burn_tail_done(Fighter *victim) {
  fighter_finish_mmove(victim, mk4_script_thrown_ready);
}

void scorpion_script_burn_tail(Fighter *victim) {
  victim->body.physics = fighter_friction;
  native_finish_clip(victim, burn_tail_done);
}

void scorpion_script_burn_begin(Fighter *fighter) {
  battle_sound(SOUND_B, 14);
  battle_sound(SOUND_B, 46);
  scorpion_entry_retail_script_004e0288(fighter);
}

void scorpion_script_burn_victim_start(Fighter *fighter) {
  Fighter *victim = scorpion_opponent(fighter);
  victim->appearance = APPEARANCE_BURNED;
  fighter_cancel_mmove(victim);
  scorpion_entry_retail_script_004e0300(victim);
  battle_sound(SOUND_B, 15);
  scorpion_entry_retail_script_004e0298(fighter);
}

void scorpion_script_burn_repeat1(Fighter *fighter) {
  scorpion_native_blend(fighter, burn_repeat_blend, scorpion_entry_retail_script_004e02a8);
}

void scorpion_script_burn_repeat2(Fighter *fighter) {
  scorpion_native_blend(fighter, burn_repeat_blend, scorpion_entry_retail_script_004e02b8);
}

void scorpion_script_burn_victory(Fighter *fighter) {
  scorpion_native_blend(fighter, burn_victory_blend, scorpion_entry_retail_script_004e02d0);
}

void scorpion_script_burn_victim_cue(Fighter *victim) {
  battle_sound(SOUND_B, 16);
  scorpion_entry_retail_script_004e0310(victim);
}

void scorpion_script_burn_victim_repeat(Fighter *victim) {
  scorpion_entry_retail_script_004e0320(victim);
}

void scorpion_script_burn_victim_blend(Fighter *victim) {
  scorpion_native_blend(victim, burn_victim_blend, scorpion_entry_retail_script_004e0338);
}

void scorpion_script_burn_victim_impact(Fighter *victim) {
  mk4_script_shake(victim, 0);
  battle_particles(victim, 0, 4, true);
  scorpion_entry_retail_script_004e0348(victim);
}

void scorpion_script_burn_victim_settle(Fighter *victim) {
  mk4_script_shake(victim, 0);
  scorpion_entry_retail_script_004e0358(victim);
}

void scorpion_script_burn_victim_end(Fighter *victim) {
  fighter_finish_mmove(victim, finish_scene);
}

/* Both scene actors have parked their frame programs; the parked poses
 * remain while the finisher completes. */
static void beast_watch(Fighter *fighter) {
  if (fighter->script_phase == 9 && scorpion_opponent(fighter)->script_phase == 9) {
    battle_end_finisher();
  }
}

void scorpion_script_beast_begin(Fighter *fighter) {
  fighter->appearance = APPEARANCE_BEAST;
  fighter->body.physics = beast_watch;
  scorpion_entry_retail_script_004d2e88(fighter);
}

void scorpion_script_beast_attack(Fighter *fighter) {
  Fighter *victim = scorpion_opponent(fighter);
  fighter_cancel_mmove(victim);
  scorpion_entry_retail_script_004d2ef8(victim);
  scorpion_entry_retail_script_004d2eb0(fighter);
}

void scorpion_script_beast_victim_pieces(Fighter *victim) {
  battle_particles(victim, 0, 12, true);
  scorpion_entry_retail_script_004d2f10(victim);
}

void scorpion_script_beast_voice(Fighter *fighter) {
  battle_sound(SOUND_IMPACT_VOICE, roster_def(fighter->roster)->asset);
}

void scorpion_script_beast_park(Fighter *fighter) {
  fighter->script_phase = 9;
  native_begin(fighter, 0, 0, NULL);
  fighter->native.frames[0].thinkfunc = mk4_mmove_hold;
  native_play(fighter, false);
}

static void beast_gore_object(Projectile *object) {
  Fighter *victim = &g_battle.fighters[object->owner];
  if (g_battle.phase != PHASE_FATALITY) {
    object->kind = 0;
  } else if (object->ticks % 4 == 0) {
    battle_particles(victim, 0, 2, true);
  }
}

void scorpion_script_beast_gore(Fighter *victim, int type) {
  require(type == 0, "Unexpected beast gore task");
  battle_spawn_projectile(victim, PROJECTILE_EFFECT, 60, beast_gore_object);
}
