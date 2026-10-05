/**
 * @file label.h
 * @brief An entity that shows text: ::lh_entity_label_t.
 *
 * The label is the entity plus a string. The string is not copied and not
 * owned: it must outlive the label. Its class is ::lh_entity_label_class.
 * That class calls the base class, which paints the rectangle, and then
 * would paint the text. The canvas has no glyphs, so the characters are
 * not painted.
 */

#ifndef LH_ENTITY_LABEL_H
#define LH_ENTITY_LABEL_H

#include <lh/char.h>
#include <lh/compiler/extern/c.h>
#include <lh/entity.h>
#include <lh/entity/label/fields.h>
#include <lh/void.h>

/**
 * @struct lh_entity_label
 * @typedef lh_entity_label_t
 * @brief An entity and the text it shows.
 */
struct lh_entity_label
{
    lh_entity_label_fields(lh_entity_t, lh_char_t);
};
typedef struct lh_entity_label lh_entity_label_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Class of ::lh_entity_label_t, derived from ::lh_entity_class.
 */
extern const lh_entity_class_t lh_entity_label_class;

/**
 * @brief Fill @p self so it covers @p rect and shows @p text.
 *
 * @p text is not copied. ::lh_null shows nothing.
 */
lh_void
lh_entity_label_init(lh_entity_label_t *self, lh_math_rect_t rect, const lh_char_t *text);

/**
 * @brief Rectangle @p self covers.
 */
lh_math_rect_t
lh_entity_label_get_rect(const lh_entity_label_t *self);

/**
 * @brief Replace the rectangle @p self covers with @p rect.
 */
lh_void
lh_entity_label_set_rect(lh_entity_label_t *self, lh_math_rect_t rect);

/**
 * @brief The string @p self shows, or ::lh_null when it has none.
 */
const lh_char_t *
lh_entity_label_get_text(const lh_entity_label_t *self);

/**
 * @brief Point @p self at @p text. @p text is not copied.
 *
 * ::lh_null clears the text.
 */
lh_void
lh_entity_label_set_text(lh_entity_label_t *self, const lh_char_t *text);

/**
 * @brief Send a draw event to the class of @p self.
 *
 * The text is not painted.
 */
lh_void
lh_entity_label_draw(const lh_entity_label_t *self, lh_ui_canvas_t *canvas, const lh_ui_brush_t *brush,
                     const lh_ui_pen_t *pen);

LH_COMPILER_EXTERN_C_END

#endif /* LH_ENTITY_LABEL_H */
