#include <gtest/gtest.h>

#include <lh/compiler/os.h>
#include <lh/os.h>
#include <lh/os/error/code.h>
#include <lh/os/fs/file.h>
#include <lh/os/fs/path.h>
#include <lh/os/fs/stat.h>
#include <lh/str/view.h>
#include <lh/util/addr.h>

namespace
{

static bool
join_exe_name(lh_os_fs_path_t *out, const char *name)
{
    lh_os_fs_path_t dir;
    lh_os_fs_path_t part;

    lh_os_fs_path_init(lh_addr_of(dir));
    lh_os_fs_path_init(out);
    lh_os_fs_path_init(lh_addr_of(part));
    if (lh_os_fs_path_set(lh_addr_of(part), lh_str_view_make(name)) != lh_bool_true)
    {
        return false;
    }
    if (lh_os_fs_path_exe_dir(lh_addr_of(dir)) != lh_bool_true)
    {
        return false;
    }
    return lh_os_fs_path_join(out, lh_addr_of(dir), lh_addr_of(part)) == lh_bool_true;
}

static bool
write_payload(const lh_os_fs_path_t *path, const char *payload, lh_usize_t n)
{
    lh_os_fs_file_t file;
    lh_ssize_t wrote;

    lh_os_fs_file_init(lh_addr_of(file));
    if (lh_os_fs_file_open(lh_addr_of(file), path, lh_os_fs_file_mode_write) != lh_bool_true)
    {
        return false;
    }
    wrote = lh_os_fs_file_write(lh_addr_of(file), payload, n);
    lh_os_fs_file_close(lh_addr_of(file));
    return wrote == static_cast<lh_ssize_t>(n);
}

} // namespace

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

TEST(os_fs_stat, path_and_file_agree_on_written_file)
{
    lh_os_fs_path_t path;
    lh_os_fs_stat_t from_path;
    lh_os_fs_stat_t from_file;
    lh_os_fs_stat_t copy;
    lh_os_fs_file_t file;
    char payload[] = "stat";
    lh_os_fs_perm_t perm;

    ASSERT_TRUE(join_exe_name(lh_addr_of(path), "lh_os_fs_stat.bin"));
    (void)lh_os_fs_path_remove(lh_addr_of(path));
    ASSERT_TRUE(write_payload(lh_addr_of(path), payload, sizeof(payload) - 1U));

    lh_os_fs_stat_init(lh_addr_of(from_path));
    ASSERT_EQ(lh_os_fs_path_stat(lh_addr_of(path), lh_addr_of(from_path)), lh_bool_true);
    EXPECT_EQ(lh_os_fs_stat_get_kind(lh_addr_of(from_path)), lh_os_fs_kind_file);
    EXPECT_EQ(lh_os_fs_stat_get_size(lh_addr_of(from_path)),
              static_cast<lh_u64_t>(sizeof(payload) - 1U));
    EXPECT_NE(lh_os_fs_stat_get_mtime(lh_addr_of(from_path)), 0);
    perm = lh_os_fs_stat_get_perm(lh_addr_of(from_path));
    EXPECT_EQ(lh_os_fs_perm_has(perm, lh_os_fs_perm_irusr), lh_bool_true);
    EXPECT_EQ(lh_os_fs_perm_has(perm, lh_os_fs_perm_iwusr), lh_bool_true);
    EXPECT_EQ(lh_os_fs_attr_has(lh_os_fs_stat_get_attr(lh_addr_of(from_path)),
                                lh_os_fs_attr_hidden),
              lh_bool_false);
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    EXPECT_EQ(lh_os_fs_perm_has(perm, lh_os_fs_perm_iwoth), lh_bool_true);
    EXPECT_EQ(lh_os_fs_perm_has(perm, lh_os_fs_perm_ixusr), lh_bool_false);
#else
    EXPECT_EQ(lh_os_fs_perm_has(perm, lh_os_fs_perm_iwoth), lh_bool_false);
#endif

    lh_os_fs_file_init(lh_addr_of(file));
    ASSERT_EQ(lh_os_fs_file_open(lh_addr_of(file), lh_addr_of(path), lh_os_fs_file_mode_read),
              lh_bool_true);
    lh_os_fs_stat_init(lh_addr_of(from_file));
    ASSERT_EQ(lh_os_fs_file_stat(lh_addr_of(file), lh_addr_of(from_file)), lh_bool_true);
    EXPECT_EQ(lh_os_fs_stat_get_kind(lh_addr_of(from_file)), lh_os_fs_kind_file);
    EXPECT_EQ(lh_os_fs_stat_get_size(lh_addr_of(from_file)),
              lh_os_fs_stat_get_size(lh_addr_of(from_path)));
    lh_os_fs_file_close(lh_addr_of(file));

    lh_os_fs_stat_init(lh_addr_of(copy));
    lh_os_fs_stat_assign(lh_addr_of(copy), lh_addr_of(from_path));
    EXPECT_EQ(lh_os_fs_stat_get_mtime(lh_addr_of(copy)),
              lh_os_fs_stat_get_mtime(lh_addr_of(from_path)));
    EXPECT_EQ(lh_os_fs_stat_get_perm(lh_addr_of(copy)), perm);

    (void)lh_os_fs_path_remove(lh_addr_of(path));
}

TEST(os_fs_stat, exe_dir_is_directory)
{
    lh_os_fs_path_t dir;
    lh_os_fs_stat_t st;

    lh_os_fs_path_init(lh_addr_of(dir));
    ASSERT_EQ(lh_os_fs_path_exe_dir(lh_addr_of(dir)), lh_bool_true);
    lh_os_fs_stat_init(lh_addr_of(st));
    ASSERT_EQ(lh_os_fs_path_stat(lh_addr_of(dir), lh_addr_of(st)), lh_bool_true);
    EXPECT_EQ(lh_os_fs_stat_get_kind(lh_addr_of(st)), lh_os_fs_kind_dir);
    EXPECT_EQ(lh_os_fs_perm_has(lh_os_fs_stat_get_perm(lh_addr_of(st)), lh_os_fs_perm_ixusr),
              lh_bool_true);
}

TEST(os_fs_stat, leading_dot_name_is_hidden)
{
    lh_os_fs_path_t path;
    lh_os_fs_stat_t st;
    char payload[] = "h";

    ASSERT_TRUE(join_exe_name(lh_addr_of(path), ".lh_os_fs_stat_hidden"));
    (void)lh_os_fs_path_remove(lh_addr_of(path));
    ASSERT_TRUE(write_payload(lh_addr_of(path), payload, sizeof(payload) - 1U));

    lh_os_fs_stat_init(lh_addr_of(st));
    ASSERT_EQ(lh_os_fs_path_stat(lh_addr_of(path), lh_addr_of(st)), lh_bool_true);
    EXPECT_EQ(lh_os_fs_attr_has(lh_os_fs_stat_get_attr(lh_addr_of(st)), lh_os_fs_attr_hidden),
              lh_bool_true);

    (void)lh_os_fs_path_remove(lh_addr_of(path));
}

TEST(os_fs_stat, missing_path_fails)
{
    lh_os_fs_path_t path;
    lh_os_fs_stat_t st;

    lh_os_fs_path_init(lh_addr_of(path));
    ASSERT_EQ(lh_os_fs_path_set(lh_addr_of(path),
                                lh_str_view_make("lh_os_fs_stat_missing_no_such_file")),
              lh_bool_true);
    lh_os_fs_stat_init(lh_addr_of(st));
    EXPECT_EQ(lh_os_fs_path_stat(lh_addr_of(path), lh_addr_of(st)), lh_bool_false);
    EXPECT_NE(lh_os_get_last_error_code(), 0);
}

TEST(os_fs_stat, closed_file_fails_not_open)
{
    lh_os_fs_file_t file;
    lh_os_fs_stat_t st;

    lh_os_fs_file_init(lh_addr_of(file));
    lh_os_fs_stat_init(lh_addr_of(st));
    EXPECT_EQ(lh_os_fs_file_stat(lh_addr_of(file), lh_addr_of(st)), lh_bool_false);
    EXPECT_EQ(lh_os_get_last_error_code(), lh_os_error_code_not_open);
}
