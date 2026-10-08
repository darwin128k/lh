/**
 * @file class.h
 * @brief One kind of entity: ::lh_ui_entity_class_t.
 *
 * The class holds the event function. Every instance of that kind points
 * at the same class. The function is not copied onto the instance.
 */

#ifndef LH_UI_ENTITY_CLASS_H
#define LH_UI_ENTITY_CLASS_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/ui/entity/class/fields.h>
#include <lh/ui/entity/event.h>
#include <lh/ui/entity/face/cb.h>
#include <lh/void.h>

struct lh_ui_entity;
struct lh_ui_canvas;

/**
 * @struct lh_ui_entity_class
 * @typedef lh_ui_entity_class_t
 * @brief The event function of one kind, and the class it extends.
 */
struct lh_ui_entity_class
{
    lh_ui_entity_class_fields(lh_ui_entity_face_cb, struct lh_ui_entity_class);
};
typedef struct lh_ui_entity_class lh_ui_entity_class_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Class of ::lh_ui_entity_t.
 *
 * It has no base class. On ::lh_ui_entity_event_draw it casts the style shadow
 * and then fills the entity rect with ::lh_ui_entity_get_fill_color on the event
 * canvas, when both exist, through ::lh_ui_canvas_fill_round_rect with the style
 * radius. Both come from the style in effect right now
 * (::lh_ui_entity_get_style_now), so a pressed entity paints its pressed style
 * without a class of its own. Other events are ignored. Derived classes call the
 * base through ::lh_ui_entity_class_event_base to keep that.
 */
extern const lh_ui_entity_class_t lh_ui_entity_class;

/**
 * @brief Event function of ::lh_ui_entity_class: on
 *        ::lh_ui_entity_event_draw, ::lh_ui_entity_class_shadow then
 *        ::lh_ui_entity_class_fill; other events are ignored.
 */
lh_void
lh_ui_entity_class_event(const struct lh_ui_entity *self, const lh_ui_entity_event_t *event);

/**
 * @brief True when @p kind is @p base or extends it through `base` links.
 */
lh_bool_t
lh_ui_entity_class_is(const lh_ui_entity_class_t *kind, const lh_ui_entity_class_t *base);

/**
 * @brief Cast the style shadow of @p self under its fill on @p canvas.
 *
 * Nothing when @p canvas is ::lh_null, when @p self has no style, or when the
 * shadow is empty (::lh_ui_shadow_is_empty) — which is every entity that does
 * not ask for one. The order it is called in does not change the picture (a
 * shadow paints nothing inside its own box); it is called first because that is
 * the order the story goes in. Public because a class that draws its own effect,
 * or skips the base fill on purpose, wants the same pixels.
 */
lh_void
lh_ui_entity_class_shadow(const struct lh_ui_entity *self, struct lh_ui_canvas *canvas);

/**
 * @brief Fill the rect of @p self with its style fill color and radius on
 *        @p canvas. Nothing when @p canvas is ::lh_null or there is no solid
 *        fill.
 */
lh_void
lh_ui_entity_class_fill(const struct lh_ui_entity *self, struct lh_ui_canvas *canvas);

/**
 * @brief Call the event function of the class @p class extends.
 *
 * Returns without calling when that class has no base.
 */
lh_void
lh_ui_entity_class_event_base(const lh_ui_entity_class_t *, const struct lh_ui_entity *self,
                              const lh_ui_entity_event_t *event);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_ENTITY_CLASS_H */
