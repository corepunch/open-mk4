#include "game.h"

/* D[0] is the low-punch contact row selected by the paired shadow task. */
static const AttackRecord shadow_contact = {0x14f5c, -0xf5c, 0xf5c2, 0x51eb, 82, 82, 0, 0, 0x2666};

static bool shadow_actor_step(BattleEvent *event, Fighter *owner, Fighter *opponent) {
  int variant = event->data[SHADOW_DATA_VARIANT];
  int age = (event->phase & 0xff) + 1;
  event->phase = (event->phase & ~0xff) | age;

  if (variant == SHADOW_ACTOR_LOW) {
    event->data[SHADOW_DATA_X] = opponent->body.pos[AXIS_X];
    event->data[SHADOW_DATA_Y] = opponent->body.pos[AXIS_Y];
    event->data[SHADOW_DATA_Z] = opponent->body.pos[AXIS_Z];
    event->data[SHADOW_DATA_SAMPLE] = fighter_pose_sample(owner);

    int sample = fighter_pose_sample(owner);
    if (sample >= 11 && sample <= 13) {
      ContactResult result = battle_contact_from(owner, &shadow_contact, owner->body.pos, owner->facing);
      if (result != CONTACT_MISS) {
        fighter_note_contact(owner, result);
        return false;
      }
    }
    return sample < 13 && age < 20;
  }

  event->data[SHADOW_DATA_SAMPLE] = age < 5 ? age : 4;
  event->data[SHADOW_DATA_Z] += 0x28f;
  event->data[SHADOW_DATA_Y] += event->data[SHADOW_DATA_VELOCITY_Y];

  if (event->phase & SHADOW_PHASE_PHYSICS) {
    event->data[SHADOW_DATA_VELOCITY_Y] += 0x20c;
  }

  int sample = fighter_pose_sample(owner);
  if (sample >= 5 && sample <= 9) {
    ContactResult result = battle_contact_from(owner, &shadow_contact, owner->body.pos, owner->facing);
    if (result != CONTACT_MISS) {
      fighter_note_contact(owner, result);
      return false;
    }
  }

  return age < 24 && (sample < 9 || !(event->phase & SHADOW_PHASE_PHYSICS));
}

bool battle_shadow_actor_event(const BattleEvent *event) {
  return event != NULL && event->active && event->step == shadow_actor_step && event->data[SHADOW_DATA_KIND] == BATTLE_EVENT_SHADOW_ACTOR;
}

void battle_shadow_actor_start(Fighter *owner, int variant) {
  require(variant == SHADOW_ACTOR_LOW || variant == SHADOW_ACTOR_UP, "Invalid shadow actor variant");
  require(owner->action == (variant == SHADOW_ACTOR_LOW ? 0x309 : 0x30a), "Shadow actor action does not match its move");

  BattleEvent *event = battle_event_start(owner, shadow_actor_step, true, true);
  Fighter *opponent = (Fighter *)fighter_opponent(owner);
  event->data[SHADOW_DATA_KIND] = BATTLE_EVENT_SHADOW_ACTOR;
  event->data[SHADOW_DATA_VARIANT] = variant;
  event->data[SHADOW_DATA_CLIP] = variant == SHADOW_ACTOR_LOW ? 26 : 22;
  event->data[SHADOW_DATA_SAMPLE] = 0;
  event->data[SHADOW_DATA_X] = opponent->body.pos[AXIS_X];
  event->data[SHADOW_DATA_Y] = variant == SHADOW_ACTOR_UP ? 0 : opponent->body.pos[AXIS_Y];
  event->data[SHADOW_DATA_Z] = opponent->body.pos[AXIS_Z];
  event->data[SHADOW_DATA_VELOCITY_Y] = variant == SHADOW_ACTOR_UP ? -0x3333 : 0;

  if (variant == SHADOW_ACTOR_UP) {
    battle_sound(SOUND_C, 0x33);
  }
}

void battle_shadow_actor_install_physics(Fighter *owner) {
  require(owner != NULL, "Shadow actor physics requires an owner");
  for (int i = 0; i < MAX_BATTLE_EVENTS; ++i) {
    BattleEvent *event = &g_battle.events[i];
    if (battle_shadow_actor_event(event) && event->owner == (owner == &g_battle.fighters[0] ? 0 : 1) && event->data[SHADOW_DATA_VARIANT] == SHADOW_ACTOR_UP) {
      event->data[SHADOW_DATA_Y] = 0;
      event->data[SHADOW_DATA_VELOCITY_Y] = -0x3333;
      event->phase |= SHADOW_PHASE_PHYSICS;
      return;
    }
  }
}
