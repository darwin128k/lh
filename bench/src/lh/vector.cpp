#include <bench/bench.h>

#include <lh/vector.h>

namespace
{
constexpr lh_usize_t kCount = 1000;
}

BENCH(vector_push_back_grow)
{
    for (std::uint64_t i = 0; i < iterations; ++i)
    {
        lh_vector_t v;
        lh_vector_init(&v, sizeof(int));
        for (lh_usize_t n = 0; n < kCount; ++n)
        {
            int value = static_cast<int>(n);
            lh_vector_push_back(&v, &value);
        }
        bench::DoNotOptimize(lh_vector_get_size(&v));
        lh_memory_typed_allocated_clear(&v.typed);
    }
}

BENCH(vector_push_back_reserved)
{
    for (std::uint64_t i = 0; i < iterations; ++i)
    {
        lh_vector_t v;
        lh_vector_init(&v, sizeof(int));
        lh_vector_reserve(&v, kCount);
        for (lh_usize_t n = 0; n < kCount; ++n)
        {
            int value = static_cast<int>(n);
            lh_vector_push_back(&v, &value);
        }
        bench::DoNotOptimize(lh_vector_get_size(&v));
        lh_memory_typed_allocated_clear(&v.typed);
    }
}

BENCH(vector_get_ptr_random_access)
{
    lh_vector_t v;
    lh_vector_init(&v, sizeof(int));
    lh_vector_reserve(&v, kCount);
    for (lh_usize_t n = 0; n < kCount; ++n)
    {
        int value = static_cast<int>(n);
        lh_vector_push_back(&v, &value);
    }

    for (std::uint64_t i = 0; i < iterations; ++i)
    {
        const lh_usize_t index = static_cast<lh_usize_t>(i) % kCount;
        bench::DoNotOptimize(lh_vector_get_ptr(&v, index));
    }

    lh_memory_typed_allocated_clear(&v.typed);
}
