#include <bench/bench.h>

#include <lh/memory/std.h>

#include <vector>

namespace
{

template <lh_usize_t Size>
struct Buffers
{
    std::vector<unsigned char> src{std::vector<unsigned char>(Size, 0x5A)};
    std::vector<unsigned char> dst{std::vector<unsigned char>(Size, 0)};
};

} // namespace

BENCH_BYTES(memory_std_copy_64B, 64)
{
    static Buffers<64> bufs;
    for (std::uint64_t i = 0; i < iterations; ++i)
    {
        bench::DoNotOptimize(lh_memory_std_copy(bufs.dst.data(), bufs.src.data(), bufs.src.size()));
    }
}

BENCH_BYTES(memory_std_copy_4KB, 4096)
{
    static Buffers<4096> bufs;
    for (std::uint64_t i = 0; i < iterations; ++i)
    {
        bench::DoNotOptimize(lh_memory_std_copy(bufs.dst.data(), bufs.src.data(), bufs.src.size()));
    }
}

BENCH_BYTES(memory_std_copy_1MB, 1024 * 1024)
{
    static Buffers<1024 * 1024> bufs;
    for (std::uint64_t i = 0; i < iterations; ++i)
    {
        bench::DoNotOptimize(lh_memory_std_copy(bufs.dst.data(), bufs.src.data(), bufs.src.size()));
    }
}

BENCH_BYTES(memory_std_set_4KB, 4096)
{
    static Buffers<4096> bufs;
    for (std::uint64_t i = 0; i < iterations; ++i)
    {
        bench::DoNotOptimize(lh_memory_std_set(bufs.dst.data(), 0x33, bufs.dst.size()));
    }
}

BENCH_BYTES(memory_std_compare_4KB_equal, 4096)
{
    static Buffers<4096> bufs;
    bufs.dst = bufs.src; // worst case: no mismatch, every byte compared
    for (std::uint64_t i = 0; i < iterations; ++i)
    {
        bench::DoNotOptimize(lh_memory_std_compare(bufs.dst.data(), bufs.src.data(), bufs.dst.size()));
    }
}

BENCH_BYTES(memory_std_compare_4KB_mismatch_at_start, 4096)
{
    static Buffers<4096> bufs; // dst starts at 0, src at 0x5A: differs at byte 0
    for (std::uint64_t i = 0; i < iterations; ++i)
    {
        bench::DoNotOptimize(lh_memory_std_compare(bufs.dst.data(), bufs.src.data(), bufs.dst.size()));
    }
}
