/**
 * @file key.h
 * @brief What a cached shadow mask was made for: ::lh_gl_canvas_sw_shadow_key_t.
 *
 * The software canvas keeps the last shadow mask it computed, and stamps it
 * again when the next shadow asks for the same one instead of computing it a
 * second time. The key is what "the same" means: the mask's size, the corner,
 * the box that casts it, the spread, the offset and the peak alpha, all in
 * whole pixels. Two keys that are equal (::lh_gl_canvas_sw_shadow_key_is_equal) make
 * the same mask.
 */

#ifndef LH_GL_CANVAS_SW_SHADOW_KEY_H
#define LH_GL_CANVAS_SW_SHADOW_KEY_H

#include <lh/core/bool.h>
#include <lh/core/compiler/extern/c.h>
#include <lh/core/numeric/fixed/types.h>
#include <lh/core/void.h>
#include <lh/gl/canvas/sw/shadow/key/fields.h>

/**
 * @struct lh_gl_canvas_sw_shadow_key
 * @brief What a cached shadow mask was made for.
 */
struct lh_gl_canvas_sw_shadow_key
{
    /** Members: ::lh_gl_canvas_sw_shadow_key_fields. */
    lh_gl_canvas_sw_shadow_key_fields();
};
/** @copybrief lh_gl_canvas_sw_shadow_key */
typedef struct lh_gl_canvas_sw_shadow_key lh_gl_canvas_sw_shadow_key_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The mask width, in pixels (level 1).
 *
 * @param self The key; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_gl_canvas_sw_shadow_key_get_width(const lh_gl_canvas_sw_shadow_key_t *self);

/**
 * @brief Store the mask width, in pixels (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The key; checked by ::lh_assert_runtime_ref.
 * @param width The new value.
 */
lh_void
lh_gl_canvas_sw_shadow_key_set_width(lh_gl_canvas_sw_shadow_key_t *self, lh_s32_t width);

/**
 * @brief The mask height, in pixels (level 1).
 *
 * @param self The key; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_gl_canvas_sw_shadow_key_get_height(const lh_gl_canvas_sw_shadow_key_t *self);

/**
 * @brief Store the mask height, in pixels (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The key; checked by ::lh_assert_runtime_ref.
 * @param height The new value.
 */
lh_void
lh_gl_canvas_sw_shadow_key_set_height(lh_gl_canvas_sw_shadow_key_t *self, lh_s32_t height);

/**
 * @brief The corner radius, in whole pixels (level 1).
 *
 * @param self The key; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_gl_canvas_sw_shadow_key_get_corner(const lh_gl_canvas_sw_shadow_key_t *self);

/**
 * @brief Store the corner radius, in whole pixels (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The key; checked by ::lh_assert_runtime_ref.
 * @param corner The new value.
 */
lh_void
lh_gl_canvas_sw_shadow_key_set_corner(lh_gl_canvas_sw_shadow_key_t *self, lh_s32_t corner);

/**
 * @brief The width of the box casting it (level 1).
 *
 * @param self The key; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_gl_canvas_sw_shadow_key_get_box_w(const lh_gl_canvas_sw_shadow_key_t *self);

/**
 * @brief Store the width of the box casting it (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The key; checked by ::lh_assert_runtime_ref.
 * @param box_w The new value.
 */
lh_void
lh_gl_canvas_sw_shadow_key_set_box_w(lh_gl_canvas_sw_shadow_key_t *self, lh_s32_t box_w);

/**
 * @brief The height of the box casting it (level 1).
 *
 * @param self The key; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_gl_canvas_sw_shadow_key_get_box_h(const lh_gl_canvas_sw_shadow_key_t *self);

/**
 * @brief Store the height of the box casting it (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The key; checked by ::lh_assert_runtime_ref.
 * @param box_h The new value.
 */
lh_void
lh_gl_canvas_sw_shadow_key_set_box_h(lh_gl_canvas_sw_shadow_key_t *self, lh_s32_t box_h);

/**
 * @brief The spread, in whole pixels (level 1).
 *
 * @param self The key; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_gl_canvas_sw_shadow_key_get_spread(const lh_gl_canvas_sw_shadow_key_t *self);

/**
 * @brief Store the spread, in whole pixels (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The key; checked by ::lh_assert_runtime_ref.
 * @param spread The new value.
 */
lh_void
lh_gl_canvas_sw_shadow_key_set_spread(lh_gl_canvas_sw_shadow_key_t *self, lh_s32_t spread);

/**
 * @brief The offset across, in whole pixels (level 1).
 *
 * @param self The key; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_gl_canvas_sw_shadow_key_get_offset_x(const lh_gl_canvas_sw_shadow_key_t *self);

/**
 * @brief Store the offset across, in whole pixels (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The key; checked by ::lh_assert_runtime_ref.
 * @param offset_x The new value.
 */
lh_void
lh_gl_canvas_sw_shadow_key_set_offset_x(lh_gl_canvas_sw_shadow_key_t *self, lh_s32_t offset_x);

/**
 * @brief The offset down, in whole pixels (level 1).
 *
 * @param self The key; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_gl_canvas_sw_shadow_key_get_offset_y(const lh_gl_canvas_sw_shadow_key_t *self);

/**
 * @brief Store the offset down, in whole pixels (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The key; checked by ::lh_assert_runtime_ref.
 * @param offset_y The new value.
 */
lh_void
lh_gl_canvas_sw_shadow_key_set_offset_y(lh_gl_canvas_sw_shadow_key_t *self, lh_s32_t offset_y);

/**
 * @brief The alpha of the shadow colour (level 1).
 *
 * @param self The key; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_gl_canvas_sw_shadow_key_get_peak(const lh_gl_canvas_sw_shadow_key_t *self);

/**
 * @brief Store the alpha of the shadow colour (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The key; checked by ::lh_assert_runtime_ref.
 * @param peak The new value.
 */
lh_void
lh_gl_canvas_sw_shadow_key_set_peak(lh_gl_canvas_sw_shadow_key_t *self, lh_s32_t peak);

/**
 * @brief Make @p self the key of a mask of @p width by @p height for a box of
 *        @p box_w by @p box_h with @p corner, @p spread, the offset
 *        (@p offset_x, @p offset_y) and @p peak alpha.
 *
 * @param self The key; checked by ::lh_assert_runtime_ref.
 */
lh_void
lh_gl_canvas_sw_shadow_key_init(lh_gl_canvas_sw_shadow_key_t *self, lh_s32_t width, lh_s32_t height,
                                lh_s32_t corner, lh_s32_t box_w, lh_s32_t box_h, lh_s32_t spread,
                                lh_s32_t offset_x, lh_s32_t offset_y, lh_s32_t peak);

/**
 * @brief Whether @p self and @p other are the key of the same mask: all nine
 *        numbers equal.
 *
 * @param self  A key; checked by ::lh_assert_runtime_ref.
 * @param other Another; checked by ::lh_assert_runtime_ref.
 */
lh_bool_t
lh_gl_canvas_sw_shadow_key_is_equal(const lh_gl_canvas_sw_shadow_key_t *self,
                                    const lh_gl_canvas_sw_shadow_key_t *other);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_CANVAS_SW_SHADOW_KEY_H */
