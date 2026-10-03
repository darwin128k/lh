#include <gtest/gtest.h>

#include <lh/expect/death.h>
#include <lh/memory/allocator.h>
#include <lh/memory/allocator/initializer.h>
#include <lh/memory/sized/allocator.h>
#include <lh/null.h>
#include <lh/numeric/limits.h>
#include <lh/self.h>

#include <cstdint>
#include <cstdlib>
#include <cstring>

namespace
{

/* The allocator's state: records what the sized functions ask of its callbacks. */
struct recording_heap
{
    int allocs;
    int deallocs;
    lh_usize_t last_alloc_size;
    lh_ptr last_alloc_block;
    lh_ptr last_dealloc_block;
};

#include <lh/compiler/extern/c.h>

LH_COMPILER_EXTERN_C_BEGIN

lh_ptr
recording_alloc(lh_self_ptr self, lh_usize_t size)
{
    recording_heap *heap = static_cast<recording_heap *>(self);
    ++heap->allocs;
    heap->last_alloc_size = size;
    heap->last_alloc_block = std::malloc(static_cast<std::size_t>(size));
    return heap->last_alloc_block;
}

lh_void
recording_dealloc(lh_self_ptr self, lh_ptr ptr)
{
    recording_heap *heap = static_cast<recording_heap *>(self);
    ++heap->deallocs;
    heap->last_dealloc_block = ptr;
    std::free(ptr);
}

LH_COMPILER_EXTERN_C_END

class SizedAllocator : public ::testing::Test
{
protected:
    void
    SetUp() override
    {
        heap = recording_heap();
        sized = lh_memory_allocator_initializer_with_context(recording_alloc, recording_dealloc, lh_null, &heap);
    }

    recording_heap heap;
    lh_memory_sized_allocator_t sized;
};

TEST_F(SizedAllocator, remembers_the_size)
{
    lh_ptr p = lh_memory_sized_allocator_alloc(&sized, 100);
    ASSERT_NE(p, nullptr);
    EXPECT_EQ(lh_memory_sized_allocator_get_size(p), 100u);
    lh_memory_sized_allocator_dealloc(&sized, p);
}

TEST_F(SizedAllocator, asks_callbacks_for_size_plus_header_and_returns_data_past_it)
{
    lh_ptr p = lh_memory_sized_allocator_alloc(&sized, 40);
    EXPECT_EQ(heap.allocs, 1);
    EXPECT_EQ(heap.last_alloc_size, 40u + LH_MEMORY_SIZED_ALLOCATOR_HEADER_SIZE);
    EXPECT_EQ(static_cast<lh_byte_t *>(p),
              static_cast<lh_byte_t *>(heap.last_alloc_block) + LH_MEMORY_SIZED_ALLOCATOR_HEADER_SIZE);

    lh_memory_sized_allocator_dealloc(&sized, p);
    EXPECT_EQ(heap.deallocs, 1);
    EXPECT_EQ(heap.last_dealloc_block, heap.last_alloc_block); // the callback gets back its own block
}

TEST_F(SizedAllocator, keeps_the_callbacks_alignment)
{
    // The header is a multiple of 16, so data is as aligned as the callback's block.
    for (lh_usize_t n : {1u, 7u, 16u, 33u})
    {
        lh_ptr p = lh_memory_sized_allocator_alloc(&sized, n);
        const auto block = reinterpret_cast<std::uintptr_t>(heap.last_alloc_block);
        const auto data = reinterpret_cast<std::uintptr_t>(p);
        EXPECT_EQ(data % 16u, block % 16u) << n;
        lh_memory_sized_allocator_dealloc(&sized, p);
    }
}

TEST_F(SizedAllocator, zero_size_block_is_valid)
{
    lh_ptr p = lh_memory_sized_allocator_alloc(&sized, 0);
    ASSERT_NE(p, nullptr);
    EXPECT_EQ(lh_memory_sized_allocator_get_size(p), 0u);
    lh_memory_sized_allocator_dealloc(&sized, p);
    EXPECT_EQ(heap.deallocs, 1);
}

TEST_F(SizedAllocator, dealloc_null_is_a_no_op)
{
    lh_memory_sized_allocator_dealloc(&sized, lh_null);
    EXPECT_EQ(heap.deallocs, 0);
}

TEST_F(SizedAllocator, realloc_needs_no_old_size_and_keeps_the_data)
{
    lh_ptr p = lh_memory_sized_allocator_alloc(&sized, 4);
    std::memcpy(p, "abcd", 4);

    p = lh_memory_sized_allocator_realloc(&sized, p, 64);
    ASSERT_NE(p, nullptr);
    EXPECT_EQ(lh_memory_sized_allocator_get_size(p), 64u);
    EXPECT_EQ(std::memcmp(p, "abcd", 4), 0);

    p = lh_memory_sized_allocator_realloc(&sized, p, 2);
    EXPECT_EQ(lh_memory_sized_allocator_get_size(p), 2u);
    EXPECT_EQ(std::memcmp(p, "ab", 2), 0);

    lh_memory_sized_allocator_dealloc(&sized, p);
    EXPECT_EQ(heap.allocs, heap.deallocs); // every block the callbacks handed out came back
}

TEST_F(SizedAllocator, realloc_null_allocates_and_zero_frees)
{
    lh_ptr p = lh_memory_sized_allocator_realloc(&sized, lh_null, 8);
    ASSERT_NE(p, nullptr);
    EXPECT_EQ(lh_memory_sized_allocator_get_size(p), 8u);

    EXPECT_EQ(lh_memory_sized_allocator_realloc(&sized, p, 0), nullptr);
    EXPECT_EQ(heap.allocs, heap.deallocs);
}

TEST_F(SizedAllocator, overflowing_size_fails)
{
    LH_EXPECT_DEATH(lh_memory_sized_allocator_alloc(&sized, LH_USIZE_T_MAX));
}

TEST(SizedAllocatorDeath, null_arguments)
{
    LH_EXPECT_DEATH(lh_memory_sized_allocator_alloc(nullptr, 8));
    LH_EXPECT_DEATH(lh_memory_sized_allocator_get_size(lh_null));
}

} // namespace
