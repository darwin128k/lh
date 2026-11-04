/**
 * @file web_buf.cpp
 * @brief Measure repeated writes to the shared web serialization buffer.
 */

#include <benchmark/benchmark.h>

#include <lh/ui/web/buf.h>
#include <lh/core/char/map.h>

#include <vector>

/** Append 32 equal fragments to an initially empty buffer. */
void
BM_ui_web_buf_append(benchmark::State &state)
{
    const lh_usize_t size = static_cast<lh_usize_t>(state.range(0));
    std::vector<char> source(size, lh_char_map_small_x);
    std::vector<lh_byte_t> output(size * 32u + 1u);
    lh_ui_web_buf_t buffer;
    for (auto iteration : state)
    {
        lh_ui_web_buf_init(lh_addr_of(buffer), output.data(), output.size());
        for (unsigned i = 0; i < 32; ++i)
        {
            lh_ui_web_buf_lit(lh_addr_of(buffer), source.data(), size);
        }
        benchmark::DoNotOptimize(lh_ui_web_buf_len(lh_addr_of(buffer)));
        benchmark::ClobberMemory();
    }
    if (!lh_ui_web_buf_ok(lh_addr_of(buffer)))
    {
        state.SkipWithError("web buffer could not hold the fragments");
    }
    state.SetBytesProcessed(state.iterations() * static_cast<std::int64_t>(size * 32u));
}

BENCHMARK(BM_ui_web_buf_append)->Arg(1)->Arg(4)->Arg(16)->Arg(64)->Arg(256);
