#include <cstdio>

#include <gtest/gtest.h>

#include <lh/io/reader.h>
#include <lh/os.h>
#include <lh/os/error/code.h>
#include <lh/os/fs/file.h>
#include <lh/str/view.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

namespace
{

TEST(os_fs_file, init_leaves_handle_invalid)
{
    lh_os_fs_file_t file;

    lh_os_fs_file_init(lh_addr_of(file));
    EXPECT_EQ(lh_os_fs_file_is_valid(lh_addr_of(file)), lh_bool_false);
    EXPECT_EQ(lh_ptr_deref(lh_os_fs_file_get_handle_as_const(lh_addr_of(file))),
              LH_OS_FS_FILE_HANDLE_INVALID);
    EXPECT_EQ(lh_os_fs_path_is_empty(lh_os_fs_file_get_path_as_const(lh_addr_of(file))),
              lh_bool_true);
    lh_os_fs_file_deinit(lh_addr_of(file));
}

TEST(os_fs_file, close_keeps_path)
{
    lh_os_fs_file_t file;
    lh_os_fs_path_t path;

    lh_os_fs_file_init(lh_addr_of(file));
    lh_os_fs_path_init(lh_addr_of(path));
    ASSERT_EQ(lh_os_fs_path_set(lh_addr_of(path), lh_str_view_make("a/b")), lh_bool_true);
    lh_os_fs_file_set_path(lh_addr_of(file), lh_addr_of(path));
    lh_os_fs_file_close(lh_addr_of(file));
    EXPECT_EQ(lh_os_fs_file_is_valid(lh_addr_of(file)), lh_bool_false);
    EXPECT_EQ(lh_os_fs_path_get_part_count(lh_os_fs_file_get_path_as_const(lh_addr_of(file))), 2U);
    lh_os_fs_path_deinit(lh_addr_of(path));
    lh_os_fs_file_deinit(lh_addr_of(file));
}

TEST(os_fs_file, open_empty_path_fails)
{
    lh_os_fs_file_t file;
    lh_os_fs_path_t path;

    lh_os_fs_file_init(lh_addr_of(file));
    lh_os_fs_path_init(lh_addr_of(path));
    EXPECT_EQ(lh_os_fs_file_open(lh_addr_of(file), lh_addr_of(path), lh_os_fs_file_mode_read),
              lh_bool_false);
    EXPECT_EQ(lh_os_get_last_error_code(), lh_os_error_code_path_empty);
    EXPECT_EQ(lh_os_fs_file_is_valid(lh_addr_of(file)), lh_bool_false);
    lh_os_fs_path_deinit(lh_addr_of(path));
    lh_os_fs_file_deinit(lh_addr_of(file));
}

TEST(os_fs_file, open_missing_read_fails)
{
    lh_os_fs_file_t file;
    lh_os_fs_path_t path;

    lh_os_fs_path_init(lh_addr_of(path));
    ASSERT_EQ(lh_os_fs_path_set(lh_addr_of(path),
                                lh_str_view_make("lh_os_fs_file_missing_no_such_file")),
              lh_bool_true);
    lh_os_fs_file_init(lh_addr_of(file));
    EXPECT_EQ(lh_os_fs_file_open(lh_addr_of(file), lh_addr_of(path), lh_os_fs_file_mode_read),
              lh_bool_false);
    EXPECT_EQ(lh_os_fs_file_is_valid(lh_addr_of(file)), lh_bool_false);
    EXPECT_NE(lh_os_get_last_error_code(), 0);
    lh_os_fs_path_deinit(lh_addr_of(path));
    lh_os_fs_file_deinit(lh_addr_of(file));
}

TEST(os_fs_file, open_write_then_read_empty)
{
    lh_os_fs_file_t file;
    lh_os_fs_path_t path;
    char buf[4];
    lh_ssize_t n;

    lh_os_fs_path_init(lh_addr_of(path));
    ASSERT_EQ(lh_os_fs_path_set(lh_addr_of(path), lh_str_view_make("lh_os_fs_file_open_test.bin")),
              lh_bool_true);
    lh_os_fs_file_init(lh_addr_of(file));
    ASSERT_EQ(lh_os_fs_file_open(lh_addr_of(file), lh_addr_of(path), lh_os_fs_file_mode_write),
              lh_bool_true);
    EXPECT_EQ(lh_os_fs_file_is_valid(lh_addr_of(file)), lh_bool_true);
    EXPECT_EQ(lh_os_fs_path_get_part_count(lh_os_fs_file_get_path_as_const(lh_addr_of(file))), 1U);
    lh_os_fs_file_close(lh_addr_of(file));
    EXPECT_EQ(lh_os_fs_file_is_valid(lh_addr_of(file)), lh_bool_false);
    EXPECT_EQ(lh_os_fs_path_is_empty(lh_os_fs_file_get_path_as_const(lh_addr_of(file))),
              lh_bool_false);

    ASSERT_EQ(lh_os_fs_file_open(lh_addr_of(file), lh_addr_of(path), lh_os_fs_file_mode_read),
              lh_bool_true);
    n = lh_os_fs_file_read(lh_addr_of(file), buf, sizeof(buf));
    EXPECT_EQ(n, 0);
    lh_os_fs_file_deinit(lh_addr_of(file));
    lh_os_fs_path_deinit(lh_addr_of(path));
    (void)std::remove("lh_os_fs_file_open_test.bin");
}

TEST(os_fs_file, read_without_open_fails)
{
    lh_os_fs_file_t file;
    char buf[4];

    lh_os_fs_file_init(lh_addr_of(file));
    EXPECT_EQ(lh_os_fs_file_read(lh_addr_of(file), buf, sizeof(buf)),
              static_cast<lh_ssize_t>(-1));
    EXPECT_EQ(lh_os_get_last_error_code(), lh_os_error_code_not_open);
    EXPECT_EQ(lh_os_fs_file_read(lh_addr_of(file), buf, 0U), 0);
    lh_os_fs_file_deinit(lh_addr_of(file));
}

TEST(os_fs_file, get_reader_plugs_file_read)
{
    lh_os_fs_file_t file;
    lh_io_reader_t reader;
    char buf[4];

    lh_os_fs_file_init(lh_addr_of(file));
    reader = lh_os_fs_file_get_reader(lh_addr_of(file));
    EXPECT_EQ(lh_io_reader_get_context(lh_addr_of(reader)),
              static_cast<lh_ptr>(lh_addr_of(file)));
    EXPECT_EQ(lh_io_reader_read(lh_addr_of(reader), buf, sizeof(buf)),
              static_cast<lh_ssize_t>(-1));
    EXPECT_EQ(lh_os_get_last_error_code(), lh_os_error_code_not_open);
    lh_os_fs_file_deinit(lh_addr_of(file));
}

} // namespace
