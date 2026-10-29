#include <gtest/gtest.h>

#include <lh/os/fs/dir.h>
#include <lh/os/logger/daily.h>
#include <lh/os/logger/queue.h>
#include <lh/os/timestamp.h>

#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>

namespace
{

const char *const k_dir = "lh-test-logger-daily";

/* 2025-06-15 10:00 UTC and the next day. */
constexpr lh_timestamp_t g_first = 1749981600;
constexpr lh_timestamp_t g_second = g_first + 86400;

std::string
day_file(lh_timestamp_t utc)
{
    lh_timestamp_t local = utc;
    char name[64];

    EXPECT_TRUE(lh_os_timestamp_try_get_local(utc, &local));
    std::snprintf(name, sizeof(name), "%s/%04d-%02d-%02d.log", k_dir,
                  static_cast<int>(lh_timestamp_get_year(local)),
                  static_cast<int>(lh_timestamp_get_month(local)),
                  static_cast<int>(lh_timestamp_get_day(local)));
    return name;
}

std::string
read_file(const std::string &path)
{
    std::ifstream in(path, std::ios::binary);
    std::stringstream text;

    text << in.rdbuf();
    return text.str();
}

void
touch(const std::string &path)
{
    std::ofstream(path, std::ios::binary) << "old\n";
}

bool
exists(const std::string &path)
{
    return std::ifstream(path).good();
}

/* A queue over a daily sink, torn down in order. */
struct logger
{
    lh_os_logger_daily_t *daily = nullptr;
    lh_os_logger_queue_t *queue = nullptr;

    explicit logger(lh_os_logger_daily_format_cb format = nullptr)
    {
        lh_os_logger_sink_t sink{};

        daily = lh_os_logger_daily_try_create(format, nullptr, nullptr);
        lh_os_logger_daily_copy_sink(daily, &sink);
        queue = lh_os_logger_queue_try_create(64U, 1000U, &sink);
    }

    ~logger()
    {
        lh_os_logger_queue_destroy(queue);
        lh_os_logger_daily_destroy(daily);
    }

    void
    open(lh_s32_t keep_days, lh_bool_t raw)
    {
        lh_os_logger_record_t record{};

        lh_timestamp_t now = 0;

        ASSERT_TRUE(lh_os_timestamp_try_get_now(&now));
        lh_os_logger_record_init_open(&record, 7U, k_dir, keep_days, raw, now);
        ASSERT_TRUE(lh_os_logger_queue_push(queue, &record));
    }

    void
    write(lh_timestamp_t utc, const char *tag, const char *text)
    {
        lh_os_logger_record_t record{};

        lh_os_logger_record_init_line(&record, 7U, lh_logger_level_warning, utc, tag, text);
        ASSERT_TRUE(lh_os_logger_queue_push(queue, &record));
    }

    void
    sync()
    {
        ASSERT_TRUE(lh_os_logger_queue_sync(queue, 5000U));
    }
};

void
clean()
{
    std::remove(day_file(g_first).c_str());
    std::remove(day_file(g_second).c_str());
    std::remove((std::string(k_dir) + "/2000-01-01.log").c_str());
    std::remove((std::string(k_dir) + "/notes.txt").c_str());
}

lh_void
shout(lh_ptr, const lh_os_logger_record_t *record, lh_timestamp_t, lh_str_t *out)
{
    lh_os_logger_daily_append_text(out, "!");
    lh_os_logger_daily_append_text(out, lh_os_logger_record_get_text(record));
}

} // namespace

TEST(os_logger_daily, lines_go_to_the_file_of_their_local_day)
{
    clean();
    {
        logger log;

        ASSERT_NE(log.queue, nullptr);
        log.open(0, lh_bool_false);
        log.write(g_first, "core", "first");
        log.write(g_first, nullptr, "second\n");
        log.write(g_second, nullptr, "next day");
        log.sync();
    }
    const std::string first = read_file(day_file(g_first));
    const std::string second = read_file(day_file(g_second));
    lh_timestamp_t local = g_first;
    char head[32];

    ASSERT_TRUE(lh_os_timestamp_try_get_local(g_first, &local));
    std::snprintf(head, sizeof(head), "%04d-%02d-%02d %02d:%02d:%02d",
                  static_cast<int>(lh_timestamp_get_year(local)),
                  static_cast<int>(lh_timestamp_get_month(local)),
                  static_cast<int>(lh_timestamp_get_day(local)),
                  static_cast<int>(lh_timestamp_get_hour(local)),
                  static_cast<int>(lh_timestamp_get_minute(local)),
                  static_cast<int>(lh_timestamp_get_second(local)));
    EXPECT_EQ(first,
              std::string(head) + " [warning] [core] first\n" + head + " [warning] second\n");
    EXPECT_NE(second.find("[warning] next day\n"), std::string::npos);
    clean();
}

TEST(os_logger_daily, a_reopened_file_is_appended_to)
{
    clean();
    {
        logger log;

        log.open(0, lh_bool_true);
        log.write(g_first, nullptr, "one");
        log.sync();
    }
    {
        logger log;

        log.open(0, lh_bool_true);
        log.write(g_first, nullptr, "two");
        log.sync();
    }
    EXPECT_EQ(read_file(day_file(g_first)), "one\ntwo\n");
    clean();
}

TEST(os_logger_daily, a_custom_format_writes_the_line)
{
    clean();
    {
        logger log(shout);

        log.open(0, lh_bool_false);
        log.write(g_first, nullptr, "hey");
        log.sync();
    }
    EXPECT_EQ(read_file(day_file(g_first)), "!\nhey\n");
    clean();
}

TEST(os_logger_daily, opening_a_channel_deletes_old_day_files_only)
{
    lh_fs_path_t dir;
    lh_str_view_t view;

    lh_fs_path_init(&dir);
    lh_str_view_init(&view, k_dir);
    lh_fs_path_set(&dir, view, lh_fs_path_style_posix);
    ASSERT_TRUE(lh_os_fs_dir_create_tree(&dir));
    lh_fs_path_deinit(&dir);
    clean();
    touch(std::string(k_dir) + "/2000-01-01.log");
    touch(std::string(k_dir) + "/notes.txt");
    {
        logger log;

        log.open(3, lh_bool_false);
        log.sync();
    }
    EXPECT_FALSE(exists(std::string(k_dir) + "/2000-01-01.log"));
    EXPECT_TRUE(exists(std::string(k_dir) + "/notes.txt"));
    clean();
}

TEST(os_logger_daily, a_line_of_an_unknown_channel_is_dropped)
{
    clean();
    {
        logger log;

        log.write(g_first, nullptr, "nobody");
        log.sync();
    }
    EXPECT_FALSE(exists(day_file(g_first)));
}
