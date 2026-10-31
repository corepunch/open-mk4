"""Symbolic native imports shared by the assembler and typed C registry emitter."""
import re

ACTOR = {'entry', 'reaction', 'throw_reaction', 'engine_reaction', 'call', 'tail', 'handler', 'physics'}
GLOBAL = {'call_global', 'tail_global'}
RESULT = {'call_result', 'predicate_result'}
ARGUMENT = {'spawn', 'handler_arg', 'handler_checked'}
REFERENCE = {'tail_selector', 'tail_reference'}
FIELDS = ACTOR | GLOBAL | RESULT | ARGUMENT | REFERENCE | {'gate', 'task', 'tail_script', 'tail_finish'}


def declaration(line):
    parts = [part.strip() for part in line.removeprefix('.native ').split(',')]
    name = parts.pop(0)
    if not re.fullmatch(r'native_[A-Za-z_]\w*', name):
        raise ValueError('Invalid native symbol: ' + name)
    fields = {}
    for part in parts:
        key, separator, value = part.partition('=')
        if not separator or key not in FIELDS or key in fields:
            raise ValueError('Invalid/duplicate native binding: ' + part)
        if key == 'task':
            if not value.isdecimal() or not 0 <= int(value) <= 0xffffffff:
                raise ValueError('Invalid native task type')
        elif not re.fullmatch(r'[A-Za-z_]\w*', value):
            raise ValueError('Invalid native implementation symbol: ' + value)
        fields[key] = value
    if ('spawn' in fields) != ('task' in fields):
        raise ValueError('Native spawn requires its task type')
    if ('tail_script' in fields) != ('tail_finish' in fields):
        raise ValueError('Native script link requires its completion')
    for role in ('call', 'tail', 'handler', 'predicate'):
        implementations = [key for key in fields if key == role or key.startswith(role + '_') and key != 'tail_finish']
        if len(implementations) > 1:
            raise ValueError('Conflicting native ' + role + ' implementations')
    return name, fields


def c_registry(natives, symbols):
    """Emit real, type-checked C pointers, never casts or code-address switches."""
    declarations, rows = {}, []
    for name, fields in sorted(natives.items()):
        values = [f'.name = "{name}"']
        for key, value in fields.items():
            if key == 'tail_script':
                if 'fn_' + value not in symbols:
                    raise ValueError('Native link names a non-script symbol: ' + value)
                values.append('.tail_script = fn_' + value)
                continue
            if key == 'tail_finish' and value == 'inherit':
                values.append('.tail_inherit = true')
                continue
            if key == 'task':
                values.append('.task = ' + value)
                continue
            if key in ACTOR or key == 'tail_finish':
                signature = 'void {name}(Fighter *fighter);'
            elif key in GLOBAL:
                signature = 'void {name}(void);'
            elif key in RESULT:
                signature = 'unsigned {name}(Fighter *fighter);'
            elif key in ARGUMENT:
                signature = 'void {name}(Fighter *fighter, int argument);'
            elif key in REFERENCE:
                signature = 'void {name}(Fighter *fighter, uint32_t argument);'
            elif key == 'gate':
                signature = 'bool {name}(const Fighter *fighter);'
            else:
                raise ValueError('Unknown native field ' + key)
            prototype = signature.format(name=value)
            if value in declarations and declarations[value] != prototype:
                raise ValueError('Incompatible native signatures for ' + value)
            declarations[value] = prototype
            values.append('.' + key + ' = ' + value)
        rows.append('  [' + name + '] = {' + ', '.join(values) + '},')
    return '\n'.join(['/* Generated symbolic native links; do not edit. */', *sorted(declarations.values()),
                      '// clang-format off: Keep table entries on one row, including long rows.',
                      'static const NativeSymbol native_symbols[MK4_NATIVE_COUNT] = {', *rows, '};', '// clang-format on', ''])
