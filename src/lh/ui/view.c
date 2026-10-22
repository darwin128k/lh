/**
 * @file view.c
 * @brief Implementation of lh/ui/view.h — entity tree on an lh_ui_canvas_t.
 */

#include <lh/math.h>
#include <lh/ui/view.h>
#include <lh/util/return.h>

#if LH_LIBRARY_OPTION_OS
#    include <lh/os/input.h>
#endif

lh_void
lh_ui_view_set_canvas(lh_ui_view_t *self, lh_ui_canvas_t *canvas)
{
    lh_assert_runtime_ref(self);
    self->canvas = canvas;
}

lh_ui_canvas_t *
lh_ui_view_get_canvas(const lh_ui_view_t *self)
{
    lh_assert_runtime_ref(self);
    return self->canvas;
}

lh_void
lh_ui_view_add_damage(lh_ui_view_t *self, const lh_ui_rect_t *rect)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(rect);
    lh_return_if(lh_ui_rect_is_empty(rect));
    if (!self->has_pending)
    {
        self->pending = lh_ptr_deref(rect);
        self->has_pending = lh_bool_true;
        return;
    }
    lh_ui_rect_union(lh_addr_of(self->pending), rect, lh_addr_of(self->pending));
}

lh_bool_t
lh_ui_view_take_damage(lh_ui_view_t *self, lh_ui_rect_t *out)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(out);
    lh_return_if(!self->has_pending, lh_bool_false);
    lh_ptr_deref(out) = self->pending;
    self->has_pending = lh_bool_false;
    lh_ui_rect_init_empty(lh_addr_of(self->pending));
    return lh_bool_true;
}

lh_ui_entity_t *
lh_ui_view_get_root(const lh_ui_view_t *self)
{
    lh_assert_runtime_ref(self);
    return self->root;
}

lh_void
lh_ui_view_set_clear(lh_ui_view_t *self, const lh_ui_color_t *clear)
{
    lh_assert_runtime_ref(self);
    self->clear = clear;
}

const lh_ui_color_t *
lh_ui_view_get_clear(const lh_ui_view_t *self)
{
    lh_assert_runtime_ref(self);
    return self->clear;
}

lh_void
lh_ui_view_fit_canvas(lh_ui_view_t *self)
{
    lh_ui_rect_t rect;

    lh_return_if(lh_null_eq(self->root));
    lh_ui_entity_get_rect(self->root, lh_addr_of(rect));
    lh_ui_canvas_set_size(self->canvas, lh_ptr_deref(lh_ui_rect_get_size(lh_addr_of(rect))));
}

lh_void
lh_ui_view_clear(lh_ui_view_t *self, const lh_ui_rect_t *damage)
{
    lh_return_if(lh_null_eq(self->clear));
    if (lh_null_eq(damage))
    {
        lh_ui_canvas_clear(self->canvas, self->clear);
        return;
    }
    lh_ui_canvas_fill_rect(self->canvas, damage, self->clear);
}

lh_void
lh_ui_view_draw_root(lh_ui_view_t *self)
{
    lh_return_if(lh_null_eq(self->root));
    lh_ui_entity_draw(self->root, self->canvas);
}

lh_void
lh_ui_view_draw_frame(lh_ui_view_t *self, const lh_ui_rect_t *damage)
{
    lh_ui_point_t zero;

    lh_ui_point_init(lh_addr_of(zero), lh_ui_scalar(0), lh_ui_scalar(0));
    lh_ui_canvas_begin(self->canvas);
    lh_ui_canvas_push(self->canvas, zero, damage);
    lh_ui_view_clear(self, damage);
    lh_ui_view_draw_root(self);
    lh_ui_canvas_pop(self->canvas);
    lh_ui_canvas_end(self->canvas);
}

/* -- Draw ------------------------------------------------------------------ */

lh_void
lh_ui_view_set_strip_height(lh_ui_view_t *self, lh_ui_scalar_t height)
{
    lh_assert_runtime_ref(self);
    self->strip_height = height;
}

lh_ui_scalar_t
lh_ui_view_get_strip_height(const lh_ui_view_t *self)
{
    lh_assert_runtime_ref(self);
    return self->strip_height;
}

lh_bool_t
lh_ui_view_is_stripped(const lh_ui_view_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_math_gt(self->strip_height, lh_ui_scalar(0)) ? lh_bool_true : lh_bool_false;
}

lh_void
lh_ui_view_get_strip(const lh_ui_view_t *self, lh_ui_scalar_t index, lh_ui_rect_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ui_size_t size;
    lh_ui_canvas_get_size(self->canvas, lh_addr_of(size));
    const lh_ui_scalar_t whole = lh_ui_size_get_height(lh_addr_of(size));
    const lh_ui_scalar_t height = lh_math_min(self->strip_height, whole);
    const lh_ui_scalar_t top = lh_math_min(index * height, whole - height);
    lh_ui_rect_t strip;

    lh_assert_runtime_ref(self);
    lh_ui_rect_init_empty(lh_addr_of(strip));
    /* Past the last strip, and never a strip taller than the target itself. */
    if (height <= lh_ui_scalar(0) || index < lh_ui_scalar(0) || index * height >= whole)
    {
        lh_ptr_deref(out) = strip;
        return;
    }
    lh_ui_rect_init(lh_addr_of(strip), lh_ui_scalar(0), top, lh_ui_size_get_width(lh_addr_of(size)),
                    height);
    lh_ptr_deref(out) = strip;
}

lh_void
lh_ui_view_draw_frame_on(lh_ui_view_t *self, const lh_ui_rect_t *area, const lh_ui_rect_t *damage)
{
    lh_ui_rect_t part;
    lh_ui_canvas_damage_in(area, damage, lh_addr_of(part));
    lh_ui_point_t zero;

    lh_return_if(lh_ui_rect_is_empty(lh_addr_of(part)));
    lh_ui_point_init(lh_addr_of(zero), lh_ui_scalar(0), lh_ui_scalar(0));
    lh_ui_canvas_begin_area(self->canvas, area);
    lh_ui_canvas_push(self->canvas, zero, lh_addr_of(part));
    lh_ui_view_clear(self, lh_addr_of(part));
    lh_ui_view_draw_root(self);
    lh_ui_canvas_pop(self->canvas);
    lh_ui_canvas_end(self->canvas);
}

lh_bool_t
lh_ui_view_add_whole_area(lh_ui_view_t *self, const lh_ui_rect_t *rect)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(rect);
    lh_return_if(lh_ui_rect_is_empty(rect), lh_bool_false);
    lh_return_if(self->whole_count >= LH_UI_VIEW_WHOLE_MAX, lh_bool_false);
    self->whole[self->whole_count] = lh_ptr_deref(rect);
    ++self->whole_count;
    return lh_bool_true;
}

lh_void
lh_ui_view_clear_whole_areas(lh_ui_view_t *self)
{
    lh_assert_runtime_ref(self);
    self->whole_count = 0;
}

lh_void
lh_ui_view_area_whole(const lh_ui_view_t *self, const lh_ui_rect_t *area, const lh_ui_rect_t *reach,
                      lh_ui_rect_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ui_size_t size;
    lh_ui_canvas_get_size(self->canvas, lh_addr_of(size));
    lh_ui_rect_t target;
    lh_ui_rect_t grown;
    lh_u32_t i;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(reach);
    lh_ui_rect_init(lh_addr_of(target), 0, 0, lh_ui_size_get_width(lh_addr_of(size)),
                    lh_ui_size_get_height(lh_addr_of(size)));
    /* ::lh_null is the whole target, the way ::lh_ui_canvas_damage_in reads it:
       the damage of a frame is that when nothing was said. */
    if (lh_null_eq(area))
    {
        grown = target;
    }
    else
    {
        lh_ui_rect_intersection(area, lh_addr_of(target), lh_addr_of(grown));
    }
    for (i = 0; i < self->whole_count; ++i)
    {
        if (lh_ui_rect_intersects(reach, lh_addr_of(self->whole[i])))
        {
            lh_ui_rect_union(lh_addr_of(grown), lh_addr_of(self->whole[i]), lh_addr_of(grown));
        }
    }
    lh_ui_rect_intersection(lh_addr_of(grown), lh_addr_of(target), out);
}

lh_void
lh_ui_view_draw_strips(lh_ui_view_t *self, const lh_ui_rect_t *damage)
{
    lh_ui_scalar_t index;
    lh_ui_rect_t covered;

    lh_ui_rect_init_empty(lh_addr_of(covered));
    for (index = lh_ui_scalar(0);; ++index)
    {
        lh_ui_rect_t strip;
        lh_ui_view_get_strip(self, index, lh_addr_of(strip));
        lh_ui_rect_t part;
        lh_ui_canvas_damage_in(lh_addr_of(strip), damage, lh_addr_of(part));
        lh_ui_rect_t shared;

        lh_return_if(lh_ui_rect_is_empty(lh_addr_of(strip)));
        /* A strip inside the area above was drawn with it, and presenting those
           pixels a second time costs a frame for no change. Inside, and not
           "below its bottom": the last strip is pulled up to a full height, so it
           overlaps the one before it and still carries rows nobody has drawn.

           One rect answers for every area drawn before it. A strip that is not
           inside it is drawn as a new area whose top starts at the strip and
           whose bottom is past the last one — the strip is the only part that
           fails containment that way — so the last area covers all of them. */
        lh_ui_rect_intersection(lh_addr_of(covered), lh_addr_of(strip), lh_addr_of(shared));
        if (lh_ui_rect_eq(lh_addr_of(shared), lh_addr_of(strip)))
        {
            continue;
        }
        /* A strip the damage does not reach keeps the pixels already on screen:
           no buffer, no clear, no blit. That is where the time and the memory of
           a partial frame come from. */
        if (lh_ui_rect_is_empty(lh_addr_of(part)))
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
            lh_ui_rect_t area;
            lh_ui_view_area_whole(self, lh_addr_of(strip), lh_addr_of(part), lh_addr_of(area));
            lh_ui_rect_t frame;
            lh_ui_view_area_whole(self, damage, lh_addr_of(part), lh_addr_of(frame));

            lh_ui_view_draw_frame_on(self, lh_addr_of(area), lh_addr_of(frame));
            covered = area;
        }
    }
}

lh_void
lh_ui_view_draw(lh_ui_view_t *self, const lh_ui_rect_t *damage)
{
    lh_assert_runtime_ref(self);
    lh_return_if(lh_null_eq(self->canvas));
    lh_ui_view_fit_canvas(self);
    lh_ui_canvas_reset_damage(self->canvas);
    if (lh_ui_view_is_stripped(self))
    {
        lh_ui_view_draw_strips(self, damage);
    }
    else
    {
        lh_ui_view_draw_frame(self, damage);
    }
    lh_ui_canvas_reset_damage(self->canvas);
}

lh_void
lh_ui_view_paint(lh_ui_view_t *self)
{
    lh_ui_view_draw(self, lh_null);
}

lh_ui_entity_t *
lh_ui_view_hit_test(lh_ui_view_t *self, lh_ui_point_t point)
{
    lh_assert_runtime_ref(self);
    lh_return_if(lh_null_eq(self->root), lh_null);
    return lh_ui_entity_find_at(self->root, point);
}

lh_void
lh_ui_view_end_scroll(lh_ui_view_t *self)
{
    lh_ui_container_t *box = self->scrolling;

    lh_return_if(lh_null_eq(box));
    self->scrolling = lh_null;
    lh_ui_entity_send(lh_ui_container_as_entity(box), lh_ui_entity_event_scroll_end, lh_null);
}

lh_void
lh_ui_view_begin_scroll(lh_ui_view_t *self, lh_ui_container_t *box)
{
    lh_return_if(self->scrolling == box);
    lh_ui_view_end_scroll(self);
    self->scrolling = box;
    lh_ui_entity_send(lh_ui_container_as_entity(box), lh_ui_entity_event_scroll_begin, lh_null);
}

/* -- Hit and click --------------------------------------------------------- */

lh_bool_t
lh_ui_view_damage_scroll(lh_ui_view_t *self, lh_ui_container_t *box, lh_ui_point_t before)
{
    lh_ui_point_t after;
    lh_ui_container_get_scroll(box, lh_addr_of(after));

    lh_return_if(lh_ui_point_equals(lh_addr_of(before), lh_addr_of(after)), lh_bool_false);
    lh_ui_view_begin_scroll(self, box);
    lh_ui_entity_send(lh_ui_container_as_entity(box), lh_ui_entity_event_scroll, lh_null);
    /* The pane and the bar that drives it, in root space. The canvas record
       below is what a test reads back from the frame; the window paints this
       one, and it has to exist when there is no canvas yet. */
    lh_ui_entity_note(lh_ui_container_as_entity(box));
    {
        lh_ui_entity_t *parent = lh_ui_entity_get_parent(lh_ui_container_as_entity(box));
        lh_ui_entity_t *child;

        if (lh_null_ne(parent))
        {
            for (child = lh_ui_entity_get_first_child(parent); lh_null_ne(child);
                 child = lh_ui_entity_get_next_child(parent, child))
            {
                lh_ui_scrollbar_t *bar = lh_ui_scrollbar_get_bound(child, box);

                if (lh_null_ne(bar))
                {
                    lh_ui_entity_note(lh_ui_scrollbar_as_entity(bar));
                }
            }
        }
    }
    lh_return_if(lh_null_eq(self->canvas), lh_bool_true);
    lh_ui_canvas_reset_damage(self->canvas);
    lh_ui_scrollbar_add_scroll_damage(box, self->canvas);
    return lh_bool_true;
}

lh_bool_t
lh_ui_view_scroll_by(lh_ui_view_t *self, lh_ui_container_t *box, lh_ui_scalar_t dx,
                     lh_ui_scalar_t dy)
{
    lh_ui_point_t before;

    lh_return_if(lh_null_eq(box), lh_bool_false);
    lh_ui_container_get_scroll(box, lh_addr_of(before));
    lh_ui_container_scroll_by(box, dx, dy);
    return lh_ui_view_damage_scroll(self, box, before);
}

lh_bool_t
lh_ui_view_is_slow(lh_ui_point_t velocity, lh_ui_scalar_t limit)
{
    return lh_math_abs(lh_ui_point_get_x(lh_addr_of(velocity))) < limit &&
                   lh_math_abs(lh_ui_point_get_y(lh_addr_of(velocity))) < limit
               ? lh_bool_true
               : lh_bool_false;
}

lh_void
lh_ui_view_stop_throw(lh_ui_view_t *self)
{
    lh_assert_runtime_ref(self);
    lh_ui_point_init(lh_addr_of(self->velocity), lh_ui_scalar(0), lh_ui_scalar(0));
    lh_return_if(lh_null_eq(self->throwing));
    self->throwing = lh_null;
    lh_ui_view_end_scroll(self);
}

/* -- Scroll gestures ------------------------------------------------------- */

lh_bool_t
lh_ui_view_wheel(lh_ui_view_t *self, lh_ui_point_t point, lh_ui_scalar_t dx, lh_ui_scalar_t dy)
{
    lh_bool_t changed;

    lh_ui_view_stop_throw(self);
    changed = lh_ui_view_scroll_by(
        self, lh_ui_scrollbar_find_scrolled(lh_ui_view_hit_test(self, point)), dx, dy);
    lh_ui_view_end_scroll(self);
    return changed;
}

lh_void
lh_ui_view_start_throw(lh_ui_view_t *self, lh_ui_container_t *box)
{
    if (lh_ui_view_is_slow(self->velocity, LH_UI_VIEW_THROW_MIN))
    {
        lh_ui_view_end_scroll(self);
        return;
    }
    self->throwing = box;
}

lh_bool_t
lh_ui_view_is_gliding(const lh_ui_view_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_null_ne(self->throwing) ? lh_bool_true : lh_bool_false;
}

lh_void
lh_ui_view_slow_down(lh_ui_point_t velocity, lh_ui_point_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ui_point_t slower;

    lh_ui_point_init(lh_addr_of(slower),
                     lh_ui_point_get_x(lh_addr_of(velocity)) * LH_UI_VIEW_THROW_KEEP / 100,
                     lh_ui_point_get_y(lh_addr_of(velocity)) * LH_UI_VIEW_THROW_KEEP / 100);
    lh_ptr_deref(out) = slower;
}

lh_void
lh_ui_view_reset_pointer(lh_ui_view_t *self)
{
    lh_os_input_pointer_id_t id;

    lh_assert_runtime_ref(self);
    for (id = 0U; id < LH_OS_INPUT_POINTER_MAX; ++id)
    {
        lh_ui_view_pointer_reset(lh_addr_of(self->pointers[id]));
    }
}

const lh_ui_view_pointer_t *
lh_ui_view_get_pointer(const lh_ui_view_t *self, lh_os_input_pointer_id_t id)
{
    lh_assert_runtime_ref(self);
    lh_return_if(id >= LH_OS_INPUT_POINTER_MAX, lh_null);
    return lh_addr_of(self->pointers[id]);
}

/* -- Throw ----------------------------------------------------------------- */

lh_void
lh_ui_view_init(lh_ui_view_t *self)
{
    lh_os_input_pointer_id_t id;
    int i;

    lh_assert_runtime_ref(self);
    self->canvas = lh_null;
    self->root = lh_null;
    self->clear = lh_null;
    self->strip_height = lh_ui_scalar(0);
    self->scrolling = lh_null;
    self->throwing = lh_null;
    self->focus = lh_null;
    self->whole_count = 0;
    lh_ui_rect_init_empty(lh_addr_of(self->pending));
    self->has_pending = lh_bool_false;
    lh_ui_point_init(lh_addr_of(self->velocity), lh_ui_scalar(0), lh_ui_scalar(0));
    for (id = 0U; id < LH_OS_INPUT_POINTER_MAX; ++id)
    {
        lh_ui_view_pointer_init(lh_addr_of(self->pointers[id]));
    }
    for (i = 0; i < LH_UI_VIEW_TRANSITION_MAX; ++i)
    {
        lh_ui_transition_init(lh_addr_of(self->transitions[i]));
    }
}

lh_ui_scalar_t
lh_ui_view_get_thumb_start_at(const lh_ui_scrollbar_t *bar, lh_ui_point_t point)
{
    const lh_ui_entity_t *entity = lh_ptr_rcast(const lh_ui_entity_t, bar);

    lh_ui_point_t point1;
    lh_ui_entity_to_local(entity, point, lh_addr_of(point1));
    return lh_ui_scrollbar_get_thumb_start_at(bar, point1);
}

lh_ui_view_pointer_t *
lh_ui_view_get_slot(lh_ui_view_t *self, lh_os_input_pointer_id_t id)
{
    lh_return_if(id >= LH_OS_INPUT_POINTER_MAX, lh_null);
    return lh_addr_of(self->pointers[id]);
}

lh_bool_t
lh_ui_view_is_driven_by_other(const lh_ui_view_t *self, const lh_ui_view_pointer_t *pointer,
                              const lh_ui_container_t *box)
{
    lh_os_input_pointer_id_t id;

    lh_return_if(lh_null_eq(box), lh_bool_false);
    for (id = 0U; id < LH_OS_INPUT_POINTER_MAX; ++id)
    {
        const lh_ui_view_pointer_t *other = lh_addr_of(self->pointers[id]);
        const lh_ui_scrollbar_t *grab = lh_ui_view_pointer_get_grab(other);

        if (other != pointer && (lh_ui_view_pointer_get_drag(other) == box ||
                                 (lh_null_ne(grab) && lh_ui_scrollbar_get_container(grab) == box)))
        {
            return lh_bool_true;
        }
    }
    return lh_bool_false;
}

lh_bool_t
lh_ui_view_is_held_by_other(const lh_ui_view_t *self, const lh_ui_view_pointer_t *pointer,
                            const lh_ui_entity_t *entity)
{
    lh_os_input_pointer_id_t id;

    lh_return_if(lh_null_eq(entity), lh_bool_false);
    for (id = 0U; id < LH_OS_INPUT_POINTER_MAX; ++id)
    {
        const lh_ui_view_pointer_t *other = lh_addr_of(self->pointers[id]);

        if (other != pointer && lh_ui_view_pointer_get_target(other) == entity)
        {
            return lh_bool_true;
        }
    }
    return lh_bool_false;
}

lh_void
lh_ui_view_end_scroll_of(lh_ui_view_t *self, const lh_ui_view_pointer_t *pointer)
{
    lh_return_if(lh_ui_view_is_driven_by_other(self, pointer, self->scrolling));
    lh_ui_view_end_scroll(self);
}

lh_void
lh_ui_view_grab_thumb(lh_ui_view_t *self, lh_ui_view_pointer_t *pointer, lh_ui_scrollbar_t *bar,
                      lh_ui_point_t point)
{
    lh_ui_point_t local;

    lh_return_if(lh_null_eq(bar));
    lh_ui_entity_to_local(lh_ui_scrollbar_as_entity(bar), point, lh_addr_of(local));
    lh_return_if(!lh_ui_scrollbar_contains_thumb(bar, local));
    lh_return_if(lh_ui_view_is_driven_by_other(self, pointer, lh_ui_scrollbar_get_container(bar)));
    lh_ui_view_pointer_set_grab(pointer, bar,
                                lh_ui_view_get_thumb_start_at(bar, point) -
                                    lh_ui_scrollbar_get_thumb_start(bar));
}

lh_ui_container_t *
lh_ui_view_get_drag_box(lh_ui_entity_t *hit)
{
    lh_return_if(lh_null_ne(lh_ui_entity_as_scrollbar(hit)), lh_null);
    return lh_ui_entity_find_container(hit);
}

lh_void
lh_ui_view_damage_looks(lh_ui_view_t *self, const lh_ui_entity_t *entity,
                        const lh_ui_style_t *first, const lh_ui_style_t *second)
{
    lh_ui_rect_t rect;
    lh_ui_rect_t bounds;
    lh_ui_scalar_t outset;

    lh_ui_point_t offset;

    lh_assert_runtime_ref(self);
    lh_return_if(lh_null_eq(entity));
    lh_ui_entity_get_rect(entity, lh_addr_of(rect));
    outset = lh_null_eq(first)
                 ? lh_ui_scalar(0)
                 : lh_ui_shadow_get_outset(lh_ui_style_get_shadow(first), lh_addr_of(rect));
    if (lh_null_ne(second))
    {
        outset = lh_math_max(
            outset, lh_ui_shadow_get_outset(lh_ui_style_get_shadow(second), lh_addr_of(rect)));
    }
    lh_ui_rect_inset(lh_addr_of(rect), lh_math_neg(outset), lh_math_neg(outset),
                     lh_addr_of(bounds));
    /* The canvas record stays in the rect's own space: that is what a frame
       reads back. The window paints root space, because a scrolled row's rect
       is where it was laid out and the pixels are where the scroll put them. */
    if (lh_null_ne(self->canvas))
    {
        lh_ui_canvas_add_damage(self->canvas, lh_addr_of(bounds));
    }
    lh_ui_entity_get_root_offset(entity, lh_addr_of(offset));
    lh_ui_rect_offset(lh_addr_of(bounds), lh_ui_point_get_x(lh_addr_of(offset)),
                      lh_ui_point_get_y(lh_addr_of(offset)), lh_addr_of(bounds));
    lh_ui_view_add_damage(self, lh_addr_of(bounds));
}

lh_void
lh_ui_view_damage_shape(lh_ui_view_t *self, const lh_ui_entity_t *entity)
{
    const lh_ui_style_t *style;

    lh_assert_runtime_ref(self);
    lh_return_if(lh_null_eq(entity));
    style = lh_ui_entity_get_style(entity);
    lh_ui_view_damage_looks(self, entity, style,
                            lh_null_eq(style) ? lh_null : lh_ui_style_get_pressed(style));
}

/* -- Pointer session ------------------------------------------------------- */

lh_ui_entity_t *
lh_ui_view_click_plain(lh_ui_view_t *self, lh_ui_point_t point)
{
    lh_ui_entity_t *clicked = lh_ui_entity_click(self->root, point);

    lh_ui_view_damage_shape(self, clicked);
    return clicked;
}

lh_ui_entity_t *
lh_ui_view_click_scrolling(lh_ui_view_t *self, lh_ui_container_t *box, lh_ui_point_t point)
{
    lh_ui_point_t before;
    lh_ui_entity_t *clicked;

    lh_return_if(lh_null_eq(box), lh_ui_view_click_plain(self, point));
    lh_ui_container_get_scroll(box, lh_addr_of(before));
    clicked = lh_ui_view_click_plain(self, point);
    (void)lh_ui_view_damage_scroll(self, box, before);
    lh_ui_view_end_scroll(self);
    return clicked;
}

lh_ui_entity_t *
lh_ui_view_click(lh_ui_view_t *self, lh_ui_point_t point)
{
    lh_assert_runtime_ref(self);
    lh_return_if(lh_null_eq(self->root), lh_null);
    return lh_ui_view_click_scrolling(
        self, lh_ui_scrollbar_get_driven(lh_ui_view_hit_test(self, point)), point);
}

lh_void
lh_ui_view_set_pressed(lh_ui_view_t *self, lh_ui_entity_t *entity, lh_bool_t pressed)
{
    lh_assert_runtime_ref(self);
    lh_return_if(lh_null_eq(entity) || lh_ui_entity_is_pressed(entity) == pressed);
    lh_ui_view_damage_shape(self, entity);
    lh_ui_entity_set_pressed(entity, pressed);
}

lh_void
lh_ui_view_unpress_target(lh_ui_view_t *self, const lh_ui_view_pointer_t *pointer)
{
    lh_ui_entity_t *target = lh_ui_view_pointer_get_target(pointer);

    lh_return_if(lh_ui_view_is_held_by_other(self, pointer, target));
    lh_ui_view_set_pressed(self, target, lh_bool_false);
}

lh_void
lh_ui_view_start_pointer(lh_ui_view_t *self, lh_ui_view_pointer_t *pointer, lh_ui_point_t point)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(pointer);
    lh_ui_view_stop_throw(self);
    /* A press of this pointer still open when it goes down again (an app that
       went away without a release) is let go rather than left pressed under
       nothing — that is the one way a pressed look could stick. */
    lh_ui_view_unpress_target(self, pointer);
    lh_ui_view_pointer_start(pointer, point);
}

lh_void
lh_ui_view_set_hot(lh_ui_view_t *self, lh_ui_button_t *button, lh_bool_t hot)
{
    lh_assert_runtime_ref(self);
    lh_return_if(lh_null_eq(button));
    lh_return_if(lh_ui_button_get_hot(button) == hot);
    /* Both looks, in whichever order they are: the one being left and the one
       arriving are painted by the same rule as a press. */
    lh_ui_view_damage_looks(self, lh_ui_button_as_entity(button), lh_ui_button_get_style(button),
                            lh_ui_button_get_hot_style(button));
    lh_ui_button_set_hot(button, hot);
}

lh_bool_t
lh_ui_view_drag_thumb(lh_ui_view_t *self, lh_ui_view_pointer_t *pointer, lh_ui_scrollbar_t *bar,
                      lh_ui_point_t point)
{
    lh_ui_container_t *box = lh_ui_scrollbar_get_container(bar);
    lh_ui_point_t before;
    lh_ui_container_get_scroll(box, lh_addr_of(before));

    lh_ui_scrollbar_set_thumb_start(bar, lh_ui_view_get_thumb_start_at(bar, point) -
                                             lh_ui_view_pointer_get_grab_offset(pointer));
    return lh_ui_view_damage_scroll(self, box, before);
}

lh_bool_t
lh_ui_view_drag_content(lh_ui_view_t *self, lh_ui_view_pointer_t *pointer, lh_ui_point_t point)
{
    lh_ui_point_t from;

    lh_ui_view_pointer_get_last_point(pointer, lh_addr_of(from));
    lh_return_if(!lh_ui_view_pointer_is_dragged(pointer) &&
                     !lh_ui_view_pointer_is_past_threshold(pointer, point),
                 lh_bool_false);
    lh_ui_view_pointer_set_dragged(pointer, lh_bool_true);
    lh_ui_view_pointer_track(pointer, point);
    return lh_ui_view_scroll_by(
        self, lh_ui_view_pointer_get_drag(pointer),
        lh_ui_point_get_x(lh_addr_of(from)) - lh_ui_point_get_x(lh_addr_of(point)),
        lh_ui_point_get_y(lh_addr_of(from)) - lh_ui_point_get_y(lh_addr_of(point)));
}

lh_bool_t
lh_ui_view_move_pointer(lh_ui_view_t *self, lh_os_input_pointer_id_t id, lh_ui_point_t point)
{
    lh_ui_view_pointer_t *pointer;
    lh_ui_scrollbar_t *grab;

    lh_assert_runtime_ref(self);
    pointer = lh_ui_view_get_slot(self, id);
    lh_return_if(lh_null_eq(pointer), lh_bool_false);
    grab = lh_ui_view_pointer_get_grab(pointer);
    if (lh_null_ne(grab))
    {
        lh_ui_view_pointer_set_dragged(pointer, lh_bool_true);
        return lh_ui_view_drag_thumb(self, pointer, grab, point);
    }
    lh_return_if(!lh_ui_view_pointer_is_pressed(pointer) ||
                     lh_null_eq(lh_ui_view_pointer_get_drag(pointer)),
                 lh_bool_false);
    return lh_ui_view_drag_content(self, pointer, point);
}

lh_bool_t
lh_ui_view_move(lh_ui_view_t *self, lh_ui_point_t point)
{
    return lh_ui_view_move_pointer(self, 0U, point);
}

lh_void
lh_ui_view_finish_gesture(lh_ui_view_t *self, lh_ui_view_pointer_t *pointer)
{
    lh_ui_container_t *box = lh_ui_view_pointer_get_drag(pointer);

    if (lh_null_ne(box) && lh_ui_view_pointer_is_dragged(pointer))
    {
        /* The glide is the view's, at the speed of the finger that let go. */
        lh_ui_view_pointer_get_velocity(pointer, lh_addr_of(self->velocity));
        lh_ui_view_start_throw(self, box);
        return;
    }
    lh_ui_view_end_scroll_of(self, pointer);
}

lh_ui_entity_t *
lh_ui_view_release_pointer(lh_ui_view_t *self, lh_os_input_pointer_id_t id, lh_ui_point_t point)
{
    lh_ui_view_pointer_t *pointer;
    lh_ui_entity_t *target;
    lh_bool_t clicked;

    lh_assert_runtime_ref(self);
    pointer = lh_ui_view_get_slot(self, id);
    lh_return_if(lh_null_eq(pointer), lh_null);
    target = lh_ui_view_pointer_get_target(pointer);
    lh_ui_entity_send_pointer(target, lh_ui_entity_event_release, point);
    /* Released before the click, and on the entity the press went down on: a
       click may move the pointer onto something else, and that thing was never
       pressed. */
    lh_ui_view_unpress_target(self, pointer);
    lh_ui_view_finish_gesture(self, pointer);
    /* A click is a press and a release on one thing: a finger that slid off the
       button it went down on and lifted over another one clicked neither. */
    clicked = lh_type_cast(
        lh_bool_t, lh_ui_view_pointer_is_pressed(pointer) &&
                       !lh_ui_view_pointer_is_dragged(pointer) &&
                       lh_ui_entity_click_target(lh_ui_view_hit_test(self, point)) == target);
    lh_ui_view_pointer_reset(pointer);
    lh_return_if(!clicked, lh_null);
    return lh_ui_view_click(self, point);
}

lh_ui_entity_t *
lh_ui_view_release(lh_ui_view_t *self, lh_ui_point_t point)
{
    return lh_ui_view_release_pointer(self, 0U, point);
}

lh_ui_entity_t *
lh_ui_view_get_focus(const lh_ui_view_t *self)
{
    lh_assert_runtime_ref(self);
    return self->focus;
}

lh_void
lh_ui_view_set_focus(lh_ui_view_t *self, lh_ui_entity_t *entity)
{
    lh_assert_runtime_ref(self);
    lh_return_if(self->focus == entity);
    if (lh_null_ne(self->focus))
    {
        lh_ui_entity_send(self->focus, lh_ui_entity_event_defocus, lh_null);
    }
    self->focus = entity;
    lh_return_if(lh_null_eq(entity));
    lh_ui_entity_send(entity, lh_ui_entity_event_focus, lh_null);
}

lh_void
lh_ui_view_press_on(lh_ui_view_t *self, lh_ui_view_pointer_t *pointer, lh_ui_entity_t *hit,
                    lh_ui_point_t point)
{
    lh_ui_container_t *box;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(pointer);
    lh_ui_view_pointer_set_target(pointer, hit);
    /* Pressed is the entity's own state and this is the one place that sets it:
       a press is a look (::lh_ui_entity_get_style_now), so it has to be paired
       with the rect it changed, or the picture keeps the old look until
       something else draws. The rect is ::lh_ui_view_take_damage's. */
    lh_ui_view_set_pressed(self, hit, lh_bool_true);
    lh_ui_entity_send_pointer(hit, lh_ui_entity_event_press, point);
    lh_ui_view_set_focus(self, lh_ui_entity_find_focusable(hit));
    lh_ui_view_grab_thumb(self, pointer, lh_ui_entity_as_scrollbar(hit), point);
    box = lh_null_eq(lh_ui_view_pointer_get_grab(pointer)) ? lh_ui_view_get_drag_box(hit) : lh_null;
    /* A container another pointer drags stays that pointer's: this one presses and
       may click inside it, but does not pull the content from under the other. */
    lh_ui_view_pointer_set_drag(pointer,
                                lh_ui_view_is_driven_by_other(self, pointer, box) ? lh_null : box);
}

lh_void
lh_ui_view_press_pointer(lh_ui_view_t *self, lh_os_input_pointer_id_t id, lh_ui_point_t point)
{
    lh_ui_view_pointer_t *pointer;

    lh_assert_runtime_ref(self);
    pointer = lh_ui_view_get_slot(self, id);
    lh_return_if(lh_null_eq(pointer));
    lh_ui_view_start_pointer(self, pointer, point);
    /* Pressing is clicking: what the pointer went down on is the thing that takes
       the pointer, not whatever is painted under it. */
    lh_ui_view_press_on(self, pointer, lh_ui_entity_click_target(lh_ui_view_hit_test(self, point)),
                        point);
}

lh_void
lh_ui_view_press(lh_ui_view_t *self, lh_ui_point_t point)
{
    lh_ui_view_press_pointer(self, 0U, point);
}

lh_bool_t
lh_ui_view_visit_focus(const lh_ui_entity_t *entity, lh_ptr context)
{
    lh_ui_view_focus_walk_t *walk = lh_ptr_rcast(lh_ui_view_focus_walk_t, context);

    lh_return_if(!lh_ui_entity_is_focusable(entity), lh_bool_true);
    if (lh_null_eq(walk->first))
    {
        walk->first = entity;
    }
    lh_return_if(walk->passed && lh_null_eq(walk->next), (walk->next = entity, lh_bool_false));
    walk->passed = entity == walk->current ? lh_bool_true : walk->passed;
    return lh_bool_true;
}

/* -- Focus and keys -------------------------------------------------------- */

lh_ui_entity_t *
lh_ui_view_get_next_focus(const lh_ui_view_t *self)
{
    lh_ui_view_focus_walk_t walk = {lh_null, lh_bool_false, lh_null, lh_null};

    lh_return_if(lh_null_eq(self->root), lh_null);
    walk.current = self->focus;
    walk.passed = lh_null_eq(self->focus) ? lh_bool_true : lh_bool_false;
    (void)lh_ui_entity_walk(self->root, lh_ui_view_visit_focus, lh_addr_of(walk));
    return lh_ptr_rcast(lh_ui_entity_t, lh_null_ne(walk.next) ? walk.next : walk.first);
}

lh_bool_t
lh_ui_view_send_input(lh_ui_view_t *self, const lh_ui_key_input_t *input)
{
    lh_ui_container_t *box = lh_ui_entity_as_container(self->focus);
    lh_ui_point_t before;
    lh_bool_t moved;

    lh_return_if(lh_null_eq(self->focus), lh_bool_false);
    lh_ui_point_init(lh_addr_of(before), lh_ui_scalar(0), lh_ui_scalar(0));
    if (lh_null_ne(box))
    {
        lh_ui_container_get_scroll(box, lh_addr_of(before));
    }
    lh_ui_entity_send(self->focus, lh_ui_entity_event_key, lh_ptr_rcast(lh_void, input));
    lh_return_if(lh_null_eq(box), lh_bool_false);
    moved = lh_ui_view_damage_scroll(self, box, before);
    lh_ui_view_end_scroll(self);
    return moved;
}

lh_bool_t
lh_ui_view_key(lh_ui_view_t *self, lh_os_key_t key, lh_bool_t pressed)
{
    lh_ui_key_input_t input;

    lh_assert_runtime_ref(self);
    if (key == lh_os_key_tab && pressed)
    {
        lh_ui_view_set_focus(self, lh_ui_view_get_next_focus(self));
        return lh_bool_false;
    }
    lh_ui_key_input_init_key(lh_addr_of(input), key, pressed);
    return lh_ui_view_send_input(self, lh_addr_of(input));
}

lh_bool_t
lh_ui_view_text(lh_ui_view_t *self, lh_u32_t code)
{
    lh_ui_key_input_t input;

    lh_assert_runtime_ref(self);
    lh_ui_key_input_init_text(lh_addr_of(input), code);
    return lh_ui_view_send_input(self, lh_addr_of(input));
}

lh_ui_transition_t *
lh_ui_view_slot_of(const lh_ui_view_t *self, const lh_ui_entity_t *entity)
{
    int i;

    for (i = 0; i < LH_UI_VIEW_TRANSITION_MAX; ++i)
    {
        if (lh_ui_transition_get_entity(lh_addr_of(self->transitions[i])) == entity)
        {
            return lh_ptr_rcast(lh_ui_transition_t, lh_addr_of(self->transitions[i]));
        }
    }
    return lh_null;
}

lh_void
lh_ui_view_land(lh_ui_transition_t *slot)
{
    lh_ui_entity_t *entity = lh_ui_transition_get_entity(slot);
    const lh_ui_style_t *to = lh_ui_transition_get_target(slot);

    lh_ui_transition_init(slot);
    lh_ui_entity_set_style_at_once(entity, to);
}

lh_bool_t
lh_ui_view_start_transition(lh_ui_view_t *self, lh_ui_entity_t *entity, const lh_ui_style_t *to)
{
    lh_ui_transition_t *slot;
    const lh_ui_style_t *from;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(entity);
    lh_assert_runtime_ref(to);
    from = lh_ui_entity_get_style(entity);
    lh_return_if(lh_null_eq(from), lh_bool_false);
    slot = lh_ui_view_slot_of(self, entity);
    if (lh_null_eq(slot))
    {
        /* A free slot is one with no entity in it. */
        slot = lh_ui_view_slot_of(self, lh_null);
    }
    lh_return_if(lh_null_eq(slot), lh_bool_false);
    /* From the look on screen, which may be this slot's own half-way look. */
    lh_ui_transition_start(slot, entity, from, to);
    lh_ui_entity_set_style_at_once(entity, lh_ui_transition_get_look(slot));
    lh_ui_entity_note(entity);
    return lh_bool_true;
}

/* -- Input events ------------------------------------------------------------ */

lh_void
lh_ui_view_cancel_pointer(lh_ui_view_t *self, lh_os_input_pointer_id_t id)
{
    lh_ui_view_pointer_t *pointer;
    lh_ui_point_t last;

    lh_assert_runtime_ref(self);
    pointer = lh_ui_view_get_slot(self, id);
    lh_return_if(lh_null_eq(pointer) || !lh_ui_view_pointer_is_active(pointer));
    lh_ui_view_pointer_get_last_point(pointer, lh_addr_of(last));
    lh_ui_entity_send_pointer(lh_ui_view_pointer_get_target(pointer), lh_ui_entity_event_release,
                              last);
    lh_ui_view_unpress_target(self, pointer);
    lh_ui_view_end_scroll_of(self, pointer);
    lh_ui_view_pointer_reset(pointer);
}

lh_void
lh_ui_view_cancel(lh_ui_view_t *self)
{
    lh_os_input_pointer_id_t id;

    lh_assert_runtime_ref(self);
    for (id = 0U; id < LH_OS_INPUT_POINTER_MAX; ++id)
    {
        lh_ui_view_cancel_pointer(self, id);
    }
}

lh_ui_point_t
lh_ui_view_point_of(lh_os_input_event_cptr event)
{
    lh_ui_point_t point;

    lh_ui_point_init(
        lh_addr_of(point),
        lh_type_cast(lh_ui_scalar_t, lh_os_input_coord_to_int(lh_os_input_event_get_x(event))),
        lh_type_cast(lh_ui_scalar_t, lh_os_input_coord_to_int(lh_os_input_event_get_y(event))));
    return point;
}

lh_s32_t
lh_ui_view_wheel_step(lh_void)
{
#if LH_LIBRARY_OPTION_OS
    return lh_os_input_get_wheel_step();
#else
    return LH_UI_VIEW_WHEEL_STEP;
#endif
}

lh_ui_scalar_t
lh_ui_view_wheel_distance(lh_s32_t notch_120, lh_s32_t step)
{
    return lh_type_cast(lh_ui_scalar_t, lh_math_neg(notch_120) * step / LH_OS_INPUT_WHEEL_NOTCH);
}

lh_bool_t
lh_ui_view_input_wheel(lh_ui_view_t *self, lh_os_input_event_cptr event)
{
    const lh_s32_t step = lh_ui_view_wheel_step();

    return lh_ui_view_wheel(self, lh_ui_view_point_of(event),
                            lh_ui_view_wheel_distance(lh_os_input_event_get_dx(event), step),
                            lh_ui_view_wheel_distance(lh_os_input_event_get_dy(event), step));
}

lh_bool_t
lh_ui_view_is_primary_press(lh_os_input_event_cptr event)
{
    return lh_type_cast(lh_bool_t,
                        lh_os_input_event_get_button(event) == lh_os_input_button_primary);
}

lh_bool_t
lh_ui_view_input_pointer(lh_ui_view_t *self, lh_os_input_event_cptr event)
{
    const lh_os_input_pointer_id_t id = lh_os_input_event_get_pointer(event);

    lh_return_if(id >= LH_OS_INPUT_POINTER_MAX, lh_bool_false);
    switch (lh_os_input_event_get_kind(event))
    {
    case lh_os_input_kind_pointer_down:
        lh_return_if(!lh_ui_view_is_primary_press(event), lh_bool_false);
        lh_ui_view_press_pointer(self, id, lh_ui_view_point_of(event));
        return lh_bool_true;
    case lh_os_input_kind_pointer_move:
        return lh_ui_view_move_pointer(self, id, lh_ui_view_point_of(event));
    case lh_os_input_kind_pointer_up:
        lh_return_if(!lh_ui_view_is_primary_press(event), lh_bool_false);
        (void)lh_ui_view_release_pointer(self, id, lh_ui_view_point_of(event));
        return lh_bool_true;
    case lh_os_input_kind_pointer_cancel:
        lh_ui_view_cancel_pointer(self, id);
        return lh_bool_true;
    case lh_os_input_kind_wheel:
        return lh_ui_view_input_wheel(self, event);
    default:
        return lh_bool_false;
    }
}

lh_bool_t
lh_ui_view_input(lh_ui_view_t *self, lh_os_input_event_cptr event)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(event);
    lh_return_if(lh_os_input_event_is_synthesized(event), lh_bool_false);
    switch (lh_os_input_event_get_kind(event))
    {
    case lh_os_input_kind_key_down:
    case lh_os_input_kind_key_up:
        (void)lh_ui_view_key(self, lh_os_input_event_get_key(event),
                             lh_type_cast(lh_bool_t, lh_os_input_event_get_kind(event) ==
                                                         lh_os_input_kind_key_down));
        return lh_bool_true;
    case lh_os_input_kind_text:
        (void)lh_ui_view_text(self, lh_os_input_event_get_code(event));
        return lh_bool_true;
    case lh_os_input_kind_focus_out:
        lh_ui_view_cancel(self);
        return lh_bool_true;
    default:
        return lh_ui_view_input_pointer(self, event);
    }
}

/* -- Transitions ----------------------------------------------------------- */

const lh_ui_style_t *
lh_ui_view_get_transition_target(const lh_ui_view_t *self, const lh_ui_entity_t *entity)
{
    const lh_ui_transition_t *slot;

    lh_assert_runtime_ref(self);
    lh_return_if(lh_null_eq(entity), lh_null);
    slot = lh_ui_view_slot_of(self, entity);
    return lh_null_eq(slot) ? lh_null : lh_ui_transition_get_target(slot);
}

lh_void
lh_ui_view_drop_transition(lh_ui_view_t *self, const lh_ui_entity_t *entity)
{
    lh_ui_transition_t *slot;

    lh_assert_runtime_ref(self);
    lh_return_if(lh_null_eq(entity));
    slot = lh_ui_view_slot_of(self, entity);
    lh_return_if(lh_null_eq(slot));
    lh_ui_transition_init(slot);
}

lh_void
lh_ui_view_finish_transitions(lh_ui_view_t *self, const lh_ui_entity_t *subtree)
{
    int i;

    lh_assert_runtime_ref(self);
    for (i = 0; i < LH_UI_VIEW_TRANSITION_MAX; ++i)
    {
        lh_ui_transition_t *slot = lh_addr_of(self->transitions[i]);
        const lh_ui_entity_t *up = lh_ui_transition_get_entity(slot);

        if (lh_null_eq(up))
        {
            continue;
        }
        /* In the subtree when @p subtree is the entity or one of its ancestors. */
        while (lh_null_ne(subtree) && lh_null_ne(up) && up != subtree)
        {
            up = lh_ui_entity_get_parent(up);
        }
        if (lh_null_eq(subtree) || up == subtree)
        {
            lh_ui_view_land(slot);
        }
    }
}

lh_void
lh_ui_view_deinit(lh_ui_view_t *self)
{
    /* Every entity in flight points into this view's slots: they land first. */
    lh_ui_view_finish_transitions(self, lh_null);
    lh_ui_view_init(self);
}

lh_void
lh_ui_view_set_root(lh_ui_view_t *self, lh_ui_entity_t *root)
{
    lh_ui_entity_t *top;

    lh_assert_runtime_ref(self);
    /* The tree that leaves takes its looks with it, landed: its entities must not
       point into a view that no longer holds them. */
    if (self->root != root)
    {
        lh_ui_view_finish_transitions(self, lh_null);
    }
    /* The pointer lives on the top of the tree, which is where a setter walks
       to find this view. A root that is itself parented still records on the
       top, or the walk would stop one short of it. */
    if (lh_null_ne(self->root) && self->root->view == self)
    {
        self->root->view = lh_null;
    }
    self->root = root;
    lh_return_if(lh_null_eq(root));
    top = root;
    while (lh_null_ne(top->parent))
    {
        top = top->parent;
    }
    top->view = self;
}

lh_bool_t
lh_ui_view_advance(lh_ui_view_t *self, lh_u16_t ms)
{
    lh_bool_t moved = lh_bool_false;
    int i;

    lh_assert_runtime_ref(self);
    lh_return_if(ms == 0, lh_bool_false);
    for (i = 0; i < LH_UI_VIEW_TRANSITION_MAX; ++i)
    {
        lh_ui_transition_t *slot = lh_addr_of(self->transitions[i]);
        lh_ui_entity_t *entity = lh_ui_transition_get_entity(slot);

        if (lh_null_eq(entity))
        {
            continue;
        }
        moved = lh_bool_true;
        /* The entity already points at the slot's look, so a new look in the same
           place is a repaint and nothing else: both the old pixels and the new. */
        lh_ui_entity_note(entity);
        if (!lh_ui_transition_advance(slot, ms))
        {
            lh_ui_view_land(slot);
        }
        lh_ui_entity_note(entity);
    }
    return moved;
}

lh_bool_t
lh_ui_view_tick(lh_ui_view_t *self)
{
    lh_bool_t moved;

    lh_bool_t faded;

    lh_assert_runtime_ref(self);
    /* Looks first, whether or not anything glides: a frame that only fades is a
       frame to paint. */
    faded = lh_ui_view_advance(self, LH_UI_VIEW_FRAME_MS);
    lh_return_if(lh_null_eq(self->throwing), faded);
    lh_ui_view_slow_down(self->velocity, lh_addr_of(self->velocity));
    moved =
        lh_ui_view_scroll_by(self, self->throwing, lh_ui_point_get_x(lh_addr_of(self->velocity)),
                             lh_ui_point_get_y(lh_addr_of(self->velocity)));
    lh_return_if(moved && !lh_ui_view_is_slow(self->velocity, lh_ui_scalar(1)), lh_bool_true);
    lh_ui_view_stop_throw(self);
    return moved ? lh_bool_true : faded;
}
