#include <bench/bench.h>

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

BENCH_BYTES(io_reader_read_4KB, kChunkSize)
{
    static std::vector<char> data(kChunkSize, 'x');
    static char dst[kChunkSize];

    MemorySource src{data.data(), data.size(), 0};
    lh_io_reader_t reader;
    lh_io_reader_init(&reader, &MemorySourceRead, &src);

    for (std::uint64_t i = 0; i < iterations; ++i)
    {
        src.pos = 0;
        bench::DoNotOptimize(lh_io_reader_read(&reader, dst, sizeof(dst)));
    }
}

BENCH_BYTES(io_writer_write_4KB, kChunkSize)
{
    static std::vector<char> src(kChunkSize, 'x');
    static char dst[kChunkSize];

    MemorySink sink{dst, sizeof(dst), 0};
    lh_io_writer_t writer;
    lh_io_writer_init(&writer, &MemorySinkWrite, &sink);

    for (std::uint64_t i = 0; i < iterations; ++i)
    {
        sink.pos = 0;
        bench::DoNotOptimize(lh_io_writer_write(&writer, src.data(), src.size()));
    }
}

BENCH_BYTES(io_reader_read_1KB_chunks_of_64B, 1024)
{
    static std::vector<char> data(1024, 'x');
    static char dst[64];

    MemorySource src{data.data(), data.size(), 0};
    lh_io_reader_t reader;
    lh_io_reader_init(&reader, &MemorySourceRead, &src);

    for (std::uint64_t i = 0; i < iterations; ++i)
    {
        src.pos = 0;
        while (src.pos < src.size)
        {
            bench::DoNotOptimize(lh_io_reader_read(&reader, dst, sizeof(dst)));
        }
    }
}
