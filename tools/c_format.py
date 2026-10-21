"""Canonical formatting for project-owned native C fixtures."""
import os
from pathlib import Path
import shutil
import subprocess


def format_c(source):
    """Canonical repository formatting; fixture generation/check use the same path."""
    root = Path(__file__).resolve().parents[1]
    formatter = os.environ.get('CLANG_FORMAT') or shutil.which('clang-format')
    if formatter is None:
        result = subprocess.run(['xcrun', '--find', 'clang-format'], capture_output=True, text=True)
        formatter = result.stdout.strip() if result.returncode == 0 else None
    if not formatter:
        raise ValueError('clang-format is required for canonical C fixtures')
    command = [formatter, '--style=file', '--assume-filename=' + str(root / 'game/native_fixture.inc')]
    # clang-format can move a trailing comment out of an empty labeled block
    # on its first pass and then reflow that block on its second pass. Generated
    # output must match the repository's in-place formatter, so use its stable
    # result rather than a one-pass intermediate.
    formatted = subprocess.run(command, input=source, text=True, capture_output=True, check=True).stdout
    return subprocess.run(command, input=formatted, text=True, capture_output=True, check=True).stdout
