#include "../character.h"

static void first_camera_start(Fighter *fighter);
static void first_victim_start(Fighter *victim);
static const SceneClip first_actor_blend[] = {BLEND(385, 0, 0x32f1, 0x3333)};
// clang-format off: Keep table entries on one row, including long rows.
static const SceneClip first_victim_setup[] = {
  BLEND(389, 0, 0x32f1, 0x3333),
  BLEND(387, 0, 0x32f1, 0x3333),
};
// clang-format on
// clang-format off: Keep table entries on one row, including long rows.
static const SceneClip first_victim_blends[] = {
  BLEND(387, 46, 0x666, 0x666),
  BLEND(387, 49, 0x43f, 0x43f),
  BLEND(387, 50, 0x43f, 0x886),
};
// clang-format on
// clang-format off: Keep table entries on one row, including long rows.
static const q16 first_camera[][3] = {
  {131072, -131072, -196608},
  {131072, -131072, -196608},
  {131072, -131072, -196608},
  {131072, -131072, -196608},
  {131072, -131072, -196608},
  {131072, -131072, -196608},
  {131072, -131072, -196608},
};
// clang-format on
// clang-format off: Keep table entries on one row, including long rows.
static const q16 orbit_camera[][3] = {
  {-327680, -163840, -458752},
  {-393216, -183500, -393216},
  {-393216, -255590, 393216},
  {393216, -216268, 393216},
  {393216, -190054, -393216},
  {-458752, -170393, -458752},
  {-524288, -157286, 524288},
  {589824, -144179, 589824},
  {655360, -117964, -655360},
  {-655360, -104857, -655360},
  {-655360, -104857, 655360},
  {655360, -104857, 655360},
  {655360, -117964, -655360},
  {-655360, -131072, -655360},
  {-655360, -144179, 655360},
  {655360, -150732, 655360},
  {655360, -157286, -655360},
  {-655360, -157286, -655360},
  {-655360, -157286, 655360},
  {655360, -157286, 655360},
  {655360, -157286, -655360},
  {-655360, -157286, -655360},
  {-655360, -157286, 655360},
  {655360, -157286, 655360},
  {655360, -157286, -655360},
};
// clang-format on
// clang-format off: Keep table entries on one row, including long rows.
static const CameraShot first_camera_shots[] = {
  {first_camera, COUNT(first_camera), 50, 0xccc, 0, 0, 0},
  {orbit_camera, COUNT(orbit_camera), 0, 0x45a, 3, 80, -0x4000},
};
// clang-format on
static const AttachmentDef first_spine = {.model = SCENE_MODEL_SPINE,
    .clip = 386,
    .first = 0,
    .last = 96,
    .loop_from = 52,
    .group_offset = {0, -0xccc / 65536.f, 0},
    .piece_offset = {0, 0x2b43 / 65536.f, 0},
    .piece_kind = 0,
    .scene_space = true};
static const ParticleVisual neck_blood_visual = {.animation = {.first = 0, .count = 1, .period = 1, .scale = Q16_ONE}};
static const ParticleSpray neck_blood = {
    .visual = &neck_blood_visual, .count = 1, .lifetime = 180, .gravity = 0x51e, .spread = {0x7ae, 0x7ae, 0x7ae}, .splash = true};
// clang-format off: Keep table entries on one row, including long rows.
static const EmissionPhase neck_blood_phases[] = {
  {11, 1, -1, 2},
  {11, 1, -1, 1},
  {39, 3, 2, 1},
  {142, 2, -1, 1},
};
// clang-format on
static const JointSprayDef first_blood = {.spray = &neck_blood,
    .phases = neck_blood_phases,
    .count = COUNT(neck_blood_phases),
    .origin_joint = 2,
    .base_joint = 3,
    .speed = 0x23d7,
    .variation = 0x7ae};

/* 45e460 -> mask 4e4178: HP + BLOCK + RUN; no Up/HK/LP/LK. */
bool sub_zero_can_first_fatality(const Fighter *fighter) {
  unsigned chord = BUTTON_BIT(BUTTON_HIGH_PUNCH) | BUTTON_BIT(BUTTON_BLOCK) | BUTTON_BIT(BUTTON_RUN);
  unsigned mask = chord | BUTTON_BIT(BUTTON_UP) | BUTTON_BIT(BUTTON_HIGH_KICK) | BUTTON_BIT(BUTTON_LOW_PUNCH) | BUTTON_BIT(BUTTON_LOW_KICK);
  return sub_zero_can_close_fatality(fighter) && (fighter->held & mask) == chord;
}

static void first_camera_start(Fighter *fighter) {
  battle_camera_sequence(fighter, sub_zero_opponent(fighter), first_camera_shots, COUNT(first_camera_shots));
  sub_zero_entry_retail_script_004e0368(fighter);
}

void sub_zero_first_fatality_script(Fighter *fighter) {
  native_hold(fighter, maxi(1, 8 - g_battle.phase_ticks), first_camera_start);
}

static void first_victim_start(Fighter *victim) {
  sub_zero_native_blends(victim, first_victim_setup, COUNT(first_victim_setup), sub_zero_entry_retail_script_004e03f0);
}

void sub_zero_script_first_begin(Fighter *fighter) {
  fighter->script_phase = 1;
  sub_zero_native_blends(fighter, first_actor_blend, COUNT(first_actor_blend), sub_zero_entry_retail_script_004e0378);
}

void sub_zero_script_first_pull(Fighter *fighter) {
  first_victim_start(sub_zero_opponent(fighter));
  sub_zero_entry_retail_script_004e0388(fighter);
}

void sub_zero_script_first_cues1(Fighter *fighter) {
  battle_sound(SOUND_B, 0x1b);
  battle_sound(SOUND_B, 0x29);
  sub_zero_entry_retail_script_004e0398(fighter);
}

void sub_zero_script_first_cues2(Fighter *fighter) {
  battle_sound(SOUND_B, 0x2b);
  sub_zero_entry_retail_script_004e03a8(fighter);
}

void sub_zero_script_first_cues3(Fighter *fighter) {
  battle_sound(SOUND_B, 0x2a);
  sub_zero_entry_retail_script_004e03b8(fighter);
}

void sub_zero_script_first_cues4(Fighter *fighter) {
  battle_sound(SOUND_B, 0x1b);
  sub_zero_entry_retail_script_004e03c8(fighter);
}

void sub_zero_script_first_head(Fighter *fighter) {
  Fighter *victim = sub_zero_opponent(fighter);
  battle_sound(SOUND_B, 0x1c);
  battle_sound(SOUND_B, 0);
  ScenePiece *head = battle_attach_head(victim, fighter, 10);
  battle_attach_piece_model(head, &first_spine, fighter);
  battle_joint_spray(victim, &first_blood);
  sub_zero_entry_retail_script_004e03d8(fighter);
}

void sub_zero_script_first_victim_blends(Fighter *victim) {
  sub_zero_native_blends(victim, first_victim_blends, COUNT(first_victim_blends), sub_zero_entry_retail_script_004e0420);
}

void sub_zero_script_first_victim_collapse(Fighter *victim) {
  battle_sound(SOUND_B, 0x18);
  battle_camera_shake(2, 3, 0x2e14);
  sub_zero_entry_retail_script_004e0430(victim);
}

void sub_zero_script_first_victim_end(Fighter *victim) {
  fighter_finish_mmove(victim, sub_zero_finish_scene);
}

void sub_zero_first_fatality(Fighter *fighter) {
  sub_zero_start_fatality(fighter, FINISH_SUB_ZERO_1);
}
