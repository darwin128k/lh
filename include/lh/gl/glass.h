/**
 * @file glass.h
 * @brief A frosted panel: ::lh_gl_glass_t.
 *
 * The panel is an entity, so it sits in a layer like a fill. It does not own
 * pixels and it does not allocate. The canvas target has to be the
 * ::lh_gl_pixmap_t the layer is drawing into: the blur reads the colours
 * already there. The scratch is the caller's, and it has to hold
 * ::lh_gl_blur_scratch_count of the panel's rectangle at its blur radius.
 * A shorter buffer draws nothing.
 *
 * The class is not opaque. The rounded corner that the arc misses is left as
 * it was, and a tint can be translucent, so a sibling behind the panel is
 * still drawn.
 */

#ifndef LH_GL_GLASS_H
#define LH_GL_GLASS_H

#include <lh/gl/color.h>
#include <lh/gl/entity.h>
#include <lh/gl/glass/fields.h>

/**
 * @struct lh_gl_glass
 * @brief An entity, a borrowed tint and a borrowed blur scratch.
 *        Fields via ::lh_gl_glass_fields.
 */
struct lh_gl_glass
{
    /** Members: ::lh_gl_glass_fields. */
    lh_gl_glass_fields(lh_gl_entity_t, lh_gl_color_t, lh_gl_scalar_t, lh_usize_t, lh_bool_t);
};
/** @copybrief lh_gl_glass */
typedef struct lh_gl_glass lh_gl_glass_t;

LH_COMPILER_EXTERN_C_BEGIN

/** @brief The entity this panel starts with. */
lh_gl_entity_t *
lh_gl_glass_get_entity_mut(lh_gl_glass_t *self);

/** @brief The entity this panel starts with. */
const lh_gl_entity_t *
lh_gl_glass_get_entity(const lh_gl_glass_t *self);

/** @brief Borrowed tint, or null when the panel only blurs. */
const lh_gl_color_t *
lh_gl_glass_get_tint(const lh_gl_glass_t *self);

/** @brief Replace the borrowed tint and mark the entity's area. */
lh_void
lh_gl_glass_set_tint(lh_gl_glass_t *self, const lh_gl_color_t *tint);

/** @brief Borrowed scratch, or null. The buffer stays writable. */
lh_gl_color_t *
lh_gl_glass_get_scratch(const lh_gl_glass_t *self);

/** @brief How many colours ::lh_gl_glass_get_scratch holds. */
lh_usize_t
lh_gl_glass_get_scratch_count(const lh_gl_glass_t *self);

/**
 * @brief Replace the borrowed scratch. @p count is how many colours it holds.
 *        The panel does not own the buffer. Marks the entity's area.
 */
lh_void
lh_gl_glass_set_scratch(lh_gl_glass_t *self, lh_gl_color_t *scratch, lh_usize_t count);

/** @brief Corner radius. `0` is a square. A negative value is stored as `0`. */
lh_gl_scalar_t
lh_gl_glass_get_radius(const lh_gl_glass_t *self);

/** @brief Store the corner radius and mark the entity's area. */
lh_void
lh_gl_glass_set_radius(lh_gl_glass_t *self, lh_gl_scalar_t radius);

/** @brief How far the blur reaches, in pixels. `0` paints the tint alone. */
lh_gl_scalar_t
lh_gl_glass_get_blur(const lh_gl_glass_t *self);

/** @brief Store the blur radius and mark the entity's area. A negative value is `0`. */
lh_void
lh_gl_glass_set_blur(lh_gl_glass_t *self, lh_gl_scalar_t blur);

/** @brief True when the panel draws and can take a press. */
lh_bool_t
lh_gl_glass_get_shown(const lh_gl_glass_t *self);

/** @brief Store whether the panel draws. */
lh_void
lh_gl_glass_set_shown(lh_gl_glass_t *self, lh_bool_t shown);

/** @brief True when a press can land on the panel. */
lh_bool_t
lh_gl_glass_get_target(const lh_gl_glass_t *self);

/** @brief Store whether a press can land on the panel. */
lh_void
lh_gl_glass_set_target(lh_gl_glass_t *self, lh_bool_t target);

/**
 * @brief The shared class: draw blurs the pixmap behind the rectangle, pick
 *        uses the corner radius. Not opaque.
 */
const lh_gl_entity_class_t *
lh_gl_glass_get_class(void);

/**
 * @brief Build a panel over @p rect. @p tint and @p scratch are borrowed.
 *        Shown, and a press target. Corner and blur start at `0`.
 */
lh_void
lh_gl_glass_init(lh_gl_glass_t *self, lh_gl_rect_t rect, const lh_gl_color_t *tint,
                 lh_gl_color_t *scratch, lh_usize_t scratch_count);

/**
 * @brief Draw call of ::lh_gl_glass_get_class.
 *
 * Blurs the pixmap behind the panel and paints the tint. The canvas target is
 * that pixmap. A hidden panel returns. A blur with no scratch, or with a
 * scratch shorter than ::lh_gl_blur_scratch_count, returns without writing.
 */
lh_void
lh_gl_glass_draw(const lh_gl_entity_t *entity, lh_gl_canvas_t *canvas, const lh_gl_place_t *place);

/**
 * @brief True when this panel takes @p point.
 *
 * False when it is hidden or not a target, and when a rounded corner does not
 * cover @p point even with @p slop.
 */
lh_bool_t
lh_gl_glass_pick(const lh_gl_entity_t *entity, lh_gl_point_t point, lh_gl_scalar_t slop);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_GLASS_H */
