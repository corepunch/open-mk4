#include "game.h"
#include "audio.h"
#include "sdl_app.h"
#include "math_util.h"
#include "cli.h"
#include <stdio.h>
#include <string.h>

static void usage(void) {
  puts("MK4 native battle game\n"
       "  --archive PATH     default MK4/FILESYS.DAT\n"
       "  --exe PATH         default MK4/MK-4.EXE\n"
       "  --p1 N --p2 N      roster slots 0..17 (default Scorpion / Sub Zero)\n"
       "  --arena N          arena 0..10 (default 8, The Prison)\n"
       "  --two-player       local controls for player two\n"
       "  --demo             CPU controls both fighters\n"
       "  --difficulty N     CPU level 1..9 (default 3)\n"
       "  --headless         render without a window\n"
       "  --frames N         stop after N frames (headless advances exactly 1/60s)\n"
       "  --screenshot PATH  save final frame as BMP\n"
       "  --list             show original roster and arenas\n"
       "  --self-test        validate battle resources and animation compatibility\n"
       "P1: A/D W/S move, U HP, J LP, I HK, K LK, L block, H run.\n"
       "P2: arrows, keypad 4 HP, 1 LP, 5 HK, 2 LK, 3 block, right Shift run.\n"
       "F1/F2 change fighters, F3 CPU/local, F4 arena, R reset, Enter rematch, Space pause.\n");
}

static void validate(const Assets *assets) {
  int roster;
  require(assets->arenas.count == 11, "Incomplete arena list");
  require(assets->audio.sounds.count == 674, "Incomplete sound archive");
  const Sound *ice_sound = sound_event(&assets->audio, SOUND_INDEXED, 4);
  require(assets->audio.event_counts[SOUND_INDEXED] == 66 && assets->audio.events[SOUND_INDEXED][4] == 0x852 && ice_sound != NULL && ice_sound->id == 426,
      "Indexed ice cue must resolve the original token to pcmk0426.esf");
  require(sound_event(&assets->audio, SOUND_B, 4) != ice_sound, "Indexed selectors must remain distinct from sound bank B");
  require(sound_event(&assets->audio, SOUND_INDEXED, 65) == NULL && sound_event(&assets->audio, SOUND_INDEXED, 66) == NULL,
      "Zero and out-of-range indexed sounds must be silent");
  const Sound *throw_sound = sound_event(&assets->audio, SOUND_INDEXED, 13);
  const Sound *impact_voice = sound_event(&assets->audio, SOUND_IMPACT_VOICE, 4);
  require(throw_sound != NULL && throw_sound->id == 420 && impact_voice != NULL && impact_voice->id == 185,
      "Air throw and Sub-Zero floor-impact cues must resolve original sampled tokens");
  require(sound_event(&assets->audio, SOUND_IMPACT_VOICE, 16) == sound_event(&assets->audio, SOUND_IMPACT_VOICE, 2) &&
              sound_event(&assets->audio, SOUND_IMPACT_VOICE, 17) == sound_event(&assets->audio, SOUND_IMPACT_VOICE, 7),
      "Impact voices must preserve Kitana/Noob roster remaps");
  for (size_t i = 0; i < assets->arenas.count; ++i) {
    const Arena *arena = &assets->arenas.data[i];
    size_t triangles = 0;
    require(arena->nodes.count && arena->geometry.textures.count == 3, "Incomplete arena");
    for (size_t j = 0; j < arena->nodes.count; ++j) {
      if (arena->nodes.data[j].flags & 4)
        continue;
      triangles += arena->geometry.meshes.data[arena->nodes.data[j].mesh].triangles.count;
    }
    require(triangles > 0, "Empty arena");
    printf("%-20s %-15s %zu nodes / %zu triangles / %zu pages\n", arena->name, arena->file, arena->nodes.count, triangles, arena->geometry.textures.count);
  }
  require(assets->hud.font.sprites.count == 143, "Invalid HUD sprite directory");
  require(roster_count() == MK4_ROSTER_COUNT, "MK4 roster size drifted");
  for (roster = 0; roster < roster_count(); ++roster) {
    const CharacterDef *def = roster_def(roster);
    const Character *ch = roster_character(assets, roster);
    int channels = 0;
    int state;
    require(strcmp(def->name, ch->name) == 0, "Roster name does not match the loaded mesh");
    require(assets->hud.names[def->asset] >= 0, "Missing fighter name");
    for (size_t j = 0; j < ch->bones.count; ++j)
      channels = maxi(channels, ch->bones.data[j].channel + 1);
    for (state = 0; state < def->move_count; ++state) {
      const MoveDef *move = def->moves[state];
      const Clip *clip = &assets->clips.data[roster_clip(def, state, ch->idle)];
      int frames = (int)clip->frames.count;
      require((int)clip->frames.data[0].angles.count == channels, "Incompatible combat animation");
      require(move->first >= 0 && move->first < frames && (move->last == MOVE_END || (move->last >= 0 && move->last < frames)),
          "Move frame range outside its clip");
      for (int k = 0; k < 2; ++k) {
        Pose nodes[MK4_MAX_BONES];
        double frame = k ? (double)clip->frames.count * 0.5 : 0.0;
        size_t n;
        pose(ch, clip, frame, nodes);
        for (n = 0; n < ch->bones.count; ++n) {
          float sum = nodes[n].position.x + nodes[n].position.y + nodes[n].position.z;
          require(isfinite(sum), "Invalid combat pose");
        }
      }
    }
  }
  puts("PASS: 11 textured arena hierarchies; original HUD/name sprites; all 18 combat rigs compatible");
}

static void apply_shell(
    ShellCommand command, bool *running, bool *paused, bool *cpu, bool *demo, int *arena, int arena_count, InputDevice pads[2], const unsigned char *keys) {
  int player;
  switch (command) {
    case SHELL_QUIT:
      *running = false;
      break;
    case SHELL_PAUSE:
      *paused = !*paused;
      input_sync(&pads[0], keys);
      input_sync(&pads[1], keys);
      break;
    case SHELL_RESET:
      battle_reset_match();
      input_sync(&pads[0], keys);
      input_sync(&pads[1], keys);
      break;
    case SHELL_REMATCH:
      if (g_battle.phase == PHASE_MATCH_OVER) {
        battle_reset_match();
        input_sync(&pads[0], keys);
        input_sync(&pads[1], keys);
      }
      break;
    case SHELL_NEXT_P1:
    case SHELL_NEXT_P2:
      player = command == SHELL_NEXT_P1 ? 0 : 1;
      g_battle.fighters[player].roster = (g_battle.fighters[player].roster + 1) % roster_count();
      battle_reset_match();
      input_sync(&pads[0], keys);
      input_sync(&pads[1], keys);
      break;
    case SHELL_TOGGLE_CPU:
      *cpu = !*cpu;
      *demo = false;
      input_sync(&pads[0], keys);
      input_sync(&pads[1], keys);
      break;
    case SHELL_NEXT_ARENA:
      *arena = (*arena + 1) % arena_count;
      battle_set_arena(*arena);
      battle_reset_match();
      input_sync(&pads[0], keys);
      input_sync(&pads[1], keys);
      break;
    case SHELL_NONE:
      break;
  }
}

int main(int argc, char **argv) {
  const char *archive = "MK4/FILESYS.DAT";
  const char *exe = "MK4/MK-4.EXE";
  const char *shot = NULL;
  bool headless = false, demo = false, cpu = true, selftest = false, list = false;
  int p1 = 0, p2 = 4, arena = 8, limit = 0, difficulty = 3;
  Assets assets = {0};
  SdlApp app;
  Canvas cv;
  GameAudio audio;
  InputDevice pads[2];
  bool running = true, paused = false;
  int rendered = 0;
  double accumulator = 0;
  uint64_t last;

  for (int i = 1; i < argc; ++i) {
    const char *arg = argv[i];
    if (!strcmp(arg, "--archive"))
      archive = cli_value(argc, argv, &i);
    else if (!strcmp(arg, "--exe"))
      exe = cli_value(argc, argv, &i);
    else if (!strcmp(arg, "--screenshot"))
      shot = cli_value(argc, argv, &i);
    else if (!strcmp(arg, "--p1"))
      p1 = cli_int(cli_value(argc, argv, &i));
    else if (!strcmp(arg, "--p2"))
      p2 = cli_int(cli_value(argc, argv, &i));
    else if (!strcmp(arg, "--arena"))
      arena = cli_int(cli_value(argc, argv, &i));
    else if (!strcmp(arg, "--frames"))
      limit = cli_int(cli_value(argc, argv, &i));
    else if (!strcmp(arg, "--difficulty"))
      difficulty = cli_int(cli_value(argc, argv, &i));
    else if (!strcmp(arg, "--headless"))
      headless = true;
    else if (!strcmp(arg, "--demo"))
      demo = true;
    else if (!strcmp(arg, "--two-player"))
      cpu = false;
    else if (!strcmp(arg, "--self-test"))
      selftest = true;
    else if (!strcmp(arg, "--list"))
      list = true;
    else if (!strcmp(arg, "--help") || !strcmp(arg, "-h")) {
      usage();
      return 0;
    } else {
      fatal("Unknown option %s", arg);
    }
  }
  require(p1 >= 0 && p1 < roster_count() && p2 >= 0 && p2 < roster_count(), "Invalid roster slot");
  require(arena >= 0 && arena < 11 && limit >= 0, "Invalid arena or frame limit");

  assets_load(&assets, archive, exe);
  assets_load_battle(&assets, archive, exe);
  if (list) {
    for (size_t i = 0; i < assets.characters.count; ++i)
      printf("Fighter %2zu: %s\n", i, assets.characters.data[i].name);
    for (size_t i = 0; i < assets.arenas.count; ++i)
      printf("Arena %2zu: %s\n", i, assets.arenas.data[i].name);
    assets_free(&assets);
    return 0;
  }
  if (selftest) {
    validate(&assets);
    assets_free(&assets);
    return 0;
  }

  sdl_app_init(&app, "Mortal Kombat 4 - Battle", 1280, 800, headless);
  canvas_init(&cv, 1280, 800);
  battle_init(p1, p2);
  require(difficulty >= 1 && difficulty <= 9, "CPU difficulty must be between 1 and 9");
  g_battle.cpu_level = difficulty;
  battle_bind_assets(&assets);
  game_audio_init(&audio, headless);
  battle_set_arena(arena);
  input_bind_player(&pads[0], 0);
  input_bind_player(&pads[1], 1);
  last = SDL_GetPerformanceCounter();
  if (headless && !limit)
    limit = 1;

  while (running) {
    SDL_Event ev;
    const unsigned char *keys;
    while (SDL_PollEvent(&ev)) {
      if (ev.type == SDL_QUIT)
        running = false;
      if (ev.type == SDL_WINDOWEVENT && ev.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
        paused = true;
        keys = SDL_GetKeyboardState(NULL);
        input_sync(&pads[0], keys);
        input_sync(&pads[1], keys);
      }
      if (ev.type != SDL_KEYDOWN || ev.key.repeat)
        continue;
      keys = SDL_GetKeyboardState(NULL);
      apply_shell(input_shell_command((int)ev.key.keysym.sym), &running, &paused, &cpu, &demo, &arena, (int)assets.arenas.count, pads, keys);
    }

    {
      uint64_t now = SDL_GetPerformanceCounter();
      double dt = headless ? 1.0 / 60.0 : fmin(0.1, (double)(now - last) / (double)SDL_GetPerformanceFrequency());
      last = now;
      if (!paused)
        accumulator += dt;
      else
        accumulator = 0;
    }
    keys = SDL_GetKeyboardState(NULL);
    while (accumulator >= 1.0 / 60.0) {
      Input in[2];
      input_sample(&pads[0], keys, &in[0]);
      input_sample(&pads[1], keys, &in[1]);
      g_battle.fighters[0].human = !demo;
      g_battle.fighters[1].human = !(cpu || demo);
      if (demo)
        input_cpu(0, &in[0]);
      if (cpu || demo)
        input_cpu(1, &in[1]);
      battle_step(in);
      game_audio_step(&audio, &assets);
      accumulator -= 1.0 / 60.0;
      if (demo && g_battle.phase == PHASE_MATCH_OVER && g_battle.phase_ticks > 240)
        battle_reset_match();
    }
    draw_battle(&cv, &assets, arena, cpu, paused);
    sdl_app_present(&app, &cv);
    ++rendered;
    if (limit && rendered >= limit)
      running = false;
    if (!headless)
      SDL_Delay(1);
  }

  if (shot != NULL)
    save_bmp(&cv, shot);
  printf("Rendered %d frames; simulated %llu ticks; health %.4f / %.4f; round %d\n", rendered, (unsigned long long)g_battle.ticks,
      q16_float(g_battle.fighters[0].health), q16_float(g_battle.fighters[1].health), g_battle.round);
  game_audio_free(&audio);
  sdl_app_free(&app);
  canvas_free(&cv);
  assets_free(&assets);
  return 0;
}
