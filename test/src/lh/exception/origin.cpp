#include <gtest/gtest.h>

#include <lh/exception/origin.h>

#include <string>

namespace
{

TEST(exception_origin, getters_read_filled_origin)
{
    static const lh_exception_origin_t origin = {"file.c", "fn", "x > 0",
                                                 "x must be positive", 42u};

    EXPECT_EQ(std::string(lh_exception_origin_get_file(&origin)), "file.c");
    EXPECT_EQ(std::string(lh_exception_origin_get_function(&origin)), "fn");
    EXPECT_EQ(std::string(lh_exception_origin_get_condition(&origin)), "x > 0");
    EXPECT_EQ(std::string(lh_exception_origin_get_message(&origin)), "x must be positive");
    EXPECT_EQ(lh_exception_origin_get_line(&origin), 42u);
}

TEST(exception_origin, getters_return_stored_pointers)
{
    static const lh_exception_origin_t origin = {"file.c", "fn", "x > 0",
                                                 "x must be positive", 42u};

    EXPECT_EQ(lh_exception_origin_get_file(&origin), origin.file);
    EXPECT_EQ(lh_exception_origin_get_function(&origin), origin.function);
    EXPECT_EQ(lh_exception_origin_get_condition(&origin), origin.condition);
    EXPECT_EQ(lh_exception_origin_get_message(&origin), origin.message);
}

TEST(exception_origin, has_is_true_on_filled_origin)
{
    static const lh_exception_origin_t origin = {"file.c", "fn", "x > 0",
                                                 "x must be positive", 42u};

    EXPECT_TRUE(lh_exception_origin_has_file(&origin));
    EXPECT_TRUE(lh_exception_origin_has_function(&origin));
    EXPECT_TRUE(lh_exception_origin_has_condition(&origin));
    EXPECT_TRUE(lh_exception_origin_has_message(&origin));
}

TEST(exception_origin, getters_read_null_origin)
{
    static const lh_exception_origin_t origin = {nullptr, nullptr, nullptr, nullptr, 0u};

    EXPECT_EQ(lh_exception_origin_get_file(&origin), nullptr);
    EXPECT_EQ(lh_exception_origin_get_function(&origin), nullptr);
    EXPECT_EQ(lh_exception_origin_get_condition(&origin), nullptr);
    EXPECT_EQ(lh_exception_origin_get_message(&origin), nullptr);
    EXPECT_EQ(lh_exception_origin_get_line(&origin), 0u);
}

TEST(exception_origin, has_is_false_on_null_origin)
{
    static const lh_exception_origin_t origin = {nullptr, nullptr, nullptr, nullptr, 0u};

    EXPECT_FALSE(lh_exception_origin_has_file(&origin));
    EXPECT_FALSE(lh_exception_origin_has_function(&origin));
    EXPECT_FALSE(lh_exception_origin_has_condition(&origin));
    EXPECT_FALSE(lh_exception_origin_has_message(&origin));
}

TEST(exception_origin, has_is_false_on_empty_text)
{
    static const lh_exception_origin_t origin = {"", "", "", "", 7u};

    EXPECT_FALSE(lh_exception_origin_has_file(&origin));
    EXPECT_FALSE(lh_exception_origin_has_function(&origin));
    EXPECT_FALSE(lh_exception_origin_has_condition(&origin));
    EXPECT_FALSE(lh_exception_origin_has_message(&origin));
    EXPECT_EQ(lh_exception_origin_get_line(&origin), 7u);
}

TEST(exception_origin, location_level_origin)
{
    static const lh_exception_origin_t origin = {"a/b.c", nullptr, nullptr, nullptr, 7u};

    EXPECT_TRUE(lh_exception_origin_has_file(&origin));
    EXPECT_FALSE(lh_exception_origin_has_function(&origin));
    EXPECT_FALSE(lh_exception_origin_has_condition(&origin));
    EXPECT_FALSE(lh_exception_origin_has_message(&origin));
    EXPECT_EQ(std::string(lh_exception_origin_get_file(&origin)), "a/b.c");
    EXPECT_EQ(lh_exception_origin_get_line(&origin), 7u);
}

} // namespace
