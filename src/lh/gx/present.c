/**
 * @file present.c
 * @brief Implementation of `lh/gx/present.h`.
 */

#include <lh/gx/present.h>

#include <lh/core/util/return.h>

lh_gx_present_show_fn *
lh_gx_present_get_show(const lh_gx_present_t *self)
{
    lh_assert_runtime_ref(self);
    return self->show;
}

lh_void
lh_gx_present_set_show(lh_gx_present_t *self, lh_gx_present_show_fn *fn)
{
    lh_assert_runtime_ref(self);
    self->show = fn;
}

lh_void
lh_gx_present_init(lh_gx_present_t *self)
{
    lh_gx_present_set_show(self, lh_null);
}

lh_bool_t
lh_gx_present_is_complete(const lh_gx_present_t *self)
{
    return lh_null_ne(lh_ptr_rcast(lh_void, lh_gx_present_get_show(self)));
}

lh_bool_t
lh_gx_present_draw(const lh_gx_present_t *self, lh_ptr context, lh_gx_render_t *render,
                   const lh_gx_pixmap_t *pixels)
{
    lh_gx_damage_t drawn;

    lh_gx_render_draw(render, lh_addr_of(drawn));
    lh_return_if(lh_gx_damage_get_count(lh_addr_of(drawn)) == 0U, lh_bool_false);
    lh_gx_point_t at;

    lh_gx_point_init(lh_addr_of(at), 0, 0);
    lh_gx_present_get_show(self)(context, pixels, at, lh_addr_of(drawn));
    return lh_bool_true;
}
