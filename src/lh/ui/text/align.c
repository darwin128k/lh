/**
 * @file align.c
 * @brief Implementation of `lh/ui/text/align.h`.
 */

#include <lh/math.h>
#include <lh/null.h>
#include <lh/ui/text/align.h>
#include <lh/util/addr.h>

lh_ui_point_t
lh_ui_text_align_get_origin(const lh_ui_rect_t *box, const lh_ui_insets_t *padding, lh_ui_size_t size,
                            lh_ui_text_align_h_t horizontal, lh_ui_text_align_v_t vertical)
{
    lh_ui_rect_t empty_box;
    lh_ui_insets_t no_padding;
    lh_ui_size_t room;
    lh_ui_scalar_t dx = 0;
    lh_ui_scalar_t dy = 0;
    lh_ui_point_t origin;

    if (lh_null_eq(box))
    {
        lh_ui_rect_init_empty(lh_addr_of(empty_box));
        box = lh_addr_of(empty_box);
    }
    if (lh_null_eq(padding))
    {
        lh_ui_insets_init_all(lh_addr_of(no_padding), 0);
        padding = lh_addr_of(no_padding);
    }

    /* Only the room the padding leaves, and never past the near edge: text wider
       than its box has nowhere to go, so every alignment starts it at the left or
       the top and it overflows the same way for all of them. */
    lh_ui_size_init(lh_addr_of(room), lh_ui_size_get_width(lh_ui_rect_get_size_as_const(box)) -
                                    lh_ui_insets_get_left((padding)) -
                                    lh_ui_insets_get_right((padding)),
                lh_ui_size_get_height(lh_ui_rect_get_size_as_const(box)) -
                                    lh_ui_insets_get_top((padding)) -
                                    lh_ui_insets_get_bottom((padding)));

    if (horizontal == lh_ui_text_align_h_center)
    {
        dx = lh_math_max(0, lh_ui_size_get_width(lh_addr_of(room)) - lh_ui_size_get_width(lh_addr_of(size))) / 2;
    }
    else if (horizontal == lh_ui_text_align_h_right)
    {
        dx = lh_math_max(0, lh_ui_size_get_width(lh_addr_of(room)) - lh_ui_size_get_width(lh_addr_of(size)));
    }
    if (vertical == lh_ui_text_align_v_center)
    {
        dy = lh_math_max(0, lh_ui_size_get_height(lh_addr_of(room)) - lh_ui_size_get_height(lh_addr_of(size))) / 2;
    }
    else if (vertical == lh_ui_text_align_v_bottom)
    {
        dy = lh_math_max(0, lh_ui_size_get_height(lh_addr_of(room)) - lh_ui_size_get_height(lh_addr_of(size)));
    }

    lh_ui_point_init(lh_addr_of(origin),
                     lh_ui_point_get_x(lh_ui_rect_get_origin_as_const(box)) + lh_ui_insets_get_left((padding)) +
                         dx,
                     lh_ui_point_get_y(lh_ui_rect_get_origin_as_const(box)) + lh_ui_insets_get_top((padding)) +
                         dy);
    return origin;
}