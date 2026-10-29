/**
 * @file backend.c
 * @brief Level-1 accessors of ::lh_ui_canvas_backend_t.
 */

#include <lh/ui/canvas/backend.h>


lh_ui_canvas_begin_fn *
lh_ui_canvas_backend_get_begin_fn(const lh_ui_canvas_backend_t *self)
{
    lh_assert_runtime_ref(self);
    return self->begin;
}

lh_void
lh_ui_canvas_backend_set_begin_fn(lh_ui_canvas_backend_t *self, lh_ui_canvas_begin_fn * begin)
{
    lh_assert_runtime_ref(self);
    self->begin = begin;
}

lh_ui_canvas_begin_area_fn *
lh_ui_canvas_backend_get_begin_area_fn(const lh_ui_canvas_backend_t *self)
{
    lh_assert_runtime_ref(self);
    return self->begin_area;
}

lh_void
lh_ui_canvas_backend_set_begin_area_fn(lh_ui_canvas_backend_t *self, lh_ui_canvas_begin_area_fn * begin_area)
{
    lh_assert_runtime_ref(self);
    self->begin_area = begin_area;
}

lh_ui_canvas_end_fn *
lh_ui_canvas_backend_get_end_fn(const lh_ui_canvas_backend_t *self)
{
    lh_assert_runtime_ref(self);
    return self->end;
}

lh_void
lh_ui_canvas_backend_set_end_fn(lh_ui_canvas_backend_t *self, lh_ui_canvas_end_fn * end)
{
    lh_assert_runtime_ref(self);
    self->end = end;
}

lh_ui_canvas_clear_fn *
lh_ui_canvas_backend_get_clear_fn(const lh_ui_canvas_backend_t *self)
{
    lh_assert_runtime_ref(self);
    return self->clear;
}

lh_void
lh_ui_canvas_backend_set_clear_fn(lh_ui_canvas_backend_t *self, lh_ui_canvas_clear_fn * clear)
{
    lh_assert_runtime_ref(self);
    self->clear = clear;
}

lh_ui_canvas_fill_rect_fn *
lh_ui_canvas_backend_get_fill_rect_fn(const lh_ui_canvas_backend_t *self)
{
    lh_assert_runtime_ref(self);
    return self->fill_rect;
}

lh_void
lh_ui_canvas_backend_set_fill_rect_fn(lh_ui_canvas_backend_t *self, lh_ui_canvas_fill_rect_fn * fill_rect)
{
    lh_assert_runtime_ref(self);
    self->fill_rect = fill_rect;
}

lh_ui_canvas_fill_round_rect_fn *
lh_ui_canvas_backend_get_fill_round_rect_fn(const lh_ui_canvas_backend_t *self)
{
    lh_assert_runtime_ref(self);
    return self->fill_round_rect;
}

lh_void
lh_ui_canvas_backend_set_fill_round_rect_fn(lh_ui_canvas_backend_t *self, lh_ui_canvas_fill_round_rect_fn * fill_round_rect)
{
    lh_assert_runtime_ref(self);
    self->fill_round_rect = fill_round_rect;
}

lh_ui_canvas_set_clip_fn *
lh_ui_canvas_backend_get_set_clip_fn(const lh_ui_canvas_backend_t *self)
{
    lh_assert_runtime_ref(self);
    return self->set_clip;
}

lh_void
lh_ui_canvas_backend_set_set_clip_fn(lh_ui_canvas_backend_t *self, lh_ui_canvas_set_clip_fn * set_clip)
{
    lh_assert_runtime_ref(self);
    self->set_clip = set_clip;
}

lh_ui_canvas_fill_mask_fn *
lh_ui_canvas_backend_get_fill_mask_fn(const lh_ui_canvas_backend_t *self)
{
    lh_assert_runtime_ref(self);
    return self->fill_mask;
}

lh_void
lh_ui_canvas_backend_set_fill_mask_fn(lh_ui_canvas_backend_t *self, lh_ui_canvas_fill_mask_fn * fill_mask)
{
    lh_assert_runtime_ref(self);
    self->fill_mask = fill_mask;
}

lh_ui_canvas_shadow_fn *
lh_ui_canvas_backend_get_shadow_fn(const lh_ui_canvas_backend_t *self)
{
    lh_assert_runtime_ref(self);
    return self->shadow;
}

lh_void
lh_ui_canvas_backend_set_shadow_fn(lh_ui_canvas_backend_t *self, lh_ui_canvas_shadow_fn * shadow)
{
    lh_assert_runtime_ref(self);
    self->shadow = shadow;
}

lh_ui_canvas_blur_fn *
lh_ui_canvas_backend_get_blur_fn(const lh_ui_canvas_backend_t *self)
{
    lh_assert_runtime_ref(self);
    return self->blur;
}

lh_void
lh_ui_canvas_backend_set_blur_fn(lh_ui_canvas_backend_t *self, lh_ui_canvas_blur_fn * blur)
{
    lh_assert_runtime_ref(self);
    self->blur = blur;
}

lh_ui_canvas_glass_fn *
lh_ui_canvas_backend_get_glass_fn(const lh_ui_canvas_backend_t *self)
{
    lh_assert_runtime_ref(self);
    return self->glass;
}

lh_void
lh_ui_canvas_backend_set_glass_fn(lh_ui_canvas_backend_t *self, lh_ui_canvas_glass_fn * glass)
{
    lh_assert_runtime_ref(self);
    self->glass = glass;
}

lh_ui_canvas_fill_colors_fn *
lh_ui_canvas_backend_get_fill_colors_fn(const lh_ui_canvas_backend_t *self)
{
    lh_assert_runtime_ref(self);
    return self->fill_colors;
}

lh_void
lh_ui_canvas_backend_set_fill_colors_fn(lh_ui_canvas_backend_t *self, lh_ui_canvas_fill_colors_fn * fill_colors)
{
    lh_assert_runtime_ref(self);
    self->fill_colors = fill_colors;
}
