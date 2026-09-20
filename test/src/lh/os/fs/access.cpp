#include <cstdio>

#include <gtest/gtest.h>

#include <lh/os.h>
#include <lh/os/error/code.h>
#include <lh/os/fs/access.h>
#include <lh/os/fs/file.h>
#include <lh/str/view.h>
#include <lh/util/addr.h>

namespace
{

TEST(os_fs_access, empty_path_fails)
{
    lh_os_fs_path_t path;

    lh_os_fs_path_init(lh_addr_of(path));
    EXPECT_EQ(lh_os_fs_can(lh_addr_of(path), lh_os_fs_access_readable), lh_bool_false);
    EXPECT_EQ(lh_os_get_last_error_code(), lh_os_error_code_path_empty);
    lh_os_fs_path_deinit(lh_addr_of(path));
}

TEST(os_fs_access, missing_not_readable)
{
    lh_os_fs_path_t path;

    lh_os_fs_path_init(lh_addr_of(path));
    ASSERT_EQ(lh_os_fs_path_set(lh_addr_of(path),
                                lh_str_view_make("lh_os_fs_access_missing_no_such")),
              lh_bool_true);
    EXPECT_EQ(lh_os_fs_can(lh_addr_of(path), lh_os_fs_access_readable), lh_bool_false);
    EXPECT_NE(lh_os_get_last_error_code(), 0);
    lh_os_fs_path_deinit(lh_addr_of(path));
}

TEST(os_fs_access, written_file_can_read_and_write)
{
    lh_os_fs_path_t path;
    lh_os_fs_file_t file;

    lh_os_fs_path_init(lh_addr_of(path));
    ASSERT_EQ(lh_os_fs_path_set(lh_addr_of(path), lh_str_view_make("lh_os_fs_access.bin")),
              lh_bool_true);
    lh_os_fs_file_init(lh_addr_of(file));
    ASSERT_EQ(lh_os_fs_file_open(lh_addr_of(file), lh_addr_of(path), lh_os_fs_file_mode_write),
              lh_bool_true);
    EXPECT_EQ(lh_os_fs_file_get_mode(lh_addr_of(file)), lh_os_fs_file_mode_write);
    lh_os_fs_file_deinit(lh_addr_of(file));

    EXPECT_EQ(lh_os_fs_can(lh_addr_of(path), lh_os_fs_access_readable), lh_bool_true);
    EXPECT_EQ(lh_os_fs_can(lh_addr_of(path), lh_os_fs_access_writable), lh_bool_true);

    lh_os_fs_path_deinit(lh_addr_of(path));
    (void)std::remove("lh_os_fs_access.bin");
}

TEST(os_fs_file, get_mode_none_when_closed)
{
    lh_os_fs_file_t file;

    lh_os_fs_file_init(lh_addr_of(file));
    EXPECT_EQ(lh_os_fs_file_get_mode(lh_addr_of(file)), lh_os_fs_file_mode_none);
    lh_os_fs_file_deinit(lh_addr_of(file));
}

} // namespace
