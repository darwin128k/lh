#include <gtest/gtest.h>

#include <lh/os.h>
#include <lh/os/error/code.h>

/* A failing check stores its error in the slot and returns false in one call. */
TEST(os_fail, stores_the_error_and_returns_false)
{
    lh_os_set_last_error_lit(lh_os_error_code_ok, "");
    EXPECT_FALSE(lh_os_fail_lit(lh_os_error_code_path_empty, "path is empty"));
    EXPECT_EQ(lh_os_get_last_error_code(), lh_os_error_code_path_empty);
    EXPECT_FALSE(lh_os_error_desc_is_empty(lh_os_get_last_error_desc()));
}

TEST(os_fail, takes_a_whole_error)
{
    const lh_os_error_t error = lh_os_error_initializer(lh_os_error_code_invalid_argument,
                                                        lh_os_error_desc_lit("invalid state"));

    EXPECT_FALSE(lh_os_fail(error));
    EXPECT_EQ(lh_os_get_last_error_code(), lh_os_error_code_invalid_argument);
}
