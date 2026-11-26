#include <lh/gl/layout/cross.h>
#include <lh/core/math.h>
#include <lh/gl/layout.h>
#include <lh/core/util/return.h>
#include <lh/core/assert.h>

lh_gl_scalar_t
lh_gl_layout_cross_get_start(const lh_gl_layout_cross_t *self)
{
    lh_assert_runtime_ref(self);
    return self->start;
}

lh_void
lh_gl_layout_cross_set_start(lh_gl_layout_cross_t *self, lh_gl_scalar_t start)
{
    lh_assert_runtime_ref(self);
    self->start = start;
}

lh_gl_scalar_t
lh_gl_layout_cross_get_own(const lh_gl_layout_cross_t *self)
{
    lh_assert_runtime_ref(self);
    return self->own;
}

lh_void
lh_gl_layout_cross_set_own(lh_gl_layout_cross_t *self, lh_gl_scalar_t own)
{
    lh_assert_runtime_ref(self);
    self->own = own;
}

lh_gl_scalar_t
lh_gl_layout_cross_get_room(const lh_gl_layout_cross_t *self)
{
    lh_assert_runtime_ref(self);
    return self->room;
}

lh_void
lh_gl_layout_cross_set_room(lh_gl_layout_cross_t *self, lh_gl_scalar_t room)
{
    lh_assert_runtime_ref(self);
    self->room = room;
}

lh_gl_scalar_t
lh_gl_layout_cross_get_before(const lh_gl_layout_cross_t *self)
{
    lh_assert_runtime_ref(self);
    return self->before;
}

lh_void
lh_gl_layout_cross_set_before(lh_gl_layout_cross_t *self, lh_gl_scalar_t before)
{
    lh_assert_runtime_ref(self);
    self->before = before;
}

lh_void
lh_gl_layout_cross_fit(const lh_gl_layout_t *self, lh_gl_entity_t *child,
                       const lh_gl_rect_t *content, lh_gl_layout_cross_t *out)
{
    const lh_gl_axis_t cross = lh_gl_axis_get_cross(lh_gl_layout_get_axis(self));

    lh_assert_runtime_ref(out);
    lh_gl_rect_t rect;
    lh_gl_entity_copy_rect(child, lh_addr_of(rect));
    lh_gl_rect_t slot;
    lh_gl_layout_slot(self, lh_gl_entity_get_place(child), content, lh_addr_of(slot));
    lh_gl_layout_cross_set_start(
        out, lh_gl_point_get_along(lh_gl_rect_get_origin(lh_addr_of(slot)), cross));
    lh_gl_layout_cross_set_own(out,
                               lh_gl_size_get_along(lh_gl_rect_get_size(lh_addr_of(rect)), cross));
    lh_gl_layout_cross_set_room(out,
                                lh_gl_size_get_along(lh_gl_rect_get_size(lh_addr_of(slot)), cross));
    /* The line is measured from the content box; the slot starts the margin further
       in, so the line is that much nearer to it. */
    lh_gl_layout_cross_set_before(out,
                                  lh_gl_layout_cross_get_start(out) -
                                      lh_gl_point_get_along(lh_gl_rect_get_origin(content), cross));
}

lh_gl_scalar_t
lh_gl_layout_cross_at(const lh_gl_layout_cross_t *fit, lh_gl_place_align_t align,
                      lh_gl_scalar_t base, lh_gl_scalar_t line)
{
    lh_assert_runtime_ref(fit);
    if (align == lh_gl_place_align_center)
    {
        return lh_gl_scalar_centre(lh_gl_layout_cross_get_room(fit),
                                   lh_gl_layout_cross_get_own(fit));
    }
    if (align == lh_gl_place_align_end)
    {
        return lh_gl_layout_cross_get_room(fit) - lh_gl_layout_cross_get_own(fit);
    }
    if (align != lh_gl_place_align_baseline)
    {
        return lh_gl_scalar(0);
    }
    /* On the row's line rather than in the middle of the room: an icon next
       to a caption is one line of text, and two things centred in the same
       16 rows are on one line only by accident.

       A child that has no line of its own is **not** on one, and -1 is not a
       line: subtracting it would put the child a row *below* the row's line,
       which is where the measurement put it and is the one row in this whole
       arrangement nobody could explain. So the answer for a child with no
       baseline, and for everybody in a vertical flow where there is no cross
       row to stand on, is the centring above. */
    return (base >= lh_gl_scalar(0) && line >= lh_gl_scalar(0))
               ? line - lh_gl_layout_cross_get_before(fit) - base
               : lh_gl_scalar_centre(lh_gl_layout_cross_get_room(fit),
                                     lh_gl_layout_cross_get_own(fit));
}
