#include <gtest/gtest.h>

#include <lh/os.h>
#include <lh/os/error/code.h>
#include <lh/os/fs.h>
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

} // namespace

TEST(os_fs_exists, missing_then_file_then_gone)
{
    lh_os_fs_path_t path;
    lh_os_fs_path_t dir;
    lh_os_fs_file_t file;
    char payload[] = "e";

    ASSERT_TRUE(join_exe_name(lh_addr_of(path), "lh_os_fs_exists.bin"));
    (void)lh_os_fs_remove(lh_addr_of(path));
    EXPECT_EQ(lh_os_fs_exists(lh_addr_of(path)), lh_bool_false);
    EXPECT_NE(lh_os_get_last_error_code(), 0);

    lh_os_fs_file_init(lh_addr_of(file));
    ASSERT_EQ(lh_os_fs_file_open(lh_addr_of(file), lh_addr_of(path), lh_os_fs_file_mode_write),
              lh_bool_true);
    ASSERT_EQ(lh_os_fs_file_write(lh_addr_of(file), payload, sizeof(payload) - 1U),
              static_cast<lh_ssize_t>(sizeof(payload) - 1U));
    lh_os_fs_file_deinit(lh_addr_of(file));

    EXPECT_EQ(lh_os_fs_exists(lh_addr_of(path)), lh_bool_true);
    lh_os_fs_path_init(lh_addr_of(dir));
    ASSERT_EQ(lh_os_fs_path_exe_dir(lh_addr_of(dir)), lh_bool_true);
    EXPECT_EQ(lh_os_fs_exists(lh_addr_of(dir)), lh_bool_true);

    EXPECT_EQ(lh_os_fs_remove(lh_addr_of(path)), lh_bool_true);
    EXPECT_EQ(lh_os_fs_exists(lh_addr_of(path)), lh_bool_false);
}

TEST(os_fs_exists, empty_path_fails)
{
    lh_os_fs_path_t path;

    lh_os_fs_path_init(lh_addr_of(path));
    EXPECT_EQ(lh_os_fs_exists(lh_addr_of(path)), lh_bool_false);
    EXPECT_EQ(lh_os_get_last_error_code(), lh_os_error_code_path_empty);
}

TEST(os_fs_remove, missing_fails)
{
    lh_os_fs_path_t path;

    lh_os_fs_path_init(lh_addr_of(path));
    ASSERT_EQ(lh_os_fs_path_set(lh_addr_of(path),
                                lh_str_view_make("lh_os_fs_remove_missing_no_such_file")),
              lh_bool_true);
    EXPECT_EQ(lh_os_fs_remove(lh_addr_of(path)), lh_bool_false);
    EXPECT_NE(lh_os_get_last_error_code(), 0);
}
