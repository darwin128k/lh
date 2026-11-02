/**
 * @file x.c
 * @brief Implementation of `lh/ui/web/x.h`.
 */

#include <lh/ui/web/x.h>
#include <lh/assert/runtime.h>

lh_ui_web_x_kind_t
lh_ui_web_x_get_kind(const lh_ui_web_x_t *self)
{
    return self->kind;
}

lh_void
lh_ui_web_x_set_kind(lh_ui_web_x_t *self, lh_ui_web_x_kind_t value)
{
    self->kind = value;
}

const lh_str_ptr
lh_ui_web_x_get_id(const lh_ui_web_x_t *self)
{
    return self->u.id;
}

lh_void
lh_ui_web_x_set_id(lh_ui_web_x_t *self, const lh_str_ptr value)
{
    self->u.id = value;
}

lh_s64_t
lh_ui_web_x_get_num(const lh_ui_web_x_t *self)
{
    return self->u.num;
}

lh_void
lh_ui_web_x_set_num(lh_ui_web_x_t *self, lh_s64_t value)
{
    self->u.num = value;
}

const lh_str_ptr
lh_ui_web_x_get_str(const lh_ui_web_x_t *self)
{
    return self->u.str;
}

lh_void
lh_ui_web_x_set_str(lh_ui_web_x_t *self, const lh_str_ptr value)
{
    self->u.str = value;
}

const lh_ui_web_x_t *
lh_ui_web_x_get_dot_obj(const lh_ui_web_x_t *self)
{
    return self->u.dot.obj;
}

lh_void
lh_ui_web_x_set_dot_obj(lh_ui_web_x_t *self, const lh_ui_web_x_t *value)
{
    self->u.dot.obj = value;
}

const lh_str_ptr
lh_ui_web_x_get_dot_name(const lh_ui_web_x_t *self)
{
    return self->u.dot.name;
}

lh_void
lh_ui_web_x_set_dot_name(lh_ui_web_x_t *self, const lh_str_ptr value)
{
    self->u.dot.name = value;
}

const lh_ui_web_x_t *
lh_ui_web_x_get_idx_obj(const lh_ui_web_x_t *self)
{
    return self->u.idx.obj;
}

lh_void
lh_ui_web_x_set_idx_obj(lh_ui_web_x_t *self, const lh_ui_web_x_t *value)
{
    self->u.idx.obj = value;
}

const lh_ui_web_x_t *
lh_ui_web_x_get_idx_ix(const lh_ui_web_x_t *self)
{
    return self->u.idx.ix;
}

lh_void
lh_ui_web_x_set_idx_ix(lh_ui_web_x_t *self, const lh_ui_web_x_t *value)
{
    self->u.idx.ix = value;
}

const lh_ui_web_x_t *
lh_ui_web_x_get_call_fn(const lh_ui_web_x_t *self)
{
    return self->u.call.fn;
}

lh_void
lh_ui_web_x_set_call_fn(lh_ui_web_x_t *self, const lh_ui_web_x_t *value)
{
    self->u.call.fn = value;
}

const lh_ui_web_x_t *const *
lh_ui_web_x_get_call_args(const lh_ui_web_x_t *self)
{
    return self->u.call.args;
}

lh_void
lh_ui_web_x_set_call_args(lh_ui_web_x_t *self, const lh_ui_web_x_t *const *value)
{
    self->u.call.args = value;
}

lh_u16_t
lh_ui_web_x_get_call_n(const lh_ui_web_x_t *self)
{
    return self->u.call.n;
}

lh_void
lh_ui_web_x_set_call_n(lh_ui_web_x_t *self, lh_u16_t value)
{
    self->u.call.n = value;
}

const lh_str_ptr
lh_ui_web_x_get_call_id_name(const lh_ui_web_x_t *self)
{
    return self->u.call_id.name;
}

lh_void
lh_ui_web_x_set_call_id_name(lh_ui_web_x_t *self, const lh_str_ptr value)
{
    self->u.call_id.name = value;
}

const lh_ui_web_x_t *const *
lh_ui_web_x_get_call_id_args(const lh_ui_web_x_t *self)
{
    return self->u.call_id.args;
}

lh_void
lh_ui_web_x_set_call_id_args(lh_ui_web_x_t *self, const lh_ui_web_x_t *const *value)
{
    self->u.call_id.args = value;
}

lh_u16_t
lh_ui_web_x_get_call_id_n(const lh_ui_web_x_t *self)
{
    return self->u.call_id.n;
}

lh_void
lh_ui_web_x_set_call_id_n(lh_ui_web_x_t *self, lh_u16_t value)
{
    self->u.call_id.n = value;
}

const lh_ui_web_x_t *
lh_ui_web_x_get_bin_a(const lh_ui_web_x_t *self)
{
    return self->u.bin.a;
}

lh_void
lh_ui_web_x_set_bin_a(lh_ui_web_x_t *self, const lh_ui_web_x_t *value)
{
    self->u.bin.a = value;
}

const lh_str_ptr
lh_ui_web_x_get_bin_op(const lh_ui_web_x_t *self)
{
    return self->u.bin.op;
}

lh_void
lh_ui_web_x_set_bin_op(lh_ui_web_x_t *self, const lh_str_ptr value)
{
    self->u.bin.op = value;
}

const lh_ui_web_x_t *
lh_ui_web_x_get_bin_b(const lh_ui_web_x_t *self)
{
    return self->u.bin.b;
}

lh_void
lh_ui_web_x_set_bin_b(lh_ui_web_x_t *self, const lh_ui_web_x_t *value)
{
    self->u.bin.b = value;
}

const lh_str_ptr
lh_ui_web_x_get_un_op(const lh_ui_web_x_t *self)
{
    return self->u.un.op;
}

lh_void
lh_ui_web_x_set_un_op(lh_ui_web_x_t *self, const lh_str_ptr value)
{
    self->u.un.op = value;
}

const lh_ui_web_x_t *
lh_ui_web_x_get_un_a(const lh_ui_web_x_t *self)
{
    return self->u.un.a;
}

lh_void
lh_ui_web_x_set_un_a(lh_ui_web_x_t *self, const lh_ui_web_x_t *value)
{
    self->u.un.a = value;
}

const lh_ui_web_x_t *
lh_ui_web_x_get_tern_c(const lh_ui_web_x_t *self)
{
    return self->u.tern.c;
}

lh_void
lh_ui_web_x_set_tern_c(lh_ui_web_x_t *self, const lh_ui_web_x_t *value)
{
    self->u.tern.c = value;
}

const lh_ui_web_x_t *
lh_ui_web_x_get_tern_t(const lh_ui_web_x_t *self)
{
    return self->u.tern.t;
}

lh_void
lh_ui_web_x_set_tern_t(lh_ui_web_x_t *self, const lh_ui_web_x_t *value)
{
    self->u.tern.t = value;
}

const lh_ui_web_x_t *
lh_ui_web_x_get_tern_f(const lh_ui_web_x_t *self)
{
    return self->u.tern.f;
}

lh_void
lh_ui_web_x_set_tern_f(lh_ui_web_x_t *self, const lh_ui_web_x_t *value)
{
    self->u.tern.f = value;
}

const lh_ui_web_x_t *
lh_ui_web_x_get_typeof_a(const lh_ui_web_x_t *self)
{
    return self->u.typeof_a;
}

lh_void
lh_ui_web_x_set_typeof_a(lh_ui_web_x_t *self, const lh_ui_web_x_t *value)
{
    self->u.typeof_a = value;
}

const lh_str_ptr *
lh_ui_web_x_get_obj_keys(const lh_ui_web_x_t *self)
{
    return self->u.obj.keys;
}

lh_void
lh_ui_web_x_set_obj_keys(lh_ui_web_x_t *self, const lh_str_ptr *value)
{
    self->u.obj.keys = value;
}

const lh_ui_web_x_t *const *
lh_ui_web_x_get_obj_vals(const lh_ui_web_x_t *self)
{
    return self->u.obj.vals;
}

lh_void
lh_ui_web_x_set_obj_vals(lh_ui_web_x_t *self, const lh_ui_web_x_t *const *value)
{
    self->u.obj.vals = value;
}

lh_u16_t
lh_ui_web_x_get_obj_n(const lh_ui_web_x_t *self)
{
    return self->u.obj.n;
}

lh_void
lh_ui_web_x_set_obj_n(lh_ui_web_x_t *self, lh_u16_t value)
{
    self->u.obj.n = value;
}

lh_void
lh_ui_web_x_id(const lh_str_ptr name, lh_ui_web_x_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ui_web_x_set_kind(out, lh_ui_web_x_kind_id);
    lh_ui_web_x_set_id(out, name);
}

lh_void
lh_ui_web_x_num(lh_s64_t n, lh_ui_web_x_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ui_web_x_set_kind(out, lh_ui_web_x_kind_num);
    lh_ui_web_x_set_num(out, n);
}

lh_void
lh_ui_web_x_str(const lh_str_ptr s, lh_ui_web_x_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ui_web_x_set_kind(out, lh_ui_web_x_kind_str);
    lh_ui_web_x_set_str(out, s);
}

lh_void
lh_ui_web_x_null(lh_ui_web_x_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ui_web_x_set_kind(out, lh_ui_web_x_kind_null);
}

lh_void
lh_ui_web_x_true(lh_ui_web_x_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ui_web_x_set_kind(out, lh_ui_web_x_kind_true);
}

lh_void
lh_ui_web_x_false(lh_ui_web_x_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ui_web_x_set_kind(out, lh_ui_web_x_kind_false);
}

lh_void
lh_ui_web_x_dot(const lh_ui_web_x_t *obj, const lh_str_ptr name, lh_ui_web_x_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ui_web_x_set_kind(out, lh_ui_web_x_kind_dot);
    lh_ui_web_x_set_dot_obj(out, obj);
    lh_ui_web_x_set_dot_name(out, name);
}

lh_void
lh_ui_web_x_idx(const lh_ui_web_x_t *obj, const lh_ui_web_x_t *ix, lh_ui_web_x_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ui_web_x_set_kind(out, lh_ui_web_x_kind_idx);
    lh_ui_web_x_set_idx_obj(out, obj);
    lh_ui_web_x_set_idx_ix(out, ix);
}

lh_void
lh_ui_web_x_call(const lh_ui_web_x_t *fn, const lh_ui_web_x_t *const *args, lh_u16_t n,
                 lh_ui_web_x_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ui_web_x_set_kind(out, lh_ui_web_x_kind_call);
    lh_ui_web_x_set_call_fn(out, fn);
    lh_ui_web_x_set_call_args(out, args);
    lh_ui_web_x_set_call_n(out, n);
}

lh_void
lh_ui_web_x_call_id(const lh_str_ptr name, const lh_ui_web_x_t *const *args, lh_u16_t n,
                    lh_ui_web_x_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ui_web_x_set_kind(out, lh_ui_web_x_kind_call_id);
    lh_ui_web_x_set_call_id_name(out, name);
    lh_ui_web_x_set_call_id_args(out, args);
    lh_ui_web_x_set_call_id_n(out, n);
}

lh_void
lh_ui_web_x_bin(const lh_ui_web_x_t *a, const lh_str_ptr op, const lh_ui_web_x_t *b,
                lh_ui_web_x_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ui_web_x_set_kind(out, lh_ui_web_x_kind_bin);
    lh_ui_web_x_set_bin_a(out, a);
    lh_ui_web_x_set_bin_op(out, op);
    lh_ui_web_x_set_bin_b(out, b);
}

lh_void
lh_ui_web_x_un(const lh_str_ptr op, const lh_ui_web_x_t *a, lh_ui_web_x_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ui_web_x_set_kind(out, lh_ui_web_x_kind_un);
    lh_ui_web_x_set_un_op(out, op);
    lh_ui_web_x_set_un_a(out, a);
}

lh_void
lh_ui_web_x_tern(const lh_ui_web_x_t *c, const lh_ui_web_x_t *t, const lh_ui_web_x_t *f,
                 lh_ui_web_x_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ui_web_x_set_kind(out, lh_ui_web_x_kind_tern);
    lh_ui_web_x_set_tern_c(out, c);
    lh_ui_web_x_set_tern_t(out, t);
    lh_ui_web_x_set_tern_f(out, f);
}

lh_void
lh_ui_web_x_typeof(const lh_ui_web_x_t *a, lh_ui_web_x_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ui_web_x_set_kind(out, lh_ui_web_x_kind_typeof);
    lh_ui_web_x_set_typeof_a(out, a);
}

lh_void
lh_ui_web_x_obj(const lh_str_ptr *keys, const lh_ui_web_x_t *const *vals, lh_u16_t n,
                lh_ui_web_x_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ui_web_x_set_kind(out, lh_ui_web_x_kind_obj);
    lh_ui_web_x_set_obj_keys(out, keys);
    lh_ui_web_x_set_obj_vals(out, vals);
    lh_ui_web_x_set_obj_n(out, n);
}
