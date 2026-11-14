/**
 * @file fill.h
 * @brief A rectangle painted with a borrowed gradient: ::lh_gl_gradient_fill_t.
 *
 * The fill starts with an ::lh_gl_entity_t. Beside it is a borrowed
 * ::lh_gl_gradient_t and an angle in degrees: `0` points up, `90` to the right,
 * `180` down. Init leaves the angle at `180`. Each pixel takes
 * ::lh_gl_gradient_linear_frac. A row is one color when the axis is vertical,
 * and a column when it is horizontal. A diagonal paints pixel by pixel.
 * `radius` clips that paint to the rounded rectangle: a corner outside the
 * arc is not painted. Radius `0` is the plain rectangle. A null gradient
 * draws nothing. The class is not opaque. Replacing the gradient pointer,
 * the angle or the radius marks the area. Writing a channel of the gradient
 * does not.
 */

#ifndef LH_GL_GRADIENT_FILL_H
#define LH_GL_GRADIENT_FILL_H

#include <lh/gl/entity.h>
#include <lh/gl/gradient.h>
#include <lh/gl/gradient/fill/fields.h>

/**
 * @struct lh_gl_gradient_fill
 * @brief An entity plus a borrowed gradient. Fields via
 *        ::lh_gl_gradient_fill_fields.
 */
struct lh_gl_gradient_fill
{
    /** Members: ::lh_gl_gradient_fill_fields. */
    lh_gl_gradient_fill_fields(lh_gl_entity_t, lh_gl_gradient_t, lh_gl_scalar_t);
};
/** @copybrief lh_gl_gradient_fill */
typedef struct lh_gl_gradient_fill lh_gl_gradient_fill_t;

LH_COMPILER_EXTERN_C_BEGIN

/** @brief This fill as an entity. */
lh_gl_entity_t *
lh_gl_gradient_fill_get_entity_mut(lh_gl_gradient_fill_t *self);

/** @brief This fill as an entity. */
const lh_gl_entity_t *
lh_gl_gradient_fill_get_entity(const lh_gl_gradient_fill_t *self);

/** @brief Borrowed gradient, or ::lh_null. */
const lh_gl_gradient_t *
lh_gl_gradient_fill_get_gradient(const lh_gl_gradient_fill_t *self);

/** @brief Borrow @p gradient. @p gradient may be ::lh_null. Marks the area. */
lh_void
lh_gl_gradient_fill_set_gradient(lh_gl_gradient_fill_t *self, const lh_gl_gradient_t *gradient);

/** @brief Degrees. `0` up, `90` right, `180` down. */
lh_gl_scalar_t
lh_gl_gradient_fill_get_angle(const lh_gl_gradient_fill_t *self);

/** @brief Store the angle and mark the area. */
lh_void
lh_gl_gradient_fill_set_angle(lh_gl_gradient_fill_t *self, lh_gl_scalar_t angle);

/** @brief Corner radius in pixels. `0` is a rectangle. */
lh_gl_scalar_t
lh_gl_gradient_fill_get_radius(const lh_gl_gradient_fill_t *self);

/** @brief Store the radius and mark the area. A negative value is stored as zero. */
lh_void
lh_gl_gradient_fill_set_radius(lh_gl_gradient_fill_t *self, lh_gl_scalar_t radius);

/** @brief The class every gradient fill has. Kind entity, not opaque. */
const lh_gl_entity_class_t *
lh_gl_gradient_fill_get_class(void);

/** @brief Entity of @p rect that borrows @p gradient. */
lh_void
lh_gl_gradient_fill_init(lh_gl_gradient_fill_t *self, lh_gl_rect_t rect,
                         const lh_gl_gradient_t *gradient);

/** @brief Draw call of ::lh_gl_gradient_fill_get_class. */
lh_void
lh_gl_gradient_fill_draw(const lh_gl_entity_t *entity, lh_gl_canvas_t *canvas);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_GRADIENT_FILL_H */
