/**
 * @file werror.h
 * @brief Wide error value type (::lh_werror_t) and field accessors.
 *
 * Same shape as ::lh_error_t: a numeric ::lh_error_code_t with an optional
 * ::lh_werror_desc_t description (a ::lh_wstr_view_t). The view does not own
 * text. Layout is compatible with ::lh_error_t (code + ::lh_memory_view_t).
 *
 * @see lh_error_t
 * @see lh_wstr_view_t
 */

#ifndef LH_WERROR_H
#define LH_WERROR_H

#include <lh/attribute/symbol.h>
#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/error/code.h>
#include <lh/error/fields.h>
#include <lh/werror/desc.h>

/**
 * @struct lh_werror
 * @brief Error code and optional wide human-readable description.
 */
typedef struct lh_werror
{
    lh_error_fields(lh_error_code_t, lh_werror_desc_t);
} lh_werror_t; /**< Typedef for struct ::lh_werror. */

LH_COMPILER_EXTERN_C_BEGIN

/* ── set ─────────────────────────────────────────────────────────────────── */

/**
 * @brief Replace @p self with @p code and @p desc.
 *
 * @param self Wide error object to modify.
 * @param code New error code.
 * @param desc New description view (empty view = no description).
 */
LH_ATTRIBUTE_SYMBOL
void
lh_werror_set(lh_werror_t *self, lh_error_code_t code, lh_werror_desc_t desc);

/**
 * @brief Replace only the error code stored in @p self.
 *
 * @param self Wide error object to modify.
 * @param code New error code.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_werror_set_code(lh_werror_t *self, lh_error_code_t code);

/**
 * @brief Replace only the description view stored in @p self.
 *
 * @param self Wide error object to modify.
 * @param desc New description view (empty view = no description).
 */
LH_ATTRIBUTE_SYMBOL
void
lh_werror_set_desc(lh_werror_t *self, lh_werror_desc_t desc);

/* ── accessors ───────────────────────────────────────────────────────────── */

/**
 * @brief Return the error code stored in @p self.
 * @param self Wide error object to read from.
 * @return Current ::lh_error_code_t value.
 */
LH_ATTRIBUTE_SYMBOL
lh_error_code_t
lh_werror_get_code(const lh_werror_t *self);

/**
 * @brief Return the description view stored in @p self.
 * @param self Wide error object to read from.
 * @return Current @c desc (empty view when there is no description).
 */
LH_ATTRIBUTE_SYMBOL
lh_werror_desc_t
lh_werror_get_desc(const lh_werror_t *self);

/**
 * @brief Return the description view or @p fallback when it is empty.
 *
 * @param self Wide error object to read from.
 * @param fallback Description returned when @p self has no description.
 * @return Stored description when non-empty, otherwise @p fallback.
 */
LH_ATTRIBUTE_SYMBOL
lh_werror_desc_t
lh_werror_get_desc_or(const lh_werror_t *self, lh_werror_desc_t fallback);

/**
 * @brief Test whether @p self stores @p code.
 *
 * @param self Wide error object to read from.
 * @param code Error code to compare with.
 * @return ::lh_bool_true when @p self carries @p code, otherwise ::lh_bool_false.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_werror_has_code(const lh_werror_t *self, lh_error_code_t code);

/**
 * @brief Test whether @p self has the success code.
 *
 * @param self Wide error object to read from.
 * @return ::lh_bool_true when @p self stores ::lh_error_code_ok.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_werror_is_ok(const lh_werror_t *self);

/**
 * @brief Test whether @p self has a non-success code.
 *
 * @param self Wide error object to read from.
 * @return ::lh_bool_true when @p self does not store ::lh_error_code_ok.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_werror_is_failure(const lh_werror_t *self);

/**
 * @brief Test whether @p self has a non-empty description.
 *
 * Empty text, ::lh_wstr_view_make of an empty / null string, and an
 * uninitialized view all mean “no description”.
 *
 * @param self Wide error object to read from.
 * @return ::lh_bool_true when @p self stores a non-empty description view.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_werror_has_desc(const lh_werror_t *self);

/**
 * @brief Test whether @p self is the cleared success state.
 *
 * @param self Wide error object to read from.
 * @return ::lh_bool_true when @p self stores ::lh_error_code_ok and no description.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_werror_is_empty(const lh_werror_t *self);

/**
 * @brief Test whether two wide error objects store the same fields.
 *
 * Description equality is span identity (same begin and end pointers);
 * ::lh_werror_t does not own or copy description text.
 *
 * @param self Wide error object to read from.
 * @param other Wide error object to compare with.
 * @return ::lh_bool_true when both objects store the same code and description span.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_werror_equals(const lh_werror_t *self, const lh_werror_t *other);

/**
 * @brief Test whether two wide error objects store the same code.
 *
 * Description views are ignored.
 *
 * @param self Wide error object to read from.
 * @param other Wide error object to compare with.
 * @return ::lh_bool_true when both objects store the same code.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_werror_has_same_code(const lh_werror_t *self, const lh_werror_t *other);

/**
 * @brief Test whether two wide error objects store different codes.
 *
 * Description views are ignored.
 *
 * @param self Wide error object to read from.
 * @param other Wide error object to compare with.
 * @return ::lh_bool_true when both objects store different codes.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_werror_has_diff_code(const lh_werror_t *self, const lh_werror_t *other);

/* ── copy / clear ────────────────────────────────────────────────────────── */

/**
 * @brief Copy the error state from @p other into @p self.
 * @param self  Destination wide error object.
 * @param other Source wide error object to copy from.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_werror_assign(lh_werror_t *self, const lh_werror_t *other);

/**
 * @brief Reset @p self to an empty success state (::lh_error_code_ok, empty @c desc).
 *
 * Uses ::lh_werror_empty_initializer via ::lh_werror_assign.
 *
 * @param self Wide error object to clear.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_werror_clear(lh_werror_t *self);

/* ── init ────────────────────────────────────────────────────────────────── */

/**
 * @brief Initialize @p self with @p code and @p desc.
 *
 * @param self Wide error object to initialize.
 * @param code Initial error code.
 * @param desc Initial description view (empty view = no description).
 */
LH_ATTRIBUTE_SYMBOL
void
lh_werror_init(lh_werror_t *self, lh_error_code_t code, lh_werror_desc_t desc);

/**
 * @brief Initialize @p self by copying from @p other.
 * @param self  Wide error object to initialize.
 * @param other Source error to copy from.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_werror_init_by_other(lh_werror_t *self, const lh_werror_t *other);

/**
 * @brief Initialize @p self to an empty success state.
 *
 * Equivalent to ::lh_werror_clear.
 *
 * @param self Wide error object to initialize.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_werror_init_by_empty(lh_werror_t *self);

/**
 * @brief Return the error code, then clear @p self (::lh_werror_clear).
 *
 * @param self Wide error object to read and reset.
 * @return Code value before clearing.
 */
LH_ATTRIBUTE_SYMBOL
lh_error_code_t
lh_werror_get_code_and_clear(lh_werror_t *self);

/* ── make ────────────────────────────────────────────────────────────────── */

/**
 * @brief Construct an ::lh_werror_t with @p code and @p desc.
 *
 * @param code Error code (::lh_error_code_t).
 * @param desc Description view (empty view = no description).
 * @return Constructed ::lh_werror_t value.
 */
LH_ATTRIBUTE_SYMBOL
lh_werror_t
lh_werror_make(lh_error_code_t code, lh_werror_desc_t desc);

/**
 * @brief Construct an ::lh_werror_t with @p code and no description.
 *
 * @param code Error code (::lh_error_code_t).
 * @return Constructed ::lh_werror_t value.
 */
LH_ATTRIBUTE_SYMBOL
lh_werror_t
lh_werror_make_by_code(lh_error_code_t code);

LH_COMPILER_EXTERN_C_END

#endif /* LH_WERROR_H */
