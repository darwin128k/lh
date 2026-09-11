/**
 * @file bench.h
 * @brief Minimal self-registering microbenchmark harness (header-only, no
 *        external dependency) used by lh_bench to find hot spots in ::lh.
 *
 * A benchmark is a function taking an iteration count; it must run its body
 * exactly that many times. The runner geometrically grows the iteration
 * count until a single run takes at least @c target_seconds, then reports
 * the last run's ns/op (and MB/s when the case declares bytes_per_op).
 */

#ifndef LH_BENCH_H
#define LH_BENCH_H

#include <chrono>
#include <cstdint>
#include <cstdio>
#include <functional>
#include <string>
#include <vector>

namespace bench
{

/**
 * @brief Prevents the optimizer from eliding @p value as dead code.
 *
 * Every benchmark body must feed its result through this — otherwise the
 * compiler is free to hoist or delete the very work being measured.
 */
template <class T>
inline void
DoNotOptimize(const T &value)
{
    static volatile T sink;
    sink = value;
}

using Fn = std::function<void(std::uint64_t iterations)>;

struct Case
{
    std::string name;
    Fn fn;
    std::uint64_t bytes_per_op; /**< 0 = no throughput column. */
};

/**
 * @brief Global registry of benchmark cases, populated by ::Registrar at
 *        static-init time and consumed by ::run_all.
 */
class Registry
{
public:
    static Registry &
    instance()
    {
        static Registry registry;
        return registry;
    }

    void
    add(std::string name, Fn fn, std::uint64_t bytes_per_op)
    {
        cases_.push_back(Case{std::move(name), std::move(fn), bytes_per_op});
    }

    /**
     * @brief Run every registered case and print a results table.
     * @param target_seconds Minimum wall time a case's measured run must take.
     */
    void
    run_all(double target_seconds = 0.2) const
    {
        std::printf("%-42s %12s %14s %12s\n", "benchmark", "ns/op", "ops/sec", "MB/s");
        std::printf("%-42s %12s %14s %12s\n", "---------", "-----", "-------", "----");

        for (const Case &c : cases_)
        {
            std::uint64_t iterations = 1;
            double elapsed_seconds = 0.0;

            for (;;)
            {
                const auto start = std::chrono::steady_clock::now();
                c.fn(iterations);
                const auto end = std::chrono::steady_clock::now();
                elapsed_seconds = std::chrono::duration<double>(end - start).count();

                if (elapsed_seconds >= target_seconds || iterations >= (std::uint64_t{1} << 34))
                {
                    break;
                }
                iterations *= 2;
            }

            const double ns_per_op = (elapsed_seconds * 1e9) / static_cast<double>(iterations);
            const double ops_per_sec = static_cast<double>(iterations) / elapsed_seconds;

            std::printf("%-42s %12.2f %14.0f", c.name.c_str(), ns_per_op, ops_per_sec);
            if (c.bytes_per_op > 0)
            {
                const double mb_per_sec = (static_cast<double>(c.bytes_per_op) * ops_per_sec) / (1024.0 * 1024.0);
                std::printf(" %12.2f", mb_per_sec);
            }
            else
            {
                std::printf(" %12s", "-");
            }
            std::printf("\n");
        }
    }

private:
    std::vector<Case> cases_;
};

/**
 * @brief Registers one benchmark case as a side effect of static init;
 *        see ::BENCH / ::BENCH_BYTES.
 */
struct Registrar
{
    Registrar(const std::string &name, Fn fn, std::uint64_t bytes_per_op = 0)
    {
        Registry::instance().add(name, std::move(fn), bytes_per_op);
    }
};

} // namespace bench

/**
 * @def BENCH
 * @brief Define and self-register a benchmark case named @p name.
 *
 * Usage:
 * @code
 * BENCH(my_case)
 * {
 *     for (std::uint64_t i = 0; i < iterations; ++i)
 *     {
 *         // ... do one unit of work ...
 *         bench::DoNotOptimize(result);
 *     }
 * }
 * @endcode
 */
#define BENCH(name)                                                                              \
    static void name(std::uint64_t iterations);                                                  \
    static const ::bench::Registrar name##_registrar{#name, &name};                               \
    static void name(std::uint64_t iterations)

/**
 * @def BENCH_BYTES
 * @brief Same as ::BENCH, but also reports MB/s assuming @p bytes each op moves.
 */
#define BENCH_BYTES(name, bytes)                                                                  \
    static void name(std::uint64_t iterations);                                                  \
    static const ::bench::Registrar name##_registrar{#name, &name, (bytes)};                       \
    static void name(std::uint64_t iterations)

#endif /* LH_BENCH_H */
