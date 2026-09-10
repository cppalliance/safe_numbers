#!/usr/bin/env python3
# Copyright 2026 Matt Borland
# Distributed under the Boost Software License, Version 1.0.
# https://www.boost.org/LICENSE_1_0.txt
#
# Summarize a benchmark_policy_operations or benchmark_library_comparison JSON
# run (Google Benchmark, median aggregates) as the ratio of every contender to
# the built-in type, per kernel shape, width, and operation, with the geometric
# mean across operations. Feeds the ablation and comparison tables in
# safe_numbers_seip.tex.
#
# Usage:
#   python3 doc/paper/ablation_summary.py results.json [--shape stream|scalar] [--latex]

import argparse
import collections
import json
import math
import re

RUN_NAME = re.compile(r"(scalar|stream)_(unsigned|signed)_(\d+)_(add|sub|mul|div|mod)_(.+)$")
OPS = ["add", "sub", "mul", "div", "mod"]

ROLE_LABELS = {
    "builtin": "built-in",
    "throwing": "throwing (default)",
    "saturating": "saturating",
    "strict": "strict",
    "wrapping_handler": "user handler, wrapping",
    "throwing_handler": "user handler, throwing",
    "overflowing": "overflowing (deferred)",
    "checked": "checked (optional)",
    "throwing_bounded_ingress": "throwing, bounded ingress",
    "builtin_byte_ingress": "built-in, byte ingress",
    "safe_numbers": "Boost.SafeNumbers",
    "safe_numerics": "Boost.SafeNumerics",
    "safeint": "Microsoft SafeInt",
    "intel": "Intel safe-arithmetic",
    "intel_throw": "Intel s.-a., throwing fallback",
    "intel_unchecked": "Intel s.-a., no runtime check",
}

ROLE_ORDER = ["throwing", "saturating", "strict", "wrapping_handler", "throwing_handler",
              "overflowing", "checked", "throwing_bounded_ingress", "builtin_byte_ingress", "safe_numbers", "safe_numerics", "safeint", "intel",
              "intel_throw", "intel_unchecked"]


def load_medians(path):
    medians = {}
    with open(path) as f:
        data = json.load(f)
    for entry in data["benchmarks"]:
        if entry.get("aggregate_name") == "median":
            medians[entry["run_name"]] = entry["real_time"]
    return medians


def geometric_mean(values):
    return math.exp(sum(math.log(v) for v in values) / len(values))


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("json_path")
    parser.add_argument("--shape", choices=["scalar", "stream"], default=None)
    parser.add_argument("--latex", action="store_true")
    parser.add_argument("--ns", action="store_true", help="print built-in ns per element as well")
    args = parser.parse_args()

    medians = load_medians(args.json_path)

    # (shape, kind, width) -> role -> op -> ratio
    table = collections.defaultdict(lambda: collections.defaultdict(dict))
    baseline = {}
    for name, value in medians.items():
        match = RUN_NAME.match(name)
        if not match:
            continue
        shape, kind, width, op, role = match.groups()
        if args.shape and shape != args.shape:
            continue
        base = medians.get("{}_{}_{}_{}_builtin".format(shape, kind, width, op))
        if not base:
            continue
        baseline[(shape, kind, width, op)] = base
        if role == "builtin":
            continue
        table[(shape, kind, width)][role][op] = value / base

    for key in sorted(table, key=lambda k: (k[0], k[1], int(k[2]))):
        shape, kind, width = key
        alias = ("u" if kind == "unsigned" else "i") + width
        if not args.latex:
            print("== {} {} (ratio to the built-in type)".format(shape, alias))
            if args.ns:
                print("   built-in ns/element: " + " ".join(
                    "{}={:.3f}".format(op, baseline[(shape, kind, width, op)] / 1e6)
                    for op in OPS if (shape, kind, width, op) in baseline))
            print("{:26s} {:>6s} {:>6s} {:>6s} {:>6s} {:>6s} {:>7s}".format("contender", *OPS, "gmean"))
        roles = [r for r in ROLE_ORDER if r in table[key]] + sorted(r for r in table[key] if r not in ROLE_ORDER)
        for role in roles:
            ratios = table[key][role]
            cells = [ratios.get(op) for op in OPS]
            present = [c for c in cells if c is not None]
            gm = geometric_mean(present) if present else float("nan")
            label = ROLE_LABELS.get(role, role)
            if args.latex:
                print("\\code{{{}}} & {} & {} & {:.2f} \\\\".format(
                    alias, label, " & ".join("{:.2f}".format(c) if c is not None else "n/a" for c in cells), gm))
            else:
                print("{:26s} {} {:7.2f}".format(
                    label, " ".join("{:6.2f}".format(c) if c is not None else "   n/a" for c in cells), gm))
        if not args.latex:
            print()


if __name__ == "__main__":
    main()
