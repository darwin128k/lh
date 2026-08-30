#include <lh/exception.h>
#include <lh/exception/origin.h>
#include <lh/util/addr.h>
#include <lh/assert.h>

#ifndef NDEBUG
void
lh_exception_set(lh_exception_t *self, lh_error_code_t code, lh_error_desc_t desc,
                 const lh_str_ptr timestamp, const lh_str_ptr file, const lh_str_ptr function,
                 lh_usize_t line)
{
    lh_error_init(lh_exception_get_error(self), code, desc);
    lh_exception_origin_init(lh_exception_get_origin(self), timestamp, file, function, line);
}

void
lh_exception_init(lh_exception_t *self, lh_error_code_t code, lh_error_desc_t desc,
                  const lh_str_ptr timestamp, const lh_str_ptr file, const lh_str_ptr function,
                  lh_usize_t line)
{
    lh_exception_set(self, code, desc, timestamp, file, function, line);
}
#else
void
lh_exception_set(lh_exception_t *self, lh_error_code_t code, lh_error_desc_t desc)
{
    lh_error_init(lh_exception_get_error(self), code, desc);
}

void
lh_exception_init(lh_exception_t *self, lh_error_code_t code, lh_error_desc_t desc)
{
    lh_exception_set(self, code, desc);
}
#endif

#ifndef NDEBUG
void
lh_exception_init_by_error(lh_exception_t *self, const lh_error_t *error,
                           const lh_exception_origin_t *origin)
{
    lh_error_init_by_other(lh_exception_get_error(self), error);
    lh_exception_origin_init_by_other(lh_exception_get_origin(self), origin);
}
#else
void
lh_exception_init_by_error(lh_exception_t *self, const lh_error_t *error)
{
    lh_error_init_by_other(lh_exception_get_error(self), error);
}
#endif

#ifndef NDEBUG
void
lh_exception_unpack(const lh_exception_t *self, lh_error_code_t *code, lh_error_desc_t *desc,
                    lh_str_cptr *timestamp, lh_str_cptr *file, lh_str_cptr *function,
                    lh_usize_t *line)
{
    lh_error_unpack(lh_exception_get_error_as_const(self), code, desc);
    lh_exception_origin_unpack(lh_exception_get_origin_as_const(self), timestamp, file, function,
                               line);
}
#else
void
lh_exception_unpack(const lh_exception_t *self, lh_error_code_t *code, lh_error_desc_t *desc)
{
    lh_error_unpack(lh_exception_get_error_as_const(self), code, desc);
}
#endif

void
lh_exception_unpack_to_other(const lh_exception_t *self, lh_exception_t *other)
{
    lh_error_unpack_to_other(lh_exception_get_error_as_const(self), lh_exception_get_error(other));
#ifndef NDEBUG
    lh_exception_origin_unpack_to_other(lh_exception_get_origin_as_const(self),
                                        lh_exception_get_origin(other));
#endif
}

lh_error_t *
lh_exception_get_error(lh_exception_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->error);
}

const lh_error_t *
lh_exception_get_error_as_const(const lh_exception_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->error);
}

#ifndef NDEBUG
lh_exception_origin_t *
lh_exception_get_origin(lh_exception_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->origin);
}

const lh_exception_origin_t *
lh_exception_get_origin_as_const(const lh_exception_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->origin);
}
#endif

void
lh_exception_set_code(lh_exception_t *self, lh_error_code_t code)
{
    lh_error_set_code(lh_exception_get_error(self), code);
}

void
lh_exception_set_desc(lh_exception_t *self, lh_error_desc_t desc)
{
    lh_error_set_desc(lh_exception_get_error(self), desc);
}

lh_error_code_t
lh_exception_get_code(const lh_exception_t *self)
{
    return lh_error_get_code(lh_exception_get_error_as_const(self));
}

lh_error_desc_t
lh_exception_get_desc(const lh_exception_t *self)
{
    return lh_error_get_desc(lh_exception_get_error_as_const(self));
}

lh_error_desc_t
lh_exception_get_desc_or(const lh_exception_t *self, lh_error_desc_t fallback)
{
    return lh_error_get_desc_or(lh_exception_get_error_as_const(self), fallback);
}

lh_bool_t
lh_exception_has_code(const lh_exception_t *self, lh_error_code_t code)
{
    return lh_error_has_code(lh_exception_get_error_as_const(self), code);
}

lh_bool_t
lh_exception_is_ok(const lh_exception_t *self)
{
    return lh_error_is_ok(lh_exception_get_error_as_const(self));
}

lh_bool_t
lh_exception_is_failure(const lh_exception_t *self)
{
    return lh_error_is_failure(lh_exception_get_error_as_const(self));
}

lh_bool_t
lh_exception_has_desc(const lh_exception_t *self)
{
    return lh_error_has_desc(lh_exception_get_error_as_const(self));
}

lh_bool_t
lh_exception_is_empty(const lh_exception_t *self)
{
    return lh_error_is_empty(lh_exception_get_error_as_const(self));
}

lh_bool_t
lh_exception_equals(const lh_exception_t *self, const lh_exception_t *other)
{
    return lh_error_equals(lh_exception_get_error_as_const(self),
                           lh_exception_get_error_as_const(other));
}

lh_bool_t
lh_exception_has_same_code(const lh_exception_t *self, const lh_exception_t *other)
{
    return lh_error_has_same_code(lh_exception_get_error_as_const(self),
                                  lh_exception_get_error_as_const(other));
}

lh_bool_t
lh_exception_has_diff_code(const lh_exception_t *self, const lh_exception_t *other)
{
    return lh_error_has_diff_code(lh_exception_get_error_as_const(self),
                                  lh_exception_get_error_as_const(other));
}
