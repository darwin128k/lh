#include <gtest/gtest.h>

#include <lh/os.h>
#include <lh/os/fs/path.h>
#include <lh/str.h>
#include <lh/str/view.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>
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

TEST(os_fs_path_span, table_entry_slices_text)
{
    lh_os_fs_path_t path;
    const lh_os_fs_path_span_t *span;
    lh_str_view_t part;

    path_set_lit(lh_addr_of(path), "a/b/c");
    span = lh_os_fs_path_get_span_as_const(lh_addr_of(path), 1U);
    EXPECT_EQ(lh_os_fs_path_span_get_offset(span), 2U);
    EXPECT_EQ(lh_os_fs_path_span_get_size(span), 1U);
    EXPECT_EQ(lh_os_fs_path_span_is_empty(span), lh_bool_false);
    part = lh_os_fs_path_span_as_view(span, lh_os_fs_path_as_view(lh_addr_of(path)));
    EXPECT_EQ(lh_str_view_get_size(lh_addr_of(part)), 1U);
    EXPECT_EQ(lh_ptr_deref(lh_str_view_get_data(lh_addr_of(part))), 'b');
}

TEST(os_fs_path_is_sep, matches_os)
{
    EXPECT_EQ(lh_os_fs_path_is_sep('/'), lh_bool_true);
#if defined(_WIN32)
    EXPECT_EQ(lh_os_fs_path_is_sep('\\'), lh_bool_true);
#else
    EXPECT_EQ(lh_os_fs_path_is_sep('\\'), lh_bool_false);
#endif
    EXPECT_EQ(lh_os_fs_path_is_sep('a'), lh_bool_false);
}

} // namespace
