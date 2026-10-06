/**
 * @file label.h
 * @brief An entity that shows text: ::lh_ui_entity_label_t.
 *
 * The label embeds an ::lh_ui_entity_t and adds a string. Use the entity API
 * on ::lh_ui_entity_label_as_entity for rect, style, class, children, and draw.
 * The string is not copied and not owned. Its class is
 * ::lh_ui_entity_label_class, which calls the base class. The characters are
 * not painted.
 */

#ifndef LH_UI_ENTITY_LABEL_H
#define LH_UI_ENTITY_LABEL_H

#include <lh/char.h>
#include <lh/compiler/extern/c.h>
#include <lh/ui/entity.h>
#include <lh/ui/entity/label/fields.h>
#include <lh/void.h>

/**
 * @struct lh_ui_entity_label
 * @typedef lh_ui_entity_label_t
 * @brief An entity and the text it shows.
 */
struct lh_ui_entity_label
{
    lh_ui_entity_label_fields(lh_ui_entity_t, lh_char_t);
};
typedef struct lh_ui_entity_label lh_ui_entity_label_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Class of ::lh_ui_entity_label_t, derived from ::lh_ui_entity_class.
 */
extern const lh_ui_entity_class_t lh_ui_entity_label_class;

/**
 * @brief Fill @p self so it covers @p rect and shows @p text.
 *
 * @p text is not copied. ::lh_null shows nothing. Style starts as ::lh_null.
 */
lh_void
lh_ui_entity_label_init(lh_ui_entity_label_t *self, lh_ui_rect_t rect, const lh_char_t *text);

/**
 * @brief The entity @p self embeds.
 */
lh_ui_entity_t *
lh_ui_entity_label_as_entity(lh_ui_entity_label_t *self);

/**
 * @brief The string @p self shows, or ::lh_null when it has none.
 */
const lh_char_t *
lh_ui_entity_label_get_text(const lh_ui_entity_label_t *self);

/**
 * @brief Point @p self at @p text. @p text is not copied.
 *
 * ::lh_null clears the text.
 */
lh_void
lh_ui_entity_label_set_text(lh_ui_entity_label_t *self, const lh_char_t *text);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_ENTITY_LABEL_H */
