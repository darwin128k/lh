/**
 * @file error.h
 * @brief Error value type (::lh_error_t) and field accessors.
 *
 * An error bundles a numeric ::lh_error_code_t with an optional
 * NUL-terminated ::lh_error_desc_ptr description. The pointer does not own text.
 */

#ifndef LH_ERROR_H
#define LH_ERROR_H

#include <lh/void.h>
#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/error/code.h>
#include <lh/error/desc.h>
#include <lh/error/fields.h>

/**
 * @struct lh_error
 * @brief Error code and optional human-readable description.
 */
struct lh_error
{
    /** Members: ::lh_error_fields. */
    lh_error_fields(lh_error_code_t, const lh_error_desc_ptr);
};
/** @copybrief lh_error */
typedef struct lh_error lh_error_t;

LH_COMPILER_EXTERN_C_BEGIN

/* ── set ─────────────────────────────────────────────────────────────────── */

/**
 * @brief Replace @p self with @p code and @p desc.
 *
 * @param self Error object to modify.
 * @param code New error code.
 * @param desc New description (null or empty = no description).
 */
lh_void
lh_error_set(lh_error_t *self, lh_error_code_t code, const lh_error_desc_ptr desc);

/**
 * @brief Replace only the error code stored in @p self.
 *
 * @param self Error object to modify.
 * @param code New error code.
 */
lh_void
lh_error_set_code(lh_error_t *self, lh_error_code_t code);

/**
 * @brief Replace only the description stored in @p self.
 *
 * @param self Error object to modify.
 * @param desc New description (null or empty = no description).
 */
lh_void
lh_error_set_desc(lh_error_t *self, const lh_error_desc_ptr desc);

/* ── accessors ───────────────────────────────────────────────────────────── */

/**
 * @brief Return the error code stored in @p self.
 * @param self Error object to read from.
 * @return Current ::lh_error_code_t value.
 */
lh_error_code_t
lh_error_get_code(const lh_error_t *self);

/**
 * @brief Return the description stored in @p self.
 * @param self Error object to read from.
 * @return Current @c desc (null or empty when there is none).
 */
const lh_error_desc_ptr
lh_error_get_desc(const lh_error_t *self);

/**
 * @brief Return the description or @p fallback when it is null or empty.
 *
 * @param self Error object to read from.
 * @param fallback Description returned when @p self has no description.
 * @return Stored description when non-empty, otherwise @p fallback.
 */
const lh_error_desc_ptr
lh_error_get_desc_or(const lh_error_t *self, const lh_error_desc_ptr fallback);

/**
 * @brief Test whether @p self stores @p code.
 *
 * @param self Error object to read from.
 * @param code Error code to compare with.
 * @return ::lh_bool_true when @p self carries @p code, otherwise ::lh_bool_false.
 */
lh_bool_t
lh_error_has_code(const lh_error_t *self, lh_error_code_t code);

/**
 * @brief Test whether @p self has the success code.
 *
 * @param self Error object to read from.
 * @return ::lh_bool_true when @p self stores ::lh_error_code_ok.
 */
lh_bool_t
lh_error_is_ok(const lh_error_t *self);

/**
 * @brief Test whether @p self has a non-success code.
 *
 * @param self Error object to read from.
 * @return ::lh_bool_true when @p self does not store ::lh_error_code_ok.
 */
lh_bool_t
lh_error_is_failure(const lh_error_t *self);

/**
 * @brief Test whether @p self has a non-empty description.
 *
 * A null pointer and empty text both mean “no description”.
 *
 * @param self Error object to read from.
 * @return ::lh_bool_true when @p self stores a non-null, non-empty description.
 */
lh_bool_t
lh_error_has_desc(const lh_error_t *self);

/**
 * @brief Test whether @p self is the cleared success state.
 *
 * @param self Error object to read from.
 * @return ::lh_bool_true when @p self stores ::lh_error_code_ok and no description.
 */
lh_bool_t
lh_error_is_empty(const lh_error_t *self);

/**
 * @brief Test whether two error objects store the same fields.
 *
 * Description equality is pointer identity;
 * ::lh_error_t does not own or copy description text.
 *
 * @param self Error object to read from.
 * @param other Error object to compare with.
 * @return ::lh_bool_true when both objects store the same code and description pointer.
 */
lh_bool_t
lh_error_equals(const lh_error_t *self, const lh_error_t *other);

/**
 * @brief Test whether two error objects store the same code.
 *
 * Descriptions are ignored.
 *
 * @param self Error object to read from.
 * @param other Error object to compare with.
 * @return ::lh_bool_true when both objects store the same code.
 */
lh_bool_t
lh_error_has_same_code(const lh_error_t *self, const lh_error_t *other);

/**
 * @brief Test whether two error objects store different codes.
 *
 * Descriptions are ignored.
 *
 * @param self Error object to read from.
 * @param other Error object to compare with.
 * @return ::lh_bool_true when both objects store different codes.
 */
lh_bool_t
lh_error_has_diff_code(const lh_error_t *self, const lh_error_t *other);

/* ── copy / clear ────────────────────────────────────────────────────────── */

/**
 * @brief Copy the error state from @p other into @p self.
 * @param self  Destination error object.
 * @param other Source error object to copy from.
 */
lh_void
lh_error_assign(lh_error_t *self, const lh_error_t *other);

/**
 * @brief Reset @p self to an empty success state (::LH_ERROR_CODE_OK, empty @c desc).
 *
 * Uses ::lh_error_empty_initializer via ::lh_error_assign.
 *
 * @param self Error object to clear.
 */
lh_void
lh_error_clear(lh_error_t *self);

/* ── init ────────────────────────────────────────────────────────────────── */

/**
 * @brief Initialize @p self with @p code and @p desc.
 *
 * @param self Error object to initialize.
 * @param code Initial error code.
 * @param desc Initial description (null or empty = no description).
 */
lh_void
lh_error_init(lh_error_t *self, lh_error_code_t code, const lh_error_desc_ptr desc);

/**
 * @brief Initialize @p self by copying from @p other.
 * @param self  Error object to initialize.
 * @param other Source error to copy from.
 */
lh_void
lh_error_init_by_other(lh_error_t *self, const lh_error_t *other);

/**
 * @brief Initialize @p self to an empty success state.
 *
 * Equivalent to ::lh_error_clear.
 *
 * @param self Error object to initialize.
 */
lh_void
lh_error_init_by_empty(lh_error_t *self);

/**
 * @brief Initialize @p self with @p code and an empty description.
 */
lh_void
lh_error_init_by_code(lh_error_t *self, lh_error_code_t code);

/**
 * @brief Return the error code, then clear @p self (::lh_error_clear).
 *
 * @param self Error object to read and reset.
 * @return Code value before clearing.
 */
lh_error_code_t
lh_error_get_code_and_clear(lh_error_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_ERROR_H */
