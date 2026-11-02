/**
 * @file x.c
 * @brief Implementation of `lh/ui/web/js/x.h`.
 */

#include <lh/ui/web/js/x.h>
#include <lh/char/map.h>
#include <lh/js/lexer/name.h>
#include <lh/str/format/sllong.h>

lh_void
lh_ui_web_js_put_s64(lh_ui_web_buf_t *self, lh_s64_t n)
{
    lh_char_t text[24];
    lh_ui_web_buf_lit(self, text, lh_str_ptr_format_sllong(n, text, sizeof text));
}

lh_void
lh_ui_web_js_put_str_lit(lh_ui_web_buf_t *self, const lh_str_ptr s)
{
    static const lh_char_t escape_chars[] = {lh_char_map_backslash, lh_char_map_squote,
                                             lh_char_map_lf,        lh_char_map_cr,
                                             lh_char_map_ht,        lh_char_map_less};
    lh_ui_web_buf_lit(self, "'", 1u);
    if (s != lh_null)
    {
        lh_str_cptr at = s;
        lh_usize_t remaining = lh_str_ptr_len(s);
        while (remaining != 0u)
        {
            const lh_str_ptr found =
                lh_str_ptr_find_of_chars(at, remaining, escape_chars, sizeof(escape_chars));
            if (found == lh_null)
            {
                lh_ui_web_buf_lit(self, at, remaining);
                break;
            }
            const lh_usize_t prefix = lh_cast_static(lh_usize_t, found - at);
            lh_ui_web_buf_lit(self, at, prefix);
            if (lh_str_ptr_contains_char(escape_chars, 2u, lh_ptr_deref(found)))
            {
                lh_ui_web_buf_lit(self, "\\", 1u);
                lh_ui_web_buf_lit(self, found, 1u);
            }
            else if (lh_ptr_deref(found) == lh_char_map_lf)
            {
                lh_ui_web_buf_lit(self, "\\n", 2u);
            }
            else if (lh_ptr_deref(found) == lh_char_map_cr)
            {
                lh_ui_web_buf_lit(self, "\\r", 2u);
            }
            else if (lh_ptr_deref(found) == lh_char_map_ht)
            {
                lh_ui_web_buf_lit(self, "\\t", 2u);
            }
            else
            {
                lh_ui_web_buf_lit(self, "\\x3c", 4u);
            }
            at = found + 1u;
            remaining -= prefix + 1u;
        }
    }
    lh_ui_web_buf_lit(self, "'", 1u);
}

lh_void
lh_ui_web_js_emit_args(lh_ui_web_buf_t *self, const lh_ui_web_x_t *const *args, lh_u16_t n)
{
    lh_u16_t i;

    lh_ui_web_buf_lit(self, "(", 1u);
    for (i = 0u; i < n; ++i)
    {
        if (i != 0u)
        {
            lh_ui_web_buf_lit(self, ",", 1u);
        }
        lh_ui_web_js_emit_x(self, args[i]);
    }
    lh_ui_web_buf_lit(self, ")", 1u);
}

lh_void
lh_ui_web_js_emit_x(lh_ui_web_buf_t *self, const lh_ui_web_x_t *x)
{
    lh_u16_t i;

    if (self == lh_null || !lh_ui_web_buf_ok(self) || x == lh_null)
    {
        lh_ui_web_buf_fail(self);
        return;
    }
    switch (lh_ui_web_x_get_kind(x))
    {
    case lh_ui_web_x_kind_id:
        lh_ui_web_buf_raw(self, lh_ui_web_x_get_id(x));
        break;
    case lh_ui_web_x_kind_num:
        lh_ui_web_js_put_s64(self, lh_ui_web_x_get_num(x));
        break;
    case lh_ui_web_x_kind_str:
        lh_ui_web_js_put_str_lit(self, lh_ui_web_x_get_str(x));
        break;
    case lh_ui_web_x_kind_null:
        lh_ui_web_buf_lit(self, "null", 4u);
        break;
    case lh_ui_web_x_kind_true:
        lh_ui_web_buf_lit(self, "true", 4u);
        break;
    case lh_ui_web_x_kind_false:
        lh_ui_web_buf_lit(self, "false", 5u);
        break;
    case lh_ui_web_x_kind_dot:
        lh_ui_web_buf_lit(self, "(", 1u);
        lh_ui_web_js_emit_x(self, lh_ui_web_x_get_dot_obj(x));
        lh_ui_web_buf_lit(self, ").", 2u);
        lh_ui_web_buf_raw(self, lh_ui_web_x_get_dot_name(x));
        break;
    case lh_ui_web_x_kind_idx:
        lh_ui_web_buf_lit(self, "(", 1u);
        lh_ui_web_js_emit_x(self, lh_ui_web_x_get_idx_obj(x));
        lh_ui_web_buf_lit(self, ")[", 2u);
        lh_ui_web_js_emit_x(self, lh_ui_web_x_get_idx_ix(x));
        lh_ui_web_buf_lit(self, "]", 1u);
        break;
    case lh_ui_web_x_kind_call:
        lh_ui_web_buf_lit(self, "(", 1u);
        lh_ui_web_js_emit_x(self, lh_ui_web_x_get_call_fn(x));
        lh_ui_web_buf_lit(self, ")", 1u);
        lh_ui_web_js_emit_args(self, lh_ui_web_x_get_call_args(x), lh_ui_web_x_get_call_n(x));
        break;
    case lh_ui_web_x_kind_call_id:
        lh_ui_web_buf_raw(self, lh_ui_web_x_get_call_id_name(x));
        lh_ui_web_js_emit_args(self, lh_ui_web_x_get_call_id_args(x), lh_ui_web_x_get_call_id_n(x));
        break;
    case lh_ui_web_x_kind_bin:
        lh_ui_web_buf_lit(self, "(", 1u);
        lh_ui_web_js_emit_x(self, lh_ui_web_x_get_bin_a(x));
        lh_ui_web_buf_lit(self, ")", 1u);
        lh_ui_web_buf_raw(self, lh_ui_web_x_get_bin_op(x));
        lh_ui_web_buf_lit(self, "(", 1u);
        lh_ui_web_js_emit_x(self, lh_ui_web_x_get_bin_b(x));
        lh_ui_web_buf_lit(self, ")", 1u);
        break;
    case lh_ui_web_x_kind_un:
        lh_ui_web_buf_raw(self, lh_ui_web_x_get_un_op(x));
        lh_ui_web_buf_lit(self, "(", 1u);
        lh_ui_web_js_emit_x(self, lh_ui_web_x_get_un_a(x));
        lh_ui_web_buf_lit(self, ")", 1u);
        break;
    case lh_ui_web_x_kind_tern:
        lh_ui_web_buf_lit(self, "(", 1u);
        lh_ui_web_js_emit_x(self, lh_ui_web_x_get_tern_c(x));
        lh_ui_web_buf_lit(self, ")?(", 3u);
        lh_ui_web_js_emit_x(self, lh_ui_web_x_get_tern_t(x));
        lh_ui_web_buf_lit(self, "):(", 3u);
        lh_ui_web_js_emit_x(self, lh_ui_web_x_get_tern_f(x));
        lh_ui_web_buf_lit(self, ")", 1u);
        break;
    case lh_ui_web_x_kind_typeof:
        lh_ui_web_buf_lit(self, "typeof(", 7u);
        lh_ui_web_js_emit_x(self, lh_ui_web_x_get_typeof_a(x));
        lh_ui_web_buf_lit(self, ")", 1u);
        break;
    case lh_ui_web_x_kind_obj:
        lh_ui_web_buf_lit(self, "{", 1u);
        for (i = 0u; i < lh_ui_web_x_get_obj_n(x); ++i)
        {
            if (i != 0u)
            {
                lh_ui_web_buf_lit(self, ",", 1u);
            }
            const lh_str_ptr key = lh_ui_web_x_get_obj_keys(x)[i];
            const lh_str_view_t prototype = lh_str_view_initializer_lit("__proto__");
            lh_str_view_t name;
            lh_str_view_init(lh_addr_of(name), key);
            if (lh_js_lexer_name_is_ascii_identifier(lh_addr_of(name)) &&
                !lh_str_view_equals(lh_addr_of(name), lh_addr_of(prototype), lh_bool_false))
            {
                lh_ui_web_buf_lit(self, key, lh_str_view_get_size(lh_addr_of(name)));
                lh_ui_web_buf_lit(self, ":", 1u);
            }
            else
            {
                lh_ui_web_buf_lit(self, "[", 1u);
                lh_ui_web_js_put_str_lit(self, key);
                lh_ui_web_buf_lit(self, "]:", 2u);
            }
            lh_ui_web_js_emit_x(self, lh_ui_web_x_get_obj_vals(x)[i]);
        }
        lh_ui_web_buf_lit(self, "}", 1u);
        break;
    default:
        lh_ui_web_buf_fail(self);
        break;
    }
}

lh_void
lh_ui_web_js_x(lh_ui_web_buf_t *self, const lh_ui_web_x_t *x)
{
    lh_ui_web_js_emit_x(self, x);
}

lh_void
lh_ui_web_js_ret_x(lh_ui_web_buf_t *self, const lh_ui_web_x_t *x)
{
    lh_ui_web_buf_lit(self, "return", 6u);
    if (x != lh_null)
    {
        lh_ui_web_buf_lit(self, " ", 1u);
        lh_ui_web_js_emit_x(self, x);
    }
    lh_ui_web_buf_lit(self, ";\n", 2u);
}

lh_void
lh_ui_web_js_if_x(lh_ui_web_buf_t *self, const lh_ui_web_x_t *cond)
{
    lh_ui_web_buf_lit(self, "if(", 3u);
    lh_ui_web_js_emit_x(self, cond);
    lh_ui_web_buf_lit(self, "){\n", 3u);
}

lh_void
lh_ui_web_js_var_x(lh_ui_web_buf_t *self, const lh_str_ptr name, const lh_ui_web_x_t *x)
{
    lh_ui_web_buf_lit(self, "var ", 4u);
    lh_ui_web_buf_raw(self, name);
    lh_ui_web_buf_lit(self, "=", 1u);
    lh_ui_web_js_emit_x(self, x);
    lh_ui_web_buf_lit(self, ";\n", 2u);
}

lh_void
lh_ui_web_js_assign_x(lh_ui_web_buf_t *self, const lh_ui_web_x_t *lhs, const lh_ui_web_x_t *rhs)
{
    lh_ui_web_js_emit_x(self, lhs);
    lh_ui_web_buf_lit(self, "=", 1u);
    lh_ui_web_js_emit_x(self, rhs);
    lh_ui_web_buf_lit(self, ";\n", 2u);
}

lh_void
lh_ui_web_js_stmt_x(lh_ui_web_buf_t *self, const lh_ui_web_x_t *x)
{
    lh_ui_web_js_emit_x(self, x);
    lh_ui_web_buf_lit(self, ";\n", 2u);
}
