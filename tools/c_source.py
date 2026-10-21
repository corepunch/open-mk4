"""Resolve native C definitions and retain real file/line navigation."""
from pathlib import Path
from functools import lru_cache
from native_source_aliases import LEGACY_NATIVE_NAMES
import re

ROOT = Path(__file__).resolve().parents[1]


def source_files(path):
    path = (ROOT / path).resolve()
    result, seen = [], set()

    def visit(current):
        if current in seen:
            return
        seen.add(current)
        text = current.read_text()
        result.append(current)
        for include in re.findall(r'^\s*#include\s+"([^"\n]+\.inc)"', text, re.M):
            child = (current.parent / include).resolve()
            if 'generated' not in child.parts and child.is_file():
                visit(child)

    if path.is_file():
        visit(path)
    elif path.name != 'character.c':
        raise FileNotFoundError(path)
    if path.name == 'character.c':
        for child in sorted((path.parent / 'moves').glob('*.c')):
            visit(child)
    return result


def read_source(path):
    # Generated includes stay explicit boundaries rather than making every
    # generated root appear referenced by its own integration source.
    return '\n'.join(path.read_text() for path in source_files(path))


def mask_c(text):
    return re.sub(r'/\*.*?\*/|//[^\n]*|"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'',
                  lambda match: re.sub(r'[^\n]', ' ', match[0]), text, flags=re.S)


def private_aliases(path):
    path = (ROOT / path).resolve()
    header = path.parent / 'character.h'
    if not header.is_file() and path.parent.name == 'moves':
        header = path.parent.parent / 'character.h'
    if not header.is_file():
        return {}
    return dict(re.findall(r'^#define (\w+) (\w+)$', header.read_text(), re.M))


def canonical_name(path, name):
    name = private_aliases(path).get(name, name)
    name = LEGACY_NATIVE_NAMES.get(name, name)
    try:
        scope = (ROOT / path).resolve().relative_to(ROOT / 'game').parts[0]
    except (ValueError, IndexError):
        return name
    if '.' not in scope:
        qualified = LEGACY_NATIVE_NAMES.get(scope + '_' + name, scope + '_' + name)
        if qualified in public_definitions():
            return qualified
    return name


@lru_cache(maxsize=1)
def public_definitions():
    """Index unique native definitions so old research paths follow relocation."""
    result = {}
    pattern = r'^[A-Za-z_][\w* \t]*\b(\w+)\s*\([^;{}]*\)\s*\{'
    for path in sorted((ROOT / 'game').rglob('*.c')):
        source = mask_c(path.read_text())
        for match in re.finditer(pattern, source, re.M):
            if source[match.start():match.end()].startswith('static '):
                continue
            result.setdefault(match[1], set()).add(path.relative_to(ROOT).as_posix())
    return result


def function_source(path, name):
    name = canonical_name(path, name)
    try:
        candidates = source_files(path)
    except FileNotFoundError:
        candidates = []
    for candidate in candidates:
        text = candidate.read_text()
        masked = mask_c(text)
        match = re.search(r'^[A-Za-z_][\w* \t]*\b' + re.escape(name) + r'\s*\([^;{}]*\)\s*\{', masked, re.M)
        if match is None:
            continue
        depth, end = 1, match.end()
        while depth and end < len(masked):
            depth += (masked[end] == '{') - (masked[end] == '}')
            end += 1
        if depth:
            raise ValueError('Unbalanced portable C definition ' + name)
        return candidate.relative_to(ROOT).as_posix(), text.count('\n', 0, match.start()) + 1, text[match.start():end]
    relocated = public_definitions().get(name, set())
    if len(relocated) == 1 and (name in LEGACY_NATIVE_NAMES.values() or name.startswith("retail_")):
        actual = next(iter(relocated))
        if (ROOT / actual).resolve() not in candidates:
            return function_source(actual, name)
    raise ValueError('Missing portable C definition %s in %s' % (name, path))
