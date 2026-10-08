/**
 * @file toggle.c
 * @brief Implementation of `lh/ui/entity/toggle.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/null.h>
#include <lh/ui/entity.h>
#include <lh/ui/entity/button.h>
#include <lh/ui/entity/toggle.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>
#include <lh/util/return.h>

const lh_ui_entity_class_t lh_ui_entity_toggle_class = {
    lh_ui_toggle_event, lh_addr_of(lh_ui_entity_button_class)};

/* The looks of the state @p self is in, put on the button — which re-aims the
   entity by itself: ::lh_ui_button_set_style puts the resting one back when the
   pointer is elsewhere, and ::lh_ui_button_set_hot_style puts the hovered one
   while it is there. That is why a flip under the pointer keeps the hot look:
   the pointer asked the button, not the state. Every setter below ends here, so
   there is one place that decides what a state change means. */
static void
lh_ui_toggle_push_looks(lh_ui_toggle_t *self)
{
    lh_ui_button_t *button = lh_addr_of(self->button);

    lh_ui_button_set_style(button, self->checked ? self->on_style : self->off_style);
    lh_ui_button_set_hot_style(button, self->checked ? self->on_hot_style : self->off_hot_style);
}

/* ── Events ──────────────────────────────────────────────────────────────── */

lh_void
lh_ui_toggle_event(const lh_ui_entity_t *self, const lh_ui_entity_event_t *event)
{
    /* The button is the first field: self is the toggle. */
    const lh_ui_toggle_t *toggle = lh_ptr_rcast(const lh_ui_toggle_t, self);

    /* The button draws and would answer a click of its own — its callback is
       never set, so what comes back from here is the draw. */
    lh_ui_entity_class_event_base(lh_addr_of(lh_ui_entity_toggle_class), self, event);
    lh_ui_toggle_on_click(toggle, event);
}

lh_void
lh_ui_toggle_on_click(const lh_ui_toggle_t *self, const lh_ui_entity_event_t *event)
{
    lh_ui_toggle_on_click_cb on_click;

    lh_return_if(lh_ui_entity_event_get_code(event) != lh_ui_entity_event_click);
    /* A toggle answers with itself: what a click means here is that the state
       changed, and that is this component's business. The app is told after the
       flip, so it reads the new state rather than tracking which way it went. */
    lh_ui_toggle_set_checked(lh_ptr_rcast(lh_ui_toggle_t, self),
                             !lh_ui_toggle_get_checked(self));
    on_click = lh_ui_toggle_get_on_click(self);
    lh_return_if(lh_null_eq(on_click));
    on_click(lh_ptr_rcast(lh_ui_toggle_t, self), self->click_context);
}

/* ── Lifetime and fields ─────────────────────────────────────────────────── */

lh_void
lh_ui_toggle_init(lh_ui_toggle_t *self, lh_ui_rect_t rect)
{
    lh_assert_runtime_ref(self);
    lh_ui_button_init(lh_addr_of(self->button), rect);
    /* The button's class is the toggle's: a toggle is a button, and a tree that
       asks for a button has to find one here. */
    lh_ui_entity_set_class(lh_ui_button_as_entity(lh_addr_of(self->button)),
                           lh_addr_of(lh_ui_entity_toggle_class));
    self->off_style = lh_null;
    self->off_hot_style = lh_null;
    self->on_style = lh_null;
    self->on_hot_style = lh_null;
    self->checked = lh_bool_false;
    self->on_click = lh_null;
    self->click_context = lh_null;
}

lh_ui_entity_t *
lh_ui_toggle_as_entity(lh_ui_toggle_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_ui_button_as_entity(lh_addr_of(self->button));
}

lh_ui_button_t *
lh_ui_toggle_as_button(lh_ui_toggle_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->button);
}

lh_ui_toggle_t *
lh_ui_entity_as_toggle(lh_ui_entity_t *entity)
{
    lh_return_if(lh_null_eq(entity), lh_null);
    lh_return_if(!lh_ui_entity_class_is(lh_ui_entity_get_class(entity),
                                        lh_addr_of(lh_ui_entity_toggle_class)),
                 lh_null);
    return lh_ptr_rcast(lh_ui_toggle_t, entity);
}

/* ── Looks ───────────────────────────────────────────────────────────────── */

lh_void
lh_ui_toggle_set_off_style(lh_ui_toggle_t *self, const lh_ui_style_t *rest, const lh_ui_style_t *hot)
{
    lh_assert_runtime_ref(self);
    self->off_style = rest;
    self->off_hot_style = hot;
    if (!self->checked)
    {
        lh_ui_toggle_push_looks(self);
    }
}

const lh_ui_style_t *
lh_ui_toggle_get_off_style(const lh_ui_toggle_t *self)
{
    lh_assert_runtime_ref(self);
    return self->off_style;
}

const lh_ui_style_t *
lh_ui_toggle_get_off_hot_style(const lh_ui_toggle_t *self)
{
    lh_assert_runtime_ref(self);
    return self->off_hot_style;
}

lh_void
lh_ui_toggle_set_on_style(lh_ui_toggle_t *self, const lh_ui_style_t *rest, const lh_ui_style_t *hot)
{
    lh_assert_runtime_ref(self);
    self->on_style = rest;
    self->on_hot_style = hot;
    if (self->checked)
    {
        lh_ui_toggle_push_looks(self);
    }
}

const lh_ui_style_t *
lh_ui_toggle_get_on_style(const lh_ui_toggle_t *self)
{
    lh_assert_runtime_ref(self);
    return self->on_style;
}

const lh_ui_style_t *
lh_ui_toggle_get_on_hot_style(const lh_ui_toggle_t *self)
{
    lh_assert_runtime_ref(self);
    return self->on_hot_style;
}

/* ── State ───────────────────────────────────────────────────────────────── */

lh_bool_t
lh_ui_toggle_get_checked(const lh_ui_toggle_t *self)
{
    lh_assert_runtime_ref(self);
    return self->checked;
}

lh_void
lh_ui_toggle_set_checked(lh_ui_toggle_t *self, lh_bool_t checked)
{
    lh_assert_runtime_ref(self);
    lh_return_if(self->checked == checked);
    self->checked = checked;
    lh_ui_toggle_push_looks(self);
}

/* ── Click ───────────────────────────────────────────────────────────────── */

lh_ui_toggle_on_click_cb
lh_ui_toggle_get_on_click(const lh_ui_toggle_t *self)
{
    lh_assert_runtime_ref(self);
    return self->on_click;
}

lh_void
lh_ui_toggle_set_on_click(lh_ui_toggle_t *self, lh_ui_toggle_on_click_cb on_click, lh_ptr context)
{
    lh_assert_runtime_ref(self);
    self->on_click = on_click;
    self->click_context = context;
}