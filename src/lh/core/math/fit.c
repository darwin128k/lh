/** @file fit.c
 * @brief Shared viewport geometry for presentation and pointer coordinates.
 */
#include <lh/core/math/fit.h>

#include <limits.h>
#include <lh/core/runtime/assert.h>

lh_bool_t
lh_math_fit_try_rect(lh_math_rect_t *out, int src_w, int src_h, int dst_w, int dst_h)
{
    if (lh_null_eq(out) || src_w <= 0 || src_h <= 0 || dst_w <= 0 || dst_h <= 0)
    {
        return lh_bool_false;
    }
    int width = dst_w;
    int height = dst_h;
    if (lh_cast_static(lh_s64_t, dst_w) * src_h > lh_cast_static(lh_s64_t, dst_h) * src_w)
    {
        width = lh_cast_static(int, lh_cast_static(lh_s64_t, dst_h) * src_w / src_h);
    }
    else
    {
        height = lh_cast_static(int, lh_cast_static(lh_s64_t, dst_w) * src_h / src_w);
    }
    width = lh_math_max(1, width);
    height = lh_math_max(1, height);
    lh_math_rect_init(out, (dst_w - width) / 2, (dst_h - height) / 2, width, height);
    return lh_bool_true;
}

lh_bool_t
lh_math_fit_try_map_point(lh_math_point_t *out, const lh_math_rect_t *fit, int src_w, int src_h,
                          lh_math_point_t point)
{
    if (lh_null_eq(out) || lh_null_eq(fit) || src_w <= 0 || src_h <= 0)
    {
        return lh_bool_false;
    }
    int width = lh_math_size_get_width(lh_math_rect_get_size(fit));
    int height = lh_math_size_get_height(lh_math_rect_get_size(fit));
    if (width <= 0 || height <= 0)
    {
        return lh_bool_false;
    }
    lh_s64_t x = (lh_cast_static(lh_s64_t, lh_math_point_get_x(lh_addr_of(point))) -
                  lh_math_point_get_x(lh_math_rect_get_origin(fit))) *
                 src_w / width;
    lh_s64_t y = (lh_cast_static(lh_s64_t, lh_math_point_get_y(lh_addr_of(point))) -
                  lh_math_point_get_y(lh_math_rect_get_origin(fit))) *
                 src_h / height;
    if (x < INT_MIN || x > INT_MAX || y < INT_MIN || y > INT_MAX)
    {
        return lh_bool_false;
    }
    lh_math_point_init(out, lh_cast_static(int, x), lh_cast_static(int, y));
    return lh_bool_true;
}

lh_void
lh_math_fit_rect(lh_math_rect_t *out, int src_w, int src_h, int dst_w, int dst_h)
{
    lh_runtime_check_ifn(lh_math_fit_try_rect(out, src_w, src_h, dst_w, dst_h),
                         lh_runtime_error_code_invalid_argument);
}

lh_void
lh_math_fit_map_point(lh_math_point_t *out, const lh_math_rect_t *fit, int src_w, int src_h,
                      lh_math_point_t point)
{
    lh_runtime_check_ifn(lh_math_fit_try_map_point(out, fit, src_w, src_h, point),
                         lh_runtime_error_code_invalid_argument);
}
