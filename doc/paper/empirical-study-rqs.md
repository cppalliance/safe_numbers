slidenumbers: true
autoscale: true
theme: work, 1

# Boost.SafeNumbers
## An Empirical Study of Performance & Correctness

Terms Used Throughout:
- CVE - Common Vulnerabilities and Exposures - An specific vulnerability with a unique ID
- CWE - Common Weakness Enumeration - A class of related CVEs. e.g. CWE-122 - Heap-based Buffer Overflow
- NVD - National Vulnerability Database - NIST repository of CVEs


^ Why the pivot: the library is complete and the evidence largely exists — a 140-CVE corpus organized by CWE, host benchmarks across seven OS/ISA targets, CUDA and SYCL device benchmarks, and Why3 proofs of the fundamental operations.
^ Both threads now evaluate engineered artifacts against measurable claims — a tighter D.Eng. story.
^ Fallout: HRP-503c is shelved rather than fixed; ENGR 699 description needs updating; CppCon 2026 and WG21 Búzios shift from recruitment windows to dissemination venues.

---

# RQ1 — What are we solving?

> To what extent do checked fixed-width and domain-bounded types prevent the integer-arithmetic weaknesses behind real, disclosed CVEs?

- CWE-190: Integer Overflow or Wraparound)
- CWE-191: Integer Underflow (Wrap or Wraparound)
- CWE-681: Incorrect Conversion Between Numeric Types
- CWE-369: Divide by Zero

As Negative Control:
— CWE-682: Incorrect Calculation

**How we answer:** NVD-sourced CVE corpus (140 examined), each fault reproduced as a self-contained test, five-way disposition, prevention rates with 95% Wilson CIs.

^ Results in hand: all four primary categories at 100% including the bounded arm (CWE-190 is 29/30 by default, 30/30 with bounded); negative control 6/10; pooled 126/130 = 96.9% (92.4–98.8%).
^ Dispositions: PREVENTED_RUNTIME / _COMPILETIME / _BOUNDED, NOT_PREVENTED, OUT_OF_SCOPE — nothing silently dropped.
^ The claim is scoped: the arithmetic mechanism in isolation, not whole-program immunity. Purposive sample, so rates are conditional on reducibility to a snippet.
^ McNemar p is ceilinged by design; the prevention rate and Wilson interval are the result.
^ Second finding: bounded domains catch in-range faults the operation-level checks provably cannot — the control rises 3 → 6 once domains are declared.

---

# RQ2 — What does deployment cost?

> What runtime overhead does always-on checking at declared width impose, relative to native arithmetic and to Boost.SafeNumerics?

Fixed-width semantics detect faults SafeNumerics' default promotion silently absorbs, so this is *cost-effectiveness*, not just cost.

**How we answer:** per-operation Google Benchmark grid, seven OS/ISA targets, native + SafeNumerics baselines, non-violating path by design

**Added after advisor feedback:**
- Comparators beyond SafeNumerics: Microsoft SafeInt and Intel safe-arithmetic, on both correctness (ten semantic probes, compile-time vs runtime vs silent outcome per library) and cost (same operands, same two kernel shapes)
- Ablation over response policies: throwing, saturating, strict, user handlers (wrapping, throwing), overflowing (deferred), checked; scalar vs streaming kernel shape to expose which responses vectorize

^ Host picture: roughly 1.0–1.7x per operation through 64 bits; division and modulo at parity almost everywhere.
^ SafeNumerics has no floating-point and no 128-bit types — the comparator's coverage gap is itself a finding.
^ Semantics gap: safe<uint8_t>(255) + 1 yields 256 under default native promotion — no detection. Stricter semantics at comparable cost.
^ Hardening before proposal text: (1) state which SafeNumerics policy configuration was benchmarked; (2) CI-runner single-run medians are fine for docs, not the dissertation — controlled machine, repetitions, dispersion; (3) keep the non-violating-path scope explicit; (4) related-work paragraph on UBSan, -ftrapv, and the __builtin_*_overflow intrinsics — diagnostics vs production types.
^ Harness lesson (2026-09-10): per-element DoNotOptimize in the grid's scalar shape spills to the stack; on Apple M4 the default throwing u32 landed in adjacent spill slots whose reload was merged into one ldp straddling two stores (store-forwarding stall, 4.8x), while a user throwing handler measured parity. Disabling the AArch64 load/store merger restores parity. The stream shape has no such artifact; report policy comparisons from it.
^ Fixed 2026-09-10: all 20 signed throwing operators (i8..i64 x add/sub/mul/div/mod) exceeded Clang's inline threshold (cost 320 vs 225) because the exception/message/source_location construction sat inline in the operator body; BOOST_SAFE_NUMBERS_THROW_EXCEPTION now calls a cold noinline outlined function. Stream i32 add went 10.3x -> 3.8x (SafeNumbers) and signed division 2.0x -> parity; SafeNumbers is now in SafeInt's class on signed too. A user handler that throws inline reproduces the old cost on signed types (10x); outline the throw in handlers.
^ 2026-09-10: vendored int128 removed; safe_numbers now depends on Boost.int128 (types uint128/int128). Local A/B on Apple M4 (native __int128): 128-bit stream rows unchanged for add/sub/mul, signed division 1.87 -> 1.55 ns builtin, unsigned division 1.00 -> 1.22 ns; the portable (no builtin __int128) path is where upstream's updates would show and needs a Windows/32-bit run.
^ Intel fairness check (2026-09-10): Intel is fast because match's fallback is a VALUE (select), not because it checks less. Same loop, u32 stream ns/elem: builtin 0.075; Intel value-fallback 0.100; Intel throwing-fallback 0.374 (slower than our default 0.271); Intel assume-only 0.096; our checked_add 0.081 (beats Intel's value form on add). Interval reasoning genuinely wins only on multiplication: input range check (two compares, vectorizable) is cheaper than the high-half product check (our checked_mul 0.321). Contracts differ: Intel enforces a declared input domain (precondition), we detect result overflow at full width (postcondition); Intel cannot express full-range u32 add at all. Design answer (2026-09-10): do NOT make bounded_uint widen like Intel's var (Intel's declared interval is an input/storage precondition; ours is an invariant enforced on every result, which is what catches P10 and CVE-2011-3062). Instead let bounded values emit a range assumption (__builtin_assume / [[assume]] / __assume) when converted to their basis type: experiment with assumed [1,31] operands made u32 throwing mul 0.304 -> 0.087 ns (parity), throwing add 0.282 -> 0.079, checked mul 0.321 -> 0.098; Clang's CVP proved the with.overflow intrinsics dead, deleted the throw path, and vectorized (width 4). Sound because bounded_uint guarantees the invariant by construction. IMPLEMENTED 2026-09-10 (BOOST_SAFE_NUMBERS_ASSUME + bounded_uint/bounded_int::assume_domain on every conversion out of the type). Ablation row 'throwing, bounded ingress' (operands validated into bounded_uint<1,bits-1>, widened, default operators): u32 mul 3.86x -> 0.55x, i32 mul 4.55x -> 0.43x, u128/i128 mul 1.7x/3.2x -> 0.6x vs the 4/8/16-byte built-in baseline (bounded storage is 1 byte, so a byte-ingress built-in control was added for fair ratios); unsigned sub keeps its check (3.27x) because [1,31]-[1,31] can underflow, which the domain does not exclude.
^ Final reruns (2026-09-10, all with Boost.int128 + outlined throw + bounded assumptions; stream shape, Apple M4 Max): ablation u32 throwing 3.67/3.51/3.87 add/sub/mul, i32 3.85/3.92/4.69, value responses 0.74x-1.32x, unsigned checked mul 4.0x-5.2x; bounded ingress vs byte-ingress control u32 1.34/5.81/1.09, i32 1.24/0.90/1.02, 128-bit 0.81x-1.07x. Library: SafeNumbers/SafeNumerics/SafeInt u32 2.6x-4.4x, i32 SafeNumbers 4.0/5.1/6.1 vs SafeInt 3.4/4.0/4.5 vs SafeNumerics 10.0/13.1/10.6; Intel value 0.95x-1.35x, throwing fallback 3.6x-5.1x, no check 0.95x-1.3x. Run-to-run variance on the ~0.1 ns vectorized baselines is up to 30%; the paper now says so. Docs: benchmarks.adoc #policy_ablation + #library_comparison (generated partials via test/benchmarks/render_ablation.py from doc/modules/ROOT/data/benchmarks-ablation-macos-arm64), comparisons.adoc #comparisons_probes (partial via run_probes.py --adoc).
^ Probe findings: SafeNumerics widens u8 255+1 to 256 and lets 60+50 escape [0,100]; Intel safe-arithmetic rejects overflow/underflow/zero-divisor/narrowing/mixed-signedness at compile time but returns INT_MIN for INT_MIN/-1; SafeInt detects all runtime faults and compares mixed signedness correctly but has no bounded type and rejects mixed SafeInt widths only by overload ambiguity.

---

# RQ3 — What is novel?

> Which error-response mechanisms let the same checked types execute in exception-free, massively parallel device code — and at what cost?

Memory-bound streaming vs compute-bound register chains, on CUDA and SYCL.

**How we answer:** identical kernels per safe type vs its basis type, two kernel shapes, throwing vs saturating vs deferred families, bit-for-bit result verification.

^ Streaming regime: checks are essentially free — they execute in the shadow of memory traffic. Exceptions: 8-bit types, f64 division (1.44x), 128-bit multiplication (~6.8x).
^ Compute-bound integer chains: 1.3–2.9x through 64 bits.
^ SYCL CPU device: throwing float chains reach ~235x from lost vectorization; the branch-free deferred family recovers the same chains to 1.1–2.9x while still detecting every error at the boundary. saturating_add hits native saturating vector instructions — parity even at 8 bits.
^ Feature matrix: no comparator — SafeNumerics, bounded::integer, type_safe, Ada, Rust std — has device support at all.
^ Design insight: decoupling detection from response is not a convenience; it is the enabling mechanism for contexts where exceptions and branches cannot go.
^ Optional hardening: one discrete-GPU SYCL run to close the "matches the CUDA column" conjecture.

---

# Contribution — detection decoupled from response

- **Policy-Based Violation Handler** - Throw, Saturate, Checked, Deferral Flags, Bounded Domains, User-Defined
- **Soundness** — fundamental operations proven in Why3
- **Relevance** — CVE corpus, 96.9% pooled prevention
- **Reach** — same types on CUDA/SYCL devices, unique in the comparator set

**Response architecture matches C++26 contracts (P2900R14):** program-wide replaceable handler; the spectrum maps onto `ignore` / `observe` / `enforce` / `quick_enforce`.

^ Novelty scoping: user-definable handling per se is prior art — SafeNumerics exception-policy templates, the BSLS_ASSERT lineage, Rust's panic hook. The defensible claim is architectural congruence with the standardized model, versus type-bound policies that fragment the type universe.
^ Contracts status: finalized in C++26 at London, March 2026 (114–12–3); still contested (Stroustrup publicly opposed) and compiler support experimental. Cite P2900R14; position as "the contracts error model in C++20, ahead of compiler support." Build the claim on the decoupling thesis, with contracts as external validation — not on adoption.
^ Measurable facet: one benchmark row showing happy-path cost invariance under handler replacement.
^ Thread bridge for the prelim: Thread B verifies concept-carried contracts statically, once for all instantiations; Thread A checks arithmetic contracts dynamically with contracts-model violation handling. Contract-carrying C++ across the static–dynamic spectrum — squarely in the gradual-verification lineage.
