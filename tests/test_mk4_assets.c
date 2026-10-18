#include "game.h"
#include "mk4_assets.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void fujin_weapon_action(Fighter *fighter);
void fujin_wind_grab(Fighter *fighter);
void fujin_second_fatality(Fighter *fighter);
void fujin_first_fatality(Fighter *fighter);
void fujin_triple_kick(Fighter *fighter);
void fujin_wind(Fighter *fighter);
void fujin_wind_slam(Fighter *fighter);
void cage_fatality_1(Fighter *fighter);
void cage_fatality_2(Fighter *fighter);

static void check(bool ok, const char *message) {
  require(ok, message);
}

/* Positive Y is down. The lowest posed vertex is the sole, and root-motion Y
 * is what holds it on the floor. */
static float mesh_sole(const Character *ch, const Clip *clip, double frame) {
  Pose nodes[MK4_MAX_BONES];
  float sole = -1e9f;
  size_t n, j, k;
  pose(ch, clip, frame, nodes);
  for (n = 0; n < ch->bones.count; ++n) {
    const Bone *bone = &ch->bones.data[n];
    const Triangles *mesh;
    if (bone->flags & 4)
      continue;
    mesh = &ch->meshes.data[bone->mesh].triangles;
    for (j = 0; j < mesh->count; ++j)
      for (k = 0; k < 3; ++k) {
        Vec3 p = vec_add(nodes[n].position, mat3_apply(nodes[n].rotation, mesh->data[j].v[k].p));
        sole = fmaxf(sole, p.y);
      }
  }
  return sole;
}

static void combat_poses(const Assets *assets) {
  const Character *dragon = &assets->liu_dragon;
  check(dragon->bones.count == 10 && dragon->idle == 418 && assets->clips.data[418].frames.data[0].angles.count == 10,
      "Dragon scene must bind its complete original rig and animation channels");
  Pose dragon_pose[MK4_MAX_BONES];
  pose(dragon, &assets->clips.data[418], 160, dragon_pose);
  const Character *spine = &assets->scene_models.data[SCENE_MODEL_SPINE];
  check(spine->bones.count == 5 && spine->bones.data[0].mesh == 36 && spine->bones.data[4].channel == 5,
      "Spine accessory must use its recovered five meshes and reserve the first animation channel for the head");
  const Character *statue = &assets->scene_models.data[SCENE_MODEL_ICE_CLONE];
  check(statue->bones.count == 16 && statue->bones.data[1].mesh == 58 && statue->bones.data[15].mesh == 72,
      "Clone must use the recovered shared ice meshes rather than an owner's costume");
  check(statue->meshes.data == assets->hud.effects.meshes.data && character_texture(statue, 0) == &assets->hud.effects.textures.data[0],
      "Scene models must share decoded multi-page materials with existing shared effects");
  check(assets->scene_models.data[SCENE_MODEL_SWORD].bones.data[0].mesh == 99 && assets->scene_models.data[SCENE_MODEL_SCEPTER].bones.data[0].mesh == 113,
      "Shared weapon presentation must use the original sword and scepter meshes");
  check(assets->scene_models.data[SCENE_MODEL_DRAGON_SWORD].bones.data[0].mesh == 96, "Liu Kang weapon uses its own original rig");
  Pose weapon_pose[MK4_MAX_BONES];
  pose_static(&assets->scene_models.data[SCENE_MODEL_SCEPTER], weapon_pose);
  for (int mesh = 74; mesh <= 82; ++mesh) {
    check(assets->hud.effects.meshes.data[mesh].triangles.count == 8, "Ice blast must retain its spawn mesh and all eight textured track meshes");
    check((assets->hud.effects.meshes.data[mesh].flags & 0x180) == 0x100, "Ice blast meshes must preserve their original additive material flag");
  }
  check(fabsf(weapon_pose[0].position.y + .3f) < .001f, "Static weapon posing must preserve the descriptor grip offset");

  battle_init(4, 15);
  battle_bind_assets(assets);
  g_battle.phase = PHASE_FIGHT;
  Fighter *sub_zero = &g_battle.fighters[0];
  sub_zero->ticks = 70;
  check(fighter_pose_sample(sub_zero) == 0, "Pose snapshot must wrap the real 35-sample idle");
  battle_react(sub_zero, 67);
  battle_step((Input[2]){0});
  check(sub_zero->sample_override == 0 && sub_zero->frozen_clip == CLIP_ASSET_IDLE, "Freeze must preserve the displayed looping pose");
  const Character *closed = fighter_model(assets, sub_zero);
  const Character *intact = roster_character(assets, 4);
  check(closed->meshes.data == intact->meshes.data && closed->texture.pixels.data == intact->texture.pixels.data,
      "Closed-surface rigs must share immutable archive geometry and texture ownership");
  check(closed->bones.data[3].id == 2 && closed->bones.data[3].mesh == 20 && intact->bones.data[3].mesh == 2,
      "Freeze must select the recovered closed head mesh rather than the intact head");
  Fighter *goro = &g_battle.fighters[1];
  battle_react(goro, 67);
  battle_step((Input[2]){0});
  check(goro->appearance == APPEARANCE_FROZEN && fighter_clip(assets, goro)->id == 587, "Boss freeze must retain its own rig and pose");
  for (int i = 0; i < 143; ++i) {
    battle_step((Input[2]){0});
  }
  check(goro->state == S_IDLE && goro->appearance == APPEARANCE_NORMAL, "Boss freeze must thaw and restore control");
  static const SceneClip fallback_blend[] = {
      {.move = {.clip = 13}, .first = 0, .last = 0, .repeats = 1},
      {.move = {.clip = 304}, .first = 0, .last = 0, .repeats = 1, .blend_step = 0x1999},
  };
  scene_begin(goro, fallback_blend, 2, -1);
  scene_step(goro);
  check(goro->blend_clip == 587, "Scene blends must capture the compatible displayed clip for the victim rig");
  Pose blended[MK4_MAX_BONES];
  fighter_pose(assets, goro, blended);
  for (size_t i = 0; i < fighter_model(assets, goro)->bones.count; ++i) {
    check(isfinite(blended[i].position.x) && isfinite(blended[i].position.y) && isfinite(blended[i].position.z), "Fallback scene pose must remain finite");
  }

  battle_init(4, 0);
  battle_bind_assets(assets);
  g_battle.phase = PHASE_FIGHT;
  const AttackRecord strike = {0x18000, -0x10000, 0x18000, 0x18000, 37, 11, 1, 0, 0x1000};
  for (int rotated = 0; rotated < 2; ++rotated) {
    battle_reset_match();
    Fighter *a = &g_battle.fighters[0], *b = &g_battle.fighters[1];
    a->body.pos[AXIS_X] = 0;
    b->body.pos[AXIS_X] = rotated ? 0 : Q16_ONE;
    b->body.pos[AXIS_Z] = rotated ? Q16_ONE : 0;
    battle_world();
    check(battle_contact(a, &strike) == CONTACT_HIT, "Joint contact must follow the pair axis after a quarter turn");
    check(b->health == Q16_ONE - 0x1000, "Pose contact must retain record damage");
  }
  battle_reset_match();
  Fighter *victim = &g_battle.fighters[1];
  ScenePiece *head = battle_attach_head(victim, &g_battle.fighters[0], 10);
  static const AttachmentDef spine_animation = {.model = SCENE_MODEL_SPINE, .clip = 386, .first = 0, .last = 96, .loop_from = 52};
  battle_attach_piece_model(head, &spine_animation, &g_battle.fighters[0]);
  check(head != NULL && g_battle.attachments[0].def != NULL, "Detached pieces must accept independent animated accessory models");
  for (int i = 0; i < 97; ++i) {
    battle_step_presentation();
  }
  check(g_battle.attachments[0].sample == 52, "Spine attachment must loop from sample 96 back to 52 independently of the fighter");
  head->active = false;
  battle_step_presentation();
  check(g_battle.attachments[0].def == NULL, "Removing a detached parent must release its accessory");
  battle_reset_match();
  victim->appearance = APPEARANCE_FROZEN;
  battle_shatter(victim);
  int pieces = 0;
  bool unnamed_piece = false;
  for (int i = 0; i < MAX_SCENE_PIECES; ++i) {
    pieces += g_battle.pieces[i].active;
    check(!g_battle.pieces[i].active || g_battle.pieces[i].frozen, "Shatter pieces must preserve the frozen material");
    if (g_battle.pieces[i].active) {
      const Character *cut = &assets->cut_characters.data[roster_def(victim->roster)->asset];
      const Bone *bone = &cut->bones.data[g_battle.pieces[i].bone];
      check(!(bone->flags & 4), "Shatter must not draw invisible control roots as duplicate body meshes");
      unnamed_piece |= bone->id < 0;
    }
  }
  check(unnamed_piece, "Shatter must preserve unnamed visible bones in its remaining-body launch");
  check(pieces >= 15 && victim->appearance == APPEARANCE_HIDDEN, "Shatter must detach posed meshes before hiding the intact body");
  q16 height = g_battle.pieces[0].pos[AXIS_Y];
  battle_step_presentation();
  check(g_battle.pieces[0].pos[AXIS_Y] < height, "Detached pieces must follow their native launch vectors");
  battle_reset_match();
  check(!g_battle.pieces[0].active && g_battle.fighters[1].hidden_joints[0] == 0 && !g_battle.camera.active,
      "Rematch must clear scene objects and mesh visibility");
  for (int player = 0; player < 2; ++player) {
    battle_init(player == 0 ? 4 : 0, player == 1 ? 4 : 0);
    battle_bind_assets(assets);
    g_battle.phase = PHASE_FIGHT;
    for (int i = 0; i < 2; ++i) {
      g_battle.fighters[i].body.pos[AXIS_X] = i * Q16_ONE;
      g_battle.fighters[i].body.pos[AXIS_Y] = -3 * Q16_ONE;
      fighter_enter_state(&g_battle.fighters[i], S_JUMP);
    }
    fighter_press(&g_battle.fighters[player], BUTTON_BLOCK);
    for (int tick = 0; tick < 16; ++tick) {
      battle_step((Input[2]){0});
    }
    for (int i = 0; i < 2; ++i) {
      const Fighter *actor = &g_battle.fighters[i];
      check(fighter_clip(assets, actor)->id == (i == player ? 82 : 202), "Air throw must pose the original attacker and victim clips on their own rigs");
      Pose posed[MK4_MAX_BONES];
      fighter_pose(assets, actor, posed);
      for (size_t bone = 0; bone < fighter_model(assets, actor)->bones.count; ++bone) {
        check(isfinite(posed[bone].position.x + posed[bone].position.y + posed[bone].position.z), "Both air throw rigs must produce finite posed joints");
      }
    }
  }
}

static void cross_clip_pose_interpolation(const Assets *assets) {
  const Character *scorpion = roster_character(assets, 0);
  const Clip *from = &assets->clips.data[293];
  const Clip *to = &assets->clips.data[66];
  Pose source[MK4_MAX_BONES], target[MK4_MAX_BONES], blend[MK4_MAX_BONES];
  pose(scorpion, from, 15, source);
  pose(scorpion, to, 12, target);
  pose_blend_frames(scorpion, from, 15, to, 12, 0.f, blend);
  for (size_t i = 0; i < scorpion->bones.count; ++i) {
    check(fabsf(blend[i].position.x - source[i].position.x) < .0001f && fabsf(blend[i].position.y - source[i].position.y) < .0001f &&
              fabsf(blend[i].position.z - source[i].position.z) < .0001f,
        "Cross-clip interpolation does not begin at the previous animation frame");
    for (size_t k = 0; k < 9; ++k) {
      check(fabsf(blend[i].rotation.m[k] - source[i].rotation.m[k]) < .0001f, "Cross-clip interpolation changes its source endpoint");
    }
  }
  pose_blend_frames(scorpion, from, 15, to, 12, 1.f, blend);
  for (size_t i = 0; i < scorpion->bones.count; ++i) {
    check(fabsf(blend[i].position.x - target[i].position.x) < .0001f && fabsf(blend[i].position.y - target[i].position.y) < .0001f &&
              fabsf(blend[i].position.z - target[i].position.z) < .0001f,
        "Cross-clip interpolation does not end at the next animation frame");
    for (size_t k = 0; k < 9; ++k) {
      check(fabsf(blend[i].rotation.m[k] - target[i].rotation.m[k]) < .0001f, "Cross-clip interpolation changes its target endpoint");
    }
  }
}

static void fighter_render_snapshots(const Assets *assets) {
  battle_init(0, 0);
  Fighter *fighter = &g_battle.fighters[0];
  fighter_start_move(fighter, S_UPPERCUT, roster_def(0)->moves[S_UPPERCUT]);
  battle_bind_assets(assets);
  fighter_start_move(fighter, S_KICK, roster_def(0)->moves[S_KICK]);
  fighter->sample_override = 12;
  g_battle.ticks = 2;
  battle_capture_render_snapshot();
  const FighterAnimationFrame *from = &g_battle.previous_animation.fighters[0];
  const FighterAnimationFrame *to = &g_battle.current_animation.fighters[0];
  check(from->clip == 293 && to->clip == 66 && from->sample != to->sample, "Animation snapshots must preserve a branch into a different clip");
  const Character *model = fighter_model(assets, fighter);
  Pose source[MK4_MAX_BONES], target[MK4_MAX_BONES], rendered[MK4_MAX_BONES];
  pose(model, &assets->clips.data[from->clip], from->sample, source);
  pose(model, &assets->clips.data[to->clip], to->sample, target);
  fighter_render_pose(assets, fighter, 0.0, rendered);
  for (size_t i = 0; i < model->bones.count; ++i) {
    check(fabsf(rendered[i].position.x - source[i].position.x) < .0001f, "Render snapshot starts from the previous clip pose");
  }
  fighter_render_pose(assets, fighter, 1.0, rendered);
  for (size_t i = 0; i < model->bones.count; ++i) {
    check(fabsf(rendered[i].position.x - target[i].position.x) < .0001f, "Render snapshot reaches the new clip pose");
  }
  fighter_render_pose(assets, fighter, 0.5, rendered);
  for (size_t i = 0; i < model->bones.count; ++i) {
    check(isfinite(rendered[i].position.x) && isfinite(rendered[i].rotation.m[0]), "Cross-animation render snapshot must remain finite");
  }
}

static void sound_samples(void) {
  Package archive;
  package_load(&archive, "MK4/FILESYS.DAT");
  Sound pcm = sound_decode(package_get(&archive, name_hash("c:\\source\\mk4\\win\\sound\\pcmk0195.esf")), 195);
  check(pcm.rate == 11025 && pcm.channels == 1 && pcm.samples.count == 10296, "PCM ESF format/length");
  check(pcm.samples.data[0] == 0 && pcm.samples.data[1] == 0 && pcm.samples.data[2] == -256, "PCM ESF unsigned samples");
  Sound adpcm = sound_decode(package_get(&archive, name_hash("c:\\source\\mk4\\win\\sound\\pcmk0760.esf")), 760);
  check(adpcm.rate == 22050 && adpcm.channels == 1 && adpcm.samples.count == 19094, "ADPCM ESF format/length");
  check(adpcm.samples.data[0] == 0 && adpcm.samples.data[1] == 4 && adpcm.samples.data[2] == 8 && adpcm.samples.data[3] == 19,
      "ADPCM ESF must decode high nibble first with the native predictor");
  FREE_ARRAY(pcm.samples);
  FREE_ARRAY(adpcm.samples);
  package_free(&archive);
}

static void fallback_clip_clock(const Assets *assets) {
  battle_init(15, 4);
  battle_bind_assets(assets);
  g_battle.phase = PHASE_FIGHT;
  Fighter *fighter = &g_battle.fighters[0];
  /* The real Goro sequence reaches a humanoid final recovery after its five
   * damage cycles. Rendering selects the existing rig-compatible idle
   * fallback, whose shorter clip must not inherit that recovery's clock. */
  fighter->body.pos[AXIS_X] = -Q16_ONE / 4;
  g_battle.fighters[1].body.pos[AXIS_X] = Q16_ONE / 4;
  fighter_press(fighter, BUTTON_LOW_PUNCH);
  bool reached = false;
  for (int tick = 0; tick < 300; ++tick) {
    battle_step((Input[2]){{0}, {0}});
    if (fighter_move(fighter)->clip == 75) {
      reached = true;
      check(fighter_clip(assets, fighter) != &assets->clips.data[75], "Goro humanoid suffix must use the existing rig fallback");
      check(
          fighter_pose_sample(fighter) == 0 && fighter_pose_frame(fighter) == 0, "Fallback rig must hold its own neutral pose independently of the move clock");
      Pose bones[MK4_MAX_BONES];
      fighter_pose(assets, fighter, bones);
    }
  }
  check(reached && fighter->state == S_IDLE, "Asset-backed Goro LP throw must complete its selected final recovery");
}

static void cage_orb_assets(const Assets *assets) {
  check(assets->hud.effects2.meshes.count > 280 && assets->hud.effects2.textures.count > 0, "Cage orbs require the original share2 geometry and materials");
  for (int slot = 0; slot < 2; ++slot) {
    for (int high = 0; high < 2; ++high) {
      battle_init(slot ? 4 : 14, slot ? 14 : 4);
      battle_bind_assets(assets);
      g_battle.phase = PHASE_FIGHT;
      Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
      q16 half_distance = (high ? 4 : 3) * Q16_ONE;
      owner->body.pos[AXIS_X] = slot ? half_distance : -half_distance;
      victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
      Input input[2] = {0};
      const Button sequence[] = {
          BUTTON_DOWN, high ? (slot ? BUTTON_LEFT : BUTTON_RIGHT) : (slot ? BUTTON_RIGHT : BUTTON_LEFT), high ? BUTTON_HIGH_PUNCH : BUTTON_LOW_PUNCH};
      for (int entry = 0; entry < 3; ++entry) {
        input[slot].held = BUTTON_BIT(sequence[entry]);
        battle_step(input);
        input[slot].held = 0;
        battle_step(input);
        battle_step(input);
      }
      bool visible = false;
      for (int tick = 0; tick < 180; ++tick) {
        battle_step(input);
        for (int index = 0; index < MAX_PROJECTILES; ++index) {
          const Projectile *orb = &g_battle.projectiles[index];
          if (orb->kind == PROJECTILE_FIREBALL && orb->mesh_animation != NULL && orb->mesh_animation->source == MESH_SHARED2) {
            visible = true;
            const Geometry *geometry = mesh_animation_geometry(assets, orb->mesh_animation);
            int mesh = projectile_mesh_frame(orb);
            check(geometry == &assets->hud.effects2 && mesh >= 271 && mesh <= 280 && geometry->sprites.data[mesh].w > 0 && orb->mesh_animation->additive,
                "Orb animation must select the extracted native sprite frames from their own geometry bank");
          }
        }
        Pose bones[MK4_MAX_BONES];
        fighter_pose(assets, owner, bones);
        fighter_pose(assets, victim, bones);
        check(!fighter_fiber_active(owner) && !fighter_fiber_active(victim), "Asset-backed Cage orb owner and victim must use frame tables");
      }
      check(visible && victim->health == Q16_ONE - 0x1999 && owner->state == S_IDLE, "Both Cage arcs must resolve native contact with animated victim bounds");
    }
  }
}

static void restored_high_punch_contacts(const Assets *assets) {
  const int rosters[] = {5, 11};
  for (size_t row = 0; row < sizeof rosters / sizeof rosters[0]; ++row) {
    for (int slot = 0; slot < 2; ++slot) {
      battle_init(slot ? 4 : rosters[row], slot ? rosters[row] : 4);
      battle_bind_assets(assets);
      g_battle.phase = PHASE_FIGHT;
      Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
      owner->body.pos[AXIS_X] = slot ? Q16_ONE / 2 : -Q16_ONE / 2;
      victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
      Input input[2] = {0};
      input[slot].held = BUTTON_BIT(BUTTON_HIGH_PUNCH);
      battle_step(input);
      input[slot].held = 0;
      for (int tick = 0; tick < 90; ++tick) {
        battle_step(input);
        Pose bones[MK4_MAX_BONES];
        fighter_pose(assets, owner, bones);
      }
      check(victim->health == Q16_ONE - 0xccc && owner->state == S_IDLE && !fighter_fiber_active(owner),
          "Restored ordinary HP must hit once through actual pose bounds and complete native frame recovery");
    }
  }
}

static void kai_weapon_contacts(const Assets *assets) {
  check(assets->scene_models.count > SCENE_MODEL_KAI_WEAPON && assets->scene_models.data[SCENE_MODEL_KAI_WEAPON].bones.count > 0,
      "Kai weapon extraction must provide its own complete shared-geometry rig");
  for (int slot = 0; slot < 2; ++slot) {
    for (int strike = 0; strike < 3; ++strike) {
      battle_init(slot ? 4 : 11, slot ? 11 : 4);
      battle_bind_assets(assets);
      g_battle.phase = PHASE_FIGHT;
      Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
      owner->body.pos[AXIS_X] = slot ? 3 * Q16_ONE : -3 * Q16_ONE;
      victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
      Input input[2] = {0};
      const Button sequence[] = {BUTTON_DOWN, slot ? BUTTON_RIGHT : BUTTON_LEFT, BUTTON_LOW_PUNCH};
      for (int entry = 0; entry < 3; ++entry) {
        input[slot].held = BUTTON_BIT(sequence[entry]);
        battle_step(input);
        input[slot].held = 0;
        battle_step(input);
        battle_step(input);
      }
      for (int tick = 0; tick < 65; ++tick) {
        battle_step(input);
      }
      check(owner->armed && owner->weapon->model == SCENE_MODEL_KAI_WEAPON, "Kai draw command must equip his extracted retail weapon");
      owner->body.pos[AXIS_X] = slot ? 0xa666 : -0xa666;
      victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
      input[slot].held = BUTTON_BIT(strike == 1 ? BUTTON_LOW_PUNCH : BUTTON_HIGH_PUNCH);
      if (strike == 2) {
        input[slot].held |= BUTTON_BIT(slot ? BUTTON_RIGHT : BUTTON_LEFT);
      }
      battle_step(input);
      battle_step(input);
      input[slot].held = 0;
      for (int tick = 0; tick < 180; ++tick) {
        battle_step(input);
        Pose bones[MK4_MAX_BONES];
        fighter_pose(assets, owner, bones);
        fighter_pose(assets, victim, bones);
        check(!fighter_fiber_active(owner) && !fighter_fiber_active(victim), "Asset-backed Kai weapon owner and victim must remain on frame tables");
      }
      check(victim->health == Q16_ONE - (strike == 1 ? 10485 : 11796) && owner->state == S_IDLE,
          "Kai weapon records must hit once with real animated bounds and recover");
    }
  }
}

static void jarek_weapon_contacts(const Assets *assets) {
  check(assets->scene_models.count > SCENE_MODEL_JAREK_WEAPON && assets->scene_models.data[SCENE_MODEL_JAREK_WEAPON].bones.count > 0,
      "Jarek weapon extraction must provide its own complete shared-geometry rig");
  for (int slot = 0; slot < 2; ++slot) {
    for (int strike = 0; strike < 4; ++strike) {
      battle_init(slot ? 4 : 12, slot ? 12 : 4);
      battle_bind_assets(assets);
      g_battle.phase = PHASE_FIGHT;
      Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
      owner->body.pos[AXIS_X] = slot ? 3 * Q16_ONE : -3 * Q16_ONE;
      victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
      Input input[2] = {0};
      const Button sequence[] = {slot ? BUTTON_LEFT : BUTTON_RIGHT, slot ? BUTTON_LEFT : BUTTON_RIGHT, BUTTON_HIGH_PUNCH};
      for (int entry = 0; entry < 3; ++entry) {
        input[slot].held = BUTTON_BIT(sequence[entry]);
        battle_step(input);
        input[slot].held = 0;
        battle_step(input);
        battle_step(input);
      }
      for (int tick = 0; tick < 65; ++tick) {
        battle_step(input);
      }
      check(owner->armed && owner->weapon->model == SCENE_MODEL_JAREK_WEAPON, "Jarek draw command must equip his extracted retail weapon");
      owner->body.pos[AXIS_X] = slot ? 0xa666 : -0xa666;
      victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
      input[slot].held = BUTTON_BIT(strike == 1 || strike == 3 ? BUTTON_LOW_PUNCH : BUTTON_HIGH_PUNCH);
      if (strike >= 2) {
        input[slot].held |= BUTTON_BIT(slot ? BUTTON_RIGHT : BUTTON_LEFT);
      }
      battle_step(input);
      battle_step(input);
      input[slot].held = 0;
      for (int tick = 0; tick < 180; ++tick) {
        battle_step(input);
        Pose bones[MK4_MAX_BONES];
        fighter_pose(assets, owner, bones);
        fighter_pose(assets, victim, bones);
        check(!fighter_fiber_active(owner) && !fighter_fiber_active(victim), "Asset-backed Jarek weapon owner and victim must remain on frame tables");
      }
      check(victim->health == Q16_ONE - (strike == 0      ? 10485
                                            : strike == 2 ? 11796
                                                          : 9830) &&
                owner->state == S_IDLE,
          "Jarek weapon records must hit once with real animated bounds and recover");
    }
  }
}

static void shinnok_weapon_contacts(const Assets *assets) {
  check(assets->scene_models.count > SCENE_MODEL_SHINNOK_WEAPON && assets->scene_models.data[SCENE_MODEL_SHINNOK_WEAPON].bones.count > 0,
      "Shinnok weapon extraction must provide its own complete shared-geometry rig");
  for (int slot = 0; slot < 2; ++slot) {
    for (int strike = 0; strike < 4; ++strike) {
      battle_init(slot ? 4 : 6, slot ? 6 : 4);
      battle_bind_assets(assets);
      g_battle.phase = PHASE_FIGHT;
      Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
      owner->body.pos[AXIS_X] = slot ? 3 * Q16_ONE : -3 * Q16_ONE;
      victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
      Input input[2] = {0};
      const Button sequence[] = {slot ? BUTTON_RIGHT : BUTTON_LEFT, slot ? BUTTON_LEFT : BUTTON_RIGHT, BUTTON_LOW_PUNCH};
      for (int entry = 0; entry < 3; ++entry) {
        input[slot].held = BUTTON_BIT(sequence[entry]);
        battle_step(input);
        input[slot].held = 0;
        battle_step(input);
        battle_step(input);
      }
      for (int tick = 0; tick < 65; ++tick) {
        battle_step(input);
      }
      check(owner->armed && owner->weapon->model == SCENE_MODEL_SHINNOK_WEAPON, "Shinnok draw command must equip his extracted retail weapon");
      owner->body.pos[AXIS_X] = slot ? 0xa666 : -0xa666;
      victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
      input[slot].held = BUTTON_BIT(strike == 1 || strike == 3 ? BUTTON_LOW_PUNCH : BUTTON_HIGH_PUNCH);
      if (strike >= 2) {
        input[slot].held |= BUTTON_BIT(slot ? BUTTON_RIGHT : BUTTON_LEFT);
      }
      battle_step(input);
      battle_step(input);
      input[slot].held = 0;
      for (int tick = 0; tick < 180; ++tick) {
        battle_step(input);
        Pose bones[MK4_MAX_BONES];
        fighter_pose(assets, owner, bones);
        fighter_pose(assets, victim, bones);
        check(!fighter_fiber_active(owner) && !fighter_fiber_active(victim), "Asset-backed Shinnok weapon owner and victim must remain on frame tables");
      }
      const q16 damages[] = {11141, 13107, 9830, 11796};
      if (victim->health != Q16_ONE - damages[strike] || owner->state != S_IDLE) {
        fprintf(stderr, "Shinnok asset slot%d strike%d health%d state%d clip%d pose%d\n", slot, strike, victim->health, owner->state, fighter_move(owner)->clip,
            fighter_sample(owner));
      }
      check(
          victim->health == Q16_ONE - damages[strike] && owner->state == S_IDLE, "Shinnok weapon records must hit once with real animated bounds and recover");
    }
  }
}

static void cage_weapon_contacts(const Assets *assets) {
  check(assets->scene_models.count > SCENE_MODEL_CAGE_WEAPON && assets->scene_models.data[SCENE_MODEL_CAGE_WEAPON].bones.count > 0,
      "Cage weapon extraction must provide its own complete shared-geometry rig");
  for (int slot = 0; slot < 2; ++slot) {
    for (int strike = 0; strike < 4; ++strike) {
      battle_init(slot ? 4 : 14, slot ? 14 : 4);
      battle_bind_assets(assets);
      g_battle.phase = PHASE_FIGHT;
      Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
      owner->body.pos[AXIS_X] = slot ? 3 * Q16_ONE : -3 * Q16_ONE;
      victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
      Input input[2] = {0};
      const Button sequence[] = {slot ? BUTTON_LEFT : BUTTON_RIGHT, BUTTON_DOWN, slot ? BUTTON_LEFT : BUTTON_RIGHT, BUTTON_LOW_KICK};
      for (int entry = 0; entry < 4; ++entry) {
        input[slot].held = BUTTON_BIT(sequence[entry]);
        battle_step(input);
        input[slot].held = 0;
        battle_step(input);
        battle_step(input);
      }
      for (int tick = 0; tick < 65; ++tick) {
        battle_step(input);
      }
      check(owner->armed && owner->weapon->model == SCENE_MODEL_CAGE_WEAPON, "Cage draw command must equip his extracted retail weapon");
      owner->body.pos[AXIS_X] = slot ? 0xa666 : -0xa666;
      victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
      input[slot].held = BUTTON_BIT(strike == 1 || strike == 3 ? BUTTON_LOW_PUNCH : BUTTON_HIGH_PUNCH);
      if (strike >= 2) {
        input[slot].held |= BUTTON_BIT(slot ? BUTTON_RIGHT : BUTTON_LEFT);
      }
      battle_step(input);
      battle_step(input);
      input[slot].held = 0;
      for (int tick = 0; tick < 180; ++tick) {
        battle_step(input);
        Pose bones[MK4_MAX_BONES];
        fighter_pose(assets, owner, bones);
        fighter_pose(assets, victim, bones);
        check(!fighter_fiber_active(owner) && !fighter_fiber_active(victim), "Asset-backed Cage weapon owner and victim must remain on frame tables");
      }
      check(victim->health == Q16_ONE - (strike == 0      ? 11796
                                            : strike == 1 ? 9830
                                            : strike == 2 ? 10485
                                                          : 9175) &&
                owner->state == S_IDLE,
          "Cage weapon records must hit once with real animated bounds and recover");
    }
  }
}

static void axe_followup_contacts(const Assets *assets) {
  for (int slot = 0; slot < 2; ++slot) {
    for (int scenario = 0; scenario < 4; ++scenario) {
      battle_init(slot ? 4 : 7, slot ? 7 : 4);
      battle_bind_assets(assets);
      g_battle.phase = PHASE_FIGHT;
      Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
      owner->body.pos[AXIS_X] = slot ? 3 * Q16_ONE : -3 * Q16_ONE;
      victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
      Input input[2] = {0};
      const Button sequence[] = {BUTTON_DOWN, slot ? BUTTON_RIGHT : BUTTON_LEFT, BUTTON_HIGH_PUNCH};
      for (int entry = 0; entry < 3; ++entry) {
        input[slot].held = BUTTON_BIT(sequence[entry]);
        battle_step(input);
        input[slot].held = 0;
        battle_step(input);
        battle_step(input);
      }
      for (int tick = 0; tick < 65; ++tick) {
        battle_step(input);
      }
      check(owner->armed && owner->weapon->model == SCENE_MODEL_AXE, "Retail axe command must equip the extracted shared rig");
      q16 distance = scenario == 2 ? 6 * Q16_ONE : Q16_ONE;
      owner->body.pos[AXIS_X] = slot ? distance / 2 : -distance / 2;
      victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
      input[slot].held = BUTTON_BIT(BUTTON_LOW_PUNCH) | BUTTON_BIT(slot ? BUTTON_RIGHT : BUTTON_LEFT);
      if (scenario == 1) {
        input[1 - slot].held = BUTTON_BIT(BUTTON_BLOCK);
      }
      battle_step(input);
      battle_step(input);
      input[slot].held = 0;
      bool first = false, second = false;
      int health_changes = 0;
      q16 previous = victim->health;
      for (int tick = 0; tick < 220; ++tick) {
        battle_step(input);
        if (scenario == 3 && tick == 3) {
          fighter_enter_state(owner, S_HIT);
          check(owner->body.physics == NULL, "Interrupting axe startup must cancel its strafe callback");
        }
        first |= fighter_move(victim)->clip == 127;
        second |= fighter_move(victim)->clip == 131;
        health_changes += victim->health < previous;
        previous = victim->health;
        check(!fighter_fiber_active(owner) && !fighter_fiber_active(victim), "Axe follow-up and both victim streams must use frame tables");
      }
      if ((scenario == 0 && (!first || !second || health_changes != 2)) || (scenario >= 2 && health_changes != 0)) {
        fprintf(stderr, "Axe slot%d scenario%d first%d second%d contacts%d health%d\n", slot, scenario, first, second, health_changes, victim->health);
      }
      check(scenario != 0 || (first && second && health_changes == 2), "Back-LP must connect both distinct native axe strikes and victim streams");
      check(scenario < 2 || victim->health == Q16_ONE, "Axe follow-up must not run after a missed first strike");
      if (owner->state != S_IDLE || victim->state != (scenario == 1 ? S_BLOCK : S_IDLE) || owner->body.physics != NULL) {
        fprintf(stderr, "Axe recovery slot%d scenario%d owner%d victim%d clip%d health%d y%d vel%d gravity%d\n", slot, scenario, owner->state, victim->state,
            fighter_move(victim)->clip, victim->health, victim->body.pos[AXIS_Y], victim->body.vel[AXIS_Y], victim->body.gravity);
      }
      check(owner->state == S_IDLE && victim->state == (scenario == 1 ? S_BLOCK : S_IDLE) && owner->body.physics == NULL,
          "Axe follow-up must release movement and recover both participants");
    }
  }
}

static const mframe_t scene_pose_rows[3] = {{0}};
static const mmove_t scene_pose_phase = {.firstframe = 0, .lastframe = 2, .frame = scene_pose_rows, .endfunc = battle_scene_actor_stop};

static void scene_pose_entry(Fighter *actor) {
  fighter_mmove_phase(actor, &scene_pose_phase);
}

static void fujin_first_fatality_assets(const Assets *assets) {
  check(assets->hud.fujin_wind.meshes.count >= 6, "Fujin tornado must load its actual six fj_fat_g.geo surfaces");
  for (int slot = 0; slot < 2; ++slot) {
    battle_init(slot ? 2 : 5, slot ? 5 : 2);
    battle_bind_assets(assets);
    g_battle.phase = PHASE_FINISH;
    g_battle.winner = slot;
    Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
    owner->body.pos[AXIS_X] = slot ? Q16_ONE : -Q16_ONE;
    victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
    victim->health = 0;
    victim->action = 0x1009;
    fujin_first_fatality(owner);
    bool wind = false, bolt = false, debris = false;
    int bolts = 0;
    for (int tick = 0; tick < 700; ++tick) {
      battle_step((Input[2]){0});
      Pose pose[MK4_MAX_BONES];
      fighter_pose(assets, owner, pose);
      fighter_pose(assets, victim, pose);
      for (int i = 0; i < MAX_PROJECTILES; ++i) {
        const Projectile *object = &g_battle.projectiles[i];
        if (object->kind == 0 || object->mesh_animation == NULL) {
          continue;
        }
        const Geometry *geometry = mesh_animation_geometry(assets, object->mesh_animation);
        if (object->mesh_animation->first == 106) {
          bolt = true;
          bolts += object->ticks == 0;
          check(geometry == &assets->hud.effects, "Fujin fatality must fire the original shared106 crossbow bolt");
        }
      }
      for (int i = 0; i < MAX_PARTICLES; ++i) {
        const Particle *particle = &g_battle.particles[i];
        if (particle->ticks > 0 && particle->visual != NULL && particle->visual->animation.source == MESH_FUJIN_WIND) {
          wind = true;
          const Geometry *geometry = mesh_animation_geometry(assets, &particle->visual->animation);
          check(geometry == &assets->hud.fujin_wind && geometry->sprites.data[particle->visual->animation.first].w > 0,
              "Tornado must use its own sprite bank without occupying projectile slots");
        }
      }
      for (int i = 0; i < MAX_SCENE_PIECES; ++i) {
        const ScenePiece *piece = &g_battle.pieces[i];
        if (piece->active) {
          debris = true;
          check(piece->model == &assets->cut_characters.data[2] && (piece->scale == 0x1999 || piece->scale == 0x147a),
              "Explosive head/hand debris must preserve its closed source model and authored tiny scale");
        }
      }
      check(!fighter_fiber_active(owner) && !fighter_fiber_active(victim), "Real tornado and three-shot scene must stay on native frame/event programs");
    }
    check(wind && bolt && debris && g_battle.phase == PHASE_MATCH_OVER && owner->armed && owner->native_variant == 3 && bolts == 3,
        "Asset-backed Fujin first scene must retain wind, crossbow, three impacts and native debris");
    battle_reset_match();
  }
}

static void fujin_second_fatality_assets(const Assets *assets) {
  const Character *flesh = &assets->scene_models.data[SCENE_MODEL_FLAYED_BODY];
  check(flesh->bones.count >= 16 && flesh->meshes.data == assets->hud.effects2.meshes.data, "Wind victim must load the actual50c444 share2 flesh rig");
  for (int slot = 0; slot < 2; ++slot) {
    battle_init(slot ? 2 : 5, slot ? 5 : 2);
    battle_bind_assets(assets);
    g_battle.phase = PHASE_FINISH;
    g_battle.winner = slot;
    Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
    owner->body.pos[AXIS_X] = slot ? Q16_ONE : -Q16_ONE;
    victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
    victim->health = 0;
    victim->action = 0x1009;
    fujin_second_fatality(owner);
    bool skin = false, flayed = false, landing = false;
    for (int tick = 0; tick < 650; ++tick) {
      battle_step((Input[2]){0});
      Pose pose[MK4_MAX_BONES];
      fighter_pose(assets, owner, pose);
      fighter_pose(assets, victim, pose);
      flayed |= fighter_model(assets, victim) == flesh;
      landing |= fighter_move(victim)->clip == 550;
      for (int i = 0; i < MAX_SCENE_PIECES; ++i) {
        const ScenePiece *piece = &g_battle.pieces[i];
        if (piece->active) {
          skin = true;
          check(piece->model != NULL && piece->model->meshes.data == assets->hud.effects2.meshes.data && piece->bone == 0 &&
                    piece->model->meshes.data[piece->model->bones.data[0].mesh].triangles.count > 0 && piece->gravity == 0x1999 && piece->lifetime > 0 &&
                    piece->lifetime <= 45,
              "Torn skin must retain its actual4ea420 closed part and native forty-five-visit lifetime");
        }
      }
      check(!fighter_fiber_active(owner) && !fighter_fiber_active(victim), "Asset-backed wind fatality must never start fibers");
    }
    check(skin && flayed && landing && g_battle.phase == PHASE_MATCH_OVER && victim->hidden_joints[0] == 0,
        "Wind fatality must replace torn skin with a complete posed flesh body and land its550 ending");
    for (int i = 0; i < MAX_SCENE_PIECES; ++i) {
      check(!g_battle.pieces[i].active, "Wind skin fragments must retire after their native lifetime");
    }
    battle_reset_match();
  }
}

static void fujin_wind_grab_assets(const Assets *assets) {
  for (int slot = 0; slot < 2; ++slot) {
    for (int blocked = 0; blocked < 2; ++blocked) {
      battle_init(slot ? 2 : 5, slot ? 5 : 2);
      battle_bind_assets(assets);
      g_battle.phase = PHASE_FIGHT;
      Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
      owner->body.pos[AXIS_X] = slot ? 2 * Q16_ONE : -2 * Q16_ONE;
      victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
      fighter_interrupt(owner, fujin_wind_grab);
      Input input[2] = {0};
      input[slot].held = BUTTON_BIT(BUTTON_LOW_PUNCH) | BUTTON_BIT(slot ? BUTTON_LEFT : BUTTON_RIGHT);
      input[1 - slot].held = blocked ? BUTTON_BIT(BUTTON_BLOCK) : 0;
      bool caught = false, flight = false, visual = false, recoil = false;
      for (int tick = 0; tick < 280; ++tick) {
        battle_step(input);
        caught |= victim->action == 0x100d;
        flight |= fighter_move(victim)->clip == 183;
        recoil |= owner->action == 0x1015 && fighter_move(owner)->clip == 13;
        if (caught || recoil) {
          input[slot].held = BUTTON_BIT(BUTTON_LOW_PUNCH);
        }
        Pose bones[MK4_MAX_BONES];
        fighter_pose(assets, owner, bones);
        fighter_pose(assets, victim, bones);
        for (int i = 0; i < MAX_PROJECTILES; ++i) {
          const Projectile *effect = &g_battle.projectiles[i];
          if (effect->kind == PROJECTILE_EFFECT && effect->mesh_animation != NULL &&
              (effect->mesh_animation->first == 356 || effect->mesh_animation->first == 350)) {
            visual = true;
            const Geometry *geometry = mesh_animation_geometry(assets, effect->mesh_animation);
            check(geometry == &assets->hud.effects && geometry->sprites.data[effect->mesh_animation->first].w > 0,
                "Held wind-grab effect must retain its actual4d1a18 shared wind sprites");
          }
        }
        check(!fighter_fiber_active(owner) && !fighter_fiber_active(victim), "Real wind-grab opening, orbit and183 release must remain on frame phases");
      }
      check(visual && (blocked ? recoil && !caught : caught && flight), "Asset-backed wind grab must follow its own capture or blocked branch");
      check(victim->health == Q16_ONE - (blocked ? 0 : 0x1eb8) && !fighter_airborne(victim) && !(victim->flags & FF_UNTOUCHABLE),
          "Real wind-grab release must retain twelve-percent damage and restore the victim's contact and floor state");
      battle_reset_match();
    }
  }
}

static void fujin_triple_kick_assets(const Assets *assets) {
  for (int slot = 0; slot < 2; ++slot) {
    for (int blocked = 0; blocked < 2; ++blocked) {
      battle_init(slot ? 2 : 5, slot ? 5 : 2);
      battle_bind_assets(assets);
      g_battle.phase = PHASE_FIGHT;
      Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
      owner->body.pos[AXIS_X] = slot ? Q16_ONE : -Q16_ONE;
      victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
      fighter_interrupt(owner, fujin_triple_kick);
      Input input[2] = {0};
      input[1 - slot].held = blocked ? BUTTON_BIT(BUTTON_BLOCK) : 0;
      bool first = false, middle = false, last = false, recoil = false;
      for (int tick = 0; tick < 240; ++tick) {
        battle_step(input);
        Pose bones[MK4_MAX_BONES];
        fighter_pose(assets, owner, bones);
        fighter_pose(assets, victim, bones);
        first |= victim->reaction == 93 && fighter_move(victim)->clip == 127;
        middle |= victim->reaction == 94 && fighter_move(victim)->clip == 311;
        last |= victim->reaction == 95 && fighter_move(victim)->clip == 131;
        recoil |= owner->body.gravity == 0x147 && fighter_airborne(owner);
        check(!fighter_fiber_active(owner) && !fighter_fiber_active(victim), "Asset-backed triple kick and victim phases must use frame tables");
      }
      check(blocked ? recoil : first && middle && last, "Real triple-kick poses must follow actual contact or blocked-recoil branches");
      check(victim->health == Q16_ONE - (blocked ? q16_mul(0x30a3, 0x3333) : 0x30a3) && !fighter_airborne(owner) && !fighter_airborne(victim),
          "Real-asset triple kick must preserve damage and settle both bodies");
      battle_reset_match();
    }
  }
}

static void fujin_wind_assets(const Assets *assets) {
  for (int slot = 0; slot < 2; ++slot) {
    for (int slam = 0; slam < 2; ++slam) {
      battle_init(slot ? 2 : 5, slot ? 5 : 2);
      battle_bind_assets(assets);
      g_battle.phase = PHASE_FIGHT;
      Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
      owner->body.pos[AXIS_X] = slot ? 0x14ccc : -0x14ccc;
      victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
      fighter_interrupt(owner, fujin_wind);
      bool caught = false, emitted = false, slammed = false, bounce = false;
      for (int tick = 0; tick < 500; ++tick) {
        if (victim->action == 0x1001) {
          caught = true;
          if (slam && owner->state == S_IDLE && !slammed) {
            fighter_interrupt(owner, fujin_wind_slam);
            slammed = true;
          }
        }
        battle_step((Input[2]){0});
        bounce |= fighter_move(victim)->clip == 179;
        Pose bones[MK4_MAX_BONES];
        fighter_pose(assets, owner, bones);
        fighter_pose(assets, victim, bones);
        for (int i = 0; i < MAX_PARTICLES; ++i) {
          const Particle *p = &g_battle.particles[i];
          if (p->ticks > 0 && p->visual != NULL && p->visual->animation.first >= 84 && p->visual->animation.first <= 91) {
            emitted = true;
            int mesh = p->visual->animation.first;
            check(assets->hud.effects.meshes.data[mesh].triangles.count > 0 && assets->hud.effects.sprites.data[mesh].w == 0,
                "Wind emitter must use the actual three-dimensional shared84..91 mesh geometry");
          }
        }
        check(!fighter_fiber_active(owner) && !fighter_fiber_active(victim), "Real wind, slam and getup clips must remain on frame phases");
      }
      check(caught && emitted && (!slam || (slammed && bounce)) && victim->state == S_IDLE && victim->health == Q16_ONE - (slam ? 0x147a : 0),
          "Real-asset wind catch, optional slam and landing must complete with the authored damage");
      battle_reset_match();
    }
  }
}

static void fujin_crossbow_assets(const Assets *assets) {
  const Character *bow = &assets->scene_models.data[SCENE_MODEL_FUJIN_WEAPON];
  check(bow->bones.count == 1 && bow->bones.data[0].mesh == 105, "Fujin crossbow must use descriptor4ece98's original shared105 rig");
  for (int slot = 0; slot < 2; ++slot) {
    for (int shot = 0; shot < 3; ++shot) {
      battle_init(slot ? 4 : 5, slot ? 5 : 4);
      battle_bind_assets(assets);
      g_battle.phase = PHASE_FIGHT;
      Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
      owner->body.pos[AXIS_X] = slot ? 4 * Q16_ONE : -4 * Q16_ONE;
      victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
      fighter_interrupt(owner, fujin_weapon_action);
      for (int tick = 0; tick < 65; ++tick) {
        battle_step((Input[2]){0});
      }
      check(owner->armed && owner->weapon->model == SCENE_MODEL_FUJIN_WEAPON, "Asset-backed draw must equip Fujin's own crossbow");
      Input input[2] = {0};
      input[slot].held = BUTTON_BIT(shot == 1 ? BUTTON_HIGH_PUNCH : BUTTON_LOW_PUNCH) | (shot == 2 ? BUTTON_BIT(slot ? BUTTON_RIGHT : BUTTON_LEFT) : 0);
      bool bolt = false;
      for (int tick = 0; tick < 220; ++tick) {
        battle_step(input);
        input[slot].held = 0;
        Pose bones[MK4_MAX_BONES];
        fighter_pose(assets, owner, bones);
        fighter_pose(assets, victim, bones);
        check(!fighter_fiber_active(owner) && !fighter_fiber_active(victim), "Asset-backed crossbow action and victim must never use a fiber");
        for (int i = 0; i < MAX_PROJECTILES; ++i) {
          const Projectile *object = &g_battle.projectiles[i];
          if (object->kind != 0 && object->mesh_animation != NULL && object->mesh_animation->first == 106) {
            bolt = true;
            const Geometry *geometry = mesh_animation_geometry(assets, object->mesh_animation);
            check(geometry == &assets->hud.effects && geometry->meshes.data[106].triangles.count > 0 && geometry->sprites.data[106].w == 0,
                "Crossbow bolt must render its original three-dimensional shared106 geometry");
          }
        }
      }
      check(bolt && owner->state == S_IDLE && owner->armed, "All three asset-backed crossbow shots must launch their own bolt and recover");
      battle_reset_match();
    }
  }
}

static void independent_scene_poses(const Assets *assets) {
  const int sources[] = {4, 2};
  for (int variant = 0; variant < 2; ++variant) {
    battle_init(14, sources[variant]);
    battle_bind_assets(assets);
    g_battle.phase = PHASE_FIGHT;
    static const MoveDef pose = {.clip = 408, .last = 142};
    Fighter *actor = battle_scene_actor_start(&g_battle.fighters[0], &g_battle.fighters[1], &pose, scene_pose_entry);
    check(actor != NULL && actor->assets == assets, "Independent scene actor must borrow its source's decoded assets");
    for (int tick = 0; tick < 2; ++tick) {
      battle_step((Input[2]){0});
      Pose bones[MK4_MAX_BONES];
      fighter_pose(assets, actor, bones);
      const Character *model = fighter_model(assets, actor);
      for (size_t bone = 0; bone < model->bones.count; ++bone) {
        check(isfinite(bones[bone].position.x) && isfinite(bones[bone].position.y) && isfinite(bones[bone].position.z),
            "Independent male/female rig poses must remain finite without borrowing a player's animation snapshot");
      }
    }
    battle_reset_match();
    check(!g_battle.scene_actors[0].active, "Asset-backed independent scene actor must retire on reset");
  }
}

static void cage_first_fatality_assets(const Assets *assets) {
  for (int slot = 0; slot < 2; ++slot) {
    for (int female = 0; female < 2; ++female) {
      int enemy = female ? 2 : 4;
      battle_init(slot ? enemy : 14, slot ? 14 : enemy);
      battle_bind_assets(assets);
      g_battle.phase = PHASE_FIGHT;
      Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
      owner->wins = 1;
      battle_damage(victim, Q16_ONE);
      for (int tick = 0; tick < 180 && !battle_can_finish(owner); ++tick) {
        battle_step((Input[2]){0});
      }
      check(battle_can_finish(owner), "Asset-backed torso rip needs the deciding-KO window");
      owner->body.pos[AXIS_X] = slot ? 0x6666 : -0x6666;
      victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
      fighter_interrupt(owner, cage_fatality_1);
      bool detached = false;
      for (int tick = 0; tick < 1100 && g_battle.phase != PHASE_MATCH_OVER; ++tick) {
        battle_step((Input[2]){0});
        Pose bones[MK4_MAX_BONES];
        fighter_pose(assets, owner, bones);
        fighter_pose(assets, victim, bones);
        for (int i = 0; i < MAX_SCENE_ACTORS; ++i) {
          Fighter *body = &g_battle.scene_actors[i].actor;
          if (g_battle.scene_actors[i].active) {
            detached = true;
            fighter_pose(assets, body, bones);
            const Character *model = fighter_model(assets, body);
            for (size_t bone = 0; bone < model->bones.count; ++bone) {
              check(isfinite(bones[bone].position.x) && isfinite(bones[bone].position.y) && isfinite(bones[bone].position.z),
                  "Cage's detached male/female torso must decode finite independent rig poses");
            }
          }
        }
      }
      check(
          detached && owner->native_variant == 7 && g_battle.phase == PHASE_MATCH_OVER, "Cage's asset-backed torso and lower body must complete independently");
      battle_reset_match();
    }
  }
}

static void cage_second_fatality_assets(const Assets *assets) {
  for (int slot = 0; slot < 2; ++slot) {
    battle_init(slot ? 4 : 14, slot ? 14 : 4);
    battle_bind_assets(assets);
    g_battle.phase = PHASE_FIGHT;
    Fighter *owner = &g_battle.fighters[slot], *victim = &g_battle.fighters[1 - slot];
    owner->wins = 1;
    battle_damage(victim, Q16_ONE);
    for (int tick = 0; tick < 180 && !battle_can_finish(owner); ++tick) {
      battle_step((Input[2]){0});
    }
    check(battle_can_finish(owner), "Asset-backed Cage fatality needs its actual KO window");
    owner->body.pos[AXIS_X] = slot ? 0x6666 : -0x6666;
    victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
    fighter_interrupt(owner, cage_fatality_2);
    bool charge = false, head = false, fall = false;
    for (int tick = 0; tick < 450 && g_battle.phase != PHASE_MATCH_OVER; ++tick) {
      battle_step((Input[2]){0});
      Pose bones[MK4_MAX_BONES];
      fighter_pose(assets, owner, bones);
      fighter_pose(assets, victim, bones);
      check(!fighter_fiber_active(owner) && !fighter_fiber_active(victim), "Asset-backed Cage fatality must use independent frame phases");
      fall |= fighter_move(victim)->clip == 206;
      for (int object = 0; object < MAX_PROJECTILES; ++object) {
        const Projectile *effect = &g_battle.projectiles[object];
        if (effect->kind == PROJECTILE_EFFECT && effect->mesh_animation != NULL) {
          int frame = projectile_mesh_frame(effect);
          charge |= effect->mesh_animation->source == MESH_SHARED2 && frame >= 271 && frame <= 280;
          const Geometry *geometry = mesh_animation_geometry(assets, effect->mesh_animation);
          check(geometry->sprites.data[frame].w > 0, "Cage charge must render its actual share2 sprite geometry");
        }
      }
      for (int piece = 0; piece < MAX_SCENE_PIECES; ++piece) {
        head |= g_battle.pieces[piece].active && g_battle.pieces[piece].roster == victim->roster;
      }
    }
    check(
        charge && head && fall && g_battle.phase == PHASE_MATCH_OVER, "Cage fatality must show its charge and detached victim head, then finish its collapse");
    battle_reset_match();
    check(victim->hidden_joints[0] == 0, "Cage asset-backed fatality must restore the victim rig on rematch");
  }
}

static void cage_groin_command(const Assets *assets) {
  for (int slot = 0; slot < 2; ++slot) {
    for (int scenario = 0; scenario < 5; ++scenario) {
      int enemy = scenario == 3 ? 2 : 0;
      battle_init(slot ? enemy : 14, slot ? 14 : enemy);
      g_battle.phase = PHASE_FIGHT;
      Fighter *owner = &g_battle.fighters[slot];
      Fighter *victim = &g_battle.fighters[1 - slot];
      q16 distance = scenario == 2 ? 5 * Q16_ONE : Q16_ONE / 2;
      owner->body.pos[AXIS_X] = slot ? distance / 2 : -distance / 2;
      victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
      battle_bind_assets(assets);
      Input input[2] = {0};
      input[slot].held = BUTTON_BIT(BUTTON_LOW_PUNCH) | BUTTON_BIT(BUTTON_BLOCK);
      if (scenario == 1) {
        input[1 - slot].held = BUTTON_BIT(BUTTON_BLOCK);
      }
      battle_step(input);
      input[slot].held = 0;
      bool selected = false, stunned = false;
      for (int tick = 0; tick < 180; ++tick) {
        battle_step(input);
        if (scenario == 4 && tick == 20) {
          fighter_enter_state(owner, S_HIT);
          check(!(owner->flags & FF_NO_GUARD), "Interrupting groin recovery must immediately remove its guard lock");
        }
        selected |= fighter_move(owner)->clip == 438;
        stunned |= fighter_move(victim)->clip == 126 && victim->action == 0x1014;
        check(!fighter_fiber_active(owner) && !fighter_fiber_active(victim), "Cage groin punch and its selected stun must use frame tables");
      }
      q16 damage = (scenario == 0 || scenario == 4) ? 0x28f5 : scenario == 1 ? q16_mul(0x28f5, 0x3333) : 0;
      if (victim->health != Q16_ONE - damage) {
        fprintf(stderr, "Cage groin slot%d scenario%d health%d expected%d selected%d stun%d\n", slot, scenario, victim->health, Q16_ONE - damage, selected,
            stunned);
      }
      check(selected && victim->health == Q16_ONE - damage, "LP+BLOCK must select the native groin attack and its hit/block/miss/female branch");
      check((scenario != 0 && scenario != 4) || stunned, "Successful groin contact must run the selected native stun, not an approximate reaction");
      check(owner->state == S_IDLE && !(owner->flags & FF_NO_GUARD), "Cage groin recovery must return controls and restore guarding");
    }
  }
}

int main(void) {
  sound_samples();

  struct Fixture {
    int kind;
    Vec3 angles;

    struct {
      int values[9];
    } q12;
  };
  const struct Fixture fixtures[] = {
#include "fixtures/original_matrices.inc"
  };
  for (size_t f = 0; f < sizeof fixtures / sizeof *fixtures; ++f) {
    Mat3 m = rotation(fixtures[f].kind, fixtures[f].angles);
    for (size_t i = 0; i < 9; ++i)
      check(fabsf(m.m[i] - (float)fixtures[f].q12.values[i] / 4096) < .002f, "Rotation differs from original x86 builder");
  }
  Assets assets = {0};
  assets_load(&assets, "MK4/FILESYS.DAT", "MK4/MK-4.EXE");
  check(assets.characters.count == 18 && assets.clips.count == 612, "Incomplete roster/pack");
  size_t frames = 0, poses = 0;
  for (size_t i = 0; i < assets.clips.count; ++i)
    frames += assets.clips.data[i].frames.count;
  check(frames == 36723, "Unexpected animation frame count");
  for (size_t c = 0; c < assets.characters.count; ++c) {
    const Character *ch = &assets.characters.data[c];
    const Clip *clip = &assets.clips.data[ch->idle];
    bool channels[MK4_MAX_BONES] = {0};
    int channel_count = 0, head = -1, left = -1, right = -1, count = (int)clip->frames.data[0].angles.count;
    for (size_t i = 0; i < ch->bones.count; ++i) {
      const Bone *b = &ch->bones.data[i];
      check(b->parent < (int)i, "Skeleton is not in parent order");
      check(b->kind == (b->parent < 0 ? 1 : 2), "Missing character transform initialization pass");
      if (b->channel >= 0) {
        check(b->channel < count && b->channel < MK4_MAX_BONES, "Animation channel out of range");
        check(!channels[b->channel], "Duplicate animation channel");
        channels[b->channel] = true;
        ++channel_count;
      }
      if (b->channel == 0)
        head = (int)i;
      if (b->channel == 13)
        left = (int)i;
      if (b->channel == 14)
        right = (int)i;
    }
    check(channel_count == count, "Animation/skeleton channels differ");
    for (int i = 0; i < count; ++i)
      check(channels[i], "Missing animation channel");
    check(head >= 0 && left >= 0 && right >= 0, "Missing head/feet");
    for (size_t f = 0; f < clip->frames.count; ++f) {
      Pose p[MK4_MAX_BONES];
      pose(ch, clip, (double)f, p);
      ++poses;
      float feet = (p[left].position.y + p[right].position.y) * .5f;
      if (feet - p[head].position.y <= .8f)
        fatal("%s frame %zu head/feet height %f", ch->name, f, (double)(feet - p[head].position.y));
      for (size_t i = 0; i < ch->bones.count; ++i) {
        const Pose *bone = &p[i];
        check(isfinite(bone->position.x + bone->position.y + bone->position.z), "Invalid posed position");
        for (int row = 0; row < 3; ++row)
          for (int col = 0; col < 3; ++col) {
            float dot = 0;
            for (int k = 0; k < 3; ++k)
              dot += bone->rotation.m[row * 3 + k] * bone->rotation.m[col * 3 + k];
            check(fabsf(dot - (row == col ? 1.f : 0.f)) < .0001f, "Joint matrix is not orthonormal");
          }
      }
    }
    Pose first[MK4_MAX_BONES], loop[MK4_MAX_BONES];
    pose(ch, clip, 0, first);
    pose(ch, clip, (double)clip->frames.count, loop);
    for (size_t i = 0; i < ch->bones.count; ++i)
      for (size_t k = 0; k < 9; ++k)
        check(first[i].rotation.m[k] == loop[i].rotation.m[k], "Clip does not wrap at its frame count");
    {
      float sole0 = mesh_sole(ch, clip, 0);
      float sole1 = mesh_sole(ch, clip, (double)clip->frames.count - 1);
      if (fabsf(sole0) > .05f || fabsf(sole1) > .05f)
        fatal("%s idle sole %f %f leaves the floor", ch->name, (double)sole0, (double)sole1);
    }
  }
  /* Endpoints of every grounded roster move. Jump is airborne. Fujin's
   * settled knockdown is his mesh on shared clip 186 (about +0.21). Goro's
   * clip 581 opens about +0.49 below the floor; its root Y plants the end. */
  for (int r = 0; r < roster_count(); ++r) {
    const CharacterDef *def = roster_def(r);
    const Character *fighter = roster_character(&assets, r);
    const Clip *idle = &assets.clips.data[fighter->idle];
    int channels = (int)idle->frames.data[0].angles.count;
    for (int state = 0; state < S_COUNT; ++state) {
      const MoveDef *move;
      const Clip *ground;
      int frames, first, last, point;
      if (state == S_JUMP)
        continue;
      move = def->moves[state];
      ground = &assets.clips.data[roster_clip(def, state, fighter->idle)];
      check((int)ground->frames.data[0].angles.count == channels, "Roster clip does not match the rig");
      frames = (int)ground->frames.count;
      first = move->first < frames ? move->first : frames - 1;
      last = move->last == MOVE_END ? frames - 1 : move->last;
      if (last >= frames)
        last = frames - 1;
      for (point = 0; point < 2; ++point) {
        int frame = point == 0 ? first : last;
        float sole = mesh_sole(fighter, ground, (double)frame);
        float plant = ground->frames.data[frame].translation.y;
        int fujin_settle = !strcmp(def->name, "FUJIN") && state == S_KNOCKDOWN && point == 1;
        int goro_open = !strcmp(def->name, "GORO") && state == S_KNOCKDOWN && point == 0;
        if (fujin_settle) {
          if (sole < .15f || sole > .30f)
            fatal("%s knockdown sole %f left the known mesh overshoot", def->name, (double)sole);
        } else if (goro_open) {
          if (sole < .40f || sole > .55f)
            fatal("%s knockdown opens at sole %f", def->name, (double)sole);
        } else if (fabsf(sole) > .15f)
          fatal("%s %s clip %d frame %d sole %f leaves the floor", def->name, state_name(state), ground->id, frame, (double)sole);
        if (!goro_open && fabsf(plant) > .3f && fabsf(sole - plant) <= fabsf(sole) + .25f)
          fatal("%s %s clip %d frame %d root Y %f is not the ground plant", def->name, state_name(state), ground->id, frame, (double)plant);
      }
    }
  }
  /* 489ff0 selector8: Liu remap1 reads token1275 -> pcmk0255.esf.
   * This catches accidental routing through indexed sounds (opcode25). */
  assets_load_battle(&assets, "MK4/FILESYS.DAT", "MK4/MK-4.EXE");
  cross_clip_pose_interpolation(&assets);
  fighter_render_snapshots(&assets);
  fallback_clip_clock(&assets);
  axe_followup_contacts(&assets);
  kai_weapon_contacts(&assets);
  cage_weapon_contacts(&assets);
  jarek_weapon_contacts(&assets);
  shinnok_weapon_contacts(&assets);
  restored_high_punch_contacts(&assets);
  cage_orb_assets(&assets);
  cage_groin_command(&assets);
  cage_first_fatality_assets(&assets);
  cage_second_fatality_assets(&assets);
  fujin_first_fatality_assets(&assets);
  fujin_second_fatality_assets(&assets);
  fujin_wind_grab_assets(&assets);
  fujin_triple_kick_assets(&assets);
  fujin_wind_assets(&assets);
  fujin_crossbow_assets(&assets);
  independent_scene_poses(&assets);
  const Sound *throw_voice = sound_event(&assets.audio, SOUND_THROW_VOICE, 3);
  check(throw_voice != NULL && throw_voice->id == 255, "Roster throw voice must resolve through the native list remap");
  combat_poses(&assets);
  assets_free(&assets);
  check(!assets.characters.data && !assets.clips.data, "Asset ownership was not released");
  printf("PASS: %zu original matrix fixtures; %zu upright idle poses; 18 complete rigs; 612 clips / %zu frames\n", sizeof fixtures / sizeof *fixtures, poses,
      frames);
  return 0;
}
