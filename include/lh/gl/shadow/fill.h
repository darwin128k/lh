/**
 * @file fill.h
 * @brief The shadow of an entity rectangle: ::lh_gl_shadow_fill_t.
 *
 * The fill starts with an ::lh_gl_entity_t and borrows an ::lh_gl_shadow_t. The
 * entity rectangle is the area the spread and the shift can reach, so a layer
 * clip does not cut the fade off. `box` is the rectangle that casts the shadow,
 * in the same coordinates. The draw leaves that box clear. Put the solid fill
 * of the box in front of this one.
 *
 * The class is not opaque. `radius` rounds the box the fade follows. `0` is
 * square. Replacing the pointer or the radius marks the area.
 */

#ifndef LH_GL_SHADOW_FILL_H
#define LH_GL_SHADOW_FILL_H

#include <lh/gl/entity.h>
#include <lh/gl/shadow.h>
#include <lh/gl/shadow/fill/fields.h>

/**
 * @struct lh_gl_shadow_fill
 * @brief An entity plus a borrowed shadow. Fields via ::lh_gl_shadow_fill_fields.
 */
struct lh_gl_shadow_fill
{
    /** Members: ::lh_gl_shadow_fill_fields. */
    lh_gl_shadow_fill_fields(lh_gl_entity_t, lh_gl_shadow_t, lh_gl_rect_t, lh_gl_scalar_t);
};
/** @copybrief lh_gl_shadow_fill */
typedef struct lh_gl_shadow_fill lh_gl_shadow_fill_t;

LH_COMPILER_EXTERN_C_BEGIN

/** @brief This fill as an entity. */
lh_gl_entity_t *
lh_gl_shadow_fill_get_entity_mut(lh_gl_shadow_fill_t *self);

/** @brief This fill as an entity. */
const lh_gl_entity_t *
lh_gl_shadow_fill_get_entity(const lh_gl_shadow_fill_t *self);

/** @brief Borrowed shadow, or ::lh_null. */
const lh_gl_shadow_t *
lh_gl_shadow_fill_get_shadow(const lh_gl_shadow_fill_t *self);

/** @brief The box that casts the shadow, in the same coordinates as the entity. */
const lh_gl_rect_t *
lh_gl_shadow_fill_get_box(const lh_gl_shadow_fill_t *self);

/** @brief Store @p box and refit the paint bounds. */
lh_void
lh_gl_shadow_fill_set_box(lh_gl_shadow_fill_t *self, lh_gl_rect_t box);

/** @brief Corner radius of the box. `0` is square. */
lh_gl_scalar_t
lh_gl_shadow_fill_get_radius(const lh_gl_shadow_fill_t *self);

/** @brief Store the radius and mark the area. A negative value is stored as zero. */
lh_void
lh_gl_shadow_fill_set_radius(lh_gl_shadow_fill_t *self, lh_gl_scalar_t radius);

/** @brief Borrow @p shadow. @p shadow may be ::lh_null. Refits the paint bounds. */
lh_void
lh_gl_shadow_fill_set_shadow(lh_gl_shadow_fill_t *self, const lh_gl_shadow_t *shadow);

/**
 * @brief Fit the entity rectangle around @p box plus the spread and the shift.
 *
 * An empty or null shadow leaves the entity rectangle equal to the box.
 */
lh_void
lh_gl_shadow_fill_place(lh_gl_shadow_fill_t *self);

/** @brief The class every shadow fill has. Kind entity, not opaque. */
const lh_gl_entity_class_t *
lh_gl_shadow_fill_get_class(void);

/** @brief Entity of @p rect that borrows @p shadow. */
lh_void
lh_gl_shadow_fill_init(lh_gl_shadow_fill_t *self, lh_gl_rect_t rect, const lh_gl_shadow_t *shadow);

/** @brief Draw call of ::lh_gl_shadow_fill_get_class. */
lh_void
lh_gl_shadow_fill_draw(const lh_gl_entity_t *entity, lh_gl_canvas_t *canvas);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_SHADOW_FILL_H */
