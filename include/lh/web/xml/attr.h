/**
 * @file attr.h
 * @brief One attribute of an element (::lh_web_xml_attr_t): a name and a
 *        value, or - for HTML - a bare name.
 *
 * An attribute owns its name and its value (two ::lh_str_t) and frees them in
 * ::lh_web_xml_attr_deinit. The value is kept as it is meant, unescaped: the
 * writer escapes it (::lh_web_xml_escape_append) on the way out and the
 * reader decodes it on the way in. A bare attribute
 * (::lh_web_xml_attr_set_bare) has no value at all: HTML writes it as its
 * name alone (`<input disabled>`), XML has no such thing and its writer
 * refuses it.
 *
 * An element holds its attributes in a list (::lh_web_xml_node_push_attr),
 * linked through the attribute's `link`, so an attribute must not be copied or
 * moved once initialized; let ::lh_web_xml_attr_create allocate it and the
 * element destroy it with the rest of the tree.
 *
 * Example usage:
 * @code{.c}
 * lh_web_xml_attr_t *lang = lh_web_xml_attr_create();
 *
 * lh_web_xml_attr_set_name(lang, lh_str_view_lit("xml:lang"));
 * lh_web_xml_attr_set_value(lang, lh_str_view_lit("ru"));
 * lh_web_xml_node_push_attr(root, lang);    // root owns it now
 * @endcode
 */

#ifndef LH_WEB_XML_ATTR_H
#define LH_WEB_XML_ATTR_H

#include <lh/web/xml/attr/fields.h>
#include <lh/list.h>
#include <lh/str.h>

/**
 * @struct lh_web_xml_attr
 * @brief Name, value and the bare flag. Fields via ::lh_web_xml_attr_fields.
 */
struct lh_web_xml_attr
{
    /** Members: ::lh_web_xml_attr_fields. */
    lh_web_xml_attr_fields(lh_list_node_t, lh_str_t, lh_bool_t);
};
/** @copybrief lh_web_xml_attr */
typedef struct lh_web_xml_attr lh_web_xml_attr_t;

/* ── accessors ───────────────────────────────────────────────────────────── */

LH_COMPILER_EXTERN_C_BEGIN

/** The link that puts @p self in its element's attributes, to change. */
lh_list_node_t *
lh_web_xml_attr_get_link_mut(lh_web_xml_attr_t *self);

/** The link that puts @p self in its element's attributes. */
const lh_list_node_t *
lh_web_xml_attr_get_link(const lh_web_xml_attr_t *self);

/** The attribute whose link is @p link, to change. @p link must belong to one. */
lh_web_xml_attr_t *
lh_web_xml_attr_get_by_link_mut(lh_list_node_t *link);

/** The attribute whose link is @p link. @p link must belong to one. */
const lh_web_xml_attr_t *
lh_web_xml_attr_get_by_link(const lh_list_node_t *link);

/** The name of @p self, to change. */
lh_str_t *
lh_web_xml_attr_get_name_mut(lh_web_xml_attr_t *self);

/** The name of @p self. */
const lh_str_t *
lh_web_xml_attr_get_name(const lh_web_xml_attr_t *self);

/** The value of @p self (unescaped), to change. */
lh_str_t *
lh_web_xml_attr_get_value_mut(lh_web_xml_attr_t *self);

/** The value of @p self (unescaped). */
const lh_str_t *
lh_web_xml_attr_get_value(const lh_web_xml_attr_t *self);

/** True when @p self is written as its name alone, without a value. */
lh_bool_t
lh_web_xml_attr_get_bare(const lh_web_xml_attr_t *self);

/** Make @p self bare (@p bare true: the name alone) or valued. */
lh_void
lh_web_xml_attr_set_bare(lh_web_xml_attr_t *self, lh_bool_t bare);

/* ── init / deinit ───────────────────────────────────────────────────────── */

/**
 * @brief Set @p self to an empty, valued attribute: no name, an empty value,
 *        not linked anywhere. Nothing is allocated.
 */
lh_void
lh_web_xml_attr_init(lh_web_xml_attr_t *self);

/**
 * @brief Free the name and the value of @p self. @p self itself stays where it
 *        is and keeps its place in the element's list; init it to reuse it.
 */
lh_void
lh_web_xml_attr_deinit(lh_web_xml_attr_t *self);

/* ── values and lifetime ─────────────────────────────────────────────────── */

/** Copy @p name into the name of @p self. The writer checks it (::lh_web_xml_name_is_valid). */
lh_void
lh_web_xml_attr_set_name(lh_web_xml_attr_t *self, lh_str_view_t name);

/**
 * @brief Copy @p value into the value of @p self, unescaped, and make @p self
 *        valued (not bare). The writer escapes it.
 */
lh_void
lh_web_xml_attr_set_value(lh_web_xml_attr_t *self, lh_str_view_t value);

/**
 * @brief A new attribute from the runtime allocator, initialized. Out of
 *        memory: runtime check failure, as for any lh allocation.
 */
lh_web_xml_attr_t *lh_web_xml_attr_create(lh_void);

/**
 * @brief Take @p self out of its element's attributes, if it is in any, deinit
 *        it and give it back to the runtime allocator. Only for an attribute
 *        from ::lh_web_xml_attr_create.
 */
lh_void
lh_web_xml_attr_destroy(lh_web_xml_attr_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_WEB_XML_ATTR_H */
