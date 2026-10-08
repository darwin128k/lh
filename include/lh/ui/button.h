/**
 * @file button.h
 * @brief A pressable thing with two looks: ::lh_ui_button_t.
 *
 * The first component that composes what the engine already knows instead of
 * repeating it. A button is an entity (::lh_ui_entity_t) with two styles — the
 * resting one and the one the pointer gets — a flag saying which of them shows,
 * and a callback for the click. Everything else comes from the core: the press
 * state and its damage (::lh_ui_view_press, ::lh_ui_view_set_pressed), the
 * shadow and the radius from the style, the clickable area from
 * ::lh_ui_style_set_hit_radius. There is no class per look.
 *
 * Hover is the one thing it holds itself, and on purpose: the pointer's position
 * arrives from the OS as coordinates, and the app is what turns them into "this
 * button is under the pointer". ::lh_ui_view_set_hot is that step with the
 * damage already added; ::lh_ui_button_set_hot is the raw one, for an app that
 * drives its own canvas.
 *
 * Class ::lh_ui_button_class: the base draw (fill, shadow, and the
 * pressed style's version of both) and, on a click, the callback if there is
 * one.
 */

#ifndef LH_UI_BUTTON_H
#define LH_UI_BUTTON_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/ptr.h>
#include <lh/ui/entity.h>
#include <lh/ui/container.h>
#include <lh/ui/button/fields.h>
#include <lh/ui/button/on/click/cb.h>
#include <lh/ui/button/on/click/fn.h>
#include <lh/ui/rect.h>
#include <lh/ui/style.h>
#include <lh/util/addr.h>
#include <lh/void.h>

/**
 * @struct lh_ui_button
 * @typedef lh_ui_button_t
 * @brief An entity, two styles, a hover flag and a click callback.
 */
struct lh_ui_button
{
    lh_ui_button_fields(lh_ui_container_t, lh_ui_style_t, lh_bool_t, lh_ui_button_on_click_cb, lh_ptr);
};
typedef struct lh_ui_button lh_ui_button_t;

LH_COMPILER_EXTERN_C_BEGIN

/* ── Class ───────────────────────────────────────────────────────────────── */

/**
 * @brief Class of ::lh_ui_button_t, derived from ::lh_ui_entity_class.
 */
extern const lh_ui_entity_class_t lh_ui_button_class;

/**
 * @brief Event function of ::lh_ui_button_class: the base class first
 *        (fill and shadow of the style in force), then
 *        ::lh_ui_button_on_click.
 */
lh_void
lh_ui_button_event(const struct lh_ui_entity *self, const lh_ui_entity_event_t *event);

/**
 * @brief On ::lh_ui_entity_event_click: call ::lh_ui_button_get_on_click if the
 *        app set one. A click with no callback is not an error and does
 *        nothing. Other events are ignored.
 */
lh_void
lh_ui_button_on_click(const struct lh_ui_button *self, const lh_ui_entity_event_t *event);

/* ── Lifetime and fields ─────────────────────────────────────────────────── */

/**
 * @brief A button at @p rect: no styles, not hovered, no callback.
 */
lh_void
lh_ui_button_init(lh_ui_button_t *self, lh_ui_rect_t rect);

/**
 * @brief The entity @p self is: what a tree holds and what a hit test returns.
 */
lh_ui_entity_t *
lh_ui_button_as_entity(lh_ui_button_t *self);

/**
 * @brief The container @p self is — where a caption and a picture go, and what
 *        ::lh_ui_container_set_layout is asked. A button **is** a container: that
 *        is the only way a button holds three entities and still places them,
 *        and this is where the flow of @p self comes from
 *        (::lh_ui_container_get_layout).
 */
lh_ui_container_t *
lh_ui_button_as_container(lh_ui_button_t *self);

/**
 * @brief The button @p entity is, or ::lh_null when it is not one — the class
 *        check is what tells the two apart, the same way
 *        ::lh_ui_entity_as_scrollbar does.
 */
lh_ui_button_t *
lh_ui_entity_as_button(lh_ui_entity_t *entity);

/* ── Looks ───────────────────────────────────────────────────────────────── */

/**
 * @brief The resting style of @p self (not owned, ::lh_null for none). It shows
 *        unless @p self is hovered and has a hot style.
 */
const lh_ui_style_t *
lh_ui_button_get_style(const lh_ui_button_t *self);

/**
 * @brief Replace the resting style of @p self. Shows it at once when @p self is
 *        not hovered.
 */
lh_void
lh_ui_button_set_style(lh_ui_button_t *self, const lh_ui_style_t *style);

/**
 * @brief The style the pointer gets on @p self (not owned, ::lh_null for none:
 *        then hovering changes nothing, which is what a button that only has a
 *        pressed look wants).
 */
const lh_ui_style_t *
lh_ui_button_get_hot_style(const lh_ui_button_t *self);

/**
 * @brief Replace the style the pointer gets on @p self. Shows it at once when
 *        @p self is hovered.
 */
lh_void
lh_ui_button_set_hot_style(lh_ui_button_t *self, const lh_ui_style_t *style);

/**
 * @brief The style @p self shows right now: the hot one when it is hovered and
 *        has one, the resting one otherwise. The pressed look is the engine's
 *        (::lh_ui_entity_get_style_now), not this function's.
 */
const lh_ui_style_t *
lh_ui_button_get_style_now(const lh_ui_button_t *self);

/* ── Hover ───────────────────────────────────────────────────────────────── */

/**
 * @brief True while the pointer is on @p self.
 */
lh_bool_t
lh_ui_button_get_hot(const lh_ui_button_t *self);

/**
 * @brief Say the pointer is on @p self, or is not, and switch the style
 *        accordingly. No damage: an app driving its own canvas calls
 *        ::lh_ui_view_set_hot instead, which does this and damages both looks.
 */
lh_void
lh_ui_button_set_hot(lh_ui_button_t *self, lh_bool_t hot);

/* ── Click ───────────────────────────────────────────────────────────────── */

/**
 * @brief Who to tell about a click on @p self, and with what context
 *        (::lh_null for none).
 */
lh_ui_button_on_click_cb
lh_ui_button_get_on_click(const lh_ui_button_t *self);

/**
 * @brief Tell @p on_click (with @p context) when a click lands on @p self.
 */
lh_void
lh_ui_button_set_on_click(lh_ui_button_t *self, lh_ui_button_on_click_cb on_click, lh_ptr context);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_BUTTON_H */