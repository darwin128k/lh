#include <gtest/gtest.h>

#include <cstring>

#include <lh/os.h>
#include <lh/os/fs/dir.h>
#include <lh/os/fs/path.h>
#include <lh/str.h>
#include <lh/str/view.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

namespace
{

TEST(os_fs_dir, init_leaves_handle_invalid)
{
    lh_os_fs_dir_t dir;

    lh_os_fs_dir_init(&dir);
    EXPECT_EQ(lh_os_fs_dir_is_valid(&dir), lh_bool_false);
    EXPECT_EQ(lh_os_fs_dir_get_handle(&dir), LH_OS_FS_DIR_HANDLE_INVALID);
}

TEST(os_fs_dir, open_missing_fails)
{
    lh_os_fs_dir_t dir;
    lh_os_fs_path_t path;

    lh_os_fs_path_init(lh_addr_of(path));
    ASSERT_EQ(lh_os_fs_path_set(lh_addr_of(path), lh_str_view_make("lh_os_fs_dir_missing_no_such_dir")),
              lh_bool_true);
    lh_os_fs_dir_init(&dir);
    EXPECT_EQ(lh_os_fs_dir_open(&dir, lh_addr_of(path)), lh_bool_false);
    EXPECT_EQ(lh_os_fs_dir_is_valid(&dir), lh_bool_false);
    EXPECT_NE(lh_os_get_last_error_code(), 0);
}

TEST(os_fs_dir, lists_exe_dir_and_finds_exe)
{
    lh_os_fs_path_t exe;
    lh_os_fs_path_t parent;
    lh_os_fs_path_t name;
    const lh_os_fs_path_parts_t *exe_parts;
    lh_usize_t exe_n;
    lh_str_view_t want;
    lh_os_fs_dir_t dir;
    lh_os_fs_dir_entry_kind_t kind = lh_os_fs_dir_entry_kind_other;
    lh_ssize_t n;
    bool found = false;

    lh_os_fs_path_init(lh_addr_of(exe));
    lh_os_fs_path_init(lh_addr_of(parent));
    lh_os_fs_path_init(lh_addr_of(name));
    ASSERT_EQ(lh_os_fs_path_exe(lh_addr_of(exe)), lh_bool_true);
    exe_parts = lh_os_fs_path_get_parts_as_const(lh_addr_of(exe));
    exe_n = lh_os_fs_path_parts_get_size(exe_parts);
    ASSERT_GT(exe_n, 0U);
    want = lh_os_fs_path_get_part(lh_addr_of(exe), exe_n - 1U);
    ASSERT_EQ(lh_os_fs_path_exe_dir(lh_addr_of(parent)), lh_bool_true);

    lh_os_fs_dir_init(&dir);
    ASSERT_EQ(lh_os_fs_dir_open(&dir, lh_addr_of(parent)), lh_bool_true);
    EXPECT_EQ(lh_os_fs_dir_is_valid(&dir), lh_bool_true);

    for (;;)
    {
        lh_str_view_t part;

        n = lh_os_fs_dir_read(&dir, lh_addr_of(name), &kind);
        ASSERT_GE(n, 0);
        if (n == 0)
        {
            break;
        }
        ASSERT_EQ(lh_os_fs_path_get_part_count(lh_addr_of(name)), 1U);
        part = lh_os_fs_path_get_part(lh_addr_of(name), 0U);
        EXPECT_EQ(static_cast<lh_usize_t>(n), lh_str_view_get_size(lh_addr_of(part)));
        EXPECT_STRNE(lh_str_get_data(lh_os_fs_path_get_text_as_const(lh_addr_of(name))), ".");
        EXPECT_STRNE(lh_str_get_data(lh_os_fs_path_get_text_as_const(lh_addr_of(name))), "..");
        if (lh_str_view_get_size(lh_addr_of(part)) == lh_str_view_get_size(lh_addr_of(want)) &&
            std::memcmp(lh_str_view_get_data(lh_addr_of(part)), lh_str_view_get_data(lh_addr_of(want)),
                        lh_str_view_get_size(lh_addr_of(part))) == 0)
        {
            EXPECT_EQ(kind, lh_os_fs_dir_entry_kind_file);
            found = true;
        }
    }

    lh_os_fs_dir_close(&dir);
    EXPECT_EQ(lh_os_fs_dir_is_valid(&dir), lh_bool_false);
    EXPECT_TRUE(found);
}

} // namespace
