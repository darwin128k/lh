#include <gtest/gtest.h>

#include <lh/core/library/fallback.h>
#include <lh/expect/death.h>
#include <lh/core/runtime/reader.h>

#include <thread>

namespace
{

lh_ssize_t
test_read(lh_self_ptr self, lh_ptr buf, lh_usize_t size)
{
    (void)self;
    (void)buf;
    return (lh_ssize_t)size;
}

/* The read callback a fresh thread finds in its own slot. */
lh_io_reader_read_cb
read_cb_on_new_thread(void)
{
    lh_io_reader_read_cb seen = lh_null;
    std::thread([&] { seen = lh_io_reader_get_read_cb(lh_runtime_reader_get_mut()); }).join();
    return seen;
}

/* One object for the whole program: a change on one thread is seen on all. */
TEST(runtime_reader_get, is_the_same_object_read_only)
{
    EXPECT_EQ(lh_runtime_reader_get(), lh_runtime_reader_get_mut());
}

TEST(runtime_reader, is_one_slot_for_every_thread)
{
    lh_io_reader_t saved;
    lh_io_reader_assign(&saved, lh_runtime_reader_get());
    lh_io_reader_init(lh_runtime_reader_get_mut(), test_read, lh_null);
    EXPECT_EQ(read_cb_on_new_thread(), test_read);
    lh_io_reader_assign(lh_runtime_reader_get_mut(), &saved);
}

} // namespace
