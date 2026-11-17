/**
 * @file entity.c
 * @brief Implementation of `lh/gx/entity.h`.
 *
 * Every setter that can change what the screen shows reports the change to
 * the listener: the entity does not remember it, the canvas does.
 */

#include <lh/core/assert/runtime.h>
#include <lh/core/util/addr.h>
#include <lh/core/util/ptr.h>
#include <lh/core/util/return.h>
#include <lh/gx/entity.h>

lh_void
lh_gx_entity_init(lh_gx_entity_t *self, lh_gx_entity_kind_t kind)
{
    lh_assert_runtime_ref(self);

    self->kind = kind;
    self->flags = LH_GX_ENTITY_FLAG_VISIBLE | LH_GX_ENTITY_FLAG_ENABLED;
    lh_gx_rect_init_empty(lh_addr_of(self->rect));
    self->style = 0U;
    lh_str_view_init_empty(lh_addr_of(self->text));
    self->parent = LH_GX_ENTITY_NO_LINK;
    self->first_child = LH_GX_ENTITY_NO_LINK;
    self->child_count = 0U;
    self->next = LH_GX_ENTITY_NO_LINK;
    self->damage_fn = lh_null;
    self->damage_context = lh_null;
    self->user = lh_null;
}

lh_void
lh_gx_entity_report(const lh_gx_entity_t *self, const lh_gx_rect_t *before,
                    const lh_gx_rect_t *after)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(before);
    lh_assert_runtime_ref(after);
    lh_return_if(lh_null_eq(self->damage_fn));

    self->damage_fn(self->damage_context, before, after);
}

lh_gx_entity_kind_t
lh_gx_entity_get_kind(const lh_gx_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return self->kind;
}

lh_void
lh_gx_entity_set_kind(lh_gx_entity_t *self, lh_gx_entity_kind_t kind)
{
    lh_gx_rect_t before;

    lh_assert_runtime_ref(self);
    lh_ptr_deref(lh_addr_of(before)) = *lh_gx_entity_get_rect(self);
    self->kind = kind;
    lh_gx_entity_report(self, lh_addr_of(before), lh_addr_of(self->rect));
}

lh_u32_t
lh_gx_entity_get_flags(const lh_gx_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return self->flags;
}

lh_void
lh_gx_entity_set_flags(lh_gx_entity_t *self, lh_u32_t flags)
{
    lh_gx_rect_t before;

    lh_assert_runtime_ref(self);
    lh_ptr_deref(lh_addr_of(before)) = *lh_gx_entity_get_rect(self);
    self->flags = flags;
    lh_gx_entity_report(self, lh_addr_of(before), lh_addr_of(self->rect));
}

const lh_gx_rect_t *
lh_gx_entity_get_rect(const lh_gx_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->rect);
}

lh_void
lh_gx_entity_set_rect(lh_gx_entity_t *self, const lh_gx_rect_t *rect)
{
    lh_gx_rect_t before;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(rect);
    lh_ptr_deref(lh_addr_of(before)) = *lh_gx_entity_get_rect(self);
    lh_ptr_deref(lh_addr_of(self->rect)) = *rect;
    lh_gx_entity_report(self, lh_addr_of(before), lh_addr_of(self->rect));
}

lh_u16_t
lh_gx_entity_get_style(const lh_gx_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return self->style;
}

lh_void
lh_gx_entity_set_style(lh_gx_entity_t *self, lh_u16_t style)
{
    lh_gx_rect_t before;

    lh_assert_runtime_ref(self);
    lh_ptr_deref(lh_addr_of(before)) = *lh_gx_entity_get_rect(self);
    self->style = style;
    lh_gx_entity_report(self, lh_addr_of(before), lh_addr_of(self->rect));
}

const lh_str_view_t *
lh_gx_entity_get_text(const lh_gx_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->text);
}

lh_void
lh_gx_entity_set_text(lh_gx_entity_t *self, const lh_str_view_t *text)
{
    lh_gx_rect_t before;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(text);
    lh_ptr_deref(lh_addr_of(before)) = *lh_gx_entity_get_rect(self);
    lh_str_view_assign(lh_addr_of(self->text), text);
    lh_gx_entity_report(self, lh_addr_of(before), lh_addr_of(self->rect));
}

lh_void
lh_gx_entity_set_text_cstr(lh_gx_entity_t *self, const lh_str_ptr text)
{
    lh_str_view_t view;

    lh_assert_runtime_ref(self);
    lh_str_view_init(lh_addr_of(view), text);
    lh_gx_entity_set_text(self, lh_addr_of(view));
}

lh_u32_t
lh_gx_entity_get_parent(const lh_gx_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return self->parent;
}

lh_u32_t
lh_gx_entity_get_first_child(const lh_gx_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return self->first_child;
}

lh_u32_t
lh_gx_entity_get_child_count(const lh_gx_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return self->child_count;
}

lh_u32_t
lh_gx_entity_get_next(const lh_gx_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return self->next;
}

lh_gx_entity_damage_cb
lh_gx_entity_get_damage_fn(const lh_gx_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return self->damage_fn;
}

lh_void
lh_gx_entity_set_damage_fn(lh_gx_entity_t *self, lh_gx_entity_damage_cb fn)
{
    lh_assert_runtime_ref(self);
    self->damage_fn = fn;
}

lh_ptr
lh_gx_entity_get_damage_context(const lh_gx_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return self->damage_context;
}

lh_void
lh_gx_entity_set_damage_context(lh_gx_entity_t *self, lh_ptr context)
{
    lh_assert_runtime_ref(self);
    self->damage_context = context;
}

lh_ptr
lh_gx_entity_get_user(const lh_gx_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return self->user;
}

lh_void
lh_gx_entity_set_user(lh_gx_entity_t *self, lh_ptr user)
{
    lh_assert_runtime_ref(self);
    self->user = user;
}
