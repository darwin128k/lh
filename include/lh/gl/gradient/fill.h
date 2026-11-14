/**
 * @file fill.h
 * @brief A rectangle painted with a borrowed gradient: ::lh_gl_gradient_fill_t.
 *
 * The fill starts with an ::lh_gl_entity_t. Beside it is a borrowed
 * ::lh_gl_gradient_t. The shared draw call paints one row at a time, top to
 * bottom: the first row is fraction `0`, the last row is `255`. A single row
 * stays on the first color. Left and right are the same color. Diagonal and
 * radial placement are not this object.
 *
 * The class is not opaque. A null gradient draws nothing. Replacing the
 * pointer marks the area; writing a channel of the gradient does not.
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
    lh_gl_gradient_fill_fields(lh_gl_entity_t, lh_gl_gradient_t);
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
