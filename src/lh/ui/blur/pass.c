#include <lh/ui/blur/pass.h>
#include <lh/math.h>
#include <lh/ui/blur.h>
#include <lh/ui/radius.h>
#include <lh/util/return.h>

lh_bool_t
lh_ui_blur_pass_init(const lh_ui_pixmap_t *pixmap, const lh_ui_rect_t *rect, lh_s32_t radius,
                     const lh_u8_t *scratch, const lh_ui_rect_t *shape, lh_ui_scalar_t corner,
                     lh_ui_blur_pass_t *out)
{
    const lh_ui_point_t *origin = lh_ui_rect_get_origin(rect);
    const lh_ui_size_t *size = lh_ui_rect_get_size(rect);

    lh_assert_runtime_ref(out);
    out->scratch = scratch;
    out->left = lh_ui_point_get_x(origin);
    out->top = lh_ui_point_get_y(origin);
    out->width = lh_ui_size_get_width(size);
    out->height = lh_ui_size_get_height(size);
    out->radius = radius;
    out->corner = lh_null_ne(shape) && corner > lh_ui_scalar(0) ? lh_ui_radius_clamp(shape, corner)
                                                                : lh_ui_scalar(0);
    out->shape = out->corner > lh_ui_scalar(0) ? shape : lh_null;
    lh_return_if(radius < 0 || lh_null_eq(scratch) || out->width <= 0 || out->height <= 0,
                 lh_bool_false);
    lh_ui_blur_band(pixmap, out->top, out->height, radius, lh_addr_of(out->y0),
                    lh_addr_of(out->y1));
    return lh_bool_true;
}
