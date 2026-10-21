#include "game.h"
#include "retail_vm.h"

/* Playback records for native scene continuations. Independent scene actors
 * can also resume their retail script through the shared interpreter. */
static void begin_clip(Fighter *fighter) {
  const SceneClip *clip = &fighter->scene.clips[fighter->scene.index];
  int source = fighter->assets != NULL ? fighter_clip(fighter->assets, fighter)->id : fighter_move(fighter)->clip;
  int sample = fighter_pose_sample(fighter);
  fighter->current_move = &clip->move;
  fighter->sample_override = clip->first;
  fighter->blend_duration = 0;
  fighter->blend_weight = -1;
  if (clip->blend_step > 0) {
    fighter->blend_clip = source;
    fighter->blend_sample = sample;
    fighter->blend_duration = 1;
    fighter->blend_weight = clip->blend_first;
  }
}

void scene_begin(Fighter *fighter, const SceneClip *clips, int count, int loop_from) {
  require(clips != NULL && count > 0 && loop_from < count, "Invalid scene playback");
  fighter->scene = (ScenePlayback){.clips = clips, .count = count, .loop_from = loop_from};
  begin_clip(fighter);
}

bool scene_step(Fighter *fighter) {
  ScenePlayback *scene = &fighter->scene;
  if (scene->done || scene->clips == NULL) {
    return true;
  }
  const SceneClip *clip = &scene->clips[scene->index];
  if (clip->blend_step > 0) {
    fighter->blend_weight += clip->blend_step;
    if (fighter->blend_weight <= Q16_ONE) {
      return false;
    }
    fighter->sample_override = clip->last;
    fighter->blend_duration = 0;
    fighter->blend_weight = -1;
  } else if (fighter->sample_override != clip->last) {
    fighter->sample_override += clip->last >= clip->first ? 1 : -1;
    return false;
  }
  if (++scene->repeat < clip->repeats) {
    fighter->sample_override = clip->first;
    return false;
  }
  scene->repeat = 0;
  if (++scene->index == scene->count) {
    if (scene->loop_from < 0) {
      scene->index--;
      scene->done = true;
      return true;
    }
    scene->index = scene->loop_from;
  }
  begin_clip(fighter);
  return false;
}

int scene_frame_count(const SceneClip *clips, int count) {
  int frames = 0;
  for (int index = 0; index < count; ++index) {
    const SceneClip *clip = &clips[index];
    int span = clip->last >= clip->first ? clip->last - clip->first : clip->first - clip->last;
    int visits = clip->blend_step > 0 ? (Q16_ONE - clip->blend_first) / clip->blend_step + 1 : span + 1;
    frames += visits * clip->repeats;
  }
  return frames;
}

/* Blend boundaries are selected by authored mframe rows, never by testing
 * a live weight or an elapsed counter. */
void scene_blend_advance(Fighter *fighter) {
  ScenePlayback *scene = &fighter->scene;
  fighter->sample_override = scene->clips[scene->index].last;
  fighter->blend_duration = 0;
  fighter->blend_weight = -1;
  if (++scene->index == scene->count) {
    scene->index--;
    scene->done = true;
    scene->clips = NULL;
  } else {
    begin_clip(fighter);
  }
}

bool battle_is_scene_actor(const Fighter *actor) {
  for (int i = 0; i < MAX_SCENE_ACTORS; ++i) {
    if (actor == &g_battle.scene_actors[i].actor) {
      return true;
    }
  }
  return false;
}

static SceneActor *scene_slot(Fighter *actor) {
  for (int i = 0; i < MAX_SCENE_ACTORS; ++i) {
    if (actor == &g_battle.scene_actors[i].actor) {
      return &g_battle.scene_actors[i];
    }
  }
  require(false, "Unknown independent scene actor");
  return NULL;
}

static Fighter *scene_actor_allocate(Fighter *owner, const Fighter *source, const MoveDef *move) {
  require((owner == &g_battle.fighters[0] || owner == &g_battle.fighters[1]) && (source == &g_battle.fighters[0] || source == &g_battle.fighters[1]),
      "Scene actor needs live owner and source fighters");
  require(move != NULL, "Scene actor needs a pose");
  for (int i = 0; i < MAX_SCENE_ACTORS; ++i) {
    SceneActor *slot = &g_battle.scene_actors[i];
    if (slot->active) {
      continue;
    }
    memset(slot, 0, sizeof *slot);
    slot->active = true;
    slot->owner = owner == &g_battle.fighters[0] ? 0 : 1;
    slot->source = source == &g_battle.fighters[0] ? 0 : 1;
    slot->move = *move;
    slot->move.flags |= SF_LOCK | SF_RECORD;
    Fighter *actor = &slot->actor;
    actor->assets = source->assets;
    actor->roster = source->roster;
    actor->facing = source->facing;
    memcpy(actor->heading, source->heading, sizeof actor->heading);
    actor->body = source->body;
    memset(actor->body.vel, 0, sizeof actor->body.vel);
    memset(actor->body.spin, 0, sizeof actor->body.spin);
    actor->body.gravity = 0;
    actor->body.root_y = 0;
    actor->body.walking = false;
    actor->body.physics = actor->body.on_land = NULL;
    actor->current_move = &slot->move;
    actor->sample_override = move->first;
    actor->blend_weight = -1;
    actor->state = S_KNOCKDOWN;
    actor->action = move->action;
    actor->flags = FF_BUSY | FF_UNTOUCHABLE;
    return actor;
  }
  return NULL;
}

Fighter *battle_scene_actor_start(Fighter *owner, const Fighter *source, const MoveDef *move, FighterFn entry) {
  require(entry != NULL, "Scene actor needs a frame entry");
  Fighter *actor = scene_actor_allocate(owner, source, move);
  if (actor != NULL) {
    entry(actor);
  }
  return actor;
}

Fighter *battle_scene_actor_start_script(Fighter *owner, const Fighter *source, const MoveDef *move, uint32_t entry, FighterFn finish, unsigned native_owner) {
  Fighter *actor = scene_actor_allocate(owner, source, move);
  if (actor != NULL) {
    retail_vm_enter_owned(actor, entry, finish, native_owner);
  }
  return actor;
}

void battle_scene_actor_stop(Fighter *actor) {
  SceneActor *slot = scene_slot(actor);
  fighter_cancel_mmove(actor);
  slot->active = false;
}

void battle_step_scene_actors(void) {
  for (int i = 0; i < MAX_SCENE_ACTORS; ++i) {
    SceneActor *slot = &g_battle.scene_actors[i];
    if (!slot->active) {
      continue;
    }
    Fighter *actor = &slot->actor;
    if (actor->retail_vm.active) {
      retail_vm_step(actor);
    } else if (actor->current_mmove != NULL) {
      fighter_step_mmove(actor);
    }
    if (!slot->active) {
      continue;
    }
    Body *body = &actor->body;
    if (body->physics != NULL) {
      body->physics(actor);
    }
    body->vel[AXIS_Y] += body->gravity;
    for (int axis = 0; axis < 3; ++axis) {
      body->pos[axis] += body->vel[axis];
    }
    if (body->gravity > 0 && body->vel[AXIS_Y] >= 0 && body->pos[AXIS_Y] >= body->ground) {
      body->pos[AXIS_Y] = body->ground;
      memset(body->vel, 0, sizeof body->vel);
      body->gravity = 0;
      FighterFn land = body->on_land;
      body->on_land = NULL;
      if (land != NULL) {
        land(actor);
      }
    }
  }
}
