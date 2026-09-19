#include <gtest/gtest.h>

#include <lh/os.h>
#include <lh/os/fs/file.h>
#include <lh/os/fs/path.h>

namespace
{

static bool
fill_test_path(char *out, lh_usize_t out_size)
{
    char dir[4096];

    if (lh_os_fs_path_exe_dir(dir, sizeof(dir)) != lh_bool_true)
    {
        return false;
    }
    return lh_os_fs_path_join(out, out_size, dir, "lh_os_fs_test.bin") == lh_bool_true;
}

TEST(os_fs_file, init_leaves_handle_invalid)
{
    lh_os_fs_file_t file;

    lh_os_fs_file_init(&file);
    EXPECT_EQ(lh_os_fs_file_is_valid(&file), lh_bool_false);
    EXPECT_EQ(lh_os_fs_file_get_handle(&file), LH_OS_FS_FILE_HANDLE_INVALID);
}

TEST(os_fs_file, open_missing_read_fails)
{
    lh_os_fs_file_t file;

    lh_os_fs_file_init(&file);
    EXPECT_EQ(lh_os_fs_file_open(&file, "lh_os_fs_file_missing_no_such_file",
                                 lh_os_fs_file_mode_read),
              lh_bool_false);
    EXPECT_EQ(lh_os_fs_file_is_valid(&file), lh_bool_false);
    EXPECT_NE(lh_os_get_last_error_code(), 0);
}

TEST(os_fs_file, write_then_read_round_trips_bytes_size_and_mtime)
{
    char path[4096];
    char payload[] = "lh-fs";
    char buf[16];
    lh_os_fs_file_t file;
    lh_u64_t size = 0;
    lh_s64_t mtime = 0;
    lh_ssize_t n;

    ASSERT_TRUE(fill_test_path(path, sizeof(path)));
    (void)lh_os_fs_path_remove(path);

    lh_os_fs_file_init(&file);
    ASSERT_EQ(lh_os_fs_file_open(&file, path, lh_os_fs_file_mode_write), lh_bool_true);
    n = lh_os_fs_file_write(&file, payload, sizeof(payload) - 1U);
    ASSERT_EQ(n, static_cast<lh_ssize_t>(sizeof(payload) - 1U));
    ASSERT_EQ(lh_os_fs_file_get_size(&file, &size), lh_bool_true);
    EXPECT_EQ(size, static_cast<lh_u64_t>(sizeof(payload) - 1U));
    lh_os_fs_file_close(&file);
    EXPECT_EQ(lh_os_fs_file_is_valid(&file), lh_bool_false);

    ASSERT_EQ(lh_os_fs_path_mtime(path, &mtime), lh_bool_true);
    EXPECT_NE(mtime, 0);

    lh_os_fs_file_init(&file);
    ASSERT_EQ(lh_os_fs_file_open(&file, path, lh_os_fs_file_mode_read), lh_bool_true);
    n = lh_os_fs_file_read(&file, buf, sizeof(buf));
    ASSERT_EQ(n, static_cast<lh_ssize_t>(sizeof(payload) - 1U));
    buf[n] = '\0';
    EXPECT_STREQ(buf, payload);
    n = lh_os_fs_file_read(&file, buf, sizeof(buf));
    EXPECT_EQ(n, 0);
    lh_os_fs_file_close(&file);

    EXPECT_EQ(lh_os_fs_path_remove(path), lh_bool_true);
}

TEST(os_fs_file, stream_write_then_read)
{
    char path[4096];
    char payload[] = "stream";
    char buf[16];
    lh_os_fs_file_t file;
    lh_ssize_t n;

    ASSERT_TRUE(fill_test_path(path, sizeof(path)));
    (void)lh_os_fs_path_remove(path);

    lh_os_fs_file_init(&file);
    ASSERT_EQ(lh_os_fs_file_open(&file, path, lh_os_fs_file_mode_write), lh_bool_true);
    {
        lh_io_stream_t stream = lh_os_fs_file_get_stream(&file);
        n = lh_io_stream_write(&stream, payload, sizeof(payload) - 1U);
        ASSERT_EQ(n, static_cast<lh_ssize_t>(sizeof(payload) - 1U));
    }
    lh_os_fs_file_close(&file);

    lh_os_fs_file_init(&file);
    ASSERT_EQ(lh_os_fs_file_open(&file, path, lh_os_fs_file_mode_read), lh_bool_true);
    {
        lh_io_stream_t stream = lh_os_fs_file_get_stream(&file);
        n = lh_io_stream_read(&stream, buf, sizeof(buf));
        ASSERT_EQ(n, static_cast<lh_ssize_t>(sizeof(payload) - 1U));
        buf[n] = '\0';
        EXPECT_STREQ(buf, payload);
    }
    lh_os_fs_file_close(&file);
    EXPECT_EQ(lh_os_fs_path_remove(path), lh_bool_true);
}

} // namespace
