#include <cstdio>

#include <gtest/gtest.h>

#include <lh/null.h>
#include <lh/os.h>
#include <lh/os/error/code.h>
#include <lh/os/fs/dir.h>
#include <lh/os/fs/file.h>
#include <lh/str.h>
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
    EXPECT_LT(lh_os_fs_dir_read(lh_addr_of(dir), lh_addr_of(name),
                                static_cast<lh_os_fs_dir_entry_kind_t *>(lh_null)),
              0);
    EXPECT_EQ(lh_os_get_last_error_code(), lh_os_error_code_not_open);
    lh_os_fs_path_deinit(lh_addr_of(name));
    lh_os_fs_dir_deinit(lh_addr_of(dir));
}

TEST(os_fs_dir, open_empty_path_fails)
{
    lh_os_fs_dir_t dir;
    lh_os_fs_path_t path;

    lh_os_fs_dir_init(lh_addr_of(dir));
    lh_os_fs_path_init(lh_addr_of(path));
    EXPECT_EQ(lh_os_fs_dir_open(lh_addr_of(dir), lh_addr_of(path)), lh_bool_false);
    EXPECT_EQ(lh_os_get_last_error_code(), lh_os_error_code_path_empty);
    lh_os_fs_path_deinit(lh_addr_of(path));
    lh_os_fs_dir_deinit(lh_addr_of(dir));
}

TEST(os_fs_dir, lists_created_file)
{
    lh_os_fs_path_t cwd;
    lh_os_fs_path_t file_path;
    lh_os_fs_file_t file;
    lh_os_fs_dir_t dir;
    lh_os_fs_path_t name;
    lh_os_fs_dir_entry_kind_t kind = lh_os_fs_dir_entry_kind_other;
    lh_ssize_t n;
    bool found = false;

    lh_os_fs_path_init(lh_addr_of(file_path));
    ASSERT_EQ(lh_os_fs_path_set(lh_addr_of(file_path), lh_str_view_make("lh_os_fs_dir_list.bin")),
              lh_bool_true);
    lh_os_fs_file_init(lh_addr_of(file));
    ASSERT_EQ(lh_os_fs_file_open(lh_addr_of(file), lh_addr_of(file_path), lh_os_fs_file_mode_write),
              lh_bool_true);
    lh_os_fs_file_deinit(lh_addr_of(file));

    lh_os_fs_path_init(lh_addr_of(cwd));
    ASSERT_EQ(lh_os_fs_path_set(lh_addr_of(cwd), lh_str_view_make(".")), lh_bool_true);
    lh_os_fs_dir_init(lh_addr_of(dir));
    ASSERT_EQ(lh_os_fs_dir_open(lh_addr_of(dir), lh_addr_of(cwd)), lh_bool_true);

    lh_os_fs_path_init(lh_addr_of(name));
    for (;;)
    {
        lh_str_view_t got;
        lh_str_view_t want;

        n = lh_os_fs_dir_read(lh_addr_of(dir), lh_addr_of(name), lh_addr_of(kind));
        ASSERT_GE(n, 0);
        if (n == 0)
        {
            break;
        }
        got = lh_os_fs_path_as_view(lh_addr_of(name));
        want = lh_str_view_make("lh_os_fs_dir_list.bin");
        if (lh_str_view_compare(lh_addr_of(got), lh_addr_of(want), lh_bool_false) == 0)
        {
            found = true;
            EXPECT_EQ(kind, lh_os_fs_dir_entry_kind_file);
            break;
        }
    }
    EXPECT_TRUE(found);

    lh_os_fs_path_deinit(lh_addr_of(name));
    lh_os_fs_dir_deinit(lh_addr_of(dir));
    lh_os_fs_path_deinit(lh_addr_of(cwd));
    lh_os_fs_path_deinit(lh_addr_of(file_path));
    (void)std::remove("lh_os_fs_dir_list.bin");
}

} // namespace
