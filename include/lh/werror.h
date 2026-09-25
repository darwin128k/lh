/**
 * @file werror.h
 * @brief Wide error value type (::lh_werror_t) and field accessors.
 *
 * ::lh_werror_t is binary-compatible with ::lh_error_t — both carry a numeric
 * code and a ::lh_memory_view_t description span in the same field order.
 * The description is interpreted as ::lh_wstr_view_t (::lh_wchar_t units).
 * All functions delegate to the corresponding ::lh_error_* counterparts.
 *
 * The wide-namespaced type documents intent: text is a wide view, not
 * ::lh_str_view_t. The view does not own text.
 *
 * @see lh_error_t
 * @see lh_wstr_view_t
 */

#ifndef LH_WERROR_H
#define LH_WERROR_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/error/code.h>
#include <lh/error/fields.h>
#include <lh/wstr/view.h>

/**
 * @struct lh_werror
 * Binary-compatible with ::lh_error_t; description field is a wide view.
 */
typedef struct lh_werror
{
    lh_error_fields(lh_error_code_t, lh_wstr_view_t);
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
void
lh_werror_set(lh_werror_t *self, lh_error_code_t code, lh_wstr_view_t desc);

/**
 * @brief Replace only the error code stored in @p self.
 *
 * @param self Wide error object to modify.
 * @param code New error code.
 */
void
lh_werror_set_code(lh_werror_t *self, lh_error_code_t code);

/**
 * @brief Replace only the description view stored in @p self.
 *
 * @param self Wide error object to modify.
 * @param desc New description view (empty view = no description).
 */
void
lh_werror_set_desc(lh_werror_t *self, lh_wstr_view_t desc);

/* ── accessors ───────────────────────────────────────────────────────────── */

/**
 * @brief Return the error code stored in @p self.
 * @param self Wide error object to read from.
 * @return Current ::lh_error_code_t value.
 */
lh_error_code_t
lh_werror_get_code(const lh_werror_t *self);

/**
 * @brief Return the description view stored in @p self.
 * @param self Wide error object to read from.
 * @return Current @c desc (empty view when there is no description).
 */
lh_wstr_view_t
lh_werror_get_desc(const lh_werror_t *self);

/**
 * @brief Return the description view or @p fallback when it is empty.
 *
 * @param self Wide error object to read from.
 * @param fallback Description returned when @p self has no description.
 * @return Stored description when non-empty, otherwise @p fallback.
 */
lh_wstr_view_t
lh_werror_get_desc_or(const lh_werror_t *self, lh_wstr_view_t fallback);

/**
 * @brief Test whether @p self stores @p code.
 *
 * @param self Wide error object to read from.
 * @param code Error code to compare with.
 * @return ::lh_bool_true when @p self carries @p code, otherwise ::lh_bool_false.
 */
lh_bool_t
lh_werror_has_code(const lh_werror_t *self, lh_error_code_t code);

/**
 * @brief Test whether @p self has the success code.
 *
 * @param self Wide error object to read from.
 * @return ::lh_bool_true when @p self stores ::lh_error_code_ok.
 */
lh_bool_t
lh_werror_is_ok(const lh_werror_t *self);

/**
 * @brief Test whether @p self has a non-success code.
 *
 * @param self Wide error object to read from.
 * @return ::lh_bool_true when @p self does not store ::lh_error_code_ok.
 */
lh_bool_t
lh_werror_is_failure(const lh_werror_t *self);

/**
 * @brief Test whether @p self has a non-empty description.
 *
 * Empty text, ::lh_wstr_view_empty, and an
 * uninitialized view all mean “no description”.
 *
 * @param self Wide error object to read from.
 * @return ::lh_bool_true when @p self stores a non-empty description view.
 */
lh_bool_t
lh_werror_has_desc(const lh_werror_t *self);

/**
 * @brief Test whether @p self is the cleared success state.
 *
 * @param self Wide error object to read from.
 * @return ::lh_bool_true when @p self stores ::lh_error_code_ok and no description.
 */
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
lh_bool_t
lh_werror_has_diff_code(const lh_werror_t *self, const lh_werror_t *other);

/* ── copy / clear ────────────────────────────────────────────────────────── */

/**
 * @brief Copy the error state from @p other into @p self.
 * @param self  Destination wide error object.
 * @param other Source wide error object to copy from.
 */
void
lh_werror_assign(lh_werror_t *self, const lh_werror_t *other);

/**
 * @brief Reset @p self to an empty success state (::lh_error_code_ok, empty @c desc).
 *
 * Uses ::lh_werror_empty_initializer via ::lh_werror_assign.
 *
 * @param self Wide error object to clear.
 */
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
void
lh_werror_init(lh_werror_t *self, lh_error_code_t code, lh_wstr_view_t desc);

/**
 * @brief Initialize @p self by copying from @p other.
 * @param self  Wide error object to initialize.
 * @param other Source error to copy from.
 */
void
lh_werror_init_by_other(lh_werror_t *self, const lh_werror_t *other);

/**
 * @brief Initialize @p self to an empty success state.
 *
 * Equivalent to ::lh_werror_clear.
 *
 * @param self Wide error object to initialize.
 */
void
lh_werror_init_by_empty(lh_werror_t *self);

/**
 * @brief Return the error code, then clear @p self (::lh_werror_clear).
 *
 * @param self Wide error object to read and reset.
 * @return Code value before clearing.
 */
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
lh_werror_t
lh_werror_make(lh_error_code_t code, lh_wstr_view_t desc);

/**
 * @brief Construct an ::lh_werror_t with @p code and no description.
 *
 * @param code Error code (::lh_error_code_t).
 * @return Constructed ::lh_werror_t value.
 */
lh_werror_t
lh_werror_make_by_code(lh_error_code_t code);

LH_COMPILER_EXTERN_C_END

#endif /* LH_WERROR_H */
