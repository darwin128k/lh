/**
 * @file value.h
 * @brief A value of one of a few kinds (::lh_value_t): a tag plus storage.
 *
 * Same idea as PHP's `zval`: a small struct that says what it holds
 * (::lh_value_kind_t) and holds it. It owns nothing — text is an
 * ::lh_str_view_t and a pointer is just a pointer — so a value is copied by
 * plain assignment, needs no deinit, and whatever it points at must outlive
 * it (a document's text, say).
 *
 * Reading checks the kind: `lh_value_get_uint` on a value that is not a
 * uint is a runtime check failure. When the kind is not certain, ask first
 * (::lh_value_get_kind) or read with a fallback (`lh_value_get_uint_or`).
 */

#ifndef LH_VALUE_H
#define LH_VALUE_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/types.h>
#include <lh/ptr.h>
#include <lh/real.h>
#include <lh/str/view.h>
#include <lh/value/data.h>
#include <lh/value/fields.h>
#include <lh/value/kind.h>

/**
 * @struct lh_value
 * @brief Kind plus storage. Fields via ::lh_value_fields.
 */
typedef struct lh_value
{
    lh_value_fields(lh_value_kind_t, lh_value_data_t);
} lh_value_t;

LH_COMPILER_EXTERN_C_BEGIN

/* ── init / make ─────────────────────────────────────────────────────────── */

/**
 * @brief Set @p self to hold nothing (::lh_value_kind_none).
 */
void
lh_value_init(lh_value_t *self);

/**
 * @brief Copy @p other into @p self.
 */
void
lh_value_assign(lh_value_t *self, const lh_value_t *other);

/** @brief A value holding nothing. */
lh_value_t
lh_value_make_none(void);

/** @brief A value holding @p value. */
lh_value_t
lh_value_make_bool(lh_bool_t value);

/** @brief A value holding @p value. */
lh_value_t
lh_value_make_sint(lh_sllong_t value);

/** @brief A value holding @p value. */
lh_value_t
lh_value_make_uint(lh_ullong_t value);

#if LH_REAL_ENABLED
/** @brief A value holding @p value. Only with ::LH_REAL_ENABLED. */
lh_value_t
lh_value_make_real(lh_real_t value);
#endif

/** @brief A value viewing @p value. The text must outlive the value. */
lh_value_t
lh_value_make_str(lh_str_view_t value);

/** @brief A value holding @p value. What it points at must outlive the value. */
lh_value_t
lh_value_make_ptr(lh_ptr value);

/* ── kind ────────────────────────────────────────────────────────────────── */

/**
 * @brief What @p self holds.
 */
lh_value_kind_t
lh_value_get_kind(const lh_value_t *self);

/**
 * @brief True when @p self holds a @p kind.
 */
lh_bool_t
lh_value_has_kind(const lh_value_t *self, lh_value_kind_t kind);

/**
 * @brief True when @p self holds nothing.
 */
lh_bool_t
lh_value_is_none(const lh_value_t *self);

/* ── read (kind checked) ─────────────────────────────────────────────────── */

/** @brief The bool held. Not a bool: runtime check failure. */
lh_bool_t
lh_value_get_bool(const lh_value_t *self);

/** @brief The signed integer held. Not a sint: runtime check failure. */
lh_sllong_t
lh_value_get_sint(const lh_value_t *self);

/** @brief The unsigned integer held. Not a uint: runtime check failure. */
lh_ullong_t
lh_value_get_uint(const lh_value_t *self);

#if LH_REAL_ENABLED
/** @brief The floating point number held. Not a real: runtime check failure. */
lh_real_t
lh_value_get_real(const lh_value_t *self);
#endif

/** @brief The text viewed. Not a str: runtime check failure. */
lh_str_view_t
lh_value_get_str(const lh_value_t *self);

/** @brief The pointer held. Not a ptr: runtime check failure. */
lh_ptr
lh_value_get_ptr(const lh_value_t *self);

/* ── read with a fallback ────────────────────────────────────────────────── */

/** @brief The bool held, or @p fallback if @p self is not a bool. */
lh_bool_t
lh_value_get_bool_or(const lh_value_t *self, lh_bool_t fallback);

/** @brief The signed integer held, or @p fallback if @p self is not a sint. */
lh_sllong_t
lh_value_get_sint_or(const lh_value_t *self, lh_sllong_t fallback);

/** @brief The unsigned integer held, or @p fallback if @p self is not a uint. */
lh_ullong_t
lh_value_get_uint_or(const lh_value_t *self, lh_ullong_t fallback);

#if LH_REAL_ENABLED
/** @brief The floating point number held, or @p fallback if @p self is not a real. */
lh_real_t
lh_value_get_real_or(const lh_value_t *self, lh_real_t fallback);
#endif

/** @brief The text viewed, or @p fallback if @p self is not a str. */
lh_str_view_t
lh_value_get_str_or(const lh_value_t *self, lh_str_view_t fallback);

/** @brief The pointer held, or @p fallback if @p self is not a ptr. */
lh_ptr
lh_value_get_ptr_or(const lh_value_t *self, lh_ptr fallback);

/* ── compare ─────────────────────────────────────────────────────────────── */

/**
 * @brief True when @p self and @p other hold the same kind and equal content.
 *
 * Text compares by characters (case-sensitive), a pointer by address, a
 * real with `==` (so NaN never equals anything). A sint and a uint are
 * different kinds and never equal, whatever the numbers.
 */
lh_bool_t
lh_value_equals(const lh_value_t *self, const lh_value_t *other);

LH_COMPILER_EXTERN_C_END

#endif /* LH_VALUE_H */
