#include <gtest/gtest.h>

#include <cstring>
#include <string>

#include <lh/io/reader.h>

namespace
{

struct MemorySource
{
    const char *data;
    lh_usize_t size;
    lh_usize_t pos;
};

lh_ssize_t
MemorySourceRead(lh_ptr context, lh_ptr buf, lh_usize_t size)
{
    MemorySource *src = static_cast<MemorySource *>(context);
    lh_usize_t remaining = src->size - src->pos;
    lh_usize_t to_copy = size < remaining ? size : remaining;
    std::memcpy(buf, src->data + src->pos, to_copy);
    src->pos += to_copy;
    return static_cast<lh_ssize_t>(to_copy);
}

TEST(io_reader_set, roundtrip_via_getters)
{
    MemorySource src{"hello", 5, 0};
    lh_io_reader_t reader{};
    lh_io_reader_set(&reader, &MemorySourceRead, &src);

    EXPECT_EQ(lh_io_reader_get_read_cb(&reader), &MemorySourceRead);
    EXPECT_EQ(lh_io_reader_get_context(&reader), static_cast<lh_ptr>(&src));
}

TEST(io_reader_read, reads_full_buffer)
{
    MemorySource src{"hello", 5, 0};
    lh_io_reader_t reader{};
    lh_io_reader_init(&reader, &MemorySourceRead, &src);

    char buf[16] = {};
    lh_ssize_t n = lh_io_reader_read(&reader, buf, sizeof(buf));
    ASSERT_EQ(n, 5);
    EXPECT_EQ(std::string(buf, static_cast<size_t>(n)), "hello");
}

TEST(io_reader_read, reads_in_chunks)
{
    MemorySource src{"hello", 5, 0};
    lh_io_reader_t reader{};
    lh_io_reader_init(&reader, &MemorySourceRead, &src);

    char buf[2] = {};
    EXPECT_EQ(lh_io_reader_read(&reader, buf, 2), 2);
    EXPECT_EQ(std::string(buf, 2), "he");
    EXPECT_EQ(lh_io_reader_read(&reader, buf, 2), 2);
    EXPECT_EQ(std::string(buf, 2), "ll");
    EXPECT_EQ(lh_io_reader_read(&reader, buf, 2), 1);
    EXPECT_EQ(buf[0], 'o');
    EXPECT_EQ(lh_io_reader_read(&reader, buf, 2), 0); /* end of stream */
}

TEST(io_reader_deinit, clears_callback_and_context)
{
    MemorySource src{"hi", 2, 0};
    lh_io_reader_t reader{};
    lh_io_reader_init(&reader, &MemorySourceRead, &src);
    lh_io_reader_deinit(&reader);

    EXPECT_EQ(lh_io_reader_get_read_cb(&reader), nullptr);
    EXPECT_EQ(lh_io_reader_get_context(&reader), nullptr);
}

TEST(io_reader_assign, copies_callback_and_context)
{
    MemorySource src{"abc", 3, 0};
    lh_io_reader_t a{};
    lh_io_reader_t b{};
    lh_io_reader_init(&a, &MemorySourceRead, &src);
    lh_io_reader_assign(&b, &a);

    char buf[8] = {};
    lh_ssize_t n = lh_io_reader_read(&b, buf, sizeof(buf));
    ASSERT_EQ(n, 3);
    EXPECT_EQ(std::string(buf, static_cast<size_t>(n)), "abc");
}

} // namespace
