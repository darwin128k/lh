/**
 * @file fill.h
 * @brief A rectangle painted with a borrowed color: ::lh_gl_fill_t.
 *
 * A fill starts with an ::lh_gl_entity_t, so it can be a child of a layer. Beside
 * that entity it stores one borrowed ::lh_gl_color_t. The class is shared: one
 * draw call reads that pointer and fills the entity's rectangle. Two fills of
 * the class can show different colors because the colors live outside the call.
 *
 * The layer stays empty. A background is a fill added first, so it is the back
 * child. Alpha is handed to the canvas as stored. This fill does not blend, and
 * its class is not opaque: a color can change to a lower alpha without the
 * class knowing, so siblings behind a fill are still drawn.
 *
 * Mutating the color's channels does not mark the area. ::lh_gl_fill_set_color
 * does, because the pointer itself changed. The caller invalidates after a
 * channel write.
 */

#ifndef LH_GL_FILL_H
#define LH_GL_FILL_H

#include <lh/gl/color.h>
#include <lh/gl/entity.h>
#include <lh/gl/fill/fields.h>

/**
 * @struct lh_gl_fill
 * @brief An entity plus a borrowed color. Fields via ::lh_gl_fill_fields.
 */
struct lh_gl_fill
{
    /** Members: ::lh_gl_fill_fields. */
    lh_gl_fill_fields(lh_gl_entity_t, lh_gl_color_t);
};
/** @copybrief lh_gl_fill */
typedef struct lh_gl_fill lh_gl_fill_t;

LH_COMPILER_EXTERN_C_BEGIN

/** @brief This fill as an entity. */
lh_gl_entity_t *
lh_gl_fill_get_entity_mut(lh_gl_fill_t *self);

/** @brief This fill as an entity. */
const lh_gl_entity_t *
lh_gl_fill_get_entity(const lh_gl_fill_t *self);

/** @brief Borrowed color, or ::lh_null. */
const lh_gl_color_t *
lh_gl_fill_get_color(const lh_gl_fill_t *self);

/**
 * @brief Borrow @p color. @p color may be ::lh_null.
 *
 * Marks the entity's area. Does not copy @p color.
 */
lh_void
lh_gl_fill_set_color(lh_gl_fill_t *self, const lh_gl_color_t *color);

/**
 * @brief The class every fill has.
 *
 * Kind entity, one draw call, not opaque.
 */
const lh_gl_entity_class_t *
lh_gl_fill_get_class(void);

/**
 * @brief Entity of @p rect that borrows @p color.
 *
 * @p color may be ::lh_null. The fill does not own it.
 */
lh_void
lh_gl_fill_init(lh_gl_fill_t *self, lh_gl_rect_t rect, const lh_gl_color_t *color);

/**
 * @brief Draw call of ::lh_gl_fill_get_class.
 *
 * Fills the entity rectangle with the borrowed color, origin at the canvas
 * origin. A null color returns. A null @p entity is a failed check.
 */
lh_void
lh_gl_fill_draw(const lh_gl_entity_t *entity, lh_gl_canvas_t *canvas);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_FILL_H */
