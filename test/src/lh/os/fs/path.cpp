#include <gtest/gtest.h>

#include <cstring>

#if !defined(_WIN32)
#    include <unistd.h>
#endif

#include <lh/os.h>
#include <lh/os/fs/file.h>
#include <lh/os/fs/path.h>
#include <lh/util/str/ptr.h>

namespace
{

TEST(os_fs_path_sep, matches_os)
{
#if defined(_WIN32)
    EXPECT_EQ(lh_os_fs_path_sep(), '\\');
#else
    EXPECT_EQ(lh_os_fs_path_sep(), '/');
#endif
}

TEST(os_fs_path_exe, writes_non_empty_absolute_looking_path)
{
    char exe[4096];

    ASSERT_EQ(lh_os_fs_path_exe(exe, sizeof(exe)), lh_bool_true);
    EXPECT_NE(exe[0], '\0');
    EXPECT_TRUE(lh_str_ptr_find_of_char(exe, lh_str_ptr_len(exe), lh_os_fs_path_sep()) != lh_null ||
                lh_str_ptr_find_of_char(exe, lh_str_ptr_len(exe), '/') != lh_null);
}

TEST(os_fs_path_exe, rejects_tiny_buffer)
{
    char exe[4];

    EXPECT_EQ(lh_os_fs_path_exe(exe, sizeof(exe)), lh_bool_false);
    EXPECT_EQ(exe[0], '\0');
    EXPECT_NE(lh_os_get_last_error_code(), 0);
}

TEST(os_fs_path_exe_dir, is_prefix_of_exe_and_join_restores_exe)
{
    char exe[4096];
    char dir[4096];
    char joined[4096];
    const char *slash;
    const char *name;

    ASSERT_EQ(lh_os_fs_path_exe(exe, sizeof(exe)), lh_bool_true);
    ASSERT_EQ(lh_os_fs_path_exe_dir(dir, sizeof(dir)), lh_bool_true);
    EXPECT_NE(dir[0], '\0');

    slash = strrchr(exe, lh_os_fs_path_sep());
    if (slash == NULL)
    {
        slash = strrchr(exe, '/');
    }
    ASSERT_TRUE(slash != NULL);
    name = slash + 1;
    ASSERT_EQ(lh_os_fs_path_join(joined, sizeof(joined), dir, name), lh_bool_true);
    EXPECT_STREQ(joined, exe);
}

TEST(os_fs_path_dir, of_exe_matches_exe_dir)
{
    char exe[4096];
    char from_exe[4096];
    char dir[4096];

    ASSERT_EQ(lh_os_fs_path_exe(exe, sizeof(exe)), lh_bool_true);
    ASSERT_EQ(lh_os_fs_path_dir(exe, from_exe, sizeof(from_exe)), lh_bool_true);
    ASSERT_EQ(lh_os_fs_path_exe_dir(dir, sizeof(dir)), lh_bool_true);
    EXPECT_STREQ(from_exe, dir);
}

TEST(os_fs_path_dir, drops_last_component)
{
    char out[64];

    ASSERT_EQ(lh_os_fs_path_dir("dir/file", out, sizeof(out)), lh_bool_true);
    EXPECT_STREQ(out, "dir");
    ASSERT_EQ(lh_os_fs_path_dir("file", out, sizeof(out)), lh_bool_true);
    EXPECT_STREQ(out, ".");
}

TEST(os_fs_path_dir, rejects_null_and_empty)
{
    char out[16];

    EXPECT_EQ(lh_os_fs_path_dir(nullptr, out, sizeof(out)), lh_bool_false);
    EXPECT_EQ(out[0], '\0');
    EXPECT_EQ(lh_os_fs_path_dir("", out, sizeof(out)), lh_bool_false);
    EXPECT_EQ(out[0], '\0');
}

TEST(os_fs_path_join, inserts_sep_when_missing)
{
    char out[64];

    ASSERT_EQ(lh_os_fs_path_join(out, sizeof(out), "dir", "file"), lh_bool_true);
#if defined(_WIN32)
    EXPECT_STREQ(out, "dir\\file");
#else
    EXPECT_STREQ(out, "dir/file");
#endif
}

TEST(os_fs_path_join, does_not_double_sep)
{
    char out[64];
    char dir[8];

    dir[0] = 'd';
    dir[1] = lh_os_fs_path_sep();
    dir[2] = '\0';
    ASSERT_EQ(lh_os_fs_path_join(out, sizeof(out), dir, "file"), lh_bool_true);
#if defined(_WIN32)
    EXPECT_STREQ(out, "d\\file");
#else
    EXPECT_STREQ(out, "d/file");
#endif
}

TEST(os_fs_path_join, rejects_empty_name)
{
    char out[16];

    EXPECT_EQ(lh_os_fs_path_join(out, sizeof(out), "dir", ""), lh_bool_false);
    EXPECT_EQ(out[0], '\0');
}

TEST(os_fs_path_mtime, missing_file_fails)
{
    lh_s64_t mtime = 1;

    EXPECT_EQ(lh_os_fs_path_mtime("lh_os_fs_path_missing_no_such_file", &mtime), lh_bool_false);
    EXPECT_NE(lh_os_get_last_error_code(), 0);
}

TEST(os_fs_path_read, returns_whole_file_and_rejects_tiny_buffer)
{
    char dir[4096];
    char path[4096];
    char payload[] = "file_get_contents";
    char buf[64];
    char tiny[4];
    lh_os_fs_file_t file;
    lh_usize_t n = 0;

    ASSERT_EQ(lh_os_fs_path_exe_dir(dir, sizeof(dir)), lh_bool_true);
    ASSERT_EQ(lh_os_fs_path_join(path, sizeof(path), dir, "lh_os_fs_path_read.bin"), lh_bool_true);
    (void)lh_os_fs_path_remove(path);

    lh_os_fs_file_init(&file);
    ASSERT_EQ(lh_os_fs_file_open(&file, path, lh_os_fs_file_mode_write), lh_bool_true);
    ASSERT_EQ(lh_os_fs_file_write(&file, payload, sizeof(payload) - 1U),
              static_cast<lh_ssize_t>(sizeof(payload) - 1U));
    lh_os_fs_file_close(&file);

    ASSERT_EQ(lh_os_fs_path_read(path, buf, sizeof(buf), &n), lh_bool_true);
    EXPECT_EQ(n, sizeof(payload) - 1U);
    buf[n] = '\0';
    EXPECT_STREQ(buf, payload);

    EXPECT_EQ(lh_os_fs_path_read(path, tiny, sizeof(tiny), &n), lh_bool_false);
    EXPECT_EQ(lh_os_fs_path_read("lh_os_fs_path_read_missing", buf, sizeof(buf), &n), lh_bool_false);

    EXPECT_EQ(lh_os_fs_path_remove(path), lh_bool_true);
}

TEST(os_fs_path_is, file_and_dir_of_exe)
{
    char exe[4096];
    char dir[4096];

    ASSERT_EQ(lh_os_fs_path_exe(exe, sizeof(exe)), lh_bool_true);
    ASSERT_EQ(lh_os_fs_path_exe_dir(dir, sizeof(dir)), lh_bool_true);

    EXPECT_EQ(lh_os_fs_path_is_file(exe), lh_bool_true);
    EXPECT_EQ(lh_os_fs_path_is(exe, lh_os_fs_kind_file), lh_bool_true);
    EXPECT_EQ(lh_os_fs_path_is_dir(exe), lh_bool_false);
    EXPECT_EQ(lh_os_fs_path_is(exe, lh_os_fs_kind_dir), lh_bool_false);
    EXPECT_EQ(lh_os_fs_path_is_symlink(exe), lh_bool_false);
    EXPECT_EQ(lh_os_fs_path_is_shortcut(exe), lh_bool_false);

    EXPECT_EQ(lh_os_fs_path_is_dir(dir), lh_bool_true);
    EXPECT_EQ(lh_os_fs_path_is(dir, lh_os_fs_kind_dir), lh_bool_true);
    EXPECT_EQ(lh_os_fs_path_is_file(dir), lh_bool_false);
    EXPECT_EQ(lh_os_fs_path_is(dir, lh_os_fs_kind_file), lh_bool_false);
    EXPECT_EQ(lh_os_fs_path_is_symlink(dir), lh_bool_false);
    EXPECT_EQ(lh_os_fs_path_is_shortcut(dir), lh_bool_false);
}

TEST(os_fs_path_is, missing_is_false)
{
    EXPECT_EQ(lh_os_fs_path_is_file("lh_os_fs_path_missing_no_such_file"), lh_bool_false);
    EXPECT_EQ(lh_os_fs_path_is_dir("lh_os_fs_path_missing_no_such_file"), lh_bool_false);
    EXPECT_EQ(lh_os_fs_path_is_symlink("lh_os_fs_path_missing_no_such_file"), lh_bool_false);
    EXPECT_EQ(lh_os_fs_path_is_shortcut("lh_os_fs_path_missing_no_such_file"), lh_bool_false);
    EXPECT_EQ(lh_os_fs_path_is("lh_os_fs_path_missing_no_such_file", lh_os_fs_kind_file),
              lh_bool_false);
    EXPECT_EQ(lh_os_fs_path_is("lh_os_fs_path_missing_no_such_file", 255U), lh_bool_false);
    EXPECT_NE(lh_os_get_last_error_code(), 0);
}

TEST(os_fs_path_is, shortcut_magic_and_not_plain_file)
{
    char dir[4096];
    char path[4096];
    unsigned char magic[20] = {0x4C, 0x00, 0x00, 0x00, 0x01, 0x14, 0x02, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46};
    char plain[] = "not-a-shortcut";
    lh_os_fs_file_t file;

    ASSERT_EQ(lh_os_fs_path_exe_dir(dir, sizeof(dir)), lh_bool_true);
    ASSERT_EQ(lh_os_fs_path_join(path, sizeof(path), dir, "lh_os_fs_path_shortcut.bin"),
              lh_bool_true);
    (void)lh_os_fs_path_remove(path);

    lh_os_fs_file_init(&file);
    ASSERT_EQ(lh_os_fs_file_open(&file, path, lh_os_fs_file_mode_write), lh_bool_true);
    ASSERT_EQ(lh_os_fs_file_write(&file, magic, sizeof(magic)),
              static_cast<lh_ssize_t>(sizeof(magic)));
    lh_os_fs_file_close(&file);

    EXPECT_EQ(lh_os_fs_path_is_file(path), lh_bool_true);
    EXPECT_EQ(lh_os_fs_path_is(path, lh_os_fs_kind_file), lh_bool_true);
    EXPECT_EQ(lh_os_fs_path_is_shortcut(path), lh_bool_true);
    EXPECT_EQ(lh_os_fs_path_is(path, lh_os_fs_kind_shortcut), lh_bool_true);
    EXPECT_EQ(lh_os_fs_path_is_symlink(path), lh_bool_false);
    EXPECT_EQ(lh_os_fs_path_is_dir(path), lh_bool_false);
    EXPECT_EQ(lh_os_fs_path_remove(path), lh_bool_true);

    lh_os_fs_file_init(&file);
    ASSERT_EQ(lh_os_fs_file_open(&file, path, lh_os_fs_file_mode_write), lh_bool_true);
    ASSERT_EQ(lh_os_fs_file_write(&file, plain, sizeof(plain) - 1U),
              static_cast<lh_ssize_t>(sizeof(plain) - 1U));
    lh_os_fs_file_close(&file);

    EXPECT_EQ(lh_os_fs_path_is_file(path), lh_bool_true);
    EXPECT_EQ(lh_os_fs_path_is_shortcut(path), lh_bool_false);
    EXPECT_EQ(lh_os_fs_path_remove(path), lh_bool_true);
}

#if !defined(_WIN32)
TEST(os_fs_path_is, posix_symlink_to_file)
{
    char exe[4096];
    char dir[4096];
    char link_path[4096];

    ASSERT_EQ(lh_os_fs_path_exe(exe, sizeof(exe)), lh_bool_true);
    ASSERT_EQ(lh_os_fs_path_exe_dir(dir, sizeof(dir)), lh_bool_true);
    ASSERT_EQ(lh_os_fs_path_join(link_path, sizeof(link_path), dir, "lh_os_fs_path_symlink"),
              lh_bool_true);
    (void)lh_os_fs_path_remove(link_path);
    if (symlink(exe, link_path) != 0)
    {
        return;
    }
    EXPECT_EQ(lh_os_fs_path_is_symlink(link_path), lh_bool_true);
    EXPECT_EQ(lh_os_fs_path_is(link_path, lh_os_fs_kind_symlink), lh_bool_true);
    EXPECT_EQ(lh_os_fs_path_is_file(link_path), lh_bool_true);
    EXPECT_EQ(lh_os_fs_path_is(link_path, lh_os_fs_kind_file), lh_bool_true);
    EXPECT_EQ(lh_os_fs_path_is_dir(link_path), lh_bool_false);
    EXPECT_EQ(lh_os_fs_path_is_shortcut(link_path), lh_bool_false);
    EXPECT_EQ(lh_os_fs_path_remove(link_path), lh_bool_true);
}
#endif

} // namespace
