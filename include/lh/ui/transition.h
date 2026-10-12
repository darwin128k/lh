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

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/types.h>
#include <lh/ui/style.h>
#include <lh/ui/transition/fields.h>
#include <lh/void.h>

struct lh_ui_entity;

/**
 * @struct lh_ui_transition
 * @typedef lh_ui_transition_t
 * @brief A slot for one look in flight.
 */
struct lh_ui_transition
{
    lh_ui_transition_fields(struct lh_ui_entity, lh_ui_style_t);
};
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
lh_ui_transition_start(lh_ui_transition_t *self, struct lh_ui_entity *entity, const lh_ui_style_t *from,
                       const lh_ui_style_t *to);

/**
 * @brief Spend @p ms and make the look again.
 *
 * @return ::lh_bool_false when the time is up: the look is then @p to's, and the
 *         slot's owner points the entity at the target and empties the slot.
 */
lh_bool_t
lh_ui_transition_advance(lh_ui_transition_t *self, lh_u16_t ms);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_TRANSITION_H */
