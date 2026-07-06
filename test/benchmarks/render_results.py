#!/usr/bin/env python3
# Copyright 2026 Matt Borland
# Distributed under the Boost Software License, Version 1.0.
# https://www.boost.org/LICENSE_1_0.txt

# Turn Google Benchmark JSON output into the AsciiDoc per-platform sections used
# by doc/modules/ROOT/pages/benchmarks.adoc.
#
# Benchmark names are "{category}_{bits}_{op}_{role}" (see the benchmark_*.cpp
# main functions). With --benchmark_repetitions and --benchmark_format=json each
# benchmark produces a "..._median" aggregate; this script reads the median and
# reports per-element time (ns) and the ratio to the same-width built-in type.
#
# Two modes:
#
#   # Update the whole page in place from the committed CI artifacts. With no
#   # arguments it uses the default data directory and page (resolved relative
#   # to this script), iterating every benchmarks-<platform> folder it knows
#   # about.
#   render_results.py
#   render_results.py --data <dir> --page <benchmarks.adoc>
#
#   # Print a single platform's section to stdout (used by the CI workflow).
#   render_results.py --title "Linux x64" --anchor linux_x64 --desc "..." \
#       --unsigned unsigned.json --signed signed.json --float float.json

import argparse
import json
import os
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

# Marks the start of the generated region in the AsciiDoc page. Everything up to
# and including this line is preserved; everything after it is regenerated.
SENTINEL = "// BENCHMARK-RESULTS-GENERATED"

# One entry per CI artifact folder, in the order the sections should appear on
# the page. The "folder" matches the artifact directory under the data dir.
PLATFORMS = [
    {
        "folder": "benchmarks-linux-x64",
        "title": "Linux x64",
        "anchor": "linux_x64",
        "desc": "Run on the GitHub Actions `ubuntu-latest` runner using GCC 14 in release mode (`-O2`, pass:[C++]20).",
    },
    {
        "folder": "benchmarks-linux-x86-32",
        "title": "Linux x86 (32-bit)",
        "anchor": "linux_x86_32",
        "desc": "Run on the GitHub Actions `ubuntu-latest` runner using GCC 14 targeting 32-bit x86 (`-m32`) in release mode (`-O2`, pass:[C++]20).",
    },
    {
        "folder": "benchmarks-linux-arm64",
        "title": "Linux ARM64",
        "anchor": "linux_arm64",
        "desc": "Run on the GitHub Actions `ubuntu-24.04-arm` runner using GCC 14 in release mode (`-O2`, pass:[C++]20).",
    },
    {
        "folder": "benchmarks-macos-arm64",
        "title": "macOS ARM64",
        "anchor": "macos_arm64",
        "desc": "Run on the GitHub Actions `macos-latest` runner (Apple Silicon) using Apple Clang in release mode (`-O2`, pass:[C++]20).",
    },
    {
        "folder": "benchmarks-windows-x64",
        "title": "Windows x64",
        "anchor": "windows_x64",
        "desc": "Run on the GitHub Actions `windows-latest` runner using MSVC in release mode (`/O2`, pass:[C++]20).",
    },
    {
        "folder": "benchmarks-windows-x86-32",
        "title": "Windows x86 (32-bit)",
        "anchor": "windows_x86_32",
        "desc": "Run on the GitHub Actions `windows-latest` runner using MSVC targeting 32-bit x86 in release mode (`/O2`, pass:[C++]20).",
    },
    {
        "folder": "benchmarks-windows-arm64",
        "title": "Windows ARM64",
        "anchor": "windows_arm64",
        "desc": "Run on the GitHub Actions `windows-11-arm` runner using MSVC in release mode (`/O2`, pass:[C++]20).",
    },
]


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


def render_section(title, anchor, desc, unsigned_json, signed_json, float_json):
    out = []
    out.append(f"[#{anchor}_benchmarks]")
    out.append(f"== {title}")
    out.append("")
    out.append(desc)
    out.append("")

    if float_json:
        data = load_ns_per_op(float_json)
        out.append(f"[#{anchor}_floating_point]")
        out.append("=== Floating-Point")
        out.append("")
        for bits in FLOAT_BITS:
            emit_float_table(out, data, bits)

    if signed_json:
        data = load_ns_per_op(signed_json)
        out.append(f"[#{anchor}_signed_integers]")
        out.append("=== Signed Integers")
        out.append("")
        for bits in INT_BITS:
            emit_int_table(out, data, "signed", bits)

    if unsigned_json:
        data = load_ns_per_op(unsigned_json)
        out.append(f"[#{anchor}_unsigned_integers]")
        out.append("=== Unsigned Integers")
        out.append("")
        for bits in INT_BITS:
            emit_int_table(out, data, "unsigned", bits)

    return "\n".join(out).rstrip()


def default_paths():
    here = os.path.dirname(os.path.abspath(__file__))
    lib_root = os.path.abspath(os.path.join(here, "..", ".."))
    data = os.path.join(lib_root, "doc", "modules", "ROOT", "data")
    page = os.path.join(lib_root, "doc", "modules", "ROOT", "pages", "benchmarks.adoc")
    return data, page


def update_page(data_dir, page_path):
    sections = []
    used = []
    for platform in PLATFORMS:
        folder = os.path.join(data_dir, platform["folder"])
        if not os.path.isdir(folder):
            continue

        def maybe(name):
            path = os.path.join(folder, name)
            return path if os.path.isfile(path) else None

        unsigned_json = maybe("unsigned.json")
        signed_json = maybe("signed.json")
        float_json = maybe("float.json")
        if not (unsigned_json or signed_json or float_json):
            continue

        sections.append(
            render_section(
                platform["title"],
                platform["anchor"],
                platform["desc"],
                unsigned_json,
                signed_json,
                float_json,
            )
        )
        used.append(platform["folder"])

    if not sections:
        sys.exit(f"error: no benchmark data found under {data_dir}")

    with open(page_path) as handle:
        lines = handle.read().splitlines()

    try:
        cut = next(i for i, line in enumerate(lines) if line.strip() == SENTINEL)
    except StopIteration:
        sys.exit(f"error: sentinel '{SENTINEL}' not found in {page_path}")

    head = lines[: cut + 1]
    body = "\n\n".join(sections)
    open(page_path, "w").write("\n".join(head) + "\n\n" + body + "\n")

    print(f"updated {page_path} with {len(used)} platform(s):")
    for folder in used:
        print(f"  - {folder}")


def main():
    parser = argparse.ArgumentParser()
    default_data, default_page = default_paths()
    parser.add_argument("--data", default=default_data, help="Directory of benchmarks-<platform> folders")
    parser.add_argument("--page", default=default_page, help="benchmarks.adoc to update in place")
    parser.add_argument("--title", help="Single-section mode: section heading, e.g. 'Linux x64'")
    parser.add_argument("--anchor", help="Single-section mode: anchor id, e.g. 'linux_x64'")
    parser.add_argument("--desc", help="Single-section mode: one-line 'Run on ...' description")
    parser.add_argument("--unsigned", help="Single-section mode: JSON for the unsigned benchmark")
    parser.add_argument("--signed", dest="signed_json", help="Single-section mode: JSON for the signed benchmark")
    parser.add_argument("--float", dest="float_json", help="Single-section mode: JSON for the float benchmark")
    args = parser.parse_args()

    if args.title or args.anchor or args.desc:
        if not (args.title and args.anchor and args.desc):
            parser.error("single-section mode requires --title, --anchor, and --desc")
        section = render_section(
            args.title, args.anchor, args.desc,
            args.unsigned, args.signed_json, args.float_json,
        )
        sys.stdout.write(section + "\n")
        return

    update_page(args.data, args.page)


if __name__ == "__main__":
    main()
