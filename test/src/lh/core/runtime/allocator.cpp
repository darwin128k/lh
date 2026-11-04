#include <gtest/gtest.h>

#include <lh/core/library/fallback.h>
#include <lh/expect/death.h>
#include <lh/core/memory/allocator/initializer.h>
#include <lh/core/runtime/allocator.h>

#include <thread>

namespace
{

lh_ptr
test_alloc(lh_self_ptr self, lh_usize_t size)
{
    (void)self;
    (void)size;
    return lh_null;
}

lh_void
test_dealloc(lh_self_ptr self, lh_ptr ptr)
{
    (void)self;
    (void)ptr;
}

/* The alloc callback a fresh thread finds in its own slot. */
lh_memory_allocator_alloc_cb
alloc_cb_on_new_thread(void)
{
    lh_memory_allocator_alloc_cb seen = lh_null;
    std::thread([&] { seen = lh_memory_allocator_get_alloc_cb(lh_runtime_allocator_get_mut()); })
        .join();
    return seen;
}

/* One object for the whole program: a change on one thread is seen on all. */
TEST(runtime_allocator_get, is_the_same_object_read_only)
{
    EXPECT_EQ(lh_runtime_allocator_get(), lh_runtime_allocator_get_mut());
}

TEST(runtime_allocator, is_one_slot_for_every_thread)
{
    lh_memory_allocator_t saved;
    lh_memory_allocator_assign(&saved, lh_runtime_allocator_get());
    lh_memory_allocator_init(lh_runtime_allocator_get_mut(), test_alloc, test_dealloc);
    EXPECT_EQ(alloc_cb_on_new_thread(), test_alloc);
    lh_memory_allocator_assign(lh_runtime_allocator_get_mut(), &saved);
}

} // namespace
