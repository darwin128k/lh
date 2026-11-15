/**
 * @file align.c
 * @brief Implementation of `lh/gl/text/align.h`.
 */

#include <lh/core/math.h>
#include <lh/gl/text/align.h>

lh_void
lh_gl_text_align_get_origin(const lh_gl_rect_t *box, const lh_gl_insets_t *padding,
                            lh_gl_size_t size, lh_gl_text_align_h_t horizontal,
                            lh_gl_text_align_v_t vertical, lh_gl_point_t *out)
{
    lh_assert_runtime_ref(out);
    lh_gl_scalar_t dx = 0;
    lh_gl_scalar_t dy = 0;

    /* The empty box lives here, not inside the branch below: the pointer that
       names it is read after the branch ends, so a variable declared inside it
       would already be gone. */
    lh_gl_rect_t empty_box;

    if (lh_null_eq(box))
    {
        lh_gl_rect_init_empty(lh_addr_of(empty_box));
        box = lh_addr_of(empty_box);
    }
    lh_gl_insets_t no_padding;

    if (lh_null_eq(padding))
    {
        lh_gl_insets_init_all(lh_addr_of(no_padding), 0);
        padding = lh_addr_of(no_padding);
    }
    lh_gl_size_t room;

    /* Only the room the padding leaves, and never past the near edge: text wider
       than its box has nowhere to go, so every alignment starts it at the left or
       the top and it overflows the same way for all of them. */
    lh_gl_size_init(lh_addr_of(room),
                    lh_gl_size_get_width(lh_gl_rect_get_size(box)) -
                        lh_gl_insets_get_left((padding)) - lh_gl_insets_get_right((padding)),
                    lh_gl_size_get_height(lh_gl_rect_get_size(box)) -
                        lh_gl_insets_get_top((padding)) - lh_gl_insets_get_bottom((padding)));

    if (horizontal == lh_gl_text_align_h_center)
    {
        /* ::lh_gl_scalar_centre: half-up, leftover half-pixel to the right / bottom.
           Measured on the demo's Hide panel button (box 180..207, ink 15 rows). */
        dx = lh_gl_scalar_centre(lh_gl_size_get_width(lh_addr_of(room)),
                                 lh_gl_size_get_width(lh_addr_of(size)));
    }
    else if (horizontal == lh_gl_text_align_h_right)
    {
        dx = lh_gl_scalar_slack(lh_gl_size_get_width(lh_addr_of(room)),
                                lh_gl_size_get_width(lh_addr_of(size)));
    }
    if (vertical == lh_gl_text_align_v_center)
    {
        dy = lh_gl_scalar_centre(lh_gl_size_get_height(lh_addr_of(room)),
                                 lh_gl_size_get_height(lh_addr_of(size)));
    }
    else if (vertical == lh_gl_text_align_v_bottom)
    {
        dy = lh_gl_scalar_slack(lh_gl_size_get_height(lh_addr_of(room)),
                                lh_gl_size_get_height(lh_addr_of(size)));
    }

    lh_gl_point_t origin;
    lh_gl_point_init(
        lh_addr_of(origin),
        lh_gl_point_get_x(lh_gl_rect_get_origin(box)) + lh_gl_insets_get_left((padding)) + dx,
        lh_gl_point_get_y(lh_gl_rect_get_origin(box)) + lh_gl_insets_get_top((padding)) + dy);
    lh_ptr_deref(out) = origin;
}