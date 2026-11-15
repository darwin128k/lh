/**
 * @file input.h
 * @brief One held entity: ::lh_gl_input_t.
 *
 * The input manager answers which entity a press is on. It stores that one
 * borrowed pointer: no scroll, no layout, no packets. A press asks
 * ::lh_gl_layer_find_entity and remembers the result, including a miss. A move
 * does not search again, so the hold stays when the point leaves the entity.
 * A release forgets it. ::lh_gl_input_apply is the same hold for one pointer
 * event: a second pointer does not take it, and a key does not either.
 *
 * The caller owns the entity and keeps it alive while it is stored here.
 */

#ifndef LH_GL_INPUT_H
#define LH_GL_INPUT_H

#include <lh/gl/entity/decl.h>
#include <lh/gl/input/fields.h>
#include <lh/gl/layer/decl.h>
#include <lh/gl/point.h>

/**
 * @struct lh_gl_input
 * @brief The entity captured by the current press. Fields via ::lh_gl_input_fields.
 */
struct lh_gl_input
{
    /** Members: ::lh_gl_input_fields. */
    lh_gl_input_fields(lh_gl_entity_t, lh_bool_t, lh_gl_scalar_t);
};
/** @copybrief lh_gl_input */
typedef struct lh_gl_input lh_gl_input_t;

LH_COMPILER_EXTERN_C_BEGIN

/** @brief Held entity, or ::lh_null when nothing is pressed. */
const lh_gl_entity_t *
lh_gl_input_get_captured(const lh_gl_input_t *self);

/**
 * @brief Store a borrowed entity. @p captured may be ::lh_null.
 *
 * Does not unlink, invalidate or otherwise touch the entity.
 */
lh_void
lh_gl_input_set_captured(lh_gl_input_t *self, const lh_gl_entity_t *captured);

/** @brief The pointer ::lh_gl_input_apply is following. Meaningful while tracking. */
lh_u8_t
lh_gl_input_get_pointer(const lh_gl_input_t *self);

/** @brief Store the pointer id. Does not search and does not change the hold. */
lh_void
lh_gl_input_set_pointer(lh_gl_input_t *self, lh_u8_t pointer);

/**
 * @brief True when the hold came from ::lh_gl_input_apply.
 *
 * A point press clears it. Release clears it.
 */
lh_bool_t
lh_gl_input_get_tracking(const lh_gl_input_t *self);

/** @brief Store whether a pointer event owns the hold. */
lh_void
lh_gl_input_set_tracking(lh_gl_input_t *self, lh_bool_t tracking);

/** @brief Finger thickness in pixels. Below 0 is stored as 0. */
lh_gl_scalar_t
lh_gl_input_get_slop(const lh_gl_input_t *self);

/** @brief Store the finger thickness. A press within this many pixels still hits. */
lh_void
lh_gl_input_set_slop(lh_gl_input_t *self, lh_gl_scalar_t slop);

/** @brief Hold nothing. */
lh_void
lh_gl_input_init(lh_gl_input_t *self);

/**
 * @brief Remember the entity under @p point and return it.
 *
 * @p point is in root coordinates. A miss stores ::lh_null, which also drops
 * whatever was held. A second press replaces the hold. Tracking is cleared:
 * this hold is not owned by a pointer event.
 *
 * @param self  Input state; must not be null.
 * @param layer Layer to search; must not be null.
 * @param point Point in root coordinates, copied.
 * @return The entity ::lh_gl_layer_find_entity returned. Borrowed.
 */
const lh_gl_entity_t *
lh_gl_input_press(lh_gl_input_t *self, const lh_gl_layer_t *layer, lh_gl_point_t point);

/**
 * @brief The entity this press is still holding.
 *
 * Does not look at a point and does not search the layer. The hold is unchanged
 * when the caller's point has left the entity.
 *
 * @param self Input state; must not be null.
 * @return The held entity, or ::lh_null when nothing is pressed.
 */
const lh_gl_entity_t *
lh_gl_input_move(const lh_gl_input_t *self);

/**
 * @brief Forget the held entity.
 *
 * @param self Input state; must not be null.
 * @return The entity that was held, or ::lh_null when nothing was.
 */
const lh_gl_entity_t *
lh_gl_input_release(lh_gl_input_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_INPUT_H */
