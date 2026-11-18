#!/usr/bin/env python3
# Copyright 2026 Matt Borland
# Distributed under the Boost Software License, Version 1.0.
# https://www.boost.org/LICENSE_1_0.txt
"""Amalgamate boost.safe_numbers into a single header.

Starts from the umbrella header and recursively inlines every internal
<boost/safe_numbers/...> include, plus quoted includes relative to the
including file (e.g. "detail/int128/int128.hpp"), in DFS pre-order so
declarations precede use. System includes and the external Boost
dependencies (<boost/throw_exception.hpp>, <boost/core/...>,
<boost/charconv.hpp>, ...) are left untouched, so the result still needs
Boost on the include path. See extra/README.md for Compiler Explorer use.

A header is inlined once at its first unconditional point of use. If it has
so far only been inlined inside an #if block, a later include expands it
again so its contents are never trapped behind an unrelated condition;
include guards make the repeat harmless.

Run with no arguments to regenerate
extra/single_include/boost/safe_numbers.hpp from include/boost/safe_numbers.hpp.
"""

import argparse
import re
import sys
from pathlib import Path

ANGLE_RE = re.compile(r'^\s*#\s*include\s*<(boost/safe_numbers(?:/[^>]+)?\.hpp)>\s*(?://.*)?$')
QUOTED_RE = re.compile(r'^\s*#\s*include\s*"([^"]+)"\s*(?://.*)?$')
IF_RE = re.compile(r'^\s*#\s*(?:if|ifdef|ifndef)\b')
ENDIF_RE = re.compile(r'^\s*#\s*endif\b')
IFNDEF_RE = re.compile(r'^\s*#\s*ifndef\s+(\w+)\s*$')
DEFINE_RE = re.compile(r'^\s*#\s*define\s+(\w+)\s*$')

# Any surviving include of these forms means the output is not self-contained
LEFTOVER_RE = re.compile(r'^\s*#\s*include\s*(?:"|<boost/safe_numbers[/.>])')

PREAMBLE = [
    '// boost.safe_numbers: amalgamated single header build.',
    '// Auto-generated: all internal boost/safe_numbers headers inlined into one file',
    '// for use in Compiler Explorer and other single-file environments.',
    '// External Boost dependencies (Config, Core, ThrowException, Charconv) are still',
    '// required on the include path.',
    '// Do not edit by hand; regenerate with extra/amalgamate.py.',
    '//',
    '// Copyright 2026 Matt Borland',
    '// Distributed under the Boost Software License, Version 1.0.',
    '// https://www.boost.org/LICENSE_1_0.txt',
    '',
]


# Detect a classic #ifndef/#define include guard as the first significant lines
def has_include_guard(lines):
    significant = [line for line in lines if line.strip() and not line.strip().startswith('//')]
    if len(significant) < 2:
        return False
    guard = IFNDEF_RE.match(significant[0])
    definition = DEFINE_RE.match(significant[1])
    return bool(guard and definition and guard.group(1) == definition.group(1))


class Amalgamator:
    def __init__(self, include_root):
        self.include_root = include_root.resolve()
        self.visited = {}  # rel path -> 'unconditional' or 'conditional'
        self.out_lines = []

    # Map an include directive in rel_path to an include-root-relative header, or None if external
    def resolve(self, line, rel_path):
        match = ANGLE_RE.match(line)
        if match:
            return match.group(1)

        match = QUOTED_RE.match(line)
        if not match:
            return None

        target = ((self.include_root / rel_path).parent / match.group(1)).resolve()
        if not target.is_file():
            sys.exit(f'amalgamate: quoted include "{match.group(1)}" in {rel_path} not found')
        try:
            return target.relative_to(self.include_root).as_posix()
        except ValueError:
            sys.exit(f'amalgamate: quoted include "{match.group(1)}" in {rel_path} is outside {self.include_root}')

    # Inline one header: emit its lines, recursing into internal includes.
    # stack holds the headers being expanded; conditional is True inside an #if block.
    def inline(self, rel_path, stack, conditional):
        full = self.include_root / rel_path
        if not full.is_file():
            sys.exit(f'amalgamate: internal header {rel_path} not found under {self.include_root}')

        if not conditional:
            self.visited[rel_path] = 'unconditional'
        elif rel_path not in self.visited:
            self.visited[rel_path] = 'conditional'

        lines = full.read_text(encoding='utf-8').splitlines()
        guard_depth = 1 if has_include_guard(lines) else 0
        depth = 0

        self.out_lines.append(f'// ===== BEGIN {rel_path} =====')
        for line in lines:
            dep = self.resolve(line, rel_path)
            if dep is not None:
                dep_conditional = conditional or depth > guard_depth
                if dep in stack or self.visited.get(dep) == 'unconditional':
                    self.out_lines.append(f'// [amalgamate] skipped duplicate include of {dep}')
                else:
                    self.inline(dep, stack + [rel_path], dep_conditional)
            else:
                if IF_RE.match(line):
                    depth += 1
                elif ENDIF_RE.match(line):
                    depth -= 1
                self.out_lines.append(line)
        self.out_lines.append(f'// ===== END {rel_path} =====')


def main():
    repo_root = Path(__file__).resolve().parent.parent
    parser = argparse.ArgumentParser(description='Amalgamate boost.safe_numbers into a single header')
    parser.add_argument('include_root', nargs='?', type=Path, default=repo_root / 'include',
                        help='path to the include directory (default: <repo>/include)')
    parser.add_argument('top', nargs='?', default='boost/safe_numbers.hpp',
                        help='umbrella header to start from (default: boost/safe_numbers.hpp)')
    parser.add_argument('out', nargs='?', type=Path,
                        default=repo_root / 'extra' / 'single_include' / 'boost' / 'safe_numbers.hpp',
                        help='output file (default: <repo>/extra/single_include/boost/safe_numbers.hpp)')
    args = parser.parse_args()

    amalgamator = Amalgamator(args.include_root)
    amalgamator.out_lines.extend(PREAMBLE)
    amalgamator.inline(args.top, [], False)

    leftovers = [line.strip() for line in amalgamator.out_lines if LEFTOVER_RE.match(line)]
    if leftovers:
        sys.exit('amalgamate: output still contains internal includes:\n  ' + '\n  '.join(leftovers))

    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text('\n'.join(amalgamator.out_lines) + '\n', encoding='utf-8')
    print(f'Wrote {args.out} ({len(amalgamator.out_lines)} lines, {len(amalgamator.visited)} headers inlined)')


if __name__ == '__main__':
    main()
