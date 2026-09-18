#include <lh/exception/origin.h>
#include <lh/assert.h>

void
lh_exception_origin_set_timestamp(lh_exception_origin_t *self, lh_str_view_t timestamp)
{
    lh_assert_runtime_ref(self);
    self->timestamp = timestamp;
}

void
lh_exception_origin_set_file(lh_exception_origin_t *self, lh_str_view_t file)
{
    lh_assert_runtime_ref(self);
    self->filename = file;
}

void
lh_exception_origin_set_function(lh_exception_origin_t *self, lh_str_view_t function)
{
    lh_assert_runtime_ref(self);
    self->function = function;
}

void
lh_exception_origin_set_line(lh_exception_origin_t *self, lh_usize_t line)
{
    lh_assert_runtime_ref(self);
    self->line = line;
}

void
lh_exception_origin_set(lh_exception_origin_t *self, lh_str_view_t timestamp, lh_str_view_t file,
                        lh_str_view_t function, lh_usize_t line)
{
    lh_exception_origin_set_timestamp(self, timestamp);
    lh_exception_origin_set_file(self, file);
    lh_exception_origin_set_function(self, function);
    lh_exception_origin_set_line(self, line);
}

void
lh_exception_origin_init(lh_exception_origin_t *self, lh_str_view_t timestamp, lh_str_view_t file,
                         lh_str_view_t function, lh_usize_t line)
{
    lh_exception_origin_set(self, timestamp, file, function, line);
}

void
lh_exception_origin_assign(lh_exception_origin_t *self, const lh_exception_origin_t *other)
{
    lh_exception_origin_set(self, lh_exception_origin_get_timestamp(other),
                            lh_exception_origin_get_file(other),
                            lh_exception_origin_get_function(other),
                            lh_exception_origin_get_line(other));
}

void
lh_exception_origin_init_by_other(lh_exception_origin_t *self, const lh_exception_origin_t *other)
{
    lh_exception_origin_assign(self, other);
}

lh_str_view_t
lh_exception_origin_get_timestamp(const lh_exception_origin_t *self)
{
    lh_assert_runtime_ref(self);
    return self->timestamp;
}

lh_str_view_t
lh_exception_origin_get_file(const lh_exception_origin_t *self)
{
    lh_assert_runtime_ref(self);
    return self->filename;
}

lh_str_view_t
lh_exception_origin_get_function(const lh_exception_origin_t *self)
{
    lh_assert_runtime_ref(self);
    return self->function;
}

lh_usize_t
lh_exception_origin_get_line(const lh_exception_origin_t *self)
{
    lh_assert_runtime_ref(self);
    return self->line;
}
