/**
 * @file image.h
 * @brief A picture from memory. The bytes are not copied.
 *
 * Pixels are 8-bit RGBA, the layout of ::lh_ui_color_t, @p stride pixels
 * from one row to the next. A button holds an image the way it holds a
 * label: the image is a child, so the button can show a picture, words,
 * or both.
 */

#ifndef LH_ENTITY_IMAGE_H
#define LH_ENTITY_IMAGE_H

#include <lh/byte.h>
#include <lh/compiler/extern/c.h>
#include <lh/entity/2d.h>
#include <lh/numeric/types.h>

/**
 * @struct lh_entity_image
 * @brief A blit of caller-owned pixels.
 */
struct lh_entity_image
{
    lh_entity_fields(lh_entity_class_t, lh_list_node_t, lh_list_t, lh_entity_flags_t);
    lh_entity_2d_fields(lh_math_vec2_t, lh_float_t, const lh_ui_style_t *, const lh_ui_effect_t *);
    const lh_byte_t *pixels;
    lh_int_t width;
    lh_int_t height;
    lh_int_t stride;
};
typedef struct lh_entity_image lh_entity_image_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Class of ::lh_entity_image_t, derived from ::lh_entity_2d_class.
 */
extern const lh_entity_class_t lh_entity_image_class;

/**
 * @brief Point @p self at @p pixels. Not copied. @p stride is pixels per
 *        row. The box becomes @p width by @p height.
 */
lh_void
lh_entity_image_set_pixels(lh_entity_image_t *self, const lh_byte_t *pixels, lh_int_t width,
                           lh_int_t height, lh_int_t stride);

LH_COMPILER_EXTERN_C_END

#endif /* LH_ENTITY_IMAGE_H */
