/**
 * @file lexer.c
 * @brief The JavaScript lexer: source text in, tokens out.
 */

#include <lh/core/web/js/lexer.h>

#include <lh/core/assert.h>
#include <lh/core/char/space.h>
#include <lh/core/char/xdigit.h>
#include <lh/core/math.h>
#include <lh/core/str/parse/hex.h>
#include <lh/core/str/parse/radix.h>
#include <lh/core/str/parse/real.h>

#include "lexer/private.h"

/* ── the tables ───────────────────────────────────────────────────────────── */

/** The keywords of JavaScript: a name that is one of them is a keyword. */
static const lh_str_view_t LH_WEB_JS_KEYWORDS[] = {
    /* clang-format off */
    lh_str_view_initializer_lit("await"),
    lh_str_view_initializer_lit("break"),
    lh_str_view_initializer_lit("case"),
    lh_str_view_initializer_lit("catch"),
    lh_str_view_initializer_lit("class"),
    lh_str_view_initializer_lit("const"),
    lh_str_view_initializer_lit("continue"),
    lh_str_view_initializer_lit("debugger"),
    lh_str_view_initializer_lit("default"),
    lh_str_view_initializer_lit("delete"),
    lh_str_view_initializer_lit("do"),
    lh_str_view_initializer_lit("else"),
    lh_str_view_initializer_lit("enum"),
    lh_str_view_initializer_lit("export"),
    lh_str_view_initializer_lit("extends"),
    lh_str_view_initializer_lit("false"),
    lh_str_view_initializer_lit("finally"),
    lh_str_view_initializer_lit("for"),
    lh_str_view_initializer_lit("function"),
    lh_str_view_initializer_lit("if"),
    lh_str_view_initializer_lit("import"),
    lh_str_view_initializer_lit("in"),
    lh_str_view_initializer_lit("instanceof"),
    lh_str_view_initializer_lit("new"),
    lh_str_view_initializer_lit("null"),
    lh_str_view_initializer_lit("return"),
    lh_str_view_initializer_lit("super"),
    lh_str_view_initializer_lit("switch"),
    lh_str_view_initializer_lit("this"),
    lh_str_view_initializer_lit("throw"),
    lh_str_view_initializer_lit("true"),
    lh_str_view_initializer_lit("try"),
    lh_str_view_initializer_lit("typeof"),
    lh_str_view_initializer_lit("var"),
    lh_str_view_initializer_lit("void"),
    lh_str_view_initializer_lit("while"),
    lh_str_view_initializer_lit("with"),
    lh_str_view_initializer_lit("yield"),
    /* clang-format on */
};

/** The operators and the brackets of JavaScript, the longest first. */
static const lh_str_view_t LH_WEB_JS_PUNCTUATION[] = {
    /* clang-format off */
    lh_str_view_initializer_lit(">>>="),
    lh_str_view_initializer_lit("<<="),
    lh_str_view_initializer_lit(">>="),
    lh_str_view_initializer_lit("&&="),
    lh_str_view_initializer_lit("||="),
    lh_str_view_initializer_lit("?" "?="),
    lh_str_view_initializer_lit("==="),
    lh_str_view_initializer_lit("!=="),
    lh_str_view_initializer_lit("**="),
    lh_str_view_initializer_lit(">>>"),
    lh_str_view_initializer_lit("..."),
    lh_str_view_initializer_lit("**"),
    lh_str_view_initializer_lit("++"),
    lh_str_view_initializer_lit("--"),
    lh_str_view_initializer_lit("&="),
    lh_str_view_initializer_lit("|="),
    lh_str_view_initializer_lit("^="),
    lh_str_view_initializer_lit("+="),
    lh_str_view_initializer_lit("-="),
    lh_str_view_initializer_lit("*="),
    lh_str_view_initializer_lit("/="),
    lh_str_view_initializer_lit("%="),
    lh_str_view_initializer_lit("=="),
    lh_str_view_initializer_lit("!="),
    lh_str_view_initializer_lit("<="),
    lh_str_view_initializer_lit(">="),
    lh_str_view_initializer_lit("&&"),
    lh_str_view_initializer_lit("||"),
    lh_str_view_initializer_lit("??"),
    lh_str_view_initializer_lit("=>"),
    lh_str_view_initializer_lit("<<"),
    lh_str_view_initializer_lit(">>"),
    lh_str_view_initializer_lit("+"),
    lh_str_view_initializer_lit("-"),
    lh_str_view_initializer_lit("*"),
    lh_str_view_initializer_lit("/"),
    lh_str_view_initializer_lit("%"),
    lh_str_view_initializer_lit("="),
    lh_str_view_initializer_lit("<"),
    lh_str_view_initializer_lit(">"),
    lh_str_view_initializer_lit("?"),
    lh_str_view_initializer_lit(":"),
    lh_str_view_initializer_lit(","),
    lh_str_view_initializer_lit(";"),
    lh_str_view_initializer_lit("."),
    lh_str_view_initializer_lit("("),
    lh_str_view_initializer_lit(")"),
    lh_str_view_initializer_lit("["),
    lh_str_view_initializer_lit("]"),
    lh_str_view_initializer_lit("{"),
    lh_str_view_initializer_lit("}"),
    lh_str_view_initializer_lit("&"),
    lh_str_view_initializer_lit("|"),
    lh_str_view_initializer_lit("^"),
    lh_str_view_initializer_lit("~"),
    lh_str_view_initializer_lit("!"),
    /* clang-format on */
};

/** The prefixes of the numbers that are not decimal. */
static const lh_char_t LH_WEB_JS_RADIX_PREFIX[] = "xXoObB";

/** The radix of each prefix of ::LH_WEB_JS_RADIX_PREFIX. */
static const lh_uint_t LH_WEB_JS_RADIX[] = {16U, 16U, 8U, 8U, 2U, 2U};

/** The characters a backslash may escape inside a string. */
static const lh_char_t LH_WEB_JS_ESCAPE_CHARS[] = "\\'\"nrtbfv0";

/** The message of a string that ends before its closing quote. */
static const lh_char_t LH_WEB_JS_ERROR_UNTERMINATED_STRING[] = "unterminated string";

/** The message of a block comment that never closes. */
static const lh_char_t LH_WEB_JS_ERROR_UNTERMINATED_COMMENT[] = "unterminated block comment";

/** The message of an escape sequence that is not one. */
static const lh_char_t LH_WEB_JS_ERROR_BAD_ESCAPE[] = "bad escape";

/** The message of a number that breaks the grammar. */
static const lh_char_t LH_WEB_JS_ERROR_BAD_NUMBER[] = "bad number";

/** The message of a character that starts no token. */
static const lh_char_t LH_WEB_JS_ERROR_UNEXPECTED_CHAR[] = "unexpected character";

/** The highest code point Unicode has. */
#define LH_WEB_JS_CODE_POINT_MAX 0x10FFFFU

/* ── level 1: the accessors ───────────────────────────────────────────────── */

const lh_str_cursor_t *
lh_web_js_lexer_get_cursor(const lh_web_js_lexer_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->cursor);
}

lh_str_cursor_t *
lh_web_js_lexer_get_cursor_mut(lh_web_js_lexer_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->cursor);
}

const lh_str_ptr
lh_web_js_lexer_get_text(const lh_web_js_lexer_t *self)
{
    return lh_str_view_get_data(lh_str_cursor_get_text(lh_web_js_lexer_get_cursor(self)));
}

lh_void
lh_web_js_lexer_set_text(lh_web_js_lexer_t *self, const lh_str_ptr text)
{
    lh_str_cursor_t *cursor = lh_web_js_lexer_get_cursor_mut(self);
    lh_str_view_t view = *lh_str_cursor_get_text(cursor);
    lh_memory_view_set_begin(lh_addr_of(view), text);
    lh_str_cursor_set_text(cursor, view);
}

lh_usize_t
lh_web_js_lexer_get_text_len(const lh_web_js_lexer_t *self)
{
    return lh_str_view_get_size(lh_str_cursor_get_text(lh_web_js_lexer_get_cursor(self)));
}

lh_void
lh_web_js_lexer_set_text_len(lh_web_js_lexer_t *self, lh_usize_t len)
{
    lh_str_cursor_t *cursor = lh_web_js_lexer_get_cursor_mut(self);
    lh_str_view_t view = *lh_str_cursor_get_text(cursor);
    lh_memory_view_set_size(lh_addr_of(view), len);
    lh_str_cursor_set_text(cursor, view);
}

lh_usize_t
lh_web_js_lexer_get_pos(const lh_web_js_lexer_t *self)
{
    return lh_str_cursor_get_pos(lh_web_js_lexer_get_cursor(self));
}

lh_void
lh_web_js_lexer_set_pos(lh_web_js_lexer_t *self, lh_usize_t pos)
{
    lh_str_cursor_set_pos(lh_web_js_lexer_get_cursor_mut(self), pos);
}

const lh_web_js_token_t *
lh_web_js_lexer_get_token(const lh_web_js_lexer_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->token);
}

lh_void
lh_web_js_lexer_set_token(lh_web_js_lexer_t *self, const lh_web_js_token_t *token)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(token);
    self->token = *token;
}

lh_usize_t
lh_web_js_lexer_get_line(const lh_web_js_lexer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->line;
}

lh_void
lh_web_js_lexer_set_line(lh_web_js_lexer_t *self, lh_usize_t line)
{
    lh_assert_runtime_ref(self);
    self->line = line;
}

lh_usize_t
lh_web_js_lexer_get_column(const lh_web_js_lexer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->column;
}

lh_void
lh_web_js_lexer_set_column(lh_web_js_lexer_t *self, lh_usize_t column)
{
    lh_assert_runtime_ref(self);
    self->column = column;
}

lh_str_ptr
lh_web_js_lexer_get_error_message(const lh_web_js_lexer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->error_message;
}

lh_void
lh_web_js_lexer_set_error_message(lh_web_js_lexer_t *self, lh_str_ptr message)
{
    lh_assert_runtime_ref(self);
    self->error_message = message;
}

lh_usize_t
lh_web_js_lexer_get_error_message_len(const lh_web_js_lexer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->error_message_len;
}

lh_void
lh_web_js_lexer_set_error_message_len(lh_web_js_lexer_t *self, lh_usize_t len)
{
    lh_assert_runtime_ref(self);
    self->error_message_len = len;
}

lh_usize_t
lh_web_js_lexer_get_error_line(const lh_web_js_lexer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->error_line;
}

lh_void
lh_web_js_lexer_set_error_line(lh_web_js_lexer_t *self, lh_usize_t line)
{
    lh_assert_runtime_ref(self);
    self->error_line = line;
}

lh_usize_t
lh_web_js_lexer_get_error_column(const lh_web_js_lexer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->error_column;
}

lh_void
lh_web_js_lexer_set_error_column(lh_web_js_lexer_t *self, lh_usize_t column)
{
    lh_assert_runtime_ref(self);
    self->error_column = column;
}

/* ── level 2: what the scanner is made of ─────────────────────────────────── */

lh_web_js_token_t *
lh_web_js_lexer_get_token_mut(lh_web_js_lexer_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->token);
}

lh_str_ptr
lh_web_js_lexer_get_error_buffer_mut(lh_web_js_lexer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->error_buffer;
}

lh_bool_t
lh_web_js_lexer_is_keyword(const lh_str_view_t *text)
{
    lh_usize_t i;

    lh_assert_runtime_ref(text);
    for (i = 0U; i < lh_array_ptr_get_size(LH_WEB_JS_KEYWORDS); ++i)
    {
        if (lh_str_view_equals(text, lh_addr_of(LH_WEB_JS_KEYWORDS[i]), lh_bool_false))
        {
            return lh_bool_true;
        }
    }
    return lh_bool_false;
}

lh_usize_t
lh_web_js_lexer_find_punctuation(const lh_str_view_t *text)
{
    lh_assert_runtime_ref(text);
    const lh_str_view_t optional_chain = lh_str_view_initializer_lit("?.");
    if (lh_str_view_starts_with(text, lh_addr_of(optional_chain), lh_bool_false) &&
        (lh_math_le(lh_str_view_get_size(text), 2U) ||
         !lh_char_is_digit(lh_str_view_get_char_from_begin(text, 2U))))
    {
        return lh_str_view_get_size(lh_addr_of(optional_chain));
    }
    for (lh_usize_t i = 0U; i < lh_array_ptr_get_size(LH_WEB_JS_PUNCTUATION);
         i = lh_math_add_one(i))
    {
        if (lh_str_view_starts_with(text, lh_addr_of(LH_WEB_JS_PUNCTUATION[i]), lh_bool_false))
        {
            return lh_str_view_get_size(lh_addr_of(LH_WEB_JS_PUNCTUATION[i]));
        }
    }
    return 0U;
}

lh_uint_t
lh_web_js_lexer_find_radix(lh_char_t prefix)
{
    const lh_str_ptr found = lh_str_ptr_find_of_char(
        LH_WEB_JS_RADIX_PREFIX, lh_str_ptr_get_size(LH_WEB_JS_RADIX_PREFIX), prefix);

    if (lh_null_eq(found))
    {
        return 0U;
    }
    return LH_WEB_JS_RADIX[found - LH_WEB_JS_RADIX_PREFIX];
}

lh_char_t
lh_web_js_lexer_get_char(const lh_web_js_lexer_t *self)
{
    return lh_str_cursor_peek(lh_web_js_lexer_get_cursor(self), 0U);
}

lh_bool_t
lh_web_js_lexer_try_get_char_at(const lh_web_js_lexer_t *self, lh_usize_t offset, lh_char_t *out)
{
    const lh_str_cursor_t *cursor = lh_web_js_lexer_get_cursor(self);

    lh_assert_runtime_ref(out);
    if (lh_math_ge(offset, lh_str_cursor_get_left(cursor)))
    {
        return lh_bool_false;
    }
    lh_ptr_deref(out) = lh_str_cursor_peek(cursor, offset);
    return lh_bool_true;
}

lh_bool_t
lh_web_js_lexer_is_at_end(const lh_web_js_lexer_t *self)
{
    return lh_str_cursor_is_end(lh_web_js_lexer_get_cursor(self));
}

lh_void
lh_web_js_lexer_get_rest(const lh_web_js_lexer_t *self, lh_str_view_t *out)
{
    lh_str_cursor_get_rest(lh_web_js_lexer_get_cursor(self), out);
}

lh_void
lh_web_js_lexer_init(lh_web_js_lexer_t *self, lh_str_cptr text, lh_usize_t len)
{
    lh_web_js_token_t *token;

    lh_str_view_t source;
    lh_str_view_init_by_size(lh_addr_of(source), text, len);
    lh_str_cursor_init(lh_web_js_lexer_get_cursor_mut(self), source);
    lh_web_js_lexer_set_line(self, 1U);
    lh_web_js_lexer_set_column(self, 1U);
    lh_web_js_lexer_set_error_message(self, lh_null);
    lh_web_js_lexer_set_error_message_len(self, 0U);
    lh_web_js_lexer_set_error_line(self, 0U);
    lh_web_js_lexer_set_error_column(self, 0U);
    token = lh_web_js_lexer_get_token_mut(self);
    lh_web_js_token_set_kind(token, lh_web_js_token_kind_end_of_input);
    lh_web_js_token_set_text(token, text);
    lh_web_js_token_set_text_len(token, 0U);
    lh_web_js_token_set_value(token, 0.0);
}

lh_bool_t
lh_web_js_lexer_get_error(const lh_web_js_lexer_t *self, lh_str_ptr *message, lh_usize_t *len)
{
    lh_usize_t size;

    lh_assert_runtime_ref(message);
    lh_assert_runtime_ref(len);
    size = lh_web_js_lexer_get_error_message_len(self);
    if (lh_math_is_zero(size))
    {
        return lh_bool_false;
    }
    lh_ptr_deref(message) = lh_web_js_lexer_get_error_message(self);
    lh_ptr_deref(len) = size;
    return lh_bool_true;
}

lh_void
lh_web_js_lexer_set_error(lh_web_js_lexer_t *self, lh_str_cptr message, lh_usize_t len)
{
    lh_str_ptr buffer;

    lh_assert_runtime_ref(message);
    buffer = lh_web_js_lexer_get_error_buffer_mut(self);
    const lh_usize_t size =
        lh_str_ptr_copy_cstr(buffer, LH_WEB_JS_LEXER_ERROR_MESSAGE_MAX, message, len);
    lh_web_js_lexer_set_error_message(self, buffer);
    lh_web_js_lexer_set_error_message_len(self, size);
    lh_web_js_lexer_set_error_line(self, lh_web_js_lexer_get_line(self));
    lh_web_js_lexer_set_error_column(self, lh_web_js_lexer_get_column(self));
}

lh_void
lh_web_js_lexer_clear_error(lh_web_js_lexer_t *self)
{
    lh_web_js_lexer_set_error_message(self, lh_null);
    lh_web_js_lexer_set_error_message_len(self, 0U);
    lh_web_js_lexer_set_error_line(self, 0U);
    lh_web_js_lexer_set_error_column(self, 0U);
}

lh_void
lh_web_js_lexer_advance(lh_web_js_lexer_t *self)
{
    lh_char_t ch = lh_web_js_lexer_get_char(self);

    if (ch == lh_char_map_cr)
    {
        lh_web_js_lexer_set_pos(self, lh_math_add_one(lh_web_js_lexer_get_pos(self)));
        lh_web_js_lexer_set_line(self, lh_math_add_one(lh_web_js_lexer_get_line(self)));
        lh_web_js_lexer_set_column(self, 1U);
        if (lh_web_js_lexer_get_char(self) == lh_char_map_lf)
        {
            lh_web_js_lexer_set_pos(self, lh_math_add_one(lh_web_js_lexer_get_pos(self)));
        }
        return;
    }
    if (ch == lh_char_map_lf)
    {
        lh_web_js_lexer_set_pos(self, lh_math_add_one(lh_web_js_lexer_get_pos(self)));
        lh_web_js_lexer_set_line(self, lh_math_add_one(lh_web_js_lexer_get_line(self)));
        lh_web_js_lexer_set_column(self, 1U);
        return;
    }
    lh_web_js_lexer_set_pos(self, lh_math_add_one(lh_web_js_lexer_get_pos(self)));
    lh_web_js_lexer_set_column(self, lh_math_add_one(lh_web_js_lexer_get_column(self)));
}

/* ── level 3: the scanners ────────────────────────────────────────────────── */

lh_bool_t
lh_web_js_lexer_skip_space(lh_web_js_lexer_t *self)
{
    while (!lh_web_js_lexer_is_at_end(self))
    {
        lh_char_t ch = lh_web_js_lexer_get_char(self);
        lh_char_t next = lh_char_map_nul;

        if (ch == lh_char_map_slash && lh_web_js_lexer_try_get_char_at(self, 1U, lh_addr_of(next)))
        {
            if (next == lh_char_map_slash)
            {
                lh_web_js_lexer_advance(self);
                lh_web_js_lexer_advance(self);
                while (!lh_web_js_lexer_is_at_end(self) &&
                       lh_web_js_lexer_get_char(self) != lh_char_map_lf &&
                       lh_web_js_lexer_get_char(self) != lh_char_map_cr)
                {
                    lh_web_js_lexer_advance(self);
                }
                continue;
            }
            if (next == lh_char_map_asterisk)
            {
                lh_bool_t closed = lh_bool_false;

                lh_web_js_lexer_advance(self);
                lh_web_js_lexer_advance(self);
                while (!lh_web_js_lexer_is_at_end(self))
                {
                    if (lh_web_js_lexer_get_char(self) == lh_char_map_asterisk)
                    {
                        lh_char_t after = lh_char_map_nul;

                        if (lh_web_js_lexer_try_get_char_at(self, 1U, lh_addr_of(after)) &&
                            after == lh_char_map_slash)
                        {
                            lh_web_js_lexer_advance(self);
                            lh_web_js_lexer_advance(self);
                            closed = lh_bool_true;
                            break;
                        }
                    }
                    lh_web_js_lexer_advance(self);
                }
                if (closed == lh_bool_false)
                {
                    lh_web_js_lexer_set_error(self, LH_WEB_JS_ERROR_UNTERMINATED_COMMENT,
                                              lh_str_ptr_len(LH_WEB_JS_ERROR_UNTERMINATED_COMMENT));
                    return lh_bool_false;
                }
                continue;
            }
        }
        if (lh_char_is_whitespace(ch) && ch != lh_char_map_lf && ch != lh_char_map_cr)
        {
            lh_web_js_lexer_advance(self);
            continue;
        }
        break;
    }
    return lh_bool_true;
}

lh_bool_t
lh_web_js_lexer_scan_name(lh_web_js_lexer_t *self)
{
    lh_usize_t start = lh_web_js_lexer_get_pos(self);

    while (!lh_web_js_lexer_is_at_end(self) &&
           lh_web_js_lexer_is_name_part(lh_web_js_lexer_get_char(self)))
    {
        lh_web_js_lexer_advance(self);
    }

    lh_str_view_t name;
    lh_str_view_init_by_size(lh_addr_of(name), lh_web_js_lexer_get_text(self) + start,
                             lh_web_js_lexer_get_pos(self) - start);
    lh_web_js_token_t *token = lh_web_js_lexer_get_token_mut(self);
    lh_web_js_token_set_kind(token, lh_web_js_lexer_is_keyword(lh_addr_of(name))
                                        ? lh_web_js_token_kind_keyword
                                        : lh_web_js_token_kind_identifier);
    lh_web_js_token_set_text(token, lh_web_js_lexer_get_text(self) + start);
    lh_web_js_token_set_text_len(token, lh_web_js_lexer_get_pos(self) - start);
    lh_web_js_token_set_value(token, 0.0);
    return lh_bool_true;
}

lh_bool_t
lh_web_js_lexer_scan_radix_number(lh_web_js_lexer_t *self, lh_uint_t radix)
{
    lh_usize_t start = lh_web_js_lexer_get_pos(self);

    lh_web_js_lexer_advance(self);
    lh_web_js_lexer_advance(self);
    lh_usize_t digits_start = lh_web_js_lexer_get_pos(self);
    while (!lh_web_js_lexer_is_at_end(self) &&
           lh_char_is_radix_digit(lh_web_js_lexer_get_char(self), radix))
    {
        lh_web_js_lexer_advance(self);
    }
    lh_usize_t digits = lh_web_js_lexer_get_pos(self) - digits_start;
    lh_ullong_t value = 0;
    if (lh_math_is_zero(digits) ||
        !lh_str_ptr_parse_radix_ullong(lh_web_js_lexer_get_text(self) + digits_start, digits, radix,
                                       LH_ULLONG_T_MAX, lh_addr_of(value)))
    {
        lh_web_js_lexer_set_error(self, LH_WEB_JS_ERROR_BAD_NUMBER,
                                  lh_str_ptr_len(LH_WEB_JS_ERROR_BAD_NUMBER));
        return lh_bool_false;
    }

    lh_web_js_token_t *token = lh_web_js_lexer_get_token_mut(self);
    lh_web_js_token_set_kind(token, lh_web_js_token_kind_number);
    lh_web_js_token_set_text(token, lh_web_js_lexer_get_text(self) + start);
    lh_web_js_token_set_text_len(token, lh_web_js_lexer_get_pos(self) - start);
    lh_web_js_token_set_value(token, lh_cast_static(lh_real_t, value));
    return lh_bool_true;
}

lh_bool_t
lh_web_js_lexer_scan_decimal_number(lh_web_js_lexer_t *self)
{
    lh_usize_t start = lh_web_js_lexer_get_pos(self);
    lh_char_t ch = lh_web_js_lexer_get_char(self);
    lh_real_t value = 0.0;

    if (ch == lh_char_map_period)
    {
        lh_web_js_lexer_advance(self);
        if (lh_web_js_lexer_is_at_end(self) || !lh_char_is_digit(lh_web_js_lexer_get_char(self)))
        {
            lh_web_js_lexer_set_error(self, LH_WEB_JS_ERROR_BAD_NUMBER,
                                      lh_str_ptr_len(LH_WEB_JS_ERROR_BAD_NUMBER));
            return lh_bool_false;
        }
        while (!lh_web_js_lexer_is_at_end(self) && lh_char_is_digit(lh_web_js_lexer_get_char(self)))
        {
            lh_web_js_lexer_advance(self);
        }
    }
    else
    {
        if (ch == lh_char_map_digit_0)
        {
            lh_web_js_lexer_advance(self);
            if (!lh_web_js_lexer_is_at_end(self) &&
                lh_char_is_digit(lh_web_js_lexer_get_char(self)))
            {
                lh_web_js_lexer_set_error(self, LH_WEB_JS_ERROR_BAD_NUMBER,
                                          lh_str_ptr_len(LH_WEB_JS_ERROR_BAD_NUMBER));
                return lh_bool_false;
            }
        }
        else
        {
            while (!lh_web_js_lexer_is_at_end(self) &&
                   lh_char_is_digit(lh_web_js_lexer_get_char(self)))
            {
                lh_web_js_lexer_advance(self);
            }
        }
        if (!lh_web_js_lexer_is_at_end(self) &&
            lh_web_js_lexer_get_char(self) == lh_char_map_period)
        {
            lh_web_js_lexer_advance(self);
            while (!lh_web_js_lexer_is_at_end(self) &&
                   lh_char_is_digit(lh_web_js_lexer_get_char(self)))
            {
                lh_web_js_lexer_advance(self);
            }
        }
    }

    if (!lh_web_js_lexer_is_at_end(self) &&
        (lh_web_js_lexer_get_char(self) == lh_char_map_small_e ||
         lh_web_js_lexer_get_char(self) == lh_char_map_capital_e))
    {
        lh_web_js_lexer_advance(self);
        if (!lh_web_js_lexer_is_at_end(self) &&
            (lh_web_js_lexer_get_char(self) == lh_char_map_plus ||
             lh_web_js_lexer_get_char(self) == lh_char_map_minus))
        {
            lh_web_js_lexer_advance(self);
        }
        if (lh_web_js_lexer_is_at_end(self) || !lh_char_is_digit(lh_web_js_lexer_get_char(self)))
        {
            lh_web_js_lexer_set_error(self, LH_WEB_JS_ERROR_BAD_NUMBER,
                                      lh_str_ptr_len(LH_WEB_JS_ERROR_BAD_NUMBER));
            return lh_bool_false;
        }
        while (!lh_web_js_lexer_is_at_end(self) && lh_char_is_digit(lh_web_js_lexer_get_char(self)))
        {
            lh_web_js_lexer_advance(self);
        }
    }

    if (!lh_str_ptr_parse_real(lh_web_js_lexer_get_text(self) + start,
                               lh_web_js_lexer_get_pos(self) - start, lh_addr_of(value)))
    {
        lh_web_js_lexer_set_error(self, LH_WEB_JS_ERROR_BAD_NUMBER,
                                  lh_str_ptr_len(LH_WEB_JS_ERROR_BAD_NUMBER));
        return lh_bool_false;
    }

    lh_web_js_token_t *token = lh_web_js_lexer_get_token_mut(self);
    lh_web_js_token_set_kind(token, lh_web_js_token_kind_number);
    lh_web_js_token_set_text(token, lh_web_js_lexer_get_text(self) + start);
    lh_web_js_token_set_text_len(token, lh_web_js_lexer_get_pos(self) - start);
    lh_web_js_token_set_value(token, value);
    return lh_bool_true;
}

lh_bool_t
lh_web_js_lexer_scan_hex_digits(lh_web_js_lexer_t *self, lh_usize_t count)
{
    lh_usize_t start = lh_web_js_lexer_get_pos(self);
    lh_uint_t value = 0;

    if (lh_math_lt(lh_math_sub(lh_web_js_lexer_get_text_len(self), start), count) ||
        !lh_str_ptr_parse_hex(lh_web_js_lexer_get_text(self) + start, count, LH_UINT_T_MAX,
                              lh_addr_of(value)))
    {
        lh_web_js_lexer_set_error(self, LH_WEB_JS_ERROR_BAD_ESCAPE,
                                  lh_str_ptr_len(LH_WEB_JS_ERROR_BAD_ESCAPE));
        return lh_bool_false;
    }
    for (lh_usize_t i = 0U; i < count; ++i)
    {
        lh_web_js_lexer_advance(self);
    }
    return lh_bool_true;
}

lh_bool_t
lh_web_js_lexer_scan_code_point(lh_web_js_lexer_t *self)
{
    lh_usize_t start = lh_web_js_lexer_get_pos(self);
    lh_usize_t digits;
    lh_uint_t value = 0;

    while (!lh_web_js_lexer_is_at_end(self) && lh_char_is_xdigit(lh_web_js_lexer_get_char(self)))
    {
        lh_web_js_lexer_advance(self);
    }
    digits = lh_web_js_lexer_get_pos(self) - start;
    if (lh_math_is_zero(digits) || lh_math_gt(digits, 6U) ||
        !lh_str_ptr_parse_hex(lh_web_js_lexer_get_text(self) + start, digits, LH_UINT_T_MAX,
                              lh_addr_of(value)) ||
        lh_math_gt(value, LH_WEB_JS_CODE_POINT_MAX))
    {
        lh_web_js_lexer_set_error(self, LH_WEB_JS_ERROR_BAD_ESCAPE,
                                  lh_str_ptr_len(LH_WEB_JS_ERROR_BAD_ESCAPE));
        return lh_bool_false;
    }
    if (lh_web_js_lexer_is_at_end(self) || lh_web_js_lexer_get_char(self) != lh_char_map_rbrace)
    {
        lh_web_js_lexer_set_error(self, LH_WEB_JS_ERROR_BAD_ESCAPE,
                                  lh_str_ptr_len(LH_WEB_JS_ERROR_BAD_ESCAPE));
        return lh_bool_false;
    }
    lh_web_js_lexer_advance(self);
    return lh_bool_true;
}

lh_bool_t
lh_web_js_lexer_scan_newline(lh_web_js_lexer_t *self)
{
    lh_usize_t start = lh_web_js_lexer_get_pos(self);

    lh_web_js_lexer_advance(self);

    lh_web_js_token_t *token = lh_web_js_lexer_get_token_mut(self);
    lh_web_js_token_set_kind(token, lh_web_js_token_kind_newline);
    lh_web_js_token_set_text(token, lh_web_js_lexer_get_text(self) + start);
    lh_web_js_token_set_text_len(token, lh_web_js_lexer_get_pos(self) - start);
    lh_web_js_token_set_value(token, 0.0);
    return lh_bool_true;
}

lh_bool_t
lh_web_js_lexer_scan_punctuation(lh_web_js_lexer_t *self)
{
    lh_usize_t start = lh_web_js_lexer_get_pos(self);
    lh_usize_t len;

    lh_str_view_t rest;
    lh_web_js_lexer_get_rest(self, lh_addr_of(rest));
    len = lh_web_js_lexer_find_punctuation(lh_addr_of(rest));
    if (lh_math_is_zero(len))
    {
        lh_web_js_lexer_set_error(self, LH_WEB_JS_ERROR_UNEXPECTED_CHAR,
                                  lh_str_ptr_len(LH_WEB_JS_ERROR_UNEXPECTED_CHAR));
        return lh_bool_false;
    }
    for (lh_usize_t i = 0U; i < len; ++i)
    {
        lh_web_js_lexer_advance(self);
    }

    lh_web_js_token_t *token = lh_web_js_lexer_get_token_mut(self);
    lh_web_js_token_set_kind(token, lh_web_js_token_kind_punctuation);
    lh_web_js_token_set_text(token, lh_web_js_lexer_get_text(self) + start);
    lh_web_js_token_set_text_len(token, lh_web_js_lexer_get_pos(self) - start);
    lh_web_js_token_set_value(token, 0.0);
    return lh_bool_true;
}

lh_bool_t
lh_web_js_lexer_scan_number(lh_web_js_lexer_t *self)
{
    lh_uint_t radix = 0U;

    if (lh_web_js_lexer_get_char(self) == lh_char_map_digit_0)
    {
        lh_char_t prefix = lh_char_map_nul;

        if (lh_web_js_lexer_try_get_char_at(self, 1U, lh_addr_of(prefix)))
        {
            radix = lh_web_js_lexer_find_radix(prefix);
        }
    }
    if (radix != 0U)
    {
        return lh_web_js_lexer_scan_radix_number(self, radix);
    }
    return lh_web_js_lexer_scan_decimal_number(self);
}

lh_bool_t
lh_web_js_lexer_scan_escape(lh_web_js_lexer_t *self)
{
    lh_char_t ch = lh_web_js_lexer_get_char(self);

    if (lh_web_js_lexer_is_at_end(self))
    {
        lh_web_js_lexer_set_error(self, LH_WEB_JS_ERROR_BAD_ESCAPE,
                                  lh_str_ptr_len(LH_WEB_JS_ERROR_BAD_ESCAPE));
        return lh_bool_false;
    }
    if (ch == lh_char_map_small_x)
    {
        lh_web_js_lexer_advance(self);
        return lh_web_js_lexer_scan_hex_digits(self, 2U);
    }
    if (ch == lh_char_map_small_u)
    {
        lh_web_js_lexer_advance(self);
        if (!lh_web_js_lexer_is_at_end(self) &&
            lh_web_js_lexer_get_char(self) == lh_char_map_lbrace)
        {
            lh_web_js_lexer_advance(self);
            return lh_web_js_lexer_scan_code_point(self);
        }
        return lh_web_js_lexer_scan_hex_digits(self, 4U);
    }
    if (lh_str_ptr_contains_char(LH_WEB_JS_ESCAPE_CHARS,
                                 lh_str_ptr_get_size(LH_WEB_JS_ESCAPE_CHARS), ch))
    {
        lh_web_js_lexer_advance(self);
        return lh_bool_true;
    }

    lh_web_js_lexer_set_error(self, LH_WEB_JS_ERROR_BAD_ESCAPE,
                              lh_str_ptr_len(LH_WEB_JS_ERROR_BAD_ESCAPE));
    return lh_bool_false;
}

lh_bool_t
lh_web_js_lexer_scan_string(lh_web_js_lexer_t *self)
{
    lh_usize_t start = lh_web_js_lexer_get_pos(self);
    lh_char_t quote = lh_web_js_lexer_get_char(self);

    lh_web_js_lexer_advance(self);

    while (!lh_web_js_lexer_is_at_end(self))
    {
        lh_char_t ch = lh_web_js_lexer_get_char(self);

        if (ch == quote)
        {
            lh_web_js_lexer_advance(self);
            lh_web_js_token_t *token = lh_web_js_lexer_get_token_mut(self);
            lh_web_js_token_set_kind(token, lh_web_js_token_kind_string);
            lh_web_js_token_set_text(token, lh_web_js_lexer_get_text(self) + start);
            lh_web_js_token_set_text_len(token, lh_web_js_lexer_get_pos(self) - start);
            lh_web_js_token_set_value(token, 0.0);
            return lh_bool_true;
        }
        if (ch == lh_char_map_backslash)
        {
            lh_web_js_lexer_advance(self);
            if (!lh_web_js_lexer_scan_escape(self))
            {
                return lh_bool_false;
            }
            continue;
        }
        if (ch == lh_char_map_lf || ch == lh_char_map_cr)
        {
            break;
        }
        lh_web_js_lexer_advance(self);
    }

    lh_web_js_lexer_set_error(self, LH_WEB_JS_ERROR_UNTERMINATED_STRING,
                              lh_str_ptr_len(LH_WEB_JS_ERROR_UNTERMINATED_STRING));
    return lh_bool_false;
}

lh_bool_t
lh_web_js_lexer_next(lh_web_js_lexer_t *self)
{
    lh_char_t ch;

    lh_web_js_lexer_clear_error(self);

    if (!lh_web_js_lexer_skip_space(self))
    {
        return lh_bool_false;
    }

    if (lh_web_js_lexer_is_at_end(self))
    {
        lh_web_js_token_t *token = lh_web_js_lexer_get_token_mut(self);
        lh_web_js_token_set_kind(token, lh_web_js_token_kind_end_of_input);
        lh_web_js_token_set_text(token,
                                 lh_web_js_lexer_get_text(self) + lh_web_js_lexer_get_pos(self));
        lh_web_js_token_set_text_len(token, 0U);
        lh_web_js_token_set_value(token, 0.0);
        return lh_bool_false;
    }

    ch = lh_web_js_lexer_get_char(self);
    if (lh_web_js_lexer_is_name_start(ch))
    {
        return lh_web_js_lexer_scan_name(self);
    }
    if (lh_char_is_digit(ch))
    {
        return lh_web_js_lexer_scan_number(self);
    }
    if (ch == lh_char_map_period)
    {
        lh_char_t next = lh_char_map_nul;

        if (lh_web_js_lexer_try_get_char_at(self, 1U, lh_addr_of(next)) && lh_char_is_digit(next))
        {
            return lh_web_js_lexer_scan_number(self);
        }
    }
    if (ch == lh_char_map_squote || ch == lh_char_map_dquot)
    {
        return lh_web_js_lexer_scan_string(self);
    }
    if (ch == lh_char_map_lf || ch == lh_char_map_cr)
    {
        return lh_web_js_lexer_scan_newline(self);
    }

    lh_str_view_t rest;
    lh_web_js_lexer_get_rest(self, lh_addr_of(rest));
    if (lh_math_ne(lh_web_js_lexer_find_punctuation(lh_addr_of(rest)), 0U))
    {
        return lh_web_js_lexer_scan_punctuation(self);
    }

    lh_web_js_lexer_set_error(self, LH_WEB_JS_ERROR_UNEXPECTED_CHAR,
                              lh_str_ptr_len(LH_WEB_JS_ERROR_UNEXPECTED_CHAR));
    return lh_bool_false;
}
