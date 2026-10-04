#include <lh/entity/image.h>
#include <lh/assert.h>
#include <lh/entity.h>
#include <lh/null.h>
#include <lh/ui/color.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

lh_void
lh_entity_image_construct(lh_entity_t *self)
{
    lh_entity_add_flags(self, lh_entity_flags_own_background);
}

lh_void
lh_entity_image_on_event(lh_entity_t *self, lh_entity_event_t *event)
{
    lh_entity_image_t *const image = lh_ptr_rcast(lh_entity_image_t, self);
    lh_ui_canvas_t *canvas;
    lh_math_rect_t bounds;
    lh_int_t y;
    if (lh_entity_event_get_code(event) != LH_ENTITY_EVENT_DRAW || lh_ptr_is_null(image->pixels) ||
        image->width <= 0 || image->height <= 0 || image->stride <= 0)
    {
        return;
    }
    canvas = lh_ptr_rcast(lh_ui_canvas_t, lh_entity_event_get_param(event));
    bounds = lh_entity_2d_get_screen_bounds(lh_ptr_rcast(const lh_entity_2d_t, self));
    for (y = 0; y < image->height; ++y)
    {
        lh_int_t x;
        const lh_byte_t *const row = image->pixels + (lh_usize_t)y * (lh_usize_t)image->stride * 4U;
        for (x = 0; x < image->width; ++x)
        {
            const lh_byte_t *const pixel = row + (lh_usize_t)x * 4U;
            lh_ui_canvas_blend_pixel(canvas, lh_math_rect_get_x(lh_addr_of(bounds)) + x,
                                     lh_math_rect_get_y(lh_addr_of(bounds)) + y,
                                     lh_ui_color_make(pixel[0], pixel[1], pixel[2], pixel[3]));
        }
    }
}

const lh_entity_class_t lh_entity_image_class =
    lh_entity_class_initializer(&lh_entity_2d_class, sizeof(lh_entity_image_t),
                                lh_entity_image_construct, lh_null, lh_entity_image_on_event);

lh_void
lh_entity_image_set_pixels(lh_entity_image_t *self, const lh_byte_t *pixels, lh_int_t width,
                           lh_int_t height, lh_int_t stride)
{
    lh_assert_runtime_ref(self);
    self->pixels = pixels;
    self->width = width;
    self->height = height;
    self->stride = stride;
    lh_entity_2d_set_size(lh_ptr_rcast(lh_entity_2d_t, self),
                          lh_math_vec2_make((lh_float_t)width, (lh_float_t)height));
}
