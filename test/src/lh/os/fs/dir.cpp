#include <gtest/gtest.h>

#include <lh/os.h>
#include <lh/os/error/code.h>
#include <lh/os/fs/dir.h>
#include <lh/str/view.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

namespace
{

TEST(os_fs_dir, init_leaves_handle_invalid)
{
    lh_os_fs_dir_t dir;

    lh_os_fs_dir_init(lh_addr_of(dir));
    EXPECT_EQ(lh_os_fs_dir_is_valid(lh_addr_of(dir)), lh_bool_false);
    EXPECT_EQ(lh_ptr_deref(lh_os_fs_dir_get_handle_as_const(lh_addr_of(dir))),
              LH_OS_FS_FILE_HANDLE_INVALID);
    lh_os_fs_dir_deinit(lh_addr_of(dir));
}

TEST(os_fs_dir, close_keeps_path)
{
    lh_os_fs_dir_t dir;
    lh_os_fs_path_t path;

    lh_os_fs_dir_init(lh_addr_of(dir));
    lh_os_fs_path_init(lh_addr_of(path));
    ASSERT_EQ(lh_os_fs_path_set(lh_addr_of(path), lh_str_view_make("a/b")), lh_bool_true);
    lh_os_fs_dir_set_path(lh_addr_of(dir), lh_addr_of(path));
    lh_os_fs_dir_close(lh_addr_of(dir));
    EXPECT_EQ(lh_os_fs_dir_is_valid(lh_addr_of(dir)), lh_bool_false);
    EXPECT_EQ(lh_os_fs_path_get_part_count(lh_os_fs_dir_get_path_as_const(lh_addr_of(dir))), 2U);
    lh_os_fs_path_deinit(lh_addr_of(path));
    lh_os_fs_dir_deinit(lh_addr_of(dir));
}

TEST(os_fs_dir, read_without_open_fails)
{
    lh_os_fs_dir_t dir;
    lh_os_fs_path_t name;

    lh_os_fs_dir_init(lh_addr_of(dir));
    lh_os_fs_path_init(lh_addr_of(name));
    EXPECT_LT(lh_os_fs_dir_read(lh_addr_of(dir), lh_addr_of(name)), 0);
    EXPECT_EQ(lh_os_get_last_error_code(), lh_os_error_code_not_open);
    lh_os_fs_path_deinit(lh_addr_of(name));
    lh_os_fs_dir_deinit(lh_addr_of(dir));
}

} // namespace
