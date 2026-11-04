/**
 * @file alloc_counter.h
 * @brief Test helper: count allocations made through the runtime allocator.
 *
 * Swaps the runtime allocator for a malloc/free pair that counts calls, and
 * restores the previous one on destruction. The slot is one for the whole
 * program: use it while no other thread allocates through lh. The counting
 * pair has no realloc of its own, so growth goes through the alloc + copy +
 * dealloc fallback and every new block is counted.
 */

#ifndef LH_TEST_ALLOC_COUNTER_H
#define LH_TEST_ALLOC_COUNTER_H

#include <lh/core/compiler/extern/c.h>
#include <lh/core/memory/allocator.h>
#include <lh/core/runtime/allocator.h>
#include <lh/core/self.h>

#include <cstdlib>

namespace lh_test
{

inline int &
alloc_count()
{
    static int count = 0;
    return count;
}

LH_COMPILER_EXTERN_C_BEGIN

inline lh_ptr
counting_alloc(lh_self_ptr self, lh_usize_t size)
{
    (void)self;
    ++alloc_count();
    return std::malloc(static_cast<std::size_t>(size));
}

inline lh_void
counting_dealloc(lh_self_ptr self, lh_ptr ptr)
{
    (void)self;
    std::free(ptr);
}

LH_COMPILER_EXTERN_C_END

/** Scoped swap of the runtime allocator for a counting one. */
class alloc_counter
{
public:
    alloc_counter()
    {
        lh_memory_allocator_assign(&m_saved, lh_runtime_allocator_get());
        lh_memory_allocator_set(lh_runtime_allocator_get_mut(), counting_alloc, counting_dealloc);
        alloc_count() = 0;
    }

    ~alloc_counter()
    {
        lh_memory_allocator_assign(lh_runtime_allocator_get_mut(), &m_saved);
    }

    alloc_counter(const alloc_counter &) = delete;
    alloc_counter &operator=(const alloc_counter &) = delete;

    int
    count() const
    {
        return alloc_count();
    }

private:
    lh_memory_allocator_t m_saved;
};

} // namespace lh_test

#endif /* LH_TEST_ALLOC_COUNTER_H */
