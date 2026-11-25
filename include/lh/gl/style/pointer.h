/**
 * @file pointer.h
 * @brief How a style answers a pointer: where it is hit and how it looks pressed:
 * ::lh_gl_style_pointer_t.
 *
 * `hit_radius` rounds the corners of what a hit test accepts (`0` = the
 * whole rect: a rounded look is pressed by its rect, which is what a finger
 * aims at), and `pressed` is the style in effect while the entity is pressed
 * (not owned, ::lh_null = it looks the same either way). A part of
 * ::lh_gl_style_t that lives inside it (::lh_gl_style_get_pointer).
 */

#ifndef LH_GL_STYLE_POINTER_H
#define LH_GL_STYLE_POINTER_H

#include <lh/core/compiler/extern/c.h>
#include <lh/core/void.h>
#include <lh/gl/scalar.h>
#include <lh/gl/style/pointer/fields.h>

struct lh_gl_style;

/**
 * @struct lh_gl_style_pointer
 * @brief How a style answers a pointer: where it is hit and how it looks pressed.
 */
struct lh_gl_style_pointer
{
    /** Members: ::lh_gl_style_pointer_fields. */
    lh_gl_style_pointer_fields(lh_gl_scalar_t, struct lh_gl_style);
};
/** @copybrief lh_gl_style_pointer */
typedef struct lh_gl_style_pointer lh_gl_style_pointer_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The style in effect while the entity is pressed (not owned, may be ::lh_null) (level 1).
 *
 * @param self The pointer; checked by ::lh_assert_runtime_ref.
 */
const struct lh_gl_style *
lh_gl_style_pointer_get_pressed(const lh_gl_style_pointer_t *self);

/**
 * @brief Store the style in effect while the entity is pressed (not owned, may be ::lh_null) (level
 * 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The pointer; checked by ::lh_assert_runtime_ref.
 * @param pressed The new value.
 */
lh_void
lh_gl_style_pointer_set_pressed(lh_gl_style_pointer_t *self, const struct lh_gl_style *pressed);

/**
 * @brief The corner radius hits are tested against (level 1).
 *
 * @param self The pointer; checked by ::lh_assert_runtime_ref.
 */
lh_gl_scalar_t
lh_gl_style_pointer_get_hit_radius(const lh_gl_style_pointer_t *self);

/**
 * @brief Store the corner radius hits are tested against (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The pointer; checked by ::lh_assert_runtime_ref.
 * @param hit_radius The new value.
 */
lh_void
lh_gl_style_pointer_set_hit_radius(lh_gl_style_pointer_t *self, lh_gl_scalar_t hit_radius);

/**
 * @brief Make @p self the part of a fresh style (::lh_gl_style_init): Hits on the whole rect, and
 * the same look pressed.
 *
 * @param self The pointer; checked by ::lh_assert_runtime_ref.
 */
lh_void
lh_gl_style_pointer_init(lh_gl_style_pointer_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_STYLE_POINTER_H */
