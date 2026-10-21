#!/usr/bin/env python3
"""Reject native actor action clocks outside the fixed-frame runners."""
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
NATIVE = ('native.c', 'native_combat.c', 'script.c', 'mk4_common.c', 'reactions.c', 'weapons.c', 'retail_combat.c', 'retail_scene.c', 'retail_native.c', 'retail_native_vm.c')


def masked(text):
    return re.sub(r'/\*.*?\*/|//[^\n]*|"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'',
                  lambda match: re.sub(r'[^\n]', ' ', match[0]), text, flags=re.S)


def audit_text(path, text):
    source = masked(text)
    errors = []
    forbidden = r'\b(?:script_timer|move_loops|fighter_start_sequence|MoveSequence|MoveSegment|MoveEvent)\b'
    for match in re.finditer(forbidden, source):
        errors.append('%s:%d: legacy native timing: %s' % (path, source.count('\n', 0, match.start()) + 1, match[0]))
    pattern = r'^[\w* \t]+\b(\w+)\s*\([^;{}]*\)\s*\{'
    for match in re.finditer(pattern, source, re.M):
        depth, end = 1, match.end()
        while depth and end < len(source):
            depth += (source[end] == '{') - (source[end] == '}')
            end += 1
        body = source[match.end():end]
        actors = re.findall(r'\bFighter\s*\*\s*(\w+)', match[0])
        clocks = [r'\bg_battle\.phase_ticks\b'] + [r'\b' + name + r'->ticks\b' for name in actors]
        for clock in clocks:
            if re.search(r'\b(?:if|while|for)\s*\([^{};]*' + clock, body):
                errors.append('%s: %s branches on an action clock; attach the action to a frame' % (path, match[1]))
        for actor in actors:
            read = re.sub(r'\b' + actor + r'->ticks\s*=\s*0\s*;', '', body)
            if re.search(r'\b' + actor + r'->ticks\b', read) and not re.search(r'\b(?:native_begin|native_motion_begin|native_play|ground_build|ground_begin)\s*\(', body):
                errors.append('%s: %s reads the actor clock outside entry-time frame construction' % (path, match[1]))
    return errors


def main():
    paths = sorted((ROOT / 'game').glob('native/*.c'))
    paths += sorted((ROOT / 'game').glob('retail_*.c'))
    paths += [ROOT / 'game' / name for name in NATIVE]
    paths = sorted(set(paths))
    errors = [error for path in paths for error in audit_text(path.relative_to(ROOT), path.read_text())]
    errors += ['Generated fighter runtime output is forbidden: ' + str(path.relative_to(ROOT))
               for path in (ROOT / 'game').glob('*/generated/*.inc')]
    errors += ['Per-character C architecture is forbidden: ' + str(path.relative_to(ROOT))
               for pattern in ('*/character.[ch]', '*/moves/*.[ch]') for path in (ROOT / 'game').glob(pattern)]
    if errors:
        print('\n'.join(errors))
        return 1
    print('PASS: native actions use the shared runtime or native frame runners; no legacy actor clocks')
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
