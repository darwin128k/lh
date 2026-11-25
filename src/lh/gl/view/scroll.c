/**
 * @file scroll.c
 * @brief Implementation of `lh/gl/view.h`: scrolling: a container scrolled by a drag of its content
 * or its thumb.
 */

#include <lh/gl/view.h>

#include <lh/core/attribute/unused.h>
#include <lh/core/util/return.h>

lh_void
lh_gl_view_end_scroll(lh_gl_view_t *self)
{
    lh_gl_container_t *box = lh_gl_view_get_scrolling(self);

    lh_return_if(lh_null_eq(box));
    lh_gl_view_set_scrolling(self, lh_null);
    lh_gl_entity_send(lh_gl_container_as_entity(box), lh_gl_entity_event_scroll_end, lh_null);
}

lh_gl_scalar_t
lh_gl_view_get_thumb_start_at(const lh_gl_scrollbar_t *bar, lh_gl_point_t point)
{
    const lh_gl_entity_t *entity = lh_ptr_rcast(const lh_gl_entity_t, bar);

    lh_gl_point_t point1;
    lh_gl_entity_to_local(entity, point, lh_addr_of(point1));
    return lh_gl_scrollbar_get_thumb_start_at(bar, point1);
}

lh_gl_container_t *
lh_gl_view_get_drag_box(lh_gl_entity_t *hit)
{
    lh_return_if(lh_null_ne(lh_gl_entity_as_scrollbar(hit)), lh_null);
    return lh_gl_entity_find_scrollable(hit);
}

lh_void
lh_gl_view_begin_scroll(lh_gl_view_t *self, lh_gl_container_t *box)
{
    lh_return_if(lh_gl_view_get_scrolling(self) == box);
    lh_gl_view_end_scroll(self);
    lh_gl_view_set_scrolling(self, box);
    lh_gl_entity_send(lh_gl_container_as_entity(box), lh_gl_entity_event_scroll_begin, lh_null);
}

lh_void
lh_gl_view_end_scroll_of(lh_gl_view_t *self, const lh_gl_view_pointer_t *pointer)
{
    lh_return_if(lh_gl_view_is_driven_by_other(self, pointer, lh_gl_view_get_scrolling(self)));
    lh_gl_view_end_scroll(self);
}

lh_void
lh_gl_view_grab_thumb(lh_gl_view_t *self, lh_gl_view_pointer_t *pointer, lh_gl_scrollbar_t *bar,
                      lh_gl_point_t point)
{
    lh_return_if(lh_null_eq(bar));
    lh_gl_container_t *box = lh_gl_scrollbar_get_container(bar);
    lh_return_if(lh_null_eq(box) || !lh_gl_container_is_scroll_enabled(box));
    lh_gl_point_t local;
    lh_gl_entity_to_local(lh_gl_scrollbar_as_entity(bar), point, lh_addr_of(local));
    lh_return_if(!lh_gl_scrollbar_contains_thumb(bar, local));
    lh_return_if(lh_gl_view_is_driven_by_other(self, pointer, lh_gl_scrollbar_get_container(bar)));
    lh_gl_view_pointer_grab(pointer, bar,
                            lh_gl_view_get_thumb_start_at(bar, point) -
                                lh_gl_scrollbar_get_thumb_start(bar));
}

lh_bool_t
lh_gl_view_damage_scroll(lh_gl_view_t *self, lh_gl_container_t *box, lh_gl_point_t before)
{
    lh_gl_point_t after;
    lh_gl_container_get_clamped_scroll(box, lh_addr_of(after));

    lh_return_if(lh_gl_point_equals(lh_addr_of(before), lh_addr_of(after)), lh_bool_false);
    lh_gl_view_begin_scroll(self, box);
    lh_gl_entity_send(lh_gl_container_as_entity(box), lh_gl_entity_event_scroll, lh_null);
    /* The pane and the bar that drives it, in root space. The canvas record
       below is what a test reads back from the frame; the window paints this
       one, and it has to exist when there is no canvas yet. */
    lh_gl_entity_note(lh_gl_container_as_entity(box));
    {
        lh_gl_entity_t *parent = lh_gl_entity_get_parent(lh_gl_container_as_entity(box));
        lh_gl_entity_t *child;

        if (lh_null_ne(parent))
        {
            for (child = lh_gl_entity_get_first_child(parent); lh_null_ne(child);
                 child = lh_gl_entity_get_next_child(parent, child))
            {
                lh_gl_scrollbar_t *bar = lh_gl_scrollbar_get_bound(child, box);

                if (lh_null_ne(bar))
                {
                    lh_gl_entity_note(lh_gl_scrollbar_as_entity(bar));
                }
            }
        }
    }
    lh_return_if(lh_null_eq(lh_gl_view_get_canvas(self)), lh_bool_true);
    lh_gl_canvas_reset_damage(lh_gl_view_get_canvas(self));
    lh_gl_scrollbar_add_scroll_damage(box, lh_gl_view_get_canvas(self));
    return lh_bool_true;
}

lh_bool_t
lh_gl_view_scroll_by(lh_gl_view_t *self, lh_gl_container_t *box, lh_gl_scalar_t dx,
                     lh_gl_scalar_t dy)
{
    lh_return_if(lh_null_eq(box) || !lh_gl_container_is_scroll_enabled(box), lh_bool_false);
    lh_gl_point_t before;
    lh_gl_container_get_clamped_scroll(box, lh_addr_of(before));
    lh_gl_container_scroll_by(box, dx, dy);
    return lh_gl_view_damage_scroll(self, box, before);
}

lh_bool_t
lh_gl_view_drag_thumb(lh_gl_view_t *self, lh_gl_view_pointer_t *pointer, lh_gl_scrollbar_t *bar,
                      lh_gl_point_t point)
{
    lh_gl_container_t *box = lh_gl_scrollbar_get_container(bar);
    lh_return_if(lh_null_eq(box) || !lh_gl_container_is_scroll_enabled(box), lh_bool_false);
    lh_gl_point_t before;
    lh_gl_container_get_clamped_scroll(box, lh_addr_of(before));

    lh_gl_scrollbar_set_thumb_start(bar, lh_gl_view_get_thumb_start_at(bar, point) -
                                             lh_gl_view_pointer_get_grab_offset(pointer));
    return lh_gl_view_damage_scroll(self, box, before);
}

lh_bool_t
lh_gl_view_drag_content(lh_gl_view_t *self, lh_gl_view_pointer_t *pointer, lh_gl_point_t point)
{
    lh_gl_point_t from;

    lh_gl_view_pointer_copy_last_point(pointer, lh_addr_of(from));
    lh_return_if(!lh_gl_view_pointer_is_dragged(pointer) &&
                     !lh_gl_view_pointer_is_past_threshold(pointer, point),
                 lh_bool_false);
    if (!lh_gl_view_pointer_is_dragged(pointer))
    {
        LH_ATTRIBUTE_UNUSED(lh_gl_view_release_target(self, pointer, point));
    }
    lh_gl_view_pointer_set_dragged(pointer, lh_bool_true);
    lh_gl_view_pointer_track(pointer, point);
    return lh_gl_view_scroll_by(
        self, lh_gl_view_pointer_get_drag(pointer),
        lh_gl_point_get_x(lh_addr_of(from)) - lh_gl_point_get_x(lh_addr_of(point)),
        lh_gl_point_get_y(lh_addr_of(from)) - lh_gl_point_get_y(lh_addr_of(point)));
}
