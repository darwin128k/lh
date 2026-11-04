/**
 * @file entity.c
 * @brief Field accessors for XML tree node data.
 */

#include <lh/core/web/xml/node/entity.h>
#include <lh/core/assert.h>

lh_list_node_t *
lh_web_xml_entity_get_link_mut(lh_web_xml_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->link);
}

const lh_list_node_t *
lh_web_xml_entity_get_link(const lh_web_xml_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->link);
}

lh_web_xml_kind_t
lh_web_xml_entity_get_kind(const lh_web_xml_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return self->kind;
}

lh_void
lh_web_xml_entity_set_kind(lh_web_xml_entity_t *self, lh_web_xml_kind_t kind)
{
    lh_assert_runtime_ref(self);
    self->kind = kind;
}

lh_str_t *
lh_web_xml_entity_get_name_mut(lh_web_xml_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->name);
}

const lh_str_t *
lh_web_xml_entity_get_name(const lh_web_xml_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->name);
}

lh_str_t *
lh_web_xml_entity_get_text_mut(lh_web_xml_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->text);
}

const lh_str_t *
lh_web_xml_entity_get_text(const lh_web_xml_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->text);
}

lh_list_t *
lh_web_xml_entity_get_attrs_mut(lh_web_xml_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->attrs);
}

const lh_list_t *
lh_web_xml_entity_get_attrs(const lh_web_xml_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->attrs);
}

lh_list_t *
lh_web_xml_entity_get_children_mut(lh_web_xml_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->children);
}

const lh_list_t *
lh_web_xml_entity_get_children(const lh_web_xml_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->children);
}
