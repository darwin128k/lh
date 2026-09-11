#include <benchmark/benchmark.h>

#include <lh/io/reader.h>
#include <lh/io/writer.h>

#include <cstring>
#include <vector>

namespace
{

struct MemorySource
{
    const char *data;
    lh_usize_t size;
    lh_usize_t pos;
};

lh_ssize_t
MemorySourceRead(lh_ptr context, lh_ptr buf, lh_usize_t size)
{
    MemorySource *src = static_cast<MemorySource *>(context);
    const lh_usize_t remaining = src->size - src->pos;
    const lh_usize_t to_copy = size < remaining ? size : remaining;
    std::memcpy(buf, src->data + src->pos, to_copy);
    src->pos += to_copy;
    return static_cast<lh_ssize_t>(to_copy);
}

struct MemorySink
{
    char *data;
    lh_usize_t capacity;
    lh_usize_t pos;
};

lh_ssize_t
MemorySinkWrite(lh_ptr context, const lh_ptr buf, lh_usize_t size)
{
    MemorySink *sink = static_cast<MemorySink *>(context);
    const lh_usize_t remaining = sink->capacity - sink->pos;
    const lh_usize_t to_copy = size < remaining ? size : remaining;
    std::memcpy(sink->data + sink->pos, buf, to_copy);
    sink->pos += to_copy;
    return static_cast<lh_ssize_t>(to_copy);
}

constexpr lh_usize_t kChunkSize = 4096;

} // namespace

static void
BM_io_reader_read_4KB(benchmark::State &state)
{
    std::vector<char> data(kChunkSize, 'x');
    char dst[kChunkSize];

    MemorySource src{data.data(), data.size(), 0};
    lh_io_reader_t reader;
    lh_io_reader_init(&reader, &MemorySourceRead, &src);

    for (auto _ : state)
    {
        src.pos = 0;
        benchmark::DoNotOptimize(lh_io_reader_read(&reader, dst, sizeof(dst)));
    }
    state.SetBytesProcessed(static_cast<std::int64_t>(state.iterations()) * kChunkSize);
}
BENCHMARK(BM_io_reader_read_4KB);

static void
BM_io_writer_write_4KB(benchmark::State &state)
{
    std::vector<char> src(kChunkSize, 'x');
    char dst[kChunkSize];

    MemorySink sink{dst, sizeof(dst), 0};
    lh_io_writer_t writer;
    lh_io_writer_init(&writer, &MemorySinkWrite, &sink);

    for (auto _ : state)
    {
        sink.pos = 0;
        benchmark::DoNotOptimize(lh_io_writer_write(&writer, src.data(), src.size()));
    }
    state.SetBytesProcessed(static_cast<std::int64_t>(state.iterations()) * kChunkSize);
}
BENCHMARK(BM_io_writer_write_4KB);

static void
BM_io_reader_read_1KB_in_64B_chunks(benchmark::State &state)
{
    constexpr lh_usize_t kTotal = 1024;
    constexpr lh_usize_t kStep = 64;
    std::vector<char> data(kTotal, 'x');
    char dst[kStep];

    MemorySource src{data.data(), data.size(), 0};
    lh_io_reader_t reader;
    lh_io_reader_init(&reader, &MemorySourceRead, &src);

    for (auto _ : state)
    {
        src.pos = 0;
        while (src.pos < src.size)
        {
            benchmark::DoNotOptimize(lh_io_reader_read(&reader, dst, sizeof(dst)));
        }
    }
    state.SetBytesProcessed(static_cast<std::int64_t>(state.iterations()) * kTotal);
}
BENCHMARK(BM_io_reader_read_1KB_in_64B_chunks);
