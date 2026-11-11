/**
 * @file capture.c
 * @brief ::lh_os_logger_capture_t: the per-thread scope and the bounded buffer of lines.
 */

#include <lh/os/logger/capture.h>
#include <lh/core/assert/runtime.h>
#include <lh/core/attribute/thread_local.h>
#include <lh/core/attribute/unused.h>
#include <lh/core/math.h>
#include <lh/core/str.h>

/** The capture the calling thread's lines go to; ::lh_null outside a scope. */
LH_ATTRIBUTE_THREAD_LOCAL
lh_os_logger_capture_t *m_os_logger_capture_current;
lh_os_logger_capture_line_t *
lh_os_logger_capture_get_lines(const lh_os_logger_capture_t *self)
{
    lh_assert_runtime_ref(self);
    return self->lines;
}

lh_void
lh_os_logger_capture_set_lines(lh_os_logger_capture_t *self, lh_os_logger_capture_line_t *lines)
{
    lh_assert_runtime_ref(self);
    self->lines = lines;
}

lh_usize_t
lh_os_logger_capture_get_capacity(const lh_os_logger_capture_t *self)
{
    lh_assert_runtime_ref(self);
    return self->capacity;
}

lh_void
lh_os_logger_capture_set_capacity(lh_os_logger_capture_t *self, lh_usize_t capacity)
{
    lh_assert_runtime_ref(self);
    self->capacity = capacity;
}

lh_usize_t
lh_os_logger_capture_get_count(const lh_os_logger_capture_t *self)
{
    lh_assert_runtime_ref(self);
    return self->count;
}

lh_void
lh_os_logger_capture_set_count(lh_os_logger_capture_t *self, lh_usize_t count)
{
    lh_assert_runtime_ref(self);
    self->count = count;
}

lh_usize_t
lh_os_logger_capture_get_dropped(const lh_os_logger_capture_t *self)
{
    lh_assert_runtime_ref(self);
    return self->dropped;
}

lh_void
lh_os_logger_capture_set_dropped(lh_os_logger_capture_t *self, lh_usize_t dropped)
{
    lh_assert_runtime_ref(self);
    self->dropped = dropped;
}

lh_os_logger_capture_t *
lh_os_logger_capture_get_current(lh_void)
{
    return m_os_logger_capture_current;
}

lh_os_logger_capture_t *
lh_os_logger_capture_begin(lh_os_logger_capture_t *self)
{
    lh_os_logger_capture_t *previous = m_os_logger_capture_current;

    m_os_logger_capture_current = self;
    return previous;
}

lh_void
lh_os_logger_capture_end(lh_os_logger_capture_t *previous)
{
    m_os_logger_capture_current = previous;
}

lh_void
lh_os_logger_capture_clear(lh_os_logger_capture_t *self)
{
    lh_os_logger_capture_set_count(self, 0U);
    lh_os_logger_capture_set_dropped(self, 0U);
}

const lh_os_logger_capture_line_t *
lh_os_logger_capture_get_line(const lh_os_logger_capture_t *self, lh_usize_t index)
{
    return lh_math_lt(index, lh_os_logger_capture_get_count(self))
               ? lh_os_logger_capture_get_lines(self) + index
               : lh_null;
}

lh_bool_t
lh_os_logger_capture_try_push_v(lh_os_logger_capture_t *self, lh_cptr_of(lh_void) target,
                                lh_ptr context, lh_logger_level_t level, const lh_str_ptr fmt,
                                va_list args)
{
    const lh_usize_t count = lh_os_logger_capture_get_count(self);

    lh_assert_runtime_ref(fmt);
    if (lh_math_ge(count, lh_os_logger_capture_get_capacity(self)))
    {
        lh_os_logger_capture_set_dropped(self,
                                         lh_math_add_one(lh_os_logger_capture_get_dropped(self)));
        return lh_bool_false;
    }

    lh_os_logger_capture_line_t *line = lh_os_logger_capture_get_lines(self) + count;
    lh_os_logger_record_t *record = lh_os_logger_capture_line_get_record_mut(line);

    lh_os_logger_capture_line_set_target(line, target);
    lh_os_logger_capture_line_set_context(line, context);
    /* Formatted whole, then cut by the record: lh has no cutting format into a
       fixed buffer yet (docs/cleanup.md). */
    lh_str_t text;

    lh_str_init(lh_addr_of(text));
    LH_ATTRIBUTE_UNUSED(lh_str_format_v(lh_addr_of(text), fmt, args));
    lh_os_logger_record_init_line(record, 0U, level, 0, lh_null, lh_str_get_data(lh_addr_of(text)));
    lh_str_deinit(lh_addr_of(text));
    lh_os_logger_capture_set_count(self, lh_math_add_one(count));
    return lh_bool_true;
}

lh_void
lh_os_logger_capture_init(lh_os_logger_capture_t *self, lh_os_logger_capture_line_t *lines,
                          lh_usize_t capacity)
{
    lh_os_logger_capture_set_lines(self, lines);
    lh_os_logger_capture_set_capacity(self, capacity);
    lh_os_logger_capture_clear(self);
}

lh_void
lh_os_logger_capture_replay(const lh_os_logger_capture_t *self, lh_os_logger_capture_replay_cb fn,
                            lh_ptr user)
{
    lh_os_logger_capture_t *previous = lh_os_logger_capture_begin(lh_null);

    lh_assert_runtime_ref(fn);
    for (lh_usize_t i = 0U; lh_math_lt(i, lh_os_logger_capture_get_count(self)); ++i)
    {
        fn(user, lh_os_logger_capture_get_line(self, i));
    }
    lh_os_logger_capture_end(previous);
}
