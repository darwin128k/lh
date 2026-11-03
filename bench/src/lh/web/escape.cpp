/**
 * @file escape.cpp
 * @brief Measure HTML and JavaScript string serialization.
 */

#include <benchmark/benchmark.h>

#include <lh/char/map.h>
#include <lh/web/js/node.h>
#include <lh/web/js/writer.h>
#include <lh/web/xml/escape.h>

#include <cstdint>
#include <string>

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
    lh_str_view_t text;
    lh_str_view_init_by_size(lh_addr_of(text), input.data(), input.size());
    lh_str_t out;
    lh_str_init(lh_addr_of(out));
    lh_web_js_writer_t writer;
    for (auto iteration : state)
    {
        /* Cleared, not freed: after the first pass the string keeps its
         * capacity and the loop measures the escaping alone. */
        lh_str_clear(lh_addr_of(out));
        if (javascript)
        {
            lh_web_js_writer_init(lh_addr_of(writer), lh_addr_of(out));
            lh_web_js_writer_put_string(lh_addr_of(writer), text);
        }
        else
        {
            lh_web_xml_escape_append(lh_addr_of(out), text, lh_bool_false);
        }
        benchmark::DoNotOptimize(lh_str_get_size(lh_addr_of(out)));
        benchmark::ClobberMemory();
    }
    lh_str_deinit(lh_addr_of(out));
    state.SetBytesProcessed(state.iterations() * static_cast<std::int64_t>(input.size()));
}

void
BM_web_escape_js(benchmark::State &state)
{
    lh_bench_web_escape(state, true);
}

BENCHMARK(BM_web_escape_js)
    ->Args({4, 0})
    ->Args({64, 0})
    ->Args({64, 1})
    ->Args({1024, 0})
    ->Args({1024, 1});

void
BM_web_escape_html(benchmark::State &state)
{
    lh_bench_web_escape(state, false);
}

BENCHMARK(BM_web_escape_html)
    ->Args({4, 0})
    ->Args({64, 0})
    ->Args({64, 1})
    ->Args({1024, 0})
    ->Args({1024, 1});

/** Serialize a two-field object, as used by the browser player. */
void
BM_web_js_object(benchmark::State &state)
{
    lh_web_js_node_t *object = lh_web_js_node_push_back(
        lh_web_js_node_push_back(
            lh_web_js_node_create_with_kind(lh_web_js_kind_object),
            lh_web_js_node_create_property(lh_str_view_lit("x"), lh_web_js_node_create_number(1U))),
        lh_web_js_node_create_property(lh_str_view_lit("y"), lh_web_js_node_create_number(2U)));
    lh_str_t out;
    lh_str_init(lh_addr_of(out));
    lh_web_js_writer_t writer;
    bool written = true;
    for (auto iteration : state)
    {
        lh_str_clear(lh_addr_of(out));
        lh_web_js_writer_init(lh_addr_of(writer), lh_addr_of(out));
        written = lh_web_js_writer_try_write_bare(lh_addr_of(writer), object) && written;
        benchmark::DoNotOptimize(lh_str_get_size(lh_addr_of(out)));
        benchmark::ClobberMemory();
    }
    if (!written)
    {
        state.SkipWithError("object was not written");
    }
    lh_str_deinit(lh_addr_of(out));
    lh_web_js_node_destroy(object);
}

BENCHMARK(BM_web_js_object);
