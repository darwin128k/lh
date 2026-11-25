/**
 * @file draw.c
 * @brief Implementation of `lh/gl/view.h`: drawing: the whole frame, a damaged rect, strip by
 * strip.
 */

#include <lh/gl/view.h>

#include <lh/core/assert.h>
#include <lh/core/math.h>
#include <lh/core/util/return.h>

lh_void
lh_gl_view_fit_canvas(lh_gl_view_t *self)
{
    lh_return_if(lh_null_eq(lh_gl_view_get_root(self)));
    lh_gl_rect_t rect;
    lh_gl_entity_copy_rect(lh_gl_view_get_root(self), lh_addr_of(rect));
    lh_gl_canvas_set_size(lh_gl_view_get_canvas(self),
                          lh_ptr_deref(lh_gl_rect_get_size(lh_addr_of(rect))));
}

lh_void
lh_gl_view_clear(lh_gl_view_t *self, const lh_gl_rect_t *damage)
{
    lh_return_if(lh_null_eq(lh_gl_view_get_clear(self)));
    if (lh_null_eq(damage))
    {
        lh_gl_canvas_clear(lh_gl_view_get_canvas(self), lh_gl_view_get_clear(self));
        return;
    }
    lh_gl_canvas_fill_rect(lh_gl_view_get_canvas(self), damage, lh_gl_view_get_clear(self));
}

lh_void
lh_gl_view_draw_root(lh_gl_view_t *self)
{
    lh_return_if(lh_null_eq(lh_gl_view_get_root(self)));
    lh_gl_entity_draw(lh_gl_view_get_root(self), lh_gl_view_get_canvas(self));
}

lh_bool_t
lh_gl_view_is_stripped(const lh_gl_view_t *self)
{
    return lh_math_gt(lh_gl_view_get_strip_height(self), lh_gl_scalar(0)) ? lh_bool_true
                                                                          : lh_bool_false;
}

lh_void
lh_gl_view_get_strip(const lh_gl_view_t *self, lh_gl_scalar_t index, lh_gl_rect_t *out)
{
    lh_assert_runtime_ref(out);
    lh_gl_size_t size;
    lh_gl_canvas_copy_size(lh_gl_view_get_canvas(self), lh_addr_of(size));
    const lh_gl_scalar_t whole = lh_gl_size_get_height(lh_addr_of(size));
    const lh_gl_scalar_t height = lh_math_min(lh_gl_view_get_strip_height(self), whole);
    const lh_gl_scalar_t top = lh_math_min(index * height, whole - height);
    lh_gl_rect_t strip;

    lh_gl_rect_init_empty(lh_addr_of(strip));
    /* Past the last strip, and never a strip taller than the target itself. */
    if (height <= lh_gl_scalar(0) || index < lh_gl_scalar(0) || index * height >= whole)
    {
        lh_ptr_deref(out) = strip;
        return;
    }
    lh_gl_rect_init(lh_addr_of(strip), lh_gl_scalar(0), top, lh_gl_size_get_width(lh_addr_of(size)),
                    height);
    lh_ptr_deref(out) = strip;
}

lh_bool_t
lh_gl_view_add_whole_area(lh_gl_view_t *self, const lh_gl_rect_t *rect)
{
    lh_assert_runtime_ref(rect);
    lh_return_if(lh_gl_rect_is_empty(rect), lh_bool_false);
    lh_return_if(lh_gl_view_get_whole_count(self) >= LH_GL_VIEW_WHOLE_MAX, lh_bool_false);
    lh_gl_view_get_whole_mut(self)[lh_gl_view_get_whole_count(self)] = lh_ptr_deref(rect);
    lh_gl_view_set_whole_count(self, lh_gl_view_get_whole_count(self) + 1);
    return lh_bool_true;
}

lh_void
lh_gl_view_clear_whole_areas(lh_gl_view_t *self)
{
    lh_assert_runtime_ref(self);
    self->whole_count = 0;
}

lh_void
lh_gl_view_area_whole(const lh_gl_view_t *self, const lh_gl_rect_t *area, const lh_gl_rect_t *reach,
                      lh_gl_rect_t *out)
{
    lh_assert_runtime_ref(out);
    lh_gl_size_t size;
    lh_gl_canvas_copy_size(lh_gl_view_get_canvas(self), lh_addr_of(size));

    lh_assert_runtime_ref(reach);
    lh_gl_rect_t target;
    lh_gl_rect_init(lh_addr_of(target), 0, 0, lh_gl_size_get_width(lh_addr_of(size)),
                    lh_gl_size_get_height(lh_addr_of(size)));
    lh_gl_rect_t grown;
    /* ::lh_null is the whole target, the way ::lh_gl_canvas_damage_in reads it:
       the damage of a frame is that when nothing was said. */
    if (lh_null_eq(area))
    {
        grown = target;
    }
    else
    {
        lh_gl_rect_intersection(area, lh_addr_of(target), lh_addr_of(grown));
    }
    for (lh_u32_t i = 0; i < lh_gl_view_get_whole_count(self); ++i)
    {
        if (lh_gl_rect_intersects(reach, lh_addr_of(lh_gl_view_get_whole(self)[i])))
        {
            lh_gl_rect_union(lh_addr_of(grown), lh_addr_of(lh_gl_view_get_whole(self)[i]),
                             lh_addr_of(grown));
        }
    }
    lh_gl_rect_intersection(lh_addr_of(grown), lh_addr_of(target), out);
}

lh_void
lh_gl_view_draw_frame(lh_gl_view_t *self, const lh_gl_rect_t *damage)
{
    lh_gl_point_t zero;
    lh_gl_point_init(lh_addr_of(zero), lh_gl_scalar(0), lh_gl_scalar(0));
    const lh_gl_size_t *size = lh_gl_canvas_get_size(lh_gl_view_get_canvas(self));

    lh_gl_rect_t whole;
    lh_gl_rect_init(lh_addr_of(whole), lh_gl_scalar(0), lh_gl_scalar(0), lh_gl_size_get_width(size),
                    lh_gl_size_get_height(size));
    lh_gl_canvas_begin(lh_gl_view_get_canvas(self));
    /* A full frame must be presented as a full frame even after descendants
       establish smaller clips. Otherwise only those internal clips reach end. */
    lh_gl_canvas_push(lh_gl_view_get_canvas(self), zero,
                      lh_null_eq(damage) ? lh_addr_of(whole) : damage);
    lh_gl_view_clear(self, damage);
    lh_gl_view_draw_root(self);
    lh_gl_canvas_pop(lh_gl_view_get_canvas(self));
    lh_gl_canvas_end(lh_gl_view_get_canvas(self));
}

lh_void
lh_gl_view_draw_frame_on(lh_gl_view_t *self, const lh_gl_rect_t *area, const lh_gl_rect_t *damage)
{
    lh_gl_rect_t part;
    lh_gl_canvas_damage_in(area, damage, lh_addr_of(part));

    lh_return_if(lh_gl_rect_is_empty(lh_addr_of(part)));
    lh_gl_point_t zero;
    lh_gl_point_init(lh_addr_of(zero), lh_gl_scalar(0), lh_gl_scalar(0));
    lh_gl_canvas_begin_area(lh_gl_view_get_canvas(self), area);
    lh_gl_canvas_push(lh_gl_view_get_canvas(self), zero, lh_addr_of(part));
    lh_gl_view_clear(self, lh_addr_of(part));
    lh_gl_view_draw_root(self);
    lh_gl_canvas_pop(lh_gl_view_get_canvas(self));
    lh_gl_canvas_end(lh_gl_view_get_canvas(self));
}

lh_void
lh_gl_view_draw_strips(lh_gl_view_t *self, const lh_gl_rect_t *damage)
{
    lh_gl_rect_t covered;

    lh_gl_rect_init_empty(lh_addr_of(covered));
    for (lh_gl_scalar_t index = lh_gl_scalar(0);; ++index)
    {
        lh_gl_rect_t strip;
        lh_gl_view_get_strip(self, index, lh_addr_of(strip));
        lh_gl_rect_t part;
        lh_gl_canvas_damage_in(lh_addr_of(strip), damage, lh_addr_of(part));
        lh_gl_rect_t shared;

        lh_return_if(lh_gl_rect_is_empty(lh_addr_of(strip)));
        /* A strip inside the area above was drawn with it, and presenting those
           pixels a second time costs a frame for no change. Inside, and not
           "below its bottom": the last strip is pulled up to a full height, so it
           overlaps the one before it and still carries rows nobody has drawn.

           One rect answers for every area drawn before it. A strip that is not
           inside it is drawn as a new area whose top starts at the strip and
           whose bottom is past the last one — the strip is the only part that
           fails containment that way — so the last area covers all of them. */
        lh_gl_rect_intersection(lh_addr_of(covered), lh_addr_of(strip), lh_addr_of(shared));
        if (lh_gl_rect_eq(lh_addr_of(shared), lh_addr_of(strip)))
        {
            continue;
        }
        /* A strip the damage does not reach keeps the pixels already on screen:
           no buffer, no clear, no blit. That is where the time and the memory of
           a partial frame come from. */
        if (lh_gl_rect_is_empty(lh_addr_of(part)))
        {
            continue;
        }
        {
            /* The area is the strip, which is what the backend sizes its buffer
               to, and the frame is the damage in the same space. Both are grown
               to hold every whole area this strip's damaged part reaches into: an
               effect that reads pixels has to find them in the buffer *and* be
               inside the part being drawn, or the part would be cleared and the
               effect refused. A part that reaches none is left alone — the effect
               in those regions is not drawn this frame anyway, so growing for it
               would buy a bigger buffer and a longer present for nothing. */
            lh_gl_rect_t area;
            lh_gl_view_area_whole(self, lh_addr_of(strip), lh_addr_of(part), lh_addr_of(area));
            lh_gl_rect_t frame;
            lh_gl_view_area_whole(self, damage, lh_addr_of(part), lh_addr_of(frame));

            lh_gl_view_draw_frame_on(self, lh_addr_of(area), lh_addr_of(frame));
            covered = area;
        }
    }
}

lh_void
lh_gl_view_draw(lh_gl_view_t *self, const lh_gl_rect_t *damage)
{
    lh_return_if(lh_null_eq(lh_gl_view_get_canvas(self)));
    lh_gl_view_fit_canvas(self);
    lh_gl_canvas_reset_damage(lh_gl_view_get_canvas(self));
    if (lh_gl_view_is_stripped(self))
    {
        lh_gl_view_draw_strips(self, damage);
    }
    else
    {
        lh_gl_view_draw_frame(self, damage);
    }
    lh_gl_canvas_reset_damage(lh_gl_view_get_canvas(self));
}

lh_void
lh_gl_view_paint(lh_gl_view_t *self)
{
    lh_gl_view_draw(self, lh_null);
}
