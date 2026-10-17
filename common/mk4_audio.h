#ifndef MK4_AUDIO_H
#define MK4_AUDIO_H
#include "mk4_package.h"

ARRAY_TYPE(SoundSamples, int16_t);

typedef struct Sound {
  int id, rate, channels;
  bool loop;
  SoundSamples samples;
} Sound;

ARRAY_TYPE(Sounds, Sound);

enum {
  SOUND_A,
  SOUND_B,
  SOUND_C,
  SOUND_INDEXED,      /* Opcode 25 / 48a0d0, separate from A/B/C. */
  SOUND_IMPACT_VOICE, /* 48a230 selector 1, indexed by roster. */
  SOUND_THROW_VOICE,  /* 489ff0 selector 8, roster remap 4ead18. */
  SOUND_DRAW_VOICE,   /* 48a230 selector 0, weapon draw voice. */
  SOUND_BANK_COUNT
};

#define SOUND_EVENT_COUNT 184

typedef struct SoundBank {
  Sounds sounds;
  int event_counts[SOUND_BANK_COUNT];
  uint16_t events[SOUND_BANK_COUNT][SOUND_EVENT_COUNT];
} SoundBank;

Sound sound_decode(View data, int id);
void sound_bank_load(SoundBank *bank, const Package *archive);
void sound_bank_free(SoundBank *bank);
const Sound *sound_find(const SoundBank *bank, int id);
const Sound *sound_event(const SoundBank *bank, int category, int selector);
#endif
