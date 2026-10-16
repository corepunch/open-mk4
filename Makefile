CC = cc
CFLAGS = -O2 -std=c11 -Wall -Wextra -Wpedantic
CXX = c++
CXXFLAGS = -O2 -std=c++17 -Wall -Wextra -Wpedantic
CPPFLAGS = -Icommon
LDLIBS = -lm
SDL2_CONFIG ?= sdl2-config
PACKAGE = common/mk4_package.c common/binary.c
ASSETS = common/mk4_assets.c $(PACKAGE)
RENDER = common/renderer.c common/sdl_app.c common/sprite_font.c common/shadow.c common/lighting.c
HEADERS = $(wildcard common/*.h)
PACKAGE_OBJECTS = build/mk4_package.o build/binary.o

.PHONY: all extract test clean viewer run viewer-test game play game-test check
all: fsys mk4-viewer mk4-game

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
	rm -f fsys mk4-viewer mk4-game $(PACKAGE_OBJECTS) build/battle.o build/test-mk4-assets build/test-battle build/test-shadows
viewer: mk4-viewer
mk4-viewer: viewer/main.c $(ASSETS) $(RENDER) $(HEADERS)
	$(CC) $(CPPFLAGS) $(CFLAGS) $$($(SDL2_CONFIG) --cflags) viewer/main.c $(ASSETS) $(RENDER) -o $@ $$($(SDL2_CONFIG) --libs) $(LDLIBS)
run: mk4-viewer
	./mk4-viewer
build/test-mk4-assets: tests/test_mk4_assets.c tests/fixtures/original_matrices.inc $(ASSETS) $(HEADERS)
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) tests/test_mk4_assets.c $(ASSETS) -o $@ $(LDLIBS)
viewer-test: mk4-viewer build/test-mk4-assets
	./mk4-viewer --self-test
	./build/test-mk4-assets
game: mk4-game
mk4-game: game/main.c game/battle.c game/battle.h game/draw.c game/draw.h $(ASSETS) $(RENDER) $(HEADERS)
	$(CC) $(CPPFLAGS) $(CFLAGS) $$($(SDL2_CONFIG) --cflags) game/main.c game/battle.c game/draw.c $(ASSETS) $(RENDER) -o $@ $$($(SDL2_CONFIG) --libs) $(LDLIBS)
play: mk4-game
	./mk4-game
build/test-battle: tests/test_battle.c game/battle.c game/battle.h
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) -Igame tests/test_battle.c game/battle.c -o $@ $(LDLIBS)
build/test-shadows: tests/test_shadows.c $(ASSETS) $(RENDER) $(HEADERS)
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) $$($(SDL2_CONFIG) --cflags) tests/test_shadows.c $(ASSETS) $(RENDER) -o $@ $$($(SDL2_CONFIG) --libs) $(LDLIBS)
game-test: mk4-game build/test-battle build/test-shadows
	./build/test-battle
	./build/test-shadows
	./mk4-game --self-test
	./mk4-game --headless --demo --frames 360 --screenshot build/battle.bmp
check: test viewer-test game-test
	python3 tests/test_apps.py
