/**
 * @file button.c
 * @brief Implementation of `lh/ui/entity/button.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/null.h>
#include <lh/ui/entity.h>
#include <lh/ui/entity/button.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>
#include <lh/util/return.h>

const lh_ui_entity_class_t lh_ui_entity_button_class = {
    lh_ui_button_event, lh_addr_of(lh_ui_entity_class)};

/* ── Events ──────────────────────────────────────────────────────────────── */

lh_void
lh_ui_button_event(const lh_ui_entity_t *self, const lh_ui_entity_event_t *event)
{
    /* The entity is the first field: self is the button. */
    const lh_ui_button_t *button = lh_ptr_rcast(const lh_ui_button_t, self);

    lh_ui_entity_class_event_base(lh_addr_of(lh_ui_entity_button_class), self, event);
    lh_ui_button_on_click(button, event);
}

lh_void
lh_ui_button_on_click(const lh_ui_button_t *self, const lh_ui_entity_event_t *event)
{
    lh_ui_button_on_click_cb on_click;

    lh_return_if(lh_ui_entity_event_get_code(event) != lh_ui_entity_event_click);
    on_click = lh_ui_button_get_on_click(self);
    lh_return_if(lh_null_eq(on_click));
    /* The app is handed the button it already has a pointer to, not the entity
       inside it. */
    on_click(lh_ptr_rcast(lh_ui_button_t, self), self->click_context);
}

/* ── Lifetime and fields ─────────────────────────────────────────────────── */

lh_void
lh_ui_button_init(lh_ui_button_t *self, lh_ui_rect_t rect)
{
    lh_assert_runtime_ref(self);
    lh_ui_entity_init(lh_addr_of(self->entity), rect);
    lh_ui_entity_set_class(lh_addr_of(self->entity), lh_addr_of(lh_ui_entity_button_class));
    self->rest_style = lh_null;
    self->hot_style = lh_null;
    self->hot = lh_bool_false;
    self->on_click = lh_null;
    self->click_context = lh_null;
}

lh_ui_entity_t *
lh_ui_button_as_entity(lh_ui_button_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->entity);
}

lh_ui_button_t *
lh_ui_entity_as_button(lh_ui_entity_t *entity)
{
    lh_return_if(lh_null_eq(entity), lh_null);
    lh_return_if(!lh_ui_entity_class_is(lh_ui_entity_get_class(entity), lh_addr_of(lh_ui_entity_button_class)),
                 lh_null);
    return lh_ptr_rcast(lh_ui_button_t, entity);
}

/* ── Looks ───────────────────────────────────────────────────────────────── */

const lh_ui_style_t *
lh_ui_button_get_style(const lh_ui_button_t *self)
{
    lh_assert_runtime_ref(self);
    return self->rest_style;
}

lh_void
lh_ui_button_set_style(lh_ui_button_t *self, const lh_ui_style_t *style)
{
    lh_assert_runtime_ref(self);
    self->rest_style = style;
    if (!self->hot)
    {
        lh_ui_entity_set_style(lh_addr_of(self->entity), style);
    }
}

const lh_ui_style_t *
lh_ui_button_get_hot_style(const lh_ui_button_t *self)
{
    lh_assert_runtime_ref(self);
    return self->hot_style;
}

lh_void
lh_ui_button_set_hot_style(lh_ui_button_t *self, const lh_ui_style_t *style)
{
    lh_assert_runtime_ref(self);
    self->hot_style = style;
    if (self->hot)
    {
        lh_ui_entity_set_style(lh_addr_of(self->entity), lh_ui_button_get_style_now(self));
    }
}

const lh_ui_style_t *
lh_ui_button_get_style_now(const lh_ui_button_t *self)
{
    lh_assert_runtime_ref(self);
    return (self->hot && !lh_null_eq(self->hot_style)) ? self->hot_style : self->rest_style;
}

/* ── Hover ───────────────────────────────────────────────────────────────── */

lh_bool_t
lh_ui_button_get_hot(const lh_ui_button_t *self)
{
    lh_assert_runtime_ref(self);
    return self->hot;
}

lh_void
lh_ui_button_set_hot(lh_ui_button_t *self, lh_bool_t hot)
{
    lh_assert_runtime_ref(self);
    self->hot = hot;
    lh_ui_entity_set_style(lh_addr_of(self->entity), lh_ui_button_get_style_now(self));
}

/* ── Click ───────────────────────────────────────────────────────────────── */

lh_ui_button_on_click_cb
lh_ui_button_get_on_click(const lh_ui_button_t *self)
{
    lh_assert_runtime_ref(self);
    return self->on_click;
}

lh_void
lh_ui_button_set_on_click(lh_ui_button_t *self, lh_ui_button_on_click_cb on_click, lh_ptr context)
{
    lh_assert_runtime_ref(self);
    self->on_click = on_click;
    self->click_context = context;
}