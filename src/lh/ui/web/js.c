/**
 * @file js.c
 * @brief Implementation of `lh/ui/web/js.h`.
 */

#include <lh/ui/web/js.h>

#include <lh/cast/const.h>
#include <lh/cast/static.h>
#include <lh/memory.h>
#include <lh/null.h>
#include <lh/str/cstr.h>
#include <lh/util/addr.h>

static lh_void js_emit_x(lh_ui_web_js_t *self, const lh_ui_web_x_t *x);

static lh_void
js_put(lh_ui_web_js_t *self, const lh_char_t *p, lh_usize_t n)
{
    if (self == lh_null || !self->ok)
    {
        return;
    }
    /* Keep one byte for an optional NUL from ::lh_ui_web_js_finish. */
    if (self->mem == lh_null || n + 1u > self->cap - self->len)
    {
        self->ok = lh_bool_false;
        return;
    }
    lh_memory_copy(self->mem + self->len, self->cap - self->len, lh_cast_const(lh_ptr, p), n);
    self->len += n;
}

lh_void
lh_ui_web_js_init(lh_ui_web_js_t *self, lh_byte_ptr mem, lh_usize_t cap)
{
    if (self == lh_null)
    {
        return;
    }
    self->mem = mem;
    self->cap = cap;
    self->len = 0u;
    self->ok = (mem != lh_null && cap > 1u) ? lh_bool_true : lh_bool_false;
}

lh_bool_t
lh_ui_web_js_ok(const lh_ui_web_js_t *self)
{
    return (self != lh_null && self->ok) ? lh_bool_true : lh_bool_false;
}

lh_usize_t
lh_ui_web_js_len(const lh_ui_web_js_t *self)
{
    return self == lh_null ? 0u : self->len;
}

lh_byte_cptr
lh_ui_web_js_bytes(const lh_ui_web_js_t *self)
{
    return self == lh_null ? lh_null : self->mem;
}

lh_void
lh_ui_web_js_lit(lh_ui_web_js_t *self, const lh_char_t *p, lh_usize_t n)
{
    if (p == lh_null || n == 0u)
    {
        return;
    }
    js_put(self, p, n);
}

lh_void
lh_ui_web_js_raw(lh_ui_web_js_t *self, const lh_char_t *s)
{
    if (s == lh_null)
    {
        return;
    }
    js_put(self, s, lh_str_len(s));
}

lh_void
lh_ui_web_js_var(lh_ui_web_js_t *self, const lh_char_t *name, const lh_char_t *expr)
{
    js_put(self, "var ", 4u);
    lh_ui_web_js_raw(self, name);
    js_put(self, "=", 1u);
    lh_ui_web_js_raw(self, expr);
    js_put(self, ";\n", 2u);
}

lh_void
lh_ui_web_js_vars(lh_ui_web_js_t *self, const lh_char_t *decls)
{
    js_put(self, "var ", 4u);
    lh_ui_web_js_raw(self, decls);
    js_put(self, ";\n", 2u);
}

lh_void
lh_ui_web_js_fun_open(lh_ui_web_js_t *self, const lh_char_t *name, const lh_char_t *params)
{
    js_put(self, "function ", 9u);
    lh_ui_web_js_raw(self, name);
    js_put(self, "(", 1u);
    lh_ui_web_js_raw(self, params);
    js_put(self, "){\n", 3u);
}

lh_void
lh_ui_web_js_fun_close(lh_ui_web_js_t *self)
{
    js_put(self, "}\n", 2u);
}

lh_void
lh_ui_web_js_fun(lh_ui_web_js_t *self, const lh_char_t *name, const lh_char_t *params,
                 const lh_char_t *body)
{
    lh_ui_web_js_fun_open(self, name, params);
    lh_ui_web_js_raw(self, body);
    js_put(self, "\n", 1u);
    lh_ui_web_js_fun_close(self);
}

lh_void
lh_ui_web_js_ret(lh_ui_web_js_t *self, const lh_char_t *expr)
{
    js_put(self, "return", 6u);
    if (expr != lh_null && expr[0] != '\0')
    {
        js_put(self, " ", 1u);
        lh_ui_web_js_raw(self, expr);
    }
    js_put(self, ";\n", 2u);
}

lh_void
lh_ui_web_js_if(lh_ui_web_js_t *self, const lh_char_t *cond)
{
    js_put(self, "if(", 3u);
    lh_ui_web_js_raw(self, cond);
    js_put(self, "){\n", 3u);
}

lh_void
lh_ui_web_js_else(lh_ui_web_js_t *self)
{
    js_put(self, "}else{\n", 7u);
}

lh_void
lh_ui_web_js_for(lh_ui_web_js_t *self, const lh_char_t *head)
{
    js_put(self, "for(", 4u);
    lh_ui_web_js_raw(self, head);
    js_put(self, "){\n", 3u);
}

lh_void
lh_ui_web_js_end(lh_ui_web_js_t *self)
{
    js_put(self, "}\n", 2u);
}

lh_void
lh_ui_web_js_assign(lh_ui_web_js_t *self, const lh_char_t *lhs, const lh_char_t *rhs)
{
    lh_ui_web_js_raw(self, lhs);
    js_put(self, "=", 1u);
    lh_ui_web_js_raw(self, rhs);
    js_put(self, ";\n", 2u);
}

lh_void
lh_ui_web_js_prop(lh_ui_web_js_t *self, const lh_char_t *obj, const lh_char_t *prop,
                  const lh_char_t *expr)
{
    lh_ui_web_js_raw(self, obj);
    js_put(self, ".", 1u);
    lh_ui_web_js_raw(self, prop);
    js_put(self, "=", 1u);
    lh_ui_web_js_raw(self, expr);
    js_put(self, ";\n", 2u);
}

lh_void
lh_ui_web_js_stmt(lh_ui_web_js_t *self, const lh_char_t *stmt)
{
    lh_ui_web_js_raw(self, stmt);
    js_put(self, ";\n", 2u);
}

lh_void
lh_ui_web_js_finish(lh_ui_web_js_t *self)
{
    if (self == lh_null || !self->ok || self->mem == lh_null)
    {
        return;
    }
    if (self->len >= self->cap)
    {
        self->ok = lh_bool_false;
        return;
    }
    self->mem[self->len] = 0;
}

static lh_void
js_put_s64(lh_ui_web_js_t *self, lh_s64_t n)
{
    lh_char_t dig[24];
    lh_usize_t i;
    lh_u64_t mag;
    lh_bool_t neg;

    if (n == 0)
    {
        js_put(self, "0", 1u);
        return;
    }
    neg = n < 0 ? lh_bool_true : lh_bool_false;
    /* Avoid UB on LH_S64_MIN: unsigned wrap from the signed bit pattern. */
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
        js_put(self, "-", 1u);
    }
    while (i > 0u)
    {
        --i;
        js_put(self, lh_addr_of(dig[i]), 1u);
    }
}

static lh_void
js_put_str_lit(lh_ui_web_js_t *self, const lh_char_t *s)
{
    js_put(self, "'", 1u);
    if (s != lh_null)
    {
        while (*s != '\0')
        {
            lh_char_t c;

            c = *s++;
            if (c == '\\' || c == '\'')
            {
                js_put(self, "\\", 1u);
                js_put(self, lh_addr_of(c), 1u);
            }
            else if (c == '\n')
            {
                js_put(self, "\\n", 2u);
            }
            else if (c == '\r')
            {
                js_put(self, "\\r", 2u);
            }
            else if (c == '\t')
            {
                js_put(self, "\\t", 2u);
            }
            else
            {
                js_put(self, lh_addr_of(c), 1u);
            }
        }
    }
    js_put(self, "'", 1u);
}

static lh_void
js_emit_args(lh_ui_web_js_t *self, const lh_ui_web_x_t *const *args, lh_u16_t n)
{
    lh_u16_t i;

    js_put(self, "(", 1u);
    for (i = 0u; i < n; ++i)
    {
        if (i != 0u)
        {
            js_put(self, ",", 1u);
        }
        js_emit_x(self, args[i]);
    }
    js_put(self, ")", 1u);
}

static lh_void
js_emit_x(lh_ui_web_js_t *self, const lh_ui_web_x_t *x)
{
    lh_u16_t i;

    if (self == lh_null || !self->ok || x == lh_null)
    {
        if (self != lh_null)
        {
            self->ok = lh_bool_false;
        }
        return;
    }
    switch (x->kind)
    {
    case lh_ui_web_x_kind_id:
        lh_ui_web_js_raw(self, x->u.id);
        break;
    case lh_ui_web_x_kind_num:
        js_put_s64(self, x->u.num);
        break;
    case lh_ui_web_x_kind_str:
        js_put_str_lit(self, x->u.str);
        break;
    case lh_ui_web_x_kind_null:
        js_put(self, "null", 4u);
        break;
    case lh_ui_web_x_kind_true:
        js_put(self, "true", 4u);
        break;
    case lh_ui_web_x_kind_false:
        js_put(self, "false", 5u);
        break;
    case lh_ui_web_x_kind_dot:
        js_put(self, "(", 1u);
        js_emit_x(self, x->u.dot.obj);
        js_put(self, ").", 2u);
        lh_ui_web_js_raw(self, x->u.dot.name);
        break;
    case lh_ui_web_x_kind_idx:
        js_put(self, "(", 1u);
        js_emit_x(self, x->u.idx.obj);
        js_put(self, ")[", 2u);
        js_emit_x(self, x->u.idx.ix);
        js_put(self, "]", 1u);
        break;
    case lh_ui_web_x_kind_call:
        js_put(self, "(", 1u);
        js_emit_x(self, x->u.call.fn);
        js_put(self, ")", 1u);
        js_emit_args(self, x->u.call.args, x->u.call.n);
        break;
    case lh_ui_web_x_kind_call_id:
        lh_ui_web_js_raw(self, x->u.call_id.name);
        js_emit_args(self, x->u.call_id.args, x->u.call_id.n);
        break;
    case lh_ui_web_x_kind_bin:
        js_put(self, "(", 1u);
        js_emit_x(self, x->u.bin.a);
        js_put(self, ")", 1u);
        lh_ui_web_js_raw(self, x->u.bin.op);
        js_put(self, "(", 1u);
        js_emit_x(self, x->u.bin.b);
        js_put(self, ")", 1u);
        break;
    case lh_ui_web_x_kind_un:
        lh_ui_web_js_raw(self, x->u.un.op);
        js_put(self, "(", 1u);
        js_emit_x(self, x->u.un.a);
        js_put(self, ")", 1u);
        break;
    case lh_ui_web_x_kind_tern:
        js_put(self, "(", 1u);
        js_emit_x(self, x->u.tern.c);
        js_put(self, ")?(", 3u);
        js_emit_x(self, x->u.tern.t);
        js_put(self, "):(", 3u);
        js_emit_x(self, x->u.tern.f);
        js_put(self, ")", 1u);
        break;
    case lh_ui_web_x_kind_typeof:
        js_put(self, "typeof(", 7u);
        js_emit_x(self, x->u.typeof_a);
        js_put(self, ")", 1u);
        break;
    case lh_ui_web_x_kind_obj:
        js_put(self, "{", 1u);
        for (i = 0u; i < x->u.obj.n; ++i)
        {
            if (i != 0u)
            {
                js_put(self, ",", 1u);
            }
            lh_ui_web_js_raw(self, x->u.obj.keys[i]);
            js_put(self, ":", 1u);
            js_emit_x(self, x->u.obj.vals[i]);
        }
        js_put(self, "}", 1u);
        break;
    default:
        self->ok = lh_bool_false;
        break;
    }
}

lh_void
lh_ui_web_js_x(lh_ui_web_js_t *self, const lh_ui_web_x_t *x)
{
    js_emit_x(self, x);
}

lh_void
lh_ui_web_js_ret_x(lh_ui_web_js_t *self, const lh_ui_web_x_t *x)
{
    js_put(self, "return", 6u);
    if (x != lh_null)
    {
        js_put(self, " ", 1u);
        js_emit_x(self, x);
    }
    js_put(self, ";\n", 2u);
}

lh_void
lh_ui_web_js_if_x(lh_ui_web_js_t *self, const lh_ui_web_x_t *cond)
{
    js_put(self, "if(", 3u);
    js_emit_x(self, cond);
    js_put(self, "){\n", 3u);
}

lh_void
lh_ui_web_js_var_x(lh_ui_web_js_t *self, const lh_char_t *name, const lh_ui_web_x_t *x)
{
    js_put(self, "var ", 4u);
    lh_ui_web_js_raw(self, name);
    js_put(self, "=", 1u);
    js_emit_x(self, x);
    js_put(self, ";\n", 2u);
}

lh_void
lh_ui_web_js_assign_x(lh_ui_web_js_t *self, const lh_ui_web_x_t *lhs, const lh_ui_web_x_t *rhs)
{
    js_emit_x(self, lhs);
    js_put(self, "=", 1u);
    js_emit_x(self, rhs);
    js_put(self, ";\n", 2u);
}

lh_void
lh_ui_web_js_stmt_x(lh_ui_web_js_t *self, const lh_ui_web_x_t *x)
{
    js_emit_x(self, x);
    js_put(self, ";\n", 2u);
}
