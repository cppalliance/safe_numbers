// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
//
// Measures the device-side overhead of Boost.safe_numbers on CUDA by running
// identical kernels over the safe types and over their raw basis types.
//
// Two regimes are measured for every type:
//   * streaming: out[i] = a[i] op b[i], one checked op per element. This is
//     the memory-bound shape that real element-wise kernels have, so it shows
//     the overhead a typical kernel actually experiences.
//   * chain: a serial dependency chain of checked ops held in registers, the
//     compute-bound worst case where nothing can hide behind memory traffic.
//
// Timing uses CUDA events around a batch of kernel launches. Every timed
// region is verified afterwards: the safe results must match the builtin
// results element for element, and a slice is recomputed on the host. A
// device_error_context guards the whole run, so if any check ever fired the
// benchmark aborts instead of silently timing the error path.
//
// Usage: benchmark_cuda_operations [stream_elements] [chain_cycles] [launches] [chain_threads]

#ifndef BOOST_SAFE_NUMBERS_ENABLE_CUDA
#  error "Compile with -DBOOST_SAFE_NUMBERS_ENABLE_CUDA=1 to build the CUDA benchmark"
#endif

#include <boost/safe_numbers/device_error_reporting.hpp>
#include "device_bench.hpp"

#include <cuda_runtime.h>

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <vector>

namespace {

constexpr int block_size {256};

struct bench_config
{
    int stream_n {1 << 22};
    int chain_cycles {256};
    int launches {10};
    int chain_threads {1 << 16};
};

void cuda_check(const cudaError_t err, const char* what)
{
    if (err != cudaSuccess)
    {
        std::fprintf(stderr, "%s failed: %s\n", what, cudaGetErrorString(err));
        std::exit(EXIT_FAILURE);
    }
}

template <typename T>
class device_array
{
public:
    explicit device_array(const std::size_t n) : n_ {n}
    {
        void* ptr {};
        cuda_check(cudaMalloc(&ptr, n * sizeof(T)), "cudaMalloc");
        data_ = static_cast<T*>(ptr);
    }

    ~device_array()
    {
        cudaFree(data_);
    }

    device_array(const device_array&) = delete;
    device_array& operator=(const device_array&) = delete;

    void upload(const std::vector<T>& src)
    {
        cuda_check(cudaMemcpy(data_, src.data(), src.size() * sizeof(T), cudaMemcpyHostToDevice), "cudaMemcpy to device");
    }

    std::vector<T> download() const
    {
        std::vector<T> host(n_, device_bench::make_val<T>(0U));
        cuda_check(cudaMemcpy(host.data(), data_, n_ * sizeof(T), cudaMemcpyDeviceToHost), "cudaMemcpy to host");
        return host;
    }

    T* get() const
    {
        return data_;
    }

private:
    T* data_ {};
    std::size_t n_ {};
};

template <typename Op, typename T>
__global__ void stream_kernel(const T* a, const T* b, T* out, const int n)
{
    const auto i {static_cast<int>(blockDim.x * blockIdx.x + threadIdx.x)};

    if (i < n)
    {
        out[i] = Op::apply(a[i], b[i]);
    }
}

template <typename Chain, typename T>
__global__ void chain_kernel(const T* x0, const T* ys, const T* aux, T* out, const int cycles, const int n_threads)
{
    const auto t {static_cast<int>(blockDim.x * blockIdx.x + threadIdx.x)};

    if (t < n_threads)
    {
        const auto base {static_cast<long long>(t) * device_bench::chain_operands};
        const T y[device_bench::chain_operands] {ys[base], ys[base + 1], ys[base + 2], ys[base + 3],
                                                 ys[base + 4], ys[base + 5], ys[base + 6], ys[base + 7]};
        const T a {aux[t]};
        T x {x0[t]};

        for (int r {0}; r < cycles; ++r)
        {
            x = Chain::cycle(x, y, a);
        }

        out[t] = x;
    }
}

// Returns the average time of one launch in nanoseconds
template <typename F>
double time_launches_ns(F launch, const int launches)
{
    launch();
    launch();
    cuda_check(cudaGetLastError(), "warmup launch");
    cuda_check(cudaDeviceSynchronize(), "warmup synchronize");

    cudaEvent_t start {};
    cudaEvent_t stop {};
    cuda_check(cudaEventCreate(&start), "cudaEventCreate");
    cuda_check(cudaEventCreate(&stop), "cudaEventCreate");

    cuda_check(cudaEventRecord(start), "cudaEventRecord");
    for (int i {0}; i < launches; ++i)
    {
        launch();
    }
    cuda_check(cudaEventRecord(stop), "cudaEventRecord");
    cuda_check(cudaGetLastError(), "timed launch");
    cuda_check(cudaEventSynchronize(stop), "cudaEventSynchronize");

    auto ms {0.0F};
    cuda_check(cudaEventElapsedTime(&ms, start, stop), "cudaEventElapsedTime");

    cudaEventDestroy(start);
    cudaEventDestroy(stop);

    return static_cast<double>(ms) * 1.0e6 / launches;
}

template <typename Op, typename TB, typename TS>
int run_stream_op(boost::safe_numbers::device_error_context& ctx,
                  const device_bench::stream_inputs<TB, TS>& host_in,
                  const device_array<TB>& a_b, const device_array<TB>& b_b, device_array<TB>& out_b,
                  const device_array<TS>& a_s, const device_array<TS>& b_s, device_array<TS>& out_s,
                  const bench_config& cfg)
{
    const auto n {cfg.stream_n};
    const auto blocks {(n + block_size - 1) / block_size};

    const auto builtin_ns {time_launches_ns([&]()
    {
        stream_kernel<Op><<<blocks, block_size>>>(a_b.get(), b_b.get(), out_b.get(), n);
    }, cfg.launches)};
    ctx.synchronize();

    const auto safe_ns {time_launches_ns([&]()
    {
        stream_kernel<Op><<<blocks, block_size>>>(a_s.get(), b_s.get(), out_s.get(), n);
    }, cfg.launches)};
    ctx.synchronize();

    const auto host_b {out_b.download()};
    const auto host_s {out_s.download()};

    auto bad {device_bench::count_mismatches(host_b, host_s)};

    const auto spot {std::min(n, 1024)};
    for (int i {0}; i < spot; ++i)
    {
        if (host_b[i] != Op::apply(host_in.a.builtin_vals[static_cast<std::size_t>(i)],
                                   host_in.b.builtin_vals[static_cast<std::size_t>(i)]))
        {
            ++bad;
        }
    }

    return device_bench::print_row(Op::name, builtin_ns / n, safe_ns / n, bad);
}

template <template <typename> class Chain, typename TB, typename TS>
int run_chain(boost::safe_numbers::device_error_context& ctx,
              const device_bench::chain_inputs<TB, TS>& host_in,
              const bench_config& cfg)
{
    const auto threads {cfg.chain_threads};
    const auto blocks {(threads + block_size - 1) / block_size};

    device_array<TB> x0_b {static_cast<std::size_t>(threads)};
    device_array<TB> ys_b {static_cast<std::size_t>(threads) * device_bench::chain_operands};
    device_array<TB> aux_b {static_cast<std::size_t>(threads)};
    device_array<TB> out_b {static_cast<std::size_t>(threads)};
    x0_b.upload(host_in.x0.builtin_vals);
    ys_b.upload(host_in.ys.builtin_vals);
    aux_b.upload(host_in.aux.builtin_vals);

    device_array<TS> x0_s {static_cast<std::size_t>(threads)};
    device_array<TS> ys_s {static_cast<std::size_t>(threads) * device_bench::chain_operands};
    device_array<TS> aux_s {static_cast<std::size_t>(threads)};
    device_array<TS> out_s {static_cast<std::size_t>(threads)};
    x0_s.upload(host_in.x0.safe_vals);
    ys_s.upload(host_in.ys.safe_vals);
    aux_s.upload(host_in.aux.safe_vals);

    const auto builtin_ns {time_launches_ns([&]()
    {
        chain_kernel<Chain<TB>><<<blocks, block_size>>>(x0_b.get(), ys_b.get(), aux_b.get(), out_b.get(),
                                                        cfg.chain_cycles, threads);
    }, cfg.launches)};
    ctx.synchronize();

    const auto safe_ns {time_launches_ns([&]()
    {
        chain_kernel<Chain<TS>><<<blocks, block_size>>>(x0_s.get(), ys_s.get(), aux_s.get(), out_s.get(),
                                                        cfg.chain_cycles, threads);
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
int run_int_family(const char* name, boost::safe_numbers::device_error_context& ctx, const bench_config& cfg)
{
    using TB = typename TS::basis_type;

    auto failures {0};

    {
        const auto in {device_bench::gen_stream_int<TB, TS>(cfg.stream_n)};

        device_array<TB> a_b {static_cast<std::size_t>(cfg.stream_n)};
        device_array<TB> b_b {static_cast<std::size_t>(cfg.stream_n)};
        device_array<TB> out_b {static_cast<std::size_t>(cfg.stream_n)};
        a_b.upload(in.a.builtin_vals);
        b_b.upload(in.b.builtin_vals);

        device_array<TS> a_s {static_cast<std::size_t>(cfg.stream_n)};
        device_array<TS> b_s {static_cast<std::size_t>(cfg.stream_n)};
        device_array<TS> out_s {static_cast<std::size_t>(cfg.stream_n)};
        a_s.upload(in.a.safe_vals);
        b_s.upload(in.b.safe_vals);

        device_bench::print_table_header(name, "streaming, one checked op per element (memory-bound)");
        failures += run_stream_op<device_bench::op_add>(ctx, in, a_b, b_b, out_b, a_s, b_s, out_s, cfg);
        failures += run_stream_op<device_bench::op_sub>(ctx, in, a_b, b_b, out_b, a_s, b_s, out_s, cfg);
        failures += run_stream_op<device_bench::op_mul>(ctx, in, a_b, b_b, out_b, a_s, b_s, out_s, cfg);
        failures += run_stream_op<device_bench::op_div>(ctx, in, a_b, b_b, out_b, a_s, b_s, out_s, cfg);
        failures += run_stream_op<device_bench::op_mod>(ctx, in, a_b, b_b, out_b, a_s, b_s, out_s, cfg);
    }

    {
        const auto in {device_bench::gen_int_chain<TB, TS>(cfg.chain_threads)};

        device_bench::print_table_header(name, "register chain of checked ops (compute-bound)");
        failures += run_chain<device_bench::int_mixed_chain>(ctx, in, cfg);
    }

    return failures;
}

template <typename TS>
int run_real_family(const char* name, boost::safe_numbers::device_error_context& ctx, const bench_config& cfg)
{
    using TB = typename TS::basis_type;

    auto failures {0};

    {
        const auto in {device_bench::gen_stream_real<TB, TS>(cfg.stream_n)};

        device_array<TB> a_b {static_cast<std::size_t>(cfg.stream_n)};
        device_array<TB> b_b {static_cast<std::size_t>(cfg.stream_n)};
        device_array<TB> out_b {static_cast<std::size_t>(cfg.stream_n)};
        a_b.upload(in.a.builtin_vals);
        b_b.upload(in.b.builtin_vals);

        device_array<TS> a_s {static_cast<std::size_t>(cfg.stream_n)};
        device_array<TS> b_s {static_cast<std::size_t>(cfg.stream_n)};
        device_array<TS> out_s {static_cast<std::size_t>(cfg.stream_n)};
        a_s.upload(in.a.safe_vals);
        b_s.upload(in.b.safe_vals);

        device_bench::print_table_header(name, "streaming, one checked op per element (memory-bound)");
        failures += run_stream_op<device_bench::op_add>(ctx, in, a_b, b_b, out_b, a_s, b_s, out_s, cfg);
        failures += run_stream_op<device_bench::op_sub>(ctx, in, a_b, b_b, out_b, a_s, b_s, out_s, cfg);
        failures += run_stream_op<device_bench::op_mul>(ctx, in, a_b, b_b, out_b, a_s, b_s, out_s, cfg);
        failures += run_stream_op<device_bench::op_div>(ctx, in, a_b, b_b, out_b, a_s, b_s, out_s, cfg);
    }

    {
        device_bench::print_table_header(name, "register chain of checked ops (compute-bound)");

        const auto add_in {device_bench::gen_real_chain<TB, TS>(cfg.chain_threads, device_bench::real_chain_kind::sum)};
        failures += run_chain<device_bench::real_add_chain>(ctx, add_in, cfg);
        failures += run_chain<device_bench::real_sub_chain>(ctx, add_in, cfg);

        const auto mul_in {device_bench::gen_real_chain<TB, TS>(cfg.chain_threads, device_bench::real_chain_kind::product)};
        failures += run_chain<device_bench::real_mul_chain>(ctx, mul_in, cfg);

        const auto div_in {device_bench::gen_real_chain<TB, TS>(cfg.chain_threads, device_bench::real_chain_kind::divisor)};
        failures += run_chain<device_bench::real_div_chain>(ctx, div_in, cfg);
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

    cudaDeviceProp prop {};
    cuda_check(cudaGetDeviceProperties(&prop, 0), "cudaGetDeviceProperties");
    std::printf("Boost.safe_numbers CUDA benchmark\n");
    std::printf("Device: %s (sm_%d%d, %d SMs)\n", prop.name, prop.major, prop.minor, prop.multiProcessorCount);
    std::printf("Streaming elements: %d, chain threads: %d, chain cycles: %d, launches per timing: %d\n",
                cfg.stream_n, cfg.chain_threads, cfg.chain_cycles, cfg.launches);

    auto failures {0};

    try
    {
        boost::safe_numbers::device_error_context ctx;

        failures += run_int_family<boost::safe_numbers::u8>("u8", ctx, cfg);
        failures += run_int_family<boost::safe_numbers::u16>("u16", ctx, cfg);
        failures += run_int_family<boost::safe_numbers::u32>("u32", ctx, cfg);
        failures += run_int_family<boost::safe_numbers::u64>("u64", ctx, cfg);
        failures += run_int_family<boost::safe_numbers::u128>("u128", ctx, cfg);

        failures += run_int_family<boost::safe_numbers::i8>("i8", ctx, cfg);
        failures += run_int_family<boost::safe_numbers::i16>("i16", ctx, cfg);
        failures += run_int_family<boost::safe_numbers::i32>("i32", ctx, cfg);
        failures += run_int_family<boost::safe_numbers::i64>("i64", ctx, cfg);
        failures += run_int_family<boost::safe_numbers::i128>("i128", ctx, cfg);

        failures += run_real_family<boost::safe_numbers::f32>("f32", ctx, cfg);
        failures += run_real_family<boost::safe_numbers::f64>("f64", ctx, cfg);
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
