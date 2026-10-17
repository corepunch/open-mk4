#!/usr/bin/env python3
"""Compile decoded retail scripts to C11 fibers, or audit compilation coverage."""
import argparse
import json
from pathlib import Path
import shutil
import subprocess
import sys

from character_catalog import character_id, moves_for, CHARACTERS
from retail_moves import RetailImage, EXE_SHA256
from retail_codegen import CompileError, compile_program, generate
from retail_bindings import bindings_for

ROOT = Path(__file__).resolve().parents[1]


def format_c(source):
    formatter = shutil.which('clang-format')
    if formatter is None:
        result = subprocess.run(['xcrun', '--find', 'clang-format'], capture_output=True, text=True)
        formatter = result.stdout.strip() if result.returncode == 0 else None
    if not formatter:
        raise ValueError('clang-format is required for canonical generated C')
    return subprocess.run([formatter, '--style=file', '--assume-filename=' + str(ROOT / 'game/generated/script.inc')],
                          input=source, text=True, capture_output=True, check=True).stdout


def audit(image, character, return_to='fighter_ready'):
    bindings = bindings_for(character)
    roots = {}
    for name, move in moves_for(character).items():
        for role, entry in move['scripts']:
            roots.setdefault(entry, []).append('%s: %s' % (name, role))
    scripts = []
    for entry, names in sorted(roots.items()):
        try:
            program, _ = compile_program(image, entry, character, bindings, return_to)
            scripts.append(dict(entry='0x%08x' % entry, moves=names, status='compilable',
                                instructions=len(program['instructions']), diagnostics=[]))
        except (CompileError, ValueError) as exc:
            diagnostics = exc.diagnostics if isinstance(exc, CompileError) else [dict(pc='0x%08x' % entry, reason=str(exc))]
            scripts.append(dict(entry='0x%08x' % entry, moves=names, status='unsupported', diagnostics=diagnostics))
    return dict(schema=1, character=CHARACTERS[character], exe_sha256=EXE_SHA256,
                scope='Catalogued VM graphs and typed native boundaries; not native implementation parity',
                scripts=scripts, compilable=sum(row['status'] == 'compilable' for row in scripts), total=len(scripts))


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--exe', type=Path, default=ROOT / 'MK4/MK-4.EXE')
    parser.add_argument('--character', type=character_id, default=0)
    action = parser.add_mutually_exclusive_group(required=True)
    action.add_argument('--script', type=lambda value: int(value, 16), action='append', help='repeat to emit several scripts atomically')
    action.add_argument('--audit', action='store_true')
    parser.add_argument('--return-to', help='explicit controller/tagged-return engine continuation')
    parser.add_argument('--output', type=Path)
    parser.add_argument('--check', action='store_true', help='compare output without writing')
    parser.add_argument('--strict', action='store_true', help='audit fails if any VM graph is unsupported')
    args = parser.parse_args(argv)
    try:
        if args.check and not args.output:
            raise ValueError('--check requires --output')
        if args.strict and not args.audit:
            raise ValueError('--strict requires --audit')
        image = RetailImage(args.exe)
        if args.audit:
            report = audit(image, args.character, args.return_to)
            output = json.dumps(report, indent=2, sort_keys=True) + '\n'
            status = 1 if args.strict and report['compilable'] != report['total'] else 0
        else:
            output = format_c(generate(image, args.script, args.character, args.return_to))
            status = 0
        if args.check:
            if not args.output.is_file() or args.output.read_text() != output:
                raise ValueError('generated output differs: %s' % args.output)
        elif args.output:
            args.output.write_text(output)
        else:
            sys.stdout.write(output)
        return status
    except (ValueError, OSError, subprocess.CalledProcessError) as exc:
        print(str(exc), file=sys.stderr)
        return 2


if __name__ == '__main__':
    sys.exit(main())
