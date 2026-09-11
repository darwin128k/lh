#include <gtest/gtest.h>

#include <cstring>
#include <string>

#include <lh/io/writer.h>

namespace
{

struct MemorySink
{
    char data[64];
    lh_usize_t size;
};

lh_ssize_t
MemorySinkWrite(lh_ptr context, const lh_ptr buf, lh_usize_t size)
{
    MemorySink *sink = static_cast<MemorySink *>(context);
    std::memcpy(sink->data + sink->size, buf, size);
    sink->size += size;
    return static_cast<lh_ssize_t>(size);
}

TEST(io_writer_set, roundtrip_via_getters)
{
    MemorySink sink{};
    lh_io_writer_t writer{};
    lh_io_writer_set(&writer, &MemorySinkWrite, &sink);

    EXPECT_EQ(lh_io_writer_get_write_cb(&writer), &MemorySinkWrite);
    EXPECT_EQ(lh_io_writer_get_context(&writer), static_cast<lh_ptr>(&sink));
}

TEST(io_writer_write, writes_full_buffer)
{
    MemorySink sink{};
    lh_io_writer_t writer{};
    lh_io_writer_init(&writer, &MemorySinkWrite, &sink);

    lh_ssize_t n = lh_io_writer_write(&writer, "hello", 5);
    ASSERT_EQ(n, 5);
    EXPECT_EQ(std::string(sink.data, sink.size), "hello");
}

TEST(io_writer_write, writes_in_multiple_calls)
{
    MemorySink sink{};
    lh_io_writer_t writer{};
    lh_io_writer_init(&writer, &MemorySinkWrite, &sink);

    lh_io_writer_write(&writer, "foo", 3);
    lh_io_writer_write(&writer, "bar", 3);
    EXPECT_EQ(std::string(sink.data, sink.size), "foobar");
}

TEST(io_writer_deinit, clears_callback_and_context)
{
    MemorySink sink{};
    lh_io_writer_t writer{};
    lh_io_writer_init(&writer, &MemorySinkWrite, &sink);
    lh_io_writer_deinit(&writer);

    EXPECT_EQ(lh_io_writer_get_write_cb(&writer), nullptr);
    EXPECT_EQ(lh_io_writer_get_context(&writer), nullptr);
}

TEST(io_writer_assign, copies_callback_and_context)
{
    MemorySink sink{};
    lh_io_writer_t a{};
    lh_io_writer_t b{};
    lh_io_writer_init(&a, &MemorySinkWrite, &sink);
    lh_io_writer_assign(&b, &a);

    lh_io_writer_write(&b, "xyz", 3);
    EXPECT_EQ(std::string(sink.data, sink.size), "xyz");
}

} // namespace
