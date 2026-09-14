#include <gtest/gtest.h>

#include <cstring>
#include <string>

#include <lh/logger.h>
#include <lh/str/format/text.h>
#include <lh/util/addr.h>

namespace
{

struct MemorySink
{
    char data[128];
    lh_usize_t size;
    lh_logger_level_t last_level;
};

lh_ssize_t
MemorySinkEmit(lh_ptr context, lh_logger_level_t level, lh_str_cptr fmt, va_list args)
{
    MemorySink *sink = static_cast<MemorySink *>(context);
    char buf[128];
    lh_usize_t written = lh_str_ptr_format_text_v(buf, sizeof(buf), fmt, args);

    std::memcpy(sink->data + sink->size, buf, written);
    sink->size += written;
    sink->last_level = level;
    return static_cast<lh_ssize_t>(written);
}

TEST(logger_set, roundtrip_via_getters)
{
    MemorySink sink{};
    lh_logger_t logger{};
    lh_logger_init(&logger, lh_logger_level_flags_all, &MemorySinkEmit, &sink);

    EXPECT_EQ(lh_logger_get_flags(&logger), lh_logger_level_flags_all);
    EXPECT_EQ(lh_logger_get_emit_cb(&logger, lh_logger_level_error), &MemorySinkEmit);
    EXPECT_EQ(lh_logger_get_emit_cb(&logger, lh_logger_level_debug), &MemorySinkEmit);
    EXPECT_EQ(lh_logger_get_context(&logger), static_cast<lh_ptr>(&sink));
}

TEST(logger_log, formats_and_emits_when_level_enabled)
{
    MemorySink sink{};
    lh_logger_t logger{};
    lh_logger_init(&logger, lh_logger_level_flags_error, &MemorySinkEmit, &sink);

    lh_ssize_t n = lh_logger_error(&logger, "n=%d", 7);
    ASSERT_EQ(n, 3);
    EXPECT_EQ(std::string(sink.data, sink.size), "n=7");
    EXPECT_EQ(sink.last_level, lh_logger_level_error);
}

TEST(logger_log, filter_skips_callback)
{
    MemorySink sink{};
    lh_logger_t logger{};
    lh_logger_init(&logger, lh_logger_level_flags_error, &MemorySinkEmit, &sink);

    EXPECT_EQ(lh_logger_debug(&logger, "secret %s", "x"), 0);
    EXPECT_EQ(sink.size, 0U);
}

TEST(logger_log, null_callback_is_noop)
{
    lh_logger_t logger{};
    lh_logger_init(&logger, lh_logger_level_flags_all, nullptr, nullptr);
    EXPECT_EQ(lh_logger_info(&logger, "hello"), 0);
}

TEST(logger_log, null_slot_skips_even_when_flag_set)
{
    MemorySink sink{};
    lh_logger_t logger{};
    lh_logger_emit_cb debug_cb = nullptr;

    lh_logger_init(&logger, lh_logger_level_flags_all, &MemorySinkEmit, &sink);
    lh_logger_pack(&logger, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
                   lh_addr_of(debug_cb), nullptr);

    EXPECT_EQ(lh_logger_debug(&logger, "nope"), 0);
    EXPECT_EQ(lh_logger_error(&logger, "ok"), 2);
    EXPECT_EQ(std::string(sink.data, sink.size), "ok");
}

TEST(logger_log, umbrella_takes_explicit_level)
{
    MemorySink sink{};
    lh_logger_t logger{};
    lh_logger_init(&logger, lh_logger_level_flags_warning, &MemorySinkEmit, &sink);

    EXPECT_EQ(lh_logger_log(&logger, lh_logger_level_info, "skip"), 0);
    EXPECT_EQ(lh_logger_log(&logger, lh_logger_level_warning, "go"), 2);
    EXPECT_EQ(std::string(sink.data, sink.size), "go");
}

TEST(logger_deinit, clears_callback_and_flags)
{
    MemorySink sink{};
    lh_logger_t logger{};
    lh_logger_init(&logger, lh_logger_level_flags_all, &MemorySinkEmit, &sink);
    lh_logger_deinit(&logger);

    EXPECT_EQ(lh_logger_get_flags(&logger), lh_logger_level_flags_none);
    EXPECT_EQ(lh_logger_get_emit_cb(&logger, lh_logger_level_info), nullptr);
    EXPECT_EQ(lh_logger_get_context(&logger), nullptr);
}

TEST(logger_assign, copies_state)
{
    MemorySink sink{};
    lh_logger_t a{};
    lh_logger_t b{};
    lh_logger_init(&a, lh_logger_level_flags_info, &MemorySinkEmit, &sink);
    lh_logger_assign(&b, &a);

    EXPECT_EQ(lh_logger_info(&b, "xyz"), 3);
    EXPECT_EQ(std::string(sink.data, sink.size), "xyz");
}

} // namespace
