/**
 * @file entry.h
 * @brief One meaningful line of an INI text (::lh_ini_entry_t).
 *
 * Either a section header or a key/value pair. Every entry carries the name
 * of the section it is in (a header carries its own name), so a pair can be
 * matched without remembering the last header seen.
 *
 * The key and value are held as an ::lh_pair_t of two str values. Like the
 * pair, an entry owns nothing: all its text points into the INI text it was
 * read from, which must outlive it.
 */

#ifndef LH_INI_ENTRY_H
#define LH_INI_ENTRY_H

#include <lh/ini/entry/fields.h>
#include <lh/ini/entry/kind.h>
#include <lh/pair.h>

/**
 * @struct lh_ini_entry
 * @brief Kind, section and key/value. Fields via ::lh_ini_entry_fields.
 */
struct lh_ini_entry
{
    /** Members: ::lh_ini_entry_fields. */
    lh_ini_entry_fields(lh_ini_entry_kind_t, lh_str_view_t, lh_pair_t);
};
/** @copybrief lh_ini_entry */
typedef struct lh_ini_entry lh_ini_entry_t;

/* ── accessors ───────────────────────────────────────────────────────────── */

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief What @p self is.
 */
lh_ini_entry_kind_t
lh_ini_entry_get_kind(const lh_ini_entry_t *self);

/** Make @p kind what @p self is. The section and pair are not touched. */
void
lh_ini_entry_set_kind(lh_ini_entry_t *self, lh_ini_entry_kind_t kind);

/**
 * @brief The section @p self is in; for a header, its own name.
 */
lh_str_view_t
lh_ini_entry_get_section(const lh_ini_entry_t *self);

/** Make @p section the section @p self is in. */
void
lh_ini_entry_set_section(lh_ini_entry_t *self, lh_str_view_t section);

/** Make @p pair the key and value of @p self, whatever its kind. */
void
lh_ini_entry_set_pair(lh_ini_entry_t *self, lh_pair_t pair);

/** The stored pair of @p self, whatever its kind; holds nothing unless a pair. */
const lh_pair_t *
lh_ini_entry_get_pair(const lh_ini_entry_t *self);

/**
 * @brief True when @p self is a @p kind.
 */
lh_bool_t
lh_ini_entry_has_kind(const lh_ini_entry_t *self, lh_ini_entry_kind_t kind);

/**
 * @brief The key (`first`) and value (`second`), both str values.
 *        Not a pair: runtime check failure.
 */
const lh_pair_t *
lh_ini_entry_get_pair_v(const lh_ini_entry_t *self);

/**
 * @brief The key. Not a pair: runtime check failure.
 */
lh_str_view_t
lh_ini_entry_get_key(const lh_ini_entry_t *self);

/**
 * @brief The value; empty for `key =`. Not a pair: runtime check failure.
 */
lh_str_view_t
lh_ini_entry_get_value(const lh_ini_entry_t *self);

/* ── init / make ─────────────────────────────────────────────────────────── */

/** Set the @p kind, @p section and @p pair of @p self in one go. */
void
lh_ini_entry_set(lh_ini_entry_t *self, lh_ini_entry_kind_t kind, lh_str_view_t section,
                 lh_pair_t pair);

/**
 * @brief Set @p self to no entry (::lh_ini_entry_kind_none): empty section,
 *        pair of nothing.
 */
void
lh_ini_entry_init(lh_ini_entry_t *self);

/**
 * @brief Copy @p other into @p self.
 */
void
lh_ini_entry_assign(lh_ini_entry_t *self, const lh_ini_entry_t *other);

/**
 * @brief The header of section @p name. Its pair holds nothing.
 */
lh_ini_entry_t
lh_ini_entry_make_section(lh_str_view_t name);

/**
 * @brief The pair @p key = @p value inside @p section.
 */
lh_ini_entry_t
lh_ini_entry_make_pair(lh_str_view_t section, lh_str_view_t key, lh_str_view_t value);

LH_COMPILER_EXTERN_C_END

#endif /* LH_INI_ENTRY_H */
