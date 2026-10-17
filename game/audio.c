#include "audio.h"
#include "math_util.h"
#include <SDL.h>
#include <string.h>

void game_audio_init(GameAudio *audio, bool headless) {
  memset(audio, 0, sizeof *audio);
  if (headless || SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
    return;
  }
  SDL_AudioSpec wanted = {.freq = 22050, .format = AUDIO_S16SYS, .channels = 2, .samples = 512};
  audio->device = SDL_OpenAudioDevice(NULL, 0, &wanted, NULL, 0);
  if (audio->device != 0) {
    SDL_PauseAudioDevice(audio->device, 0);
  }
}

void game_audio_step(GameAudio *audio, const Assets *assets) {
  if (audio->device == 0) {
    return;
  }
  if (g_battle.ticks <= audio->last_tick) {
    memset(audio->voices, 0, sizeof audio->voices);
    SDL_ClearQueuedAudio(audio->device);
  }
  audio->last_tick = g_battle.ticks;
  for (int i = 0; i < g_battle.sound_count; ++i) {
    SoundEvent event = g_battle.sounds[i];
    const Sound *sound = sound_event(&assets->audio, event.bank, event.selector);
    if (sound != NULL) {
      audio->voices[audio->next_voice] = (AudioVoice){sound, 0};
      audio->next_voice = (audio->next_voice + 1) % 16;
    }
  }
  int16_t output[368 * 2];
  int frames = (++audio->frames & 1) ? 367 : 368;
  for (int n = 0; n < frames; ++n) {
    int mixed[2] = {0};
    for (int i = 0; i < 16; ++i) {
      AudioVoice *voice = &audio->voices[i];
      const Sound *sound = voice->sound;
      if (sound == NULL) {
        continue;
      }
      size_t frame = (size_t)(voice->position >> 16);
      if (frame >= sound->samples.count / (size_t)sound->channels) {
        if (sound->loop) {
          voice->position = 0;
          frame = 0;
        } else {
          voice->sound = NULL;
          continue;
        }
      }
      for (int channel = 0; channel < 2; ++channel) {
        size_t at = frame * (size_t)sound->channels + (sound->channels == 2 ? (size_t)channel : 0);
        mixed[channel] += sound->samples.data[at];
      }
      voice->position += (uint64_t)sound->rate * Q16_ONE / 22050;
    }
    for (int channel = 0; channel < 2; ++channel) {
      output[n * 2 + channel] = (int16_t)mini(32767, maxi(-32768, mixed[channel] / 2));
    }
  }
  if (SDL_GetQueuedAudioSize(audio->device) > 22050) {
    SDL_ClearQueuedAudio(audio->device);
  }
  SDL_QueueAudio(audio->device, output, (Uint32)(frames * 2 * sizeof output[0]));
}

void game_audio_free(GameAudio *audio) {
  if (audio->device != 0) {
    SDL_CloseAudioDevice(audio->device);
  }
  memset(audio, 0, sizeof *audio);
}
