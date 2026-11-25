/**
 * @file send.c
 * @brief Implementation of `lh/gl/canvas.h`: what reaches the backend: a box whole, or cut to the
 * clip.
 */

#include <lh/gl/canvas.h>

#include <lh/core/attribute/unused.h>
#include <lh/gl/canvas/round.h>
#include <lh/core/util/return.h>

lh_void
lh_gl_canvas_send_clip(lh_gl_canvas_t *self)
{
    lh_return_if(lh_null_eq(lh_gl_canvas_get_backend(self)) ||
                 lh_null_eq(lh_gl_canvas_backend_get_set_clip_fn(lh_gl_canvas_get_backend(self))));
    /* The clip is the region the frame is about to draw into, so it is what the
       frame draws into, and this is the one place that knows both. Recorded as the
       rect the backend is handed — in the buffer's own space, so ::lh_gl_canvas_end
       presents exactly the pixels the backend was cut to — and as its own entry,
       because two cuts standing apart are two entries and their gap belongs to
       neither. A frame that sets no clip leaves the list empty, and empty means
       "everything", the way a frame with no damage draws all of it. */
    const lh_gl_rect_t *rect = lh_gl_canvas_get_clip(self);
    lh_gl_canvas_clip_t clip;
    lh_gl_canvas_backend_get_set_clip_fn(lh_gl_canvas_get_backend(self))(
        lh_gl_canvas_get_context(self), lh_gl_canvas_describe_clip(self, lh_addr_of(clip)));
    if (!lh_null_eq(rect))
    {
        lh_gl_rects_add(lh_gl_canvas_get_drawn_mut(self), rect);
    }
}

lh_void
lh_gl_canvas_send_box(lh_gl_canvas_t *self, lh_s32_t x0, lh_s32_t x1, lh_s32_t y,
                      const lh_gl_color_t *color)
{
    lh_return_if(x1 <= x0);
    lh_gl_rect_t box;
    lh_gl_rect_init(lh_addr_of(box), x0, y, x1 - x0, 1);
    lh_gl_canvas_backend_get_fill_rect_fn(lh_gl_canvas_get_backend(self))(
        lh_gl_canvas_get_context(self), lh_addr_of(box), color);
}

lh_bool_t
lh_gl_canvas_shows(const lh_gl_canvas_t *self, const lh_gl_rect_t *target)
{
    lh_gl_rect_t cut;
    lh_gl_canvas_state_cut(lh_gl_canvas_get_state(self), target, lh_addr_of(cut));

    return lh_gl_rect_is_empty(lh_addr_of(cut)) ? lh_bool_false : lh_bool_true;
}

lh_bool_t
lh_gl_canvas_covers_rect(const lh_gl_canvas_t *self, const lh_gl_rect_t *rect)
{
    lh_gl_rect_t target;
    lh_gl_canvas_state_to_target(lh_gl_canvas_get_state(self), rect, lh_addr_of(target));
    lh_gl_rect_t cut;
    lh_gl_canvas_state_cut(lh_gl_canvas_get_state(self), lh_addr_of(target), lh_addr_of(cut));

    return !lh_gl_rect_is_empty(lh_addr_of(target)) &&
                   lh_gl_rect_eq(lh_addr_of(cut), lh_addr_of(target))
               ? lh_bool_true
               : lh_bool_false;
}

lh_bool_t
lh_gl_canvas_can_send_whole(const lh_gl_canvas_t *self, const lh_gl_rect_t *target)
{
    /* The bar for a slot that *reads* its target: the pixels it is about to work
       on have to be there. No clip at all, or the target inside a plain
       (unrounded) one. A rounded cut is refused as well, because a backend that
       clips knows it and a software one does not. */
    lh_return_if(!lh_gl_canvas_state_is_clipped(lh_gl_canvas_get_state(self)), lh_bool_true);
    return lh_gl_canvas_state_get_round_count(lh_gl_canvas_get_state(self)) == 0U &&
                   lh_gl_canvas_state_contains(lh_gl_canvas_get_state(self), target)
               ? lh_bool_true
               : lh_bool_false;
}

lh_void
lh_gl_canvas_send_clip_pixel(lh_gl_canvas_t *self, const lh_gl_canvas_clip_t *clip, lh_s32_t x,
                             lh_s32_t y, const lh_gl_color_t *color)
{
    const lh_byte_t kept = lh_gl_canvas_clip_coverage(clip, x, y);
    lh_gl_color_t edge;
    lh_gl_color_with_coverage(color, kept, lh_addr_of(edge));

    lh_return_if(kept == 0U);
    lh_gl_canvas_send_box(self, x, x + 1, y, kept == 255U ? color : lh_addr_of(edge));
}

lh_bool_t
lh_gl_canvas_shows_rect(const lh_gl_canvas_t *self, const lh_gl_rect_t *rect)
{
    lh_gl_rect_t target;
    lh_gl_canvas_state_to_target(lh_gl_canvas_get_state(self), rect, lh_addr_of(target));

    return lh_gl_canvas_shows(self, lh_addr_of(target));
}

lh_void
lh_gl_canvas_send_clip_pixels(lh_gl_canvas_t *self, const lh_gl_canvas_clip_t *clip, lh_s32_t x0,
                              lh_s32_t x1, lh_s32_t y, const lh_gl_color_t *color)
{
    for (; x0 < x1; ++x0)
    {
        lh_gl_canvas_send_clip_pixel(self, clip, x0, y, color);
    }
}

lh_void
lh_gl_canvas_send_clip_row(lh_gl_canvas_t *self, const lh_gl_canvas_clip_t *clip, lh_s32_t x0,
                           lh_s32_t x1, lh_s32_t y, const lh_gl_color_t *color)
{
    {
        lh_s32_t mid0;
        lh_s32_t mid1;

        lh_gl_canvas_clip_split_row(clip, x0, x1, y, lh_addr_of(mid0), lh_addr_of(mid1));
        lh_gl_canvas_send_clip_pixels(self, clip, x0, mid0, y, color);
        lh_gl_canvas_send_box(self, mid0, mid1, y, color);
        lh_gl_canvas_send_clip_pixels(self, clip, mid1, x1, y, color);
    }
}

lh_void
lh_gl_canvas_send_clip_rows(lh_gl_canvas_t *self, const lh_gl_rect_t *cut,
                            const lh_gl_color_t *color)
{
    const lh_s32_t y1 = lh_gl_canvas_round_bottom(cut);

    lh_gl_canvas_clip_t clip;
    LH_ATTRIBUTE_UNUSED(lh_gl_canvas_describe_clip(self, lh_addr_of(clip)));
    for (lh_s32_t y = lh_gl_canvas_round_top(cut); y < y1; ++y)
    {
        lh_gl_canvas_send_clip_row(self, lh_addr_of(clip), lh_gl_canvas_round_left(cut),
                                   lh_gl_canvas_round_right(cut), y, color);
    }
}

lh_void
lh_gl_canvas_send_cut(lh_gl_canvas_t *self, const lh_gl_rect_t *cut, const lh_gl_color_t *color)
{
    if (lh_gl_canvas_state_get_round_count(lh_gl_canvas_get_state(self)) == 0U)
    {
        lh_gl_canvas_backend_get_fill_rect_fn(lh_gl_canvas_get_backend(self))(
            lh_gl_canvas_get_context(self), cut, color);
        return;
    }
    lh_gl_canvas_send_clip_rows(self, cut, color);
}
