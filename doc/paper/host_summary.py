#!/usr/bin/env python3
# Copyright 2026 Matt Borland
# Distributed under the Boost Software License, Version 1.0.
# https://www.boost.org/LICENSE_1_0.txt
#
# Regenerate the host overhead summary (Table IV of safe_numbers_seip.tex) from
# the Google Benchmark JSON artifacts under doc/modules/ROOT/data/benchmarks-*.
#
# For every platform and type class the script reports the geometric mean, the
# median, and the maximum of the per-operation ratio (wrapper median time over
# built-in median time), for both Boost.SafeNumbers ("sn") and
# Boost.SafeNumerics ("safe"). Pure standard library only.
#
# Usage:
#   python3 doc/paper/host_summary.py [--data-root DIR] [--latex]

import argparse
import collections
import glob
import json
import math
import os
import re

RUN_NAME = re.compile(r"(unsigned|signed|float)_(\d+)_(\w+?)_(builtin|sn|safe)$")

CLASS_ORDER = ["int8-64", "128", "float"]
WHO_ORDER = ["sn", "safe"]

PLATFORM_LABELS = {
    "linux-x64": ("Linux x64", "GCC 14"),
    "linux-arm64": ("Linux ARM64", "GCC 14"),
    "linux-x86-32": ("Linux x86 32-bit", "GCC 14"),
    "macos-arm64": ("macOS ARM64", "Apple Clang"),
    "windows-x64": ("Windows x64", "MSVC"),
    "windows-arm64": ("Windows ARM64", "MSVC"),
    "windows-x86-32": ("Windows x86 32-bit", "MSVC"),
}

PLATFORM_ORDER = [
    "linux-x64", "linux-arm64", "linux-x86-32", "macos-arm64",
    "windows-x64", "windows-arm64", "windows-x86-32",
]


def load_medians(platform_dir):
    # Returns run_name -> median real time (ns per 1e6 elements) for one platform.
    medians = {}
    for suite in ("unsigned", "signed", "float"):
        path = os.path.join(platform_dir, suite + ".json")
        if not os.path.exists(path):
            continue
        with open(path) as f:
            data = json.load(f)
        for entry in data["benchmarks"]:
            if entry.get("aggregate_name") == "median":
                medians[entry["run_name"]] = entry["real_time"]
    return medians


def type_class(kind, width):
    if width == "128":
        return "128"
    if kind == "float":
        return "float"
    return "int8-64"


def summarize(medians):
    # Returns (class, who) -> list of ratios against the built-in baseline.
    groups = collections.defaultdict(list)
    for name, value in medians.items():
        match = RUN_NAME.match(name)
        if not match:
            continue
        kind, width, op, who = match.groups()
        if who == "builtin":
            continue
        base = medians.get("{}_{}_{}_builtin".format(kind, width, op))
        if not base:
            continue
        groups[(type_class(kind, width), who)].append(value / base)
    return groups


def geometric_mean(values):
    return math.exp(sum(math.log(v) for v in values) / len(values))


def median(values):
    ordered = sorted(values)
    return ordered[len(ordered) // 2]


def cell(values):
    if not values:
        return "n/a"
    return "{:.2f} / {:.2f} / {:.2f}".format(geometric_mean(values), median(values), max(values))


def main():
    parser = argparse.ArgumentParser()
    here = os.path.dirname(os.path.abspath(__file__))
    default_root = os.path.normpath(os.path.join(here, "..", "modules", "ROOT", "data"))
    parser.add_argument("--data-root", default=default_root)
    parser.add_argument("--latex", action="store_true", help="emit LaTeX table rows")
    args = parser.parse_args()

    found = {}
    for platform_dir in glob.glob(os.path.join(args.data_root, "benchmarks-*")):
        key = os.path.basename(platform_dir).replace("benchmarks-", "")
        found[key] = summarize(load_medians(platform_dir))

    columns = [("int8-64", "sn"), ("int8-64", "safe"), ("128", "sn"), ("float", "sn")]
    if not args.latex:
        header = "{:20s} {:12s} {:20s} {:20s} {:20s} {:20s}".format(
            "platform", "compiler", "sn int8-64", "safe int8-64", "sn 128", "sn float")
        print(header)

    for key in PLATFORM_ORDER:
        if key not in found:
            continue
        label, compiler = PLATFORM_LABELS.get(key, (key, ""))
        cells = [cell(found[key].get(col, [])) for col in columns]
        if args.latex:
            print("{} & {} & {} \\\\".format(label, compiler, " & ".join(cells)))
        else:
            print("{:20s} {:12s} {:20s} {:20s} {:20s} {:20s}".format(label, compiler, *cells))


if __name__ == "__main__":
    main()
