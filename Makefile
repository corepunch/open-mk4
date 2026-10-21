CC = cc
CFLAGS = -O2 -std=c11 -Wall -Wextra -Wpedantic
CXX = c++
CXXFLAGS = -O2 -std=c++17 -Wall -Wextra -Wpedantic
CPPFLAGS = -Icommon
LDLIBS = -lm
SDL2_CONFIG ?= sdl2-config
CLANG_FORMAT ?= $(shell command -v clang-format 2>/dev/null || xcrun --find clang-format 2>/dev/null || echo clang-format)
export CLANG_FORMAT
FORMAT_FILES = $(wildcard *.[ch] *.cpp *.hpp common/*.[ch] game/*.[ch] game/*.inc game/*/*.[ch] game/*/moves/*.[ch] viewer/*.[ch] tests/*.[ch] tests/fixtures/*.inc)
PACKAGE = common/mk4_package.c common/binary.c
ASSETS = common/mk4_image.c common/mk4_assets.c common/mk4_audio.c $(PACKAGE)
RENDER = common/renderer.c common/sdl_app.c common/sprite_font.c common/shadow.c common/lighting.c
HEADERS = $(wildcard common/*.h)
PACKAGE_OBJECTS = build/mk4_package.o build/binary.o

SCRIPT_CHARACTERS = $(patsubst game/%/port.json,%,$(wildcard game/*/port.json))
SCRIPT_CHECK_TARGETS = $(addprefix scripts-check-,$(SCRIPT_CHARACTERS))
SCRIPT_AUDIT_TARGETS = $(addprefix scripts-audit-,$(SCRIPT_CHARACTERS))

.PHONY: retail-vm-test native-frames-check scripts scripts-check scripts-audit mmove-frames mmove-frames-check $(SCRIPT_CHECK_TARGETS) $(SCRIPT_AUDIT_TARGETS) all wargods-test extract extract-wargods test clean viewer run viewer-test game play game-test format format-check check
all: fsys mk4-viewer mk4-game

scripts: scripts-audit
scripts-check: scripts-audit
scripts-audit:
	python3 tools/compile_retail.py --all --audit --strict --summary --require-implemented
mmove-frames:
	python3 tools/generate_mmove_frames.py
native-frames-check:
	python3 tests/test_c_reachability.py
	python3 tools/check_native_frames.py
	python3 tests/test_native_frames.py
mmove-frames-check:
	python3 tools/generate_mmove_frames.py --check
$(SCRIPT_CHECK_TARGETS): scripts-check-%:
	python3 tools/compile_retail.py --character $* --audit --strict --summary --require-implemented
$(SCRIPT_AUDIT_TARGETS): scripts-audit-%:
	python3 tools/compile_retail.py --character $* --audit --strict --summary --require-implemented

format:
	$(CLANG_FORMAT) --style=file -i $(FORMAT_FILES)
format-check:
	$(CLANG_FORMAT) --style=file --dry-run --Werror $(FORMAT_FILES)

build/%.o: common/%.c $(HEADERS)
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@
fsys: fsys.cpp $(PACKAGE_OBJECTS) $(HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) fsys.cpp $(PACKAGE_OBJECTS) -o $@
extract-wargods: fsys
	./fsys Wargods/WARGODS.DAT -o extracted/wargods
extract: fsys
	./fsys MK4/FILESYS.DAT --exe MK4/MK-4.EXE -o extracted
build/test-wargods: tests/test_wargods.c common/wargods_wiff.c common/wargods_wiff.h $(PACKAGE) $(HEADERS)
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) tests/test_wargods.c common/wargods_wiff.c $(PACKAGE) -o $@ $(LDLIBS)
wargods-test: build/test-wargods
	./build/test-wargods
build/test-package: tests/test_package.c $(PACKAGE) $(HEADERS)
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) tests/test_package.c $(PACKAGE) -o $@
test: fsys build/test-package
	./build/test-package
	python3 tests/test_fsys.py ./fsys
clean:
	rm -f fsys mk4-viewer mk4-game $(PACKAGE_OBJECTS) build/battle.o build/test-package build/test-mk4-assets build/test-battle build/test-sub-zero build/test-liu-kang build/test-raiden-sonya build/test-shadows build/test-frames build/test-retail-vm
viewer: mk4-viewer
mk4-viewer: viewer/main.c $(ASSETS) $(RENDER) $(HEADERS)
	$(CC) $(CPPFLAGS) $(CFLAGS) $$($(SDL2_CONFIG) --cflags) viewer/main.c $(ASSETS) $(RENDER) -o $@ $$($(SDL2_CONFIG) --libs) $(LDLIBS)
run: mk4-viewer
	./mk4-viewer
CHARACTER_SOURCES = $(wildcard game/*/character.c game/*/moves/*.c) game/mk4_common.c
ROSTER_SOURCES = game/roster.c game/moves.c game/native.c game/native_combat.c game/script.c game/retail_data.c game/retail_native.c game/retail_native_vm.c game/retail_scene.c game/retail_combat.c game/retail_air.c game/retail_air_throw.c game/retail_teleport.c game/retail_controls.c game/retail_throw.c game/retail_vm.c game/pose.c game/weapons.c game/scene.c game/presentation.c game/cpu.c game/commands.c game/combat.c game/reactions.c game/shadow_actor.c $(CHARACTER_SOURCES)
GAME_HEADERS = $(wildcard game/*/*.h game/*/moves/*.[ch]) game/native.h game/script.h game/retail_vm.h game/retail_data.h game/retail_native.h game/retail_native_input.inc game/game.h game/menu.h game/mk4_characters.h game/audio.h game/retail_actions.inc
build/test-mk4-assets: tests/test_mk4_assets.c tests/fixtures/original_matrices.inc game/battle.c $(ROSTER_SOURCES) $(GAME_HEADERS) $(ASSETS) $(HEADERS)
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) -Igame tests/test_mk4_assets.c game/battle.c $(ROSTER_SOURCES) $(ASSETS) -o $@ $(LDLIBS)
viewer-test: mk4-viewer build/test-mk4-assets
	./mk4-viewer --self-test
	./build/test-mk4-assets
game: mk4-game
GAME_SOURCES = game/main.c game/menu.c game/input.c game/audio.c $(ROSTER_SOURCES) game/battle.c game/draw.c
mk4-game: $(GAME_SOURCES) $(GAME_HEADERS) $(ASSETS) $(RENDER) $(HEADERS)
	$(CC) $(CPPFLAGS) $(CFLAGS) -Igame $$($(SDL2_CONFIG) --cflags) $(GAME_SOURCES) $(ASSETS) $(RENDER) -o $@ $$($(SDL2_CONFIG) --libs) $(LDLIBS)
play: mk4-game
	./mk4-game
build/test-battle: tests/test_battle.c game/battle.c $(ROSTER_SOURCES) game/input.c $(GAME_HEADERS) $(ASSETS)
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) -Igame $$($(SDL2_CONFIG) --cflags) tests/test_battle.c game/battle.c $(ROSTER_SOURCES) game/input.c $(ASSETS) -o $@ $(LDLIBS)
build/test-shadows: tests/test_shadows.c $(ASSETS) $(RENDER) $(HEADERS)
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) $$($(SDL2_CONFIG) --cflags) tests/test_shadows.c $(ASSETS) $(RENDER) -o $@ $$($(SDL2_CONFIG) --libs) $(LDLIBS)
build/test-sub-zero: tests/test_sub_zero.c game/battle.c $(ROSTER_SOURCES) $(GAME_HEADERS) $(ASSETS) $(HEADERS)
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) -Igame tests/test_sub_zero.c game/battle.c $(ROSTER_SOURCES) $(ASSETS) -o $@ $(LDLIBS)
build/test-liu-kang: tests/test_liu_kang.c game/battle.c $(ROSTER_SOURCES) $(GAME_HEADERS) $(ASSETS) $(HEADERS)
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) -Igame tests/test_liu_kang.c game/battle.c $(ROSTER_SOURCES) $(ASSETS) -o $@ $(LDLIBS)
build/test-frames: tests/test_frames.c game/battle.c $(ROSTER_SOURCES) $(GAME_HEADERS) $(ASSETS) $(HEADERS)
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) -Igame tests/test_frames.c game/battle.c $(ROSTER_SOURCES) $(ASSETS) -o $@ $(LDLIBS)
build/test-raiden-sonya: tests/test_raiden_sonya.c game/battle.c $(ROSTER_SOURCES) $(GAME_HEADERS) $(ASSETS) $(HEADERS)
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) -Igame tests/test_raiden_sonya.c game/battle.c $(ROSTER_SOURCES) $(ASSETS) -o $@ $(LDLIBS)
build/test-retail-vm: tests/test_retail_vm.c game/battle.c $(ROSTER_SOURCES) $(GAME_HEADERS) $(ASSETS) $(HEADERS)
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) -Igame tests/test_retail_vm.c game/battle.c $(ROSTER_SOURCES) $(ASSETS) -o $@ $(LDLIBS)
retail-vm-test: build/test-retail-vm
	./build/test-retail-vm
	python3 tests/test_retail_vm.py
game-test: mk4-game build/test-battle build/test-sub-zero build/test-liu-kang build/test-raiden-sonya build/test-shadows build/test-frames
	./build/test-battle
	./build/test-sub-zero
	./build/test-liu-kang
	./build/test-raiden-sonya
	./build/test-shadows
	./build/test-frames
	./mk4-game --self-test
	./mk4-game --headless --demo --frames 360 --screenshot build/battle.bmp
check: retail-vm-test format-check scripts-check mmove-frames-check native-frames-check test viewer-test game-test
	python3 tests/test_apps.py
	python3 tests/test_retail_moves.py
	python3 tests/test_disassembly.py
	python3 tests/test_retail_symbols.py
	python3 tests/test_retail_helpers.py
	python3 tests/test_retail_actions.py
	python3 tests/test_retail_port.py
	python3 tests/test_retail_audit.py
	python3 tests/test_retail_controls.py
