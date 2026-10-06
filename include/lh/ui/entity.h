/**
 * @file entity.h
 * @brief One object in a UI tree: ::lh_ui_entity_t.
 *
 * An entity covers a ::lh_ui_rect_t, may share a ::lh_ui_style_t, and points
 * at a class whose event function is shared by every instance of that kind.
 * Larger widgets are built by linking children (composition), not by copying
 * getters onto every specialized type. ::lh_ui_entity_draw sends
 * ::lh_ui_entity_event_draw to the class, then draws each child. The style and
 * the children are not owned: they must outlive the links.
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
 * @brief A tree node: rectangle, style pointer, class, children, and link.
 */
struct lh_ui_entity
{
    lh_ui_entity_fields(lh_ui_rect_t, lh_ui_style_t, lh_ui_entity_class_t);
};
typedef struct lh_ui_entity lh_ui_entity_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Fill @p self so it covers @p rect with no style and no children.
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
 * @brief Point @p self at @p class. The class is not copied.
 */
lh_void
lh_ui_entity_set_class(lh_ui_entity_t *self, const lh_ui_entity_class_t *);

/**
 * @brief Append @p child to the children of @p self.
 *
 * Neither owns the other. @p child must not already be in a tree.
 */
lh_void
lh_ui_entity_add_child(lh_ui_entity_t *self, lh_ui_entity_t *child);

/**
 * @brief Unlink @p child from the children of @p self.
 *
 * Does not free @p child.
 */
lh_void
lh_ui_entity_remove_child(lh_ui_entity_t *self, lh_ui_entity_t *child);

/**
 * @brief First child of @p self, or ::lh_null when it has none.
 */
lh_ui_entity_t *
lh_ui_entity_get_first_child(const lh_ui_entity_t *self);

/**
 * @brief Child after @p child under @p self, or ::lh_null when @p child is last.
 */
lh_ui_entity_t *
lh_ui_entity_get_next_child(const lh_ui_entity_t *self, const lh_ui_entity_t *child);

/**
 * @brief Event of ::lh_ui_entity_class.
 */
lh_void
lh_ui_entity_face_rect(const struct lh_ui_entity *self, const lh_ui_entity_event_t *event);

/**
 * @brief Send ::lh_ui_entity_event_draw to the class of @p self, then draw
 *        each child in order.
 */
lh_void
lh_ui_entity_draw(const lh_ui_entity_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_ENTITY_H */
