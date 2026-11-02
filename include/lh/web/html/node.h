/**
 * @file node.h
 * @brief One HTML node and its children (::lh_web_html_node_t): the tree a
 *        page is built as before it is written out.
 *
 * A node is a doctype, an element, a void element, text or raw markup
 * (::lh_web_html_kind_t). An element owns its attributes
 * (::lh_web_html_attr_t) and its children, both in order, in lists threaded
 * through the attribute's and the child's own `link`; ::lh_web_html_node_deinit
 * frees them all, recursively, with the tag name and the text. Nothing in the
 * tree points at caller memory: every string is copied in.
 *
 * Text is kept as it is meant and escaped by the writer
 * (::lh_web_html_writer_try_write). Raw markup is written as it is: it is for
 * the body of `style` and `script`, which HTML does not unescape, and the
 * caller answers for it not to close its element early.
 *
 * A node links its children through a list head stored inside it, so it must
 * not be copied or moved once initialized: keep the root where it was
 * initialized, and let the tree allocate the rest (::lh_web_html_node_create
 * and the `create_*` composites).
 *
 * Example usage:
 * @code{.c}
 * lh_web_html_node_t *p = lh_web_html_node_create_element(lh_str_view_lit("p"));
 * lh_str_t out;
 * lh_web_html_writer_t writer;
 *
 * lh_web_html_node_add_attr(p, lh_str_view_lit("class"), lh_str_view_lit("note"));
 * lh_web_html_node_push_child(p, lh_web_html_node_create_text(lh_str_view_lit("a < b")));
 * lh_str_init(&out);
 * lh_web_html_writer_init(&writer, &out);
 * lh_web_html_writer_write(&writer, p);     // <p class="note">a &lt; b</p>
 * lh_web_html_node_destroy(p);
 * lh_str_deinit(&out);
 * @endcode
 */

#ifndef LH_WEB_HTML_NODE_H
#define LH_WEB_HTML_NODE_H

#include <lh/web/html/attr.h>
#include <lh/web/html/kind.h>
#include <lh/web/html/node/fields.h>

/**
 * @struct lh_web_html_node
 * @brief Kind, tag name, text, attributes and children. Fields via
 *        ::lh_web_html_node_fields.
 */
struct lh_web_html_node
{
    /** Members: ::lh_web_html_node_fields. */
    lh_web_html_node_fields(lh_list_node_t, lh_web_html_kind_t, lh_str_t, lh_list_t);
};
/** @copybrief lh_web_html_node */
typedef struct lh_web_html_node lh_web_html_node_t;

/* ── accessors ───────────────────────────────────────────────────────────── */

LH_COMPILER_EXTERN_C_BEGIN

/** The link that puts @p self in its parent's children, to change. */
lh_list_node_t *
lh_web_html_node_get_link_mut(lh_web_html_node_t *self);

/** The link that puts @p self in its parent's children. */
const lh_list_node_t *
lh_web_html_node_get_link(const lh_web_html_node_t *self);

/** The node whose link is @p link, to change. @p link must belong to one. */
lh_web_html_node_t *
lh_web_html_node_get_by_link_mut(lh_list_node_t *link);

/** The node whose link is @p link. @p link must belong to one. */
const lh_web_html_node_t *
lh_web_html_node_get_by_link(const lh_list_node_t *link);

/** What @p self is. */
lh_web_html_kind_t
lh_web_html_node_get_kind(const lh_web_html_node_t *self);

/** Make @p self a @p kind, touching nothing else. The `set_*` composites do it right. */
lh_void
lh_web_html_node_set_kind(lh_web_html_node_t *self, lh_web_html_kind_t kind);

/** The tag name of @p self, to change. */
lh_str_t *
lh_web_html_node_get_name_mut(lh_web_html_node_t *self);

/** The tag name of @p self; empty unless it is an element. */
const lh_str_t *
lh_web_html_node_get_name(const lh_web_html_node_t *self);

/** The text of @p self, to change. */
lh_str_t *
lh_web_html_node_get_text_mut(lh_web_html_node_t *self);

/** The text of @p self (unescaped); empty unless it is text or raw markup. */
const lh_str_t *
lh_web_html_node_get_text(const lh_web_html_node_t *self);

/** The attributes of @p self, to change. */
lh_list_t *
lh_web_html_node_get_attrs_mut(lh_web_html_node_t *self);

/** The attributes of @p self, in order. */
const lh_list_t *
lh_web_html_node_get_attrs(const lh_web_html_node_t *self);

/** The children of @p self, to change. */
lh_list_t *
lh_web_html_node_get_children_mut(lh_web_html_node_t *self);

/** The children of @p self, in order. */
const lh_list_t *
lh_web_html_node_get_children(const lh_web_html_node_t *self);

/**
 * @brief True when @p self is a @p kind.
 */
lh_bool_t
lh_web_html_node_has_kind(const lh_web_html_node_t *self, lh_web_html_kind_t kind);

/* ── init / deinit ───────────────────────────────────────────────────────── */

/**
 * @brief Set @p self to hold nothing (::lh_web_html_kind_none): no name, no
 *        text, no attributes, no children. Nothing is allocated.
 */
lh_void
lh_web_html_node_init(lh_web_html_node_t *self);

/* ── children, attributes and lifetime ───────────────────────────────────── */

/**
 * @brief Make @p child the last child of @p self. The tree owns it from then
 *        on: @p child must come from ::lh_web_html_node_create and belong to no
 *        other parent.
 *
 * @return @p self, so pushes chain while a document is built in one expression.
 */
lh_web_html_node_t *
lh_web_html_node_push_child(lh_web_html_node_t *self, lh_web_html_node_t *child);

/**
 * @brief Make @p attr the last attribute of @p self. The node owns it from then
 *        on: @p attr must come from ::lh_web_html_attr_create and belong to no
 *        other element.
 *
 * @return @p self, so pushes chain.
 */
lh_web_html_node_t *
lh_web_html_node_push_attr(lh_web_html_node_t *self, lh_web_html_attr_t *attr);

/** The first child of @p self; ::lh_null without children. */
const lh_web_html_node_t *
lh_web_html_node_get_first_child(const lh_web_html_node_t *self);

/** The child after @p child among the children of @p self; ::lh_null after the last. */
const lh_web_html_node_t *
lh_web_html_node_get_next_child(const lh_web_html_node_t *self, const lh_web_html_node_t *child);

/** The first attribute of @p self; ::lh_null without attributes. */
const lh_web_html_attr_t *
lh_web_html_node_get_first_attr(const lh_web_html_node_t *self);

/** The attribute after @p attr among the attributes of @p self; ::lh_null after the last. */
const lh_web_html_attr_t *
lh_web_html_node_get_next_attr(const lh_web_html_node_t *self, const lh_web_html_attr_t *attr);

/**
 * @brief Drop what @p self holds - its name, its text, every attribute and
 *        every child, recursively - and leave it holding nothing
 *        (::lh_web_html_kind_none). Its place in the parent stays.
 */
lh_void
lh_web_html_node_clear(lh_web_html_node_t *self);

/**
 * @brief Free what @p self holds: its name, its text, every attribute and every
 *        child, recursively. @p self itself stays where it is; init it to reuse it.
 */
lh_void
lh_web_html_node_deinit(lh_web_html_node_t *self);

/**
 * @brief A new node from the runtime allocator, initialized. Out of memory:
 *        runtime check failure, as for any lh allocation.
 */
lh_web_html_node_t *lh_web_html_node_create(lh_void);

/**
 * @brief Take @p self out of its parent's children, if it is in any, deinit it
 *        and give it back to the runtime allocator. Only for a node from
 *        ::lh_web_html_node_create: this is how a node and its subtree are deleted.
 */
lh_void
lh_web_html_node_destroy(lh_web_html_node_t *self);

/**
 * @brief Append the attribute `name="value"` to @p self: a new
 *        ::lh_web_html_attr_t with both copied, pushed with
 *        ::lh_web_html_node_push_attr.
 *
 * @return @p self, so attributes chain.
 */
lh_web_html_node_t *
lh_web_html_node_add_attr(lh_web_html_node_t *self, lh_str_view_t name, lh_str_view_t value);

/* ── kinds ───────────────────────────────────────────────────────────────── */

/** Clear @p self and make it the doctype, `<!doctype html>`. */
lh_void
lh_web_html_node_set_doctype(lh_web_html_node_t *self);

/**
 * @brief Clear @p self and make it an element named @p name, without
 *        attributes or children yet. The name is copied; the writer checks it
 *        (::lh_web_html_name_is_valid).
 */
lh_void
lh_web_html_node_set_element(lh_web_html_node_t *self, lh_str_view_t name);

/**
 * @brief Clear @p self and make it a void element named @p name (`meta`,
 *        `br`, ...): attributes, but no children and no close tag. The name is
 *        copied; the writer checks it and refuses a void element with children.
 */
lh_void
lh_web_html_node_set_void(lh_web_html_node_t *self, lh_str_view_t name);

/** Clear @p self and make it text: @p text, copied, escaped by the writer. */
lh_void
lh_web_html_node_set_text(lh_web_html_node_t *self, lh_str_view_t text);

/**
 * @brief Clear @p self and make it raw markup: @p text, copied, written as it
 *        is. Not checked: the caller answers for it.
 */
lh_void
lh_web_html_node_set_raw(lh_web_html_node_t *self, lh_str_view_t text);

/* ── composites ──────────────────────────────────────────────────────────── */

/** A new doctype node (::lh_web_html_node_create + ::lh_web_html_node_set_doctype). */
lh_web_html_node_t *lh_web_html_node_create_doctype(lh_void);

/** A new element named @p name (::lh_web_html_node_create + ::lh_web_html_node_set_element). */
lh_web_html_node_t *
lh_web_html_node_create_element(lh_str_view_t name);

/** A new void element named @p name (::lh_web_html_node_create + ::lh_web_html_node_set_void). */
lh_web_html_node_t *
lh_web_html_node_create_void(lh_str_view_t name);

/** A new text node of @p text (::lh_web_html_node_create + ::lh_web_html_node_set_text). */
lh_web_html_node_t *
lh_web_html_node_create_text(lh_str_view_t text);

/** A new raw node of @p text (::lh_web_html_node_create + ::lh_web_html_node_set_raw). */
lh_web_html_node_t *
lh_web_html_node_create_raw(lh_str_view_t text);

LH_COMPILER_EXTERN_C_END

#endif /* LH_WEB_HTML_NODE_H */
