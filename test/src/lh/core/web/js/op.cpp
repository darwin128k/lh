/**
 * @file op.cpp
 * @brief ::lh_web_js_op_t: each operator's text and precedence.
 */

#include <gtest/gtest.h>

#include <lh/core/web/js/op.h>

#include <string>

namespace
{

std::string
text(lh_web_js_op_t op)
{
    lh_str_view_t view = lh_web_js_op_get_text(op);

    return std::string(lh_str_view_get_data(&view) ? lh_str_view_get_data(&view) : "",
                       lh_str_view_get_size(&view));
}

} // namespace

TEST(web_js_op, spells_every_operator)
{
    EXPECT_EQ(text(lh_web_js_op_none), "");
    EXPECT_EQ(text(lh_web_js_op_not), "!");
    EXPECT_EQ(text(lh_web_js_op_typeof), "typeof");
    EXPECT_EQ(text(lh_web_js_op_increment), "++");
    EXPECT_EQ(text(lh_web_js_op_add), "+");
    EXPECT_EQ(text(lh_web_js_op_ushr), ">>>");
    EXPECT_EQ(text(lh_web_js_op_instanceof), "instanceof");
    EXPECT_EQ(text(lh_web_js_op_strict_ne), "!==");
    EXPECT_EQ(text(lh_web_js_op_or), "||");
    EXPECT_EQ(text(lh_web_js_op_assign), "=");
    EXPECT_EQ(text(lh_web_js_op_bit_or_assign), "|=");
    EXPECT_EQ(text(LH_WEB_JS_OP_COUNT), "");
}

TEST(web_js_op, ranks_operators_by_precedence)
{
    EXPECT_EQ(lh_web_js_op_get_precedence(lh_web_js_op_mul), lh_web_js_precedence_multiplicative);
    EXPECT_EQ(lh_web_js_op_get_precedence(lh_web_js_op_add), lh_web_js_precedence_additive);
    EXPECT_EQ(lh_web_js_op_get_precedence(lh_web_js_op_shl), lh_web_js_precedence_shift);
    EXPECT_EQ(lh_web_js_op_get_precedence(lh_web_js_op_in), lh_web_js_precedence_relational);
    EXPECT_EQ(lh_web_js_op_get_precedence(lh_web_js_op_strict_eq), lh_web_js_precedence_equality);
    EXPECT_EQ(lh_web_js_op_get_precedence(lh_web_js_op_bit_and), lh_web_js_precedence_bit_and);
    EXPECT_EQ(lh_web_js_op_get_precedence(lh_web_js_op_and), lh_web_js_precedence_and);
    EXPECT_EQ(lh_web_js_op_get_precedence(lh_web_js_op_or), lh_web_js_precedence_or);
    EXPECT_EQ(lh_web_js_op_get_precedence(lh_web_js_op_add_assign), lh_web_js_precedence_assign);
    EXPECT_EQ(lh_web_js_op_get_precedence(lh_web_js_op_not), lh_web_js_precedence_unary);
    EXPECT_LT(lh_web_js_op_get_precedence(lh_web_js_op_add),
              lh_web_js_op_get_precedence(lh_web_js_op_mul));
}

TEST(web_js_op, words_need_spaces)
{
    EXPECT_TRUE(lh_web_js_op_is_word(lh_web_js_op_typeof));
    EXPECT_TRUE(lh_web_js_op_is_word(lh_web_js_op_in));
    EXPECT_TRUE(lh_web_js_op_is_word(lh_web_js_op_instanceof));
    EXPECT_FALSE(lh_web_js_op_is_word(lh_web_js_op_add));
    EXPECT_FALSE(lh_web_js_op_is_word(lh_web_js_op_none));
}
