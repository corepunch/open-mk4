CXX = c++
CXXFLAGS = -O2 -std=c++17 -Wall -Wextra -Wpedantic
CPPFLAGS = -Icommon
SDL2_CONFIG ?= sdl2-config
PACKAGE = common/mk4_package.cpp
ASSETS = common/mk4_assets.cpp $(PACKAGE)
RENDER = common/renderer.cpp common/sdl_app.cpp common/sprite_font.cpp common/shadow.cpp common/lighting.cpp
HEADERS = $(wildcard common/*.h)

.PHONY: all extract test clean viewer run viewer-test game play game-test check
all: fsys mk4-viewer mk4-game

fsys: fsys.cpp $(PACKAGE) $(HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) fsys.cpp $(PACKAGE) -o $@
extract: fsys
	./fsys MK4/FILESYS.DAT --exe MK4/MK-4.EXE -o extracted
test: fsys
	python3 tests/test_fsys.py ./fsys
clean:
	rm -f fsys mk4-viewer mk4-game build/test-mk4-assets build/test-battle build/test-shadows
viewer: mk4-viewer
mk4-viewer: viewer/main.cpp $(ASSETS) $(RENDER) $(HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $$($(SDL2_CONFIG) --cflags) viewer/main.cpp $(ASSETS) $(RENDER) -o $@ $$($(SDL2_CONFIG) --libs)
run: mk4-viewer
	./mk4-viewer
build/test-mk4-assets: tests/test_mk4_assets.cpp tests/fixtures/original_matrices.inc $(ASSETS) $(HEADERS)
	mkdir -p build
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) tests/test_mk4_assets.cpp $(ASSETS) -o $@
viewer-test: mk4-viewer build/test-mk4-assets
	./mk4-viewer --self-test
	./build/test-mk4-assets
game: mk4-game
mk4-game: game/main.cpp game/battle.cpp game/battle.h game/draw.cpp game/draw.h $(ASSETS) $(RENDER) $(HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $$($(SDL2_CONFIG) --cflags) game/main.cpp game/battle.cpp game/draw.cpp $(ASSETS) $(RENDER) -o $@ $$($(SDL2_CONFIG) --libs)
play: mk4-game
	./mk4-game
build/test-battle: tests/test_battle.cpp game/battle.cpp game/battle.h
	mkdir -p build
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -Igame tests/test_battle.cpp game/battle.cpp -o $@
build/test-shadows: tests/test_shadows.cpp $(ASSETS) $(RENDER) $(HEADERS)
	mkdir -p build
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $$($(SDL2_CONFIG) --cflags) tests/test_shadows.cpp $(ASSETS) $(RENDER) -o $@ $$($(SDL2_CONFIG) --libs)
game-test: mk4-game build/test-battle build/test-shadows
	./build/test-battle
	./build/test-shadows
	./mk4-game --self-test
	./mk4-game --headless --demo --frames 360 --screenshot build/battle.bmp
check: test viewer-test game-test
	python3 tests/test_apps.py
