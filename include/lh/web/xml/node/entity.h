/**
 * @file entity.h
 * @brief Data of one XML or HTML tree node (::lh_web_xml_entity_t).
 *
 * An entity holds its tree link, kind, strings, attribute list and child list.
 * The node API operates on the same object and owns its initialization and
 * cleanup. An initialized object must not be moved: its lists contain links
 * to their heads. Its strings and lists belong to it until node deinit.
 * This entity is a tree object, distinct from XML named character entities
 * such as `&amp;` in `lh/web/xml/entity.h`.
 */

#ifndef LH_WEB_XML_NODE_ENTITY_H
#define LH_WEB_XML_NODE_ENTITY_H

#include <lh/web/xml/attr.h>
#include <lh/web/xml/kind.h>
#include <lh/web/xml/node/fields.h>

/** @brief The data of an XML tree node. */
struct lh_web_xml_node
{
    /** Members: ::lh_web_xml_node_fields. */
    lh_web_xml_node_fields(lh_list_node_t, lh_web_xml_kind_t, lh_str_t, lh_list_t);
};
/** @copybrief lh_web_xml_node */
typedef struct lh_web_xml_node lh_web_xml_entity_t;

LH_COMPILER_EXTERN_C_BEGIN

/** Return the owned link of @p self for editing; @p self must be non-null. */
lh_list_node_t *
lh_web_xml_entity_get_link_mut(lh_web_xml_entity_t *self);

/** Return the owned link of @p self for reading; @p self must be non-null. */
const lh_list_node_t *
lh_web_xml_entity_get_link(const lh_web_xml_entity_t *self);

/** Return the stored kind of @p self. @p self must be non-null. */
lh_web_xml_kind_t
lh_web_xml_entity_get_kind(const lh_web_xml_entity_t *self);

/** Store @p kind without changing other fields. @p self must be non-null. */
lh_void
lh_web_xml_entity_set_kind(lh_web_xml_entity_t *self, lh_web_xml_kind_t kind);

/** Return the owned name of @p self for editing; @p self must be non-null. */
lh_str_t *
lh_web_xml_entity_get_name_mut(lh_web_xml_entity_t *self);

/** Return the owned name of @p self for reading; @p self must be non-null. */
const lh_str_t *
lh_web_xml_entity_get_name(const lh_web_xml_entity_t *self);

/** Return the owned text of @p self for editing; @p self must be non-null. */
lh_str_t *
lh_web_xml_entity_get_text_mut(lh_web_xml_entity_t *self);

/** Return the owned text of @p self for reading; @p self must be non-null. */
const lh_str_t *
lh_web_xml_entity_get_text(const lh_web_xml_entity_t *self);

/** Return the owned attrs of @p self for editing; @p self must be non-null. */
lh_list_t *
lh_web_xml_entity_get_attrs_mut(lh_web_xml_entity_t *self);

/** Return the owned attrs of @p self for reading; @p self must be non-null. */
const lh_list_t *
lh_web_xml_entity_get_attrs(const lh_web_xml_entity_t *self);

/** Return the owned children of @p self for editing; @p self must be non-null. */
lh_list_t *
lh_web_xml_entity_get_children_mut(lh_web_xml_entity_t *self);

/** Return the owned children of @p self for reading; @p self must be non-null. */
const lh_list_t *
lh_web_xml_entity_get_children(const lh_web_xml_entity_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_WEB_XML_NODE_ENTITY_H */
