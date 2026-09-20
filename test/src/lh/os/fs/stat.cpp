#include <cstdio>

#include <gtest/gtest.h>

#include <lh/compiler/os.h>
#include <lh/os.h>
#include <lh/os/error/code.h>
#include <lh/os/fs/file.h>
#include <lh/os/fs/stat.h>
#include <lh/str/view.h>
#include <lh/util/addr.h>

namespace
{

TEST(os_fs_stat, init_is_empty_other)
{
    lh_os_fs_stat_t st;

    lh_os_fs_stat_init(lh_addr_of(st));
    EXPECT_EQ(lh_os_fs_stat_get_kind(lh_addr_of(st)), lh_os_fs_kind_other);
    EXPECT_EQ(lh_os_fs_stat_get_perm(lh_addr_of(st)), 0);
    EXPECT_EQ(lh_os_fs_stat_get_size(lh_addr_of(st)), 0U);
    EXPECT_EQ(lh_os_fs_stat_get_atime(lh_addr_of(st)), 0);
    EXPECT_EQ(lh_os_fs_stat_get_mtime(lh_addr_of(st)), 0);
    EXPECT_EQ(lh_os_fs_stat_get_ctime(lh_addr_of(st)), 0);
    EXPECT_EQ(lh_os_fs_stat_get_attr(lh_addr_of(st)), 0U);
}

TEST(os_fs_stat, empty_path_fails)
{
    lh_os_fs_path_t path;
    lh_os_fs_stat_t st;

    lh_os_fs_path_init(lh_addr_of(path));
    lh_os_fs_stat_init(lh_addr_of(st));
    EXPECT_EQ(lh_os_fs_stat(lh_addr_of(path), lh_addr_of(st)), lh_bool_false);
    EXPECT_EQ(lh_os_get_last_error_code(), lh_os_error_code_path_empty);
    lh_os_fs_path_deinit(lh_addr_of(path));
}

TEST(os_fs_stat, path_on_empty_file)
{
    lh_os_fs_path_t path;
    lh_os_fs_stat_t st;
    lh_os_fs_stat_t copy;
    lh_os_fs_file_t file;
    lh_os_fs_perm_t perm;
    lh_s64_t mtime = 0;

    lh_os_fs_path_init(lh_addr_of(path));
    ASSERT_EQ(lh_os_fs_path_set(lh_addr_of(path), lh_str_view_make("lh_os_fs_stat.bin")),
              lh_bool_true);
    lh_os_fs_file_init(lh_addr_of(file));
    ASSERT_EQ(lh_os_fs_file_open(lh_addr_of(file), lh_addr_of(path), lh_os_fs_file_mode_write),
              lh_bool_true);
    lh_os_fs_file_deinit(lh_addr_of(file));

    lh_os_fs_stat_init(lh_addr_of(st));
    ASSERT_EQ(lh_os_fs_stat(lh_addr_of(path), lh_addr_of(st)), lh_bool_true);
    EXPECT_EQ(lh_os_fs_stat_get_kind(lh_addr_of(st)), lh_os_fs_kind_file);
    EXPECT_EQ(lh_os_fs_stat_get_size(lh_addr_of(st)), 0U);
    EXPECT_NE(lh_os_fs_stat_get_mtime(lh_addr_of(st)), 0);
    perm = lh_os_fs_stat_get_perm(lh_addr_of(st));
    EXPECT_EQ(lh_os_fs_perm_has(perm, lh_os_fs_perm_irusr), lh_bool_true);
    EXPECT_EQ(lh_os_fs_attr_has(lh_os_fs_stat_get_attr(lh_addr_of(st)), lh_os_fs_attr_hidden),
              lh_bool_false);
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    EXPECT_EQ(lh_os_fs_perm_has(perm, lh_os_fs_perm_iwusr), lh_bool_true);
    EXPECT_EQ(lh_os_fs_perm_has(perm, lh_os_fs_perm_ixusr), lh_bool_false);
#else
    EXPECT_EQ(lh_os_fs_perm_has(perm, lh_os_fs_perm_iwusr), lh_bool_true);
#endif

    lh_os_fs_stat_assign(lh_addr_of(copy), lh_addr_of(st));
    EXPECT_EQ(lh_os_fs_stat_get_kind(lh_addr_of(copy)), lh_os_fs_kind_file);

    ASSERT_EQ(lh_os_fs_mtime(lh_addr_of(path), lh_addr_of(mtime)), lh_bool_true);
    EXPECT_EQ(mtime, lh_os_fs_stat_get_mtime(lh_addr_of(st)));

    lh_os_fs_path_deinit(lh_addr_of(path));
    (void)std::remove("lh_os_fs_stat.bin");
}

TEST(os_fs_stat, leading_dot_sets_hidden)
{
    lh_os_fs_path_t path;
    lh_os_fs_file_t file;
    lh_os_fs_stat_t st;

    lh_os_fs_path_init(lh_addr_of(path));
    ASSERT_EQ(lh_os_fs_path_set(lh_addr_of(path), lh_str_view_make(".lh_os_fs_stat_hidden")),
              lh_bool_true);
    lh_os_fs_file_init(lh_addr_of(file));
    ASSERT_EQ(lh_os_fs_file_open(lh_addr_of(file), lh_addr_of(path), lh_os_fs_file_mode_write),
              lh_bool_true);
    lh_os_fs_file_deinit(lh_addr_of(file));

    lh_os_fs_stat_init(lh_addr_of(st));
    ASSERT_EQ(lh_os_fs_stat(lh_addr_of(path), lh_addr_of(st)), lh_bool_true);
    EXPECT_EQ(lh_os_fs_attr_has(lh_os_fs_stat_get_attr(lh_addr_of(st)), lh_os_fs_attr_hidden),
              lh_bool_true);

    lh_os_fs_path_deinit(lh_addr_of(path));
    (void)std::remove(".lh_os_fs_stat_hidden");
}

} // namespace
