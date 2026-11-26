/**
 * @file button.c
 * @brief Implementation of `lh/gl/button.h`.
 */
#include <lh/core/assert.h>

#include <lh/gl/button.h>
#include <lh/core/util/return.h>

const lh_gl_entity_class_t lh_gl_button_class = {lh_gl_button_event,
                                                 lh_addr_of(lh_gl_container_class)};

/* -- Lifetime and fields --------------------------------------------------- */

lh_gl_container_t *
lh_gl_button_as_container(lh_gl_button_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->container);
}

lh_gl_button_t *
lh_gl_entity_as_button(lh_gl_entity_t *entity)
{
    lh_return_if(lh_null_eq(entity), lh_null);
    lh_return_if(
        !lh_gl_entity_class_is(lh_gl_entity_get_class(entity), lh_addr_of(lh_gl_button_class)),
        lh_null);
    return lh_ptr_rcast(lh_gl_button_t, entity);
}

const lh_gl_style_t *
lh_gl_button_get_style(const lh_gl_button_t *self)
{
    lh_assert_runtime_ref(self);
    return self->rest_style;
}

/* -- Looks ----------------------------------------------------------------- */

const lh_gl_style_t *
lh_gl_button_get_hot_style(const lh_gl_button_t *self)
{
    lh_assert_runtime_ref(self);
    return self->hot_style;
}

lh_bool_t
lh_gl_button_get_hot(const lh_gl_button_t *self)
{
    lh_assert_runtime_ref(self);
    return self->hot;
}

/* -- Hover ----------------------------------------------------------------- */

lh_gl_button_on_click_cb
lh_gl_button_get_on_click(const lh_gl_button_t *self)
{
    lh_assert_runtime_ref(self);
    return self->on_click;
}

const lh_gl_container_t *
lh_gl_button_get_container(const lh_gl_button_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->container);
}

lh_gl_container_t *
lh_gl_button_get_container_mut(lh_gl_button_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->container);
}

lh_void
lh_gl_button_set_container(lh_gl_button_t *self, lh_gl_container_t container)
{
    lh_assert_runtime_ref(self);
    self->container = container;
}

const lh_gl_style_t *
lh_gl_button_get_rest_style(const lh_gl_button_t *self)
{
    lh_assert_runtime_ref(self);
    return self->rest_style;
}

lh_void
lh_gl_button_set_rest_style(lh_gl_button_t *self, const lh_gl_style_t *rest_style)
{
    lh_assert_runtime_ref(self);
    self->rest_style = rest_style;
}

lh_bool_t
lh_gl_button_is_hot(const lh_gl_button_t *self)
{
    lh_assert_runtime_ref(self);
    return self->hot;
}

lh_gl_button_on_click_cb
lh_gl_button_get_on_click_fn(const lh_gl_button_t *self)
{
    lh_assert_runtime_ref(self);
    return self->on_click;
}

lh_void
lh_gl_button_set_on_click_fn(lh_gl_button_t *self, lh_gl_button_on_click_cb on_click)
{
    lh_assert_runtime_ref(self);
    self->on_click = on_click;
}

lh_ptr
lh_gl_button_get_click_context(const lh_gl_button_t *self)
{
    lh_assert_runtime_ref(self);
    return self->click_context;
}

lh_void
lh_gl_button_set_click_context(lh_gl_button_t *self, lh_ptr click_context)
{
    lh_assert_runtime_ref(self);
    self->click_context = click_context;
}

lh_void
lh_gl_button_set_hot(lh_gl_button_t *self, lh_bool_t hot)
{
    lh_assert_runtime_ref(self);
    self->hot = hot;
}

lh_void
lh_gl_button_set_hot_style(lh_gl_button_t *self, const lh_gl_style_t *hot_style)
{
    lh_assert_runtime_ref(self);
    self->hot_style = hot_style;
}

/* -- Events ---------------------------------------------------------------- */

lh_void
lh_gl_button_init(lh_gl_button_t *self, lh_gl_rect_t rect)
{
    /* The button is a container: that is what gives it somewhere to put a
       caption and a picture, and the flow that places them. */
    lh_gl_container_init(lh_gl_button_get_container_mut(self), rect);
    lh_gl_container_set_scroll_enabled(lh_gl_button_get_container_mut(self), lh_bool_false);
    lh_gl_entity_set_class_v(lh_gl_container_as_entity(lh_gl_button_get_container_mut(self)),
                             lh_addr_of(lh_gl_button_class));
    lh_gl_button_set_rest_style(self, lh_null);
    lh_gl_button_set_hot_style(self, lh_null);
    lh_gl_button_set_hot(self, lh_bool_false);
    lh_gl_button_set_on_click_fn(self, lh_null);
    lh_gl_button_set_click_context(self, lh_null);
}

lh_gl_entity_t *
lh_gl_button_as_entity(lh_gl_button_t *self)
{
    return lh_gl_container_as_entity(lh_gl_button_get_container_mut(self));
}

const lh_gl_style_t *
lh_gl_button_get_style_now(const lh_gl_button_t *self)
{
    return (lh_gl_button_is_hot(self) && !lh_null_eq(lh_gl_button_get_hot_style(self)))
               ? lh_gl_button_get_hot_style(self)
               : lh_gl_button_get_rest_style(self);
}

/** @copydoc lh_gl_button_on_click(const struct lh_gl_button *, const lh_gl_entity_event_t *) */
lh_void
lh_gl_button_on_click(const lh_gl_button_t *self, const lh_gl_entity_event_t *event)
{
    lh_return_if(lh_gl_entity_event_get_code(event) != lh_gl_entity_event_click);
    lh_gl_button_on_click_cb on_click = lh_gl_button_get_on_click(self);
    lh_return_if(lh_null_eq(on_click));
    /* The app is handed the button it already has a pointer to, not the entity
       inside it. */
    on_click(lh_ptr_rcast(lh_gl_button_t, self), lh_gl_button_get_click_context(self));
}

lh_void
lh_gl_button_set_on_click(lh_gl_button_t *self, lh_gl_button_on_click_cb on_click, lh_ptr context)
{
    lh_gl_button_set_on_click_fn(self, on_click);
    lh_gl_button_set_click_context(self, context);
}

lh_void
lh_gl_button_set_style(lh_gl_button_t *self, const lh_gl_style_t *style)
{
    lh_gl_button_set_rest_style(self, style);
    if (!lh_gl_button_is_hot(self))
    {
        lh_gl_entity_set_style_v(lh_gl_button_as_entity(self), style);
    }
}

lh_void
lh_gl_button_set_hot_style_v(lh_gl_button_t *self, const lh_gl_style_t *style)
{
    lh_gl_button_set_hot_style(self, style);
    if (lh_gl_button_is_hot(self))
    {
        lh_gl_entity_set_style_v(lh_gl_button_as_entity(self), lh_gl_button_get_style_now(self));
    }
}

lh_void
lh_gl_button_set_hot_v(lh_gl_button_t *self, lh_bool_t hot)
{
    lh_gl_button_set_hot(self, hot);
    lh_gl_entity_set_style_v(lh_gl_button_as_entity(self), lh_gl_button_get_style_now(self));
}

/* -- Click ----------------------------------------------------------------- */

/** @copydoc lh_gl_button_event(const struct lh_gl_entity *, const lh_gl_entity_event_t *) */
lh_void
lh_gl_button_event(const lh_gl_entity_t *self, const lh_gl_entity_event_t *event)
{
    /* The entity is the first field: self is the button. */
    const lh_gl_button_t *button = lh_ptr_rcast(const lh_gl_button_t, self);

    lh_gl_entity_class_event_base(lh_addr_of(lh_gl_button_class), self, event);
    if (lh_gl_entity_event_get_code(event) == lh_gl_entity_event_clickable)
    {
        /* A button takes the pointer over everything inside it: the caption and the
           picture are the button's, not two things of their own that take a click
           away from it. */
        lh_ptr_deref(lh_gl_entity_event_get_clickable(event)) = lh_bool_true;
    }
    lh_gl_button_on_click(button, event);
}
