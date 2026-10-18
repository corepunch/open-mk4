#!/usr/bin/env python3
"""Simplify the control flow emitted by the retail script compiler."""
import re


LABEL = re.compile(r'^L_([0-9a-f]{8}): \{$')
GOTO = re.compile(r'^\s*goto L_([0-9a-f]{8});$')


def _blocks(lines):
    """Return generated instruction blocks as (label, start, end) line indexes."""
    blocks = []
    index = 0
    while index < len(lines):
        match = LABEL.match(lines[index])
        if not match:
            index += 1
            continue
        depth = 0
        end = index
        while end < len(lines):
            depth += lines[end].count('{') - lines[end].count('}')
            if depth == 0:
                break
            end += 1
        if end == len(lines):
            raise ValueError('unterminated generated instruction block')
        blocks.append((match.group(1), index, end))
        index = end + 1
    return blocks


def _drop_fallthrough_gotos(lines):
    blocks = _blocks(lines)
    labels = {label: start for label, start, _ in blocks}
    block_by_line = {index: position for position, (_, start, end) in enumerate(blocks)
                     for index in range(start, end + 1)}
    result = []
    for index, line in enumerate(lines):
        match = GOTO.match(line)
        if match:
            source_block = block_by_line.get(index)
            destination_start = labels.get(match.group(1))
            if source_block is not None:
                if (index == blocks[source_block][2] - 1 and source_block + 1 < len(blocks) and
                        blocks[source_block + 1][1] == destination_start):
                    continue
            elif destination_start is not None:
                next_code = index + 1
                while next_code < len(lines) and not lines[next_code].strip():
                    next_code += 1
                if next_code == destination_start:
                    continue
        result.append(line)
    return result


def _structure_simple_loops(lines):
    blocks = _blocks(lines)
    label_starts = {label: start for label, start, _ in blocks}
    gotos = [(index, match.group(1)) for index, line in enumerate(lines)
             if (match := GOTO.match(line))]
    for backedge_index, target in reversed(gotos):
        target_start = label_starts.get(target)
        if target_start is None or target_start >= backedge_index:
            continue
        backedge_block = next((block for block in blocks if block[1] <= backedge_index <= block[2]), None)
        target_block = next((block for block in blocks if block[0] == target), None)
        if backedge_block is None or target_block is None:
            continue
        label, _, backedge_end = backedge_block
        conditional = backedge_index == backedge_end - 2 and lines[backedge_index - 1].lstrip().startswith('if (')
        unconditional = backedge_index == backedge_end - 1
        if not (conditional or unconditional) or lines[backedge_index].strip() != 'goto L_%s;' % target:
            continue
        region_start, region_end = target_block[1], backedge_end
        region_labels = {item[0] for item in blocks if region_start <= item[1] <= region_end}
        safe = True
        for source_index, destination in gotos:
            if destination in region_labels and destination != target and not region_start <= source_index <= region_end:
                safe = False
                break
        if not safe:
            continue
        # Don't form overlapping or nested loops in this deliberately small
        # post-processor; keep those CFGs explicit for the C compiler.
        if any(line == 'for (;;) {' for line in lines[max(0, region_start - 1):region_end + 2]):
            continue
        lines[backedge_index] = lines[backedge_index][:len(lines[backedge_index]) - len(lines[backedge_index].lstrip())] + 'continue;'
        if conditional:
            lines.insert(backedge_index + 2, '  break;')
            region_end += 1
        lines.insert(region_end + 1, '  }')
        lines.insert(region_start, '  for (;;) {')
        return lines, True
    return lines, False


def postprocess(source):
    """Remove redundant jumps and structure straightforward infinite loops."""
    lines = source.splitlines()
    function_starts = [index for index, line in enumerate(lines) if line.startswith('static inline void ')]
    if not function_starts:
        return source
    output = lines[:function_starts[0]]
    for start_index, start in enumerate(function_starts):
        end = function_starts[start_index + 1] if start_index + 1 < len(function_starts) else len(lines)
        function = _drop_fallthrough_gotos(lines[start:end])
        while True:
            function, changed = _structure_simple_loops(function)
            if not changed:
                break
        referenced = {match.group(1) for line in function if (match := GOTO.match(line))}
        output.extend(re.sub(r'^L_([0-9a-f]{8}): ', '', line)
                      if LABEL.match(line) and LABEL.match(line).group(1) not in referenced else line
                      for line in function)
    return '\n'.join(output) + ('\n' if source.endswith('\n') else '')
