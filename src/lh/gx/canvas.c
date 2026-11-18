/**
 * @file canvas.c
 * @brief Implementation of `lh/gx/canvas.h`.
 */

#include <lh/gx/canvas.h>

lh_gx_canvas_fill_rect_fn *
lh_gx_canvas_get_fill_rect(const lh_gx_canvas_t *self)
{
    lh_assert_runtime_ref(self);
    return self->fill_rect;
}

lh_void
lh_gx_canvas_set_fill_rect(lh_gx_canvas_t *self, lh_gx_canvas_fill_rect_fn *fn)
{
    lh_assert_runtime_ref(self);
    self->fill_rect = fn;
}

lh_gx_canvas_fill_mask_fn *
lh_gx_canvas_get_fill_mask(const lh_gx_canvas_t *self)
{
    lh_assert_runtime_ref(self);
    return self->fill_mask;
}

lh_void
lh_gx_canvas_set_fill_mask(lh_gx_canvas_t *self, lh_gx_canvas_fill_mask_fn *fn)
{
    lh_assert_runtime_ref(self);
    self->fill_mask = fn;
}

lh_void
lh_gx_canvas_init(lh_gx_canvas_t *self)
{
    lh_gx_canvas_set_fill_rect(self, lh_null);
    lh_gx_canvas_set_fill_mask(self, lh_null);
}

lh_bool_t
lh_gx_canvas_is_complete(const lh_gx_canvas_t *self)
{
    return lh_cast_static(lh_bool_t,
                          lh_null_ne(lh_ptr_rcast(lh_void, lh_gx_canvas_get_fill_rect(self))));
}
