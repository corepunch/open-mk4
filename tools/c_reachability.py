"""Conservative definition/reference graph for the project's procedural C modules.

Function addresses and data initializers are edges, as well as direct calls.
Prototypes, comments and manifest strings are not edges. Static symbols retain
translation-unit identity; cycles cannot make themselves reachable. This is a
source integration check, not proof that every runtime branch can execute.
"""
from functools import lru_cache
from pathlib import Path
import re

from c_source import ROOT, mask_c, private_aliases


#4344a0/434550 CPU finish selection uses the same scene dispatch entries
#as the reviewed input tables, independently of their human command gates.
CPU_FINISH_TARGETS = {
    'CPU_FINISH_1': '0x467ce0',
    'CPU_FINISH_2': '0x467cd0',
    'CPU_FINISH_FAN': '0x467e00',
    'CPU_FINISH_SPIKE': '0x467e70',
}


def cpu_finish_dispatch_errors(graph, profile):
    if not profile.get('controls_output'):
        return []
    expected = {slot: profile.get('command_bindings', {}).get(target)
                for slot, target in CPU_FINISH_TARGETS.items()}
    expected = {slot: ref for slot, ref in expected.items() if ref}
    if not expected:
        return []
    path = profile['integration_source']
    table = graph.find(path, 'retail_cpu_moves')
    body = graph.nodes.get(table, '')
    errors = []
    aliases = private_aliases(ROOT / path)
    for slot, reference in expected.items():
        name = reference.get('symbol')
        if not isinstance(name, str) or not re.fullmatch(r'\w+', name):
            errors.append('runtime CPU finish %s: missing reviewed symbol' % slot)
            continue
        symbol = aliases.get(name, name)
        match = re.search(r'\[\s*' + slot + r'\s*\]\s*=\s*&?\s*(\w+)', body)
        actual = match[1] if match else 'missing'
        if actual != symbol:
            errors.append('runtime CPU finish %s: expected %s, found %s (human reachability does not connect the CPU slot)' % (slot, symbol, actual))
    return errors


def definitions(source):
    source = mask_c(source)
    source = re.sub(r'^\s*#.*(?:\\\n.*)*', '', source, flags=re.M)
    start, depth = 0, 0
    for index, char in enumerate(source):
        if char == '{':
            depth += 1
        elif char == '}':
            depth -= 1
            if depth:
                continue
            head = source[start:source.index('{', start)]
            # A function body ends at }, while an initialized object ends at ;.
            if '=' not in head:
                match = re.search(r'\b(\w+)\s*\([^;{}]*\)\s*$', head)
                if match:
                    yield match[1], 'static' in head.split(), source[start:index + 1]
                    start = index + 1
        elif char == ';' and depth == 0:
            declaration = source[start:index]
            if '=' in declaration:
                head = declaration.split('=', 1)[0]
                match = re.search(r'\b(\w+)\s*(?:\[[^]]*\]\s*)*$', head)
                if match:
                    yield match[1], 'static' in head.split(), declaration
            start = index + 1


def translation_source(path, seen=None):
    seen = set() if seen is None else seen
    if path in seen:
        return ''
    seen.add(path)
    source = path.read_text()
    def include(match):
        child = path.parent / match[1]
        return translation_source(child.resolve(), seen) if child.is_file() else ''
    source = re.sub(r'^\s*#include\s+"([^"\n]+\.inc)"', include, source, flags=re.M)
    aliases = private_aliases(path)
    if aliases:
        source = re.sub(r'\b\w+\b', lambda m: aliases.get(m[0], m[0]), source)
    return source


class DefinitionGraph:
    def __init__(self, units):
        self.nodes, self.exports, self.locals = {}, {}, {}
        for path, source in units.items():
            local = self.locals.setdefault(path, {})
            for name, static, body in definitions(source):
                key = (path, name)
                self.nodes[key] = body
                local[name] = key
                if not static:
                    self.exports[name] = key
        self.edges = {}
        for key, body in self.nodes.items():
            local = self.locals[key[0]]
            refs = {local.get(name, self.exports.get(name)) for name in re.findall(r'\b\w+\b', body)}
            self.edges[key] = refs - {None, key}

    def reachable(self, roots, stop=()):
        todo = [self.exports[name] for name in roots if name in self.exports]
        reached = set(todo)
        while todo:
            for key in self.edges[todo.pop()] - reached:
                if key[1] in stop:
                    continue
                reached.add(key)
                todo.append(key)
        return reached

    def find(self, path, name):
        return self.locals.get(path, {}).get(name) or self.exports.get(name)


@lru_cache(maxsize=1)
def workspace_graph():
    paths = sorted((ROOT / 'game').rglob('*.c'))
    units = {path.relative_to(ROOT).as_posix(): translation_source(path) for path in paths}
    return DefinitionGraph(units)


def weapon_dispatch_cycles(graph, reached, directory):
    """An armed callback must implement its move, not press its own switch again."""
    fields = ('type', 'hp', 'lp', 'back_hp', 'back_lp', 'crouch_lp', 'throw_move', 'equip', 'model')
    buttons = {'hp': 'BUTTON_HIGH_PUNCH', 'back_hp': 'BUTTON_HIGH_PUNCH',
               'lp': 'BUTTON_LOW_PUNCH', 'back_lp': 'BUTTON_LOW_PUNCH', 'crouch_lp': 'BUTTON_LOW_PUNCH'}
    errors = []
    for key in sorted(reached):
        path, name = key
        body = graph.nodes[key]
        head = body.split('=', 1)[0]
        if not path.startswith(directory) or not re.search(r'\bWeaponDef\b', head) or '=' not in body:
            continue
        initializer = body.split('=', 1)[1].strip().strip('{} \n')
        values = initializer.split(',')
        callbacks = dict(zip(fields, values))
        if any(re.match(r'\s*\.', value) for value in values):
            callbacks = {match[1]: match[2] for match in re.finditer(r'\.(\w+)\s*=\s*(\w+)', initializer)}
        for field, button in buttons.items():
            callback = callbacks.get(field, '').strip()
            if not re.fullmatch(r'\w+', callback):
                continue
            target = graph.find(path, callback)
            if target and re.search(r'\bfighter_press\s*\([^,;{}]+,\s*' + button + r'\s*\)', graph.nodes[target]):
                errors.append('weapon %s:%s.%s: callback %s redispatches its own %s instead of implementing the armed move' %
                              (path, name, field, callback, button))
    return errors


def unreachable_references(profile, compiled_graphs=None):
    graph = workspace_graph()
    # The roster lookup is an indirect runtime selection, not an edge from
    # every fighter to every other fighter's command tables.
    reached = graph.reachable(['mk4_' + profile['character']], stop={'roster'})
    errors = []
    references = []
    for section in ('moves', 'entry_adapters', 'stances', 'command_bindings', 'cpu_bindings'):
        references.extend((section + ' ' + label, ref) for label, ref in profile.get(section, {}).items())
    if profile.get('controls_output'):
        references.append(('runtime command table', {'source': profile['integration_source'], 'symbol': 'retail_commands'}))
    if profile.get('cpu_bindings') or (profile.get('controls_output') and any(target in profile.get('command_bindings', {}) for target in CPU_FINISH_TARGETS.values())):
        references.append(('runtime CPU table', {'source': profile['integration_source'], 'symbol': 'retail_cpu_moves'}))
    reported = set()
    for label, ref in references:
        path, name = ref.get('source'), ref.get('symbol')
        if not path or not name:
            continue
        name = private_aliases(ROOT / path).get(name, name)
        # Profiles historically name character.c as the private module boundary.
        candidates = [graph.find(path, name)]
        if Path(path).name == 'character.c':
            candidates += [graph.find(p, name) for p in graph.locals if p.startswith(str(Path(path).parent / 'moves') + '/')]
        candidates = [key for key in candidates if key]
        if candidates and not any(key in reached for key in candidates):
            errors.append('%s: unreachable implementation %s (not connected to gameplay)' % (label, name))
            reported.update(candidates)
    # A manifest is a review index, not the universe of possible omissions.
    # Inspect authored callbacks and control tables even if nobody listed them
    # in port.json. Generated VM bodies and their mechanical integration thunks
    # are checked through the catalog roots above, not as independent moves.
    directory = 'game/' + profile['character'] + '/'
    errors.extend(weapon_dispatch_cycles(graph, reached, directory))
    errors.extend(cpu_finish_dispatch_errors(graph, profile))
    # A generated phase may be entered through its native loop coordinator.
    # Linked continuations can be folded into another generated C function;
    # use the compiler's decoded CFG, not a requirement to call every wrapper.
    live_programs = set()
    for path, name in reached:
        match = re.match(r'retail_script_([0-9a-fA-F]{8})(?:_|$)', name)
        if path.startswith(directory) and match:
            live_programs.add(int(match[1], 16))
    implemented = set(live_programs)
    for entry in live_programs:
        implemented.update((compiled_graphs or {}).get(entry, ()))
    for entry in profile.get('generated_scripts', ()):
        if int(entry, 16) not in implemented:
            errors.append('generated script %s: compiled implementation is unreachable from gameplay' % entry)
    for key, body in graph.nodes.items():
        path, name = key
        if not path.startswith(directory) or key in reached or key in reported:
            continue
        head = body.split('{', 1)[0]
        function = '=' not in head and re.search(r'\b' + re.escape(name) + r'\s*\(', head)
        table = re.search(r'\b(?:ButtonTable|CommandDef|Controls|CpuMoves)\b', head)
        generated_thunk = path == directory + 'character.c' and (name.startswith('retail_script_') or name.startswith(profile['character'] + '_entry_retail_script_'))
        if (function or table) and not generated_thunk:
            errors.append('unreachable move definition %s:%s (not connected to gameplay)' % key)
    return errors
