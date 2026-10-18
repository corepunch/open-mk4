#include "game.h"
#include "native.h"
#include "mk4_characters.h"
#include "script.h"

/* Combat values remain the native approximation until measured against retail. */
static const MoveDef s_idle = {CLIP_ASSET_IDLE, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_walk = {267, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_crouch = {259, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_jump = {304, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_jab = {294, 0, MOVE_END, 5, 4, 12, 7, 19, 1.25f, 0.13f, SF_ATTACK | SF_LOCK, NULL, 0};
static const MoveDef s_kick = {71, 0, MOVE_END, 10, 5, 18, 12, 25, 1.85f, 0.32f, SF_ATTACK | SF_LOCK, NULL, 0};
static const MoveDef s_sweep = {63, 0, MOVE_END, 11, 5, 23, 10, 45, 1.65f, 0.25f, SF_ATTACK | SF_LOCK | SF_LOW | SF_LAUNCH, NULL, 0};
static const MoveDef s_uppercut = {293, 0, MOVE_END, 9, 5, 30, 18, 60, 1.35f, 0.5f, SF_ATTACK | SF_LOCK | SF_LAUNCH, NULL, 0};
static const MoveDef s_throw = {75, 0, MOVE_END, 6, 2, 25, 14, 42, 0.95f, 0.45f, SF_ATTACK | SF_LOCK | SF_LAUNCH | SF_UNBLOCKABLE, NULL, 0};
static const MoveDef s_block = {322, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_hit = {247, 0, MOVE_END, 0, 0, 24, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_HOLD, NULL, 0};
static const MoveDef s_knockdown = {186, 0, MOVE_END, 0, 0, 54, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_WAIT_LAND, NULL, 0};
static const MoveDef s_victory = {111, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_air_throw = {82, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_ATTACK | SF_LOCK | SF_UNBLOCKABLE, NULL, 0};
static const MoveDef s_air_thrown = {202, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION, NULL, 0};
static const MoveDef s_cpu_kick_script = {308, 0, 24, 0, 0, 18, 11, 0, 0.f, 0.f, SF_LOCK, NULL, 0};
static const MoveDef s_powered_punch_script = {85, 0, 54, 0, 0, 18, 9, 0, 0.f, 0.f, SF_LOCK, NULL, 0};
static const MoveDef s_powered_punch_finish = {105, 0, 29, 0, 0, 19, 12, 0, 0.f, 0.f, SF_LOCK, NULL, 0};

static void cage_script_cpu_kick_tail(Fighter *fighter);
static void cage_script_powered_punch_tail(Fighter *fighter);
static void cage_script_powered_punch_finish_tail(Fighter *fighter);
static void cage_script_fatality_1_signal(Fighter *fighter);
static void cage_script_fatality_2_continue(Fighter *fighter);

static void cage_ready(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    fighter_start_move(fighter, S_JUMP, roster_def(fighter->roster)->moves[S_JUMP]);
  } else {
    fighter_enter_state(fighter, S_IDLE);
  }
}

#include "generated/cage.inc"
#include "generated/frames_cage.inc"

/* Contact records from Cage's 53d2b8/53d268 tables, selected by the
 * generated controller callbacks 479b70/479f00/47a500. */
static const AttackRecord s_cage_cpu_kick_contact = {0x15c28, -0x35c2, 0x104df, 0x5c28, 17, 10, 6, 0, 0x1c28};
static const AttackRecord s_cage_powered_punch_contact = {0x107ae, -0x9c28, 0xdc28, 0xa8f5, 38, 10, 143, 0, 0x170a};
static const AttackRecord s_cage_powered_finish_contact = {0x12b85, -0x4f5c, 0x10a3d, 0xf5c2, 39, 39, 4, 0, 0x1eb8};

enum {
  CAGE_EVENT_CPU_KICK_RECOVERY = 1,
  CAGE_EVENT_POWERED_RECOVERY,
  CAGE_EVENT_FINISH_RECOVERY,
  CAGE_EVENT_SCENE_LOOP,
  CAGE_EVENT_FATALITY_1 = 17,
  CAGE_EVENT_FATALITY_2
};

static void cage_start_event_recovery(BattleEvent *event, int frames, int kind) {
  (void)kind;
  Fighter *owner = &g_battle.fighters[event->owner];
  native_hold(owner, frames, cage_ready);
}

static bool cage_combo_event(BattleEvent *event, Fighter *owner, Fighter *opponent) {
  ContactResult contact;
  (void)opponent;

  if (event->phase == 0) {
    if (event->data[0] == 0) {
      contact = battle_contact(owner, &s_cage_cpu_kick_contact);
      if (contact == CONTACT_MISS) {
        event->data[0] = 1;
        fighter_start_move(owner, S_PUNCH, &s_powered_punch_script);
        fighter_start_fiber(owner, retail_script_004e80c8, NULL);
        return false;
      }
      cage_start_event_recovery(event, s_cpu_kick_script.recovery, CAGE_EVENT_CPU_KICK_RECOVERY);
      return false;
    }

    if (event->data[0] == 1) {
      contact = battle_contact(owner, &s_cage_powered_punch_contact);
      if (contact != CONTACT_MISS) {
        fighter_start_move(owner, S_KICK, &s_powered_punch_finish);
        fighter_start_fiber(owner, retail_script_004e80f4, NULL);
        return false;
      }
      cage_start_event_recovery(event, 6, CAGE_EVENT_POWERED_RECOVERY);
      return false;
    }

    contact = battle_contact(owner, &s_cage_powered_finish_contact);
    cage_start_event_recovery(event, contact == CONTACT_MISS ? 5 : s_powered_punch_finish.recovery, CAGE_EVENT_FINISH_RECOVERY);
    return false;
  }

  return false;
}

static void cage_script_cpu_kick_tail(Fighter *fighter) {
  BattleEvent *event = battle_event_start(fighter, cage_combo_event, false, true);
  event->data[0] = 0;
}

static void cage_script_powered_punch_tail(Fighter *fighter) {
  BattleEvent *event = battle_event_start(fighter, cage_combo_event, false, true);
  event->data[0] = 1;
}

static void cage_script_powered_punch_finish_tail(Fighter *fighter) {
  BattleEvent *event = battle_event_start(fighter, cage_combo_event, false, true);
  event->data[0] = 2;
}

static bool cage_fatality_scene_event(BattleEvent *event, Fighter *owner, Fighter *opponent) {
  (void)opponent;
  if (g_battle.phase != PHASE_FATALITY) {
    return false;
  }
  if (event->phase == 0) {
    event->phase = 1;
    return true;
  }
  if (owner->sample_override >= 29) {
    battle_end_finisher();
    return false;
  }
  return true;
}

static void cage_script_fatality_1_signal(Fighter *fighter) {
  (void)battle_event_start(fighter, cage_fatality_scene_event, false, true);
}

static bool cage_fatality_2_event(BattleEvent *event, Fighter *owner, Fighter *opponent) {
  (void)opponent;
  if (g_battle.phase != PHASE_FATALITY) {
    return false;
  }
  if (event->phase == 0) {
    event->phase = 1;
    fighter_start_fiber(owner, retail_script_004e12b0, NULL);
    return true;
  }
  if (owner->sample_override >= 29) {
    battle_end_finisher();
    return false;
  }
  return true;
}

static void cage_script_fatality_2_continue(Fighter *fighter) {
  (void)battle_event_start(fighter, cage_fatality_2_event, false, true);
}

const uint32_t cage_weapon_descriptor[] = {0x6e, 0x1430cb, 0x144359, 0x13b318, 0x13eeb9, 0x13ee9e, 0x13eea1, 0xffffb334};

static void cage_start_script(Fighter *fighter, StateId state, const MoveDef *move, FighterFn script) {
  fighter_start_move(fighter, state, move);
  fighter_start_fiber(fighter, script, NULL);
}

static void cage_script_lp_return(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter->flags &= ~(unsigned)FF_PAIRED;
  victim->flags &= ~(unsigned)FF_PAIRED;
  mk4_script_stop_body(fighter);
  fighter_fiber_finish(cage_ready);
}

static void cage_script_fan_cycle(Fighter *fighter) {
  mk4_script_fan_cycle(fighter);
}

static void cage_script_fan_victim_end(Fighter *fighter) {
  mk4_script_fan_victim_end(fighter);
}

static void cage_script_fan_victory(Fighter *fighter) {
  retail_script_004e0158(fighter);
}

static void cage_script_fan_tail(Fighter *fighter) {
  mk4_script_fan_tail(fighter, &retail_script_004e0138_repeat_phase, retail_script_004e0148, retail_script_004e01a0);
}

static void cage_script_air_release(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_fiber(victim, retail_script_004e9910, NULL);
  mk4_script_air_throw_release(fighter);
  retail_script_004e5e50(fighter);
}

static void cage_script_spike_tail(Fighter *fighter) {
  mk4_script_spike_tail(fighter);
}

static void cage_script_kick_finish(Fighter *fighter) {
  retail_script_004e5c70(fighter);
}

static void cage_script_finish(Fighter *fighter) {
  mk4_script_finish_clip(fighter, 1);
  fighter_fiber_finish(cage_ready);
}

static void cage_script_air_finish(Fighter *fighter) {
  mk4_script_finish_clip(fighter, 1);
  fighter_fiber_finish(mk4_land);
}

static void cage_script_roundhouse_start(Fighter *fighter) {
  if (fighter_fast_start(fighter)) {
    retail_script_004e6860(fighter);
  } else {
    retail_script_004e6878(fighter);
  }
}

static void cage_script_fatality_1_continue(Fighter *fighter) {
  retail_script_004e12b0(fighter);
}

static void cage_script_lk_throw_return(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter->flags &= ~(unsigned)FF_PAIRED;
  victim->flags &= ~(unsigned)FF_PAIRED;
  fighter_fiber_finish(cage_ready);
}

static void cage_script_opponent_reaction(Fighter *fighter, int reaction) {
  if (reaction == 175) {
    return;
  }
  if (reaction == 77) {
    Fighter *victim = mk4_script_release_pair(fighter);
    victim->reaction = reaction;
    victim->flags |= FF_REACTING;
    fighter_fiber_finish(cage_ready);
    return;
  }
  require(false, "Unexpected Cage opponent reaction");
}

static void cage_lp_throw(Fighter *fighter) {
  cage_start_script(fighter, S_THROW, &s_throw, retail_script_004de798);
}

static void cage_lk_throw(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_move(fighter, S_THROW, &s_throw);
  fighter_start_move(victim, S_KNOCKDOWN, &s_air_thrown);
  fighter->flags |= FF_PAIRED;
  victim->flags |= FF_PAIRED | FF_REACTING;
  fighter_start_fiber(fighter, retail_script_004de2f8, NULL);
}

static void cage_jab(Fighter *fighter) {
  cage_start_script(fighter, S_PUNCH, &s_jab, retail_script_004e62f0);
}

static void cage_uppercut(Fighter *fighter) {
  cage_start_script(fighter, S_UPPERCUT, &s_uppercut, retail_script_004e62a0);
}

static void cage_sweep(Fighter *fighter) {
  cage_start_script(fighter, S_SWEEP, &s_sweep, retail_script_004e6798);
}

static void cage_air_hp(Fighter *fighter) {
  cage_start_script(fighter, S_PUNCH, &s_jab, retail_script_004e6658);
}

static void cage_air_lp(Fighter *fighter) {
  cage_start_script(fighter, S_PUNCH, &s_jab, retail_script_004e6710);
}

static void cage_air_kick(Fighter *fighter) {
  cage_start_script(fighter, S_KICK, &s_kick, retail_script_004e61f8);
}

static void cage_air_diagonal_hk(Fighter *fighter) {
  cage_start_script(fighter, S_KICK, &s_kick, retail_script_004e6500);
}

static void cage_air_diagonal_lk(Fighter *fighter) {
  cage_start_script(fighter, S_KICK, &s_kick, retail_script_004e63d8);
}

static void cage_air_throw(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_move(fighter, S_THROW, &s_air_throw);
  fighter_start_move(victim, S_KNOCKDOWN, &s_air_thrown);
  fighter->flags |= FF_PAIRED;
  victim->flags |= FF_PAIRED | FF_REACTING;
  fighter_start_fiber(fighter, retail_script_004e5e28, NULL);
}

static void cage_fan_fatality(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_fiber(victim, retail_script_004e0188, NULL);
  fighter_start_fiber(fighter, retail_script_004e0118, NULL);
}

static void cage_spike_fatality(Fighter *fighter) {
  fighter_start_fiber(fighter, retail_script_004e6950, NULL);
}

static void cage_fatality_1(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, CAGE_EVENT_FATALITY_1)) {
    return;
  }
  fighter_start_fiber(fighter, retail_script_004e12a0, cage_script_fatality_1_continue);
}

static void cage_fatality_2(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, CAGE_EVENT_FATALITY_2)) {
    return;
  }
  fighter_start_fiber(fighter, retail_script_004e5870, NULL);
}

static void cage_shadow_kick(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &s_kick);
  fighter->sample_override = 27;
  fighter_start_fiber(fighter, retail_script_004e5ba8, NULL);
}

static void cage_powered_punch(Fighter *fighter) {
  cage_start_script(fighter, S_PUNCH, &s_powered_punch_script, retail_script_004e80d0);
}

static void cage_cpu_kick(Fighter *fighter) {
  cage_start_script(fighter, S_KICK, &s_cpu_kick_script, retail_script_004e80a8);
}

static void cage_cpu_projectile(Fighter *fighter) {
  cage_sweep(fighter);
}

static void cage_high_punch(Fighter *fighter) {
  cage_powered_punch(fighter);
}

static void cage_high_kick(Fighter *fighter) {
  cage_start_script(fighter, S_KICK, &s_kick, retail_script_004e5ca0);
}

static void cage_low_kick(Fighter *fighter) {
  cage_start_script(fighter, S_KICK, &s_kick, retail_script_004e5d90);
}

static void cage_roundhouse(Fighter *fighter) {
  cage_start_script(fighter, S_KICK, &s_kick, retail_script_004e6800);
}

static void cage_press_high_punch(Fighter *fighter) {
  fighter_interrupt(fighter, cage_jab);
}

static void cage_press_low_punch(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_distance(fighter) <= 0.7f ? cage_lp_throw : cage_jab);
}

static void cage_press_high_kick(Fighter *fighter) {
  fighter_interrupt(fighter, cage_high_kick);
}

static void cage_press_low_kick(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? cage_sweep : cage_low_kick);
}

static void cage_press_crouching_hp(Fighter *fighter) {
  fighter_interrupt(fighter, cage_uppercut);
}

static void cage_press_crouching_kick(Fighter *fighter) {
  fighter_interrupt(fighter, cage_sweep);
}

static void cage_press_air_hp(Fighter *fighter) {
  fighter_interrupt(fighter, cage_air_hp);
}

static void cage_press_air_lp(Fighter *fighter) {
  fighter_interrupt(fighter, cage_air_lp);
}

static void cage_press_air_hk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z] ? cage_air_diagonal_hk : cage_air_kick);
}

static void cage_press_air_lk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z] ? cage_air_diagonal_lk : cage_air_kick);
}

// clang-format off: Keep table entries on one row, including long rows.
static const ButtonTable cage_standing = {{[BUTTON_HIGH_PUNCH] = cage_press_high_punch, [BUTTON_LOW_PUNCH] = cage_press_low_punch, [BUTTON_HIGH_KICK] = cage_press_high_kick, [BUTTON_LOW_KICK] = cage_press_low_kick}};
static const ButtonTable cage_crouching = {{[BUTTON_HIGH_PUNCH] = cage_press_crouching_hp, [BUTTON_LOW_PUNCH] = cage_press_crouching_hp, [BUTTON_HIGH_KICK] = cage_press_crouching_kick, [BUTTON_LOW_KICK] = cage_press_crouching_kick}};
static const ButtonTable cage_straight_air = {{[BUTTON_HIGH_PUNCH] = cage_press_air_hp, [BUTTON_LOW_PUNCH] = cage_press_air_lp, [BUTTON_HIGH_KICK] = cage_press_air_hk, [BUTTON_LOW_KICK] = cage_press_air_lk}};
static const ButtonTable cage_air_punch_followup = {{[BUTTON_LOW_PUNCH] = cage_press_air_lp}};
static const ButtonTable cage_diagonal_air = {{[BUTTON_HIGH_PUNCH] = cage_press_air_hp, [BUTTON_LOW_PUNCH] = cage_press_air_lp, [BUTTON_HIGH_KICK] = cage_press_air_hk, [BUTTON_LOW_KICK] = cage_press_air_lk}};
static const ButtonTable cage_close_hp_followup = {{[BUTTON_HIGH_PUNCH] = cage_press_high_punch}};
static const ButtonTable cage_combo_hp_followup = {{[BUTTON_HIGH_PUNCH] = cage_press_high_punch}};
static const ButtonTable cage_close_hk_followup = {{[BUTTON_HIGH_KICK] = cage_press_high_kick}};
static const ButtonTable cage_air_followup = {{[BUTTON_HIGH_KICK] = cage_press_air_hk, [BUTTON_LOW_KICK] = cage_press_air_lk}};
// clang-format on

static const Controls cage_controls = {NULL, 0,
    {
        [STANCE_STANDING] = &cage_standing,
        [STANCE_CROUCHING] = &cage_crouching,
        [STANCE_JUMP] = &cage_straight_air,
    }};

// clang-format off: Keep table entries on one row, including long rows.
static const MoveDef *const moves[S_COUNT] = {
  [S_IDLE] = &s_idle,
  [S_WALK] = &s_walk,
  [S_CROUCH] = &s_crouch,
  [S_JUMP] = &s_jump,
  [S_PUNCH] = &s_jab,
  [S_KICK] = &s_kick,
  [S_SWEEP] = &s_sweep,
  [S_UPPERCUT] = &s_uppercut,
  [S_THROW] = &s_throw,
  [S_BLOCK] = &s_block,
  [S_HIT] = &s_hit,
  [S_KNOCKDOWN] = &s_knockdown,
  [S_VICTORY] = &s_victory,
};
// clang-format on

const CharacterDef mk4_cage = {GAME_MK4, 14, "CAGE", &mk4_motion, moves, S_COUNT, &cage_controls, NULL, -74645, mk4_walk, mk4_jump, NULL};
