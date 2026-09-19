#include <lh/os/fs/path.h>
#include "path/local.h"
#include <lh/assert.h>
#include <lh/str/view.h>
#include <lh/str/view/initializer.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

lh_char_t *
lh_os_fs_path_get_sep(lh_os_fs_path_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->sep);
}

const lh_char_t *
lh_os_fs_path_get_sep_as_const(const lh_os_fs_path_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->sep);
}

lh_str_t *
lh_os_fs_path_get_text(lh_os_fs_path_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->text);
}

const lh_str_t *
lh_os_fs_path_get_text_as_const(const lh_os_fs_path_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->text);
}

lh_vector_t *
lh_os_fs_path_get_parts(lh_os_fs_path_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->parts);
}

const lh_vector_t *
lh_os_fs_path_get_parts_as_const(const lh_os_fs_path_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->parts);
}

static const lh_os_fs_path_span_t *
lh_os_fs_path_get_span(const lh_os_fs_path_t *self, lh_uindex_t index)
{
    return lh_ptr_cast(const lh_os_fs_path_span_t,
                       lh_vector_get_ptr(lh_os_fs_path_get_parts_as_const(self), index));
}

lh_str_view_t
lh_os_fs_path_get_part(const lh_os_fs_path_t *self, lh_uindex_t index)
{
    const lh_os_fs_path_span_t *span;
    lh_str_view_t view;

    span = lh_os_fs_path_get_span(self, index);
    if (span->size == 0U)
    {
        lh_str_view_init_empty(lh_addr_of(view));
        return view;
    }
    lh_str_init_by_size(
        lh_addr_of(view),
        lh_ptr_add_by_offset(const lh_char_t,
                             lh_str_get_data(lh_os_fs_path_get_text_as_const(self)), span->offset),
        span->size);
    return view;
}

lh_str_view_t
lh_os_fs_path_as_view(const lh_os_fs_path_t *self)
{
    const lh_str_t *text;
    lh_str_view_t view;

    text = lh_os_fs_path_get_text_as_const(self);
    if (lh_str_is_empty(text))
    {
        lh_str_view_init_empty(lh_addr_of(view));
        return view;
    }
    return lh_str_as_view(text);
}

lh_bool_t
lh_os_fs_path_is_empty(const lh_os_fs_path_t *self)
{
    return lh_vector_is_empty(lh_os_fs_path_get_parts_as_const(self));
}

lh_char_t
lh_os_fs_path_sep(void)
{
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    return '\\';
#else
    return '/';
#endif
}

void
lh_os_fs_path_clear(lh_os_fs_path_t *self)
{
    lh_str_clear(lh_os_fs_path_get_text(self));
    lh_vector_clear(lh_os_fs_path_get_parts(self));
}

void
lh_os_fs_path_init(lh_os_fs_path_t *self)
{
    lh_ptr_deref(lh_os_fs_path_get_sep(self)) = lh_os_fs_path_sep();
    lh_str_init(lh_os_fs_path_get_text(self));
    lh_vector_init(lh_os_fs_path_get_parts(self), sizeof(lh_os_fs_path_span_t));
}

void
lh_os_fs_path_deinit(lh_os_fs_path_t *self)
{
    lh_str_deinit(lh_os_fs_path_get_text(self));
    lh_vector_deinit(lh_os_fs_path_get_parts(self));
}

void
lh_os_fs_path_assign(lh_os_fs_path_t *self, const lh_os_fs_path_t *other)
{
    const lh_str_t *src;
    const lh_vector_t *parts;
    lh_usize_t n;

    lh_assert_runtime_ref(other);
    if (self == other)
    {
        return;
    }
    lh_os_fs_path_clear(self);
    lh_ptr_deref(lh_os_fs_path_get_sep(self)) =
        lh_ptr_deref(lh_os_fs_path_get_sep_as_const(other));
    src = lh_os_fs_path_get_text_as_const(other);
    if (!lh_str_is_empty(src))
    {
        lh_str_append(lh_os_fs_path_get_text(self), lh_str_get_data(src), lh_str_get_size(src));
    }
    parts = lh_os_fs_path_get_parts_as_const(other);
    n = lh_vector_get_size(parts);
    if (n > 0U)
    {
        lh_vector_push_back_of(lh_os_fs_path_get_parts(self), lh_vector_get_data(parts), n);
    }
}
