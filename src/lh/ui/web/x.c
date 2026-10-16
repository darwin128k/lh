/**
 * @file x.c
 * @brief Implementation of `lh/ui/web/x.h`.
 */

#include <lh/ui/web/x.h>
#include <lh/assert/runtime.h>

lh_void
lh_ui_web_x_id(const lh_str_ptr name, lh_ui_web_x_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ui_web_x_t x;

    x.kind = lh_ui_web_x_kind_id;
    x.u.id = name;
    lh_ptr_deref(out) = x;
}

lh_void
lh_ui_web_x_num(lh_s64_t n, lh_ui_web_x_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ui_web_x_t x;

    x.kind = lh_ui_web_x_kind_num;
    x.u.num = n;
    lh_ptr_deref(out) = x;
}

lh_void
lh_ui_web_x_str(const lh_str_ptr s, lh_ui_web_x_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ui_web_x_t x;

    x.kind = lh_ui_web_x_kind_str;
    x.u.str = s;
    lh_ptr_deref(out) = x;
}

lh_void
lh_ui_web_x_null(lh_ui_web_x_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ui_web_x_t x;

    x.kind = lh_ui_web_x_kind_null;
    lh_ptr_deref(out) = x;
}

lh_void
lh_ui_web_x_true(lh_ui_web_x_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ui_web_x_t x;

    x.kind = lh_ui_web_x_kind_true;
    lh_ptr_deref(out) = x;
}

lh_void
lh_ui_web_x_false(lh_ui_web_x_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ui_web_x_t x;

    x.kind = lh_ui_web_x_kind_false;
    lh_ptr_deref(out) = x;
}

lh_void
lh_ui_web_x_dot(const lh_ui_web_x_t *obj, const lh_str_ptr name, lh_ui_web_x_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ui_web_x_t x;

    x.kind = lh_ui_web_x_kind_dot;
    x.u.dot.obj = obj;
    x.u.dot.name = name;
    lh_ptr_deref(out) = x;
}

lh_void
lh_ui_web_x_idx(const lh_ui_web_x_t *obj, const lh_ui_web_x_t *ix, lh_ui_web_x_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ui_web_x_t x;

    x.kind = lh_ui_web_x_kind_idx;
    x.u.idx.obj = obj;
    x.u.idx.ix = ix;
    lh_ptr_deref(out) = x;
}

lh_void
lh_ui_web_x_call(const lh_ui_web_x_t *fn, const lh_ui_web_x_t *const *args, lh_u16_t n, lh_ui_web_x_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ui_web_x_t x;

    x.kind = lh_ui_web_x_kind_call;
    x.u.call.fn = fn;
    x.u.call.args = args;
    x.u.call.n = n;
    lh_ptr_deref(out) = x;
}

lh_void
lh_ui_web_x_call_id(const lh_str_ptr name, const lh_ui_web_x_t *const *args, lh_u16_t n, lh_ui_web_x_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ui_web_x_t x;

    x.kind = lh_ui_web_x_kind_call_id;
    x.u.call_id.name = name;
    x.u.call_id.args = args;
    x.u.call_id.n = n;
    lh_ptr_deref(out) = x;
}

lh_void
lh_ui_web_x_bin(const lh_ui_web_x_t *a, const lh_str_ptr op, const lh_ui_web_x_t *b, lh_ui_web_x_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ui_web_x_t x;

    x.kind = lh_ui_web_x_kind_bin;
    x.u.bin.a = a;
    x.u.bin.op = op;
    x.u.bin.b = b;
    lh_ptr_deref(out) = x;
}

lh_void
lh_ui_web_x_un(const lh_str_ptr op, const lh_ui_web_x_t *a, lh_ui_web_x_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ui_web_x_t x;

    x.kind = lh_ui_web_x_kind_un;
    x.u.un.op = op;
    x.u.un.a = a;
    lh_ptr_deref(out) = x;
}

lh_void
lh_ui_web_x_tern(const lh_ui_web_x_t *c, const lh_ui_web_x_t *t, const lh_ui_web_x_t *f, lh_ui_web_x_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ui_web_x_t x;

    x.kind = lh_ui_web_x_kind_tern;
    x.u.tern.c = c;
    x.u.tern.t = t;
    x.u.tern.f = f;
    lh_ptr_deref(out) = x;
}

lh_void
lh_ui_web_x_typeof(const lh_ui_web_x_t *a, lh_ui_web_x_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ui_web_x_t x;

    x.kind = lh_ui_web_x_kind_typeof;
    x.u.typeof_a = a;
    lh_ptr_deref(out) = x;
}

lh_void
lh_ui_web_x_obj(const lh_str_ptr *keys, const lh_ui_web_x_t *const *vals, lh_u16_t n, lh_ui_web_x_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ui_web_x_t x;

    x.kind = lh_ui_web_x_kind_obj;
    x.u.obj.keys = keys;
    x.u.obj.vals = vals;
    x.u.obj.n = n;
    lh_ptr_deref(out) = x;
}
