#include <lh/assert.h>
#include <lh/attribute/static.h>
#include <lh/memory/std.h>
#include <lh/str/format/text.h>
#include <lh/util/str/ptr.h>
#include <lh/util/str/ptr/empty.h>
#include <lh/math.h>

#include <stdarg.h>

const lh_runtime_error_t *
lh_exception_get_error(const lh_exception_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->error);
}

lh_runtime_error_code_t
lh_exception_get_code(const lh_exception_t *self)
{
    return lh_runtime_error_get_code(lh_exception_get_error(self));
}

const lh_exception_origin_t *
lh_exception_get_origin(const lh_exception_t *self)
{
    lh_assert_runtime_ref(self);
    return self->origin;
}

/* One part of the report at @p pos: counted when @p str is lh_null, written
   otherwise (the caller checked the whole report fits). Returns the position
   after it — so counting and writing can never disagree. */
lh_usize_t
lh_exception_format_put(lh_str_ptr str, lh_usize_t str_size, lh_usize_t pos, lh_str_cptr fmt, ...)
{
    va_list args;
    lh_usize_t n;

    va_start(args, fmt);
    n = lh_ptr_is_null(str)
            ? lh_str_ptr_format_text_size_v(fmt, args)
            : lh_str_ptr_format_text_v(str + pos, lh_math_sub(str_size, pos), fmt, args);
    va_end(args);
    return lh_math_add(pos, n);
}

/* The whole report: its size when @p str is lh_null, else written into @p str. */
lh_usize_t
lh_exception_format_walk(const lh_exception_t *self, lh_str_ptr str, lh_usize_t str_size)
{
    const lh_exception_origin_t *origin = lh_exception_get_origin(self);
    const lh_str_cptr desc = lh_runtime_error_get_desc(lh_exception_get_error(self));
    lh_usize_t pos;

    pos = lh_exception_format_put(str, str_size, 0U, "lh: runtime check failed: error %d",
                                  lh_cast_static(lh_sint_t, lh_exception_get_code(self)));
    if (!lh_str_ptr_is_empty(desc))
    {
        const lh_usize_t desc_size = lh_str_ptr_len(desc);

        pos = lh_exception_format_put(str, str_size, pos, ", ");
        if (lh_ptr_is_set(str))
        {
            lh_memory_std_copy(str + pos, desc, desc_size);
        }
        pos = lh_math_add(pos, desc_size);
    }
    pos = lh_exception_format_put(str, str_size, pos, "\n");
    if (lh_ptr_is_set(origin) && lh_exception_origin_has_condition(origin))
    {
        pos = lh_exception_format_put(str, str_size, pos, "  condition: %s\n",
                                      lh_exception_origin_get_condition(origin));
    }
    if (lh_ptr_is_set(origin) && lh_exception_origin_has_file(origin))
    {
        const lh_bool_t has_function = lh_exception_origin_has_function(origin);

        pos = lh_exception_format_put(
            str, str_size, pos, "  at %s:%u%s%s\n", lh_exception_origin_get_file(origin),
            lh_cast_static(lh_uint_t, lh_exception_origin_get_line(origin)),
            has_function ? " in " : "",
            has_function ? lh_exception_origin_get_function(origin) : "");
    }
    return pos;
}

lh_usize_t
lh_exception_format_size(const lh_exception_t *self)
{
    return lh_exception_format_walk(self, lh_null, 0U);
}

lh_usize_t
lh_exception_format(const lh_exception_t *self, lh_str_ptr str, lh_usize_t str_size)
{
    lh_assert_runtime_ref(str);
    if (lh_math_gt(lh_exception_format_size(self), str_size))
    {
        return 0U;
    }
    return lh_exception_format_walk(self, str, str_size);
}
