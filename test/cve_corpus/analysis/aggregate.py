#!/usr/bin/env python3
# Copyright 2026 Matt Borland
# Distributed under the Boost Software License, Version 1.0.
# https://www.boost.org/LICENSE_1_0.txt
#
# Aggregate the safe_numbers CVE corpus into per-CWE prevention statistics.
# Pure standard library only (no third party dependencies).
#
# It walks the corpus tree, parses the machine readable metadata block at the top
# of each .cpp file, reads the out_of_scope.csv manifest, optionally cross checks a
# results file that says whether each test actually demonstrated its behavior, and
# emits: corpus.csv (auditable manifest), results_by_cwe.csv (the per-CWE table),
# and results.adoc (a rendered results section for the manual).
#
# Usage:
#   python3 aggregate.py [--corpus-root DIR] [--out-dir DIR] [--results PATH|none]
#
# The results file, when given, is a TSV of "stem<TAB>PASS|FAIL" where stem is the
# file basename without extension. A prevented entry whose test did not pass is
# flagged and is NOT counted as prevented. Exit status is nonzero on any schema or
# consistency error so a CI job can gate the corpus.

import argparse
import csv
import glob
import math
import os
import re
import sys

CATEGORIES = ["CWE-190", "CWE-191", "CWE-681", "CWE-369", "CWE-682"]
CATEGORY_TITLES = {
    "CWE-190": "Integer Overflow or Wraparound",
    "CWE-191": "Integer Underflow",
    "CWE-681": "Incorrect Conversion between Numeric Types",
    "CWE-369": "Divide By Zero",
    "CWE-682": "Incorrect Calculation (negative control)",
}
ALL_CLASSES = {
    "PREVENTED_RUNTIME", "PREVENTED_COMPILETIME", "PREVENTED_BOUNDED",
    "NOT_PREVENTED", "OUT_OF_SCOPE",
}
PREVENTED = {"PREVENTED_RUNTIME", "PREVENTED_COMPILETIME", "PREVENTED_BOUNDED"}
PREVENTED_DEFAULT = {"PREVENTED_RUNTIME", "PREVENTED_COMPILETIME"}
EXCEPTIONS = {"std::overflow_error", "std::underflow_error", "std::domain_error", "none"}
NONE_EXC_CLASSES = {"PREVENTED_COMPILETIME", "NOT_PREVENTED", "OUT_OF_SCOPE"}

BEGIN = "BOOST_SAFE_NUMBERS_CVE_BEGIN"
END = "BOOST_SAFE_NUMBERS_CVE_END"
REQUIRED = ["cve-id", "cwe", "classification", "expected-exception", "tier-form"]
Z95 = 1.959963984540054


def wilson(k, n, z=Z95):
    # Wilson score interval for a binomial proportion. Wald is unusable here because
    # it degenerates to zero width at k == n, exactly the near ceiling regime we expect.
    if n == 0:
        return (float("nan"), float("nan"))
    p = k / n
    z2 = z * z
    denom = 1.0 + z2 / n
    center = (p + z2 / (2.0 * n)) / denom
    margin = (z / denom) * math.sqrt((p * (1.0 - p) + z2 / (4.0 * n)) / n)
    return (max(0.0, center - margin), min(1.0, center + margin))


def mcnemar_two_sided(b, n10=0):
    # Native baseline detects 0 by construction, so n10 == 0 and every prevented case
    # is a discordant pair favoring safe_numbers. This is the exact McNemar test
    # reduced to a binomial tail. It is ceilinged by design: it certifies the
    # asymmetry is not chance, not the size of the effect.
    d = b + n10
    if d == 0:
        return 1.0
    m = min(b, n10)
    tail = sum(math.comb(d, k) for k in range(0, m + 1)) * (0.5 ** d)
    return min(1.0, 2.0 * tail)


def fail(msg):
    print("ERROR: " + msg, file=sys.stderr)
    sys.exit(2)


def parse_block(path):
    with open(path, "r") as f:
        text = f.read()
    if BEGIN not in text or END not in text:
        fail("missing metadata block in " + path)
    body = text.split(BEGIN, 1)[1].split(END, 1)[0]
    rec = {"cwe": [], "reference": [], "notes": []}
    for raw in body.splitlines():
        line = raw.strip()
        if line.startswith("//"):
            line = line[2:].strip()
        if not line or ":" not in line:
            continue
        key, val = line.split(":", 1)
        key = key.strip()
        val = val.strip()
        if key == "cwe":
            rec["cwe"] = [c.strip() for c in val.split(",") if c.strip()]
        elif key in ("reference", "notes"):
            rec[key].append(val)
        else:
            rec[key] = val
    return rec


def category_from_name(stem):
    m = re.search(r"cwe(\d+)", stem)
    return ("CWE-" + m.group(1)) if m else None


def validate(rec, stem, path):
    for k in REQUIRED:
        if k not in rec or (isinstance(rec[k], str) and not rec[k]):
            fail("%s: missing required key '%s'" % (path, k))
    cls = rec["classification"]
    if cls not in ALL_CLASSES:
        fail("%s: bad classification '%s'" % (path, cls))
    exc = rec["expected-exception"]
    if exc not in EXCEPTIONS:
        fail("%s: bad expected-exception '%s'" % (path, exc))
    if cls in NONE_EXC_CLASSES and exc != "none":
        fail("%s: classification %s must have expected-exception none" % (path, cls))
    if cls not in NONE_EXC_CLASSES and exc == "none":
        fail("%s: classification %s must name an exception" % (path, cls))
    cat = category_from_name(stem)
    if cat is None:
        fail("%s: filename does not encode a cweNNN category" % path)
    if cat not in rec["cwe"]:
        fail("%s: filename category %s is not in the cwe tags %s" % (path, cat, rec["cwe"]))
    tier = rec["tier-form"]
    if cls == "PREVENTED_COMPILETIME" and tier != "compile-fail":
        fail("%s: PREVENTED_COMPILETIME must be tier-form compile-fail" % path)
    if cls in ("PREVENTED_RUNTIME", "PREVENTED_BOUNDED", "NOT_PREVENTED") and tier != "run":
        fail("%s: %s must be tier-form run" % (path, cls))
    return cat


def load_results(path):
    if not path or path == "none":
        return None
    status = {}
    with open(path, "r") as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            parts = line.split("\t") if "\t" in line else line.split()
            if len(parts) >= 2:
                status[parts[0]] = parts[1].upper()
    return status


def fmt_pct(x):
    return "%.1f%%" % (100.0 * x) if x == x else "n/a"


def main():
    ap = argparse.ArgumentParser()
    here = os.path.dirname(os.path.abspath(__file__))
    ap.add_argument("--corpus-root", default=os.path.dirname(here))
    ap.add_argument("--out-dir", default=here)
    ap.add_argument("--results", default="none")
    ap.add_argument("--partial-out", default=os.path.normpath(os.path.join(
        os.path.dirname(here), "..", "..", "doc", "modules", "ROOT", "partials", "cve_results.adoc")))
    args = ap.parse_args()

    results = load_results(args.results)

    rows = []
    for path in sorted(glob.glob(os.path.join(args.corpus_root, "**", "*.cpp"), recursive=True)):
        stem = os.path.splitext(os.path.basename(path))[0]
        rec = parse_block(path)
        cat = validate(rec, stem, path)
        cls = rec["classification"]
        demonstrated = "unverified"
        counts_prevented = cls in PREVENTED
        if results is not None:
            st = results.get(stem)
            if st is None:
                demonstrated = "missing"
            else:
                demonstrated = st
                if st != "PASS" and counts_prevented:
                    print("WARNING: %s is %s but its test did not PASS; not counted as prevented"
                          % (stem, cls), file=sys.stderr)
                    counts_prevented = False
        rows.append({
            "cve": rec.get("cve-id", ""),
            "category": cat,
            "cwe_tags": ";".join(rec["cwe"]),
            "product": rec.get("product", ""),
            "classification": cls,
            "counts_prevented": counts_prevented,
            "operation": rec.get("root-cause", ""),
            "width": rec.get("root-cause-width", ""),
            "expected_exception": rec["expected-exception"],
            "tier_form": rec["tier-form"],
            "nvd_url": (rec["reference"][0] if rec["reference"] else ""),
            "demonstrated": demonstrated,
            "stem": stem,
        })

    # Out of scope manifest.
    oos_rows = []
    oos_path = os.path.join(here, "out_of_scope.csv")
    if os.path.exists(oos_path):
        with open(oos_path, "r") as f:
            for r in csv.DictReader(f):
                oos_rows.append(r)

    if not rows:
        fail("no corpus files found under " + args.corpus_root)

    # Per category aggregation.
    per = {}
    for cat in CATEGORIES:
        crows = [r for r in rows if r["category"] == cat]
        n_prevented = sum(1 for r in crows if r["counts_prevented"])
        n_prevented_default = sum(
            1 for r in crows if r["counts_prevented"] and r["classification"] in PREVENTED_DEFAULT)
        n_not_prev = sum(1 for r in crows if r["classification"] == "NOT_PREVENTED")
        n_oos = sum(1 for r in oos_rows if r["category"] == cat) + \
            sum(1 for r in crows if r["classification"] == "OUT_OF_SCOPE")
        n_in_scope = n_prevented + n_not_prev
        n_total = n_in_scope + n_oos
        rate = (n_prevented / n_in_scope) if n_in_scope else float("nan")
        drate = (n_prevented_default / n_in_scope) if n_in_scope else float("nan")
        lo, hi = wilson(n_prevented, n_in_scope)
        per[cat] = {
            "n_total": n_total, "n_oos": n_oos, "n_in_scope": n_in_scope,
            "n_prevented": n_prevented, "n_prevented_default": n_prevented_default,
            "n_not_prevented": n_not_prev, "rate": rate, "default_rate": drate,
            "wilson_lo": lo, "wilson_hi": hi, "mcnemar_p": mcnemar_two_sided(n_prevented),
        }

    # Pooled and category averaged.
    pooled_prev = sum(per[c]["n_prevented"] for c in CATEGORIES)
    pooled_prev_default = sum(per[c]["n_prevented_default"] for c in CATEGORIES)
    pooled_in = sum(per[c]["n_in_scope"] for c in CATEGORIES)
    pooled_rate = (pooled_prev / pooled_in) if pooled_in else float("nan")
    pooled_lo, pooled_hi = wilson(pooled_prev, pooled_in)
    valid_cats = [c for c in CATEGORIES if per[c]["n_in_scope"] > 0]
    cat_avg = (sum(per[c]["rate"] for c in valid_cats) / len(valid_cats)) if valid_cats else float("nan")

    # Emit corpus.csv.
    corpus_csv = os.path.join(args.out_dir, "corpus.csv")
    with open(corpus_csv, "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["cve", "category", "cwe_tags", "product", "classification",
                    "counts_prevented", "operation", "width", "expected_exception",
                    "tier_form", "demonstrated", "nvd_url", "stem"])
        for r in sorted(rows, key=lambda x: (x["category"], x["cve"])):
            w.writerow([r["cve"], r["category"], r["cwe_tags"], r["product"],
                        r["classification"], r["counts_prevented"], r["operation"],
                        r["width"], r["expected_exception"], r["tier_form"],
                        r["demonstrated"], r["nvd_url"], r["stem"]])
        for r in oos_rows:
            w.writerow([r["cve"], r["category"], "", "", "OUT_OF_SCOPE", False,
                        "", "", "none", "excluded", "unverified", r.get("nvd_url", ""), ""])

    # Emit results_by_cwe.csv.
    by_cwe_csv = os.path.join(args.out_dir, "results_by_cwe.csv")
    with open(by_cwe_csv, "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["category", "title", "n_total", "n_out_of_scope", "n_in_scope",
                    "n_prevented", "n_prevented_default", "n_not_prevented",
                    "prevention_rate", "wilson_lo", "wilson_hi", "prevented_default_rate",
                    "mcnemar_two_sided_p"])
        for c in CATEGORIES:
            d = per[c]
            w.writerow([c, CATEGORY_TITLES[c], d["n_total"], d["n_oos"], d["n_in_scope"],
                        d["n_prevented"], d["n_prevented_default"], d["n_not_prevented"],
                        "%.4f" % d["rate"] if d["rate"] == d["rate"] else "nan",
                        "%.4f" % d["wilson_lo"] if d["wilson_lo"] == d["wilson_lo"] else "nan",
                        "%.4f" % d["wilson_hi"] if d["wilson_hi"] == d["wilson_hi"] else "nan",
                        "%.4f" % d["default_rate"] if d["default_rate"] == d["default_rate"] else "nan",
                        "%.3g" % d["mcnemar_p"]])
        w.writerow(["POOLED", "All categories", pooled_in + sum(per[c]["n_oos"] for c in CATEGORIES),
                    sum(per[c]["n_oos"] for c in CATEGORIES), pooled_in, pooled_prev, "", "",
                    "%.4f" % pooled_rate if pooled_rate == pooled_rate else "nan",
                    "%.4f" % pooled_lo, "%.4f" % pooled_hi, "",
                    "%.3g" % mcnemar_two_sided(pooled_prev)])

    # Emit results.adoc.
    adoc = os.path.join(args.out_dir, "results.adoc")
    with open(adoc, "w") as f:
        f.write("// Generated by aggregate.py. Do not edit by hand.\n")
        f.write("= CVE corpus results\n\n")
        f.write("This section is generated from the corpus metadata by ")
        f.write("`test/cve_corpus/analysis/aggregate.py`.\n\n")
        f.write("== Disposition funnel\n\n")
        total_drawn = sum(per[c]["n_total"] for c in CATEGORIES)
        total_oos = sum(per[c]["n_oos"] for c in CATEGORIES)
        f.write("[cols=\"1,1,1,1,1,1\",options=\"header\"]\n|===\n")
        f.write("|Category |Examined |Out of scope |In scope |Prevented |Not prevented\n")
        for c in CATEGORIES:
            d = per[c]
            f.write("|%s |%d |%d |%d |%d |%d\n" % (
                c, d["n_total"], d["n_oos"], d["n_in_scope"], d["n_prevented"], d["n_not_prevented"]))
        f.write("|All |%d |%d |%d |%d |%d\n" % (
            total_drawn, total_oos, pooled_in, pooled_prev, pooled_in - pooled_prev))
        f.write("|===\n\n")
        f.write("== Prevention rate by category\n\n")
        f.write("The \"by default\" column counts detection with the default checked types alone; ")
        f.write("the \"incl. bounded\" column adds cases prevented once a value's domain is expressed ")
        f.write("with a bounded type.\n\n")
        f.write("[cols=\"1,3,1,1,1,1,1\",options=\"header\"]\n|===\n")
        f.write("|CWE |Weakness |In scope |Prevented by default |Prevented incl. bounded "
                "|Prevention rate (95% Wilson CI) |McNemar p\n")
        for c in CATEGORIES:
            d = per[c]
            ci = "%s (%s to %s)" % (fmt_pct(d["rate"]), fmt_pct(d["wilson_lo"]), fmt_pct(d["wilson_hi"]))
            f.write("|%s |%s |%d |%d |%d |%s |%.3g\n" % (
                c, CATEGORY_TITLES[c], d["n_in_scope"], d["n_prevented_default"], d["n_prevented"],
                ci, d["mcnemar_p"]))
        pci = "%s (%s to %s)" % (fmt_pct(pooled_rate), fmt_pct(pooled_lo), fmt_pct(pooled_hi))
        f.write("|POOLED |All categories |%d |%d |%d |%s |%.3g\n" % (
            pooled_in, pooled_prev_default, pooled_prev, pci, mcnemar_two_sided(pooled_prev)))
        f.write("|===\n\n")
        f.write("Category averaged prevention rate (unweighted mean of the per category rates): %s.\n\n"
                % fmt_pct(cat_avg))
        f.write("== Reading these numbers\n\n")
        f.write("The native baseline detects zero of these faults by construction, since each CVE ")
        f.write("shipped in native integer code. Every prevented case is therefore a discordant pair ")
        f.write("favoring safe_numbers, so the McNemar p value is ceilinged by design: it certifies ")
        f.write("that the asymmetry is not chance, and it is not an effect size. The prevention rate ")
        f.write("and its Wilson interval are the primary result, bounded by the genuine failure mode ")
        f.write("visible in the not prevented cases.\n\n")
        f.write("In the CWE-682 negative control the gap between the two prevented columns is the ")
        f.write("contribution of bounded types: calculation errors whose wrong result is used as an ")
        f.write("index, offset, or length into a buffer of known size are caught when that domain is ")
        f.write("declared with a bounded type, even though the arithmetic itself does not overflow. ")
        f.write("The cases that remain not prevented are in range value errors (rounding, a wrong ")
        f.write("cryptographic result, a floating point mishandling) where no bound applies.\n")

    # Also emit the results as an Antora partial so the manual page stays in sync.
    wrote_partial = None
    doc_root = os.path.dirname(os.path.dirname(args.partial_out))  # .../ROOT
    if os.path.isdir(doc_root):
        os.makedirs(os.path.dirname(args.partial_out), exist_ok=True)
        with open(adoc, "r") as src, open(args.partial_out, "w") as dst:
            dst.write(src.read())
        wrote_partial = args.partial_out

    # Console summary.
    print("Parsed %d corpus files, %d out of scope manifest rows." % (len(rows), len(oos_rows)))
    for c in CATEGORIES:
        d = per[c]
        print("  %-8s in_scope=%2d prevented=%2d rate=%s CI=[%s,%s] p=%.3g" % (
            c, d["n_in_scope"], d["n_prevented"], fmt_pct(d["rate"]),
            fmt_pct(d["wilson_lo"]), fmt_pct(d["wilson_hi"]), d["mcnemar_p"]))
    print("  POOLED   in_scope=%2d prevented=%2d rate=%s CI=[%s,%s] p=%.3g" % (
        pooled_in, pooled_prev, fmt_pct(pooled_rate), fmt_pct(pooled_lo), fmt_pct(pooled_hi),
        mcnemar_two_sided(pooled_prev)))
    print("Wrote:\n  %s\n  %s\n  %s" % (corpus_csv, by_cwe_csv, adoc))
    if wrote_partial:
        print("  %s" % wrote_partial)


if __name__ == "__main__":
    main()
