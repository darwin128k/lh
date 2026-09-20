#include <gtest/gtest.h>

#if defined(_WIN32)
#    define WIN32_LEAN_AND_MEAN
#    include <windows.h>
#else
#    include <sys/stat.h>
#    include <unistd.h>
#endif

#include <lh/os.h>
#include <lh/os/fs.h>
#include <lh/str.h>
#include <lh/str/view.h>
#include <lh/str/view/initializer.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>
#include <lh/util/str/ptr.h>
#include <lh/vector.h>

namespace
{

static const char *
path_cstr(const lh_os_fs_path_t *path)
{
    return lh_str_get_data(lh_os_fs_path_get_text_as_const(path));
}

static void
path_set_lit(lh_os_fs_path_t *path, const char *text)
{
    lh_os_fs_path_init(path);
    ASSERT_EQ(lh_os_fs_path_set(path, lh_str_view_make(text)), lh_bool_true);
}

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
    lh_os_fs_path_t exe;
    lh_str_view_t view;

    lh_os_fs_path_init(lh_addr_of(exe));
    ASSERT_EQ(lh_os_fs_path_exe(lh_addr_of(exe)), lh_bool_true);
    EXPECT_EQ(lh_os_fs_path_is_empty(lh_addr_of(exe)), lh_bool_false);
    view = lh_os_fs_path_as_view(lh_addr_of(exe));
    EXPECT_TRUE(lh_str_view_contains_char(lh_addr_of(view), lh_os_fs_path_sep()) == lh_bool_true ||
                lh_str_view_contains_char(lh_addr_of(view), '/') == lh_bool_true);
}

TEST(os_fs_path_exe_dir, is_prefix_of_exe_and_join_restores_exe)
{
    lh_os_fs_path_t exe;
    lh_os_fs_path_t dir;
    lh_os_fs_path_t name;
    lh_os_fs_path_t joined;
    const lh_vector_t *parts;
    lh_usize_t n;
    lh_str_view_t last;

    lh_os_fs_path_init(lh_addr_of(exe));
    lh_os_fs_path_init(lh_addr_of(dir));
    lh_os_fs_path_init(lh_addr_of(name));
    lh_os_fs_path_init(lh_addr_of(joined));
    ASSERT_EQ(lh_os_fs_path_exe(lh_addr_of(exe)), lh_bool_true);
    ASSERT_EQ(lh_os_fs_path_exe_dir(lh_addr_of(dir)), lh_bool_true);
    EXPECT_EQ(lh_os_fs_path_is_empty(lh_addr_of(dir)), lh_bool_false);

    parts = lh_os_fs_path_get_parts_as_const(lh_addr_of(exe));
    n = lh_vector_get_size(parts);
    ASSERT_GT(n, 0U);
    last = lh_os_fs_path_get_part(lh_addr_of(exe), n - 1U);
    ASSERT_EQ(lh_str_view_is_empty(lh_addr_of(last)), lh_bool_false);
    ASSERT_EQ(lh_os_fs_path_set(lh_addr_of(name), last), lh_bool_true);
    ASSERT_EQ(lh_os_fs_path_join(lh_addr_of(joined), lh_addr_of(dir), lh_addr_of(name)),
              lh_bool_true);
    EXPECT_STREQ(path_cstr(lh_addr_of(joined)), path_cstr(lh_addr_of(exe)));
}

TEST(os_fs_path_dir, of_exe_matches_exe_dir)
{
    lh_os_fs_path_t exe;
    lh_os_fs_path_t from_exe;
    lh_os_fs_path_t dir;

    lh_os_fs_path_init(lh_addr_of(exe));
    lh_os_fs_path_init(lh_addr_of(from_exe));
    lh_os_fs_path_init(lh_addr_of(dir));
    ASSERT_EQ(lh_os_fs_path_exe(lh_addr_of(exe)), lh_bool_true);
    ASSERT_EQ(lh_os_fs_path_dir(lh_addr_of(exe), lh_addr_of(from_exe)), lh_bool_true);
    ASSERT_EQ(lh_os_fs_path_exe_dir(lh_addr_of(dir)), lh_bool_true);
    EXPECT_STREQ(path_cstr(lh_addr_of(from_exe)), path_cstr(lh_addr_of(dir)));
}

TEST(os_fs_path_dir, drops_last_component)
{
    lh_os_fs_path_t path;
    lh_os_fs_path_t out;

    path_set_lit(lh_addr_of(path), "dir/file");
    lh_os_fs_path_init(lh_addr_of(out));
    ASSERT_EQ(lh_os_fs_path_dir(lh_addr_of(path), lh_addr_of(out)), lh_bool_true);
    EXPECT_STREQ(path_cstr(lh_addr_of(out)), "dir");
    ASSERT_EQ(lh_os_fs_path_set(lh_addr_of(path), lh_str_view_make("file")), lh_bool_true);
    ASSERT_EQ(lh_os_fs_path_dir(lh_addr_of(path), lh_addr_of(out)), lh_bool_true);
    EXPECT_STREQ(path_cstr(lh_addr_of(out)), ".");
}

TEST(os_fs_path_dir, keeps_root)
{
    lh_os_fs_path_t path;
    lh_os_fs_path_t out;

    lh_os_fs_path_init(lh_addr_of(out));
#if defined(_WIN32)
    path_set_lit(lh_addr_of(path), "C:/foo/bar");
    ASSERT_EQ(lh_os_fs_path_dir(lh_addr_of(path), lh_addr_of(out)), lh_bool_true);
    EXPECT_STREQ(path_cstr(lh_addr_of(out)), "C:\\foo");
    ASSERT_EQ(lh_os_fs_path_set(lh_addr_of(path), lh_str_view_make("C:/foo")), lh_bool_true);
    ASSERT_EQ(lh_os_fs_path_dir(lh_addr_of(path), lh_addr_of(out)), lh_bool_true);
    EXPECT_STREQ(path_cstr(lh_addr_of(out)), "C:\\");
#else
    path_set_lit(lh_addr_of(path), "/usr/bin");
    ASSERT_EQ(lh_os_fs_path_dir(lh_addr_of(path), lh_addr_of(out)), lh_bool_true);
    EXPECT_STREQ(path_cstr(lh_addr_of(out)), "/usr");
    ASSERT_EQ(lh_os_fs_path_set(lh_addr_of(path), lh_str_view_make("/usr")), lh_bool_true);
    ASSERT_EQ(lh_os_fs_path_dir(lh_addr_of(path), lh_addr_of(out)), lh_bool_true);
    EXPECT_STREQ(path_cstr(lh_addr_of(out)), "/");
#endif
}

TEST(os_fs_path_dir, rejects_empty)
{
    lh_os_fs_path_t empty;
    lh_os_fs_path_t out;

    lh_os_fs_path_init(lh_addr_of(empty));
    lh_os_fs_path_init(lh_addr_of(out));
    EXPECT_EQ(lh_os_fs_path_dir(lh_addr_of(empty), lh_addr_of(out)), lh_bool_false);
    EXPECT_EQ(lh_os_fs_path_is_empty(lh_addr_of(out)), lh_bool_true);
}

TEST(os_fs_path_join, inserts_sep_when_missing)
{
    lh_os_fs_path_t dir;
    lh_os_fs_path_t name;
    lh_os_fs_path_t out;

    path_set_lit(lh_addr_of(dir), "dir");
    path_set_lit(lh_addr_of(name), "file");
    lh_os_fs_path_init(lh_addr_of(out));
    ASSERT_EQ(lh_os_fs_path_join(lh_addr_of(out), lh_addr_of(dir), lh_addr_of(name)),
              lh_bool_true);
#if defined(_WIN32)
    EXPECT_STREQ(path_cstr(lh_addr_of(out)), "dir\\file");
#else
    EXPECT_STREQ(path_cstr(lh_addr_of(out)), "dir/file");
#endif
}

TEST(os_fs_path_join, does_not_double_sep)
{
    lh_os_fs_path_t dir;
    lh_os_fs_path_t name;
    lh_os_fs_path_t out;
    lh_char_t dir_text[3];

    dir_text[0] = 'd';
    dir_text[1] = lh_os_fs_path_sep();
    dir_text[2] = '\0';
    path_set_lit(lh_addr_of(dir), dir_text);
    path_set_lit(lh_addr_of(name), "file");
    lh_os_fs_path_init(lh_addr_of(out));
    ASSERT_EQ(lh_os_fs_path_join(lh_addr_of(out), lh_addr_of(dir), lh_addr_of(name)),
              lh_bool_true);
#if defined(_WIN32)
    EXPECT_STREQ(path_cstr(lh_addr_of(out)), "d\\file");
#else
    EXPECT_STREQ(path_cstr(lh_addr_of(out)), "d/file");
#endif
}

TEST(os_fs_path_join, rejects_empty_name)
{
    lh_os_fs_path_t dir;
    lh_os_fs_path_t name;
    lh_os_fs_path_t out;

    path_set_lit(lh_addr_of(dir), "dir");
    lh_os_fs_path_init(lh_addr_of(name));
    lh_os_fs_path_init(lh_addr_of(out));
    EXPECT_EQ(lh_os_fs_path_join(lh_addr_of(out), lh_addr_of(dir), lh_addr_of(name)),
              lh_bool_false);
    EXPECT_EQ(lh_os_fs_path_is_empty(lh_addr_of(out)), lh_bool_true);
}

TEST(os_fs_path_join, appends_when_out_is_dir)
{
    lh_os_fs_path_t dir;
    lh_os_fs_path_t name;

    path_set_lit(lh_addr_of(dir), "dir");
    path_set_lit(lh_addr_of(name), "file");
    ASSERT_EQ(lh_os_fs_path_join(lh_addr_of(dir), lh_addr_of(dir), lh_addr_of(name)),
              lh_bool_true);
#if defined(_WIN32)
    EXPECT_STREQ(path_cstr(lh_addr_of(dir)), "dir\\file");
#else
    EXPECT_STREQ(path_cstr(lh_addr_of(dir)), "dir/file");
#endif
}

TEST(os_fs_path_join, copies_when_out_is_name)
{
    lh_os_fs_path_t dir;
    lh_os_fs_path_t name;

    path_set_lit(lh_addr_of(dir), "dir");
    path_set_lit(lh_addr_of(name), "file");
    ASSERT_EQ(lh_os_fs_path_join(lh_addr_of(name), lh_addr_of(dir), lh_addr_of(name)),
              lh_bool_true);
#if defined(_WIN32)
    EXPECT_STREQ(path_cstr(lh_addr_of(name)), "dir\\file");
#else
    EXPECT_STREQ(path_cstr(lh_addr_of(name)), "dir/file");
#endif
}

TEST(os_fs_path_mtime, missing_file_fails)
{
    lh_os_fs_path_t path;
    lh_s64_t mtime = 1;

    path_set_lit(lh_addr_of(path), "lh_os_fs_path_missing_no_such_file");
    EXPECT_EQ(lh_os_fs_mtime(lh_addr_of(path), &mtime), lh_bool_false);
    EXPECT_NE(lh_os_get_last_error_code(), 0);
}

TEST(os_fs_path_read, returns_whole_file_and_rejects_tiny_buffer)
{
    lh_os_fs_path_t dir;
    lh_os_fs_path_t name;
    lh_os_fs_path_t path;
    lh_os_fs_path_t missing;
    char payload[] = "file_get_contents";
    char buf[64];
    char tiny[4];
    lh_os_fs_file_t file;
    lh_usize_t n = 0;

    lh_os_fs_path_init(lh_addr_of(dir));
    lh_os_fs_path_init(lh_addr_of(path));
    path_set_lit(lh_addr_of(name), "lh_os_fs_path_read.bin");
    path_set_lit(lh_addr_of(missing), "lh_os_fs_path_read_missing");
    ASSERT_EQ(lh_os_fs_path_exe_dir(lh_addr_of(dir)), lh_bool_true);
    ASSERT_EQ(lh_os_fs_path_join(lh_addr_of(path), lh_addr_of(dir), lh_addr_of(name)),
              lh_bool_true);
    (void)lh_os_fs_remove(lh_addr_of(path));

    lh_os_fs_file_init(&file);
    ASSERT_EQ(lh_os_fs_file_open(&file, lh_addr_of(path), lh_os_fs_file_mode_write), lh_bool_true);
    ASSERT_EQ(lh_os_fs_file_write(&file, payload, sizeof(payload) - 1U),
              static_cast<lh_ssize_t>(sizeof(payload) - 1U));
    lh_os_fs_file_deinit(&file);

    ASSERT_EQ(lh_os_fs_read(lh_addr_of(path), buf, sizeof(buf), &n), lh_bool_true);
    EXPECT_EQ(n, sizeof(payload) - 1U);
    buf[n] = '\0';
    EXPECT_STREQ(buf, payload);

    EXPECT_EQ(lh_os_fs_read(lh_addr_of(path), tiny, sizeof(tiny), &n), lh_bool_false);
    EXPECT_EQ(lh_os_fs_read(lh_addr_of(missing), buf, sizeof(buf), &n), lh_bool_false);

    EXPECT_EQ(lh_os_fs_remove(lh_addr_of(path)), lh_bool_true);
}

TEST(os_fs_path_is, file_and_dir_of_exe)
{
    lh_os_fs_path_t exe;
    lh_os_fs_path_t dir;

    lh_os_fs_path_init(lh_addr_of(exe));
    lh_os_fs_path_init(lh_addr_of(dir));
    ASSERT_EQ(lh_os_fs_path_exe(lh_addr_of(exe)), lh_bool_true);
    ASSERT_EQ(lh_os_fs_path_exe_dir(lh_addr_of(dir)), lh_bool_true);

    EXPECT_EQ(lh_os_fs_is_file(lh_addr_of(exe)), lh_bool_true);
    EXPECT_EQ(lh_os_fs_is(lh_addr_of(exe), lh_os_fs_kind_file), lh_bool_true);
    EXPECT_EQ(lh_os_fs_is_dir(lh_addr_of(exe)), lh_bool_false);
    EXPECT_EQ(lh_os_fs_is(lh_addr_of(exe), lh_os_fs_kind_dir), lh_bool_false);
    EXPECT_EQ(lh_os_fs_is_symlink(lh_addr_of(exe)), lh_bool_false);
    EXPECT_EQ(lh_os_fs_is_shortcut(lh_addr_of(exe)), lh_bool_false);

    EXPECT_EQ(lh_os_fs_is_dir(lh_addr_of(dir)), lh_bool_true);
    EXPECT_EQ(lh_os_fs_is(lh_addr_of(dir), lh_os_fs_kind_dir), lh_bool_true);
    EXPECT_EQ(lh_os_fs_is_file(lh_addr_of(dir)), lh_bool_false);
    EXPECT_EQ(lh_os_fs_is(lh_addr_of(dir), lh_os_fs_kind_file), lh_bool_false);
    EXPECT_EQ(lh_os_fs_is_symlink(lh_addr_of(dir)), lh_bool_false);
    EXPECT_EQ(lh_os_fs_is_shortcut(lh_addr_of(dir)), lh_bool_false);
}

TEST(os_fs_path_is, missing_is_false)
{
    lh_os_fs_path_t path;

    path_set_lit(lh_addr_of(path), "lh_os_fs_path_missing_no_such_file");
    EXPECT_EQ(lh_os_fs_is_file(lh_addr_of(path)), lh_bool_false);
    EXPECT_EQ(lh_os_fs_is_dir(lh_addr_of(path)), lh_bool_false);
    EXPECT_EQ(lh_os_fs_is_symlink(lh_addr_of(path)), lh_bool_false);
    EXPECT_EQ(lh_os_fs_is_shortcut(lh_addr_of(path)), lh_bool_false);
    EXPECT_EQ(lh_os_fs_is(lh_addr_of(path), lh_os_fs_kind_file), lh_bool_false);
    EXPECT_EQ(lh_os_fs_is(lh_addr_of(path), 255U), lh_bool_false);
    EXPECT_NE(lh_os_get_last_error_code(), 0);
}

TEST(os_fs_path_is, shortcut_magic_and_not_plain_file)
{
    lh_os_fs_path_t dir;
    lh_os_fs_path_t name;
    lh_os_fs_path_t path;
    unsigned char magic[20] = {0x4C, 0x00, 0x00, 0x00, 0x01, 0x14, 0x02, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46};
    char plain[] = "not-a-shortcut";
    lh_os_fs_file_t file;

    lh_os_fs_path_init(lh_addr_of(dir));
    lh_os_fs_path_init(lh_addr_of(path));
    path_set_lit(lh_addr_of(name), "lh_os_fs_path_shortcut.lnk");
    ASSERT_EQ(lh_os_fs_path_exe_dir(lh_addr_of(dir)), lh_bool_true);
    ASSERT_EQ(lh_os_fs_path_join(lh_addr_of(path), lh_addr_of(dir), lh_addr_of(name)),
              lh_bool_true);
    (void)lh_os_fs_remove(lh_addr_of(path));

    lh_os_fs_file_init(&file);
    ASSERT_EQ(lh_os_fs_file_open(&file, lh_addr_of(path), lh_os_fs_file_mode_write), lh_bool_true);
    ASSERT_EQ(lh_os_fs_file_write(&file, magic, sizeof(magic)),
              static_cast<lh_ssize_t>(sizeof(magic)));
    lh_os_fs_file_deinit(&file);

    EXPECT_EQ(lh_os_fs_is_file(lh_addr_of(path)), lh_bool_true);
    EXPECT_EQ(lh_os_fs_is(lh_addr_of(path), lh_os_fs_kind_file), lh_bool_true);
    EXPECT_EQ(lh_os_fs_is_shortcut(lh_addr_of(path)), lh_bool_true);
    EXPECT_EQ(lh_os_fs_is(lh_addr_of(path), lh_os_fs_kind_shortcut), lh_bool_true);
    EXPECT_EQ(lh_os_fs_is_symlink(lh_addr_of(path)), lh_bool_false);
    EXPECT_EQ(lh_os_fs_is_dir(lh_addr_of(path)), lh_bool_false);
    EXPECT_EQ(lh_os_fs_remove(lh_addr_of(path)), lh_bool_true);

    lh_os_fs_file_init(&file);
    ASSERT_EQ(lh_os_fs_file_open(&file, lh_addr_of(path), lh_os_fs_file_mode_write), lh_bool_true);
    ASSERT_EQ(lh_os_fs_file_write(&file, plain, sizeof(plain) - 1U),
              static_cast<lh_ssize_t>(sizeof(plain) - 1U));
    lh_os_fs_file_deinit(&file);

    EXPECT_EQ(lh_os_fs_is_file(lh_addr_of(path)), lh_bool_true);
    EXPECT_EQ(lh_os_fs_is_shortcut(lh_addr_of(path)), lh_bool_false);
    EXPECT_EQ(lh_os_fs_remove(lh_addr_of(path)), lh_bool_true);
}

#if !defined(_WIN32)
TEST(os_fs_path_is, posix_symlink_to_file)
{
    lh_os_fs_path_t exe;
    lh_os_fs_path_t dir;
    lh_os_fs_path_t name;
    lh_os_fs_path_t link_path;

    lh_os_fs_path_init(lh_addr_of(exe));
    lh_os_fs_path_init(lh_addr_of(dir));
    lh_os_fs_path_init(lh_addr_of(link_path));
    path_set_lit(lh_addr_of(name), "lh_os_fs_path_symlink");
    ASSERT_EQ(lh_os_fs_path_exe(lh_addr_of(exe)), lh_bool_true);
    ASSERT_EQ(lh_os_fs_path_exe_dir(lh_addr_of(dir)), lh_bool_true);
    ASSERT_EQ(lh_os_fs_path_join(lh_addr_of(link_path), lh_addr_of(dir), lh_addr_of(name)),
              lh_bool_true);
    (void)lh_os_fs_remove(lh_addr_of(link_path));
    if (symlink(path_cstr(lh_addr_of(exe)), path_cstr(lh_addr_of(link_path))) != 0)
    {
        return;
    }
    EXPECT_EQ(lh_os_fs_is_symlink(lh_addr_of(link_path)), lh_bool_true);
    EXPECT_EQ(lh_os_fs_is(lh_addr_of(link_path), lh_os_fs_kind_symlink), lh_bool_true);
    EXPECT_EQ(lh_os_fs_is_file(lh_addr_of(link_path)), lh_bool_true);
    EXPECT_EQ(lh_os_fs_is(lh_addr_of(link_path), lh_os_fs_kind_file), lh_bool_true);
    EXPECT_EQ(lh_os_fs_is_dir(lh_addr_of(link_path)), lh_bool_false);
    EXPECT_EQ(lh_os_fs_is_shortcut(lh_addr_of(link_path)), lh_bool_false);
    EXPECT_EQ(lh_os_fs_remove(lh_addr_of(link_path)), lh_bool_true);
}
#endif

TEST(os_fs_path_parts, parse_glues_with_os_sep)
{
    lh_os_fs_path_t path;
    lh_str_view_t part0;
    lh_str_view_t part1;
    lh_str_view_t part2;

    path_set_lit(lh_addr_of(path), "a/b/c");
    ASSERT_EQ(lh_vector_get_size(lh_os_fs_path_get_parts_as_const(lh_addr_of(path))), 3U);
    part0 = lh_os_fs_path_get_part(lh_addr_of(path), 0U);
    part1 = lh_os_fs_path_get_part(lh_addr_of(path), 1U);
    part2 = lh_os_fs_path_get_part(lh_addr_of(path), 2U);
    EXPECT_EQ(lh_str_view_get_size(lh_addr_of(part0)), 1U);
    EXPECT_EQ(lh_ptr_deref(lh_str_view_get_data(lh_addr_of(part0))), 'a');
    EXPECT_EQ(lh_ptr_deref(lh_str_view_get_data(lh_addr_of(part1))), 'b');
    EXPECT_EQ(lh_ptr_deref(lh_str_view_get_data(lh_addr_of(part2))), 'c');
#if defined(_WIN32)
    EXPECT_STREQ(path_cstr(lh_addr_of(path)), "a\\b\\c");
#else
    EXPECT_STREQ(path_cstr(lh_addr_of(path)), "a/b/c");
#endif
    EXPECT_EQ(lh_ptr_deref(lh_os_fs_path_get_sep_as_const(lh_addr_of(path))), lh_os_fs_path_sep());
}

TEST(os_fs_path_access, readable_writable_like_php)
{
    lh_os_fs_path_t dir;
    lh_os_fs_path_t name;
    lh_os_fs_path_t path;
    lh_os_fs_file_t file;
    char payload[] = "rw";

    lh_os_fs_path_init(lh_addr_of(dir));
    lh_os_fs_path_init(lh_addr_of(name));
    lh_os_fs_path_init(lh_addr_of(path));
    ASSERT_EQ(lh_os_fs_path_exe_dir(lh_addr_of(dir)), lh_bool_true);
    ASSERT_EQ(lh_os_fs_path_set(lh_addr_of(name), lh_str_view_make("lh_os_fs_access.bin")),
              lh_bool_true);
    ASSERT_EQ(lh_os_fs_path_join(lh_addr_of(path), lh_addr_of(dir), lh_addr_of(name)), lh_bool_true);
    (void)lh_os_fs_remove(lh_addr_of(path));

    EXPECT_EQ(lh_os_fs_is_readable(lh_addr_of(path)), lh_bool_false);
    EXPECT_EQ(lh_os_fs_is_writable(lh_addr_of(path)), lh_bool_false);

    lh_os_fs_file_init(lh_addr_of(file));
    ASSERT_EQ(lh_os_fs_file_open(lh_addr_of(file), lh_addr_of(path), lh_os_fs_file_mode_write),
              lh_bool_true);
    ASSERT_EQ(lh_os_fs_file_write(lh_addr_of(file), payload, sizeof(payload) - 1U),
              static_cast<lh_ssize_t>(sizeof(payload) - 1U));
    lh_os_fs_file_deinit(lh_addr_of(file));

    EXPECT_EQ(lh_os_fs_is_readable(lh_addr_of(path)), lh_bool_true);
    EXPECT_EQ(lh_os_fs_is_writable(lh_addr_of(path)), lh_bool_true);
    EXPECT_EQ(lh_os_fs_is_writable(lh_addr_of(dir)), lh_bool_true);

#if defined(_WIN32)
    ASSERT_NE(SetFileAttributesA(path_cstr(lh_addr_of(path)), FILE_ATTRIBUTE_READONLY), 0);
    EXPECT_EQ(lh_os_fs_is_readable(lh_addr_of(path)), lh_bool_true);
    EXPECT_EQ(lh_os_fs_is_writable(lh_addr_of(path)), lh_bool_false);
    ASSERT_NE(SetFileAttributesA(path_cstr(lh_addr_of(path)), FILE_ATTRIBUTE_NORMAL), 0);
#else
    ASSERT_EQ(chmod(path_cstr(lh_addr_of(path)), 0444), 0);
    EXPECT_EQ(lh_os_fs_is_readable(lh_addr_of(path)), lh_bool_true);
    EXPECT_EQ(lh_os_fs_is_writable(lh_addr_of(path)), lh_bool_false);
    ASSERT_EQ(chmod(path_cstr(lh_addr_of(path)), 0644), 0);
#endif

    EXPECT_EQ(lh_os_fs_remove(lh_addr_of(path)), lh_bool_true);
}

} // namespace
