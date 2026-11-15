/**
 * @file toggle.c
 * @brief Implementation of `lh/gl/toggle.h`.
 */
#include <lh/core/assert.h>

#include <lh/gl/toggle.h>
#include <lh/core/util/return.h>

const lh_gl_entity_class_t lh_gl_toggle_class = {lh_gl_toggle_event,
                                                 lh_addr_of(lh_gl_button_class)};

lh_void
lh_gl_toggle_push_looks(lh_gl_toggle_t *self)
{
    lh_gl_button_t *button = lh_gl_toggle_get_button_mut(self);

    lh_gl_button_set_style(button, lh_gl_toggle_is_checked(self)
                                       ? lh_gl_toggle_get_on_style(self)
                                       : lh_gl_toggle_get_off_style(self));
    lh_gl_button_set_hot_style_v(button, lh_gl_toggle_is_checked(self)
                                             ? lh_gl_toggle_get_on_hot_style(self)
                                             : lh_gl_toggle_get_off_hot_style(self));
}

/* -- Events ---------------------------------------------------------------- */

lh_void
lh_gl_toggle_init(lh_gl_toggle_t *self, lh_gl_rect_t rect)
{
    lh_gl_button_init(lh_gl_toggle_get_button_mut(self), rect);
    /* The button's class is the toggle's: a toggle is a button, and a tree that
       asks for a button has to find one here. */
    lh_gl_entity_set_class_v(lh_gl_button_as_entity(lh_gl_toggle_get_button_mut(self)),
                             lh_addr_of(lh_gl_toggle_class));
    lh_gl_toggle_set_off_style(self, lh_null);
    lh_gl_toggle_set_off_hot_style(self, lh_null);
    lh_gl_toggle_set_on_style(self, lh_null);
    lh_gl_toggle_set_on_hot_style(self, lh_null);
    lh_gl_toggle_set_checked(self, lh_bool_false);
    lh_gl_toggle_set_on_click_fn(self, lh_null);
    lh_gl_toggle_set_click_context(self, lh_null);
}

lh_gl_entity_t *
lh_gl_toggle_as_entity(lh_gl_toggle_t *self)
{
    return lh_gl_button_as_entity(lh_gl_toggle_get_button_mut(self));
}

/* -- Lifetime and fields --------------------------------------------------- */

lh_gl_button_t *
lh_gl_toggle_as_button(lh_gl_toggle_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->button);
}

lh_gl_toggle_t *
lh_gl_entity_as_toggle(lh_gl_entity_t *entity)
{
    lh_return_if(lh_null_eq(entity), lh_null);
    lh_return_if(
        !lh_gl_entity_class_is(lh_gl_entity_get_class(entity), lh_addr_of(lh_gl_toggle_class)),
        lh_null);
    return lh_ptr_rcast(lh_gl_toggle_t, entity);
}

lh_void
lh_gl_toggle_set_off_styles(lh_gl_toggle_t *self, const lh_gl_style_t *rest,
                            const lh_gl_style_t *hot)
{
    lh_gl_toggle_set_off_style(self, rest);
    lh_gl_toggle_set_off_hot_style(self, hot);
    if (!lh_gl_toggle_is_checked(self))
    {
        lh_gl_toggle_push_looks(self);
    }
}

const lh_gl_style_t *
lh_gl_toggle_get_off_style(const lh_gl_toggle_t *self)
{
    lh_assert_runtime_ref(self);
    return self->off_style;
}

/* -- Looks ----------------------------------------------------------------- */

const lh_gl_style_t *
lh_gl_toggle_get_off_hot_style(const lh_gl_toggle_t *self)
{
    lh_assert_runtime_ref(self);
    return self->off_hot_style;
}

lh_void
lh_gl_toggle_set_on_styles(lh_gl_toggle_t *self, const lh_gl_style_t *rest,
                           const lh_gl_style_t *hot)
{
    lh_gl_toggle_set_on_style(self, rest);
    lh_gl_toggle_set_on_hot_style(self, hot);
    if (lh_gl_toggle_is_checked(self))
    {
        lh_gl_toggle_push_looks(self);
    }
}

const lh_gl_style_t *
lh_gl_toggle_get_on_style(const lh_gl_toggle_t *self)
{
    lh_assert_runtime_ref(self);
    return self->on_style;
}

const lh_gl_style_t *
lh_gl_toggle_get_on_hot_style(const lh_gl_toggle_t *self)
{
    lh_assert_runtime_ref(self);
    return self->on_hot_style;
}

lh_bool_t
lh_gl_toggle_get_checked(const lh_gl_toggle_t *self)
{
    lh_assert_runtime_ref(self);
    return self->checked;
}

lh_void
lh_gl_toggle_set_checked_v(lh_gl_toggle_t *self, lh_bool_t checked)
{
    lh_return_if(lh_gl_toggle_is_checked(self) == checked);
    lh_gl_toggle_set_checked(self, checked);
    lh_gl_toggle_push_looks(self);
}

/* -- State ----------------------------------------------------------------- */

lh_gl_toggle_on_click_cb
lh_gl_toggle_get_on_click(const lh_gl_toggle_t *self)
{
    lh_assert_runtime_ref(self);
    return self->on_click;
}

lh_void
lh_gl_toggle_on_click(const lh_gl_toggle_t *self, const lh_gl_entity_event_t *event)
{
    lh_return_if(lh_gl_entity_event_get_code(event) != lh_gl_entity_event_click);
    /* A toggle answers with itself: what a click means here is that the state
       changed, and that is this component's business. The app is told after the
       flip, so it reads the new state rather than tracking which way it went. */
    lh_gl_toggle_set_checked_v(lh_ptr_rcast(lh_gl_toggle_t, self), !lh_gl_toggle_get_checked(self));
    lh_gl_toggle_on_click_cb on_click = lh_gl_toggle_get_on_click(self);
    lh_return_if(lh_null_eq(on_click));
    on_click(lh_ptr_rcast(lh_gl_toggle_t, self), lh_gl_toggle_get_click_context(self));
}

/* -- Click ----------------------------------------------------------------- */

/** @copydoc lh_gl_toggle_event(const struct lh_gl_entity *, const lh_gl_entity_event_t *) */
lh_void
lh_gl_toggle_event(const lh_gl_entity_t *self, const lh_gl_entity_event_t *event)
{
    /* The button is the first field: self is the toggle. */
    const lh_gl_toggle_t *toggle = lh_ptr_rcast(const lh_gl_toggle_t, self);

    /* The button draws and would answer a click of its own — its callback is
       never set, so what comes back from here is the draw. */
    lh_gl_entity_class_event_base(lh_addr_of(lh_gl_toggle_class), self, event);
    lh_gl_toggle_on_click(toggle, event);
}

lh_void
lh_gl_toggle_set_on_click(lh_gl_toggle_t *self, lh_gl_toggle_on_click_cb on_click, lh_ptr context)
{
    lh_gl_toggle_set_on_click_fn(self, on_click);
    lh_gl_toggle_set_click_context(self, context);
}

const lh_gl_button_t *
lh_gl_toggle_get_button(const lh_gl_toggle_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->button);
}

lh_gl_button_t *
lh_gl_toggle_get_button_mut(lh_gl_toggle_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->button);
}

lh_void
lh_gl_toggle_set_button(lh_gl_toggle_t *self, lh_gl_button_t button)
{
    lh_assert_runtime_ref(self);
    self->button = button;
}

lh_void
lh_gl_toggle_set_off_hot_style(lh_gl_toggle_t *self, const lh_gl_style_t *off_hot_style)
{
    lh_assert_runtime_ref(self);
    self->off_hot_style = off_hot_style;
}

lh_void
lh_gl_toggle_set_on_hot_style(lh_gl_toggle_t *self, const lh_gl_style_t *on_hot_style)
{
    lh_assert_runtime_ref(self);
    self->on_hot_style = on_hot_style;
}

lh_bool_t
lh_gl_toggle_is_checked(const lh_gl_toggle_t *self)
{
    lh_assert_runtime_ref(self);
    return self->checked;
}

lh_gl_toggle_on_click_cb
lh_gl_toggle_get_on_click_fn(const lh_gl_toggle_t *self)
{
    lh_assert_runtime_ref(self);
    return self->on_click;
}

lh_void
lh_gl_toggle_set_on_click_fn(lh_gl_toggle_t *self, lh_gl_toggle_on_click_cb on_click)
{
    lh_assert_runtime_ref(self);
    self->on_click = on_click;
}

lh_ptr
lh_gl_toggle_get_click_context(const lh_gl_toggle_t *self)
{
    lh_assert_runtime_ref(self);
    return self->click_context;
}

lh_void
lh_gl_toggle_set_click_context(lh_gl_toggle_t *self, lh_ptr click_context)
{
    lh_assert_runtime_ref(self);
    self->click_context = click_context;
}

lh_void
lh_gl_toggle_set_off_style(lh_gl_toggle_t *self, const lh_gl_style_t *off_style)
{
    lh_assert_runtime_ref(self);
    self->off_style = off_style;
}

lh_void
lh_gl_toggle_set_on_style(lh_gl_toggle_t *self, const lh_gl_style_t *on_style)
{
    lh_assert_runtime_ref(self);
    self->on_style = on_style;
}

lh_void
lh_gl_toggle_set_checked(lh_gl_toggle_t *self, lh_bool_t checked)
{
    lh_assert_runtime_ref(self);
    self->checked = checked;
}
