/**
 * @file transition.h
 * @brief One look in flight between two styles: ::lh_ui_transition_t.
 *
 * When an entity is switched to a style that asks for time
 * (::lh_ui_style_set_transition), it does not take the new look at once. Its view
 * gives it one of these, the entity points at the slot's own look, and every frame
 * (::lh_ui_view_advance) the look is made again a little further from where it set
 * out (::lh_ui_style_init_blend) on the style's curve (::lh_ui_easing_t). When the
 * time is up the entity points at the style it was switched to and the slot is free.
 *
 * Because the entity simply points at a style, everything that draws it -- the fill
 * and its corners, the text, the border, the shadow -- shows the transition without
 * knowing there is one.
 */

#ifndef LH_UI_TRANSITION_H
#define LH_UI_TRANSITION_H

#include <lh/ui/style.h>
#include <lh/ui/transition/fields.h>

struct lh_ui_entity;

/**
 * @struct lh_ui_transition
 * @brief A slot for one look in flight.
 */
struct lh_ui_transition
{
    /** Members: ::lh_ui_transition_fields. */
    lh_ui_transition_fields(struct lh_ui_entity, lh_ui_style_t);
};
/** @copybrief lh_ui_transition */
typedef struct lh_ui_transition lh_ui_transition_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief An empty slot.
 */
lh_void
lh_ui_transition_init(lh_ui_transition_t *self);

/**
 * @brief The entity in flight, or ::lh_null for an empty slot.
 */
struct lh_ui_entity *
lh_ui_transition_get_entity(const lh_ui_transition_t *self);

/**
 * @brief The style the entity is heading for.
 */
const lh_ui_style_t *
lh_ui_transition_get_target(const lh_ui_transition_t *self);

/**
 * @brief The look on screen: what the entity points at while the slot is in use.
 */
const lh_ui_style_t *
lh_ui_transition_get_look(const lh_ui_transition_t *self);

/**
 * @brief Set @p entity out from the look @p from towards @p to, over @p to's time.
 *
 * @p from is copied first, so it may be this slot's own look: a transition turned
 * round half way sets out from the pixel on screen.
 */
lh_void
lh_ui_transition_start(lh_ui_transition_t *self, struct lh_ui_entity *entity,
                       const lh_ui_style_t *from, const lh_ui_style_t *to);

/**
 * @brief Spend @p ms and make the look again.
 *
 * @return ::lh_bool_false when the time is up: the look is then @p to's, and the
 *         slot's owner points the entity at the target and empties the slot.
 */
lh_bool_t
lh_ui_transition_advance(lh_ui_transition_t *self, lh_u16_t ms);

/**
 * @brief Store the entity the transition animates (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The transition; checked by ::lh_assert_runtime_ref.
 * @param entity The new value.
 */
lh_void
lh_ui_transition_set_entity(lh_ui_transition_t *self, struct lh_ui_entity * entity);

/**
 * @brief The style the transition ends at (level 1).
 *
 * @param self The transition; checked by ::lh_assert_runtime_ref.
 */
const lh_ui_style_t *
lh_ui_transition_get_to(const lh_ui_transition_t *self);

/**
 * @brief Store the style the transition ends at (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The transition; checked by ::lh_assert_runtime_ref.
 * @param to The new value.
 */
lh_void
lh_ui_transition_set_to(lh_ui_transition_t *self, const lh_ui_style_t * to);

/**
 * @brief The style the transition starts at (level 1).
 *
 * @param self The transition; checked by ::lh_assert_runtime_ref.
 */
const lh_ui_style_t *
lh_ui_transition_get_from(const lh_ui_transition_t *self);

/**
 * @brief The style the transition starts at, to change in place (level 1).
 *
 * @param self The transition; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_ui_style_t *
lh_ui_transition_get_from_mut(lh_ui_transition_t *self);

/**
 * @brief Store the style the transition starts at (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The transition; checked by ::lh_assert_runtime_ref.
 * @param from The new value.
 */
lh_void
lh_ui_transition_set_from(lh_ui_transition_t *self, lh_ui_style_t from);

/**
 * @brief The style shown now, to change in place (level 1).
 *
 * @param self The transition; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_ui_style_t *
lh_ui_transition_get_look_mut(lh_ui_transition_t *self);

/**
 * @brief Store the style shown now (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The transition; checked by ::lh_assert_runtime_ref.
 * @param look The new value.
 */
lh_void
lh_ui_transition_set_look(lh_ui_transition_t *self, lh_ui_style_t look);

/**
 * @brief How many milliseconds of the transition have passed (level 1).
 *
 * @param self The transition; checked by ::lh_assert_runtime_ref.
 */
lh_u16_t
lh_ui_transition_get_elapsed(const lh_ui_transition_t *self);

/**
 * @brief Store how many milliseconds of the transition have passed (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The transition; checked by ::lh_assert_runtime_ref.
 * @param elapsed The new value.
 */
lh_void
lh_ui_transition_set_elapsed(lh_ui_transition_t *self, lh_u16_t elapsed);

/**
 * @brief How many milliseconds the transition takes (level 1).
 *
 * @param self The transition; checked by ::lh_assert_runtime_ref.
 */
lh_u16_t
lh_ui_transition_get_total(const lh_ui_transition_t *self);

/**
 * @brief Store how many milliseconds the transition takes (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The transition; checked by ::lh_assert_runtime_ref.
 * @param total The new value.
 */
lh_void
lh_ui_transition_set_total(lh_ui_transition_t *self, lh_u16_t total);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_TRANSITION_H */
