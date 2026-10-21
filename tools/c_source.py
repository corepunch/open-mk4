"""Resolve native C definitions and retain real file/line navigation."""
from pathlib import Path
from functools import lru_cache
from native_source_aliases import LEGACY_NATIVE_NAMES, LEGACY_NATIVE_FILES, LEGACY_LOCAL_NAMES
import re

ROOT = Path(__file__).resolve().parents[1]


@lru_cache(maxsize=256)
def _source_text(path, modified):
    text = path.read_text()
    return text, mask_c(text)


def source_text(path):
    return _source_text(path, path.stat().st_mtime_ns)


def source_files(path):
    path = (ROOT / path).resolve()
    historical = path.relative_to(ROOT).as_posix() if path.is_relative_to(ROOT) else ''
    result, seen = [], set()

    def visit(current):
        if current in seen:
            return
        seen.add(current)
        text = source_text(current)[0]
        result.append(current)
        for include in re.findall(r'^\s*#include\s+"([^"\n]+\.inc)"', text, re.M):
            child = (current.parent / include).resolve()
            if 'generated' not in child.parts and child.is_file():
                visit(child)

    if path.is_file():
        visit(path)
    elif historical in LEGACY_NATIVE_FILES:
        visit(ROOT / LEGACY_NATIVE_FILES[historical])
    elif path.name != 'character.c':
        raise FileNotFoundError(path)
    if path.name == 'character.c':
        prefix = path.parent.relative_to(ROOT).as_posix() + '/moves/'
        for child in sorted({target for old, target in LEGACY_NATIVE_FILES.items() if old.startswith(prefix)}):
            visit(ROOT / child)
    return result


def read_source(path):
    # Generated includes stay explicit boundaries rather than making every
    # generated root appear referenced by its own integration source.
    return '\n'.join(source_text(path)[0] for path in source_files(path))


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


def native_alias(name):
    for _ in range(len(LEGACY_NATIVE_NAMES) + 1):
        target = LEGACY_NATIVE_NAMES.get(name, name)
        if target == name:
            return name
        name = target
    raise ValueError('Cyclic native research alias')


def canonical_name(path, name):
    actual = (ROOT / path).resolve()
    if actual.is_relative_to(ROOT):
        historical = actual.relative_to(ROOT).as_posix()
        renamed = LEGACY_LOCAL_NAMES.get(historical + '::' + name)
        if renamed is None and actual.name == 'character.c':
            prefix = actual.parent.relative_to(ROOT).as_posix() + '/moves/'
            matches = {value for key, value in LEGACY_LOCAL_NAMES.items()
                       if key.startswith(prefix) and key.endswith('::' + name)}
            if len(matches) == 1:
                renamed = next(iter(matches))
        name = renamed or name
    name = private_aliases(path).get(name, name)
    name = native_alias(name)
    try:
        scope = (ROOT / path).resolve().relative_to(ROOT / 'game').parts[0]
    except (ValueError, IndexError):
        return name
    if '.' not in scope:
        qualified = native_alias(scope + '_' + name)
        if qualified in public_definitions():
            return qualified
    return name


@lru_cache(maxsize=1)
def public_definitions():
    """Index unique native definitions so old research paths follow relocation."""
    result = {}
    pattern = r'^[A-Za-z_][\w* \t]*\b(\w+)\s*\([^;{}]*\)\s*\{'
    for path in sorted((ROOT / 'game').rglob('*.c')):
        source = source_text(path)[1]
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
        text, masked = source_text(candidate)
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
