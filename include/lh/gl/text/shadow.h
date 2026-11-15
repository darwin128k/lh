/**
 * @file shadow.h
 * @brief A soft shadow of a line's glyphs: ::lh_gl_text_shadow_t.
 *
 * The shadow is its own entity. It borrows a ::lh_gl_text_t and a
 * ::lh_gl_shadow_t. The glyphs are painted in the shadow color, shifted by the
 * shadow's offset, then blurred by the spread. The blur follows the ink, so a
 * letter keeps its shape. A spread of `0` is that shifted ink with a hard edge.
 *
 * The scratch is the caller's. ::lh_gl_text_shadow_scratch_count says how many
 * colours it needs for the shadow's rectangle: the ink pixmap, then the blur
 * scratch. A shorter buffer draws nothing. Nothing here allocates.
 *
 * ::lh_gl_text_shadow_place fits the rectangle around the line plus the spread
 * and the offset. Move the line, or change the shadow's spread or offset, and
 * place again: writing those fields does not refit by itself. The class is not
 * opaque. A press always passes through.
 */

#ifndef LH_GL_TEXT_SHADOW_H
#define LH_GL_TEXT_SHADOW_H

#include <lh/gl/shadow.h>
#include <lh/gl/text.h>
#include <lh/gl/text/shadow/fields.h>

/**
 * @struct lh_gl_text_shadow
 * @brief An entity, a borrowed line and a borrowed shadow.
 *        Fields via ::lh_gl_text_shadow_fields.
 */
struct lh_gl_text_shadow
{
    /** Members: ::lh_gl_text_shadow_fields. */
    lh_gl_text_shadow_fields(lh_gl_entity_t, lh_gl_text_t, lh_gl_shadow_t, lh_gl_color_t,
                             lh_usize_t, lh_bool_t);
};
/** @copybrief lh_gl_text_shadow */
typedef struct lh_gl_text_shadow lh_gl_text_shadow_t;

LH_COMPILER_EXTERN_C_BEGIN

/** @brief This shadow as an entity. */
lh_gl_entity_t *
lh_gl_text_shadow_get_entity_mut(lh_gl_text_shadow_t *self);

/** @brief This shadow as an entity. */
const lh_gl_entity_t *
lh_gl_text_shadow_get_entity(const lh_gl_text_shadow_t *self);

/** @brief Borrowed line, or ::lh_null. */
const lh_gl_text_t *
lh_gl_text_shadow_get_line(const lh_gl_text_shadow_t *self);

/** @brief Borrow @p line and refit the rectangle. @p line may be ::lh_null. */
lh_void
lh_gl_text_shadow_set_line(lh_gl_text_shadow_t *self, const lh_gl_text_t *line);

/** @brief Borrowed shadow, or ::lh_null. */
const lh_gl_shadow_t *
lh_gl_text_shadow_get_shadow(const lh_gl_text_shadow_t *self);

/** @brief Borrow @p shadow and refit the rectangle. @p shadow may be ::lh_null. */
lh_void
lh_gl_text_shadow_set_shadow(lh_gl_text_shadow_t *self, const lh_gl_shadow_t *shadow);

/** @brief Borrowed scratch, or ::lh_null. The buffer stays writable. */
lh_gl_color_t *
lh_gl_text_shadow_get_scratch(const lh_gl_text_shadow_t *self);

/** @brief How many colours ::lh_gl_text_shadow_get_scratch holds. */
lh_usize_t
lh_gl_text_shadow_get_scratch_count(const lh_gl_text_shadow_t *self);

/** @brief Replace the borrowed scratch. The shadow does not own the buffer. */
lh_void
lh_gl_text_shadow_set_scratch(lh_gl_text_shadow_t *self, lh_gl_color_t *scratch, lh_usize_t count);

/** @brief True when the shadow draws. */
lh_bool_t
lh_gl_text_shadow_get_shown(const lh_gl_text_shadow_t *self);

/** @brief Store whether the shadow draws. */
lh_void
lh_gl_text_shadow_set_shown(lh_gl_text_shadow_t *self, lh_bool_t shown);

/**
 * @brief Colours of scratch for a shadow whose rectangle is @p rect.
 *
 * An empty rectangle returns `0`. A spread above `0` adds the blur scratch
 * after the ink pixmap. A spread of `0` or below is the pixmap alone.
 */
lh_usize_t
lh_gl_text_shadow_scratch_count(const lh_gl_rect_t *rect, lh_gl_scalar_t spread);

/**
 * @brief Fit the rectangle around the line, the spread and the offset.
 *
 * A null line clears the rectangle. A null shadow uses no spread and no shift.
 */
lh_void
lh_gl_text_shadow_place(lh_gl_text_shadow_t *self);

/** @brief The class every text shadow has. Kind entity, not opaque. */
const lh_gl_entity_class_t *
lh_gl_text_shadow_get_class(void);

/**
 * @brief A shadow of @p line. @p shadow and @p scratch are borrowed.
 *        Shown. The rectangle is fitted.
 */
lh_void
lh_gl_text_shadow_init(lh_gl_text_shadow_t *self, const lh_gl_text_t *line,
                       const lh_gl_shadow_t *shadow, lh_gl_color_t *scratch,
                       lh_usize_t scratch_count);

/** @brief Draw call of ::lh_gl_text_shadow_get_class. */
lh_void
lh_gl_text_shadow_draw(const lh_gl_entity_t *entity, lh_gl_canvas_t *canvas);

/** @brief A text shadow never takes a press. The search continues behind it. */
lh_bool_t
lh_gl_text_shadow_pick(const lh_gl_entity_t *entity, lh_gl_point_t point, lh_gl_scalar_t slop);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_TEXT_SHADOW_H */
