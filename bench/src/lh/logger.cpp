#include <benchmark/benchmark.h>

#include <lh/logger.h>
#include <lh/str/format/text.h>

#include <cstdarg>
#include <cstring>

namespace
{

struct Sink
{
    char data[256];
    lh_usize_t size;
};

lh_ssize_t
SinkEmit(lh_self_ptr self, lh_logger_level_t /*level*/, const lh_str_ptr fmt, va_list args)
{
    Sink *sink = static_cast<Sink *>(self);
    char buf[256];
    lh_usize_t written = lh_str_ptr_format_text_v(buf, sizeof(buf), fmt, args);
    if (written > sizeof(sink->data) - sink->size)
    {
        written = sizeof(sink->data) - sink->size;
    }
    std::memcpy(sink->data + sink->size, buf, written);
    sink->size += written;
    return static_cast<lh_ssize_t>(written);
}

lh_ssize_t
NullEmit(lh_self_ptr /*self*/, lh_logger_level_t /*level*/, const lh_str_ptr /*fmt*/,
         va_list /*args*/)
{
    return 0;
}

} // namespace

static void
BM_logger_log_enabled(benchmark::State &state)
{
    Sink sink{};
    lh_logger_t logger{};
    lh_logger_init(&logger, lh_logger_level_flags_all, &SinkEmit, &sink);
    for (auto _ : state)
    {
        sink.size = 0;
        benchmark::DoNotOptimize(lh_logger_info(&logger, "n=%d name=%s", 42, "bench"));
    }
}
BENCHMARK(BM_logger_log_enabled);

static void
BM_logger_log_filtered(benchmark::State &state)
{
    Sink sink{};
    lh_logger_t logger{};
    lh_logger_init(&logger, lh_logger_level_flags_error, &SinkEmit, &sink);
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_logger_debug(&logger, "n=%d name=%s", 42, "bench"));
    }
}
BENCHMARK(BM_logger_log_filtered);

static void
BM_logger_log_null_sink(benchmark::State &state)
{
    lh_logger_t logger{};
    lh_logger_init(&logger, lh_logger_level_flags_all, &NullEmit, nullptr);
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_logger_info(&logger, "n=%d", 7));
    }
}
BENCHMARK(BM_logger_log_null_sink);
