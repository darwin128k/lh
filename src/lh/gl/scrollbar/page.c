/**
 * @file page.c
 * @brief Implementation of `lh/gl/scrollbar.h`: paging: a click on the track moves a page toward
 * it.
 */

#include <lh/gl/scrollbar.h>

#include <lh/core/util/return.h>

lh_gl_scalar_t
lh_gl_scrollbar_get_page_toward(const lh_gl_scrollbar_t *self, lh_gl_point_t point)
{
    lh_gl_rect_t thumb;
    lh_gl_scrollbar_get_thumb_rect(self, lh_addr_of(thumb));
    const lh_gl_scalar_t page = lh_gl_scrollbar_get_viewport_length(self);

    lh_return_if(lh_gl_rect_contains_point(lh_addr_of(thumb), point), lh_gl_scalar(0));
    return lh_gl_point_get_along(lh_addr_of(point), lh_gl_scrollbar_get_axis(self)) <
                   lh_gl_point_get_along(lh_gl_rect_get_origin(lh_addr_of(thumb)),
                                         lh_gl_scrollbar_get_axis(self))
               ? lh_math_neg(page)
               : page;
}

lh_void
lh_gl_scrollbar_page_toward(const lh_gl_scrollbar_t *self, lh_gl_point_t point)
{
    lh_gl_point_t delta;

    lh_gl_point_init_along(lh_addr_of(delta), lh_gl_scrollbar_get_axis(self),
                           lh_gl_scrollbar_get_page_toward(self, point), lh_gl_scalar(0));
    lh_gl_container_scroll_by(lh_gl_scrollbar_get_container(self),
                              lh_gl_point_get_x(lh_addr_of(delta)),
                              lh_gl_point_get_y(lh_addr_of(delta)));
}
