/**
 * @file x.c
 * @brief Implementation of `lh/ui/web/js/x.h`.
 */

#include <lh/ui/web/js/x.h>

static lh_void
js_emit_x(lh_ui_web_buf_t *self, const lh_ui_web_x_t *x);

/** Append @p n in decimal. */
static lh_void
js_put_s64(lh_ui_web_buf_t *self, lh_s64_t n)
{
    lh_char_t dig[24];
    lh_usize_t i;
    lh_u64_t mag;
    lh_bool_t neg;

    if (n == 0)
    {
        lh_ui_web_buf_lit(self, "0", 1u);
        return;
    }
    neg = n < 0 ? lh_bool_true : lh_bool_false;
    mag = neg ? (lh_cast_static(lh_u64_t, 0) - lh_cast_static(lh_u64_t, n))
              : lh_cast_static(lh_u64_t, n);
    i = 0u;
    while (mag != 0u && i < sizeof(dig))
    {
        dig[i++] = lh_cast_static(lh_char_t, '0' + (mag % 10u));
        mag /= 10u;
    }
    if (neg)
    {
        lh_ui_web_buf_lit(self, "-", 1u);
    }
    while (i > 0u)
    {
        --i;
        lh_ui_web_buf_lit(self, lh_addr_of(dig[i]), 1u);
    }
}

/** Append @p s as a single-quoted JavaScript string literal, escaped. */
static lh_void
js_put_str_lit(lh_ui_web_buf_t *self, const lh_str_ptr s)
{
    lh_ui_web_buf_lit(self, "'", 1u);
    if (s != lh_null)
    {
        while (lh_ptr_deref(s) != '\0')
        {
            lh_char_t c;

            c = lh_ptr_deref(s++);
            if (c == '\\' || c == '\'')
            {
                lh_ui_web_buf_lit(self, "\\", 1u);
                lh_ui_web_buf_lit(self, lh_addr_of(c), 1u);
            }
            else if (c == '\n')
            {
                lh_ui_web_buf_lit(self, "\\n", 2u);
            }
            else if (c == '\r')
            {
                lh_ui_web_buf_lit(self, "\\r", 2u);
            }
            else if (c == '\t')
            {
                lh_ui_web_buf_lit(self, "\\t", 2u);
            }
            else
            {
                lh_ui_web_buf_lit(self, lh_addr_of(c), 1u);
            }
        }
    }
    lh_ui_web_buf_lit(self, "'", 1u);
}

/** Append an argument list `(a,b,...)` of @p n expressions. */
static lh_void
js_emit_args(lh_ui_web_buf_t *self, const lh_ui_web_x_t *const *args, lh_u16_t n)
{
    lh_u16_t i;

    lh_ui_web_buf_lit(self, "(", 1u);
    for (i = 0u; i < n; ++i)
    {
        if (i != 0u)
        {
            lh_ui_web_buf_lit(self, ",", 1u);
        }
        js_emit_x(self, args[i]);
    }
    lh_ui_web_buf_lit(self, ")", 1u);
}

/** Append expression node @p x as JavaScript, parenthesised so precedence never matters. */
static lh_void
js_emit_x(lh_ui_web_buf_t *self, const lh_ui_web_x_t *x)
{
    lh_u16_t i;

    if (self == lh_null || !lh_ui_web_buf_ok(self) || x == lh_null)
    {
        lh_ui_web_buf_fail(self);
        return;
    }
    switch (x->kind)
    {
    case lh_ui_web_x_kind_id:
        lh_ui_web_buf_raw(self, x->u.id);
        break;
    case lh_ui_web_x_kind_num:
        js_put_s64(self, x->u.num);
        break;
    case lh_ui_web_x_kind_str:
        js_put_str_lit(self, x->u.str);
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
        js_emit_x(self, x->u.dot.obj);
        lh_ui_web_buf_lit(self, ").", 2u);
        lh_ui_web_buf_raw(self, x->u.dot.name);
        break;
    case lh_ui_web_x_kind_idx:
        lh_ui_web_buf_lit(self, "(", 1u);
        js_emit_x(self, x->u.idx.obj);
        lh_ui_web_buf_lit(self, ")[", 2u);
        js_emit_x(self, x->u.idx.ix);
        lh_ui_web_buf_lit(self, "]", 1u);
        break;
    case lh_ui_web_x_kind_call:
        lh_ui_web_buf_lit(self, "(", 1u);
        js_emit_x(self, x->u.call.fn);
        lh_ui_web_buf_lit(self, ")", 1u);
        js_emit_args(self, x->u.call.args, x->u.call.n);
        break;
    case lh_ui_web_x_kind_call_id:
        lh_ui_web_buf_raw(self, x->u.call_id.name);
        js_emit_args(self, x->u.call_id.args, x->u.call_id.n);
        break;
    case lh_ui_web_x_kind_bin:
        lh_ui_web_buf_lit(self, "(", 1u);
        js_emit_x(self, x->u.bin.a);
        lh_ui_web_buf_lit(self, ")", 1u);
        lh_ui_web_buf_raw(self, x->u.bin.op);
        lh_ui_web_buf_lit(self, "(", 1u);
        js_emit_x(self, x->u.bin.b);
        lh_ui_web_buf_lit(self, ")", 1u);
        break;
    case lh_ui_web_x_kind_un:
        lh_ui_web_buf_raw(self, x->u.un.op);
        lh_ui_web_buf_lit(self, "(", 1u);
        js_emit_x(self, x->u.un.a);
        lh_ui_web_buf_lit(self, ")", 1u);
        break;
    case lh_ui_web_x_kind_tern:
        lh_ui_web_buf_lit(self, "(", 1u);
        js_emit_x(self, x->u.tern.c);
        lh_ui_web_buf_lit(self, ")?(", 3u);
        js_emit_x(self, x->u.tern.t);
        lh_ui_web_buf_lit(self, "):(", 3u);
        js_emit_x(self, x->u.tern.f);
        lh_ui_web_buf_lit(self, ")", 1u);
        break;
    case lh_ui_web_x_kind_typeof:
        lh_ui_web_buf_lit(self, "typeof(", 7u);
        js_emit_x(self, x->u.typeof_a);
        lh_ui_web_buf_lit(self, ")", 1u);
        break;
    case lh_ui_web_x_kind_obj:
        lh_ui_web_buf_lit(self, "{", 1u);
        for (i = 0u; i < x->u.obj.n; ++i)
        {
            if (i != 0u)
            {
                lh_ui_web_buf_lit(self, ",", 1u);
            }
            lh_ui_web_buf_raw(self, x->u.obj.keys[i]);
            lh_ui_web_buf_lit(self, ":", 1u);
            js_emit_x(self, x->u.obj.vals[i]);
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
    js_emit_x(self, x);
}

lh_void
lh_ui_web_js_ret_x(lh_ui_web_buf_t *self, const lh_ui_web_x_t *x)
{
    lh_ui_web_buf_lit(self, "return", 6u);
    if (x != lh_null)
    {
        lh_ui_web_buf_lit(self, " ", 1u);
        js_emit_x(self, x);
    }
    lh_ui_web_buf_lit(self, ";\n", 2u);
}

lh_void
lh_ui_web_js_if_x(lh_ui_web_buf_t *self, const lh_ui_web_x_t *cond)
{
    lh_ui_web_buf_lit(self, "if(", 3u);
    js_emit_x(self, cond);
    lh_ui_web_buf_lit(self, "){\n", 3u);
}

lh_void
lh_ui_web_js_var_x(lh_ui_web_buf_t *self, const lh_str_ptr name, const lh_ui_web_x_t *x)
{
    lh_ui_web_buf_lit(self, "var ", 4u);
    lh_ui_web_buf_raw(self, name);
    lh_ui_web_buf_lit(self, "=", 1u);
    js_emit_x(self, x);
    lh_ui_web_buf_lit(self, ";\n", 2u);
}

lh_void
lh_ui_web_js_assign_x(lh_ui_web_buf_t *self, const lh_ui_web_x_t *lhs, const lh_ui_web_x_t *rhs)
{
    js_emit_x(self, lhs);
    lh_ui_web_buf_lit(self, "=", 1u);
    js_emit_x(self, rhs);
    lh_ui_web_buf_lit(self, ";\n", 2u);
}

lh_void
lh_ui_web_js_stmt_x(lh_ui_web_buf_t *self, const lh_ui_web_x_t *x)
{
    js_emit_x(self, x);
    lh_ui_web_buf_lit(self, ";\n", 2u);
}
