#include <gtest/gtest.h>

#include <lh/library/fallback.h>
#include <lh/expect/death.h>
#include <lh/memory/allocator/initializer.h>
#include <lh/runtime/allocator.h>

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

const lh_memory_allocator_t test_allocator =
    lh_memory_allocator_initializer(test_alloc, test_dealloc);

/* The alloc callback a fresh thread finds in its own slot. */
lh_memory_allocator_alloc_cb
alloc_cb_on_new_thread(void)
{
    lh_memory_allocator_alloc_cb seen = lh_null;
    std::thread([&] { seen = lh_memory_allocator_get_alloc_cb(lh_runtime_allocator()); }).join();
    return seen;
}

TEST(runtime_allocator_set_process, publishes_the_allocator_by_address)
{
    const lh_memory_allocator_t *saved = lh_runtime_allocator_process();
    lh_runtime_allocator_set_process(&test_allocator);
    EXPECT_EQ(lh_runtime_allocator_process(), &test_allocator);
    lh_runtime_allocator_set_process(saved);
    EXPECT_EQ(lh_runtime_allocator_process(), saved);
}

TEST(runtime_allocator_process, is_never_null)
{
    EXPECT_NE(lh_runtime_allocator_process(), nullptr);
}

TEST(runtime_allocator_set_process_death, rejects_null)
{
    LH_EXPECT_DEATH(lh_runtime_allocator_set_process(lh_null));
}

#if (LH_LIBRARY_OPTION_THREAD_LOCAL == LH_LIBRARY_OPTION_ON)

TEST(runtime_allocator_process, seeds_a_thread_on_its_first_use)
{
    /* Seed this thread first: the change below must not reach it. */
    lh_memory_allocator_alloc_cb own = lh_memory_allocator_get_alloc_cb(lh_runtime_allocator());
    const lh_memory_allocator_t *saved = lh_runtime_allocator_process();
    lh_runtime_allocator_set_process(&test_allocator);
    EXPECT_EQ(alloc_cb_on_new_thread(), test_alloc);
    EXPECT_EQ(lh_memory_allocator_get_alloc_cb(lh_runtime_allocator()), own);
    lh_runtime_allocator_set_process(saved);
}

TEST(runtime_allocator, change_stays_in_its_thread)
{
    lh_memory_allocator_t saved;
    lh_memory_allocator_assign(&saved, lh_runtime_allocator());
    lh_memory_allocator_init(lh_runtime_allocator(), test_alloc, test_dealloc);
    EXPECT_EQ(lh_memory_allocator_get_alloc_cb(lh_runtime_allocator()), test_alloc);
    EXPECT_EQ(alloc_cb_on_new_thread(),
              lh_memory_allocator_get_alloc_cb(lh_runtime_allocator_process()));
    lh_memory_allocator_assign(lh_runtime_allocator(), &saved);
}

#else

/* THREAD_LOCAL=OFF: the thread slot is one object for every thread. */
TEST(runtime_allocator, is_one_slot_for_every_thread)
{
    lh_memory_allocator_t saved;
    lh_memory_allocator_assign(&saved, lh_runtime_allocator());
    lh_memory_allocator_init(lh_runtime_allocator(), test_alloc, test_dealloc);
    EXPECT_EQ(alloc_cb_on_new_thread(), test_alloc);
    lh_memory_allocator_assign(lh_runtime_allocator(), &saved);
}

#endif /* LH_LIBRARY_OPTION_THREAD_LOCAL */

} // namespace
