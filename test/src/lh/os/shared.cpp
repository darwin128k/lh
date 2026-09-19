#include <gtest/gtest.h>

#include <lh/null.h>
#include <lh/os/fs/path.h>
#include <lh/os/shared.h>
#include <lh/util/ptr.h>

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
    EXPECT_EQ(lh_os_shared_is(lh_ptr_cast(const lh_char_t, lh_null)), lh_bool_false);
    EXPECT_EQ(lh_os_shared_is(""), lh_bool_false);
    EXPECT_EQ(lh_os_shared_is("foo.txt"), lh_bool_false);
#if defined(_WIN32)
    EXPECT_EQ(lh_os_shared_is("mod.dll"), lh_bool_true);
    EXPECT_EQ(lh_os_shared_is("mod.DLL"), lh_bool_true);
    EXPECT_EQ(lh_os_shared_is("mod.so"), lh_bool_false);
#else
    EXPECT_EQ(lh_os_shared_is("mod.so"), lh_bool_true);
    EXPECT_EQ(lh_os_shared_is("mod.dll"), lh_bool_false);
#endif
}

TEST(os_shared_path_is, rejects_exe_and_missing)
{
    char exe[4096];

    ASSERT_EQ(lh_os_fs_path_exe(exe, sizeof(exe)), lh_bool_true);
    EXPECT_EQ(lh_os_shared_path_is(exe), lh_bool_false);
    EXPECT_EQ(lh_os_shared_path_is("no-such-module.dll"), lh_bool_false);
    EXPECT_EQ(lh_os_shared_path_is("no-such-module.so"), lh_bool_false);
}
