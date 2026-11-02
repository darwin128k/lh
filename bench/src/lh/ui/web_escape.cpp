/**
 * @file web_escape.cpp
 * @brief Measure HTML and JavaScript string serialization.
 */

#include <benchmark/benchmark.h>

#include <lh/ui/web/html.h>
#include <lh/char/map.h>
#include <lh/ui/web/js/x.h>

#include <string>
#include <vector>

/** Measure one string with either no escapes or one escaped byte every eight. */
void
lh_bench_web_escape(benchmark::State &state, bool javascript)
{
    std::string input(static_cast<std::size_t>(state.range(0)), lh_char_map_small_x);
    if (state.range(1) != 0)
    {
        for (std::size_t i = 7; i < input.size(); i += 8)
        {
            input[i] = javascript ? lh_char_map_squote : lh_char_map_ampersand;
        }
    }
    std::vector<lh_byte_t> output(input.size() * 6u + 3u);
    lh_ui_web_buf_t buffer;
    for (auto iteration : state)
    {
        lh_ui_web_buf_init(lh_addr_of(buffer), output.data(), output.size());
        if (javascript)
        {
            lh_ui_web_js_put_str_lit(lh_addr_of(buffer), input.c_str());
        }
        else
        {
            lh_ui_web_html_put_escaped(lh_addr_of(buffer), input.c_str(), lh_bool_false);
        }
        benchmark::DoNotOptimize(lh_ui_web_buf_len(lh_addr_of(buffer)));
        benchmark::ClobberMemory();
    }
    if (!lh_ui_web_buf_ok(lh_addr_of(buffer)))
    {
        state.SkipWithError("escaped string did not fit");
    }
    state.SetBytesProcessed(state.iterations() * static_cast<std::int64_t>(input.size()));
}

void
BM_ui_web_escape_js(benchmark::State &state)
{
    lh_bench_web_escape(state, true);
}

BENCHMARK(BM_ui_web_escape_js)
    ->Args({4, 0})
    ->Args({64, 0})
    ->Args({64, 1})
    ->Args({1024, 0})
    ->Args({1024, 1});

void
BM_ui_web_escape_html(benchmark::State &state)
{
    lh_bench_web_escape(state, false);
}

BENCHMARK(BM_ui_web_escape_html)
    ->Args({4, 0})
    ->Args({64, 0})
    ->Args({64, 1})
    ->Args({1024, 0})
    ->Args({1024, 1});

/** Serialize a two-field object, as used by the browser player. */
void
BM_ui_web_js_object(benchmark::State &state)
{
    const lh_str_ptr keys[] = {"x", "y"};
    lh_ui_web_x_t one, two, object;
    lh_ui_web_x_num(1, lh_addr_of(one));
    lh_ui_web_x_num(2, lh_addr_of(two));
    const lh_ui_web_x_t *values[] = {lh_addr_of(one), lh_addr_of(two)};
    lh_ui_web_x_obj(keys, values, 2, lh_addr_of(object));
    lh_byte_t output[128];
    lh_ui_web_buf_t buffer;
    for (auto iteration : state)
    {
        lh_ui_web_buf_init(lh_addr_of(buffer), output, sizeof(output));
        lh_ui_web_js_x(lh_addr_of(buffer), lh_addr_of(object));
        benchmark::DoNotOptimize(lh_ui_web_buf_len(lh_addr_of(buffer)));
        benchmark::ClobberMemory();
    }
    if (!lh_ui_web_buf_ok(lh_addr_of(buffer)))
    {
        state.SkipWithError("object did not fit");
    }
}

BENCHMARK(BM_ui_web_js_object);
