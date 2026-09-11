#include <gtest/gtest.h>

#include <cstring>
#include <string>

#include <lh/io/stream.h>

namespace
{

struct LoopbackBuffer
{
    char data[64];
    lh_usize_t write_pos;
    lh_usize_t read_pos;
};

lh_ssize_t
LoopbackRead(lh_ptr context, lh_ptr buf, lh_usize_t size)
{
    LoopbackBuffer *lb = static_cast<LoopbackBuffer *>(context);
    lh_usize_t available = lb->write_pos - lb->read_pos;
    lh_usize_t to_copy = size < available ? size : available;
    std::memcpy(buf, lb->data + lb->read_pos, to_copy);
    lb->read_pos += to_copy;
    return static_cast<lh_ssize_t>(to_copy);
}

lh_ssize_t
LoopbackWrite(lh_ptr context, const lh_ptr buf, lh_usize_t size)
{
    LoopbackBuffer *lb = static_cast<LoopbackBuffer *>(context);
    std::memcpy(lb->data + lb->write_pos, buf, size);
    lb->write_pos += size;
    return static_cast<lh_ssize_t>(size);
}

TEST(io_stream_make, roundtrip_via_getters)
{
    LoopbackBuffer lb{};
    lh_io_reader_t reader{};
    lh_io_writer_t writer{};
    lh_io_reader_init(&reader, &LoopbackRead, &lb);
    lh_io_writer_init(&writer, &LoopbackWrite, &lb);

    lh_io_stream_t stream = lh_io_stream_make(&reader, &writer);

    lh_io_reader_t got_reader = lh_io_stream_get_reader(&stream);
    lh_io_writer_t got_writer = lh_io_stream_get_writer(&stream);
    EXPECT_EQ(lh_io_reader_get_read_cb(&got_reader), &LoopbackRead);
    EXPECT_EQ(lh_io_writer_get_write_cb(&got_writer), &LoopbackWrite);
}

TEST(io_stream_write_read, roundtrips_through_the_same_stream)
{
    LoopbackBuffer lb{};
    lh_io_reader_t reader{};
    lh_io_writer_t writer{};
    lh_io_reader_init(&reader, &LoopbackRead, &lb);
    lh_io_writer_init(&writer, &LoopbackWrite, &lb);

    lh_io_stream_t stream = lh_io_stream_make(&reader, &writer);

    lh_ssize_t written = lh_io_stream_write(&stream, "ping", 4);
    ASSERT_EQ(written, 4);

    char buf[8] = {};
    lh_ssize_t n = lh_io_stream_read(&stream, buf, sizeof(buf));
    ASSERT_EQ(n, 4);
    EXPECT_EQ(std::string(buf, static_cast<size_t>(n)), "ping");
}

TEST(io_stream_set_reader, replaces_only_reader)
{
    LoopbackBuffer lb{};
    lh_io_reader_t reader{};
    lh_io_writer_t writer{};
    lh_io_reader_init(&reader, &LoopbackRead, &lb);
    lh_io_writer_init(&writer, &LoopbackWrite, &lb);
    lh_io_stream_t stream = lh_io_stream_make(&reader, &writer);

    lh_io_reader_t new_reader{};
    lh_io_reader_deinit(&new_reader);
    lh_io_stream_set_reader(&stream, &new_reader);

    lh_io_writer_t got_writer = lh_io_stream_get_writer(&stream);
    EXPECT_EQ(lh_io_writer_get_write_cb(&got_writer), &LoopbackWrite);

    lh_io_reader_t got_reader = lh_io_stream_get_reader(&stream);
    EXPECT_EQ(lh_io_reader_get_read_cb(&got_reader), nullptr);
}

TEST(io_stream_assign, copies_both_halves)
{
    LoopbackBuffer lb{};
    lh_io_reader_t reader{};
    lh_io_writer_t writer{};
    lh_io_reader_init(&reader, &LoopbackRead, &lb);
    lh_io_writer_init(&writer, &LoopbackWrite, &lb);
    lh_io_stream_t a = lh_io_stream_make(&reader, &writer);
    lh_io_stream_t b{};
    lh_io_stream_assign(&b, &a);

    lh_ssize_t written = lh_io_stream_write(&b, "hi", 2);
    ASSERT_EQ(written, 2);
}

} // namespace
