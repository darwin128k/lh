#include <gtest/gtest.h>

#include <lh/core/logger.h>
#include <lh/os/debug.h>

/* What the debugger shows cannot be read back; the calls must take the text where
   the platform has the channel and refuse it where it has none. */
TEST(os_debug, a_line_reaches_the_channel_where_there_is_one)
{
#if defined(_WIN32)
    EXPECT_TRUE(lh_os_debug_print_line("lh test: a line to the debugger"));
#else
    EXPECT_FALSE(lh_os_debug_print_line("lh test: a line to the debugger"));
#endif
    EXPECT_FALSE(lh_os_debug_print_line(nullptr));
}

TEST(os_debug, the_logger_sink_prints_a_record)
{
    lh_logger_t logger;

    lh_logger_init(&logger, lh_logger_level_flags_all, lh_os_debug_log, nullptr);
#if defined(_WIN32)
    EXPECT_GT(lh_logger_log(&logger, lh_logger_level_info, "lh test: %s=%d", "answer", 42), 0);
#else
    EXPECT_LT(lh_logger_log(&logger, lh_logger_level_info, "lh test: %s=%d", "answer", 42), 0);
#endif
}
