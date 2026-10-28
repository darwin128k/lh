/**
 * @file run.h
 * @brief ::lh_ui_layout_run_t — state of one line of the flow while it is measured, justified and placed (::lh_ui_layout_run).
 */

#ifndef LH_UI_LAYOUT_RUN_H
#define LH_UI_LAYOUT_RUN_H

#include <lh/ui/entity.h>
#include <lh/ui/layout/fields.h>
#include <lh/ui/justify.h>
#include <lh/ui/layout/cross.h>
#include <lh/compiler/extern/c.h>

/**
 * @struct lh_ui_layout_run
 * @brief State of one line of the flow while it is measured, justified and
 *        placed (::lh_ui_layout_run).
 */
struct lh_ui_layout_run
{
    /** Length the shown children take along, margins and gaps included. */
    lh_ui_scalar_t used;
    /** Sum of the fill weights. */
    lh_ui_scalar_t weights;
    /** Room left along, never below zero. */
    lh_ui_scalar_t left;
    /** The row's baseline down from the content's cross origin, or -1. */
    lh_ui_scalar_t line;
    /** Where the next child starts along (absolute). */
    lh_ui_scalar_t along;
    /** Extra room after each shown child (`space-*` justify). */
    lh_ui_scalar_t spread;
    /** How many children take room. */
    int shown;
};

/** @brief Alias for ::lh_ui_layout_run. */
typedef struct lh_ui_layout_run lh_ui_layout_run_t;

struct lh_ui_layout;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Start a line in @p content: `along` at its origin, `line` -1, the
 *        rest zero. @p out must not be null (::lh_assert_runtime_ref).
 */
lh_void
lh_ui_layout_run_init(const struct lh_ui_layout *self, const lh_ui_rect_t *content,
                      lh_ui_layout_run_t *out);

/**
 * @brief Count one child into @p run: its wanted length and margin, its fill
 *        weight, and its baseline. A hidden child counts for nothing.
 */
lh_void
lh_ui_layout_run_measure_child(const struct lh_ui_layout *self, lh_ui_layout_run_t *run,
                               lh_ui_entity_t *child, const lh_ui_rect_t *content);

/**
 * @brief Measure the children @p first up to @p end (::lh_null for the last)
 *        into @p run, gaps between shown children included, and set `left`.
 */
lh_void
lh_ui_layout_run_measure(const struct lh_ui_layout *self, lh_ui_layout_run_t *run,
                         lh_ui_entity_t *parent, lh_ui_entity_t *first, lh_ui_entity_t *end,
                         const lh_ui_rect_t *content);

/**
 * @brief Apply the justify of @p self to a measured @p run: moves `along` by
 *        the lead and sets `spread`. Nothing when a child fills or none shows.
 */
lh_void
lh_ui_layout_run_justify(const struct lh_ui_layout *self, lh_ui_layout_run_t *run);

/**
 * @brief Length of @p child along the flow: its share of `left` by weight when
 *        it fills (held by its bounds), what it wants otherwise.
 */
lh_ui_scalar_t
lh_ui_layout_run_length(const struct lh_ui_layout *self, const lh_ui_layout_run_t *run,
                        lh_ui_entity_t *child);

/**
 * @brief Place one child at `along` in @p content and advance `along` past it.
 *        A hidden child is left where it is.
 */
lh_void
lh_ui_layout_run_place_child(const struct lh_ui_layout *self, lh_ui_layout_run_t *run,
                             lh_ui_entity_t *child, const lh_ui_rect_t *content);

/**
 * @brief Place the children @p first up to @p end (::lh_null for the last) by
 *        a measured and justified @p run.
 */
lh_void
lh_ui_layout_run_place(const struct lh_ui_layout *self, lh_ui_layout_run_t *run, lh_ui_entity_t *parent,
                       lh_ui_entity_t *first, lh_ui_entity_t *end, const lh_ui_rect_t *content);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_LAYOUT_RUN_H */
