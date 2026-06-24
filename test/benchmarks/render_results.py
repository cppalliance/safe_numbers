#!/usr/bin/env python3
# Copyright 2026 Matt Borland
# Distributed under the Boost Software License, Version 1.0.
# https://www.boost.org/LICENSE_1_0.txt

# Turn Google Benchmark JSON output into the AsciiDoc per-platform section used
# by doc/modules/ROOT/pages/benchmarks.adoc.
#
# Benchmark names are "{category}_{bits}_{op}_{role}" (see the benchmark_*.cpp
# main functions). With --benchmark_repetitions and --benchmark_format=json each
# benchmark produces a "..._median" aggregate; this script reads the median and
# reports per-element time (ns) and the ratio to the same-width built-in type.
#
# Usage:
#   render_results.py --title "Linux x64" --anchor linux_x64 \
#       --desc "Run on the GitHub Actions ubuntu-latest runner using GCC 14 ..." \
#       --unsigned unsigned.json --signed signed.json --float float.json
#
# Prints the AsciiDoc section to stdout.

import argparse
import json
import sys

OP_LABELS = {
    "add": "Addition",
    "sub": "Subtraction",
    "mul": "Multiplication",
    "div": "Division",
    "mod": "Modulo",
}

INT_OPS = ["add", "sub", "mul", "div", "mod"]
FLOAT_OPS = ["add", "sub", "mul", "div"]
INT_BITS = [8, 16, 32, 64, 128]
FLOAT_BITS = [32, 64]


def load_ns_per_op(path):
    # Map "{category}_{bits}_{op}_{role}" -> nanoseconds per element (from median).
    with open(path) as handle:
        doc = json.load(handle)

    medians = {}
    raw = {}
    for entry in doc.get("benchmarks", []):
        ips = entry.get("items_per_second")
        if not ips:
            continue
        ns = 1.0e9 / ips
        aggregate = entry.get("aggregate_name")
        name = entry.get("run_name", entry.get("name", ""))
        if aggregate == "median":
            medians[name] = ns
        elif aggregate is None:
            raw.setdefault(name, ns)

    return medians if medians else raw


def builtin_label(category, bits):
    if category == "float":
        return "float" if bits == 32 else "double"
    prefix = "uint" if category == "unsigned" else "int"
    return f"{prefix}128_t" if bits == 128 else f"std::{prefix}{bits}_t"


def sn_label(category, bits):
    letter = {"unsigned": "u", "signed": "i", "float": "f"}[category]
    return f"boost::sn::{letter}{bits}"


def safe_label(category, bits):
    prefix = "uint" if category == "unsigned" else "int"
    return f"safe<{prefix}{bits}_t>"


def fmt(value):
    return f"{value:.2f}"


def emit_int_table(out, data, category, bits):
    has_safe = bits != 128
    title_unit = builtin_label(category, bits)
    out.append(f".{bits}-bit {category} (ratio relative to `{title_unit}`)")
    if has_safe:
        out.append('[cols="1,>1,>1,>1,>1,>1",options="header"]')
    else:
        out.append('[cols="1,>1,>1,>1",options="header"]')
    out.append("|===")
    if has_safe:
        out.append(
            f"| Operation | `{builtin_label(category, bits)}` (ns/op) "
            f"| `{sn_label(category, bits)}` (ns/op) | Ratio "
            f"| `{safe_label(category, bits)}` (ns/op) | Ratio"
        )
    else:
        out.append(
            f"| Operation | `{builtin_label(category, bits)}` (ns/op) "
            f"| `{sn_label(category, bits)}` (ns/op) | Ratio"
        )

    for op in INT_OPS:
        base = data.get(f"{category}_{bits}_{op}_builtin")
        sn = data.get(f"{category}_{bits}_{op}_sn")
        if base is None or sn is None:
            continue
        row = f"| {OP_LABELS[op]} | {fmt(base)} | {fmt(sn)} | {fmt(sn / base)}"
        if has_safe:
            safe = data.get(f"{category}_{bits}_{op}_safe")
            if safe is None:
                row += " | n/a | n/a"
            else:
                row += f" | {fmt(safe)} | {fmt(safe / base)}"
        out.append(row)

    out.append("|===")
    out.append("")


def emit_float_table(out, data, bits):
    title_unit = builtin_label("float", bits)
    out.append(f".{bits}-bit floating point (ratio relative to `{title_unit}`)")
    out.append('[cols="1,>1,>1,>1",options="header"]')
    out.append("|===")
    out.append(
        f"| Operation | `{builtin_label('float', bits)}` (ns/op) "
        f"| `{sn_label('float', bits)}` (ns/op) | Ratio"
    )
    for op in FLOAT_OPS:
        base = data.get(f"float_{bits}_{op}_builtin")
        sn = data.get(f"float_{bits}_{op}_sn")
        if base is None or sn is None:
            continue
        out.append(f"| {OP_LABELS[op]} | {fmt(base)} | {fmt(sn)} | {fmt(sn / base)}")
    out.append("|===")
    out.append("")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--title", required=True, help="Section heading, e.g. 'Linux x64'")
    parser.add_argument("--anchor", required=True, help="Anchor id, e.g. 'linux_x64'")
    parser.add_argument("--desc", required=True, help="One-line 'Run on ...' description")
    parser.add_argument("--unsigned", help="Google Benchmark JSON for the unsigned benchmark")
    parser.add_argument("--signed", dest="signed_json", help="JSON for the signed benchmark")
    parser.add_argument("--float", dest="float_json", help="JSON for the float benchmark")
    args = parser.parse_args()

    out = []
    out.append(f"[#{args.anchor}_benchmarks]")
    out.append(f"== {args.title}")
    out.append("")
    out.append(args.desc)
    out.append("")

    if args.float_json:
        data = load_ns_per_op(args.float_json)
        out.append("=== Floating-Point")
        out.append("")
        for bits in FLOAT_BITS:
            emit_float_table(out, data, bits)

    if args.signed_json:
        data = load_ns_per_op(args.signed_json)
        out.append("=== Signed Integers")
        out.append("")
        for bits in INT_BITS:
            emit_int_table(out, data, "signed", bits)

    if args.unsigned:
        data = load_ns_per_op(args.unsigned)
        out.append("=== Unsigned Integers")
        out.append("")
        for bits in INT_BITS:
            emit_int_table(out, data, "unsigned", bits)

    sys.stdout.write("\n".join(out).rstrip() + "\n")


if __name__ == "__main__":
    main()
