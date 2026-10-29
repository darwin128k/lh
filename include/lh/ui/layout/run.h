/**
 * @file run.h
 * @brief ::lh_ui_layout_run_t — state of one line of the flow while it is measured, justified and placed (::lh_ui_layout_run).
 */

#ifndef LH_UI_LAYOUT_RUN_H
#define LH_UI_LAYOUT_RUN_H

#include <lh/ui/layout/cross.h>

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

/**
 * @brief The extent the children of the run take (level 1).
 *
 * @param self The run; checked by ::lh_assert_runtime_ref.
 */
lh_ui_scalar_t
lh_ui_layout_run_get_used(const lh_ui_layout_run_t *self);

/**
 * @brief Store the extent the children of the run take (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The run; checked by ::lh_assert_runtime_ref.
 * @param used The new value.
 */
lh_void
lh_ui_layout_run_set_used(lh_ui_layout_run_t *self, lh_ui_scalar_t used);

/**
 * @brief The sum of the weights of the children that grow (level 1).
 *
 * @param self The run; checked by ::lh_assert_runtime_ref.
 */
lh_ui_scalar_t
lh_ui_layout_run_get_weights(const lh_ui_layout_run_t *self);

/**
 * @brief Store the sum of the weights of the children that grow (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The run; checked by ::lh_assert_runtime_ref.
 * @param weights The new value.
 */
lh_void
lh_ui_layout_run_set_weights(lh_ui_layout_run_t *self, lh_ui_scalar_t weights);

/**
 * @brief The extent left for the children that grow (level 1).
 *
 * @param self The run; checked by ::lh_assert_runtime_ref.
 */
lh_ui_scalar_t
lh_ui_layout_run_get_left(const lh_ui_layout_run_t *self);

/**
 * @brief Store the extent left for the children that grow (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The run; checked by ::lh_assert_runtime_ref.
 * @param left The new value.
 */
lh_void
lh_ui_layout_run_set_left(lh_ui_layout_run_t *self, lh_ui_scalar_t left);

/**
 * @brief The extent of the line on the cross axis (level 1).
 *
 * @param self The run; checked by ::lh_assert_runtime_ref.
 */
lh_ui_scalar_t
lh_ui_layout_run_get_line(const lh_ui_layout_run_t *self);

/**
 * @brief Store the extent of the line on the cross axis (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The run; checked by ::lh_assert_runtime_ref.
 * @param line The new value.
 */
lh_void
lh_ui_layout_run_set_line(lh_ui_layout_run_t *self, lh_ui_scalar_t line);

/**
 * @brief Where the next child goes along the axis (level 1).
 *
 * @param self The run; checked by ::lh_assert_runtime_ref.
 */
lh_ui_scalar_t
lh_ui_layout_run_get_along(const lh_ui_layout_run_t *self);

/**
 * @brief Store where the next child goes along the axis (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The run; checked by ::lh_assert_runtime_ref.
 * @param along The new value.
 */
lh_void
lh_ui_layout_run_set_along(lh_ui_layout_run_t *self, lh_ui_scalar_t along);

/**
 * @brief The space added between children (level 1).
 *
 * @param self The run; checked by ::lh_assert_runtime_ref.
 */
lh_ui_scalar_t
lh_ui_layout_run_get_spread(const lh_ui_layout_run_t *self);

/**
 * @brief Store the space added between children (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The run; checked by ::lh_assert_runtime_ref.
 * @param spread The new value.
 */
lh_void
lh_ui_layout_run_set_spread(lh_ui_layout_run_t *self, lh_ui_scalar_t spread);

/**
 * @brief How many children of the run are shown (level 1).
 *
 * @param self The run; checked by ::lh_assert_runtime_ref.
 */
int
lh_ui_layout_run_get_shown(const lh_ui_layout_run_t *self);

/**
 * @brief Store how many children of the run are shown (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The run; checked by ::lh_assert_runtime_ref.
 * @param shown The new value.
 */
lh_void
lh_ui_layout_run_set_shown(lh_ui_layout_run_t *self, int shown);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_LAYOUT_RUN_H */
