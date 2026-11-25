/**
 * @file metrics.c
 * @brief Implementation of `lh/gl/label/metrics.h`.
 */

#include <lh/gl/label/metrics.h>

#include <lh/core/assert.h>

const lh_gl_rect_t *
lh_gl_label_metrics_get_rect(const lh_gl_label_metrics_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->rect);
}

lh_gl_rect_t *
lh_gl_label_metrics_get_rect_mut(lh_gl_label_metrics_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->rect);
}

lh_void
lh_gl_label_metrics_set_rect(lh_gl_label_metrics_t *self, lh_gl_rect_t rect)
{
    lh_assert_runtime_ref(self);
    self->rect = rect;
}

const lh_gl_rect_t *
lh_gl_label_metrics_get_ink(const lh_gl_label_metrics_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->ink);
}

lh_gl_rect_t *
lh_gl_label_metrics_get_ink_mut(lh_gl_label_metrics_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->ink);
}

lh_void
lh_gl_label_metrics_set_ink(lh_gl_label_metrics_t *self, lh_gl_rect_t ink)
{
    lh_assert_runtime_ref(self);
    self->ink = ink;
}

const lh_gl_insets_t *
lh_gl_label_metrics_get_padding(const lh_gl_label_metrics_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->padding);
}

lh_gl_insets_t *
lh_gl_label_metrics_get_padding_mut(lh_gl_label_metrics_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->padding);
}

lh_void
lh_gl_label_metrics_set_padding(lh_gl_label_metrics_t *self, lh_gl_insets_t padding)
{
    lh_assert_runtime_ref(self);
    self->padding = padding;
}

const lh_gl_point_t *
lh_gl_label_metrics_get_origin(const lh_gl_label_metrics_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->origin);
}

lh_gl_point_t *
lh_gl_label_metrics_get_origin_mut(lh_gl_label_metrics_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->origin);
}

lh_void
lh_gl_label_metrics_set_origin(lh_gl_label_metrics_t *self, lh_gl_point_t origin)
{
    lh_assert_runtime_ref(self);
    self->origin = origin;
}

const lh_gl_size_t *
lh_gl_label_metrics_get_size(const lh_gl_label_metrics_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->size);
}

lh_gl_size_t *
lh_gl_label_metrics_get_size_mut(lh_gl_label_metrics_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->size);
}

lh_void
lh_gl_label_metrics_set_size(lh_gl_label_metrics_t *self, lh_gl_size_t size)
{
    lh_assert_runtime_ref(self);
    self->size = size;
}

const struct lh_font *
lh_gl_label_metrics_get_font(const lh_gl_label_metrics_t *self)
{
    lh_assert_runtime_ref(self);
    return self->font;
}

lh_void
lh_gl_label_metrics_set_font(lh_gl_label_metrics_t *self, const struct lh_font *font)
{
    lh_assert_runtime_ref(self);
    self->font = font;
}

lh_gl_scalar_t
lh_gl_label_metrics_get_ink_top(const lh_gl_label_metrics_t *self)
{
    lh_assert_runtime_ref(self);
    return self->ink_top;
}

lh_void
lh_gl_label_metrics_set_ink_top(lh_gl_label_metrics_t *self, lh_gl_scalar_t ink_top)
{
    lh_assert_runtime_ref(self);
    self->ink_top = ink_top;
}

lh_gl_scalar_t
lh_gl_label_metrics_get_wrap(const lh_gl_label_metrics_t *self)
{
    lh_assert_runtime_ref(self);
    return self->wrap;
}

lh_void
lh_gl_label_metrics_set_wrap(lh_gl_label_metrics_t *self, lh_gl_scalar_t wrap)
{
    lh_assert_runtime_ref(self);
    self->wrap = wrap;
}

lh_gl_text_align_h_t
lh_gl_label_metrics_get_align_h(const lh_gl_label_metrics_t *self)
{
    lh_assert_runtime_ref(self);
    return self->align_h;
}

lh_void
lh_gl_label_metrics_set_align_h(lh_gl_label_metrics_t *self, lh_gl_text_align_h_t align_h)
{
    lh_assert_runtime_ref(self);
    self->align_h = align_h;
}

lh_gl_text_align_v_t
lh_gl_label_metrics_get_align_v(const lh_gl_label_metrics_t *self)
{
    lh_assert_runtime_ref(self);
    return self->align_v;
}

lh_void
lh_gl_label_metrics_set_align_v(lh_gl_label_metrics_t *self, lh_gl_text_align_v_t align_v)
{
    lh_assert_runtime_ref(self);
    self->align_v = align_v;
}
