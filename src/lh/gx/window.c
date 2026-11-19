/**
 * @file window.c
 * @brief Implementation of `lh/gx/window.h`.
 */

#include <lh/gx/window.h>

#include <lh/core/attribute/unused.h>
#include <lh/core/util/return.h>
#include <lh/gx/input.h>
#include <lh/gx/window/input.h>

lh_os_window_t *
lh_gx_window_get_window_mut(lh_gx_window_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->window);
}

lh_os_system_surface_handle_t
lh_gx_window_get_surface(const lh_gx_window_t *self)
{
    lh_assert_runtime_ref(self);
    return self->surface;
}

lh_void
lh_gx_window_set_surface(lh_gx_window_t *self, lh_os_system_surface_handle_t surface)
{
    lh_assert_runtime_ref(self);
    self->surface = surface;
}

lh_memory_bounds_t *
lh_gx_window_get_bits_mut(lh_gx_window_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->bits);
}

const lh_gx_pixmap_t *
lh_gx_window_get_pixmap(const lh_gx_window_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->pixmap);
}

lh_gx_pixmap_t *
lh_gx_window_get_pixmap_mut(lh_gx_window_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->pixmap);
}

lh_gx_backend_sw_t *
lh_gx_window_get_sw_mut(lh_gx_window_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->sw);
}

lh_gx_canvas_t *
lh_gx_window_get_canvas_mut(lh_gx_window_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->canvas);
}

const lh_gx_render_t *
lh_gx_window_get_render(const lh_gx_window_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->render);
}

lh_gx_render_t *
lh_gx_window_get_render_mut(lh_gx_window_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->render);
}

const lh_gx_present_t *
lh_gx_window_get_present(const lh_gx_window_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->present);
}

lh_gx_present_t *
lh_gx_window_get_present_mut(lh_gx_window_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->present);
}

lh_gx_pointer_t *
lh_gx_window_get_pointer_mut(lh_gx_window_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->pointer);
}

const lh_gx_focus_t *
lh_gx_window_get_focus(const lh_gx_window_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->focus);
}

lh_gx_focus_t *
lh_gx_window_get_focus_mut(lh_gx_window_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->focus);
}

lh_gx_entity_t *
lh_gx_window_get_screen_mut(lh_gx_window_t *self)
{
    return lh_gx_render_get_screen_mut(lh_gx_window_get_render_mut(self));
}

lh_void
lh_gx_window_show(lh_ptr context, const lh_gx_pixmap_t *pixels, lh_gx_point_t at,
                  const lh_gx_damage_t *drawn)
{
    lh_gx_window_t *self = lh_ptr_rcast(lh_gx_window_t, context);

    LH_ATTRIBUTE_UNUSED(pixels);
    LH_ATTRIBUTE_UNUSED(at);
    for (lh_u32_t i = 0U; i < lh_gx_damage_get_count(drawn); ++i)
    {
        lh_math_rect_t area;

        lh_gx_rect_to_rect(lh_gx_damage_get_rect(drawn, i), lh_addr_of(area));
        const lh_math_point_t *at = lh_math_rect_get_origin(lh_addr_of(area));
        const lh_math_size_t *size = lh_math_rect_get_size(lh_addr_of(area));

        lh_os_window_invalidate_rect(lh_gx_window_get_window_mut(self), lh_math_point_get_x(at),
                                     lh_math_point_get_y(at),
                                     lh_math_point_get_x(at) + lh_math_size_get_width(size),
                                     lh_math_point_get_y(at) + lh_math_size_get_height(size));
    }
}

lh_bool_t
lh_gx_window_update(lh_gx_window_t *self)
{
    lh_return_if(lh_gx_window_get_surface(self) == LH_OS_SYSTEM_SURFACE_HANDLE_INVALID,
                 lh_bool_false);
    return lh_gx_present_draw(lh_gx_window_get_present(self), self,
                              lh_gx_window_get_render_mut(self), lh_gx_window_get_pixmap(self));
}

lh_void
lh_gx_window_paint(lh_gx_window_t *self)
{
    const lh_os_system_surface_handle_t surface = lh_gx_window_get_surface(self);

    lh_return_if(surface == LH_OS_SYSTEM_SURFACE_HANDLE_INVALID);
    LH_ATTRIBUTE_UNUSED(lh_os_system_surface_present_at(
        surface, lh_os_window_get_paint_target(lh_gx_window_get_window_mut(self)), 0, 0));
}

lh_void
lh_gx_window_deinit(lh_gx_window_t *self)
{
    lh_gx_pointer_cancel(lh_gx_window_get_pointer_mut(self));
    lh_os_window_deinit(lh_gx_window_get_window_mut(self));
    lh_os_system_surface_destroy(lh_gx_window_get_surface(self));
    lh_gx_window_set_surface(self, LH_OS_SYSTEM_SURFACE_HANDLE_INVALID);
}

lh_bool_t
lh_gx_window_try_resize(lh_gx_window_t *self, lh_s32_t width, lh_s32_t height)
{
    lh_os_system_surface_destroy(lh_gx_window_get_surface(self));
    const lh_os_system_surface_handle_t surface = lh_os_system_surface_create(width, height, 32);

    lh_gx_window_set_surface(self, surface);
    lh_return_if(surface == LH_OS_SYSTEM_SURFACE_HANDLE_INVALID, lh_bool_false);
    lh_memory_bounds_t *bits = lh_gx_window_get_bits_mut(self);
    lh_gx_pixmap_t *pixmap = lh_gx_window_get_pixmap_mut(self);

    lh_memory_bounds_init(bits, lh_os_system_surface_get_pixels(surface),
                          lh_cast_static(lh_usize_t, width) * lh_cast_static(lh_usize_t, height) *
                              4U);
    lh_gx_pixmap_init(pixmap, bits, lh_cast_static(lh_u32_t, width),
                      lh_cast_static(lh_u32_t, height), lh_gx_pixmap_format_argb8888);
    lh_gx_backend_sw_set_target(lh_gx_window_get_sw_mut(self), pixmap);
    lh_gx_rect_t place;

    lh_gx_rect_init(lh_addr_of(place), 0, 0, lh_gx_scalar_of(width), lh_gx_scalar_of(height));
    lh_gx_entity_set_place(lh_gx_window_get_screen_mut(self), place);
    lh_gx_render_add_damage(lh_gx_window_get_render_mut(self), lh_addr_of(place));
    return lh_bool_true;
}

lh_bool_t
lh_gx_window_take_input(lh_gx_window_t *self, const lh_os_input_event_t *event)
{
    lh_gx_event_t taken;

    lh_return_if(!lh_gx_window_input_try_get_event(event, lh_addr_of(taken)), lh_bool_false);
    lh_gx_input_take(lh_gx_window_get_pointer_mut(self), lh_gx_window_get_focus_mut(self),
                     lh_gx_window_get_screen_mut(self), lh_addr_of(taken));
    LH_ATTRIBUTE_UNUSED(lh_gx_window_update(self));
    return lh_bool_true;
}

lh_void
lh_gx_window_on_tick(lh_os_window_t *window, lh_ptr context)
{
    LH_ATTRIBUTE_UNUSED(window);
    LH_ATTRIBUTE_UNUSED(lh_gx_window_update(lh_ptr_rcast(lh_gx_window_t, context)));
}

lh_void
lh_gx_window_on_paint(lh_os_window_t *window, lh_ptr context)
{
    LH_ATTRIBUTE_UNUSED(window);
    lh_gx_window_paint(lh_ptr_rcast(lh_gx_window_t, context));
}

lh_void
lh_gx_window_on_resize(lh_os_window_t *window, int width, int height, lh_ptr context)
{
    lh_gx_window_t *self = lh_ptr_rcast(lh_gx_window_t, context);

    lh_return_if(width <= 0 || height <= 0);
    lh_gx_pointer_cancel(lh_gx_window_get_pointer_mut(self));
    lh_return_if(!lh_gx_window_try_resize(self, width, height));
    LH_ATTRIBUTE_UNUSED(lh_gx_window_update(self));
    lh_os_window_invalidate(window);
}

lh_void
lh_gx_window_on_input(lh_self_ptr self, const lh_os_input_event_ptr event)
{
    LH_ATTRIBUTE_UNUSED(lh_gx_window_take_input(lh_ptr_rcast(lh_gx_window_t, self), event));
}

lh_bool_t
lh_gx_window_try_open(lh_gx_window_t *self, struct lh_os_app *app, const lh_char_t *title,
                      lh_s32_t width, lh_s32_t height)
{
    lh_return_if(!lh_os_window_open(app, lh_gx_window_get_window_mut(self), title, width, height),
                 lh_bool_false);
    return lh_gx_window_try_resize(self, width, height);
}

lh_void
lh_gx_window_init(lh_gx_window_t *self)
{
    lh_os_window_t *window = lh_gx_window_get_window_mut(self);
    lh_gx_canvas_t *canvas = lh_gx_window_get_canvas_mut(self);
    lh_gx_backend_sw_t *sw = lh_gx_window_get_sw_mut(self);
    lh_gx_present_t *present = lh_gx_window_get_present_mut(self);

    lh_gx_window_set_surface(self, LH_OS_SYSTEM_SURFACE_HANDLE_INVALID);
    lh_gx_backend_sw_init(sw, lh_gx_window_get_pixmap_mut(self), 0, 0);
    lh_gx_backend_sw_canvas_init(canvas);
    lh_gx_render_init(lh_gx_window_get_render_mut(self), canvas, sw, 0, 0);
    lh_gx_present_init(present);
    lh_gx_present_set_show(present, lh_gx_window_show);
    lh_gx_pointer_init(lh_gx_window_get_pointer_mut(self));
    lh_gx_focus_init(lh_gx_window_get_focus_mut(self));
    lh_os_window_init(window);
    lh_os_window_set_on_paint(window, lh_gx_window_on_paint, self);
    lh_os_window_set_on_resize(window, lh_gx_window_on_resize, self);
    lh_os_window_set_on_input(window, lh_gx_window_on_input, self);
    lh_os_window_set_on_tick(window, lh_gx_window_on_tick, self, LH_GX_WINDOW_FRAME_MS);
}
