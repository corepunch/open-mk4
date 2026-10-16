#include "mk4_audio.h"
#include "math_util.h"
#include <stdio.h>

/* ESF header and high-nibble-first IMA stream: supplied 4b05b0/4b0670
 * and reference/mk4decomp/src/engine/esf.c, src/audio/adpcm.c. */
Sound sound_decode(View data, int id) {
  require(data.n >= 8 && view_u32(data, 0) == 0x06465345, "Invalid ESF header");
  uint32_t format = view_u32(data, 4);
  size_t bytes = format & 0x1fffffff;
  bool adpcm = (format & 0x20000000) != 0;
  require(bytes <= 64 * 1024 * 1024 && bytes > 0, "Invalid ESF decoded length");
  size_t packed = adpcm ? bytes / 4 : bytes;
  require(!adpcm || bytes % 2 == 0, "Unaligned ESF ADPCM length");
  view_check(data, 8, packed);
  /* 4c201b..4c2020 always creates mono PCM. Bit 31 is copied to the
   * buffer's loop flag, consumed by 4c24a3..4c24bb; it is not stereo. */
  Sound sound = {.id = id, .rate = format & 0x40000000 ? 22050 : 11025, .channels = 1, .loop = (format >> 31) != 0};
  RESIZE(sound.samples, adpcm ? bytes / 2 : bytes);
  require(sound.samples.count % (size_t)sound.channels == 0, "Unaligned ESF channel data");
  if (!adpcm) {
    for (size_t i = 0; i < sound.samples.count; ++i) {
      sound.samples.data[i] = (int16_t)(((int)data.p[8 + i] - 128) * 256);
    }
    return sound;
  }
  // clang-format off: Keep table entries on one row, including long rows.
  static const int steps[89] = {
    7,8,9,10,11,12,13,14,16,17,19,21,23,25,28,31,34,37,41,45,50,55,60,66,73,80,88,97,107,118,130,143,157,173,190,209,230,253,279,307,337,371,408,449,494,544,598,658,724,796,876,963,1060,1166,1282,1411,1552,1707,1878,2066,2272,2499,2749,3024,3327,3660,4026,4428,4871,5358,5894,6484,7132,7845,8630,9493,10442,11487,12635,13899,15289,16818,18500,20350,22385,24623,27086,29794,32767
  };
  static const int indices[8] = {-1,-1,-1,-1,2,4,6,8};
  // clang-format on
  int predictor = 0, index = 0;
  for (size_t i = 0; i < sound.samples.count; ++i) {
    if (i / 2 >= packed) {
      /* Three supplied looping files lack the final ADPCM nibble.
       * Preserve their declared duration with silence, without an overread. */
      sound.samples.data[i] = 0;
      continue;
    }
    unsigned byte = data.p[8 + i / 2];
    unsigned nibble = (i & 1) ? byte & 15 : byte >> 4;
    int step = steps[index];
    int delta = step / 8;
    if (nibble & 4) {
      delta += step;
    }
    if (nibble & 2) {
      delta += step / 2;
    }
    if (nibble & 1) {
      delta += step / 4;
    }
    predictor += (nibble & 8) ? -delta : delta;
    predictor = mini(32767, maxi(-32768, predictor));
    index = mini(88, maxi(0, index + indices[nibble & 7]));
    sound.samples.data[i] = (int16_t)predictor;
  }
  return sound;
}

void sound_bank_load(SoundBank *bank, const Package *archive) {
  require(bank->sounds.count == 0, "Sounds already loaded");
  for (int id = 0; id < 2200; ++id) {
    char path[128];
    snprintf(path, sizeof path, "c:\\source\\mk4\\win\\sound\\pcmk%04d.esf", id);
    uint32_t hash = name_hash(path);
    for (size_t i = 0; i < archive->count; ++i) {
      const PackageEntry *entry = &archive->entries[i];
      if (entry->hash == hash) {
        View data = view_sub(view(archive->bytes), entry->offset, entry->size);
        PUSH(bank->sounds, sound_decode(data, id));
        break;
      }
    }
  }
}

void sound_bank_free(SoundBank *bank) {
  for (size_t i = 0; i < bank->sounds.count; ++i) {
    FREE_ARRAY(bank->sounds.data[i].samples);
  }
  FREE_ARRAY(bank->sounds);
}

const Sound *sound_find(const SoundBank *bank, int id) {
  for (size_t i = 0; i < bank->sounds.count; ++i) {
    if (bank->sounds.data[i].id == id) {
      return &bank->sounds.data[i];
    }
  }
  return NULL;
}

const Sound *sound_event(const SoundBank *bank, int category, int selector) {
  if (category < 0 || category >= SOUND_BANK_COUNT || selector < 0 || selector >= SOUND_EVENT_COUNT) {
    return NULL;
  }
  int token = bank->events[category][selector];
  /* 4bd4f0: positive sampled effects use token/5 as the file/buffer ID.
   * Small tokens select the retail music/control slots instead. */
  return token > 100 && token < 0x8000 ? sound_find(bank, token / 5) : NULL;
}
