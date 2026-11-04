/**
 * @file node.h
 * @brief One XML node and its children (::lh_web_xml_node_t): the tree a
 *        document is built as before it is written out, or read into.
 *
 * ::lh_web_xml_node_t and ::lh_web_xml_entity_t name the same object:
 * the entity API exposes its fields and this API supplies tree operations
 * and lifetime. A node is a document, an element, an empty element, text, raw markup, a
 * comment, a CDATA section, a processing instruction or a doctype
 * (::lh_web_xml_kind_t). An element owns its attributes
 * (::lh_web_xml_attr_t) and its children, both in order, in lists threaded
 * through the attribute's and the child's own `link`; a document owns its
 * top-level nodes the same way. ::lh_web_xml_node_deinit frees them all,
 * recursively, with the names and the texts. Nothing in the tree points at
 * caller memory: every string is copied in.
 *
 * Text, attribute values and the bodies of comments, CDATA sections,
 * processing instructions and doctypes are kept as they are meant -
 * unescaped - and the writer (::lh_web_xml_writer_try_write) escapes or
 * checks them on the way out. Raw markup is written as it is, and the caller
 * answers for it.
 *
 * The tree is shared with HTML: `lh/core/web/html/` writes and reads the same
 * nodes with HTML's rules (void elements, bare attributes, raw-text
 * elements), so a page is an XML tree written by the HTML writer.
 *
 * A node links its children through a list head stored inside it, so it must
 * not be copied or moved once initialized: keep the root where it was
 * initialized, and let the tree allocate the rest (::lh_web_xml_node_create
 * and the `create_*` composites).
 *
 * Example usage:
 * @code{.c}
 * lh_web_xml_node_t *p = lh_web_xml_node_create_element(lh_str_view_lit("p"));
 * lh_str_t out;
 * lh_web_xml_writer_t writer;
 *
 * lh_web_xml_node_add_attr(p, lh_str_view_lit("class"), lh_str_view_lit("note"));
 * lh_web_xml_node_push_child(p, lh_web_xml_node_create_text(lh_str_view_lit("a < b")));
 * lh_str_init(&out);
 * lh_web_xml_writer_init(&writer, &out);
 * lh_web_xml_writer_write(&writer, p);     // <p class="note">a &lt; b</p>
 * lh_web_xml_node_destroy(p);
 * lh_str_deinit(&out);
 * @endcode
 */

#ifndef LH_WEB_XML_NODE_H
#define LH_WEB_XML_NODE_H

#include <lh/core/web/xml/node/entity.h>

/** @brief An XML or HTML entity with the node operations declared below. */
typedef lh_web_xml_entity_t lh_web_xml_node_t;

/* ── accessors ───────────────────────────────────────────────────────────── */

LH_COMPILER_EXTERN_C_BEGIN

/** The link that puts @p self in its parent's children, to change. */
lh_list_node_t *
lh_web_xml_node_get_link_mut(lh_web_xml_node_t *self);

/** The link that puts @p self in its parent's children. */
const lh_list_node_t *
lh_web_xml_node_get_link(const lh_web_xml_node_t *self);

/** The node whose link is @p link, to change. @p link must belong to one. */
lh_web_xml_node_t *
lh_web_xml_node_get_by_link_mut(lh_list_node_t *link);

/** The node whose link is @p link. @p link must belong to one. */
const lh_web_xml_node_t *
lh_web_xml_node_get_by_link(const lh_list_node_t *link);

/** What @p self is. */
lh_web_xml_kind_t
lh_web_xml_node_get_kind(const lh_web_xml_node_t *self);

/** Make @p self a @p kind, touching nothing else. The `set_*` composites do it right. */
lh_void
lh_web_xml_node_set_kind(lh_web_xml_node_t *self, lh_web_xml_kind_t kind);

/** The name (element) or target (processing instruction) of @p self, to change. */
lh_str_t *
lh_web_xml_node_get_name_mut(lh_web_xml_node_t *self);

/** The name of @p self; empty unless it is an element or a processing instruction. */
const lh_str_t *
lh_web_xml_node_get_name(const lh_web_xml_node_t *self);

/** The text of @p self, to change. */
lh_str_t *
lh_web_xml_node_get_text_mut(lh_web_xml_node_t *self);

/** The text of @p self (unescaped); empty for a document and an element. */
const lh_str_t *
lh_web_xml_node_get_text(const lh_web_xml_node_t *self);

/** The attributes of @p self, to change. */
lh_list_t *
lh_web_xml_node_get_attrs_mut(lh_web_xml_node_t *self);

/** The attributes of @p self, in order. */
const lh_list_t *
lh_web_xml_node_get_attrs(const lh_web_xml_node_t *self);

/** The children of @p self, to change. */
lh_list_t *
lh_web_xml_node_get_children_mut(lh_web_xml_node_t *self);

/** The children of @p self, in order. */
const lh_list_t *
lh_web_xml_node_get_children(const lh_web_xml_node_t *self);

/**
 * @brief True when @p self is a @p kind.
 */
lh_bool_t
lh_web_xml_node_has_kind(const lh_web_xml_node_t *self, lh_web_xml_kind_t kind);

/* ── init / deinit ───────────────────────────────────────────────────────── */

/**
 * @brief Set @p self to hold nothing (::lh_web_xml_kind_none): no name, no
 *        text, no attributes, no children. Nothing is allocated.
 */
lh_void
lh_web_xml_node_init(lh_web_xml_node_t *self);

/* ── children, attributes and lifetime ───────────────────────────────────── */

/**
 * @brief Make @p child the last child of @p self. The tree owns it from then
 *        on: @p child must come from ::lh_web_xml_node_create and belong to no
 *        other parent.
 *
 * @return @p self, so pushes chain while a document is built in one expression.
 */
lh_web_xml_node_t *
lh_web_xml_node_push_child(lh_web_xml_node_t *self, lh_web_xml_node_t *child);

/**
 * @brief Make @p attr the last attribute of @p self. The node owns it from then
 *        on: @p attr must come from ::lh_web_xml_attr_create and belong to no
 *        other element.
 *
 * @return @p self, so pushes chain.
 */
lh_web_xml_node_t *
lh_web_xml_node_push_attr(lh_web_xml_node_t *self, lh_web_xml_attr_t *attr);

/** The first child of @p self; ::lh_null without children. */
const lh_web_xml_node_t *
lh_web_xml_node_get_first_child(const lh_web_xml_node_t *self);

/** The child after @p child among the children of @p self; ::lh_null after the last. */
const lh_web_xml_node_t *
lh_web_xml_node_get_next_child(const lh_web_xml_node_t *self, const lh_web_xml_node_t *child);

/** The first attribute of @p self; ::lh_null without attributes. */
const lh_web_xml_attr_t *
lh_web_xml_node_get_first_attr(const lh_web_xml_node_t *self);

/** The attribute after @p attr among the attributes of @p self; ::lh_null after the last. */
const lh_web_xml_attr_t *
lh_web_xml_node_get_next_attr(const lh_web_xml_node_t *self, const lh_web_xml_attr_t *attr);

/**
 * @brief The first attribute of @p self named exactly @p name; ::lh_null when
 *        it has none.
 */
const lh_web_xml_attr_t *
lh_web_xml_node_find_attr(const lh_web_xml_node_t *self, lh_str_view_t name);

/**
 * @brief Drop what @p self holds - its name, its text, every attribute and
 *        every child, recursively - and leave it holding nothing
 *        (::lh_web_xml_kind_none). Its place in the parent stays.
 */
lh_void
lh_web_xml_node_clear(lh_web_xml_node_t *self);

/**
 * @brief Free what @p self holds: its name, its text, every attribute and every
 *        child, recursively. @p self itself stays where it is; init it to reuse it.
 */
lh_void
lh_web_xml_node_deinit(lh_web_xml_node_t *self);

/**
 * @brief A new node from the runtime allocator, initialized. Out of memory:
 *        runtime check failure, as for any lh allocation.
 */
lh_web_xml_node_t *lh_web_xml_node_create(lh_void);

/**
 * @brief Take @p self out of its parent's children, if it is in any, deinit it
 *        and give it back to the runtime allocator. Only for a node from
 *        ::lh_web_xml_node_create: this is how a node and its subtree are deleted.
 */
lh_void
lh_web_xml_node_destroy(lh_web_xml_node_t *self);

/**
 * @brief Append the attribute `name="value"` to @p self: a new
 *        ::lh_web_xml_attr_t with both copied, pushed with
 *        ::lh_web_xml_node_push_attr.
 *
 * @return @p self, so attributes chain.
 */
lh_web_xml_node_t *
lh_web_xml_node_add_attr(lh_web_xml_node_t *self, lh_str_view_t name, lh_str_view_t value);

/* ── kinds ───────────────────────────────────────────────────────────────── */

/** Clear @p self and make it a document, without children yet. */
lh_void
lh_web_xml_node_set_document(lh_web_xml_node_t *self);

/**
 * @brief Clear @p self and make it an element named @p name, without
 *        attributes or children yet. The name is copied; the writer checks it
 *        (::lh_web_xml_name_is_valid).
 */
lh_void
lh_web_xml_node_set_element(lh_web_xml_node_t *self, lh_str_view_t name);

/**
 * @brief Clear @p self and make it an empty element named @p name: attributes,
 *        but no children and no end tag (`<br/>`, or the HTML void `<br>`).
 *        The name is copied; the writer checks it and refuses an empty
 *        element with children.
 */
lh_void
lh_web_xml_node_set_empty(lh_web_xml_node_t *self, lh_str_view_t name);

/** Clear @p self and make it text: @p text, copied, escaped by the writer. */
lh_void
lh_web_xml_node_set_text(lh_web_xml_node_t *self, lh_str_view_t text);

/**
 * @brief Clear @p self and make it raw markup: @p text, copied, written as it
 *        is. Not checked: the caller answers for it.
 */
lh_void
lh_web_xml_node_set_raw(lh_web_xml_node_t *self, lh_str_view_t text);

/**
 * @brief Clear @p self and make it a comment of @p text, copied. The writer
 *        refuses a text with `--` in it or a `-` at its end, which would end
 *        the comment early.
 */
lh_void
lh_web_xml_node_set_comment(lh_web_xml_node_t *self, lh_str_view_t text);

/**
 * @brief Clear @p self and make it a CDATA section of @p text, copied. The
 *        writer refuses a text with `]]>` in it.
 */
lh_void
lh_web_xml_node_set_cdata(lh_web_xml_node_t *self, lh_str_view_t text);

/**
 * @brief Clear @p self and make it the processing instruction
 *        `<?target text?>`, both copied. The writer checks the target
 *        (::lh_web_xml_name_is_valid) and refuses a text with `?>` in it.
 */
lh_void
lh_web_xml_node_set_pi(lh_web_xml_node_t *self, lh_str_view_t target, lh_str_view_t text);

/**
 * @brief Clear @p self and make it the doctype `<!DOCTYPE text>` (`html` for
 *        a page), @p text copied and written as it is.
 */
lh_void
lh_web_xml_node_set_doctype(lh_web_xml_node_t *self, lh_str_view_t text);

/* ── composites ──────────────────────────────────────────────────────────── */

/** A new document (::lh_web_xml_node_create + ::lh_web_xml_node_set_document). */
lh_web_xml_node_t *lh_web_xml_node_create_document(lh_void);

/** A new element named @p name (::lh_web_xml_node_create + ::lh_web_xml_node_set_element). */
lh_web_xml_node_t *
lh_web_xml_node_create_element(lh_str_view_t name);

/** A new empty element named @p name (::lh_web_xml_node_create + ::lh_web_xml_node_set_empty). */
lh_web_xml_node_t *
lh_web_xml_node_create_empty(lh_str_view_t name);

/** A new text node of @p text (::lh_web_xml_node_create + ::lh_web_xml_node_set_text). */
lh_web_xml_node_t *
lh_web_xml_node_create_text(lh_str_view_t text);

/** A new raw node of @p text (::lh_web_xml_node_create + ::lh_web_xml_node_set_raw). */
lh_web_xml_node_t *
lh_web_xml_node_create_raw(lh_str_view_t text);

/** A new comment of @p text (::lh_web_xml_node_create + ::lh_web_xml_node_set_comment). */
lh_web_xml_node_t *
lh_web_xml_node_create_comment(lh_str_view_t text);

/** A new CDATA section of @p text (::lh_web_xml_node_create + ::lh_web_xml_node_set_cdata). */
lh_web_xml_node_t *
lh_web_xml_node_create_cdata(lh_str_view_t text);

/**
 * @brief A new processing instruction (::lh_web_xml_node_create +
 *        ::lh_web_xml_node_set_pi).
 */
lh_web_xml_node_t *
lh_web_xml_node_create_pi(lh_str_view_t target, lh_str_view_t text);

/** A new doctype of @p text (::lh_web_xml_node_create + ::lh_web_xml_node_set_doctype). */
lh_web_xml_node_t *
lh_web_xml_node_create_doctype(lh_str_view_t text);

LH_COMPILER_EXTERN_C_END

#endif /* LH_WEB_XML_NODE_H */
