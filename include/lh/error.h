/**
 * @file error.h
 * @brief Error value type (::lh_error_t) and pack/unpack API.
 *
 * An error bundles a numeric ::lh_error_code_t with an optional C string description.
 * The pack/unpack API supports optional field pointers, so callers can update/read
 * code and description together or independently.
 */

#ifndef LH_ERROR_H
#define LH_ERROR_H

#include <lh/attribute/symbol.h>
#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/error/code.h>
#include <lh/error/desc.h>
#include <lh/error/fields.h>

/**
 * @struct lh_error
 * @brief Error code and optional human-readable description.
 */
typedef struct lh_error
{
    lh_error_fields(lh_error_code_t, lh_error_desc_t);
} lh_error_t;             /**< Typedef for struct ::lh_error. */

LH_COMPILER_EXTERN_C_BEGIN

/* ── pack / unpack ───────────────────────────────────────────────────────── */

/**
 * @brief Update fields on @p self from optional input pointers.
 *
 * @param self Error object to modify.
 * @param code Input pointer for new error code, or ::lh_null to keep current value.
 * @param desc Input pointer for new description pointer, or ::lh_null to keep current value.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_error_pack(lh_error_t *self, const lh_error_code_t *code, lh_error_desc_t *desc);

/**
 * @brief Read fields from @p self into optional output pointers.
 *
 * Pass ::lh_null for any pointer to skip that field.
 *
 * @param self  Error object to read from.
 * @param code  Output for @c code, or ::lh_null to skip.
 * @param desc  Output for @c desc (::lh_error_desc_t *), or ::lh_null to skip.
 *
 * Example usage:
 * @code{.c}
 * lh_error_code_t code;
 * lh_error_unpack(&err, &code, lh_null);
 * @endcode
 */
LH_ATTRIBUTE_SYMBOL
void
lh_error_unpack(const lh_error_t *self, lh_error_code_t *code, lh_error_desc_t *desc);

/**
 * @brief Unpack @p self into @p other (alias for ::lh_error_assign).
 * @param self  Source error (not null).
 * @param other Destination error (not null).
 */
LH_ATTRIBUTE_SYMBOL
void
lh_error_unpack_to_other(const lh_error_t *self, lh_error_t *other);

/**
 * @brief Replace @p self with @p code and @p desc.
 *
 * Equivalent to ::lh_error_pack with both fields provided.
 *
 * @param self Error object to modify.
 * @param code New error code.
 * @param desc New description pointer (may be null).
 */
LH_ATTRIBUTE_SYMBOL
void
lh_error_set(lh_error_t *self, lh_error_code_t code, lh_error_desc_t desc);

/**
 * @brief Replace only the error code stored in @p self.
 *
 * Equivalent to ::lh_error_pack with @p code provided and @c desc skipped.
 *
 * @param self Error object to modify.
 * @param code New error code.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_error_set_code(lh_error_t *self, lh_error_code_t code);

/**
 * @brief Replace only the description pointer stored in @p self.
 *
 * Equivalent to ::lh_error_pack with @c code skipped and @p desc provided.
 *
 * @param self Error object to modify.
 * @param desc New description pointer (may be null).
 */
LH_ATTRIBUTE_SYMBOL
void
lh_error_set_desc(lh_error_t *self, lh_error_desc_t desc);

/* ── accessors ───────────────────────────────────────────────────────────── */

/**
 * @brief Return the error code stored in @p self.
 * @param self Error object to read from.
 * @return Current ::lh_error_code_t value.
 */
LH_ATTRIBUTE_SYMBOL
lh_error_code_t
lh_error_get_code(const lh_error_t *self);

/**
 * @brief Return the description pointer stored in @p self.
 * @param self Error object to read from.
 * @return Current @c desc (may be null).
 */
LH_ATTRIBUTE_SYMBOL
lh_error_desc_t
lh_error_get_desc(const lh_error_t *self);

/**
 * @brief Return the description pointer or @p fallback when it is null.
 *
 * @param self Error object to read from.
 * @param fallback Description returned when @p self has no description.
 * @return Stored description when non-null, otherwise @p fallback.
 */
LH_ATTRIBUTE_SYMBOL
lh_error_desc_t
lh_error_get_desc_or(const lh_error_t *self, lh_error_desc_t fallback);

/**
 * @brief Test whether @p self stores @p code.
 *
 * @param self Error object to read from.
 * @param code Error code to compare with.
 * @return ::lh_bool_true when @p self carries @p code, otherwise ::lh_bool_false.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_error_has_code(const lh_error_t *self, lh_error_code_t code);

/**
 * @brief Test whether @p self has the success code.
 *
 * @param self Error object to read from.
 * @return ::lh_bool_true when @p self stores ::lh_error_code_ok.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_error_is_ok(const lh_error_t *self);

/**
 * @brief Test whether @p self has a non-success code.
 *
 * @param self Error object to read from.
 * @return ::lh_bool_true when @p self does not store ::lh_error_code_ok.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_error_is_failure(const lh_error_t *self);

/**
 * @brief Test whether @p self has a non-null description.
 *
 * @param self Error object to read from.
 * @return ::lh_bool_true when @p self stores a description pointer.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_error_has_desc(const lh_error_t *self);

/**
 * @brief Test whether @p self is the cleared success state.
 *
 * @param self Error object to read from.
 * @return ::lh_bool_true when @p self stores ::lh_error_code_ok and no description.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_error_is_empty(const lh_error_t *self);

/**
 * @brief Test whether two error objects store the same fields.
 *
 * Description equality is pointer equality; ::lh_error_t does not own or copy
 * description text.
 *
 * @param self Error object to read from.
 * @param other Error object to compare with.
 * @return ::lh_bool_true when both objects store the same code and description pointer.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_error_equals(const lh_error_t *self, const lh_error_t *other);

/**
 * @brief Test whether two error objects store the same code.
 *
 * Description pointers are ignored.
 *
 * @param self Error object to read from.
 * @param other Error object to compare with.
 * @return ::lh_bool_true when both objects store the same code.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_error_has_same_code(const lh_error_t *self, const lh_error_t *other);

/**
 * @brief Test whether two error objects store different codes.
 *
 * Description pointers are ignored.
 *
 * @param self Error object to read from.
 * @param other Error object to compare with.
 * @return ::lh_bool_true when both objects store different codes.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_error_has_diff_code(const lh_error_t *self, const lh_error_t *other);

/* ── copy / clear ────────────────────────────────────────────────────────── */

/**
 * @brief Copy the error state from @p other into @p self.
 * @param self  Destination error object.
 * @param other Source error object to copy from.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_error_assign(lh_error_t *self, const lh_error_t *other);

/**
 * @brief Reset @p self to an empty success state (::LH_ERROR_CODE_OK, null @c desc).
 *
 * Uses ::lh_error_empty_initializer via ::lh_error_assign.
 *
 * @param self Error object to clear.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_error_clear(lh_error_t *self);

/* ── init ────────────────────────────────────────────────────────────────── */

/**
 * @brief Initialize @p self with @p code and @p desc.
 *
 * Equivalent to ::lh_error_pack with both fields provided.
 *
 * @param self Error object to initialize.
 * @param code Initial error code.
 * @param desc Initial description pointer (may be null).
 */
LH_ATTRIBUTE_SYMBOL
void
lh_error_init(lh_error_t *self, lh_error_code_t code, lh_error_desc_t desc);

/**
 * @brief Initialize @p self by copying from @p other.
 * @param self  Error object to initialize.
 * @param other Source error to copy from.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_error_init_by_other(lh_error_t *self, const lh_error_t *other);

/**
 * @brief Initialize @p self to an empty success state.
 *
 * Equivalent to ::lh_error_clear.
 *
 * @param self Error object to initialize.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_error_init_by_empty(lh_error_t *self);

/**
 * @brief Return the error code, then clear @p self (::lh_error_clear).
 *
 * @param self Error object to read and reset.
 * @return Code value before clearing.
 */
LH_ATTRIBUTE_SYMBOL
lh_error_code_t
lh_error_get_code_and_clear(lh_error_t *self);

/* ── make ────────────────────────────────────────────────────────────────── */

/**
 * @brief Construct an ::lh_error_t with @p code and @p desc.
 *
 * @param code Error code (::lh_error_code_t).
 * @param desc Description string (may be @c lh_null).
 * @return Constructed ::lh_error_t value.
 */
LH_ATTRIBUTE_SYMBOL
lh_error_t
lh_error_make(lh_error_code_t code, lh_error_desc_t desc);

/**
 * @brief Construct an ::lh_error_t with @p code and no description.
 *
 * @param code Error code (::lh_error_code_t).
 * @return Constructed ::lh_error_t value.
 */
LH_ATTRIBUTE_SYMBOL
lh_error_t
lh_error_make_by_code(lh_error_code_t code);

LH_COMPILER_EXTERN_C_END

#endif // LH_ERROR_H
