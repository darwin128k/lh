/**
 * @file card.h
 * @brief One rounded face and one line: ::lh_gl_card_t.
 *
 * A card is a layer. The face and the line are its children and do not take a
 * press, so the press lands on the card. The face is painted with the borrowed
 * color and clipped to @p radius. Hiding the card hides both children and
 * refuses the press. The caller owns the color, the font and the string.
 */

#ifndef LH_GL_CARD_H
#define LH_GL_CARD_H

#include <lh/gl/card/fields.h>
#include <lh/gl/fill.h>
#include <lh/gl/layer.h>
#include <lh/gl/text.h>

/**
 * @struct lh_gl_card
 * @brief A layer, its face and its line. Fields via ::lh_gl_card_fields.
 */
struct lh_gl_card
{
    /** Members: ::lh_gl_card_fields. */
    lh_gl_card_fields(lh_gl_layer_t, lh_gl_fill_t, lh_gl_text_t, lh_gl_scalar_t, lh_bool_t);
};
/** @copybrief lh_gl_card */
typedef struct lh_gl_card lh_gl_card_t;

LH_COMPILER_EXTERN_C_BEGIN

/** @brief The card as an entity. */
lh_gl_entity_t *
lh_gl_card_get_entity_mut(lh_gl_card_t *self);

/** @brief The card as an entity. */
const lh_gl_entity_t *
lh_gl_card_get_entity(const lh_gl_card_t *self);

/** @brief Corner radius. `0` is a rectangle. */
lh_gl_scalar_t
lh_gl_card_get_radius(const lh_gl_card_t *self);

/** @brief Store the radius. A negative value is zero. Marks the area. */
lh_void
lh_gl_card_set_radius(lh_gl_card_t *self, lh_gl_scalar_t radius);

/** @brief True when the card is drawn and takes a press. */
lh_bool_t
lh_gl_card_get_shown(const lh_gl_card_t *self);

/** @brief Store whether the card is drawn and takes a press. */
lh_void
lh_gl_card_set_shown(lh_gl_card_t *self, lh_bool_t shown);

/**
 * @brief A layer of @p rect with a face of @p color and a line of @p text.
 *
 * @p color, @p font and @p text are borrowed. The line uses the whole
 * rectangle. The radius starts at 0.
 */
lh_void
lh_gl_card_init(lh_gl_card_t *self, lh_gl_rect_t rect, const lh_gl_color_t *color,
                const lh_gl_font_t *font, const lh_char_t *text);

/** @brief True when the card takes @p point, including its rounded corner. */
lh_bool_t
lh_gl_card_pick(const lh_gl_entity_t *entity, lh_gl_point_t point, lh_gl_scalar_t slop);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_CARD_H */
