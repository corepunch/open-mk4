#include "game.h"
#include "mk4_program.h"
#include "native/actions.h"
#include "native/capabilities.h"
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

static void fighter_facing_reflection(const Assets *assets) {
  battle_init(&assets->image, 0, 0);
  battle_bind_assets(assets);
  Fighter right = g_battle.fighters[0];
  right.body.angle[0] = Q16_ONE / 3;
  right.body.angle[2] = -Q16_ONE / 5;
  Pose nodes[MK4_MAX_BONES];
  fighter_pose(assets, &right, nodes);
  const Character *model = fighter_model(assets, &right);
  const q16 axes[][2] = {{Q16_ONE, 0}, {3 * Q16_ONE / 5, 4 * Q16_ONE / 5}, {0, Q16_ONE}};
  for (size_t a = 0; a < sizeof axes / sizeof *axes; ++a) {
    right.heading[0] = axes[a][0];
    right.heading[1] = axes[a][1];
    Fighter left = right;
    left.facing = -1;
    left.heading[0] = -right.heading[0];
    left.heading[1] = -right.heading[1];
    Mat3 rt = fighter_turn(assets, &right), lt = fighter_turn(assets, &left);
    float x = q16_float(axes[a][0]), z = q16_float(axes[a][1]);
    float length = sqrtf(x * x + z * z);
    x /= length;
    z /= length;
    for (size_t i = 0; i < model->bones.count; ++i) {
      Vec3 r = mat3_apply(rt, nodes[i].position), l = mat3_apply(lt, nodes[i].position);
      check(fabsf(l.x * x + l.z * z + r.x * x + r.z * z) < .0001f, "Facing must reflect joint positions along the fight axis");
      check(fabsf(-l.x * z + l.z * x + r.x * z - r.z * x) < .0001f, "Facing must preserve stance depth as the arena rotates");
      check(fabsf(l.y - r.y) < .0001f, "Facing must preserve joint height with body rotation");
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
  battle_init(&assets->image, 0, 0);
  battle_bind_assets(assets);
  const InputFrame idle[2] = {0};
  battle_step(idle);
  battle_step(idle);
  check(g_battle.current_animation.valid && g_battle.current_animation.tick == g_battle.ticks && g_battle.previous_animation.tick == 0,
      "Simulation ticks must publish animation history without drawing");
  battle_reset_match();
  check(g_battle.current_animation.valid && g_battle.current_animation.tick == 0 && g_battle.previous_animation.tick == 0,
      "Reset must replace both animation snapshots");
  battle_init(&assets->image, 0, 0);
  Fighter *fighter = &g_battle.fighters[0];
  action_uppercut(fighter);
  battle_bind_assets(assets);
  Mat3 source_turn = fighter_turn(assets, fighter);
  action_high_kick(fighter);
  Mat3 target_turn = fighter_turn(assets, fighter);
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
  fighter_render_pose(&g_battle, assets, fighter, 0.0, rendered);
  for (size_t i = 0; i < model->bones.count; ++i) {
    check(fabsf(rendered[i].position.x - source[i].position.x) < .0001f, "Render snapshot starts from the previous clip pose");
  }
  fighter_render_pose(&g_battle, assets, fighter, 1.0, rendered);
  for (size_t i = 0; i < model->bones.count; ++i) {
    check(fabsf(rendered[i].position.x - target[i].position.x) < .0001f, "Render snapshot reaches the new clip pose");
  }
  Mat3 render_source = fighter_render_turn(&g_battle, assets, fighter, 0.0);
  Mat3 render_target = fighter_render_turn(&g_battle, assets, fighter, 1.0);
  for (size_t k = 0; k < 9; ++k) {
    check(fabsf(render_source.m[k] - source_turn.m[k]) < .0001f, "Clip orientation must stay with the source pose at transition start");
    check(fabsf(render_target.m[k] - target_turn.m[k]) < .0001f, "Clip orientation must reach the target with its pose");
  }
  fighter_render_pose(&g_battle, assets, fighter, 0.5, rendered);
  for (size_t i = 0; i < model->bones.count; ++i) {
    check(isfinite(rendered[i].position.x) && isfinite(rendered[i].rotation.m[0]), "Cross-animation render snapshot must remain finite");
  }
  /* A loop or script branch selects endpoints, not all intervening samples.
   * Exercise both directions inside one clip, including its wrap boundary. */
  FighterAnimationFrame *previous = &g_battle.previous_animation.fighters[0];
  FighterAnimationFrame *current = &g_battle.current_animation.fighters[0];
  current->clip = previous->clip;
  const Clip *clip = &assets->clips.data[current->clip];
  const int samples[][2] = {{(int)clip->frames.count - 1, 0}, {0, (int)clip->frames.count - 1}, {1, 7}};
  for (size_t pair = 0; pair < sizeof samples / sizeof *samples; ++pair) {
    previous->sample = samples[pair][0];
    current->sample = samples[pair][1];
    pose_frame_sample(clip, previous->sample, &previous->pose);
    pose_frame_sample(clip, current->sample, &current->pose);
    pose_blend_frames(model, clip, previous->sample, clip, current->sample, .5f, target);
    fighter_render_pose(&g_battle, assets, fighter, .5, rendered);
    for (size_t i = 0; i < model->bones.count; ++i) {
      check(fabsf(rendered[i].position.x - target[i].position.x) < .0001f && fabsf(rendered[i].position.y - target[i].position.y) < .0001f &&
                fabsf(rendered[i].position.z - target[i].position.z) < .0001f,
          "Same-clip loops and branches must blend selected poses without visiting skipped samples");
      for (size_t k = 0; k < 9; ++k) {
        check(fabsf(rendered[i].rotation.m[k] - target[i].rotation.m[k]) < .0001f, "Same-clip transitions must interpolate endpoint joint angles");
      }
    }
  }
}

static void interrupted_blend_history(const Assets *assets) {
  for (int walking = 0; walking < 2; ++walking) {
    battle_init(&assets->image, 0, 0);
    battle_bind_assets(assets);
    Fighter *fighter = &g_battle.fighters[0];
    action_high_kick(fighter);
    fighter->sample_override = 12;
    mk4_return_to_stance(fighter);
    Pose expected[MK4_MAX_BONES], rendered[MK4_MAX_BONES];
    fighter_pose(assets, fighter, expected);
    g_battle.ticks = 2;
    battle_capture_render_snapshot();
    if (walking) {
      fighter->held = BUTTON_BIT(BUTTON_RIGHT);
      mk4_walk(fighter);
    } else {
      action_jab(fighter);
    }
    check(fighter->blend_duration == 0, "New movement or attack must interrupt stance blending");
    g_battle.ticks = 4;
    battle_capture_render_snapshot();
    fighter_render_pose(&g_battle, assets, fighter, 0., rendered);
    const Character *model = fighter_model(assets, fighter);
    for (size_t i = 0; i < model->bones.count; ++i) {
      check(fabsf(rendered[i].position.x - expected[i].position.x) < .0001f && fabsf(rendered[i].position.y - expected[i].position.y) < .0001f &&
                fabsf(rendered[i].position.z - expected[i].position.z) < .0001f,
          "Interrupted blend history must preserve the displayed pose, not flash the unblended idle target");
      for (size_t k = 0; k < 9; ++k) {
        check(fabsf(rendered[i].rotation.m[k] - expected[i].rotation.m[k]) < .0001f, "Interrupted blend history must preserve displayed joint angles");
      }
    }
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

static const mframe_t scene_pose_rows[3] = {{0}};
static const mmove_t scene_pose_phase = {.firstframe = 0, .lastframe = 2, .frame = scene_pose_rows, .endfunc = battle_scene_actor_stop};

static void scene_pose_entry(Fighter *actor) {
  fighter_mmove_phase(actor, &scene_pose_phase);
}

static void independent_scene_poses(const Assets *assets) {
  const int sources[] = {4, 2};
  for (int variant = 0; variant < 2; ++variant) {
    battle_init(&assets->image, 14, sources[variant]);
    battle_bind_assets(assets);
    g_battle.phase = PHASE_FIGHT;
    static const MoveDef pose = {.clip = 408, .last = 142};
    Fighter *actor = battle_scene_actor_start(&g_battle.fighters[0], &g_battle.fighters[1], &pose, scene_pose_entry);
    check(actor != NULL && actor->assets == assets, "Independent scene actor must borrow its source's decoded assets");
    for (int tick = 0; tick < 2; ++tick) {
      battle_step((InputFrame[2]){0});
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
  assets_load(&assets, "MK4/FILESYS.DAT", "build/mk4.data");
  battle_init(&assets.image, 0, 4);
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
  /* 489ff0 selector8: Liu remap1 reads token1275 -> pcmk0255.esf.
   * This catches accidental routing through indexed sounds (opcode25). */
  assets_load_battle(&assets, "MK4/FILESYS.DAT");
  fighter_facing_reflection(&assets);
  cross_clip_pose_interpolation(&assets);
  fighter_render_snapshots(&assets);
  interrupted_blend_history(&assets);
  independent_scene_poses(&assets);
  const Sound *throw_voice = sound_event(&assets.audio, SOUND_THROW_VOICE, 3);
  check(throw_voice != NULL && throw_voice->id == 255, "Roster throw voice must resolve through the native list remap");
  assets_free(&assets);
  check(!assets.characters.data && !assets.clips.data, "Asset ownership was not released");
  printf("PASS: %zu original matrix fixtures; %zu upright idle poses; 18 complete rigs; 612 clips / %zu frames\n", sizeof fixtures / sizeof *fixtures, poses,
      frames);
  return 0;
}
