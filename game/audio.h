#ifndef MK4_GAME_AUDIO_H
#define MK4_GAME_AUDIO_H
#include "game.h"

typedef struct AudioVoice {
  const Sound *sound;
  uint64_t position;
} AudioVoice;

typedef struct GameAudio {
  uint32_t device;
  uint64_t last_tick;
  int next_voice, frames;
  AudioVoice voices[16];
} GameAudio;

void game_audio_init(GameAudio *audio, bool headless);
void game_audio_step(GameAudio *audio, const Assets *assets, const Battle *battle);
void game_audio_free(GameAudio *audio);
#endif
