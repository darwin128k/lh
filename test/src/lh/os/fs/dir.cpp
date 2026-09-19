#include <gtest/gtest.h>

#include <cstring>

#include <lh/os.h>
#include <lh/os/fs/dir.h>
#include <lh/os/fs/path.h>
#include <lh/util/str/ptr.h>

namespace
{

static const char *
exe_basename(char *exe, lh_usize_t exe_size)
{
    const char *slash;

    if (lh_os_fs_path_exe(exe, exe_size) != lh_bool_true)
    {
        return nullptr;
    }
    slash = strrchr(exe, lh_os_fs_path_sep());
    if (slash == NULL)
    {
        slash = strrchr(exe, '/');
    }
    if (slash == NULL)
    {
        return exe;
    }
    return slash + 1;
}

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

    lh_os_fs_dir_init(&dir);
    EXPECT_EQ(lh_os_fs_dir_open(&dir, "lh_os_fs_dir_missing_no_such_dir"), lh_bool_false);
    EXPECT_EQ(lh_os_fs_dir_is_valid(&dir), lh_bool_false);
    EXPECT_NE(lh_os_get_last_error_code(), 0);
}

TEST(os_fs_dir, lists_exe_dir_and_finds_exe)
{
    char exe[4096];
    char parent[4096];
    char name[LH_OS_FS_DIR_NAME_MAX + 1U];
    const char *want;
    lh_os_fs_dir_t dir;
    lh_os_fs_dir_entry_kind_t kind = lh_os_fs_dir_entry_kind_other;
    lh_ssize_t n;
    bool found = false;

    want = exe_basename(exe, sizeof(exe));
    ASSERT_TRUE(want != nullptr);
    ASSERT_EQ(lh_os_fs_path_exe_dir(parent, sizeof(parent)), lh_bool_true);

    lh_os_fs_dir_init(&dir);
    ASSERT_EQ(lh_os_fs_dir_open(&dir, parent), lh_bool_true);
    EXPECT_EQ(lh_os_fs_dir_is_valid(&dir), lh_bool_true);

    for (;;)
    {
        n = lh_os_fs_dir_read(&dir, name, sizeof(name), &kind);
        ASSERT_GE(n, 0);
        if (n == 0)
        {
            break;
        }
        EXPECT_EQ(static_cast<lh_usize_t>(n), lh_str_ptr_len(name));
        EXPECT_STRNE(name, ".");
        EXPECT_STRNE(name, "..");
        if (std::strcmp(name, want) == 0)
        {
            EXPECT_EQ(kind, lh_os_fs_dir_entry_kind_file);
            found = true;
        }
    }

    lh_os_fs_dir_close(&dir);
    EXPECT_EQ(lh_os_fs_dir_is_valid(&dir), lh_bool_false);
    EXPECT_TRUE(found);
}

TEST(os_fs_dir, tiny_buffer_keeps_entry)
{
    char parent[4096];
    char tiny[1];
    char name[LH_OS_FS_DIR_NAME_MAX + 1U];
    char again[LH_OS_FS_DIR_NAME_MAX + 1U];
    lh_os_fs_dir_t dir;
    lh_ssize_t n;

    ASSERT_EQ(lh_os_fs_path_exe_dir(parent, sizeof(parent)), lh_bool_true);

    lh_os_fs_dir_init(&dir);
    ASSERT_EQ(lh_os_fs_dir_open(&dir, parent), lh_bool_true);

    n = lh_os_fs_dir_read(&dir, tiny, sizeof(tiny), nullptr);
    EXPECT_EQ(n, -1);
    EXPECT_NE(lh_os_get_last_error_code(), 0);

    n = lh_os_fs_dir_read(&dir, name, sizeof(name), nullptr);
    ASSERT_GT(n, 0);

    n = lh_os_fs_dir_read(&dir, tiny, sizeof(tiny), nullptr);
    if (n < 0)
    {
        n = lh_os_fs_dir_read(&dir, again, sizeof(again), nullptr);
        ASSERT_GT(n, 0);
        EXPECT_STRNE(again, name);
    }

    lh_os_fs_dir_close(&dir);
}

} // namespace
