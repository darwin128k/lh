#include <gtest/gtest.h>

#include <lh/null.h>
#include <lh/os/fs/path.h>
#include <lh/os/shared.h>
#include <lh/str/view.h>
#include <lh/str/view/initializer.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

namespace
{

char in_image;

static lh_str_view_t
view_lit(const char *text)
{
    return lh_str_view_make(text);
}

} // namespace

TEST(os_shared_ext, matches_os)
{
#if defined(_WIN32)
    EXPECT_STREQ(lh_os_shared_ext(), ".dll");
#else
    EXPECT_STREQ(lh_os_shared_ext(), ".so");
#endif
}

TEST(os_shared_is, suffix)
{
    lh_str_view_t empty = lh_str_view_empty();
    lh_str_view_t foo = view_lit("foo.txt");
    lh_str_view_t dll = view_lit("mod.dll");
    lh_str_view_t DLL = view_lit("mod.DLL");
    lh_str_view_t so = view_lit("mod.so");

    EXPECT_EQ(lh_os_shared_is(lh_ptr_cast(const lh_str_view_t, lh_null)), lh_bool_false);
    EXPECT_EQ(lh_os_shared_is(lh_addr_of(empty)), lh_bool_false);
    EXPECT_EQ(lh_os_shared_is(lh_addr_of(foo)), lh_bool_false);
#if defined(_WIN32)
    EXPECT_EQ(lh_os_shared_is(lh_addr_of(dll)), lh_bool_true);
    EXPECT_EQ(lh_os_shared_is(lh_addr_of(DLL)), lh_bool_true);
    EXPECT_EQ(lh_os_shared_is(lh_addr_of(so)), lh_bool_false);
#else
    EXPECT_EQ(lh_os_shared_is(lh_addr_of(so)), lh_bool_true);
    EXPECT_EQ(lh_os_shared_is(lh_addr_of(dll)), lh_bool_false);
#endif
}

TEST(os_shared_get_path_of, this_image)
{
    lh_os_shared_handle_t handle;
    lh_os_fs_path_t path;

    lh_os_fs_path_init(lh_addr_of(path));
    handle = lh_os_shared_of_addr(lh_addr_of(in_image));
    ASSERT_TRUE(lh_null_ne(handle));
    ASSERT_EQ(lh_os_shared_get_path_of(handle, lh_addr_of(path)), lh_bool_true);
    EXPECT_EQ(lh_os_fs_path_is_empty(lh_addr_of(path)), lh_bool_false);
#if !defined(_WIN32)
    EXPECT_EQ(lh_os_shared_close(handle), lh_bool_true);
#endif
}

TEST(os_shared_path_is, rejects_exe_and_missing)
{
    lh_os_fs_path_t exe;
    lh_os_fs_path_t missing_dll;
    lh_os_fs_path_t missing_so;

    lh_os_fs_path_init(lh_addr_of(exe));
    lh_os_fs_path_init(lh_addr_of(missing_dll));
    lh_os_fs_path_init(lh_addr_of(missing_so));
    ASSERT_EQ(lh_os_fs_path_exe(lh_addr_of(exe)), lh_bool_true);
    EXPECT_EQ(lh_os_shared_path_is(lh_addr_of(exe)), lh_bool_false);
    ASSERT_EQ(lh_os_fs_path_set(lh_addr_of(missing_dll), lh_str_view_make("no-such-module.dll")),
              lh_bool_true);
    ASSERT_EQ(lh_os_fs_path_set(lh_addr_of(missing_so), lh_str_view_make("no-such-module.so")),
              lh_bool_true);
    EXPECT_EQ(lh_os_shared_path_is(lh_addr_of(missing_dll)), lh_bool_false);
    EXPECT_EQ(lh_os_shared_path_is(lh_addr_of(missing_so)), lh_bool_false);
}
