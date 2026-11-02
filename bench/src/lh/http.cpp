/**
 * @file http.cpp
 * @brief Measure HTTP request parsing and body framing separately.
 */

#include <benchmark/benchmark.h>

#include <lh/http/framing.h>
#include <lh/http/parser.h>

/** Measure one complete request head, optionally including body framing. */
void
lh_bench_http_request(benchmark::State &state, bool framing)
{
    const lh_str_view_t input =
        lh_str_view_initializer_lit("POST /api/items?limit=10 HTTP/1.1\r\nHost: localhost\r\n"
                                    "Content-Type: application/json\r\nContent-Length: 17\r\n\r\n");
    lh_http_parser_t parser;
    lh_http_request_t request;
    lh_http_header_t headers[8];
    lh_bool_t valid = lh_bool_true;
    lh_usize_t length = 0;
    lh_http_parser_init(lh_addr_of(parser));
    for (auto _ : state)
    {
        const lh_http_parse_status_t result =
            lh_http_parser_request(lh_addr_of(parser), input, lh_addr_of(request), headers, 8);
        valid = valid && result == lh_http_parse_complete;
        if (framing)
        {
            valid = valid && lh_http_framing_try_get_content_length(
                                 headers, lh_http_request_get_header_count(lh_addr_of(request)),
                                 lh_addr_of(length));
        }
        benchmark::DoNotOptimize(result);
        benchmark::DoNotOptimize(length);
    }
    if (!valid || (framing && length != 17))
    {
        state.SkipWithError("HTTP request parsing or framing failed");
    }
    state.SetBytesProcessed(state.iterations() * lh_str_view_get_size(lh_addr_of(input)));
}

void
BM_http_request_head(benchmark::State &state)
{
    lh_bench_http_request(state, false);
}

BENCHMARK(BM_http_request_head);

void
BM_http_request_head_and_framing(benchmark::State &state)
{
    lh_bench_http_request(state, true);
}

BENCHMARK(BM_http_request_head_and_framing);
