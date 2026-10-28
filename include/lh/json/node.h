/**
 * @file node.h
 * @brief One JSON value and its children (::lh_json_node_t): the tree a
 *        JSON text is read into.
 *
 * The same tree as cJSON's: every value is a node, an array or an object
 * holds its children in order, and a member of an object carries its key.
 * Unlike the INI entries, a node owns what it holds — strings come out of
 * escapes, so they cannot point into the text — and frees it in
 * ::lh_json_node_deinit, children included.
 *
 * A number keeps the text it was written as. Nothing is lost on the way in
 * (a 64-bit id, `1e400`), and ::lh_json_node_get_sint /
 * ::lh_json_node_get_uint turn it into an integer when it is one and fits.
 *
 * A node links its children through a list head stored inside it, so it
 * must not be copied or moved once initialized: keep the root where it was
 * initialized, and let the tree allocate the rest (::lh_json_node_create).
 *
 * Example usage:
 * @code{.c}
 * lh_json_reader_t reader;
 * lh_json_node_t root;
 * lh_sllong_t asn;
 *
 * lh_json_node_init(&root);
 * lh_json_reader_init(&reader, text);
 * if (lh_json_reader_read(&reader, &root)) {
 *     lh_str_view_t key = lh_str_view_lit("asn");
 *     asn = lh_json_node_get_sint_or(lh_json_node_get_member(&root, key), 0);
 * }
 * lh_json_node_deinit(&root);
 * @endcode
 */

#ifndef LH_JSON_NODE_H
#define LH_JSON_NODE_H

#include <lh/json/kind.h>
#include <lh/json/node/fields.h>
#include <lh/list.h>
#include <lh/real.h>
#include <lh/str.h>

/**
 * @def LH_JSON_NODE_INTEGER_DIGITS_MAX
 * @brief Characters the longest 64-bit integer takes as text: a sign and
 *        20 digits (::lh_json_node_set_sint / ::lh_json_node_set_uint).
 */
#define LH_JSON_NODE_INTEGER_DIGITS_MAX 21U

/**
 * @struct lh_json_node
 * @brief Kind, value and children. Fields via ::lh_json_node_fields.
 */
struct lh_json_node
{
    /** Members: ::lh_json_node_fields. */
    lh_json_node_fields(lh_list_node_t, lh_json_kind_t, lh_bool_t, lh_str_t, lh_list_t);
};
/** @copybrief lh_json_node */
typedef struct lh_json_node lh_json_node_t;

/* ── accessors ───────────────────────────────────────────────────────────── */

LH_COMPILER_EXTERN_C_BEGIN

/** The link that puts @p self in its parent's children, to change. */
lh_list_node_t *
lh_json_node_get_link_mut(lh_json_node_t *self);

/** The link that puts @p self in its parent's children. */
const lh_list_node_t *
lh_json_node_get_link(const lh_json_node_t *self);

/** The node whose link is @p link, to change; ::lh_null for ::lh_null. */
lh_json_node_t *
lh_json_node_get_by_link_mut(lh_list_node_t *link);

/** The node whose link is @p link; ::lh_null for ::lh_null. */
const lh_json_node_t *
lh_json_node_get_by_link(const lh_list_node_t *link);

/**
 * @brief What @p self holds.
 */
lh_json_kind_t
lh_json_node_get_kind(const lh_json_node_t *self);

/** Make @p kind what @p self holds. The value and children are not touched. */
lh_void
lh_json_node_set_kind(lh_json_node_t *self, lh_json_kind_t kind);

/** The stored flag of @p self, whatever its kind; the value of a bool. */
lh_bool_t
lh_json_node_get_flag(const lh_json_node_t *self);

/** Make @p flag the stored flag of @p self. */
lh_void
lh_json_node_set_flag(lh_json_node_t *self, lh_bool_t flag);

/** The key of @p self inside its object, to change. */
lh_str_t *
lh_json_node_get_key_mut(lh_json_node_t *self);

/** The key of @p self inside its object; empty outside an object. */
const lh_str_t *
lh_json_node_get_key(const lh_json_node_t *self);

/** The text of @p self — a string's characters, a number as written — to change. */
lh_str_t *
lh_json_node_get_text_mut(lh_json_node_t *self);

/** The text of @p self: a string's characters, a number as written. */
const lh_str_t *
lh_json_node_get_text(const lh_json_node_t *self);

/** The children of @p self, to change. */
lh_list_t *
lh_json_node_get_children_mut(lh_json_node_t *self);

/** The children of @p self: elements of an array, members of an object. */
const lh_list_t *
lh_json_node_get_children(const lh_json_node_t *self);

/**
 * @brief True when @p self holds a @p kind.
 */
lh_bool_t
lh_json_node_has_kind(const lh_json_node_t *self, lh_json_kind_t kind);

/* ── init / deinit ───────────────────────────────────────────────────────── */

/**
 * @brief Set @p self to hold nothing (::lh_json_kind_none): no key, no text,
 *        no children.
 */
lh_void
lh_json_node_init(lh_json_node_t *self);

/**
 * @brief Drop the value of @p self — its text, its flag and every child —
 *        and leave it holding nothing (::lh_json_kind_none). Its key and its
 *        place in the parent stay.
 */
lh_void
lh_json_node_clear(lh_json_node_t *self);

/**
 * @brief Free what @p self holds: its key, its text and every child,
 *        recursively. @p self itself stays where it is; init it to reuse it.
 */
lh_void
lh_json_node_deinit(lh_json_node_t *self);

/**
 * @brief A new node from the runtime allocator, initialized. Out of memory:
 *        runtime check failure, as for any lh allocation.
 */
lh_json_node_t *
lh_json_node_create(void);

/**
 * @brief Take @p self out of its parent's children, if it is in any, deinit
 *        it and give it back to the runtime allocator. Only for a node from
 *        ::lh_json_node_create: this is how a child is deleted.
 */
lh_void
lh_json_node_destroy(lh_json_node_t *self);

/* ── children ────────────────────────────────────────────────────────────── */

/**
 * @brief Make @p child the last child of @p self. The tree owns it from then
 *        on: @p child must come from ::lh_json_node_create.
 */
lh_void
lh_json_node_push_back(lh_json_node_t *self, lh_json_node_t *child);

/** The first child of @p self, to change; ::lh_null without children. */
lh_json_node_t *
lh_json_node_get_first_mut(lh_json_node_t *self);

/** The first child of @p self; ::lh_null without children. */
const lh_json_node_t *
lh_json_node_get_first(const lh_json_node_t *self);

/** The child of @p self after @p child, to change; ::lh_null after the last. */
lh_json_node_t *
lh_json_node_get_next_mut(lh_json_node_t *self, lh_json_node_t *child);

/**
 * @brief The child of @p self after @p child; ::lh_null after the last.
 *
 * @code{.c}
 * for (const lh_json_node_t *c = lh_json_node_get_first(arr); c;
 *      c = lh_json_node_get_next(arr, c)) { ... }
 * @endcode
 */
const lh_json_node_t *
lh_json_node_get_next(const lh_json_node_t *self, const lh_json_node_t *child);

/** How many children @p self has. O(n): the list keeps no count. */
lh_usize_t
lh_json_node_get_size(const lh_json_node_t *self);

/** The child of @p self at @p index, to change; ::lh_null past the end. O(index). */
lh_json_node_t *
lh_json_node_get_at_mut(lh_json_node_t *self, lh_uindex_t index);

/** The child of @p self at @p index; ::lh_null past the end. O(index). */
const lh_json_node_t *
lh_json_node_get_at(const lh_json_node_t *self, lh_uindex_t index);

/** True when the key of @p self is @p key (case sensitive). */
lh_bool_t
lh_json_node_has_key(const lh_json_node_t *self, lh_str_view_t key);

/**
 * @brief The first member of object @p self whose key is @p key (case
 *        sensitive); ::lh_null when there is none or @p self is not an
 *        object. Duplicate keys are kept; the first one wins here.
 */
const lh_json_node_t *
lh_json_node_get_member(const lh_json_node_t *self, lh_str_view_t key);

/**
 * @brief The first member of object @p self whose key is @p key (case
 *        sensitive), to change; ::lh_null when there is none or @p self is
 *        not an object.
 */
lh_json_node_t *
lh_json_node_get_member_mut(lh_json_node_t *self, lh_str_view_t key);

/* ── read (kind checked) ─────────────────────────────────────────────────── */

/** @brief The bool held. Not a bool: runtime check failure. */
lh_bool_t
lh_json_node_get_bool(const lh_json_node_t *self);

/** @brief The string held, unescaped. Not a string: runtime check failure. */
lh_str_view_t
lh_json_node_get_str(const lh_json_node_t *self);

/**
 * @brief The number held as a signed integer, into @p out.
 *
 * @return ::lh_bool_false, with @p out untouched, when @p self is not a
 *         number, not written as an integer (`1.0`, `1e2`) or out of
 *         ::lh_sllong_t.
 */
lh_bool_t
lh_json_node_get_sint(const lh_json_node_t *self, lh_sllong_t *out);

/**
 * @brief The number held as an unsigned integer, into @p out.
 *
 * @return ::lh_bool_false, with @p out untouched, when @p self is not a
 *         number, not written as an integer, negative or out of
 *         ::lh_ullong_t.
 */
lh_bool_t
lh_json_node_get_uint(const lh_json_node_t *self, lh_ullong_t *out);

#if LH_REAL_ENABLED
/**
 * @brief The number held as the nearest ::lh_real_t, into @p out (see
 *        ::lh_str_ptr_parse_real). Only with ::LH_REAL_ENABLED.
 *
 * @return ::lh_bool_false, with @p out untouched, when @p self is not a
 *         number or the number is too big for ::lh_real_t (`1e400`).
 */
lh_bool_t
lh_json_node_get_real(const lh_json_node_t *self, lh_real_t *out);
#endif

/* ── read with a fallback ────────────────────────────────────────────────── */

/** @brief The bool held, or @p fallback if @p self is ::lh_null or not a bool. */
lh_bool_t
lh_json_node_get_bool_or(const lh_json_node_t *self, lh_bool_t fallback);

/** @brief The string held, or @p fallback if @p self is ::lh_null or not a string. */
lh_str_view_t
lh_json_node_get_str_or(const lh_json_node_t *self, lh_str_view_t fallback);

/**
 * @brief The signed integer held, or @p fallback if @p self is ::lh_null or
 *        ::lh_json_node_get_sint fails.
 */
lh_sllong_t
lh_json_node_get_sint_or(const lh_json_node_t *self, lh_sllong_t fallback);

/**
 * @brief The unsigned integer held, or @p fallback if @p self is ::lh_null or
 *        ::lh_json_node_get_uint fails.
 */
lh_ullong_t
lh_json_node_get_uint_or(const lh_json_node_t *self, lh_ullong_t fallback);

#if LH_REAL_ENABLED
/**
 * @brief The number held as an ::lh_real_t, or @p fallback if @p self is
 *        ::lh_null or ::lh_json_node_get_real fails.
 */
lh_real_t
lh_json_node_get_real_or(const lh_json_node_t *self, lh_real_t fallback);
#endif

/* ── write values ────────────────────────────────────────────────────────── */

/** Make @p self `null`. */
lh_void
lh_json_node_set_null(lh_json_node_t *self);

/** Make @p self the bool @p value. */
lh_void
lh_json_node_set_bool(lh_json_node_t *self, lh_bool_t value);

/** Make @p self the number @p value. */
lh_void
lh_json_node_set_sint(lh_json_node_t *self, lh_sllong_t value);

/** Make @p self the number @p value. */
lh_void
lh_json_node_set_uint(lh_json_node_t *self, lh_ullong_t value);

/**
 * @brief Make @p self the number written as @p text (`3.25`, `-1e-9`), kept
 *        as is.
 *
 * @return ::lh_bool_false, with @p self holding nothing, when @p text is not
 *         a JSON number — the same grammar ::lh_json_reader_read_number
 *         checks, so the tree never holds a number it could not print.
 */
lh_bool_t
lh_json_node_set_number(lh_json_node_t *self, lh_str_view_t text);

#if LH_REAL_ENABLED
/**
 * @brief Make @p self the number @p value, written as the shortest text that
 *        reads back as it (::lh_str_ptr_format_real): `0.1`, `1e+21`.
 *
 * @return ::lh_bool_false, with @p self holding nothing, for an infinity or
 *         a NaN: JSON has no way to write them.
 */
lh_bool_t
lh_json_node_set_real(lh_json_node_t *self, lh_real_t value);
#endif

/** Make @p self the string @p value (copied, any bytes). */
lh_void
lh_json_node_set_str(lh_json_node_t *self, lh_str_view_t value);

/** Make @p self an empty array. */
lh_void
lh_json_node_set_array(lh_json_node_t *self);

/** Make @p self an empty object. */
lh_void
lh_json_node_set_object(lh_json_node_t *self);

/** Make @p key the key of @p self (copied). Only an object's members are written with one. */
lh_void
lh_json_node_set_key(lh_json_node_t *self, lh_str_view_t key);

/* ── write children ──────────────────────────────────────────────────────── */

/**
 * @brief Put @p child right before @p at among the children of @p at's
 *        parent. The tree owns it from then on: @p child must come from
 *        ::lh_json_node_create and be in no parent's children.
 */
lh_void
lh_json_node_insert_before(lh_json_node_t *at, lh_json_node_t *child);

/** Give @p child the key @p key and make it the last member of object @p self. */
lh_void
lh_json_node_add_member(lh_json_node_t *self, lh_str_view_t key, lh_json_node_t *child);

/**
 * @brief Take @p self out of its parent's children and hand it back to the
 *        caller, who must destroy it or put it elsewhere. Nothing happens to
 *        a node in no parent.
 */
lh_void
lh_json_node_detach(lh_json_node_t *self);

/**
 * @brief Destroy the first member of object @p self whose key is @p key.
 *
 * @return ::lh_bool_false if there was none.
 */
lh_bool_t
lh_json_node_remove_member(lh_json_node_t *self, lh_str_view_t key);

/**
 * @brief Put @p with where @p self is in its parent, under the key of
 *        @p self, and destroy @p self.
 */
lh_void
lh_json_node_replace(lh_json_node_t *self, lh_json_node_t *with);

/**
 * @brief Make @p self a deep copy of the value of @p other: kind, text and
 *        every child with its key. The key of @p self stays its own.
 */
lh_void
lh_json_node_assign(lh_json_node_t *self, const lh_json_node_t *other);

/**
 * @brief True when @p self and @p other hold the same value.
 *
 * Strings compare by bytes, numbers by their text (`1` and `1.0` differ),
 * arrays element by element in order, objects by member: the same count,
 * and each key of @p self found in @p other with an equal value, in any
 * order. Keys of @p self and @p other themselves are not compared.
 */
lh_bool_t
lh_json_node_equals(const lh_json_node_t *self, const lh_json_node_t *other);

LH_COMPILER_EXTERN_C_END

#endif /* LH_JSON_NODE_H */
