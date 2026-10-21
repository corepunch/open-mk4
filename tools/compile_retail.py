#!/usr/bin/env python3
"""Read-only retail inventory and shared-runtime coverage audits.

The historical filename is retained for research commands. Script-to-C/frame
translation is retired; inspect individual scripts with disassemble_retail.py.
"""
import argparse
import json
from pathlib import Path
import sys

from character_catalog import character_id, CHARACTERS
from retail_moves import RetailImage
from retail_coverage import audit, inventory, summary

ROOT = Path(__file__).resolve().parents[1]


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--exe', type=Path, default=ROOT / 'MK4/MK-4.EXE')
    selection = parser.add_mutually_exclusive_group()
    selection.add_argument('--character', type=character_id, default=0)
    selection.add_argument('--all', action='store_true', help='audit all roster slots in one process')
    action = parser.add_mutually_exclusive_group(required=True)
    action.add_argument('--audit', action='store_true')
    action.add_argument('--inventory', action='store_true')
    parser.add_argument('--summary', action='store_true')
    parser.add_argument('--strict', action='store_true')
    parser.add_argument('--require-implemented', action='store_true')
    parser.add_argument('--output', type=Path, help='save the inventory/audit report')
    args = parser.parse_args(argv)
    if (args.summary or args.strict or args.require_implemented) and not args.audit:
        parser.error('--summary/--strict/--require-implemented require --audit')
    try:
        image = RetailImage(args.exe)
        selected = range(len(CHARACTERS)) if args.all else [args.character]
        reports, status = [], 0
        for character in selected:
            report = audit(image, character) if args.audit else inventory(image, character)
            reports.append(report)
            if args.audit and ((args.strict and report['coverage_errors']) or
                               (args.require_implemented and (report['coverage_errors'] or report['runtime'] != report['total']))):
                status = 1
        output = ''.join(summary(report) for report in reports) if args.summary else json.dumps(reports if args.all else reports[0], indent=2, sort_keys=True) + '\n'
        if args.output:
            args.output.write_text(output)
        else:
            sys.stdout.write(output)
        return status
    except (ValueError, OSError) as exc:
        print(str(exc), file=sys.stderr)
        return 2


if __name__ == '__main__':
    sys.exit(main())
