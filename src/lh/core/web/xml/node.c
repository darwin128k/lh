/**
 * @file node.c
 * @brief Implementation of `lh/core/web/xml/node.h`.
 */

#include <lh/core/web/xml/node.h>
#include <lh/core/assert.h>
#include <lh/core/runtime/allocator.h>

/* ── accessors ───────────────────────────────────────────────────────────── */

lh_list_node_t *
lh_web_xml_node_get_link_mut(lh_web_xml_node_t *self)
{
    return lh_web_xml_entity_get_link_mut(self);
}

const lh_list_node_t *
lh_web_xml_node_get_link(const lh_web_xml_node_t *self)
{
    return lh_web_xml_entity_get_link(self);
}

lh_web_xml_node_t *
lh_web_xml_node_get_by_link_mut(lh_list_node_t *link)
{
    return lh_list_entry(lh_web_xml_node_t, link, link);
}

const lh_web_xml_node_t *
lh_web_xml_node_get_by_link(const lh_list_node_t *link)
{
    return lh_list_entry(const lh_web_xml_node_t, link, link);
}

lh_web_xml_kind_t
lh_web_xml_node_get_kind(const lh_web_xml_node_t *self)
{
    return lh_web_xml_entity_get_kind(self);
}

lh_void
lh_web_xml_node_set_kind(lh_web_xml_node_t *self, lh_web_xml_kind_t kind)
{
    lh_web_xml_entity_set_kind(self, kind);
}

lh_str_t *
lh_web_xml_node_get_name_mut(lh_web_xml_node_t *self)
{
    return lh_web_xml_entity_get_name_mut(self);
}

const lh_str_t *
lh_web_xml_node_get_name(const lh_web_xml_node_t *self)
{
    return lh_web_xml_entity_get_name(self);
}

lh_str_t *
lh_web_xml_node_get_text_mut(lh_web_xml_node_t *self)
{
    return lh_web_xml_entity_get_text_mut(self);
}

const lh_str_t *
lh_web_xml_node_get_text(const lh_web_xml_node_t *self)
{
    return lh_web_xml_entity_get_text(self);
}

lh_list_t *
lh_web_xml_node_get_attrs_mut(lh_web_xml_node_t *self)
{
    return lh_web_xml_entity_get_attrs_mut(self);
}

const lh_list_t *
lh_web_xml_node_get_attrs(const lh_web_xml_node_t *self)
{
    return lh_web_xml_entity_get_attrs(self);
}

lh_list_t *
lh_web_xml_node_get_children_mut(lh_web_xml_node_t *self)
{
    return lh_web_xml_entity_get_children_mut(self);
}

const lh_list_t *
lh_web_xml_node_get_children(const lh_web_xml_node_t *self)
{
    return lh_web_xml_entity_get_children(self);
}

lh_bool_t
lh_web_xml_node_has_kind(const lh_web_xml_node_t *self, lh_web_xml_kind_t kind)
{
    return lh_cast_static(lh_bool_t, lh_math_eq(lh_web_xml_node_get_kind(self), kind));
}

/* ── init / deinit ───────────────────────────────────────────────────────── */

lh_void
lh_web_xml_node_init(lh_web_xml_node_t *self)
{
    lh_list_node_init(lh_web_xml_node_get_link_mut(self));
    lh_web_xml_node_set_kind(self, lh_web_xml_kind_none);
    lh_str_init(lh_web_xml_node_get_name_mut(self));
    lh_str_init(lh_web_xml_node_get_text_mut(self));
    lh_list_init(lh_web_xml_node_get_attrs_mut(self));
    lh_list_init(lh_web_xml_node_get_children_mut(self));
}

/* ── children, attributes and lifetime ───────────────────────────────────── */

lh_web_xml_node_t *
lh_web_xml_node_push_child(lh_web_xml_node_t *self, lh_web_xml_node_t *child)
{
    lh_list_push_back(lh_web_xml_node_get_children_mut(self), lh_web_xml_node_get_link_mut(child));
    return self;
}

lh_web_xml_node_t *
lh_web_xml_node_push_attr(lh_web_xml_node_t *self, lh_web_xml_attr_t *attr)
{
    lh_list_push_back(lh_web_xml_node_get_attrs_mut(self), lh_web_xml_attr_get_link_mut(attr));
    return self;
}

const lh_web_xml_node_t *
lh_web_xml_node_get_first_child(const lh_web_xml_node_t *self)
{
    return lh_web_xml_node_get_by_link(lh_list_get_first(lh_web_xml_node_get_children(self)));
}

const lh_web_xml_node_t *
lh_web_xml_node_get_next_child(const lh_web_xml_node_t *self, const lh_web_xml_node_t *child)
{
    return lh_web_xml_node_get_by_link(
        lh_list_get_next(lh_web_xml_node_get_children(self), lh_web_xml_node_get_link(child)));
}

const lh_web_xml_attr_t *
lh_web_xml_node_get_first_attr(const lh_web_xml_node_t *self)
{
    return lh_web_xml_attr_get_by_link(lh_list_get_first(lh_web_xml_node_get_attrs(self)));
}

const lh_web_xml_attr_t *
lh_web_xml_node_get_next_attr(const lh_web_xml_node_t *self, const lh_web_xml_attr_t *attr)
{
    return lh_web_xml_attr_get_by_link(
        lh_list_get_next(lh_web_xml_node_get_attrs(self), lh_web_xml_attr_get_link(attr)));
}

const lh_web_xml_attr_t *
lh_web_xml_node_find_attr(const lh_web_xml_node_t *self, lh_str_view_t name)
{
    for (const lh_web_xml_attr_t *attr = lh_web_xml_node_get_first_attr(self); lh_null_ne(attr);
         attr = lh_web_xml_node_get_next_attr(self, attr))
    {
        lh_str_view_t own;

        lh_str_as_view(lh_web_xml_attr_get_name(attr), lh_addr_of(own));
        if (lh_str_view_equals(lh_addr_of(own), lh_addr_of(name), lh_bool_false))
        {
            return attr;
        }
    }
    return lh_null;
}

lh_void
lh_web_xml_node_clear(lh_web_xml_node_t *self)
{
    lh_list_t *attrs = lh_web_xml_node_get_attrs_mut(self);
    lh_list_t *children = lh_web_xml_node_get_children_mut(self);
    lh_list_node_t *link;

    for (link = lh_list_pop_front(attrs); lh_null_ne(link); link = lh_list_pop_front(attrs))
    {
        lh_web_xml_attr_destroy(lh_web_xml_attr_get_by_link_mut(link));
    }
    for (link = lh_list_pop_front(children); lh_null_ne(link); link = lh_list_pop_front(children))
    {
        lh_web_xml_node_destroy(lh_web_xml_node_get_by_link_mut(link));
    }
    lh_str_clear(lh_web_xml_node_get_name_mut(self));
    lh_str_clear(lh_web_xml_node_get_text_mut(self));
    lh_web_xml_node_set_kind(self, lh_web_xml_kind_none);
}

lh_void
lh_web_xml_node_deinit(lh_web_xml_node_t *self)
{
    lh_web_xml_node_clear(self);
    lh_str_deinit(lh_web_xml_node_get_name_mut(self));
    lh_str_deinit(lh_web_xml_node_get_text_mut(self));
}

lh_void
lh_web_xml_node_destroy(lh_web_xml_node_t *self)
{
    lh_list_node_unlink(lh_web_xml_node_get_link_mut(self));
    lh_web_xml_node_deinit(self);
    lh_runtime_allocator_free(self);
}

lh_web_xml_node_t *
lh_web_xml_node_create(lh_void)
{
    lh_web_xml_node_t *self =
        lh_ptr_cast(lh_web_xml_node_t, lh_runtime_allocator_alloc(sizeof(lh_web_xml_node_t)));

    lh_web_xml_node_init(self);
    return self;
}

lh_web_xml_node_t *
lh_web_xml_node_add_attr(lh_web_xml_node_t *self, lh_str_view_t name, lh_str_view_t value)
{
    lh_web_xml_attr_t *attr = lh_web_xml_attr_create();

    lh_web_xml_attr_set_name(attr, name);
    lh_web_xml_attr_set_value(attr, value);
    return lh_web_xml_node_push_attr(self, attr);
}

/* ── kinds ───────────────────────────────────────────────────────────────── */

lh_void
lh_web_xml_node_set_document(lh_web_xml_node_t *self)
{
    lh_web_xml_node_clear(self);
    lh_web_xml_node_set_kind(self, lh_web_xml_kind_document);
}

lh_void
lh_web_xml_node_set_element(lh_web_xml_node_t *self, lh_str_view_t name)
{
    lh_web_xml_node_clear(self);
    lh_web_xml_node_set_kind(self, lh_web_xml_kind_element);
    lh_str_assign_view(lh_web_xml_node_get_name_mut(self), name);
}

lh_void
lh_web_xml_node_set_empty(lh_web_xml_node_t *self, lh_str_view_t name)
{
    lh_web_xml_node_clear(self);
    lh_web_xml_node_set_kind(self, lh_web_xml_kind_empty);
    lh_str_assign_view(lh_web_xml_node_get_name_mut(self), name);
}

lh_void
lh_web_xml_node_set_text(lh_web_xml_node_t *self, lh_str_view_t text)
{
    lh_web_xml_node_clear(self);
    lh_web_xml_node_set_kind(self, lh_web_xml_kind_text);
    lh_str_assign_view(lh_web_xml_node_get_text_mut(self), text);
}

lh_void
lh_web_xml_node_set_raw(lh_web_xml_node_t *self, lh_str_view_t text)
{
    lh_web_xml_node_clear(self);
    lh_web_xml_node_set_kind(self, lh_web_xml_kind_raw);
    lh_str_assign_view(lh_web_xml_node_get_text_mut(self), text);
}

lh_void
lh_web_xml_node_set_comment(lh_web_xml_node_t *self, lh_str_view_t text)
{
    lh_web_xml_node_clear(self);
    lh_web_xml_node_set_kind(self, lh_web_xml_kind_comment);
    lh_str_assign_view(lh_web_xml_node_get_text_mut(self), text);
}

lh_void
lh_web_xml_node_set_cdata(lh_web_xml_node_t *self, lh_str_view_t text)
{
    lh_web_xml_node_clear(self);
    lh_web_xml_node_set_kind(self, lh_web_xml_kind_cdata);
    lh_str_assign_view(lh_web_xml_node_get_text_mut(self), text);
}

lh_void
lh_web_xml_node_set_pi(lh_web_xml_node_t *self, lh_str_view_t target, lh_str_view_t text)
{
    lh_web_xml_node_clear(self);
    lh_web_xml_node_set_kind(self, lh_web_xml_kind_pi);
    lh_str_assign_view(lh_web_xml_node_get_name_mut(self), target);
    lh_str_assign_view(lh_web_xml_node_get_text_mut(self), text);
}

lh_void
lh_web_xml_node_set_doctype(lh_web_xml_node_t *self, lh_str_view_t text)
{
    lh_web_xml_node_clear(self);
    lh_web_xml_node_set_kind(self, lh_web_xml_kind_doctype);
    lh_str_assign_view(lh_web_xml_node_get_text_mut(self), text);
}

/* ── composites ──────────────────────────────────────────────────────────── */

lh_web_xml_node_t *
lh_web_xml_node_create_document(lh_void)
{
    lh_web_xml_node_t *self = lh_web_xml_node_create();

    lh_web_xml_node_set_document(self);
    return self;
}

lh_web_xml_node_t *
lh_web_xml_node_create_element(lh_str_view_t name)
{
    lh_web_xml_node_t *self = lh_web_xml_node_create();

    lh_web_xml_node_set_element(self, name);
    return self;
}

lh_web_xml_node_t *
lh_web_xml_node_create_empty(lh_str_view_t name)
{
    lh_web_xml_node_t *self = lh_web_xml_node_create();

    lh_web_xml_node_set_empty(self, name);
    return self;
}

lh_web_xml_node_t *
lh_web_xml_node_create_text(lh_str_view_t text)
{
    lh_web_xml_node_t *self = lh_web_xml_node_create();

    lh_web_xml_node_set_text(self, text);
    return self;
}

lh_web_xml_node_t *
lh_web_xml_node_create_raw(lh_str_view_t text)
{
    lh_web_xml_node_t *self = lh_web_xml_node_create();

    lh_web_xml_node_set_raw(self, text);
    return self;
}

lh_web_xml_node_t *
lh_web_xml_node_create_comment(lh_str_view_t text)
{
    lh_web_xml_node_t *self = lh_web_xml_node_create();

    lh_web_xml_node_set_comment(self, text);
    return self;
}

lh_web_xml_node_t *
lh_web_xml_node_create_cdata(lh_str_view_t text)
{
    lh_web_xml_node_t *self = lh_web_xml_node_create();

    lh_web_xml_node_set_cdata(self, text);
    return self;
}

lh_web_xml_node_t *
lh_web_xml_node_create_pi(lh_str_view_t target, lh_str_view_t text)
{
    lh_web_xml_node_t *self = lh_web_xml_node_create();

    lh_web_xml_node_set_pi(self, target, text);
    return self;
}

lh_web_xml_node_t *
lh_web_xml_node_create_doctype(lh_str_view_t text)
{
    lh_web_xml_node_t *self = lh_web_xml_node_create();

    lh_web_xml_node_set_doctype(self, text);
    return self;
}
