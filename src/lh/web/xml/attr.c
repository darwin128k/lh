/**
 * @file attr.c
 * @brief Implementation of `lh/web/xml/attr.h`.
 */

#include <lh/web/xml/attr.h>
#include <lh/assert.h>
#include <lh/runtime/allocator.h>

/* ── accessors ───────────────────────────────────────────────────────────── */

lh_list_node_t *
lh_web_xml_attr_get_link_mut(lh_web_xml_attr_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->link);
}

const lh_list_node_t *
lh_web_xml_attr_get_link(const lh_web_xml_attr_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->link);
}

lh_web_xml_attr_t *
lh_web_xml_attr_get_by_link_mut(lh_list_node_t *link)
{
    return lh_list_entry(lh_web_xml_attr_t, link, link);
}

const lh_web_xml_attr_t *
lh_web_xml_attr_get_by_link(const lh_list_node_t *link)
{
    return lh_list_entry(const lh_web_xml_attr_t, link, link);
}

lh_str_t *
lh_web_xml_attr_get_name_mut(lh_web_xml_attr_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->name);
}

const lh_str_t *
lh_web_xml_attr_get_name(const lh_web_xml_attr_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->name);
}

lh_str_t *
lh_web_xml_attr_get_value_mut(lh_web_xml_attr_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->value);
}

const lh_str_t *
lh_web_xml_attr_get_value(const lh_web_xml_attr_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->value);
}

lh_bool_t
lh_web_xml_attr_get_bare(const lh_web_xml_attr_t *self)
{
    lh_assert_runtime_ref(self);
    return self->bare;
}

lh_void
lh_web_xml_attr_set_bare(lh_web_xml_attr_t *self, lh_bool_t bare)
{
    lh_assert_runtime_ref(self);
    self->bare = bare;
}

/* ── init / deinit ───────────────────────────────────────────────────────── */

lh_void
lh_web_xml_attr_init(lh_web_xml_attr_t *self)
{
    lh_list_node_init(lh_web_xml_attr_get_link_mut(self));
    lh_str_init(lh_web_xml_attr_get_name_mut(self));
    lh_str_init(lh_web_xml_attr_get_value_mut(self));
    lh_web_xml_attr_set_bare(self, lh_bool_false);
}

lh_void
lh_web_xml_attr_deinit(lh_web_xml_attr_t *self)
{
    lh_str_deinit(lh_web_xml_attr_get_name_mut(self));
    lh_str_deinit(lh_web_xml_attr_get_value_mut(self));
}

/* ── values and lifetime ─────────────────────────────────────────────────── */

lh_void
lh_web_xml_attr_set_name(lh_web_xml_attr_t *self, lh_str_view_t name)
{
    lh_str_assign_view(lh_web_xml_attr_get_name_mut(self), name);
}

lh_void
lh_web_xml_attr_set_value(lh_web_xml_attr_t *self, lh_str_view_t value)
{
    lh_str_assign_view(lh_web_xml_attr_get_value_mut(self), value);
    lh_web_xml_attr_set_bare(self, lh_bool_false);
}

lh_web_xml_attr_t *
lh_web_xml_attr_create(lh_void)
{
    lh_web_xml_attr_t *self =
        lh_ptr_cast(lh_web_xml_attr_t, lh_runtime_allocator_alloc(sizeof(lh_web_xml_attr_t)));

    lh_web_xml_attr_init(self);
    return self;
}

lh_void
lh_web_xml_attr_destroy(lh_web_xml_attr_t *self)
{
    lh_list_node_unlink(lh_web_xml_attr_get_link_mut(self));
    lh_web_xml_attr_deinit(self);
    lh_runtime_allocator_free(self);
}
