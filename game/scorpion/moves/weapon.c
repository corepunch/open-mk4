#include "../character.h"

static const WeaponDef sword;
static void equip_sword(Fighter *fighter);
static void sword_slash_recover(Fighter *fighter);
static void sword_slash_query(Fighter *fighter);
static void sword_slash_build(Fighter *fighter, int first, bool hit);
static void sword_attack(Fighter *fighter, bool hp, bool back);
static void sword_hp(Fighter *fighter);
static void sword_lp(Fighter *fighter);
static void sword_back_hp(Fighter *fighter);
static void sword_back_lp(Fighter *fighter);
static void sword_spin_recover(Fighter *fighter);
static void sword_spin(Fighter *fighter);
// clang-format off: Keep table entries on one row, including long rows.
static const AttackRecord sword_records[] = {
  {98304, -35389, 57016, 39321, 48, 12, 119, 0, 13107}, /* HP / LP */
  {124518, -34078, 83230, 51118, 49, 12, 119, 0, 13107}, /* back LP */
  {111411, -34078, 83230, 51118, 50, 12, 119, 0, 13107}, /* back HP */
  {78643, 1900, 193331, 24903, 99, 10, 118, 0, 3276}, /* spin */
  {110690, -60882, 68157, 62914, 156, 156, 4, 0, 11796}, /* air HP */
  {101580, -4587, 64225, 53739, 155, 155, 4, 0, 11796}, /* air LP */
  {57606, -37945, 73990, 35979, 154, 154, 0, ATTACK_MARK, 16384}, /* throw */
};
// clang-format on
SWORD_MOVE(s_sword_hp, 235, ACTION_RETAIL_010E);
SWORD_MOVE(s_sword_lp, 236, ACTION_RETAIL_010E);
SWORD_MOVE(s_sword_back_lp, 237, ACTION_RETAIL_0112);
SWORD_MOVE(s_sword_back_hp, 238, ACTION_RETAIL_0112);
SWORD_MOVE(s_spin_start, 223, ACTION_RETAIL_0407);
static const WeaponDef sword = {0x60, sword_hp, sword_lp, sword_back_hp, sword_back_lp, sword_spin, weapon_throw, equip_sword, SCENE_MODEL_SWORD};

static void equip_sword(Fighter *fighter) {
  fighter_equip_weapon(fighter, &sword);
}

void scorpion_script_weapon_ready(Fighter *fighter) {
  fighter_finish_mmove(fighter, fighter_ready);
}

static void sword_slash_recover(Fighter *fighter) {
  fighter->body.vel[AXIS_X] = fighter->body.vel[AXIS_Z] = 0;
  fighter->action = ACTION_RETAIL_1006;
}

static void sword_slash_query(Fighter *fighter) {
  if (!fighter->connected && battle_contact(fighter, &sword_records[0]) != CONTACT_MISS) {
    sword_slash_recover(fighter);
    sword_slash_build(fighter, fighter->mmove_frame, true);
  }
}

static void sword_slash_build(Fighter *fighter, int first, bool hit) {
  int last = hit ? 42 : 43;
  native_begin(fighter, first, last, scorpion_special_ready);
  for (int frame = first; frame <= last; ++frame) {
    FighterFn action = !hit && frame >= 17 && frame < 21 ? sword_slash_query : !hit && frame == 21 ? sword_slash_recover : NULL;
    native_frame(fighter, frame, frame + hit, action);
  }
  native_play(fighter, hit);
}

static void sword_attack(Fighter *fighter, bool hp, bool back) {
  if (fighter_airborne(fighter)) {
    weapon_air_frames(fighter, hp, (hp) ? scorpion_entry_retail_script_004e66d0 : scorpion_entry_retail_script_004e66f0);
    return;
  }
  const MoveDef *move = back ? (hp ? &s_sword_back_hp : &s_sword_back_lp) : (hp ? &s_sword_hp : &s_sword_lp);
  fighter_start_move(fighter, S_PUNCH, move);
  fighter->sample_override = 0;
  if (back) {
    (hp ? scorpion_entry_retail_script_004ec918 : scorpion_entry_retail_script_004ec8b0)(fighter);
  } else {
    sword_slash_build(fighter, 1, false);
  }
}

static void sword_hp(Fighter *fighter) {
  sword_attack(fighter, true, false);
}

static void sword_lp(Fighter *fighter) {
  sword_attack(fighter, false, false);
}

static void sword_back_hp(Fighter *fighter) {
  sword_attack(fighter, true, true);
}

static void sword_back_lp(Fighter *fighter) {
  sword_attack(fighter, false, true);
}

static void sword_spin_recover(Fighter *fighter) {
  scorpion_stop_body(fighter);
  fighter->script_phase = 1;
  native_hold(fighter, 12, scorpion_special_ready);
}

void scorpion_sword_spin_control(Fighter *fighter) {
  if (fighter_opponent(fighter)->hits >= 3) {
    sword_spin_recover(fighter);
    return;
  }
  if (!(fighter->held & BUTTON_BIT(BUTTON_LOW_PUNCH))) {
    fighter_finish_mmove(fighter, scorpion_special_ready);
    return;
  }
  /* 496d20: raw Left, then Right (which wins), steer at 0.1 * 0.6 along the
   * screen-side axis. */
  int direction = (fighter->held & BUTTON_BIT(BUTTON_RIGHT)) ? 1 : (fighter->held & BUTTON_BIT(BUTTON_LEFT)) ? -1 : 0;
  fighter_set_screen_speed(fighter, direction * 0xf5c);
}

void scorpion_sword_spin_query(Fighter *fighter) {
  if (battle_contact(fighter, &sword_records[3]) == CONTACT_BLOCK) {
    sword_spin_recover(fighter);
  } else {
    scorpion_sword_spin_control(fighter);
  }
}

static void sword_spin(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &s_spin_start);
  fighter->sample_override = 0;
  fighter->mmove_cleanup = scorpion_stop_body;
  scorpion_entry_retail_script_004e8850(fighter);
}

bool scorpion_can_weapon(const Fighter *fighter) {
  return g_battle.phase == PHASE_FIGHT && (fighter->armed || (!fighter->weapon_used && !(fighter->flags & FF_WEAPON_LOST)));
}

void scorpion_weapon(Fighter *fighter) {
  if (fighter->armed) {
    fighter->weapon->throw_move(fighter);
  } else if (fighter->flags & FF_FOLLOW_UP) {
    equip_sword(fighter);
    weapon_followup_victim(fighter);
    fighter_start_move(fighter, S_PUNCH, &s_sword_back_lp);
    fighter->sample_override = 0;
    scorpion_entry_retail_script_004ec8b0(fighter);
  } else {
    weapon_draw(fighter, &sword);
    scorpion_entry_retail_script_004e62c0(fighter);
  }
  fighter->sample_override = 0;
}

bool scorpion_can_pickup(const Fighter *fighter) {
  return weapon_can_pickup(fighter);
}

void scorpion_pickup(Fighter *fighter) {
  weapon_pickup(fighter);
}

void scorpion_script_draw_equip(Fighter *fighter) {
  equip_sword(fighter);
  scorpion_entry_retail_script_004e62d8(fighter);
}
