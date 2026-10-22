#include <lh/ini/entry.h>
#include <lh/assert.h>
#include <lh/cast/static.h>
#include <lh/math/ops.h>
#include <lh/null.h>
#include <lh/runtime/error.h>

/* ── accessors ───────────────────────────────────────────────────────────── */

lh_ini_entry_kind_t
lh_ini_entry_get_kind(const lh_ini_entry_t *self)
{
    lh_assert_runtime_ref(self);
    return self->kind;
}

lh_void
lh_ini_entry_set_kind(lh_ini_entry_t *self, lh_ini_entry_kind_t kind)
{
    lh_assert_runtime_ref(self);
    self->kind = kind;
}

lh_str_view_t
lh_ini_entry_get_section(const lh_ini_entry_t *self)
{
    lh_assert_runtime_ref(self);
    return self->section;
}

lh_void
lh_ini_entry_set_section(lh_ini_entry_t *self, lh_str_view_t section)
{
    lh_assert_runtime_ref(self);
    self->section = section;
}

lh_void
lh_ini_entry_set_pair(lh_ini_entry_t *self, lh_pair_t pair)
{
    lh_assert_runtime_ref(self);
    lh_pair_assign(lh_addr_of(self->pair), lh_addr_of(pair));
}

const lh_pair_t *
lh_ini_entry_get_pair(const lh_ini_entry_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->pair);
}

lh_bool_t
lh_ini_entry_has_kind(const lh_ini_entry_t *self, lh_ini_entry_kind_t kind)
{
    return lh_cast_static(lh_bool_t, lh_math_eq(lh_ini_entry_get_kind(self), kind));
}

const lh_pair_t *
lh_ini_entry_get_pair_v(const lh_ini_entry_t *self)
{
    lh_assert_runtime_if(!lh_ini_entry_has_kind(self, lh_ini_entry_kind_pair),
                         lh_runtime_error_code_invalid_argument, "entry is not a pair");
    return lh_ini_entry_get_pair(self);
}

lh_str_view_t
lh_ini_entry_get_key(const lh_ini_entry_t *self)
{
    return lh_value_get_str(lh_pair_get_first(lh_ini_entry_get_pair_v(self)));
}

lh_str_view_t
lh_ini_entry_get_value(const lh_ini_entry_t *self)
{
    return lh_value_get_str(lh_pair_get_second(lh_ini_entry_get_pair_v(self)));
}

/* ── init / make ─────────────────────────────────────────────────────────── */

lh_void
lh_ini_entry_set(lh_ini_entry_t *self, lh_ini_entry_kind_t kind, lh_str_view_t section,
                 lh_pair_t pair)
{
    lh_ini_entry_set_kind(self, kind);
    lh_ini_entry_set_section(self, section);
    lh_ini_entry_set_pair(self, pair);
}

lh_void
lh_ini_entry_init(lh_ini_entry_t *self)
{
    lh_ini_entry_set(self, lh_ini_entry_kind_none, lh_str_view_empty(), lh_pair_make_none());
}

lh_void
lh_ini_entry_assign(lh_ini_entry_t *self, const lh_ini_entry_t *other)
{
    lh_ini_entry_set(self, lh_ini_entry_get_kind(other), lh_ini_entry_get_section(other),
                     lh_ptr_deref(lh_ini_entry_get_pair(other)));
}

lh_ini_entry_t
lh_ini_entry_make_section(lh_str_view_t name)
{
    lh_ini_entry_t self;

    lh_ini_entry_set(lh_addr_of(self), lh_ini_entry_kind_section, name, lh_pair_make_none());
    return self;
}

lh_ini_entry_t
lh_ini_entry_make_pair(lh_str_view_t section, lh_str_view_t key, lh_str_view_t value)
{
    lh_ini_entry_t self;

    lh_ini_entry_set(lh_addr_of(self), lh_ini_entry_kind_pair, section,
                     lh_pair_make(lh_value_make_str(key), lh_value_make_str(value)));
    return self;
}
