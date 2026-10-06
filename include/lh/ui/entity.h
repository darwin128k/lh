/**
 * @file entity.h
 * @brief One object: ::lh_ui_entity_t, the ::lh_ui_rect_t it covers and a
 *        pointer to its ::lh_ui_style_t.
 *
 * The class pointer names the kind: the event function lives on that class,
 * once. ::lh_ui_entity_draw sends ::lh_ui_entity_event_draw to it. The style
 * is not owned: it must outlive the entity, and several entities may share
 * one.
 */

#ifndef LH_UI_ENTITY_H
#define LH_UI_ENTITY_H

#include <lh/compiler/extern/c.h>
#include <lh/ui/entity/class.h>
#include <lh/ui/entity/event.h>
#include <lh/ui/entity/face/cb.h>
#include <lh/ui/entity/fields.h>
#include <lh/ui/rect.h>
#include <lh/ui/style.h>
#include <lh/void.h>

/**
 * @struct lh_ui_entity
 * @typedef lh_ui_entity_t
 * @brief An object, the rectangle it covers, a style pointer, and the class
 *        it belongs to.
 */
struct lh_ui_entity
{
    lh_ui_entity_fields(lh_ui_rect_t, lh_ui_style_t, lh_ui_entity_class_t);
};
typedef struct lh_ui_entity lh_ui_entity_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Fill @p self so it covers @p rect with no style.
 */
lh_void
lh_ui_entity_init(lh_ui_entity_t *self, lh_ui_rect_t rect);

/**
 * @brief Rectangle @p self covers.
 */
lh_ui_rect_t
lh_ui_entity_get_rect(const lh_ui_entity_t *self);

/**
 * @brief Replace the rectangle @p self covers with @p rect.
 */
lh_void
lh_ui_entity_set_rect(lh_ui_entity_t *self, lh_ui_rect_t rect);

/**
 * @brief Style of @p self, or ::lh_null when it has none.
 */
const lh_ui_style_t *
lh_ui_entity_get_style(const lh_ui_entity_t *self);

/**
 * @brief Point @p self at @p style. The style is not copied.
 *
 * ::lh_null clears the style.
 */
lh_void
lh_ui_entity_set_style(lh_ui_entity_t *self, const lh_ui_style_t *style);

/**
 * @brief Class of @p self.
 */
const lh_ui_entity_class_t *
lh_ui_entity_get_class(const lh_ui_entity_t *self);

/**
 * @brief Point @p self at @p class_p. The class is not copied.
 */
lh_void
lh_ui_entity_set_class(lh_ui_entity_t *self, const lh_ui_entity_class_t *class_p);

/**
 * @brief Event of ::lh_ui_entity_class.
 */
lh_void
lh_ui_entity_face_rect(const struct lh_ui_entity *self, const lh_ui_entity_event_t *event);

/**
 * @brief Send ::lh_ui_entity_event_draw to the class of @p self.
 */
lh_void
lh_ui_entity_draw(const lh_ui_entity_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_ENTITY_H */
