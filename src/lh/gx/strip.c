/**
 * @file strip.c
 * @brief Implementation of `lh/gx/strip.h`.
 */

#include <lh/gx/strip.h>

#include <lh/core/util/return.h>

lh_bool_t
lh_gx_strip_draw(lh_gx_render_t *render, lh_gx_backend_sw_t *sw, const lh_gx_pixmap_t *band,
                 const lh_gx_present_t *present, lh_ptr context)
{
    lh_gx_damage_t damage;

    lh_gx_render_take_damage(render, lh_addr_of(damage));
    lh_return_if(lh_gx_damage_get_count(lh_addr_of(damage)) == 0U, lh_bool_false);
    const lh_gx_scalar_t rows = lh_gx_scalar_of(lh_gx_pixmap_get_height(band));

    for (lh_u32_t i = 0U; i < lh_gx_damage_get_count(lh_addr_of(damage)); ++i)
    {
        const lh_gx_rect_t *changed = lh_gx_damage_get_rect(lh_addr_of(damage), i);
        const lh_gx_point_t *corner = lh_gx_rect_get_origin(changed);
        const lh_gx_size_t *size = lh_gx_rect_get_size(changed);
        const lh_gx_scalar_t bottom = lh_gx_point_get_y(corner) + lh_gx_size_get_height(size);

        for (lh_gx_scalar_t top = lh_gx_point_get_y(corner); top < bottom; top += rows)
        {
            lh_gx_rect_t run;

            lh_gx_rect_init(lh_addr_of(run), lh_gx_point_get_x(corner), top,
                            lh_gx_size_get_width(size), lh_math_min(rows, bottom - top));
            lh_math_point_t origin;

            lh_math_point_init(lh_addr_of(origin), 0, lh_cast_static(lh_math_scalar_t, top));
            lh_gx_backend_sw_set_origin(sw, origin);
            lh_gx_render_paint_area(render, lh_addr_of(run));
            lh_gx_damage_t drawn;

            lh_gx_damage_init(lh_addr_of(drawn));
            lh_gx_damage_add(lh_addr_of(drawn), lh_addr_of(run));
            lh_gx_point_t at;

            lh_gx_point_init(lh_addr_of(at), 0, top);
            lh_gx_present_get_show(present)(context, band, at, lh_addr_of(drawn));
        }
    }
    return lh_bool_true;
}
