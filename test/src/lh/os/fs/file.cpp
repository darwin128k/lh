#include <gtest/gtest.h>

#include <lh/char.h>
#include <lh/memory.h>
#include <lh/os.h>
#include <lh/os/error/code.h>
#include <lh/os/fs.h>
#include <lh/str/view.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

namespace
{

static bool
fill_test_path(lh_os_fs_path_t *out)
{
    lh_os_fs_path_t dir;
    lh_os_fs_path_t name;

    lh_os_fs_path_init(lh_addr_of(dir));
    lh_os_fs_path_init(out);
    lh_os_fs_path_init(lh_addr_of(name));
    if (lh_os_fs_path_set(lh_addr_of(name), lh_str_view_make("lh_os_fs_test.bin")) != lh_bool_true)
    {
        return false;
    }
    if (lh_os_fs_path_exe_dir(lh_addr_of(dir)) != lh_bool_true)
    {
        return false;
    }
    return lh_os_fs_path_join(out, lh_addr_of(dir), lh_addr_of(name)) == lh_bool_true;
}

TEST(os_fs_file, init_leaves_handle_invalid)
{
    lh_os_fs_file_t file;

    lh_os_fs_file_init(&file);
    EXPECT_EQ(lh_os_fs_file_is_valid(&file), lh_bool_false);
    EXPECT_EQ(lh_os_fs_file_get_handle(&file), LH_OS_FS_FILE_HANDLE_INVALID);
    EXPECT_EQ(lh_os_fs_path_is_empty(lh_os_fs_file_get_path_as_const(&file)), lh_bool_true);
    lh_os_fs_file_deinit(&file);
}

TEST(os_fs_file, open_missing_read_fails)
{
    lh_os_fs_file_t file;
    lh_os_fs_path_t path;

    lh_os_fs_path_init(lh_addr_of(path));
    ASSERT_EQ(lh_os_fs_path_set(lh_addr_of(path), lh_str_view_make("lh_os_fs_file_missing_no_such_file")),
              lh_bool_true);
    lh_os_fs_file_init(&file);
    EXPECT_EQ(lh_os_fs_file_open(&file, lh_addr_of(path), lh_os_fs_file_mode_read), lh_bool_false);
    EXPECT_EQ(lh_os_fs_file_is_valid(&file), lh_bool_false);
    EXPECT_NE(lh_os_get_last_error_code(), 0);
    lh_os_fs_file_deinit(&file);
}

TEST(os_fs_file, write_then_read_round_trips_bytes_size_and_mtime)
{
    lh_os_fs_path_t path;
    char payload[] = "lh-fs";
    char buf[16];
    lh_os_fs_file_t file;
    lh_u64_t size = 0;
    lh_s64_t mtime = 0;
    lh_ssize_t n;

    ASSERT_TRUE(fill_test_path(lh_addr_of(path)));
    (void)lh_os_fs_remove(lh_addr_of(path));

    lh_os_fs_file_init(&file);
    ASSERT_EQ(lh_os_fs_file_open(&file, lh_addr_of(path), lh_os_fs_file_mode_write), lh_bool_true);
    n = lh_os_fs_file_write(&file, payload, sizeof(payload) - 1U);
    ASSERT_EQ(n, static_cast<lh_ssize_t>(sizeof(payload) - 1U));
    ASSERT_EQ(lh_os_fs_file_get_size(&file, &size), lh_bool_true);
    EXPECT_EQ(size, static_cast<lh_u64_t>(sizeof(payload) - 1U));
    lh_os_fs_file_close(&file);
    EXPECT_EQ(lh_os_fs_file_is_valid(&file), lh_bool_false);
    EXPECT_EQ(lh_os_fs_path_is_empty(lh_os_fs_file_get_path_as_const(&file)), lh_bool_false);

    ASSERT_EQ(lh_os_fs_mtime(lh_addr_of(path), &mtime), lh_bool_true);
    EXPECT_NE(mtime, 0);

    ASSERT_EQ(lh_os_fs_file_open(&file, lh_addr_of(path), lh_os_fs_file_mode_read), lh_bool_true);
    n = lh_os_fs_file_read(&file, buf, sizeof(buf));
    ASSERT_EQ(n, static_cast<lh_ssize_t>(sizeof(payload) - 1U));
    buf[n] = '\0';
    EXPECT_STREQ(buf, payload);
    n = lh_os_fs_file_read(&file, buf, sizeof(buf));
    EXPECT_EQ(n, 0);
    lh_os_fs_file_deinit(&file);

    EXPECT_EQ(lh_os_fs_remove(lh_addr_of(path)), lh_bool_true);
}

TEST(os_fs_file, stream_write_then_read)
{
    lh_os_fs_path_t path;
    char payload[] = "stream";
    char buf[16];
    lh_os_fs_file_t file;
    lh_ssize_t n;

    ASSERT_TRUE(fill_test_path(lh_addr_of(path)));
    (void)lh_os_fs_remove(lh_addr_of(path));

    lh_os_fs_file_init(&file);
    ASSERT_EQ(lh_os_fs_file_open(&file, lh_addr_of(path), lh_os_fs_file_mode_write), lh_bool_true);
    {
        lh_io_stream_t stream = lh_os_fs_file_get_stream(&file);
        n = lh_io_stream_write(&stream, payload, sizeof(payload) - 1U);
        ASSERT_EQ(n, static_cast<lh_ssize_t>(sizeof(payload) - 1U));
    }
    lh_os_fs_file_close(&file);

    ASSERT_EQ(lh_os_fs_file_open(&file, lh_addr_of(path), lh_os_fs_file_mode_read), lh_bool_true);
    {
        lh_io_stream_t stream = lh_os_fs_file_get_stream(&file);
        n = lh_io_stream_read(&stream, buf, sizeof(buf));
        ASSERT_EQ(n, static_cast<lh_ssize_t>(sizeof(payload) - 1U));
        buf[n] = '\0';
        EXPECT_STREQ(buf, payload);
    }
    lh_os_fs_file_deinit(&file);
    EXPECT_EQ(lh_os_fs_remove(lh_addr_of(path)), lh_bool_true);
}

TEST(os_fs_file, seek_set_cur_end)
{
    lh_os_fs_path_t path;
    char payload[] = "ABCDEF";
    char buf[8];
    lh_os_fs_file_t file;
    lh_u64_t pos = 0;
    lh_ssize_t n;

    ASSERT_TRUE(fill_test_path(lh_addr_of(path)));
    (void)lh_os_fs_remove(lh_addr_of(path));

    lh_os_fs_file_init(&file);
    ASSERT_EQ(lh_os_fs_file_open(&file, lh_addr_of(path), lh_os_fs_file_mode_write), lh_bool_true);
    ASSERT_EQ(lh_os_fs_file_write(&file, payload, sizeof(payload) - 1U),
              static_cast<lh_ssize_t>(sizeof(payload) - 1U));
    lh_os_fs_file_close(&file);

    ASSERT_EQ(lh_os_fs_file_open(&file, lh_addr_of(path), lh_os_fs_file_mode_read), lh_bool_true);
    ASSERT_EQ(lh_os_fs_file_seek(&file, 3, lh_os_fs_file_seek_set, &pos), lh_bool_true);
    EXPECT_EQ(pos, 3U);
    n = lh_os_fs_file_read(&file, buf, 3U);
    ASSERT_EQ(n, 3);
    buf[3] = '\0';
    EXPECT_STREQ(buf, "DEF");

    ASSERT_EQ(lh_os_fs_file_seek(&file, 0, lh_os_fs_file_seek_set, &pos), lh_bool_true);
    EXPECT_EQ(pos, 0U);
    ASSERT_EQ(lh_os_fs_file_seek(&file, 0, lh_os_fs_file_seek_end, &pos), lh_bool_true);
    EXPECT_EQ(pos, static_cast<lh_u64_t>(sizeof(payload) - 1U));
    ASSERT_EQ(lh_os_fs_file_seek(&file, -1, lh_os_fs_file_seek_end, &pos), lh_bool_true);
    n = lh_os_fs_file_read(&file, buf, 1U);
    ASSERT_EQ(n, 1);
    EXPECT_EQ(buf[0], 'F');
    lh_os_fs_file_deinit(&file);

    EXPECT_EQ(lh_os_fs_remove(lh_addr_of(path)), lh_bool_true);
}

TEST(os_fs_file, seek_on_closed_fails_not_open)
{
    lh_os_fs_file_t file;
    lh_u64_t pos = 1;

    lh_os_fs_file_init(&file);
    EXPECT_EQ(lh_os_fs_file_seek(&file, 0, lh_os_fs_file_seek_set, &pos), lh_bool_false);
    EXPECT_EQ(lh_os_get_last_error_code(), lh_os_error_code_not_open);
    lh_os_fs_file_deinit(&file);
}

TEST(os_fs_file, map_reads_written_bytes)
{
    lh_os_fs_path_t path;
    char payload[] = "map-bytes";
    lh_os_fs_file_t file;
    lh_os_fs_file_map_t map;

    ASSERT_TRUE(fill_test_path(lh_addr_of(path)));
    (void)lh_os_fs_remove(lh_addr_of(path));

    lh_os_fs_file_init(&file);
    ASSERT_EQ(lh_os_fs_file_open(&file, lh_addr_of(path), lh_os_fs_file_mode_write), lh_bool_true);
    ASSERT_EQ(lh_os_fs_file_write(&file, payload, sizeof(payload) - 1U),
              static_cast<lh_ssize_t>(sizeof(payload) - 1U));
    lh_os_fs_file_close(&file);

    ASSERT_EQ(lh_os_fs_file_open(&file, lh_addr_of(path), lh_os_fs_file_mode_read), lh_bool_true);
    lh_os_fs_file_map_init(lh_addr_of(map));
    ASSERT_EQ(lh_os_fs_file_map(&file, 0, 0, lh_os_fs_file_map_prot_read, lh_addr_of(map)),
              lh_bool_true);
    EXPECT_EQ(lh_os_fs_file_map_is_valid(lh_addr_of(map)), lh_bool_true);
    EXPECT_EQ(lh_os_fs_file_map_get_size(lh_addr_of(map)), sizeof(payload) - 1U);
    EXPECT_EQ(lh_memory_compare(lh_os_fs_file_map_get_data(lh_addr_of(map)), sizeof(payload) - 1U,
                                payload, sizeof(payload) - 1U),
              static_cast<void *>(lh_null));
    lh_os_fs_file_map_unmap(lh_addr_of(map));
    EXPECT_EQ(lh_os_fs_file_map_is_valid(lh_addr_of(map)), lh_bool_false);
    lh_os_fs_file_deinit(&file);

    EXPECT_EQ(lh_os_fs_remove(lh_addr_of(path)), lh_bool_true);
}

TEST(os_fs_file, map_offset_and_write_through)
{
    lh_os_fs_path_t path;
    char payload[] = "ABCDEF";
    char buf[8];
    lh_os_fs_file_t file;
    lh_os_fs_file_map_t map;
    lh_ptr data;
    lh_ssize_t n;

    ASSERT_TRUE(fill_test_path(lh_addr_of(path)));
    (void)lh_os_fs_remove(lh_addr_of(path));

    lh_os_fs_file_init(&file);
    ASSERT_EQ(lh_os_fs_file_open(&file, lh_addr_of(path), lh_os_fs_file_mode_write), lh_bool_true);
    ASSERT_EQ(lh_os_fs_file_write(&file, payload, sizeof(payload) - 1U),
              static_cast<lh_ssize_t>(sizeof(payload) - 1U));
    lh_os_fs_file_close(&file);

    ASSERT_EQ(lh_os_fs_file_open(&file, lh_addr_of(path), lh_os_fs_file_mode_readwrite),
              lh_bool_true);
    lh_os_fs_file_map_init(lh_addr_of(map));
    ASSERT_EQ(lh_os_fs_file_map(&file, 3, 3, lh_os_fs_file_map_prot_write, lh_addr_of(map)),
              lh_bool_true);
    EXPECT_EQ(lh_os_fs_file_map_get_size(lh_addr_of(map)), 3U);
    EXPECT_EQ(lh_memory_compare(lh_os_fs_file_map_get_data(lh_addr_of(map)), 3U, "DEF", 3U),
              static_cast<void *>(lh_null));
    data = lh_os_fs_file_map_get_data(lh_addr_of(map));
    lh_ptr_deref(lh_ptr_cast(lh_char_t, data)) = 'X';
    lh_os_fs_file_map_unmap(lh_addr_of(map));
    lh_os_fs_file_close(&file);

    ASSERT_EQ(lh_os_fs_file_open(&file, lh_addr_of(path), lh_os_fs_file_mode_read), lh_bool_true);
    n = lh_os_fs_file_read(&file, buf, sizeof(buf));
    ASSERT_EQ(n, static_cast<lh_ssize_t>(sizeof(payload) - 1U));
    buf[n] = '\0';
    EXPECT_STREQ(buf, "ABCXEF");
    lh_os_fs_file_deinit(&file);

    EXPECT_EQ(lh_os_fs_remove(lh_addr_of(path)), lh_bool_true);
}

} // namespace
