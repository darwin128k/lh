/**
 * @file range.c
 * @brief Implementation of `lh/ui/font/range.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/runtime/error/code.h>
#include <lh/ui/font/range.h>
#include <lh/util/addr.h>

lh_void
lh_ui_font_range_init(lh_ui_font_range_t *self, lh_u32_t first, lh_u32_t length, lh_u32_t base)
{
    lh_assert_runtime_ref(self);
    self->first = first;
    self->length = length;
    self->base = base;
}

lh_u32_t
lh_ui_font_range_get_first(const lh_ui_font_range_t *self)
{
    lh_assert_runtime_ref(self);
    return self->first;
}

lh_u32_t
lh_ui_font_range_get_length(const lh_ui_font_range_t *self)
{
    lh_assert_runtime_ref(self);
    return self->length;
}

lh_u32_t
lh_ui_font_range_get_base(const lh_ui_font_range_t *self)
{
    lh_assert_runtime_ref(self);
    return self->base;
}

lh_u32_t
lh_ui_font_range_get_last(const lh_ui_font_range_t *self)
{
    lh_assert_runtime_ref(self);
    /* `first - 1` and not `first + length - 1`, so an empty run -- and a run that
       ends at the top of the code space -- has a last code below its first, and
       ::lh_ui_font_range_has says no to every code rather than saying yes to the
       whole space. */
    return self->length > 0U ? self->first + self->length - 1U : self->first - 1U;
}

lh_bool_t
lh_ui_font_range_has(const lh_ui_font_range_t *self, lh_u32_t code)
{
    lh_assert_runtime_ref(self);
    return code >= lh_ui_font_range_get_first(self) &&
                   code - lh_ui_font_range_get_first(self) < lh_ui_font_range_get_length(self)
               ? lh_bool_true
               : lh_bool_false;
}