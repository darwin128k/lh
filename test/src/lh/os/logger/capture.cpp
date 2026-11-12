#include <gtest/gtest.h>

#include <lh/os/logger/capture.h>
#include <lh/os/thread.h>

#include <cstdarg>
#include <string>
#include <vector>

namespace
{

/* What a replay saw: each line's target, context, level and text, and whether
   capture was suspended while it ran. */
struct replay_log
{
    std::vector<std::string> texts;
    std::vector<lh_logger_level_t> levels;
    std::vector<const lh_void *> targets;
    std::vector<lh_ptr> contexts;
    bool suspended = true;

    static lh_void
    replay(lh_ptr user, const lh_os_logger_capture_line_t *line)
    {
        auto *self = static_cast<replay_log *>(user);
        const lh_os_logger_record_t *record = lh_os_logger_capture_line_get_record(line);

        self->texts.emplace_back(lh_os_logger_record_get_text(record));
        self->levels.push_back(lh_os_logger_record_get_level(record));
        self->targets.push_back(lh_os_logger_capture_line_get_target(line));
        self->contexts.push_back(lh_os_logger_capture_line_get_context(line));
        self->suspended = self->suspended && lh_os_logger_capture_get_current() == lh_null;
    }
};

lh_bool_t
push(lh_os_logger_capture_t *capture, const lh_void *target, lh_ptr context,
     lh_logger_level_t level, const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    const lh_bool_t pushed =
        lh_os_logger_capture_try_push_v(capture, target, context, level, fmt, args);
    va_end(args);
    return pushed;
}

} // namespace

TEST(lh_os_logger_capture, holds_lines_in_order_with_their_target)
{
    int target = 0;
    int context = 0;

    lh_os_logger_capture_line_t lines[4];
    lh_os_logger_capture_t capture;
    lh_os_logger_capture_init(&capture, lines, 4U);
    EXPECT_TRUE(push(&capture, &target, &context, lh_logger_level_info, "a %d", 1));
    EXPECT_TRUE(push(&capture, &target, lh_null, lh_logger_level_error, "%s", "b"));
    EXPECT_EQ(lh_os_logger_capture_get_count(&capture), 2U);
    EXPECT_EQ(lh_os_logger_capture_get_line(&capture, 2U), nullptr);

    replay_log log;
    lh_os_logger_capture_replay(&capture, replay_log::replay, &log);
    ASSERT_EQ(log.texts.size(), 2U);
    EXPECT_EQ(log.texts[0], "a 1");
    EXPECT_EQ(log.texts[1], "b");
    EXPECT_EQ(log.levels[0], lh_logger_level_info);
    EXPECT_EQ(log.levels[1], lh_logger_level_error);
    EXPECT_EQ(log.targets[0], &target);
    EXPECT_EQ(log.contexts[0], &context);
    EXPECT_EQ(log.contexts[1], nullptr);
    /* Replay leaves the lines held; clear forgets them. */
    EXPECT_EQ(lh_os_logger_capture_get_count(&capture), 2U);
    lh_os_logger_capture_clear(&capture);
    EXPECT_EQ(lh_os_logger_capture_get_count(&capture), 0U);
}

TEST(lh_os_logger_capture, full_buffer_drops_and_counts)
{
    lh_os_logger_capture_line_t lines[1];
    lh_os_logger_capture_t capture;

    lh_os_logger_capture_init(&capture, lines, 1U);
    EXPECT_TRUE(push(&capture, lh_null, lh_null, lh_logger_level_info, "kept"));
    EXPECT_FALSE(push(&capture, lh_null, lh_null, lh_logger_level_info, "lost"));
    EXPECT_FALSE(push(&capture, lh_null, lh_null, lh_logger_level_info, "lost"));
    EXPECT_EQ(lh_os_logger_capture_get_count(&capture), 1U);
    EXPECT_EQ(lh_os_logger_capture_get_dropped(&capture), 2U);
    lh_os_logger_capture_clear(&capture);
    EXPECT_EQ(lh_os_logger_capture_get_dropped(&capture), 0U);
}

TEST(lh_os_logger_capture, long_text_is_cut)
{
    const std::string text(LH_OS_LOGGER_RECORD_TEXT_SIZE + 10U, 'x');

    lh_os_logger_capture_line_t lines[1];
    lh_os_logger_capture_t capture;
    lh_os_logger_capture_init(&capture, lines, 1U);
    EXPECT_TRUE(push(&capture, lh_null, lh_null, lh_logger_level_info, "%s", text.c_str()));
    const lh_os_logger_record_t *record =
        lh_os_logger_capture_line_get_record(lh_os_logger_capture_get_line(&capture, 0U));
    EXPECT_EQ(std::string(lh_os_logger_record_get_text(record)).size(),
              LH_OS_LOGGER_RECORD_TEXT_SIZE - 1U);
}

TEST(lh_os_logger_capture, scope_nests_and_replay_suspends_it)
{
    lh_os_logger_capture_line_t lines[1];
    lh_os_logger_capture_t outer;

    lh_os_logger_capture_init(&outer, lines, 1U);
    lh_os_logger_capture_t inner;
    lh_os_logger_capture_init(&inner, lines, 1U);
    EXPECT_EQ(lh_os_logger_capture_get_current(), nullptr);

    lh_os_logger_capture_t *const none = lh_os_logger_capture_begin(&outer);
    EXPECT_EQ(none, nullptr);

    lh_os_logger_capture_t *const previous = lh_os_logger_capture_begin(&inner);
    EXPECT_EQ(previous, &outer);
    EXPECT_EQ(lh_os_logger_capture_get_current(), &inner);
    EXPECT_TRUE(push(&inner, lh_null, lh_null, lh_logger_level_info, "x"));
    replay_log log;
    lh_os_logger_capture_replay(&inner, replay_log::replay, &log);
    EXPECT_TRUE(log.suspended);
    EXPECT_EQ(lh_os_logger_capture_get_current(), &inner);
    lh_os_logger_capture_end(previous);
    EXPECT_EQ(lh_os_logger_capture_get_current(), &outer);
    lh_os_logger_capture_end(none);
    EXPECT_EQ(lh_os_logger_capture_get_current(), nullptr);
}

TEST(lh_os_logger_capture, scope_is_per_thread)
{
    lh_os_logger_capture_t capture;
    lh_os_logger_capture_t *seen = &capture;

    lh_os_logger_capture_line_t lines[1];
    lh_os_logger_capture_init(&capture, lines, 1U);
    lh_os_thread_t thread;
    lh_os_thread_init(&thread);
    lh_os_logger_capture_t *const previous = lh_os_logger_capture_begin(&capture);
    ASSERT_TRUE(lh_os_thread_start(
        &thread, [](lh_ptr user)
        { *static_cast<lh_os_logger_capture_t **>(user) = lh_os_logger_capture_get_current(); },
        &seen, -1, 0U));
    ASSERT_TRUE(lh_os_thread_join(&thread));
    lh_os_logger_capture_end(previous);
    EXPECT_EQ(seen, nullptr);
}
