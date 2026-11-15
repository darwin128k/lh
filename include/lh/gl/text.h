/**
 * @file text.h
 * @brief One line of borrowed text: ::lh_gl_text_t.
 *
 * The text starts with an ::lh_gl_entity_t. Beside it are a borrowed
 * ::lh_gl_font_t, a borrowed ::lh_gl_color_t and a borrowed string. The draw
 * call reads those pointers, so one class paints every line. The pen starts at
 * the entity origin. The baseline is the ascent down from that origin. One
 * byte is one code. A code the font does not hold is skipped and the pen does
 * not move. A zero byte ends the line.
 *
 * The class is not opaque. Replacing a pointer marks the area. Writing a
 * channel of the color, or a byte of the string, does not.
 */

#ifndef LH_GL_TEXT_H
#define LH_GL_TEXT_H

#include <lh/gl/color.h>
#include <lh/gl/entity.h>
#include <lh/gl/font.h>
#include <lh/gl/text/fields.h>

/**
 * @struct lh_gl_text
 * @brief An entity plus a borrowed font, color and string. Fields via
 *        ::lh_gl_text_fields.
 */
struct lh_gl_text
{
    /** Members: ::lh_gl_text_fields. */
    lh_gl_text_fields(lh_gl_entity_t, lh_gl_font_t, lh_gl_color_t, lh_char_t, lh_bool_t);
};
/** @copybrief lh_gl_text */
typedef struct lh_gl_text lh_gl_text_t;

LH_COMPILER_EXTERN_C_BEGIN

/** @brief This text as an entity. */
lh_gl_entity_t *
lh_gl_text_get_entity_mut(lh_gl_text_t *self);

/** @brief This text as an entity. */
const lh_gl_entity_t *
lh_gl_text_get_entity(const lh_gl_text_t *self);

/** @brief Borrowed font, or ::lh_null. */
const lh_gl_font_t *
lh_gl_text_get_font(const lh_gl_text_t *self);

/** @brief Borrow @p font. @p font may be ::lh_null. Marks the area. */
lh_void
lh_gl_text_set_font(lh_gl_text_t *self, const lh_gl_font_t *font);

/** @brief Borrowed color, or ::lh_null. */
const lh_gl_color_t *
lh_gl_text_get_color(const lh_gl_text_t *self);

/** @brief Borrow @p color. @p color may be ::lh_null. Marks the area. */
lh_void
lh_gl_text_set_color(lh_gl_text_t *self, const lh_gl_color_t *color);

/** @brief Borrowed string, or ::lh_null. */
const lh_char_t *
lh_gl_text_get_text(const lh_gl_text_t *self);

/** @brief Borrow @p text. @p text may be ::lh_null. Marks the area. */
lh_void
lh_gl_text_set_text(lh_gl_text_t *self, const lh_char_t *text);

/** @brief True when this line is drawn. */
lh_bool_t
lh_gl_text_get_shown(const lh_gl_text_t *self);

/** @brief Store whether this line is drawn. False also refuses a press. */
lh_void
lh_gl_text_set_shown(lh_gl_text_t *self, lh_bool_t shown);

/** @brief True when a press on this line stops at it. */
lh_bool_t
lh_gl_text_get_target(const lh_gl_text_t *self);

/** @brief Store whether a press stops at this line. */
lh_void
lh_gl_text_set_target(lh_gl_text_t *self, lh_bool_t target);

/** @brief The class every text has. Kind entity, not opaque. */
const lh_gl_entity_class_t *
lh_gl_text_get_class(void);

/** @brief Entity of @p rect that borrows @p font, @p color and @p text. */
lh_void
lh_gl_text_init(lh_gl_text_t *self, lh_gl_rect_t rect, const lh_gl_font_t *font,
                const lh_gl_color_t *color, const lh_char_t *text);

/** @brief Draw call of ::lh_gl_text_get_class. */
lh_void
lh_gl_text_draw(const lh_gl_entity_t *entity, lh_gl_canvas_t *canvas);

/** @brief True when this line takes @p point. Hidden or not a target is false. */
lh_bool_t
lh_gl_text_pick(const lh_gl_entity_t *entity, lh_gl_point_t point, lh_gl_scalar_t slop);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_TEXT_H */
