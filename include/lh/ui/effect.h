/**
 * @file effect.h
 * @brief An optional paint pass attached to a box.
 *
 * An effect is a small record plus a function that paints it. Shadow, blur
 * and glass are effects. The record is not owned by whoever points at it,
 * and a null pointer paints nothing, so a device that never attaches one
 * does not run it. Leaving the effect's source out of a build leaves it out
 * of the program.
 *
 * Several effects chain with ::lh_ui_effect_set_next. The first is drawn
 * first. docs/scene.md shows the shipped effects and how to write one.
 */

#ifndef LH_UI_EFFECT_H
#define LH_UI_EFFECT_H

#include <lh/compiler/extern/c.h>
#include <lh/numeric/types.h>
#include <lh/ui/canvas.h>
#include <lh/ui/effect/class/fields.h>
#include <lh/ui/effect/fields.h>

struct lh_ui_effect;

/**
 * @brief Paints one effect into @p canvas over @p box.
 *
 * @p corner is the box corner radius in pixels. Zero is square.
 */
typedef lh_void (*lh_ui_effect_draw_fn)(const struct lh_ui_effect *self, lh_ui_canvas_t *canvas,
                                        lh_math_rect_t box, lh_int_t corner);

/**
 * @brief How many pixels @p self paints past its box.
 */
typedef lh_int_t (*lh_ui_effect_outset_fn)(const struct lh_ui_effect *self);

/**
 * @struct lh_ui_effect_class
 * @brief Fields via ::lh_ui_effect_class_fields.
 */
struct lh_ui_effect_class
{
    lh_ui_effect_class_fields(lh_ui_effect_draw_fn, lh_ui_effect_outset_fn);
};
typedef struct lh_ui_effect_class lh_ui_effect_class_t;

/**
 * @struct lh_ui_effect
 * @brief Fields via ::lh_ui_effect_fields.
 */
struct lh_ui_effect
{
    lh_ui_effect_fields(const lh_ui_effect_class_t *, const struct lh_ui_effect *);
};
typedef struct lh_ui_effect lh_ui_effect_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Point @p self at @p effect_class and clear its chain.
 *
 * The field and the parameter are spelled @c class_ptr and @c effect_class
 * because a member called `class` is legal C and not legal C++, and this
 * header is included from the C++ test suite.
 */
lh_void
lh_ui_effect_init(lh_ui_effect_t *self, const lh_ui_effect_class_t *effect_class);

/**
 * @brief The effect drawn after @p self, or ::lh_null.
 */
const lh_ui_effect_t *
lh_ui_effect_get_next(const lh_ui_effect_t *self);

/**
 * @brief Draw @p next after @p self. @p next is not owned. Null ends the chain.
 *
 * Does not redraw by itself.
 */
lh_void
lh_ui_effect_set_next(lh_ui_effect_t *self, const lh_ui_effect_t *next);

/**
 * @brief Paint @p self and every effect after it. Null paints nothing.
 */
lh_void
lh_ui_effect_draw(const lh_ui_effect_t *self, lh_ui_canvas_t *canvas, lh_math_rect_t box,
                  lh_int_t corner);

/**
 * @brief The farthest any effect in the chain of @p self paints past the box.
 *        Zero when @p self is null.
 */
lh_int_t
lh_ui_effect_outset(const lh_ui_effect_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_EFFECT_H */
