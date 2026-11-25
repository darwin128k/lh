/**
 * @file event.c
 * @brief Implementation of `lh/gl/scrollbar.h`: the entity events of a bar.
 */

#include <lh/gl/scrollbar.h>

#include <lh/core/util/return.h>

lh_void
lh_gl_scrollbar_on_visible(const lh_gl_scrollbar_t *self, const lh_gl_entity_event_t *event)
{
    lh_return_if(lh_gl_entity_event_get_code(event) != lh_gl_entity_event_visible);
    lh_return_if(lh_gl_scrollbar_is_visible(self));
    lh_ptr_deref(lh_gl_entity_event_get_visible(event)) = lh_bool_false;
}

lh_void
lh_gl_scrollbar_on_draw(const lh_gl_scrollbar_t *self, const lh_gl_entity_event_t *event)
{
    lh_return_if(lh_gl_entity_event_get_code(event) != lh_gl_entity_event_draw);
    lh_gl_scrollbar_draw_thumb(self, lh_gl_entity_event_get_canvas(event));
}

lh_void
lh_gl_scrollbar_on_click(const lh_gl_scrollbar_t *self, const lh_gl_entity_event_t *event)
{
    lh_return_if(lh_gl_entity_event_get_code(event) != lh_gl_entity_event_click);
    lh_gl_container_t *box = lh_gl_scrollbar_get_container(self);
    lh_return_if(lh_null_eq(box) || !lh_gl_container_is_scroll_enabled(box));
    lh_gl_point_t point;
    lh_gl_entity_event_get_point(event, lh_addr_of(point));
    lh_gl_scrollbar_page_toward(self, point);
}

lh_void
lh_gl_scrollbar_event(const struct lh_gl_entity *self, const lh_gl_entity_event_t *event)
{
    /* The entity is the first field: self is the scrollbar. */
    const lh_gl_scrollbar_t *scrollbar = lh_ptr_rcast(const lh_gl_scrollbar_t, self);

    lh_gl_entity_class_event_base(lh_addr_of(lh_gl_scrollbar_class), self, event);
    lh_gl_scrollbar_on_draw(scrollbar, event);
    if (lh_gl_entity_event_get_code(event) == lh_gl_entity_event_clickable)
    {
        /* The thumb is drawn by this class and not by a child of its own, but the
           answer is the same one a button gives: the pointer over a scrollbar is the
           scrollbar's. */
        lh_ptr_deref(lh_gl_entity_event_get_clickable(event)) = lh_bool_true;
    }
    lh_gl_scrollbar_on_click(scrollbar, event);
    lh_gl_scrollbar_on_visible(scrollbar, event);
}
