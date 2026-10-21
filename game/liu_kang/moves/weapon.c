#include "../character.h"

static const WeaponDef dragon_sword;
static void sword_travel(Fighter *fighter, int delay, int duration);
static void sword_cleanup(Fighter *fighter);
static bool sword_repeat(Fighter *fighter);
static void sword_blend_weight(Fighter *fighter, float weight);
static void sword_blend(Fighter *fighter, int ticks, FighterFn end);
static void sword_finish(Fighter *fighter);
static void sword_settle(Fighter *fighter);
static void sword_hit_blend(Fighter *fighter);
static void sword_recover(Fighter *fighter);
static void sword_contact(Fighter *fighter);
static void sword_swing(Fighter *fighter);
static void sword_strike_script(Fighter *fighter);
static void equip_dragon_sword(Fighter *fighter);
static void sword_attack(Fighter *fighter, int index);
static void sword_hp(Fighter *fighter);
static void sword_lp(Fighter *fighter);
static void sword_back_hp(Fighter *fighter);
static void sword_back_lp(Fighter *fighter);
static const MoveDef s_sword_hp = {.clip = 233, .last = 44, .flags = RETAIL, .action = ACTION_RETAIL_021C};
static const WeaponDef dragon_sword = {
    0x66, sword_hp, sword_lp, sword_back_hp, sword_back_lp, NULL, weapon_throw, equip_dragon_sword, SCENE_MODEL_DRAGON_SWORD};

static void sword_travel(Fighter *fighter, int delay, int duration) {
  q16 speed = fighter->weapon_strike == NULL ? 0x11eb : fighter->weapon_strike->speed;
  fighter->saved_motion[0] = speed;
  native_schedule_speed(fighter, speed, delay, duration);
}

static void sword_cleanup(Fighter *fighter) {
  if (fighter->body.physics == native_motion_step) {
    native_motion_stop(fighter);
    fighter_set_speed(fighter, 0);
  }
}

static bool sword_repeat(Fighter *fighter) {
  bool repeat = (fighter->buffered & BUTTON_BIT(BUTTON_HIGH_PUNCH)) != 0;
  fighter->buffered &= ~BUTTON_BIT(BUTTON_HIGH_PUNCH);
  return repeat;
}

static void sword_blend_weight(Fighter *fighter, float weight) {
  fighter->blend_ticks = (int)weight;
}

static void sword_blend(Fighter *fighter, int ticks, FighterFn end) {
  fighter->blend_clip = fighter_move(fighter)->clip;
  fighter->blend_sample = fighter_sample(fighter);
  fighter->blend_ticks = 0;
  fighter->blend_duration = ticks;
  native_begin(fighter, 1, ticks, end);
  for (int frame = 1; frame <= ticks; ++frame) {
    native_frame(fighter, frame, fighter->blend_sample, NULL);
    fighter->native.frames[frame - 1].aifunc = sword_blend_weight;
    fighter->native.frames[frame - 1].dist = (float)frame;
  }
  native_play(fighter, false);
}

static void sword_finish(Fighter *fighter) {
  fighter_finish_mmove(fighter, fighter_ready);
}

static void sword_settle(Fighter *fighter) {
  native_hold(fighter, 1, sword_finish);
}

static void sword_hit_blend(Fighter *fighter) {
  sword_blend(fighter, 10, sword_settle); /* 494e10: fixed armed stance, step 0x1999. */
}

/* script_phase selects the strike. A hit LP repeats into the second strike;
 * otherwise the clip completes, with the back-HP hit blend, before return. */
static void sword_recover(Fighter *fighter) {
  int index = fighter->script_phase;
  const WeaponStrike *strike = &liu_kang_sword_attacks[index];
  bool hit = fighter->connected;
  fighter->action = ACTION_RETAIL_1006;
  if (index == 0 && hit && sword_repeat(fighter)) {
    /* 496500 -> 4ec720/730, delayed travel then second strike. The move
     * rebind keeps the action's cleanup for the second strike. */
    fighter->mmove_cleanup = NULL;
    fighter_start_move(fighter, S_PUNCH, &liu_kang_sword_attacks[1].move);
    fighter->mmove_cleanup = sword_cleanup;
    fighter->script_phase = 1;
    fighter->weapon_strike = &liu_kang_sword_attacks[1];
    fighter->sample_override = liu_kang_sword_attacks[1].first;
    sword_travel(fighter, liu_kang_sword_attacks[1].delay, liu_kang_sword_attacks[1].travel);
    sword_strike_script(fighter);
    return;
  }
  fighter_set_speed(fighter, 0);
  fighter->body.physics = NULL;
  if (index == 2 && hit) {
    native_play_rows(fighter, strike->move.last, 1, sword_hit_blend);
  } else {
    native_finish_clip(fighter, sword_finish);
  }
}

static void sword_contact(Fighter *fighter) {
  const WeaponStrike *strike = &liu_kang_sword_attacks[fighter->script_phase];
  native_query_rows(fighter, &strike->record, strike->queries, 1, sword_recover);
}

static void sword_swing(Fighter *fighter) {
  const WeaponStrike *strike = &liu_kang_sword_attacks[fighter->script_phase];
  battle_sound(SOUND_C, strike->sound);
  native_play_rows(fighter, strike->target, 1, sword_contact);
}

static void sword_strike_script(Fighter *fighter) {
  const WeaponStrike *strike = &liu_kang_sword_attacks[fighter->script_phase];
  int opening = strike->windup - strike->first;
  if (opening <= 0) {
    sword_swing(fighter);
    return;
  }
  int pose = fighter_sample(fighter), last = fighter_move(fighter)->last;
  native_begin(fighter, 1, opening, sword_swing);
  for (int frame = 1; frame <= opening; ++frame) {
    native_frame(fighter, frame, last == MOVE_END ? pose + frame : mini(last, pose + frame), NULL);
  }
  native_play(fighter, false);
}

static void equip_dragon_sword(Fighter *fighter) {
  fighter_equip_weapon(fighter, &dragon_sword);
}

static void sword_attack(Fighter *fighter, int index) {
  if (fighter_airborne(fighter)) {
    retail_air_attack(fighter, index == 2 ? 0 : 1);
    return;
  }
  fighter_start_move(fighter, S_PUNCH, &liu_kang_sword_attacks[index].move);
  fighter->script_phase = index;
  fighter->weapon_strike = &liu_kang_sword_attacks[index];
  fighter->sample_override = liu_kang_sword_attacks[index].first;
  if (index == 3) {
    const WeaponStrike *strike = &liu_kang_sword_attacks[index];
    native_schedule_speed(fighter, strike->speed, maxi(0, strike->delay - 1), strike->travel);
    fighter->mmove_cleanup = sword_cleanup;
    liu_kang_entry_retail_script_004ec7f0(fighter);
    return;
  }
  if (liu_kang_sword_attacks[index].travel != 0) {
    sword_travel(fighter, liu_kang_sword_attacks[index].delay, liu_kang_sword_attacks[index].travel);
  }
  fighter->mmove_cleanup = sword_cleanup;
  sword_strike_script(fighter);
}

static void sword_hp(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    retail_air_high_punch(fighter);
    return;
  }
  fighter_start_move(fighter, S_PUNCH, &s_sword_hp);
  fighter->weapon_strike = NULL;
  fighter->sample_override = 0;
  fighter->mmove_cleanup = sword_cleanup;
  liu_kang_entry_retail_script_004ec680(fighter);
}

static void sword_lp(Fighter *fighter) {
  sword_attack(fighter, 0);
}

static void sword_back_hp(Fighter *fighter) {
  sword_attack(fighter, 2);
}

static void sword_back_lp(Fighter *fighter) {
  sword_attack(fighter, 3);
}

void liu_kang_weapon(Fighter *fighter) {
  if (fighter->armed) {
    fighter->weapon->throw_move(fighter);
  } else if (fighter->flags & FF_FOLLOW_UP) {
    equip_dragon_sword(fighter);
    weapon_followup_victim(fighter);
    sword_lp(fighter); /* 4ecb40[type 66 - 60] -> 4964d0. */
  } else {
    weapon_draw(fighter, &dragon_sword);
    liu_kang_entry_retail_script_004e62c0(fighter);
  }
}

void liu_kang_liu_script_draw_equip(Fighter *fighter) {
  equip_dragon_sword(fighter);
  liu_kang_entry_retail_script_004e62d8(fighter);
}

unsigned liu_kang_liu_script_sword_gate(Fighter *fighter) {
  return sword_repeat(fighter) ? 0u : 4u;
}

void liu_kang_liu_script_sword_blend(Fighter *fighter) {
  sword_blend(fighter, 5, sword_finish);
}
