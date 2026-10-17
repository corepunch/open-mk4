#!/usr/bin/env python3
"""Compile decoded retail scripts to C11 fibers, or audit compilation coverage."""
import argparse
import json
import os
import tempfile
from pathlib import Path
import subprocess
import sys

from character_catalog import character_id, CHARACTERS
from retail_moves import RetailImage
from retail_codegen import generate, format_c
from retail_coverage import audit, inventory, profile_for, summary

ROOT = Path(__file__).resolve().parents[1]


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--exe', type=Path, default=ROOT / 'MK4/MK-4.EXE')
    parser.add_argument('--character', type=character_id, default=0)
    action = parser.add_mutually_exclusive_group(required=True)
    action.add_argument('--script', type=lambda value: int(value, 16), action='append', help='repeat to emit several scripts atomically')
    action.add_argument('--audit', action='store_true')
    action.add_argument('--inventory', action='store_true', help='read typed EXE roots and graph fingerprints before implementation')
    action.add_argument('--generate', action='store_true', help='generate the character profile checked into tools/ports')
    parser.add_argument('--return-to', help='explicit controller/tagged-return engine continuation')
    parser.add_argument('--output', type=Path)
    parser.add_argument('--check', action='store_true', help='compare output without writing')
    parser.add_argument('--strict', action='store_true', help='audit fails for unreviewed roots, missing adapters or changed inventory')
    parser.add_argument('--require-generated', action='store_true', help='also fail audit when any catalogued VM root remains legacy/unreviewed')
    parser.add_argument('--summary', action='store_true', help='compact audit report; default is detailed JSON')
    args = parser.parse_args(argv)
    try:
        if args.generate:
            profile = profile_for(args.character)
            if profile is None:
                raise ValueError('No generation profile for ' + CHARACTERS[args.character])
            if profile.get('schema') != 1 or profile.get('character') != CHARACTERS[args.character]:
                raise ValueError('Invalid character generation profile')
            args.script = [int(entry, 16) for entry in profile['generated_scripts']]
            args.return_to = profile['return_to']
            if args.output is None:
                args.output = ROOT / profile['output']
        if args.check and not args.output:
            raise ValueError('--check requires --output')
        if (args.summary or args.require_generated) and not args.audit:
            raise ValueError('--summary/--require-generated require --audit')
        if args.strict and not args.audit:
            raise ValueError('--strict requires --audit')
        image = RetailImage(args.exe)
        if args.audit:
            report = audit(image, args.character, args.return_to)
            output = summary(report) if args.summary else json.dumps(report, indent=2, sort_keys=True) + '\n'
            status = 1 if (((args.strict or args.require_generated) and report['coverage_errors']) or
                           (args.require_generated and report['generated'] != report['total'])) else 0
        elif args.inventory:
            output = json.dumps(inventory(image, args.character), indent=2, sort_keys=True) + '\n'
            status = 0
        else:
            output = format_c(generate(image, args.script, args.character, args.return_to))
            status = 0
        if args.check:
            if not args.output.is_file() or args.output.read_text() != output:
                raise ValueError('generated output differs: %s' % args.output)
        elif args.output:
            temporary = None
            try:
                with tempfile.NamedTemporaryFile(mode='w', dir=args.output.parent, prefix='.' + args.output.name, suffix='.tmp', delete=False) as handle:
                    temporary = Path(handle.name)
                    handle.write(output)
                os.replace(temporary, args.output)
            finally:
                if temporary is not None:
                    temporary.unlink(missing_ok=True)
        else:
            sys.stdout.write(output)
        return status
    except (ValueError, OSError, subprocess.CalledProcessError) as exc:
        print(str(exc), file=sys.stderr)
        return 2


if __name__ == '__main__':
    sys.exit(main())
