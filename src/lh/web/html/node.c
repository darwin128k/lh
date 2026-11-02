/**
 * @file node.c
 * @brief Implementation of `lh/web/html/node.h`.
 */

#include <lh/web/html/node.h>
#include <lh/assert.h>
#include <lh/runtime/allocator.h>

/* ── accessors ───────────────────────────────────────────────────────────── */

lh_list_node_t *
lh_web_html_node_get_link_mut(lh_web_html_node_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->link);
}

const lh_list_node_t *
lh_web_html_node_get_link(const lh_web_html_node_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->link);
}

lh_web_html_node_t *
lh_web_html_node_get_by_link_mut(lh_list_node_t *link)
{
    return lh_list_entry(lh_web_html_node_t, link, link);
}

const lh_web_html_node_t *
lh_web_html_node_get_by_link(const lh_list_node_t *link)
{
    return lh_list_entry(const lh_web_html_node_t, link, link);
}

lh_web_html_kind_t
lh_web_html_node_get_kind(const lh_web_html_node_t *self)
{
    lh_assert_runtime_ref(self);
    return self->kind;
}

lh_void
lh_web_html_node_set_kind(lh_web_html_node_t *self, lh_web_html_kind_t kind)
{
    lh_assert_runtime_ref(self);
    self->kind = kind;
}

lh_str_t *
lh_web_html_node_get_name_mut(lh_web_html_node_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->name);
}

const lh_str_t *
lh_web_html_node_get_name(const lh_web_html_node_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->name);
}

lh_str_t *
lh_web_html_node_get_text_mut(lh_web_html_node_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->text);
}

const lh_str_t *
lh_web_html_node_get_text(const lh_web_html_node_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->text);
}

lh_list_t *
lh_web_html_node_get_attrs_mut(lh_web_html_node_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->attrs);
}

const lh_list_t *
lh_web_html_node_get_attrs(const lh_web_html_node_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->attrs);
}

lh_list_t *
lh_web_html_node_get_children_mut(lh_web_html_node_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->children);
}

const lh_list_t *
lh_web_html_node_get_children(const lh_web_html_node_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->children);
}

lh_bool_t
lh_web_html_node_has_kind(const lh_web_html_node_t *self, lh_web_html_kind_t kind)
{
    return lh_cast_static(lh_bool_t, lh_math_eq(lh_web_html_node_get_kind(self), kind));
}

/* ── init / deinit ───────────────────────────────────────────────────────── */

lh_void
lh_web_html_node_init(lh_web_html_node_t *self)
{
    lh_list_node_init(lh_web_html_node_get_link_mut(self));
    lh_web_html_node_set_kind(self, lh_web_html_kind_none);
    lh_str_init(lh_web_html_node_get_name_mut(self));
    lh_str_init(lh_web_html_node_get_text_mut(self));
    lh_list_init(lh_web_html_node_get_attrs_mut(self));
    lh_list_init(lh_web_html_node_get_children_mut(self));
}

/* ── children, attributes and lifetime ───────────────────────────────────── */

lh_web_html_node_t *
lh_web_html_node_push_child(lh_web_html_node_t *self, lh_web_html_node_t *child)
{
    lh_list_push_back(lh_web_html_node_get_children_mut(self),
                      lh_web_html_node_get_link_mut(child));
    return self;
}

lh_web_html_node_t *
lh_web_html_node_push_attr(lh_web_html_node_t *self, lh_web_html_attr_t *attr)
{
    lh_list_push_back(lh_web_html_node_get_attrs_mut(self), lh_web_html_attr_get_link_mut(attr));
    return self;
}

const lh_web_html_node_t *
lh_web_html_node_get_first_child(const lh_web_html_node_t *self)
{
    return lh_web_html_node_get_by_link(lh_list_get_first(lh_web_html_node_get_children(self)));
}

const lh_web_html_node_t *
lh_web_html_node_get_next_child(const lh_web_html_node_t *self, const lh_web_html_node_t *child)
{
    return lh_web_html_node_get_by_link(
        lh_list_get_next(lh_web_html_node_get_children(self), lh_web_html_node_get_link(child)));
}

const lh_web_html_attr_t *
lh_web_html_node_get_first_attr(const lh_web_html_node_t *self)
{
    return lh_web_html_attr_get_by_link(lh_list_get_first(lh_web_html_node_get_attrs(self)));
}

const lh_web_html_attr_t *
lh_web_html_node_get_next_attr(const lh_web_html_node_t *self, const lh_web_html_attr_t *attr)
{
    return lh_web_html_attr_get_by_link(
        lh_list_get_next(lh_web_html_node_get_attrs(self), lh_web_html_attr_get_link(attr)));
}

lh_void
lh_web_html_node_clear(lh_web_html_node_t *self)
{
    lh_list_t *attrs = lh_web_html_node_get_attrs_mut(self);
    lh_list_t *children = lh_web_html_node_get_children_mut(self);
    lh_list_node_t *link;

    for (link = lh_list_pop_front(attrs); lh_null_ne(link); link = lh_list_pop_front(attrs))
    {
        lh_web_html_attr_destroy(lh_web_html_attr_get_by_link_mut(link));
    }
    for (link = lh_list_pop_front(children); lh_null_ne(link); link = lh_list_pop_front(children))
    {
        lh_web_html_node_destroy(lh_web_html_node_get_by_link_mut(link));
    }
    lh_str_clear(lh_web_html_node_get_name_mut(self));
    lh_str_clear(lh_web_html_node_get_text_mut(self));
    lh_web_html_node_set_kind(self, lh_web_html_kind_none);
}

lh_void
lh_web_html_node_deinit(lh_web_html_node_t *self)
{
    lh_web_html_node_clear(self);
    lh_str_deinit(lh_web_html_node_get_name_mut(self));
    lh_str_deinit(lh_web_html_node_get_text_mut(self));
}

lh_void
lh_web_html_node_destroy(lh_web_html_node_t *self)
{
    lh_list_node_unlink(lh_web_html_node_get_link_mut(self));
    lh_web_html_node_deinit(self);
    lh_runtime_allocator_free(self);
}

lh_web_html_node_t *
lh_web_html_node_create(lh_void)
{
    lh_web_html_node_t *self =
        lh_ptr_cast(lh_web_html_node_t, lh_runtime_allocator_alloc(sizeof(lh_web_html_node_t)));

    lh_web_html_node_init(self);
    return self;
}

lh_web_html_node_t *
lh_web_html_node_add_attr(lh_web_html_node_t *self, lh_str_view_t name, lh_str_view_t value)
{
    lh_web_html_attr_t *attr = lh_web_html_attr_create();

    lh_web_html_attr_set_name(attr, name);
    lh_web_html_attr_set_value(attr, value);
    return lh_web_html_node_push_attr(self, attr);
}

/* ── kinds ───────────────────────────────────────────────────────────────── */

lh_void
lh_web_html_node_set_doctype(lh_web_html_node_t *self)
{
    lh_web_html_node_clear(self);
    lh_web_html_node_set_kind(self, lh_web_html_kind_doctype);
}

lh_void
lh_web_html_node_set_element(lh_web_html_node_t *self, lh_str_view_t name)
{
    lh_web_html_node_clear(self);
    lh_web_html_node_set_kind(self, lh_web_html_kind_element);
    lh_str_assign_view(lh_web_html_node_get_name_mut(self), name);
}

lh_void
lh_web_html_node_set_void(lh_web_html_node_t *self, lh_str_view_t name)
{
    lh_web_html_node_clear(self);
    lh_web_html_node_set_kind(self, lh_web_html_kind_void);
    lh_str_assign_view(lh_web_html_node_get_name_mut(self), name);
}

lh_void
lh_web_html_node_set_text(lh_web_html_node_t *self, lh_str_view_t text)
{
    lh_web_html_node_clear(self);
    lh_web_html_node_set_kind(self, lh_web_html_kind_text);
    lh_str_assign_view(lh_web_html_node_get_text_mut(self), text);
}

lh_void
lh_web_html_node_set_raw(lh_web_html_node_t *self, lh_str_view_t text)
{
    lh_web_html_node_clear(self);
    lh_web_html_node_set_kind(self, lh_web_html_kind_raw);
    lh_str_assign_view(lh_web_html_node_get_text_mut(self), text);
}

/* ── composites ──────────────────────────────────────────────────────────── */

lh_web_html_node_t *
lh_web_html_node_create_doctype(lh_void)
{
    lh_web_html_node_t *self = lh_web_html_node_create();

    lh_web_html_node_set_doctype(self);
    return self;
}

lh_web_html_node_t *
lh_web_html_node_create_element(lh_str_view_t name)
{
    lh_web_html_node_t *self = lh_web_html_node_create();

    lh_web_html_node_set_element(self, name);
    return self;
}

lh_web_html_node_t *
lh_web_html_node_create_void(lh_str_view_t name)
{
    lh_web_html_node_t *self = lh_web_html_node_create();

    lh_web_html_node_set_void(self, name);
    return self;
}

lh_web_html_node_t *
lh_web_html_node_create_text(lh_str_view_t text)
{
    lh_web_html_node_t *self = lh_web_html_node_create();

    lh_web_html_node_set_text(self, text);
    return self;
}

lh_web_html_node_t *
lh_web_html_node_create_raw(lh_str_view_t text)
{
    lh_web_html_node_t *self = lh_web_html_node_create();

    lh_web_html_node_set_raw(self, text);
    return self;
}
