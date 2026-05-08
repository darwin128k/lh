#include <gtest/gtest.h>

#include <lh/attribute/unused.h>
#include <lh/expect/death.h>
#include <lh/memory/allocator.h>
#include <lh/memory/allocator/initializer.h>

#include <cstdlib>
#include <cstring>

namespace
{

lh_usize_t g_test_alloc_last_size = 0;
int g_test_alloc_calls = 0;
int g_test_dealloc_calls = 0;

#include <lh/compiler/extern/c.h>

LH_COMPILER_EXTERN_C_BEGIN

lh_ptr
test_alloc_malloc(lh_usize_t size)
{
    ++g_test_alloc_calls;
    g_test_alloc_last_size = size;
    return lh_cast_static(lh_ptr, std::malloc(lh_cast_static(std::size_t, size)));
}

lh_void
test_dealloc_free(lh_ptr ptr)
{
    ++g_test_dealloc_calls;
    std::free(ptr);
}

lh_void
test_dealloc_alt(lh_ptr ptr)
{
    std::free(ptr);
}

lh_ptr
test_alloc_other(lh_usize_t size)
{
    LH_ATTRIBUTE_UNUSED(size);
    return lh_cast_static(lh_ptr, std::malloc(1));
}

lh_ptr
test_alloc_always_null(lh_usize_t size)
{
    LH_ATTRIBUTE_UNUSED(size);
    return nullptr;
}

LH_COMPILER_EXTERN_C_END

TEST(memory_allocator_pack, updates_only_alloc_when_dealloc_pointer_null)
{
    lh_memory_allocator_t a = lh_memory_allocator_initializer(test_alloc_malloc, test_dealloc_free);
    lh_memory_allocator_alloc_cb new_alloc = test_alloc_other;
    lh_memory_allocator_pack(&a, &new_alloc, nullptr);
    EXPECT_EQ(lh_memory_allocator_get_alloc_cb(&a), test_alloc_other);
    EXPECT_EQ(lh_memory_allocator_get_dealloc_cb(&a), test_dealloc_free);
}

TEST(memory_allocator_pack, updates_only_dealloc_when_alloc_pointer_null)
{
    lh_memory_allocator_t a = lh_memory_allocator_initializer(test_alloc_malloc, test_dealloc_free);
    lh_memory_allocator_dealloc_cb new_dealloc = test_dealloc_alt;
    lh_memory_allocator_pack(&a, nullptr, &new_dealloc);
    EXPECT_EQ(lh_memory_allocator_get_alloc_cb(&a), test_alloc_malloc);
    EXPECT_EQ(lh_memory_allocator_get_dealloc_cb(&a), test_dealloc_alt);
}

TEST(memory_allocator_unpack, skips_null_output_pointers)
{
    const lh_memory_allocator_t a =
        lh_memory_allocator_initializer(test_alloc_malloc, test_dealloc_free);
    lh_memory_allocator_alloc_cb ac = nullptr;
    lh_memory_allocator_unpack(&a, &ac, nullptr);
    EXPECT_EQ(ac, test_alloc_malloc);

    lh_memory_allocator_dealloc_cb dc = nullptr;
    lh_memory_allocator_unpack(&a, nullptr, &dc);
    EXPECT_EQ(dc, test_dealloc_free);
}

TEST(memory_allocator_init, matches_set)
{
    lh_memory_allocator_t a = lh_memory_allocator_empty_initializer();
    lh_memory_allocator_t b = lh_memory_allocator_empty_initializer();
    lh_memory_allocator_init(&a, test_alloc_malloc, test_dealloc_free);
    lh_memory_allocator_set(&b, test_alloc_malloc, test_dealloc_free);
    EXPECT_EQ(lh_memory_allocator_get_alloc_cb(&a), lh_memory_allocator_get_alloc_cb(&b));
    EXPECT_EQ(lh_memory_allocator_get_dealloc_cb(&a), lh_memory_allocator_get_dealloc_cb(&b));
}

TEST(memory_allocator_deinit, clears_callbacks)
{
    lh_memory_allocator_t a = lh_memory_allocator_initializer(test_alloc_malloc, test_dealloc_free);
    lh_memory_allocator_deinit(&a);
    EXPECT_EQ(lh_memory_allocator_get_alloc_cb(&a), nullptr);
    EXPECT_EQ(lh_memory_allocator_get_dealloc_cb(&a), nullptr);
}

TEST(memory_allocator_alloc_dealloc, roundtrip_with_malloc_free)
{
    lh_memory_allocator_t a = lh_memory_allocator_empty_initializer();
    g_test_alloc_calls = 0;
    g_test_dealloc_calls = 0;
    lh_memory_allocator_set(&a, test_alloc_malloc, test_dealloc_free);

    lh_ptr p = lh_memory_allocator_alloc(&a, 32);
    ASSERT_NE(p, nullptr);
    EXPECT_EQ(g_test_alloc_calls, 1);
    EXPECT_EQ(g_test_alloc_last_size, 32u);

#if (LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_INIT_ALLOCATED == LH_LIBRARY_OPTION_ON)
    auto *bytes = lh_cast_static(unsigned char *, p);
    for (lh_usize_t i = 0; i < 32; ++i)
    {
        EXPECT_EQ(bytes[i], 0u);
    }
#endif

    std::memset(p, 0xAB, 32);
    lh_memory_allocator_dealloc(&a, p);
    EXPECT_EQ(g_test_dealloc_calls, 1);
}

TEST(memory_allocator_dealloc, null_pointer_noop)
{
    lh_memory_allocator_t a = lh_memory_allocator_initializer(test_alloc_malloc, test_dealloc_free);
    g_test_dealloc_calls = 0;
    lh_memory_allocator_dealloc(&a, nullptr);
    EXPECT_EQ(g_test_dealloc_calls, 0);
}

TEST(memory_allocator_realloc, same_size_returns_same_pointer)
{
    lh_memory_allocator_t a = lh_memory_allocator_empty_initializer();
    lh_memory_allocator_set(&a, test_alloc_malloc, test_dealloc_free);
    lh_ptr p = lh_memory_allocator_alloc(&a, 16);
    ASSERT_NE(p, nullptr);
    lh_ptr q = lh_memory_allocator_realloc(&a, p, 16, 16);
    EXPECT_EQ(q, p);
    lh_memory_allocator_dealloc(&a, p);
}

TEST(memory_allocator_realloc, null_old_pointer_allocates)
{
    lh_memory_allocator_t a = lh_memory_allocator_empty_initializer();
    lh_memory_allocator_set(&a, test_alloc_malloc, test_dealloc_free);
    lh_ptr p = lh_memory_allocator_realloc(&a, nullptr, 0, 24);
    ASSERT_NE(p, nullptr);
    lh_memory_allocator_dealloc(&a, p);
}

TEST(memory_allocator_realloc, zero_new_size_frees_and_returns_null)
{
    lh_memory_allocator_t a = lh_memory_allocator_empty_initializer();
    g_test_dealloc_calls = 0;
    lh_memory_allocator_set(&a, test_alloc_malloc, test_dealloc_free);
    lh_ptr p = lh_memory_allocator_alloc(&a, 8);
    ASSERT_NE(p, nullptr);
    lh_ptr r = lh_memory_allocator_realloc(&a, p, 8, 0);
    EXPECT_EQ(r, nullptr);
    EXPECT_EQ(g_test_dealloc_calls, 1);
}

TEST(memory_allocator_realloc, grows_and_copies_prefix)
{
    lh_memory_allocator_t a = lh_memory_allocator_empty_initializer();
    lh_memory_allocator_set(&a, test_alloc_malloc, test_dealloc_free);
    lh_ptr p = lh_memory_allocator_alloc(&a, 4);
    ASSERT_NE(p, nullptr);
    auto *b = lh_cast_static(unsigned char *, p);
    b[0] = 1;
    b[1] = 2;
    b[2] = 3;
    b[3] = 4;

    lh_ptr q = lh_memory_allocator_realloc(&a, p, 4, 8);
    ASSERT_NE(q, nullptr);
    auto *b2 = lh_cast_static(unsigned char *, q);
    EXPECT_EQ(b2[0], 1u);
    EXPECT_EQ(b2[1], 2u);
    EXPECT_EQ(b2[2], 3u);
    EXPECT_EQ(b2[3], 4u);

    lh_memory_allocator_dealloc(&a, q);
}

#if LH_TEST_EXPECT_DEATH_ENABLED

TEST(memory_allocator_death, pack_null_self)
{
    lh_memory_allocator_alloc_cb ac = test_alloc_malloc;
    lh_memory_allocator_dealloc_cb dc = test_dealloc_free;
    LH_EXPECT_DEATH(lh_memory_allocator_pack(nullptr, &ac, &dc));
}

TEST(memory_allocator_death, unpack_null_self)
{
    lh_memory_allocator_alloc_cb ac = nullptr;
    LH_EXPECT_DEATH(lh_memory_allocator_unpack(nullptr, &ac, nullptr));
}

TEST(memory_allocator_death, alloc_uninitialized_callback)
{
    lh_memory_allocator_t a = lh_memory_allocator_empty_initializer();
    LH_EXPECT_DEATH(lh_memory_allocator_alloc(&a, 1));
}

TEST(memory_allocator_death, alloc_callback_returns_null)
{
    lh_memory_allocator_t a = lh_memory_allocator_empty_initializer();
    lh_memory_allocator_set(&a, test_alloc_always_null, test_dealloc_free);
    LH_EXPECT_DEATH(lh_memory_allocator_alloc(&a, 8));
}

TEST(memory_allocator_death, dealloc_without_callback)
{
    lh_memory_allocator_t a = lh_memory_allocator_empty_initializer();
    int x = 0;
    LH_EXPECT_DEATH(lh_memory_allocator_dealloc(&a, lh_cast_static(lh_ptr, &x)));
}

#endif // LH_TEST_EXPECT_DEATH_ENABLED

} // namespace
