#include <gtest/gtest.h>

#include <lh/core/library/fallback.h>
#include <lh/expect/death.h>
#include <lh/core/runtime/writer.h>

#include <thread>

namespace
{

lh_ssize_t
test_write(lh_self_ptr self, const lh_ptr buf, lh_usize_t size)
{
    (void)self;
    (void)buf;
    return (lh_ssize_t)size;
}

/* The write callback a fresh thread finds in its own slot. */
lh_io_writer_write_cb
write_cb_on_new_thread(void)
{
    lh_io_writer_write_cb seen = lh_null;
    std::thread([&] { seen = lh_io_writer_get_write_cb(lh_runtime_writer_get_mut()); }).join();
    return seen;
}

/* One object for the whole program: a change on one thread is seen on all. */
TEST(runtime_writer_get, is_the_same_object_read_only)
{
    EXPECT_EQ(lh_runtime_writer_get(), lh_runtime_writer_get_mut());
}

TEST(runtime_writer, is_one_slot_for_every_thread)
{
    lh_io_writer_t saved;
    lh_io_writer_assign(&saved, lh_runtime_writer_get());
    lh_io_writer_init(lh_runtime_writer_get_mut(), test_write, lh_null);
    EXPECT_EQ(write_cb_on_new_thread(), test_write);
    lh_io_writer_assign(lh_runtime_writer_get_mut(), &saved);
}

} // namespace
