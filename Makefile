CC = cc
CFLAGS = -O2 -std=c11 -Wall -Wextra -Wpedantic
CXX = c++
CXXFLAGS = -O2 -std=c++17 -Wall -Wextra -Wpedantic
CPPFLAGS = -Icommon
LDLIBS = -lm
SDL2_CONFIG ?= sdl2-config
CLANG_FORMAT ?= $(shell command -v clang-format 2>/dev/null || xcrun --find clang-format 2>/dev/null || echo clang-format)
FORMAT_FILES = $(wildcard *.[ch] *.cpp *.hpp common/*.[ch] game/*.[ch] viewer/*.[ch] tests/*.[ch] tests/fixtures/*.inc)
PACKAGE = common/mk4_package.c common/binary.c
ASSETS = common/mk4_assets.c $(PACKAGE)
RENDER = common/renderer.c common/sdl_app.c common/sprite_font.c common/shadow.c common/lighting.c
HEADERS = $(wildcard common/*.h)
PACKAGE_OBJECTS = build/mk4_package.o build/binary.o

.PHONY: all extract test clean viewer run viewer-test game play game-test format format-check check
all: fsys mk4-viewer mk4-game

format:
	$(CLANG_FORMAT) --style=file -i $(FORMAT_FILES)
format-check:
	$(CLANG_FORMAT) --style=file --dry-run --Werror $(FORMAT_FILES)

build/%.o: common/%.c $(HEADERS)
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@
fsys: fsys.cpp $(PACKAGE_OBJECTS) $(HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) fsys.cpp $(PACKAGE_OBJECTS) -o $@
extract: fsys
	./fsys MK4/FILESYS.DAT --exe MK4/MK-4.EXE -o extracted
test: fsys
	python3 tests/test_fsys.py ./fsys
clean:
	rm -f fsys mk4-viewer mk4-game $(PACKAGE_OBJECTS) build/battle.o build/test-mk4-assets build/test-battle build/test-sub-zero build/test-shadows
viewer: mk4-viewer
mk4-viewer: viewer/main.c $(ASSETS) $(RENDER) $(HEADERS)
	$(CC) $(CPPFLAGS) $(CFLAGS) $$($(SDL2_CONFIG) --cflags) viewer/main.c $(ASSETS) $(RENDER) -o $@ $$($(SDL2_CONFIG) --libs) $(LDLIBS)
run: mk4-viewer
	./mk4-viewer
CHARACTER_SOURCES = $(wildcard game/c_*.c) game/mk4_common.c
ROSTER_SOURCES = game/roster.c game/moves.c game/commands.c game/combat.c game/reactions.c $(CHARACTER_SOURCES)
GAME_HEADERS = game/game.h game/mk4_characters.h
build/test-mk4-assets: tests/test_mk4_assets.c tests/fixtures/original_matrices.inc game/battle.c $(ROSTER_SOURCES) $(GAME_HEADERS) $(ASSETS) $(HEADERS)
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) -Igame tests/test_mk4_assets.c game/battle.c $(ROSTER_SOURCES) $(ASSETS) -o $@ $(LDLIBS)
viewer-test: mk4-viewer build/test-mk4-assets
	./mk4-viewer --self-test
	./build/test-mk4-assets
game: mk4-game
GAME_SOURCES = game/main.c game/input.c $(ROSTER_SOURCES) game/battle.c game/draw.c
mk4-game: $(GAME_SOURCES) $(GAME_HEADERS) $(ASSETS) $(RENDER) $(HEADERS)
	$(CC) $(CPPFLAGS) $(CFLAGS) -Igame $$($(SDL2_CONFIG) --cflags) $(GAME_SOURCES) $(ASSETS) $(RENDER) -o $@ $$($(SDL2_CONFIG) --libs) $(LDLIBS)
play: mk4-game
	./mk4-game
build/test-battle: tests/test_battle.c tests/fixtures/original_kicks.inc tests/fixtures/original_scorpion.inc game/battle.c $(ROSTER_SOURCES) game/input.c $(GAME_HEADERS) common/binary.c
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) -Igame $$($(SDL2_CONFIG) --cflags) tests/test_battle.c game/battle.c $(ROSTER_SOURCES) game/input.c common/binary.c -o $@ $(LDLIBS)
build/test-shadows: tests/test_shadows.c $(ASSETS) $(RENDER) $(HEADERS)
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) $$($(SDL2_CONFIG) --cflags) tests/test_shadows.c $(ASSETS) $(RENDER) -o $@ $$($(SDL2_CONFIG) --libs) $(LDLIBS)
build/test-sub-zero: tests/test_sub_zero.c game/battle.c $(ROSTER_SOURCES) $(GAME_HEADERS) common/binary.c $(HEADERS)
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) -Igame tests/test_sub_zero.c game/battle.c $(ROSTER_SOURCES) common/binary.c -o $@ $(LDLIBS)
game-test: mk4-game build/test-battle build/test-sub-zero build/test-shadows
	./build/test-battle
	./build/test-sub-zero
	./build/test-shadows
	./mk4-game --self-test
	./mk4-game --headless --demo --frames 360 --screenshot build/battle.bmp
check: format-check test viewer-test game-test
	python3 tests/test_apps.py
	python3 tests/test_retail_moves.py
	python3 tests/test_disassembly.py
