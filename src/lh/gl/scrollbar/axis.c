/**
 * @file axis.c
 * @brief Implementation of `lh/gl/scrollbar.h`: the container on the bar's axis: its scroll, its
 * viewport, its content, the track.
 */

#include <lh/gl/scrollbar.h>

#include <lh/core/util/return.h>

lh_gl_scalar_t
lh_gl_scrollbar_get_scroll(const lh_gl_scrollbar_t *self)
{
    lh_gl_point_t scroll;
    lh_gl_container_get_clamped_scroll(lh_gl_scrollbar_get_container(self), lh_addr_of(scroll));

    return lh_gl_point_get_along(lh_addr_of(scroll), lh_gl_scrollbar_get_axis(self));
}

lh_gl_scalar_t
lh_gl_scrollbar_get_scroll_max(const lh_gl_scrollbar_t *self)
{
    lh_gl_point_t max;
    lh_gl_container_get_scroll_max(lh_gl_scrollbar_get_container(self), lh_addr_of(max));

    return lh_gl_point_get_along(lh_addr_of(max), lh_gl_scrollbar_get_axis(self));
}

lh_gl_scalar_t
lh_gl_scrollbar_get_viewport_length(const lh_gl_scrollbar_t *self)
{
    lh_gl_size_t viewport;
    lh_gl_container_get_viewport_size(lh_gl_scrollbar_get_container(self), lh_addr_of(viewport));

    return lh_gl_size_get_along(lh_addr_of(viewport), lh_gl_scrollbar_get_axis(self));
}

lh_gl_scalar_t
lh_gl_scrollbar_get_content_length(const lh_gl_scrollbar_t *self)
{
    lh_gl_size_t content;
    lh_gl_container_get_content_size(lh_gl_scrollbar_get_container(self), lh_addr_of(content));

    return lh_gl_size_get_along(lh_addr_of(content), lh_gl_scrollbar_get_axis(self));
}

lh_gl_scalar_t
lh_gl_scrollbar_get_track_length(const lh_gl_scrollbar_t *self)
{
    return lh_gl_size_get_along(
        lh_gl_rect_get_size(lh_gl_entity_get_rect(lh_gl_scrollbar_get_entity(self))),
        lh_gl_scrollbar_get_axis(self));
}

lh_gl_container_t *
lh_gl_scrollbar_get_driven(lh_gl_entity_t *entity)
{
    const lh_gl_scrollbar_t *bar = lh_gl_entity_as_scrollbar(entity);

    return lh_null_eq(bar) ? lh_null : lh_gl_scrollbar_get_container(bar);
}

lh_gl_scrollbar_t *
lh_gl_scrollbar_get_bound(lh_gl_entity_t *entity, const lh_gl_container_t *container)
{
    lh_gl_scrollbar_t *bar = lh_gl_entity_as_scrollbar(entity);

    lh_return_if(lh_null_eq(bar) || lh_gl_scrollbar_get_container(bar) != container, lh_null);
    return bar;
}

lh_bool_t
lh_gl_scrollbar_is_needed(const lh_gl_scrollbar_t *self)
{
    return lh_gl_scrollbar_get_scroll_max(self) > lh_gl_scalar(0) ? lh_bool_true : lh_bool_false;
}

lh_gl_container_t *
lh_gl_scrollbar_find_scrolled(lh_gl_entity_t *entity)
{
    lh_gl_container_t *container = lh_gl_scrollbar_get_driven(entity);

    return lh_null_ne(container) ? container : lh_gl_entity_find_scrollable(entity);
}

lh_bool_t
lh_gl_scrollbar_is_visible(const lh_gl_scrollbar_t *self)
{
    return lh_gl_scrollbar_get_mode(self) == lh_gl_scrollbar_mode_always ||
                   (lh_gl_scrollbar_get_mode(self) == lh_gl_scrollbar_mode_auto &&
                    lh_gl_scrollbar_is_needed(self))
               ? lh_bool_true
               : lh_bool_false;
}
