/**
 * @file logger.cpp
 * @brief The runtime logger: one instance library code writes to, reconfigured by the app.
 */

#include <gtest/gtest.h>

#include <lh/config.h>
#include <lh/core/library/option.h>
#include <lh/expect/death.h>
#include <lh/core/runtime/logger.h>
#include <lh/core/str/format/text.h>

#include <cstdarg>
#include <string>
#include <thread>

namespace
{

struct capture
{
    std::string text;
    lh_logger_level_t level = 0;
    int calls = 0;
};

lh_ssize_t
capture_emit(lh_self_ptr self, lh_logger_level_t level, lh_str_cptr fmt, va_list args)
{
    capture *c = static_cast<capture *>(self);
    char line[256];
    const lh_usize_t n = lh_str_ptr_format_text_v(line, sizeof(line), fmt, args);

    c->text.assign(line, n);
    c->level = level;
    ++c->calls;
    return (lh_ssize_t)n;
}

/* Puts the runtime logger back however a test leaves it. */
struct keep_runtime_logger
{
    lh_logger_t saved;

    keep_runtime_logger()
    {
        lh_logger_assign(&saved, lh_runtime_logger_get());
    }

    ~keep_runtime_logger()
    {
        lh_logger_assign(lh_runtime_logger_get_mut(), &saved);
    }
};

} // namespace

TEST(runtime_logger, is_one_instance)
{
    ASSERT_NE(lh_runtime_logger_get_mut(), nullptr);
    EXPECT_EQ(lh_runtime_logger_get_mut(), lh_runtime_logger_get_mut());
}

/* The core starts empty; the runner (test/src/main.cpp) installed its logger. */
TEST(runtime_logger, the_runner_installed_its_logger)
{
    EXPECT_EQ(lh_logger_get_flags(lh_runtime_logger_get()),
              (lh_logger_level_flags_t)lh_logger_level_flags_all);
    EXPECT_EQ(lh_logger_get_flags(lh_runtime_logger_get()),
              (lh_logger_level_flags_t)lh_logger_level_flags_all);
    for (lh_logger_level_t level = lh_logger_level_emergency; level <= lh_logger_level_debug;
         ++level)
    {
        EXPECT_NE(lh_logger_get_emit_cb(lh_runtime_logger_get(), level), nullptr)
            << "level " << (int)level;
    }
}

TEST(runtime_logger, an_installed_sink_gets_the_records)
{
    keep_runtime_logger keep;
    capture c;

    lh_logger_init(lh_runtime_logger_get_mut(), lh_logger_level_flags_all, capture_emit, &c);
    lh_logger_info(lh_runtime_logger_get_mut(), "listening on %u", (lh_uint_t)8765u);
    EXPECT_EQ(c.calls, 1);
    EXPECT_EQ(c.level, (lh_logger_level_t)lh_logger_level_info);
    EXPECT_EQ(c.text, "listening on 8765");

    lh_logger_init(lh_runtime_logger_get_mut(), lh_logger_level_flags_error, capture_emit, &c);
    lh_logger_info(lh_runtime_logger_get_mut(), "filtered");
    EXPECT_EQ(c.calls, 1);
}

namespace
{

/* The info slot a fresh thread finds in its own logger. */
lh_logger_emit_cb
info_cb_on_new_thread(void)
{
    lh_logger_emit_cb seen = lh_null;
    std::thread(
        [&] { seen = lh_logger_get_emit_cb(lh_runtime_logger_get_mut(), lh_logger_level_info); })
        .join();
    return seen;
}

} // namespace

/* One object for the whole program: a change on one thread is seen on all. */
TEST(runtime_logger, is_one_slot_for_every_thread)
{
    keep_runtime_logger keep;
    capture c;

    lh_logger_init(lh_runtime_logger_get_mut(), lh_logger_level_flags_all, capture_emit, &c);
    EXPECT_EQ(info_cb_on_new_thread(), capture_emit);
}
