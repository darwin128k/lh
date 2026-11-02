/**
 * @file html.h
 * @brief HTML markup entities: construct a tree, then serialize to a buf.
 *
 * Attributes live in ::lh/ui/web/html/attr.h. Serialize with
 * ::lh_ui_web_html_emit / ::lh_ui_web_html_open / ::lh_ui_web_html_close into a
 * ::lh_ui_web_buf_t — not by writing strings by hand.
 *
 * Requires ::LH_LIBRARY_OPTION_UI.
 */

#ifndef LH_UI_WEB_HTML_H
#define LH_UI_WEB_HTML_H

#include <lh/config.h>
#include <lh/ui/web/buf.h>
#include <lh/ui/web/html/attr.h>
#include <lh/ui/web/html/kind.h>

#if !LH_LIBRARY_OPTION_UI
#    error "lh/ui/web/html.h requires LH_LIBRARY_OPTION_UI"
#endif

/**
 * @brief One markup node.
 *
 * `tag.void_el` is for HTML void elements (`meta`, …) — no children, no close.
 * The content is a union: read only the member selected by the node kind,
 * after it has been written. Referenced strings and arrays remain caller-owned.
 */
typedef struct lh_ui_web_html
{
    lh_ui_web_html_kind_t kind; /**< Which member of @c u is set. */

    /** @brief The content of the node, by @c kind. */
    union
    {
        /** @brief Text of the node, for ::lh_ui_web_html_kind_text. */
        const lh_str_ptr text;
        /** @brief Markup of the node, for ::lh_ui_web_html_kind_raw. */
        const lh_str_ptr raw;

        /** @brief Element of the node, for ::lh_ui_web_html_kind_tag. */
        struct
        {
            /** @brief Tag name. */
            const lh_str_ptr name;
            /** @brief Attributes, in order. */
            const lh_ui_web_html_attr_t *attrs;
            /** @brief How many attributes @c attrs holds. */
            lh_u16_t nattrs;
            /** @brief Child nodes, in order. */
            const struct lh_ui_web_html *const *kids;
            /** @brief How many children @c kids holds. */
            lh_u16_t nkids;
            /** @brief Whether the element is void: no children, no close tag. */
            lh_bool_t void_el;
        } tag;
    } u;
} lh_ui_web_html_t; /**< One markup node. */

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Read the stored kind without validation.
 *
 * @param self Node whose storage remains caller-owned; must be valid.
 * @return The stored kind; referenced data remains caller-owned.
 */
lh_ui_web_html_kind_t
lh_ui_web_html_get_kind(const lh_ui_web_html_t *self);

/**
 * @brief Store the kind without validation or copying referenced data.
 *
 * @param self Valid caller-owned node to update.
 * @param value Value to store; referenced data must outlive its use.
 */
lh_void
lh_ui_web_html_set_kind(lh_ui_web_html_t *self, lh_ui_web_html_kind_t value);

/**
 * @brief Read the stored text without validation.
 *
 * @param self Node whose storage remains caller-owned; must be valid.
 * @return The stored text; referenced data remains caller-owned.
 */
const lh_str_ptr
lh_ui_web_html_get_text(const lh_ui_web_html_t *self);

/**
 * @brief Store the text without validation or copying referenced data.
 *
 * @param self Valid caller-owned node to update.
 * @param value Value to store; referenced data must outlive its use.
 */
lh_void
lh_ui_web_html_set_text(lh_ui_web_html_t *self, const lh_str_ptr value);

/**
 * @brief Read the stored raw without validation.
 *
 * @param self Node whose storage remains caller-owned; must be valid.
 * @return The stored raw; referenced data remains caller-owned.
 */
const lh_str_ptr
lh_ui_web_html_get_raw(const lh_ui_web_html_t *self);

/**
 * @brief Store the raw without validation or copying referenced data.
 *
 * @param self Valid caller-owned node to update.
 * @param value Value to store; referenced data must outlive its use.
 */
lh_void
lh_ui_web_html_set_raw(lh_ui_web_html_t *self, const lh_str_ptr value);

/**
 * @brief Read the stored tag name without validation.
 *
 * @param self Node whose storage remains caller-owned; must be valid.
 * @return The stored tag name; referenced data remains caller-owned.
 */
const lh_str_ptr
lh_ui_web_html_get_tag_name(const lh_ui_web_html_t *self);

/**
 * @brief Store the tag name without validation or copying referenced data.
 *
 * @param self Valid caller-owned node to update.
 * @param value Value to store; referenced data must outlive its use.
 */
lh_void
lh_ui_web_html_set_tag_name(lh_ui_web_html_t *self, const lh_str_ptr value);

/**
 * @brief Read the stored attribute array without validation.
 *
 * @param self Node whose storage remains caller-owned; must be valid.
 * @return The stored attribute array; referenced data remains caller-owned.
 */
const lh_ui_web_html_attr_t *
lh_ui_web_html_get_tag_attrs(const lh_ui_web_html_t *self);

/**
 * @brief Store the attribute array without validation or copying referenced data.
 *
 * @param self Valid caller-owned node to update.
 * @param value Value to store; referenced data must outlive its use.
 */
lh_void
lh_ui_web_html_set_tag_attrs(lh_ui_web_html_t *self, const lh_ui_web_html_attr_t *value);

/**
 * @brief Read the stored attribute count without validation.
 *
 * @param self Node whose storage remains caller-owned; must be valid.
 * @return The stored attribute count; referenced data remains caller-owned.
 */
lh_u16_t
lh_ui_web_html_get_tag_nattrs(const lh_ui_web_html_t *self);

/**
 * @brief Store the attribute count without validation or copying referenced data.
 *
 * @param self Valid caller-owned node to update.
 * @param value Value to store; referenced data must outlive its use.
 */
lh_void
lh_ui_web_html_set_tag_nattrs(lh_ui_web_html_t *self, lh_u16_t value);

/**
 * @brief Read the stored child array without validation.
 *
 * @param self Node whose storage remains caller-owned; must be valid.
 * @return The stored child array; referenced data remains caller-owned.
 */
const lh_ui_web_html_t *const *
lh_ui_web_html_get_tag_kids(const lh_ui_web_html_t *self);

/**
 * @brief Store the child array without validation or copying referenced data.
 *
 * @param self Valid caller-owned node to update.
 * @param value Value to store; referenced data must outlive its use.
 */
lh_void
lh_ui_web_html_set_tag_kids(lh_ui_web_html_t *self, const lh_ui_web_html_t *const *value);

/**
 * @brief Read the stored child count without validation.
 *
 * @param self Node whose storage remains caller-owned; must be valid.
 * @return The stored child count; referenced data remains caller-owned.
 */
lh_u16_t
lh_ui_web_html_get_tag_nkids(const lh_ui_web_html_t *self);

/**
 * @brief Store the child count without validation or copying referenced data.
 *
 * @param self Valid caller-owned node to update.
 * @param value Value to store; referenced data must outlive its use.
 */
lh_void
lh_ui_web_html_set_tag_nkids(lh_ui_web_html_t *self, lh_u16_t value);

/**
 * @brief Read the stored void-element flag without validation.
 *
 * @param self Node whose storage remains caller-owned; must be valid.
 * @return The stored void-element flag; referenced data remains caller-owned.
 */
lh_bool_t
lh_ui_web_html_get_tag_void_el(const lh_ui_web_html_t *self);

/**
 * @brief Store the void-element flag without validation or copying referenced data.
 *
 * @param self Valid caller-owned node to update.
 * @param value Value to store; referenced data must outlive its use.
 */
lh_void
lh_ui_web_html_set_tag_void_el(lh_ui_web_html_t *self, lh_bool_t value);

/**
 * @brief The entity that stands for @p c in HTML, or ::lh_null when @p c goes as is.
 *
 * Element text escapes `&`, `<` and `>`; an attribute value (@p in_attr) escapes
 * `&`, `<` and `"`.
 *
 * @param c      The character to look up.
 * @param in_attr Whether the character stands in an attribute value.
 * @return The entity text, or ::lh_null when @p c needs none.
 */
const lh_str_ptr
lh_ui_web_html_entity_of(lh_char_t c, lh_bool_t in_attr);

/**
 * @brief A doctype node, written as `<!doctype html>`.
 *
 * @param out Receives the node.
 */
lh_void
lh_ui_web_html_doctype(lh_ui_web_html_t *out);

/**
 * @brief A text node; `&`, `<` and `>` are escaped on emit.
 *
 * @param s   NUL-terminated text. Not copied.
 * @param out Receives the node.
 */
lh_void
lh_ui_web_html_text(const lh_str_ptr s, lh_ui_web_html_t *out);

/**
 * @brief A raw node, written as is.
 *
 * @param s   NUL-terminated text. Not copied.
 * @param out Receives the node.
 */
lh_void
lh_ui_web_html_raw(const lh_str_ptr s, lh_ui_web_html_t *out);

/**
 * @brief Element node.
 *
 * @param name    Tag name.
 * @param attrs   Attributes, in order; may be ::lh_null when @p nattrs is `0`.
 * @param nattrs  How many attributes @p attrs holds.
 * @param kids    Child nodes, in order; may be ::lh_null when @p nkids is `0`.
 * @param nkids   How many children @p kids holds.
 * @param void_el When true, emit `<name …>` with no children and no close tag.
 * @param out     Receives the node.
 */
lh_void
lh_ui_web_html_tag(const lh_str_ptr name, const lh_ui_web_html_attr_t *attrs, lh_u16_t nattrs,
                   const lh_ui_web_html_t *const *kids, lh_u16_t nkids, lh_bool_t void_el,
                   lh_ui_web_html_t *out);

/**
 * @brief Append NUL-terminated @p s with ::lh_ui_web_html_entity_of applied:
 *        as element text, or as an attribute value when @p in_attr.
 *
 * ::lh_null @p s appends nothing.
 *
 * @param out    Buffer to append to.
 * @param s      NUL-terminated text. Not copied.
 * @param in_attr Whether the text stands in an attribute value.
 */
lh_void
lh_ui_web_html_put_escaped(lh_ui_web_buf_t *out, const lh_str_ptr s, lh_bool_t in_attr);

/**
 * @brief Emit `</tag>`.
 *
 * A void element has no close tag and writes nothing. A node that is not an element
 * fails the buffer.
 *
 * @param out  Buffer to append to.
 * @param node Element node to close.
 */
lh_void
lh_ui_web_html_close(lh_ui_web_buf_t *out, const lh_ui_web_html_t *node);

/**
 * @brief Append @p n attributes as ` name="value"` (or bare ` name`), values escaped.
 *
 * @param out   Buffer to append to.
 * @param attrs Attributes, in order.
 * @param n     How many attributes @p attrs holds.
 */
lh_void
lh_ui_web_html_put_attrs(lh_ui_web_buf_t *out, const lh_ui_web_html_attr_t *attrs, lh_u16_t n);

/**
 * @brief Emit only `<tag attrs>` — for elements whose body is filled elsewhere (script).
 *
 * A node that is not an element fails the buffer.
 *
 * @param out  Buffer to append to.
 * @param node Element node to open.
 */
lh_void
lh_ui_web_html_open(lh_ui_web_buf_t *out, const lh_ui_web_html_t *node);

/**
 * @brief Serialize a full node (doctype / text / raw / element with kids).
 *
 * A null node fails the buffer; a null buffer is ignored.
 *
 * @param out  Buffer to append to.
 * @param node Node to serialize.
 */
lh_void
lh_ui_web_html_emit(lh_ui_web_buf_t *out, const lh_ui_web_html_t *node);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_WEB_HTML_H */
