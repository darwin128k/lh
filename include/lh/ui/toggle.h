/**
 * @file toggle.h
 * @brief A button that remembers whether it is on: ::lh_ui_toggle_t.
 *
 * The second component, and the first one built *on* the first: a toggle **is** a
 * ::lh_ui_button_t (::lh_ui_button_class is its base, and the button is
 * its first field), so the press, its damage, the shadow, the radius and the
 * clickable area are the button's code, unchanged. A toggle adds one thing the
 * button has no place for: a second pair of looks, and the flag saying which
 * pair shows.
 *
 * A button has two looks (resting and hovered). A toggle has four (off or on,
 * each of them resting and hovered), and the engine only ever knows about two
 * of them at a time — the pair of the state the toggle is in. So the embedded
 * button holds that pair, and the toggle remembers both. Flipping pushes the
 * other pair into the button through its own setters and lets the button put the
 * right one back on the entity, which is why the hover the pointer is holding
 * survives a flip: it asked the button for a look, and the button answered from
 * the pair it was given.
 *
 * The pressed look is still the engine's (::lh_ui_entity_get_style_now), and the
 * damage of every state change is the view's, exactly as for a button — nothing
 * here draws or invalidates. A click flips first and calls the app after, so the
 * callback reads the new state instead of guessing which way it went.
 */

#ifndef LH_UI_TOGGLE_H
#define LH_UI_TOGGLE_H

#include <lh/ui/button.h>
#include <lh/ui/toggle/on/click/cb.h>

/**
 * @struct lh_ui_toggle
 * @brief A button, the two pairs of looks it has, and whether it is on.
 */
struct lh_ui_toggle
{
    lh_ui_button_t button;              /**< The button this toggle is (first field). */
    const lh_ui_style_t *off_style;     /**< Resting look while off. */
    const lh_ui_style_t *off_hot_style; /**< Hovered look while off. */
    const lh_ui_style_t *on_style;      /**< Resting look while on. */
    const lh_ui_style_t *on_hot_style;  /**< Hovered look while on. */
    lh_bool_t checked;                  /**< True while it is on. */
    lh_ui_toggle_on_click_cb on_click;  /**< Who to tell about a click. */
    lh_ptr click_context;               /**< What to tell them with. */
};
/** @copybrief lh_ui_toggle */
typedef struct lh_ui_toggle lh_ui_toggle_t;

/* -- Class ----------------------------------------------------------------- */

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Class of ::lh_ui_toggle_t, derived from ::lh_ui_button_class.
 */
extern const lh_ui_entity_class_t lh_ui_toggle_class;

/**
 * @brief Event function of ::lh_ui_toggle_class: the button first (the
 *        base draw and its click, which is unset and does nothing), then
 *        ::lh_ui_toggle_on_click.
 */
lh_void
lh_ui_toggle_event(const struct lh_ui_entity *self, const lh_ui_entity_event_t *event);

/**
 * @brief On ::lh_ui_entity_event_click: flip, then call
 *        ::lh_ui_toggle_get_on_click if the app set one. A click with no
 *        callback still flips — the state is the component's business, the
 *        callback is the app's. Other events are ignored.
 */
lh_void
lh_ui_toggle_on_click(const lh_ui_toggle_t *self, const lh_ui_entity_event_t *event);

/* -- Lifetime and fields --------------------------------------------------- */

/** The looks of the state @p self is in, put on the button — which re-aims the
   entity by itself: ::lh_ui_button_set_style puts the resting one back when the
   pointer is elsewhere, and ::lh_ui_button_set_hot_style puts the hovered one
   while it is there. That is why a flip under the pointer keeps the hot look:
   the pointer asked the button, not the state. Every setter below ends here, so
   there is one place that decides what a state change means. */
lh_void
lh_ui_toggle_push_looks(lh_ui_toggle_t *self);

/**
 * @brief A toggle at @p rect: no styles, off, not hovered, no callback.
 */
lh_void
lh_ui_toggle_init(lh_ui_toggle_t *self, lh_ui_rect_t rect);

/**
 * @brief The entity @p self is: what a tree holds and what a hit test returns.
 */
lh_ui_entity_t *
lh_ui_toggle_as_entity(lh_ui_toggle_t *self);

/**
 * @brief The button @p self is — the whole of it, hover included. This is what
 *        ::lh_ui_view_set_hot takes, and it is the button's own API: the toggle
 *        re-exports nothing (no `lh_ui_toggle_get_hot` and no
 *        `lh_ui_toggle_get_style_now`), because the button already answers
 *        those for the state the toggle is in.
 */
lh_ui_button_t *
lh_ui_toggle_as_button(lh_ui_toggle_t *self);

/**
 * @brief The toggle @p entity is, or ::lh_null when it is not one — the class
 *        check is what tells the two apart, the same way
 *        ::lh_ui_entity_as_button does. A toggle is found as a button too, since
 *        it is one.
 */
lh_ui_toggle_t *
lh_ui_entity_as_toggle(lh_ui_entity_t *entity);

/* -- Looks ----------------------------------------------------------------- */

/**
 * @brief The two looks while @p self is off (not owned, either ::lh_null for
 *        none).
 */
lh_void
lh_ui_toggle_set_off_styles(lh_ui_toggle_t *self, const lh_ui_style_t *rest,
                           const lh_ui_style_t *hot);

/**
 * @brief The resting look while off (::lh_null for none).
 */
const lh_ui_style_t *
lh_ui_toggle_get_off_style(const lh_ui_toggle_t *self);

/**
 * @brief The hovered look while off (::lh_null for none).
 */
const lh_ui_style_t *
lh_ui_toggle_get_off_hot_style(const lh_ui_toggle_t *self);

/**
 * @brief The two looks while @p self is on (not owned, either ::lh_null for
 *        none). Shows them at once when @p self is already on.
 */
lh_void
lh_ui_toggle_set_on_styles(lh_ui_toggle_t *self, const lh_ui_style_t *rest,
                          const lh_ui_style_t *hot);

/**
 * @brief The resting look while on (::lh_null for none).
 */
const lh_ui_style_t *
lh_ui_toggle_get_on_style(const lh_ui_toggle_t *self);

/**
 * @brief The hovered look while on (::lh_null for none).
 */
const lh_ui_style_t *
lh_ui_toggle_get_on_hot_style(const lh_ui_toggle_t *self);

/* -- State ----------------------------------------------------------------- */

/**
 * @brief True while @p self is on.
 */
lh_bool_t
lh_ui_toggle_get_checked(const lh_ui_toggle_t *self);

/**
 * @brief Say whether @p self is on, and put the looks of that state on the
 *        entity. No damage: the app invalidates, the same as after a scroll.
 */
lh_void
lh_ui_toggle_set_checked(lh_ui_toggle_t *self, lh_bool_t checked);

/* -- Click ----------------------------------------------------------------- */

/**
 * @brief Who to tell about a click on @p self, and with what context
 *        (::lh_null for none).
 */
lh_ui_toggle_on_click_cb
lh_ui_toggle_get_on_click(const lh_ui_toggle_t *self);

/**
 * @brief Tell @p on_click (with @p context) when a click lands on @p self —
 *        after it has flipped.
 */
lh_void
lh_ui_toggle_set_on_click(lh_ui_toggle_t *self, lh_ui_toggle_on_click_cb on_click, lh_ptr context);

/**
 * @brief The button the toggle is built on (level 1).
 *
 * @param self The toggle; checked by ::lh_assert_runtime_ref.
 */
const lh_ui_button_t *
lh_ui_toggle_get_button(const lh_ui_toggle_t *self);

/**
 * @brief The button the toggle is built on, to change in place (level 1).
 *
 * @param self The toggle; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_ui_button_t *
lh_ui_toggle_get_button_mut(lh_ui_toggle_t *self);

/**
 * @brief Store the button the toggle is built on (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The toggle; checked by ::lh_assert_runtime_ref.
 * @param button The new value.
 */
lh_void
lh_ui_toggle_set_button(lh_ui_toggle_t *self, lh_ui_button_t button);

/**
 * @brief Store the style the toggle has when off and hot (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The toggle; checked by ::lh_assert_runtime_ref.
 * @param off_hot_style The new value.
 */
lh_void
lh_ui_toggle_set_off_hot_style(lh_ui_toggle_t *self, const lh_ui_style_t * off_hot_style);

/**
 * @brief Store the style the toggle has when on and hot (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The toggle; checked by ::lh_assert_runtime_ref.
 * @param on_hot_style The new value.
 */
lh_void
lh_ui_toggle_set_on_hot_style(lh_ui_toggle_t *self, const lh_ui_style_t * on_hot_style);

/**
 * @brief Whether the toggle is on (level 1).
 *
 * @param self The toggle; checked by ::lh_assert_runtime_ref.
 */
lh_bool_t
lh_ui_toggle_is_checked(const lh_ui_toggle_t *self);

/**
 * @brief The callback run when the toggle is clicked (level 1).
 *
 * @param self The toggle; checked by ::lh_assert_runtime_ref.
 */
lh_ui_toggle_on_click_cb
lh_ui_toggle_get_on_click_fn(const lh_ui_toggle_t *self);

/**
 * @brief Store the callback run when the toggle is clicked (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The toggle; checked by ::lh_assert_runtime_ref.
 * @param on_click The new value.
 */
lh_void
lh_ui_toggle_set_on_click_fn(lh_ui_toggle_t *self, lh_ui_toggle_on_click_cb on_click);

/**
 * @brief The value passed to the click callback (level 1).
 *
 * @param self The toggle; checked by ::lh_assert_runtime_ref.
 */
lh_ptr
lh_ui_toggle_get_click_context(const lh_ui_toggle_t *self);

/**
 * @brief Store the value passed to the click callback (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The toggle; checked by ::lh_assert_runtime_ref.
 * @param click_context The new value.
 */
lh_void
lh_ui_toggle_set_click_context(lh_ui_toggle_t *self, lh_ptr click_context);

/**
 * @brief Store the style the toggle has when off (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The toggle; checked by ::lh_assert_runtime_ref.
 * @param off_style The new value.
 */
lh_void
lh_ui_toggle_set_off_style(lh_ui_toggle_t *self, const lh_ui_style_t * off_style);

/**
 * @brief Store the style the toggle has when on (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The toggle; checked by ::lh_assert_runtime_ref.
 * @param on_style The new value.
 */
lh_void
lh_ui_toggle_set_on_style(lh_ui_toggle_t *self, const lh_ui_style_t * on_style);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_TOGGLE_H */