#include <lh/json/node.h>
#include <lh/assert.h>
#include <lh/json/reader.h>
#include <lh/cast/const.h>
#include <lh/runtime/allocator.h>
#include <lh/str/format/real.h>
#include <lh/str/format/sllong.h>
#include <lh/str/parse/real.h>
#include <lh/str/parse/sllong.h>
#include <lh/str/parse/ullong.h>

/* ── accessors ───────────────────────────────────────────────────────────── */

lh_list_node_t *
lh_json_node_get_link_mut(lh_json_node_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->link);
}

const lh_list_node_t *
lh_json_node_get_link(const lh_json_node_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->link);
}

lh_json_node_t *
lh_json_node_get_by_link_mut(lh_list_node_t *link)
{
    return lh_list_entry(lh_json_node_t, link, link);
}

const lh_json_node_t *
lh_json_node_get_by_link(const lh_list_node_t *link)
{
    return lh_list_entry(const lh_json_node_t, link, link);
}

lh_json_kind_t
lh_json_node_get_kind(const lh_json_node_t *self)
{
    lh_assert_runtime_ref(self);
    return self->kind;
}

lh_void
lh_json_node_set_kind(lh_json_node_t *self, lh_json_kind_t kind)
{
    lh_assert_runtime_ref(self);
    self->kind = kind;
}

lh_bool_t
lh_json_node_get_flag(const lh_json_node_t *self)
{
    lh_assert_runtime_ref(self);
    return self->flag;
}

lh_void
lh_json_node_set_flag(lh_json_node_t *self, lh_bool_t flag)
{
    lh_assert_runtime_ref(self);
    self->flag = flag;
}

lh_str_t *
lh_json_node_get_key_mut(lh_json_node_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->key);
}

const lh_str_t *
lh_json_node_get_key(const lh_json_node_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->key);
}

lh_str_t *
lh_json_node_get_text_mut(lh_json_node_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->text);
}

const lh_str_t *
lh_json_node_get_text(const lh_json_node_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->text);
}

lh_list_t *
lh_json_node_get_children_mut(lh_json_node_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->children);
}

const lh_list_t *
lh_json_node_get_children(const lh_json_node_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->children);
}

lh_bool_t
lh_json_node_has_kind(const lh_json_node_t *self, lh_json_kind_t kind)
{
    return lh_cast_static(lh_bool_t, lh_math_eq(lh_json_node_get_kind(self), kind));
}

/* ── init / deinit ───────────────────────────────────────────────────────── */

lh_void
lh_json_node_init(lh_json_node_t *self)
{
    lh_list_node_init(lh_json_node_get_link_mut(self));
    lh_json_node_set_kind(self, lh_json_kind_none);
    lh_json_node_set_flag(self, lh_bool_false);
    lh_str_init(lh_json_node_get_key_mut(self));
    lh_str_init(lh_json_node_get_text_mut(self));
    lh_list_init(lh_json_node_get_children_mut(self));
}

lh_void
lh_json_node_clear(lh_json_node_t *self)
{
    lh_list_t *children = lh_json_node_get_children_mut(self);
    lh_list_node_t *link;

    for (link = lh_list_pop_front(children); lh_null_ne(link); link = lh_list_pop_front(children))
    {
        lh_json_node_destroy(lh_json_node_get_by_link_mut(link));
    }
    lh_str_clear(lh_json_node_get_text_mut(self));
    lh_json_node_set_flag(self, lh_bool_false);
    lh_json_node_set_kind(self, lh_json_kind_none);
}

lh_void
lh_json_node_deinit(lh_json_node_t *self)
{
    lh_json_node_clear(self);
    lh_str_deinit(lh_json_node_get_key_mut(self));
    lh_str_deinit(lh_json_node_get_text_mut(self));
}

lh_json_node_t *
lh_json_node_create(lh_void)
{
    lh_json_node_t *self =
        lh_ptr_cast(lh_json_node_t, lh_runtime_allocator_alloc(sizeof(lh_json_node_t)));

    lh_json_node_init(self);
    return self;
}

lh_void
lh_json_node_destroy(lh_json_node_t *self)
{
    lh_list_node_unlink(lh_json_node_get_link_mut(self));
    lh_json_node_deinit(self);
    lh_runtime_allocator_free(self);
}

/* ── children ────────────────────────────────────────────────────────────── */

lh_void
lh_json_node_push_back(lh_json_node_t *self, lh_json_node_t *child)
{
    lh_list_push_back(lh_json_node_get_children_mut(self), lh_json_node_get_link_mut(child));
}

lh_json_node_t *
lh_json_node_get_first_mut(lh_json_node_t *self)
{
    return lh_json_node_get_by_link_mut(lh_list_get_first(lh_json_node_get_children(self)));
}

const lh_json_node_t *
lh_json_node_get_first(const lh_json_node_t *self)
{
    return lh_json_node_get_by_link(lh_list_get_first(lh_json_node_get_children(self)));
}

lh_json_node_t *
lh_json_node_get_next_mut(lh_json_node_t *self, lh_json_node_t *child)
{
    return lh_json_node_get_by_link_mut(
        lh_list_get_next(lh_json_node_get_children(self), lh_json_node_get_link(child)));
}

const lh_json_node_t *
lh_json_node_get_next(const lh_json_node_t *self, const lh_json_node_t *child)
{
    return lh_json_node_get_by_link(
        lh_list_get_next(lh_json_node_get_children(self), lh_json_node_get_link(child)));
}

lh_usize_t
lh_json_node_get_size(const lh_json_node_t *self)
{
    return lh_list_get_size(lh_json_node_get_children(self));
}

lh_json_node_t *
lh_json_node_get_at_mut(lh_json_node_t *self, lh_uindex_t index)
{
    return lh_json_node_get_by_link_mut(lh_list_get_at(lh_json_node_get_children(self), index));
}

const lh_json_node_t *
lh_json_node_get_at(const lh_json_node_t *self, lh_uindex_t index)
{
    return lh_json_node_get_by_link(lh_list_get_at(lh_json_node_get_children(self), index));
}

lh_bool_t
lh_json_node_has_key(const lh_json_node_t *self, lh_str_view_t key)
{
    lh_str_view_t name;

    lh_str_as_view(lh_json_node_get_key(self), lh_addr_of(name));
    return lh_str_view_equals(lh_addr_of(name), lh_addr_of(key), lh_bool_false);
}

const lh_json_node_t *
lh_json_node_get_member(const lh_json_node_t *self, lh_str_view_t key)
{
    const lh_json_node_t *child;

    if (!lh_json_node_has_kind(self, lh_json_kind_object))
    {
        return lh_null;
    }
    for (child = lh_json_node_get_first(self); lh_null_ne(child);
         child = lh_json_node_get_next(self, child))
    {
        if (lh_json_node_has_key(child, key))
        {
            return child;
        }
    }
    return lh_null;
}

lh_json_node_t *
lh_json_node_get_member_mut(lh_json_node_t *self, lh_str_view_t key)
{
    /* The search only reads; the caller owns @p self mutably, so handing the
       found member back mutable gives away nothing it did not have. */
    return lh_cast_const(lh_json_node_t *, lh_json_node_get_member(self, key));
}

/* ── read (kind checked) ─────────────────────────────────────────────────── */

lh_bool_t
lh_json_node_get_bool(const lh_json_node_t *self)
{
    lh_assert_runtime_if(!lh_json_node_has_kind(self, lh_json_kind_bool),
                         lh_runtime_error_code_invalid_argument, "json node is not a bool");
    return lh_json_node_get_flag(self);
}

lh_str_view_t
lh_json_node_get_str(const lh_json_node_t *self)
{
    lh_str_view_t view;

    lh_assert_runtime_if(!lh_json_node_has_kind(self, lh_json_kind_string),
                         lh_runtime_error_code_invalid_argument, "json node is not a string");
    lh_str_as_view(lh_json_node_get_text(self), lh_addr_of(view));
    return view;
}

lh_bool_t
lh_json_node_get_sint(const lh_json_node_t *self, lh_sllong_t *out)
{
    const lh_str_t *text = lh_json_node_get_text(self);

    if (!lh_json_node_has_kind(self, lh_json_kind_number))
    {
        return lh_bool_false;
    }
    return lh_str_ptr_parse_sllong(lh_str_get_data(text), lh_str_get_size(text),
                                   lh_numeric_limit_min(lh_sllong_t),
                                   lh_numeric_limit_max(lh_sllong_t), out);
}

lh_bool_t
lh_json_node_get_uint(const lh_json_node_t *self, lh_ullong_t *out)
{
    const lh_str_t *text = lh_json_node_get_text(self);

    if (!lh_json_node_has_kind(self, lh_json_kind_number))
    {
        return lh_bool_false;
    }
    return lh_str_ptr_parse_ullong(lh_str_get_data(text), lh_str_get_size(text),
                                   lh_numeric_limit_max(lh_ullong_t), out);
}

#if LH_REAL_ENABLED
lh_bool_t
lh_json_node_get_real(const lh_json_node_t *self, lh_real_t *out)
{
    const lh_str_t *text = lh_json_node_get_text(self);

    if (!lh_json_node_has_kind(self, lh_json_kind_number))
    {
        return lh_bool_false;
    }
    return lh_str_ptr_parse_real(lh_str_get_data(text), lh_str_get_size(text), out);
}
#endif

/* ── read with a fallback ────────────────────────────────────────────────── */

lh_bool_t
lh_json_node_get_bool_or(const lh_json_node_t *self, lh_bool_t fallback)
{
    return lh_null_ne(self) && lh_json_node_has_kind(self, lh_json_kind_bool)
               ? lh_json_node_get_bool(self)
               : fallback;
}

lh_str_view_t
lh_json_node_get_str_or(const lh_json_node_t *self, lh_str_view_t fallback)
{
    return lh_null_ne(self) && lh_json_node_has_kind(self, lh_json_kind_string)
               ? lh_json_node_get_str(self)
               : fallback;
}

lh_sllong_t
lh_json_node_get_sint_or(const lh_json_node_t *self, lh_sllong_t fallback)
{
    lh_sllong_t value;

    return lh_null_ne(self) && lh_json_node_get_sint(self, lh_addr_of(value)) ? value : fallback;
}

lh_ullong_t
lh_json_node_get_uint_or(const lh_json_node_t *self, lh_ullong_t fallback)
{
    lh_ullong_t value;

    return lh_null_ne(self) && lh_json_node_get_uint(self, lh_addr_of(value)) ? value : fallback;
}

#if LH_REAL_ENABLED
lh_real_t
lh_json_node_get_real_or(const lh_json_node_t *self, lh_real_t fallback)
{
    lh_real_t value;

    return lh_null_ne(self) && lh_json_node_get_real(self, lh_addr_of(value)) ? value : fallback;
}
#endif

/* ── write values ────────────────────────────────────────────────────────── */

lh_void
lh_json_node_set_null(lh_json_node_t *self)
{
    lh_json_node_clear(self);
    lh_json_node_set_kind(self, lh_json_kind_null);
}

lh_void
lh_json_node_set_bool(lh_json_node_t *self, lh_bool_t value)
{
    lh_json_node_clear(self);
    lh_json_node_set_kind(self, lh_json_kind_bool);
    lh_json_node_set_flag(self, value);
}

lh_void
lh_json_node_set_sint(lh_json_node_t *self, lh_sllong_t value)
{
    lh_char_t digits[LH_JSON_NODE_INTEGER_DIGITS_MAX];

    lh_json_node_clear(self);
    lh_json_node_set_kind(self, lh_json_kind_number);
    lh_str_append(lh_json_node_get_text_mut(self), digits,
                  lh_str_ptr_format_sllong(value, digits, LH_JSON_NODE_INTEGER_DIGITS_MAX));
}

lh_void
lh_json_node_set_uint(lh_json_node_t *self, lh_ullong_t value)
{
    lh_char_t digits[LH_JSON_NODE_INTEGER_DIGITS_MAX];

    lh_json_node_clear(self);
    lh_json_node_set_kind(self, lh_json_kind_number);
    lh_str_append(lh_json_node_get_text_mut(self), digits,
                  lh_str_ptr_format_ullong(value, digits, LH_JSON_NODE_INTEGER_DIGITS_MAX));
}

lh_bool_t
lh_json_node_set_number(lh_json_node_t *self, lh_str_view_t text)
{
    lh_json_reader_t reader;

    lh_json_node_clear(self);
    lh_json_reader_init(lh_addr_of(reader), text);
    if (!lh_json_reader_read_number(lh_addr_of(reader), self) ||
        !lh_json_reader_is_end(lh_addr_of(reader)))
    {
        lh_json_node_clear(self);
        return lh_bool_false;
    }
    return lh_bool_true;
}

#if LH_REAL_ENABLED
lh_bool_t
lh_json_node_set_real(lh_json_node_t *self, lh_real_t value)
{
    lh_char_t digits[LH_STR_FORMAT_REAL_SIZE_MAX];
    const lh_usize_t size = lh_str_ptr_format_real(value, digits, LH_STR_FORMAT_REAL_SIZE_MAX);

    lh_json_node_clear(self);
    if (lh_math_is_zero(size))
    {
        return lh_bool_false;
    }
    lh_json_node_set_kind(self, lh_json_kind_number);
    lh_str_append(lh_json_node_get_text_mut(self), digits, size);
    return lh_bool_true;
}
#endif

lh_void
lh_json_node_set_str(lh_json_node_t *self, lh_str_view_t value)
{
    lh_json_node_clear(self);
    lh_json_node_set_kind(self, lh_json_kind_string);
    lh_str_assign_view(lh_json_node_get_text_mut(self), value);
}

lh_void
lh_json_node_set_array(lh_json_node_t *self)
{
    lh_json_node_clear(self);
    lh_json_node_set_kind(self, lh_json_kind_array);
}

lh_void
lh_json_node_set_object(lh_json_node_t *self)
{
    lh_json_node_clear(self);
    lh_json_node_set_kind(self, lh_json_kind_object);
}

lh_void
lh_json_node_set_key(lh_json_node_t *self, lh_str_view_t key)
{
    lh_str_assign_view(lh_json_node_get_key_mut(self), key);
}

/* ── write children ──────────────────────────────────────────────────────── */

lh_void
lh_json_node_insert_before(lh_json_node_t *at, lh_json_node_t *child)
{
    lh_list_node_insert_before(lh_json_node_get_link_mut(child), lh_json_node_get_link_mut(at));
}

lh_void
lh_json_node_add_member(lh_json_node_t *self, lh_str_view_t key, lh_json_node_t *child)
{
    lh_json_node_set_key(child, key);
    lh_json_node_push_back(self, child);
}

lh_void
lh_json_node_detach(lh_json_node_t *self)
{
    lh_list_node_unlink(lh_json_node_get_link_mut(self));
}

lh_bool_t
lh_json_node_remove_member(lh_json_node_t *self, lh_str_view_t key)
{
    lh_json_node_t *member = lh_json_node_get_member_mut(self, key);

    if (lh_null_eq(member))
    {
        return lh_bool_false;
    }
    lh_json_node_destroy(member);
    return lh_bool_true;
}

lh_void
lh_json_node_replace(lh_json_node_t *self, lh_json_node_t *with)
{
    lh_str_assign(lh_json_node_get_key_mut(with), lh_json_node_get_key(self));
    lh_json_node_insert_before(self, with);
    lh_json_node_destroy(self);
}

lh_void
lh_json_node_assign(lh_json_node_t *self, const lh_json_node_t *other)
{
    const lh_json_node_t *child;

    lh_json_node_clear(self);
    lh_json_node_set_kind(self, lh_json_node_get_kind(other));
    lh_json_node_set_flag(self, lh_json_node_get_flag(other));
    lh_str_assign(lh_json_node_get_text_mut(self), lh_json_node_get_text(other));
    for (child = lh_json_node_get_first(other); lh_null_ne(child);
         child = lh_json_node_get_next(other, child))
    {
        lh_json_node_t *copy = lh_json_node_create();

        lh_str_assign(lh_json_node_get_key_mut(copy), lh_json_node_get_key(child));
        lh_json_node_assign(copy, child);
        lh_json_node_push_back(self, copy);
    }
}

lh_bool_t
lh_json_node_equals(const lh_json_node_t *self, const lh_json_node_t *other)
{
    const lh_json_kind_t kind = lh_json_node_get_kind(self);
    const lh_json_node_t *a;
    const lh_json_node_t *b;
    lh_str_view_t text_a;
    lh_str_view_t text_b;

    if (lh_math_ne(kind, lh_json_node_get_kind(other)))
    {
        return lh_bool_false;
    }
    switch (kind)
    {
    case lh_json_kind_bool:
        return lh_cast_static(
            lh_bool_t, lh_bool_eq(lh_json_node_get_flag(self), lh_json_node_get_flag(other)));
    case lh_json_kind_number:
    case lh_json_kind_string:
        lh_str_as_view(lh_json_node_get_text(self), lh_addr_of(text_a));
        lh_str_as_view(lh_json_node_get_text(other), lh_addr_of(text_b));
        return lh_str_view_equals(lh_addr_of(text_a), lh_addr_of(text_b), lh_bool_false);
    case lh_json_kind_array:
        for (a = lh_json_node_get_first(self), b = lh_json_node_get_first(other);
             lh_null_ne(a) && lh_null_ne(b);
             a = lh_json_node_get_next(self, a), b = lh_json_node_get_next(other, b))
        {
            if (!lh_json_node_equals(a, b))
            {
                return lh_bool_false;
            }
        }
        return lh_cast_static(lh_bool_t, lh_null_eq(a) && lh_null_eq(b));
    case lh_json_kind_object:
        if (lh_math_ne(lh_json_node_get_size(self), lh_json_node_get_size(other)))
        {
            return lh_bool_false;
        }
        for (a = lh_json_node_get_first(self); lh_null_ne(a); a = lh_json_node_get_next(self, a))
        {
            lh_str_as_view(lh_json_node_get_key(a), lh_addr_of(text_a));
            b = lh_json_node_get_member(other, text_a);
            if (lh_null_eq(b) || !lh_json_node_equals(a, b))
            {
                return lh_bool_false;
            }
        }
        return lh_bool_true;
    default:
        return lh_bool_true;
    }
}
