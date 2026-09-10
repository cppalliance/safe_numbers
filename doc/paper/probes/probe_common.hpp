// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// Shared harness for the semantic probes. Each probe file selects one probe
// with -DPROBE=n, evaluates it inside run_probe, and prints exactly one line:
// "VALUE <n>" for a result that was produced, or "THREW <type>: <what>".
// A probe that does not compile is recorded by the driver as a compile-time
// rejection, so every outcome is observable from the driver.

#ifndef PROBE_COMMON_HPP
#define PROBE_COMMON_HPP

#include <cstdio>
#include <cstdlib>
#include <exception>
#include <typeinfo>
#include <cxxabi.h>

template <typename F>
int run_probe(F&& f)
{
    try
    {
        f();
    }
    catch (const std::exception& e)
    {
        int status {};
        char* name {abi::__cxa_demangle(typeid(e).name(), nullptr, nullptr, &status)};
        std::printf("THREW %s: %s\n", status == 0 ? name : typeid(e).name(), e.what());
        std::free(name);
    }
    catch (...)
    {
        std::printf("THREW unknown\n");
    }

    return 0;
}

template <typename T>
void print_value(const T value)
{
    std::printf("VALUE %lld\n", static_cast<long long>(value));
}

#endif // PROBE_COMMON_HPP
