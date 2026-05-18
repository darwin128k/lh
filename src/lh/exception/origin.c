#include <lh/exception/origin.h>
#include <lh/assert.h>

void
lh_exception_origin_set(lh_exception_origin_t *self, lh_str_cptr timestamp,
                        lh_str_cptr file, lh_str_cptr function, lh_usize_t line)
{
    lh_assert_runtime_ref(self);
    self->timestamp = timestamp;
    self->filename  = file;
    self->function  = function;
    self->line      = line;
}

void
lh_exception_origin_init(lh_exception_origin_t *self, lh_str_cptr timestamp,
                         lh_str_cptr file, lh_str_cptr function, lh_usize_t line)
{
    lh_exception_origin_set(self, timestamp, file, function, line);
}

void
lh_exception_origin_unpack(const lh_exception_origin_t *self, lh_str_cptr *timestamp,
                           lh_str_cptr *file, lh_str_cptr *function, lh_usize_t *line)
{
    lh_assert_runtime_ref(self);
    if (timestamp) *timestamp = self->timestamp;
    if (file)      *file      = self->filename;
    if (function)  *function  = self->function;
    if (line)      *line      = self->line;
}

lh_str_cptr
lh_exception_origin_get_timestamp(const lh_exception_origin_t *self)
{
    lh_assert_runtime_ref(self);
    return self->timestamp;
}

lh_str_cptr
lh_exception_origin_get_file(const lh_exception_origin_t *self)
{
    lh_assert_runtime_ref(self);
    return self->filename;
}

lh_str_cptr
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

void
lh_exception_origin_assign(lh_exception_origin_t *self, const lh_exception_origin_t *other)
{
    lh_assert_runtime_ref(other);
    lh_exception_origin_set(self, other->timestamp, other->filename, other->function, other->line);
}

void
lh_exception_origin_init_by_other(lh_exception_origin_t *self,
                                  const lh_exception_origin_t *other)
{
    lh_exception_origin_assign(self, other);
}

void
lh_exception_origin_unpack_to_other(const lh_exception_origin_t *self,
                                    lh_exception_origin_t *other)
{
    lh_exception_origin_assign(other, self);
}
