// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
//
// Measures the device-side overhead of Boost.safe_numbers on SYCL by running
// identical kernels over the safe types and over their raw basis types.
// The structure mirrors benchmark_cuda_operations.cu; see that file and
// device_bench.hpp for the methodology.
//
// Timing sums per-kernel device execution time from SYCL event profiling, so
// submission gaps are excluded. Every timed region is verified afterwards and
// a device_error_context guards the whole run, so if any check ever fired the
// benchmark aborts instead of silently timing the error path.
//
// Build with icpx: -fsycl (compile and link) -DBOOST_SAFE_NUMBERS_ENABLE_SYCL=1
//
// Usage: benchmark_sycl_operations [stream_elements] [chain_cycles] [launches] [chain_threads]

#include <sycl/sycl.hpp>

#ifndef BOOST_SAFE_NUMBERS_ENABLE_SYCL
#  error "Compile with -DBOOST_SAFE_NUMBERS_ENABLE_SYCL=1 to build the SYCL benchmark"
#endif

#include <boost/safe_numbers/device_error_reporting.hpp>
#include "device_bench.hpp"

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

struct bench_config
{
    int stream_n {1 << 22};
    int chain_cycles {256};
    int launches {10};
    int chain_threads {1 << 16};
};

template <typename T>
class usm_array
{
public:
    usm_array(sycl::queue& q, const std::size_t n) : q_ {&q}, n_ {n}, data_ {sycl::malloc_device<T>(n, q)}
    {
        if (data_ == nullptr)
        {
            throw std::runtime_error("sycl::malloc_device failed");
        }
    }

    ~usm_array()
    {
        sycl::free(data_, *q_);
    }

    usm_array(const usm_array&) = delete;
    usm_array& operator=(const usm_array&) = delete;

    void upload(const std::vector<T>& src)
    {
        q_->memcpy(data_, src.data(), src.size() * sizeof(T)).wait();
    }

    std::vector<T> download() const
    {
        std::vector<T> host(n_, device_bench::make_val<T>(0U));
        q_->memcpy(host.data(), data_, n_ * sizeof(T)).wait();
        return host;
    }

    T* get() const
    {
        return data_;
    }

private:
    sycl::queue* q_ {};
    std::size_t n_ {};
    T* data_ {};
};

// Returns the average device execution time of one launch in nanoseconds,
// summed from event profiling so host-side submission gaps are excluded
template <typename F>
double time_launches_ns(sycl::queue& q, F launch, const int launches)
{
    launch().wait();
    launch().wait();

    std::vector<sycl::event> events;
    events.reserve(static_cast<std::size_t>(launches));
    for (int i {0}; i < launches; ++i)
    {
        events.push_back(launch());
    }
    q.wait();

    auto total_ns {std::uint64_t {0}};
    for (auto& ev : events)
    {
        const auto begin {ev.get_profiling_info<sycl::info::event_profiling::command_start>()};
        const auto end {ev.get_profiling_info<sycl::info::event_profiling::command_end>()};
        total_ns += end - begin;
    }

    return static_cast<double>(total_ns) / launches;
}

template <typename Op, typename T>
sycl::event launch_stream(sycl::queue& q, const T* a, const T* b, T* out, const int n)
{
    return q.parallel_for(sycl::range<1>(static_cast<std::size_t>(n)), [=](sycl::id<1> idx)
    {
        const auto i {idx[0]};
        out[i] = Op::apply(a[i], b[i]);
    });
}

template <typename Chain, typename T>
sycl::event launch_chain(sycl::queue& q, const T* x0, const T* ys, const T* aux, T* out, const int cycles, const int threads)
{
    return q.parallel_for(sycl::range<1>(static_cast<std::size_t>(threads)), [=](sycl::id<1> idx)
    {
        const auto t {idx[0]};
        const auto base {t * device_bench::chain_operands};
        const T y[device_bench::chain_operands] {ys[base], ys[base + 1], ys[base + 2], ys[base + 3],
                                                 ys[base + 4], ys[base + 5], ys[base + 6], ys[base + 7]};
        const T a {aux[t]};
        T x {x0[t]};

        for (int r {0}; r < cycles; ++r)
        {
            x = Chain::cycle(x, y, a);
        }

        out[t] = x;
    });
}

template <typename Op, typename TB, typename TS>
int run_stream_op(sycl::queue& q, boost::safe_numbers::device_error_context& ctx,
                  const device_bench::stream_inputs<TB, TS>& host_in,
                  const usm_array<TB>& a_b, const usm_array<TB>& b_b, usm_array<TB>& out_b,
                  const usm_array<TS>& a_s, const usm_array<TS>& b_s, usm_array<TS>& out_s,
                  const bench_config& cfg)
{
    const auto n {cfg.stream_n};

    const auto builtin_ns {time_launches_ns(q, [&]()
    {
        return launch_stream<Op>(q, a_b.get(), b_b.get(), out_b.get(), n);
    }, cfg.launches)};
    ctx.synchronize();

    const auto safe_ns {time_launches_ns(q, [&]()
    {
        return launch_stream<Op>(q, a_s.get(), b_s.get(), out_s.get(), n);
    }, cfg.launches)};
    ctx.synchronize();

    const auto host_b {out_b.download()};
    const auto host_s {out_s.download()};

    auto bad {device_bench::count_mismatches(host_b, host_s)};

    const auto spot {std::min(n, 1024)};
    for (int i {0}; i < spot; ++i)
    {
        if (host_b[static_cast<std::size_t>(i)] != Op::apply(host_in.a.builtin_vals[static_cast<std::size_t>(i)],
                                                             host_in.b.builtin_vals[static_cast<std::size_t>(i)]))
        {
            ++bad;
        }
    }

    return device_bench::print_row(Op::name, builtin_ns / n, safe_ns / n, bad);
}

template <template <typename> class Chain, typename TB, typename TS>
int run_chain(sycl::queue& q, boost::safe_numbers::device_error_context& ctx,
              const device_bench::chain_inputs<TB, TS>& host_in,
              const bench_config& cfg)
{
    const auto threads {cfg.chain_threads};

    usm_array<TB> x0_b {q, static_cast<std::size_t>(threads)};
    usm_array<TB> ys_b {q, static_cast<std::size_t>(threads) * device_bench::chain_operands};
    usm_array<TB> aux_b {q, static_cast<std::size_t>(threads)};
    usm_array<TB> out_b {q, static_cast<std::size_t>(threads)};
    x0_b.upload(host_in.x0.builtin_vals);
    ys_b.upload(host_in.ys.builtin_vals);
    aux_b.upload(host_in.aux.builtin_vals);

    usm_array<TS> x0_s {q, static_cast<std::size_t>(threads)};
    usm_array<TS> ys_s {q, static_cast<std::size_t>(threads) * device_bench::chain_operands};
    usm_array<TS> aux_s {q, static_cast<std::size_t>(threads)};
    usm_array<TS> out_s {q, static_cast<std::size_t>(threads)};
    x0_s.upload(host_in.x0.safe_vals);
    ys_s.upload(host_in.ys.safe_vals);
    aux_s.upload(host_in.aux.safe_vals);

    const auto builtin_ns {time_launches_ns(q, [&]()
    {
        return launch_chain<Chain<TB>>(q, x0_b.get(), ys_b.get(), aux_b.get(), out_b.get(), cfg.chain_cycles, threads);
    }, cfg.launches)};
    ctx.synchronize();

    const auto safe_ns {time_launches_ns(q, [&]()
    {
        return launch_chain<Chain<TS>>(q, x0_s.get(), ys_s.get(), aux_s.get(), out_s.get(), cfg.chain_cycles, threads);
    }, cfg.launches)};
    ctx.synchronize();

    const auto host_b {out_b.download()};
    const auto host_s {out_s.download()};

    auto bad {device_bench::count_mismatches(host_b, host_s)};

    // Recompute a slice of the chains on the host to catch shared miscompiles
    const auto spot_threads {std::min(threads, 128)};
    for (int t {0}; t < spot_threads; ++t)
    {
        const auto base {static_cast<std::size_t>(t) * device_bench::chain_operands};
        TB y[device_bench::chain_operands] {};
        for (int k {0}; k < device_bench::chain_operands; ++k)
        {
            y[k] = host_in.ys.builtin_vals[base + static_cast<std::size_t>(k)];
        }

        auto x {host_in.x0.builtin_vals[static_cast<std::size_t>(t)]};
        const auto a {host_in.aux.builtin_vals[static_cast<std::size_t>(t)]};
        for (int r {0}; r < cfg.chain_cycles; ++r)
        {
            x = Chain<TB>::cycle(x, y, a);
        }

        if (host_b[static_cast<std::size_t>(t)] != x)
        {
            ++bad;
        }
    }

    const auto ops {static_cast<double>(threads) * cfg.chain_cycles * Chain<TB>::ops_per_cycle};
    return device_bench::print_row(Chain<TB>::name, builtin_ns / ops, safe_ns / ops, bad);
}

template <typename TS>
int run_int_family(const char* name, sycl::queue& q, boost::safe_numbers::device_error_context& ctx, const bench_config& cfg)
{
    using TB = typename TS::basis_type;

    auto failures {0};

    {
        const auto in {device_bench::gen_stream_int<TB, TS>(cfg.stream_n)};

        usm_array<TB> a_b {q, static_cast<std::size_t>(cfg.stream_n)};
        usm_array<TB> b_b {q, static_cast<std::size_t>(cfg.stream_n)};
        usm_array<TB> out_b {q, static_cast<std::size_t>(cfg.stream_n)};
        a_b.upload(in.a.builtin_vals);
        b_b.upload(in.b.builtin_vals);

        usm_array<TS> a_s {q, static_cast<std::size_t>(cfg.stream_n)};
        usm_array<TS> b_s {q, static_cast<std::size_t>(cfg.stream_n)};
        usm_array<TS> out_s {q, static_cast<std::size_t>(cfg.stream_n)};
        a_s.upload(in.a.safe_vals);
        b_s.upload(in.b.safe_vals);

        device_bench::print_table_header(name, "streaming, one checked op per element (memory-bound)");
        failures += run_stream_op<device_bench::op_add>(q, ctx, in, a_b, b_b, out_b, a_s, b_s, out_s, cfg);
        failures += run_stream_op<device_bench::op_sub>(q, ctx, in, a_b, b_b, out_b, a_s, b_s, out_s, cfg);
        failures += run_stream_op<device_bench::op_mul>(q, ctx, in, a_b, b_b, out_b, a_s, b_s, out_s, cfg);
        failures += run_stream_op<device_bench::op_div>(q, ctx, in, a_b, b_b, out_b, a_s, b_s, out_s, cfg);
        failures += run_stream_op<device_bench::op_mod>(q, ctx, in, a_b, b_b, out_b, a_s, b_s, out_s, cfg);
        failures += run_stream_op<device_bench::op_sat_add>(q, ctx, in, a_b, b_b, out_b, a_s, b_s, out_s, cfg);
    }

    {
        const auto in {device_bench::gen_int_chain<TB, TS>(cfg.chain_threads)};

        device_bench::print_table_header(name, "register chain of checked ops (compute-bound)");
        failures += run_chain<device_bench::int_mixed_chain>(q, ctx, in, cfg);
        failures += run_chain<device_bench::int_mixed_sat_chain>(q, ctx, in, cfg);
        failures += run_chain<device_bench::int_mixed_ovf_chain>(q, ctx, in, cfg);
    }

    return failures;
}

template <typename TS>
int run_real_family(const char* name, sycl::queue& q, boost::safe_numbers::device_error_context& ctx, const bench_config& cfg)
{
    using TB = typename TS::basis_type;

    auto failures {0};

    {
        const auto in {device_bench::gen_stream_real<TB, TS>(cfg.stream_n)};

        usm_array<TB> a_b {q, static_cast<std::size_t>(cfg.stream_n)};
        usm_array<TB> b_b {q, static_cast<std::size_t>(cfg.stream_n)};
        usm_array<TB> out_b {q, static_cast<std::size_t>(cfg.stream_n)};
        a_b.upload(in.a.builtin_vals);
        b_b.upload(in.b.builtin_vals);

        usm_array<TS> a_s {q, static_cast<std::size_t>(cfg.stream_n)};
        usm_array<TS> b_s {q, static_cast<std::size_t>(cfg.stream_n)};
        usm_array<TS> out_s {q, static_cast<std::size_t>(cfg.stream_n)};
        a_s.upload(in.a.safe_vals);
        b_s.upload(in.b.safe_vals);

        device_bench::print_table_header(name, "streaming, one checked op per element (memory-bound)");
        failures += run_stream_op<device_bench::op_add>(q, ctx, in, a_b, b_b, out_b, a_s, b_s, out_s, cfg);
        failures += run_stream_op<device_bench::op_sub>(q, ctx, in, a_b, b_b, out_b, a_s, b_s, out_s, cfg);
        failures += run_stream_op<device_bench::op_mul>(q, ctx, in, a_b, b_b, out_b, a_s, b_s, out_s, cfg);
        failures += run_stream_op<device_bench::op_div>(q, ctx, in, a_b, b_b, out_b, a_s, b_s, out_s, cfg);
    }

    {
        device_bench::print_table_header(name, "register chain of checked ops (compute-bound)");

        const auto add_in {device_bench::gen_real_chain<TB, TS>(cfg.chain_threads, device_bench::real_chain_kind::sum)};
        failures += run_chain<device_bench::real_add_chain>(q, ctx, add_in, cfg);
        failures += run_chain<device_bench::real_sub_chain>(q, ctx, add_in, cfg);

        const auto mul_in {device_bench::gen_real_chain<TB, TS>(cfg.chain_threads, device_bench::real_chain_kind::product)};
        failures += run_chain<device_bench::real_mul_chain>(q, ctx, mul_in, cfg);

        const auto div_in {device_bench::gen_real_chain<TB, TS>(cfg.chain_threads, device_bench::real_chain_kind::divisor)};
        failures += run_chain<device_bench::real_div_chain>(q, ctx, div_in, cfg);

        failures += run_chain<device_bench::real_ovf_add_chain>(q, ctx, add_in, cfg);
        failures += run_chain<device_bench::real_ovf_sub_chain>(q, ctx, add_in, cfg);
        failures += run_chain<device_bench::real_ovf_mul_chain>(q, ctx, mul_in, cfg);
        failures += run_chain<device_bench::real_ovf_div_chain>(q, ctx, div_in, cfg);
    }

    return failures;
}

} // namespace

int main(int argc, char** argv)
{
    bench_config cfg {};
    if (argc > 1)
    {
        cfg.stream_n = std::atoi(argv[1]);
    }
    if (argc > 2)
    {
        cfg.chain_cycles = std::atoi(argv[2]);
    }
    if (argc > 3)
    {
        cfg.launches = std::atoi(argv[3]);
    }
    if (argc > 4)
    {
        cfg.chain_threads = std::atoi(argv[4]);
    }
    if (cfg.stream_n <= 0 || cfg.chain_cycles <= 0 || cfg.launches <= 0 || cfg.chain_threads <= 0)
    {
        std::fprintf(stderr, "usage: %s [stream_elements] [chain_cycles] [launches] [chain_threads]\n", argv[0]);
        return EXIT_FAILURE;
    }

    auto failures {0};

    try
    {
        sycl::queue q {sycl::property_list{sycl::property::queue::in_order{}, sycl::property::queue::enable_profiling{}}};

        const auto device_name {q.get_device().get_info<sycl::info::device::name>()};
        std::printf("Boost.safe_numbers SYCL benchmark\n");
        std::printf("Device: %s\n", device_name.c_str());
        std::printf("Streaming elements: %d, chain threads: %d, chain cycles: %d, launches per timing: %d\n",
                    cfg.stream_n, cfg.chain_threads, cfg.chain_cycles, cfg.launches);

        boost::safe_numbers::device_error_context ctx {q};

        failures += run_int_family<boost::safe_numbers::u8>("u8", q, ctx, cfg);
        failures += run_int_family<boost::safe_numbers::u16>("u16", q, ctx, cfg);
        failures += run_int_family<boost::safe_numbers::u32>("u32", q, ctx, cfg);
        failures += run_int_family<boost::safe_numbers::u64>("u64", q, ctx, cfg);
        failures += run_int_family<boost::safe_numbers::u128>("u128", q, ctx, cfg);

        failures += run_int_family<boost::safe_numbers::i8>("i8", q, ctx, cfg);
        failures += run_int_family<boost::safe_numbers::i16>("i16", q, ctx, cfg);
        failures += run_int_family<boost::safe_numbers::i32>("i32", q, ctx, cfg);
        failures += run_int_family<boost::safe_numbers::i64>("i64", q, ctx, cfg);
        failures += run_int_family<boost::safe_numbers::i128>("i128", q, ctx, cfg);

        failures += run_real_family<boost::safe_numbers::f32>("f32", q, ctx, cfg);
        failures += run_real_family<boost::safe_numbers::f64>("f64", q, ctx, cfg);
    }
    catch (const std::exception& e)
    {
        std::fprintf(stderr, "\nUnexpected error (a safety check fired inside a timed region): %s\n", e.what());
        return EXIT_FAILURE;
    }

    if (failures != 0)
    {
        std::printf("\n%d benchmark(s) FAILED verification\n", failures);
        return EXIT_FAILURE;
    }

    std::printf("\nAll benchmarks verified\n");
    return EXIT_SUCCESS;
}
