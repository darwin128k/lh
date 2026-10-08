/**
 * @file label.h
 * @brief An entity that shows text: ::lh_ui_label_t.
 *
 * The label embeds an ::lh_ui_container_t and adds a string. Use the
 * entity API on ::lh_ui_label_as_entity for rect, style, class,
 * children, and draw. The string is not copied and not owned. Its class is
 * ::lh_ui_label_class, which extends ::lh_ui_container_class.
 *
 * The text is drawn on the draw event with the style font and text color
 * (`lh/ui/text.h`), from the top-left of the rect, inside the same canvas
 * push as the children: the container scroll moves it and its rect cuts it.
 * On the measure event the text rect joins the content bounds, so long text
 * makes the container scroll.
 */

#ifndef LH_UI_LABEL_H
#define LH_UI_LABEL_H

#include <lh/bool.h>
#include <lh/char.h>
#include <lh/compiler/extern/c.h>
#include <lh/ui/canvas.h>
#include <lh/ui/color.h>
#include <lh/ui/container.h>
#include <lh/ui/label/fields.h>
#include <lh/ui/font.h>
#include <lh/ui/point.h>
#include <lh/ui/rect.h>
#include <lh/void.h>

/**
 * @struct lh_ui_entity_label
 * @typedef lh_ui_label_t
 * @brief A container and the text it shows.
 */
struct lh_ui_entity_label
{
    lh_ui_label_fields(lh_ui_container_t, lh_char_t);
};
typedef struct lh_ui_entity_label lh_ui_label_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Class of ::lh_ui_label_t, derived from
 *        ::lh_ui_container_class.
 */
extern const lh_ui_entity_class_t lh_ui_label_class;

/**
 * @brief Event function of ::lh_ui_label_class: the container class
 *        first (fill and scrolling kept), then the draw and measure handlers
 *        below. A derived class may call it directly.
 */
lh_void
lh_ui_label_event(const struct lh_ui_entity *self, const lh_ui_entity_event_t *event);

/**
 * @brief On ::lh_ui_entity_event_draw: ::lh_ui_label_draw_text.
 *        Other events are ignored.
 */
lh_void
lh_ui_label_on_draw(const lh_ui_label_t *self, const lh_ui_entity_event_t *event);

/**
 * @brief On ::lh_ui_entity_event_measure: grow the bounds by
 *        ::lh_ui_label_get_text_rect. Other events are ignored.
 */
lh_void
lh_ui_label_on_measure(const lh_ui_label_t *self, const lh_ui_entity_event_t *event);

/**
 * @brief Fill @p self so it covers @p rect and shows @p text.
 *
 * @p text is not copied. ::lh_null shows nothing. Style starts as ::lh_null,
 * so nothing is drawn until a style with a font and a text color is set.
 */
lh_void
lh_ui_label_init(lh_ui_label_t *self, lh_ui_rect_t rect, const lh_char_t *text);

/**
 * @brief The entity @p self embeds (through its container).
 */
lh_ui_entity_t *
lh_ui_label_as_entity(lh_ui_label_t *self);

/**
 * @brief The container @p self embeds.
 */
lh_ui_container_t *
lh_ui_label_as_container(lh_ui_label_t *self);

/**
 * @brief The string @p self shows, or ::lh_null when it has none.
 */
const lh_char_t *
lh_ui_label_get_text(const lh_ui_label_t *self);

/**
 * @brief Point @p self at @p text. @p text is not copied.
 *
 * ::lh_null clears the text.
 */
lh_void
lh_ui_label_set_text(lh_ui_label_t *self, const lh_char_t *text);

/**
 * @brief Font of the style of @p self, or ::lh_null (no style or no font).
 */
const lh_ui_font_t *
lh_ui_label_get_font(const lh_ui_label_t *self);

/**
 * @brief Solid text color of the style of @p self, or ::lh_null.
 */
const lh_ui_color_t *
lh_ui_label_get_text_color(const lh_ui_label_t *self);

/**
 * @brief Where the first line starts: the top-left of the rect of @p self,
 *        moved in by the left and top style padding (::lh_ui_entity_get_padding).
 */
lh_ui_point_t
lh_ui_label_get_text_origin(const lh_ui_label_t *self);

/**
 * @brief The rect the text covers (unscrolled); empty without text or font.
 */
lh_ui_rect_t
lh_ui_label_get_text_rect(const lh_ui_label_t *self);

/**
 * @brief True when there is a canvas, text, a font and a text color.
 */
lh_bool_t
lh_ui_label_can_draw_text(const lh_ui_label_t *self, const lh_ui_canvas_t *canvas);

/**
 * @brief Draw the text of @p self inside ::lh_ui_entity_push_children (scroll
 *        offset and rect clip), then pop.
 */
lh_void
lh_ui_label_draw_text(const lh_ui_label_t *self, lh_ui_canvas_t *canvas);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_LABEL_H */
