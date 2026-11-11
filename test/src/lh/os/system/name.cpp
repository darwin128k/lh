#include <gtest/gtest.h>

#include <lh/os/system/name.h>

namespace
{

TEST(os_system_get_name, names_the_system_built_for)
{
    lh_str_view_t name;
    lh_os_system_get_name(&name);
#if defined(_WIN32)
    const lh_str_view_t want = lh_str_view_lit("win32");
#elif defined(__linux__)
    const lh_str_view_t want = lh_str_view_lit("linux");
#else
    const lh_str_view_t want = name;
#endif
    EXPECT_TRUE(lh_str_view_equals(&name, &want, lh_bool_false));
}

} // namespace
