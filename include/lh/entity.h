/**
 * @file entity.h
 * @brief One drawn object: ::lh_entity_t, the ::lh_math_rect_t it covers.
 *
 * The rectangle is the one ::lh_ui_canvas_fill_rect and
 * ::lh_ui_canvas_stroke_rect already take. The class pointer names the
 * kind: the event function lives on that class, once. ::lh_entity_draw
 * sends ::lh_entity_event_draw to it. Brush and pen arrive with the call.
 * The canvas does not keep this entity.
 */

#ifndef LH_ENTITY_H
#define LH_ENTITY_H

#include <lh/compiler/extern/c.h>
#include <lh/entity/class.h>
#include <lh/entity/event.h>
#include <lh/entity/face/cb.h>
#include <lh/entity/fields.h>
#include <lh/math/rect.h>
#include <lh/ui/brush.h>
#include <lh/ui/canvas.h>
#include <lh/ui/pen.h>
#include <lh/void.h>

/**
 * @struct lh_entity
 * @typedef lh_entity_t
 * @brief An object, the rectangle it covers, and the class it belongs to.
 */
struct lh_entity
{
    lh_entity_fields(lh_math_rect_t, lh_entity_class_t);
};
typedef struct lh_entity lh_entity_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Fill @p self so it covers @p rect.
 */
lh_void
lh_entity_init(lh_entity_t *self, lh_math_rect_t rect);

/**
 * @brief Rectangle @p self covers.
 */
lh_math_rect_t
lh_entity_get_rect(const lh_entity_t *self);

/**
 * @brief Replace the rectangle @p self covers with @p rect.
 */
lh_void
lh_entity_set_rect(lh_entity_t *self, lh_math_rect_t rect);

/**
 * @brief Class of @p self.
 */
const lh_entity_class_t *
lh_entity_get_class(const lh_entity_t *self);

/**
 * @brief Point @p self at @p class_p. The class is not copied.
 */
lh_void
lh_entity_set_class(lh_entity_t *self, const lh_entity_class_t *class_p);

/**
 * @brief Event of ::lh_entity_class. Fills the rectangle with the event
 *        brush and outlines it with the event pen.
 */
lh_void
lh_entity_face_rect(const struct lh_entity *self, const lh_entity_event_t *event);

/**
 * @brief Send ::lh_entity_event_draw to the class of @p self.
 *
 * The class event paints. This call does not.
 */
lh_void
lh_entity_draw(const lh_entity_t *self, lh_ui_canvas_t *canvas, const lh_ui_brush_t *brush,
               const lh_ui_pen_t *pen);

LH_COMPILER_EXTERN_C_END

#endif /* LH_ENTITY_H */
