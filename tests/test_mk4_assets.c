#include "game.h"
#include "mk4_assets.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
  Battle battle;
  battle_init(&battle, 4, 15);
  battle_bind_assets(&battle, assets);
  battle.phase = PHASE_FIGHT;
  Fighter *sub_zero = &battle.fighters[0];
  sub_zero->ticks = 70;
  check(fighter_pose_sample(sub_zero) == 0, "Pose snapshot must wrap the real 35-sample idle");
  battle_react(&battle, sub_zero, 67);
  battle_step(&battle, (Input[2]){0});
  check(sub_zero->sample_override == 0 && sub_zero->frozen_clip == CLIP_ASSET_IDLE, "Freeze must preserve the displayed looping pose");
  Fighter *goro = &battle.fighters[1];
  battle_react(&battle, goro, 67);
  battle_step(&battle, (Input[2]){0});
  check(goro->appearance == APPEARANCE_FROZEN && fighter_clip(assets, goro)->id == 587, "Boss freeze must retain its own rig and pose");
  for (int i = 0; i < 143; ++i) {
    battle_step(&battle, (Input[2]){0});
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

  battle_init(&battle, 4, 0);
  battle_bind_assets(&battle, assets);
  battle.phase = PHASE_FIGHT;
  const AttackRecord strike = {0x18000, -0x10000, 0x18000, 0x18000, 37, 11, 1, 0, 0x1000};
  for (int rotated = 0; rotated < 2; ++rotated) {
    battle_reset_match(&battle);
    Fighter *a = &battle.fighters[0], *b = &battle.fighters[1];
    a->body.pos[AXIS_X] = 0;
    b->body.pos[AXIS_X] = rotated ? 0 : Q16_ONE;
    b->body.pos[AXIS_Z] = rotated ? Q16_ONE : 0;
    battle_world(&battle);
    check(battle_contact(&battle, a, &strike) == CONTACT_HIT, "Joint contact must follow the pair axis after a quarter turn");
    check(b->health == Q16_ONE - 0x1000, "Pose contact must retain record damage");
  }
  battle_reset_match(&battle);
  Fighter *victim = &battle.fighters[1];
  victim->appearance = APPEARANCE_FROZEN;
  battle_shatter(&battle, victim);
  int pieces = 0;
  for (int i = 0; i < MAX_SCENE_PIECES; ++i) {
    pieces += battle.pieces[i].active;
    check(!battle.pieces[i].active || battle.pieces[i].frozen, "Shatter pieces must preserve the frozen material");
  }
  check(pieces >= 15 && victim->appearance == APPEARANCE_HIDDEN, "Shatter must detach posed meshes before hiding the intact body");
  q16 height = battle.pieces[0].pos[AXIS_Y];
  battle_step_presentation(&battle);
  check(battle.pieces[0].pos[AXIS_Y] < height, "Detached pieces must follow their native launch vectors");
  battle_reset_match(&battle);
  check(!battle.pieces[0].active && battle.fighters[1].hidden_joints[0] == 0 && !battle.camera.active, "Rematch must clear scene objects and mesh visibility");
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
  combat_poses(&assets);
  assets_free(&assets);
  check(!assets.characters.data && !assets.clips.data, "Asset ownership was not released");
  printf("PASS: %zu original matrix fixtures; %zu upright idle poses; 18 complete rigs; 612 clips / %zu frames\n", sizeof fixtures / sizeof *fixtures, poses,
      frames);
  return 0;
}
