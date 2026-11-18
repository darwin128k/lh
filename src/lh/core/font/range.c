/**
 * @file range.c
 * @brief Implementation of `lh/core/font/range.h`.
 */

#include <lh/core/font/range.h>

#include <lh/core/assert.h>
lh_u32_t
lh_font_range_get_first(const lh_font_range_t *self)
{
    lh_assert_runtime_ref(self);
    return self->first;
}

lh_u32_t
lh_font_range_get_length(const lh_font_range_t *self)
{
    lh_assert_runtime_ref(self);
    return self->length;
}

lh_u32_t
lh_font_range_get_base(const lh_font_range_t *self)
{
    lh_assert_runtime_ref(self);
    return self->base;
}

lh_void
lh_font_range_set_first(lh_font_range_t *self, lh_u32_t first)
{
    lh_assert_runtime_ref(self);
    self->first = first;
}

lh_void
lh_font_range_set_length(lh_font_range_t *self, lh_u32_t length)
{
    lh_assert_runtime_ref(self);
    self->length = length;
}

lh_void
lh_font_range_set_base(lh_font_range_t *self, lh_u32_t base)
{
    lh_assert_runtime_ref(self);
    self->base = base;
}

lh_void
lh_font_range_init(lh_font_range_t *self, lh_u32_t first, lh_u32_t length, lh_u32_t base)
{
    lh_font_range_set_first(self, first);
    lh_font_range_set_length(self, length);
    lh_font_range_set_base(self, base);
}

lh_bool_t
lh_font_range_has(const lh_font_range_t *self, lh_u32_t code)
{
    return code >= lh_font_range_get_first(self) &&
                   code - lh_font_range_get_first(self) < lh_font_range_get_length(self)
               ? lh_bool_true
               : lh_bool_false;
}

lh_u32_t
lh_font_range_get_last(const lh_font_range_t *self)
{
    /* `first - 1` and not `first + length - 1`, so an empty run -- and a run that
       ends at the top of the code space -- has a last code below its first, and
       ::lh_font_range_has says no to every code rather than saying yes to the
       whole space. */
    return lh_font_range_get_length(self) > 0U
               ? lh_font_range_get_first(self) + lh_font_range_get_length(self) - 1U
               : lh_font_range_get_first(self) - 1U;
}
