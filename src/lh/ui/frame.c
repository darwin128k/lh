/**
 * @file frame.c
 * @brief Level-1 accessors of ::lh_ui_frame_t.
 */

#include <lh/ui/frame.h>


const lh_ui_rect_t *
lh_ui_frame_get_drawn(const lh_ui_frame_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->drawn);
}

lh_ui_rect_t *
lh_ui_frame_get_drawn_mut(lh_ui_frame_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->drawn);
}

lh_void
lh_ui_frame_set_drawn(lh_ui_frame_t *self, lh_ui_rect_t drawn)
{
    lh_assert_runtime_ref(self);
    self->drawn = drawn;
}

lh_bool_t
lh_ui_frame_is_asked_known(const lh_ui_frame_t *self)
{
    lh_assert_runtime_ref(self);
    return self->asked_known;
}

lh_void
lh_ui_frame_set_asked_known(lh_ui_frame_t *self, lh_bool_t asked_known)
{
    lh_assert_runtime_ref(self);
    self->asked_known = asked_known;
}

lh_u64_t
lh_ui_frame_get_asked_px(const lh_ui_frame_t *self)
{
    lh_assert_runtime_ref(self);
    return self->asked_px;
}

lh_void
lh_ui_frame_set_asked_px(lh_ui_frame_t *self, lh_u64_t asked_px)
{
    lh_assert_runtime_ref(self);
    self->asked_px = asked_px;
}

lh_u32_t
lh_ui_frame_get_rects(const lh_ui_frame_t *self)
{
    lh_assert_runtime_ref(self);
    return self->rects;
}

lh_void
lh_ui_frame_set_rects(lh_ui_frame_t *self, lh_u32_t rects)
{
    lh_assert_runtime_ref(self);
    self->rects = rects;
}

lh_u64_t
lh_ui_frame_get_us(const lh_ui_frame_t *self)
{
    lh_assert_runtime_ref(self);
    return self->us;
}

lh_void
lh_ui_frame_set_us(lh_ui_frame_t *self, lh_u64_t us)
{
    lh_assert_runtime_ref(self);
    self->us = us;
}
