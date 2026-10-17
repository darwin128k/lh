#include <lh/exception/origin.h>
#include <lh/util/str/ptr/empty.h>
#include <lh/assert.h>

/* -- getters --------------------------------------------------------------- */

const lh_str_ptr
lh_exception_origin_get_file(const lh_exception_origin_t *self)
{
    lh_assert_runtime_ref(self);
    return self->file;
}

const lh_str_ptr
lh_exception_origin_get_function(const lh_exception_origin_t *self)
{
    lh_assert_runtime_ref(self);
    return self->function;
}

const lh_str_ptr
lh_exception_origin_get_condition(const lh_exception_origin_t *self)
{
    lh_assert_runtime_ref(self);
    return self->condition;
}

const lh_str_ptr
lh_exception_origin_get_message(const lh_exception_origin_t *self)
{
    lh_assert_runtime_ref(self);
    return self->message;
}

lh_uint_t
lh_exception_origin_get_line(const lh_exception_origin_t *self)
{
    lh_assert_runtime_ref(self);
    return self->line;
}

/* -- predicates ------------------------------------------------------------ */

lh_bool_t
lh_exception_origin_has_file(const lh_exception_origin_t *self)
{
    return !lh_str_ptr_is_empty(lh_exception_origin_get_file(self));
}

lh_bool_t
lh_exception_origin_has_function(const lh_exception_origin_t *self)
{
    return !lh_str_ptr_is_empty(lh_exception_origin_get_function(self));
}

lh_bool_t
lh_exception_origin_has_condition(const lh_exception_origin_t *self)
{
    return !lh_str_ptr_is_empty(lh_exception_origin_get_condition(self));
}

lh_bool_t
lh_exception_origin_has_message(const lh_exception_origin_t *self)
{
    return !lh_str_ptr_is_empty(lh_exception_origin_get_message(self));
}
