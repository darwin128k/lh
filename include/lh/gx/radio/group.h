/**
 * @file group.h
 * @brief A group of radios of which one is chosen: ::lh_gx_radio_group_t.
 *
 * The radios (`lh/gx/radio.h`) of a group are entities anywhere on the screen;
 * the group is not an entity and draws nothing. It holds which radio is
 * chosen and the application's reaction. Choosing a radio shows its mark and
 * hides the mark of the one chosen before (::lh_gx_checkbox_check on each), so
 * only those two squares are redrawn.
 *
 * Two ways to choose: ::lh_gx_radio_group_select is the program's — no
 * reaction; ::lh_gx_radio_group_choose is the user's — a tap on a radio — and
 * calls the reaction when the choice changed.
 *
 * Ownership: the group is the application's; it points at the chosen radio
 * and owns nothing. Each radio points at its group, so the group must outlive
 * its radios.
 */

#ifndef LH_GX_RADIO_GROUP_H
#define LH_GX_RADIO_GROUP_H

#include <lh/core/bool.h>
#include <lh/gx/radio/group/change/fn.h>
#include <lh/gx/radio/group/fields.h>

/**
 * @struct lh_gx_radio_group
 * @brief The choice and the reaction. Members: ::lh_gx_radio_group_fields.
 */
struct lh_gx_radio_group
{
    /** Members: ::lh_gx_radio_group_fields. */
    lh_gx_radio_group_fields(struct lh_gx_radio, lh_gx_radio_group_change_fn);
};
/** @copybrief lh_gx_radio_group */
typedef struct lh_gx_radio_group lh_gx_radio_group_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The radio chosen (level 1).
 *
 * @param self The group; checked by ::lh_assert_runtime_ref.
 * @return The radio, or ::lh_null for none.
 */
struct lh_gx_radio *
lh_gx_radio_group_get_selected(const lh_gx_radio_group_t *self);

/**
 * @brief Store the radio chosen (level 1); no mark is shown or hidden — that
 *        is ::lh_gx_radio_group_select.
 *
 * @param self  The group; checked by ::lh_assert_runtime_ref.
 * @param radio The radio, or ::lh_null for none; not owned.
 */
lh_void
lh_gx_radio_group_set_selected(lh_gx_radio_group_t *self, struct lh_gx_radio *radio);

/**
 * @brief The application's reaction (level 1).
 *
 * @param self The group; checked by ::lh_assert_runtime_ref.
 * @return The reaction, or ::lh_null.
 */
lh_gx_radio_group_change_fn *
lh_gx_radio_group_get_on_change(const lh_gx_radio_group_t *self);

/**
 * @brief Store the application's reaction (level 1).
 *
 * @param self The group; checked by ::lh_assert_runtime_ref.
 * @param fn   The reaction, or ::lh_null.
 */
lh_void
lh_gx_radio_group_set_on_change(lh_gx_radio_group_t *self, lh_gx_radio_group_change_fn *fn);

/**
 * @brief What the reaction is given (level 1).
 *
 * @param self The group; checked by ::lh_assert_runtime_ref.
 */
lh_ptr
lh_gx_radio_group_get_context(const lh_gx_radio_group_t *self);

/**
 * @brief Store what the reaction is given (level 1).
 *
 * @param self    The group; checked by ::lh_assert_runtime_ref.
 * @param context The application's data.
 */
lh_void
lh_gx_radio_group_set_context(lh_gx_radio_group_t *self, lh_ptr context);

/**
 * @brief A group with nothing chosen and the reaction @p on_change given
 *        @p context.
 *
 * @param self      The group; checked by ::lh_assert_runtime_ref.
 * @param on_change The reaction, or ::lh_null.
 * @param context   What the reaction is given.
 */
lh_void
lh_gx_radio_group_init(lh_gx_radio_group_t *self, lh_gx_radio_group_change_fn *on_change,
                       lh_ptr context);

/**
 * @brief Call the application's reaction with the radio chosen; nothing when
 *        there is no reaction or nothing is chosen.
 *
 * @param self The group; must not be null.
 */
lh_void
lh_gx_radio_group_change(lh_gx_radio_group_t *self);

/**
 * @brief Choose @p radio from the program: the mark of the radio chosen
 *        before is hidden, the mark of @p radio shown. The reaction is not
 *        called.
 *
 * @param self  The group; must not be null.
 * @param radio A radio of this group, or ::lh_null to choose none.
 */
lh_void
lh_gx_radio_group_select(lh_gx_radio_group_t *self, struct lh_gx_radio *radio);

/**
 * @brief The user chose @p radio: ::lh_gx_radio_group_select and the reaction,
 *        when @p radio is not the one chosen already.
 *
 * @param self  The group; must not be null.
 * @param radio A radio of this group; must not be null.
 * @return ::lh_bool_true when the choice changed.
 */
lh_bool_t
lh_gx_radio_group_choose(lh_gx_radio_group_t *self, struct lh_gx_radio *radio);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_RADIO_GROUP_H */
