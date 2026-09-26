#include <gtest/gtest.h>

#include <lh/exception.h>
#include <lh/exception/initializer.h>
#include <lh/runtime/error/initializer.h>
#include <lh/str/view.h>
#include <lh/str/view/initializer.h>

#include <string>

namespace
{

std::string
desc_of(const lh_exception_t *exception)
{
    lh_str_view_t desc = lh_runtime_error_get_desc(lh_exception_get_error(exception));
    if (lh_str_view_is_empty(&desc))
    {
        return std::string();
    }
    return std::string(lh_str_view_get_data(&desc), lh_str_view_get_size(&desc));
}

TEST(exception, getters_read_error_and_site)
{
    static const lh_exception_origin_t origin = {"file.c", "fn", "x > 0",
                                                 lh_str_view_empty_initializer(), 42u};
    const lh_exception_t exception = lh_exception_initializer(
        lh_runtime_error_initializer(lh_runtime_error_code_invalid_argument,
                                     lh_str_view_initializer_lit("x must be positive")),
        &origin);

    EXPECT_EQ(lh_exception_get_code(&exception), lh_runtime_error_code_invalid_argument);
    EXPECT_EQ(lh_runtime_error_get_code(lh_exception_get_error(&exception)),
              lh_runtime_error_code_invalid_argument);
    EXPECT_EQ(desc_of(&exception), "x must be positive");
    EXPECT_EQ(lh_exception_get_origin(&exception), &origin);
    EXPECT_EQ(lh_exception_get_origin(&exception)->line, 42u);
}

TEST(exception, empty_message_and_no_site)
{
    const lh_exception_t exception =
        lh_exception_initializer(lh_runtime_error_initializer(lh_runtime_error_code_overflow,
                                                              lh_str_view_empty_initializer()),
                                 nullptr);

    EXPECT_EQ(lh_exception_get_code(&exception), lh_runtime_error_code_overflow);
    EXPECT_EQ(desc_of(&exception), "");
    EXPECT_EQ(lh_exception_get_origin(&exception), nullptr);
}

} // namespace
