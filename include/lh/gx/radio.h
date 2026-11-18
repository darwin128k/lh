/**
 * @file radio.h
 * @brief A radio: ::lh_gx_radio_t, one choice of a group.
 *
 * A radio looks like a checkbox and is one: it is made of an
 * ::lh_gx_checkbox_t — the row, the square, the mark, the caption, laid out
 * the same — whose row is of ::lh_gx_radio_class instead. What differs is the
 * tap: on a checkbox it toggles, on a radio it chooses the radio in its group
 * (::lh_gx_radio_group_choose), which unmarks the one chosen before. A tap on
 * the radio already chosen does nothing.
 *
 * A round radio is a matter of style: a `radius` of half the square in the
 * square's style and of half the mark in the mark's.
 *
 * Ownership: the struct is the application's and gx never stores it. The
 * application attaches the row (::lh_gx_radio_get_row_mut) where the radio goes.
 * The group, the string and the styles are the application's and must outlive
 * the radio.
 *
 * Usage:
 * @code
 * lh_gx_radio_group_init(&screen->size, on_size, screen);
 * lh_gx_radio_init(&screen->small, &screen->size, &theme.round, &theme.dot, 16, 40, 120, 20,
 *                  "Small");
 * lh_gx_radio_init(&screen->large, &screen->size, &theme.round, &theme.dot, 16, 64, 120, 20,
 *                  "Large");
 * lh_gx_radio_group_select(&screen->size, &screen->small);
 * @endcode
 */

#ifndef LH_GX_RADIO_H
#define LH_GX_RADIO_H

#include <lh/gx/checkbox.h>
#include <lh/gx/radio/fields.h>
#include <lh/gx/radio/group.h>

/**
 * @struct lh_gx_radio
 * @brief A radio. Members: ::lh_gx_radio_fields.
 */
struct lh_gx_radio
{
    /** Members: ::lh_gx_radio_fields. */
    lh_gx_radio_fields(lh_gx_checkbox_t, lh_gx_radio_group_t);
};
/** @copybrief lh_gx_radio */
typedef struct lh_gx_radio lh_gx_radio_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The class of a radio's row: no `paint`, no `covers`, its own `input`
 *        (::lh_gx_radio_input).
 */
extern const lh_gx_class_t lh_gx_radio_class;

/**
 * @brief The checkbox the radio is made of (level 1).
 *
 * @param self The radio; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
const lh_gx_checkbox_t *
lh_gx_radio_get_checkbox(const lh_gx_radio_t *self);

/**
 * @brief The checkbox the radio is made of, to change (level 1).
 *
 * @param self The radio; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_gx_checkbox_t *
lh_gx_radio_get_checkbox_mut(lh_gx_radio_t *self);

/**
 * @brief The group the radio is in (level 1).
 *
 * @param self The radio; checked by ::lh_assert_runtime_ref.
 * @return The group; not owned.
 */
lh_gx_radio_group_t *
lh_gx_radio_get_group_mut(const lh_gx_radio_t *self);

/**
 * @brief Store the group the radio is in (level 1).
 *
 * @param self  The radio; checked by ::lh_assert_runtime_ref.
 * @param group The group; not owned.
 */
lh_void
lh_gx_radio_set_group(lh_gx_radio_t *self, lh_gx_radio_group_t *group);

/**
 * @brief The radio whose row is @p entity: the row's content.
 *
 * @param entity The row of a radio; must not be null.
 * @return The radio.
 */
lh_gx_radio_t *
lh_gx_radio_from_entity(lh_gx_entity_t *entity);

/**
 * @brief The row: the entity the application attaches.
 *
 * @param self The radio; must not be null.
 * @return The row, never ::lh_null.
 */
lh_gx_entity_t *
lh_gx_radio_get_row_mut(lh_gx_radio_t *self);

/**
 * @brief Whether the radio is the one chosen in its group: its mark shows.
 *
 * @param self The radio; must not be null.
 */
lh_bool_t
lh_gx_radio_is_selected(const lh_gx_radio_t *self);

/**
 * @brief `input` of the radio class (::lh_gx_class_input_fn): the event goes to
 *        the checkbox's tap (::lh_gx_tap_take), and a tap chooses the radio in
 *        its group (::lh_gx_radio_group_choose).
 *
 * @param entity The row of a radio.
 * @param event  The event.
 */
lh_void
lh_gx_radio_input(lh_gx_entity_t *entity, const lh_gx_event_t *event);

/**
 * @brief A radio of @p group at @p x, @p y, @p width by @p height (relative to
 *        the owner it will be attached to), with the caption @p text, not
 *        chosen.
 *
 * The checkbox is made (::lh_gx_checkbox_init, unchecked, with no reaction of
 * its own), then its row is made a radio's. Choosing it is the group's
 * (::lh_gx_radio_group_select). The row is attached to nothing.
 *
 * @param self       The radio; must not be null.
 * @param group      Its group; not owned.
 * @param style      The square's fill, border and radius, the caption's ink and
 *                   font; not owned.
 * @param mark_style The mark's fill and radius; not owned.
 * @param x          Left edge, relative to the owner.
 * @param y          Top edge, relative to the owner.
 * @param width      Width of the whole row, caption included.
 * @param height     Height; the square is this high and as wide.
 * @param text       The caption, UTF-8, or ::lh_null; not owned.
 */
lh_void
lh_gx_radio_init(lh_gx_radio_t *self, lh_gx_radio_group_t *group, const lh_gx_style_t *style,
                 const lh_gx_style_t *mark_style, lh_gx_scalar_t x, lh_gx_scalar_t y,
                 lh_gx_scalar_t width, lh_gx_scalar_t height, const lh_char_t *text);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_RADIO_H */
