/* 45e460 -> mask 4e4178: HP + BLOCK + RUN; no Up/HK/LP/LK. */
static bool can_first_fatality(const Fighter *fighter) {
  unsigned chord = BUTTON_BIT(BUTTON_HIGH_PUNCH) | BUTTON_BIT(BUTTON_BLOCK) | BUTTON_BIT(BUTTON_RUN);
  unsigned mask = chord | BUTTON_BIT(BUTTON_UP) | BUTTON_BIT(BUTTON_HIGH_KICK) | BUTTON_BIT(BUTTON_LOW_PUNCH) | BUTTON_BIT(BUTTON_LOW_KICK);
  return can_close_fatality(fighter) && (fighter->held & mask) == chord;
}

static void first_camera_start(Fighter *fighter) {
  battle_camera_sequence(fighter, opponent(fighter), first_camera_shots, COUNT(first_camera_shots));
  retail_script_004e0368(fighter);
}

static void first_fatality_script(Fighter *fighter) {
  native_hold(fighter, maxi(1, 8 - g_battle.phase_ticks), first_camera_start);
}

static void first_victim_start(Fighter *victim) {
  native_blends(victim, first_victim_setup, COUNT(first_victim_setup), retail_script_004e03f0);
}

static void script_first_begin(Fighter *fighter) {
  fighter->script_phase = 1;
  native_blends(fighter, first_actor_blend, COUNT(first_actor_blend), retail_script_004e0378);
}

static void script_first_pull(Fighter *fighter) {
  first_victim_start(opponent(fighter));
  retail_script_004e0388(fighter);
}

static void script_first_cues1(Fighter *fighter) {
  battle_sound(SOUND_B, 0x1b);
  battle_sound(SOUND_B, 0x29);
  retail_script_004e0398(fighter);
}

static void script_first_cues2(Fighter *fighter) {
  battle_sound(SOUND_B, 0x2b);
  retail_script_004e03a8(fighter);
}

static void script_first_cues3(Fighter *fighter) {
  battle_sound(SOUND_B, 0x2a);
  retail_script_004e03b8(fighter);
}

static void script_first_cues4(Fighter *fighter) {
  battle_sound(SOUND_B, 0x1b);
  retail_script_004e03c8(fighter);
}

static void script_first_head(Fighter *fighter) {
  Fighter *victim = opponent(fighter);
  battle_sound(SOUND_B, 0x1c);
  battle_sound(SOUND_B, 0);
  ScenePiece *head = battle_attach_head(victim, fighter, 10);
  battle_attach_piece_model(head, &first_spine, fighter);
  battle_joint_spray(victim, &first_blood);
  retail_script_004e03d8(fighter);
}

static void script_first_victim_blends(Fighter *victim) {
  native_blends(victim, first_victim_blends, COUNT(first_victim_blends), retail_script_004e0420);
}

static void script_first_victim_collapse(Fighter *victim) {
  battle_sound(SOUND_B, 0x18);
  battle_camera_shake(2, 3, 0x2e14);
  retail_script_004e0430(victim);
}

static void script_first_victim_end(Fighter *victim) {
  fighter_finish_mmove(victim, finish_scene);
}

static void sub_zero_first_fatality(Fighter *fighter) {
  start_fatality(fighter, FINISH_SUB_ZERO_1);
}
