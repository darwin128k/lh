/**
 * @file error.h
 * @brief Runtime error value type (::lh_runtime_error_t) and its API.
 *
 * ::lh_runtime_error_t is binary-compatible with ::lh_error_t — both carry
 * a numeric code and an optional description pointer in the same field order.
 * All functions delegate to the corresponding ::lh_error_* counterparts.
 *
 * The runtime-namespaced type exists to document intent: an
 * ::lh_runtime_error_t is produced and consumed by the runtime throw / assert
 * layer, not by general-purpose code that works with ::lh_error_t directly.
 */

#ifndef LH_RUNTIME_ERROR_H
#define LH_RUNTIME_ERROR_H

#include <lh/attribute/symbol.h>
#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/runtime/error/code.h>
#include <lh/runtime/error/desc.h>
#include <lh/runtime/error/fields.h>

/**
 * @struct lh_runtime_error
 * @brief Runtime error: numeric code and optional human-readable description.
 *
 * Binary-compatible with ::lh_error_t.
 */
typedef struct lh_runtime_error
{
    lh_runtime_error_fields(lh_runtime_error_code_t, lh_runtime_error_desc_t);
} lh_runtime_error_t; /**< Typedef for struct ::lh_runtime_error. */

LH_COMPILER_EXTERN_C_BEGIN

/* ── pack / unpack ───────────────────────────────────────────────────────── */

/**
 * @brief Update fields on @p self from optional input pointers.
 *
 * @param self Error object to modify.
 * @param code Input pointer for new error code, or ::lh_null to keep current value.
 * @param desc Input pointer for new description, or ::lh_null to keep current value.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_runtime_error_pack(lh_runtime_error_t *self, const lh_runtime_error_code_t *code,
                      lh_runtime_error_desc_t *desc);

/**
 * @brief Read fields from @p self into optional output pointers.
 *
 * Pass ::lh_null for any pointer to skip that field.
 *
 * @param self Error object to read from.
 * @param code Output for @c code, or ::lh_null to skip.
 * @param desc Output for @c desc, or ::lh_null to skip.
 *
 * Example usage:
 * @code{.c}
 * lh_runtime_error_code_t code;
 * lh_runtime_error_unpack(&err, &code, lh_null);
 * @endcode
 */
LH_ATTRIBUTE_SYMBOL
void
lh_runtime_error_unpack(const lh_runtime_error_t *self, lh_runtime_error_code_t *code,
                        lh_runtime_error_desc_t *desc);

/**
 * @brief Unpack @p self into @p other (alias for ::lh_runtime_error_assign).
 * @param self  Source error (not null).
 * @param other Destination error (not null).
 */
LH_ATTRIBUTE_SYMBOL
void
lh_runtime_error_unpack_to_other(const lh_runtime_error_t *self, lh_runtime_error_t *other);

/* ── set ─────────────────────────────────────────────────────────────────── */

/**
 * @brief Replace @p self with @p code and @p desc.
 *
 * @param self Error object to modify.
 * @param code New error code.
 * @param desc New description pointer (may be null).
 */
LH_ATTRIBUTE_SYMBOL
void
lh_runtime_error_set(lh_runtime_error_t *self, lh_runtime_error_code_t code,
                     lh_runtime_error_desc_t desc);

/**
 * @brief Replace only the error code stored in @p self.
 *
 * @param self Error object to modify.
 * @param code New error code.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_runtime_error_set_code(lh_runtime_error_t *self, lh_runtime_error_code_t code);

/**
 * @brief Replace only the description pointer stored in @p self.
 *
 * @param self Error object to modify.
 * @param desc New description pointer (may be null).
 */
LH_ATTRIBUTE_SYMBOL
void
lh_runtime_error_set_desc(lh_runtime_error_t *self, lh_runtime_error_desc_t desc);

/* ── accessors ───────────────────────────────────────────────────────────── */

/**
 * @brief Return the error code stored in @p self.
 * @param self Error object to read from.
 * @return Current ::lh_runtime_error_code_t value.
 */
LH_ATTRIBUTE_SYMBOL
lh_runtime_error_code_t
lh_runtime_error_get_code(const lh_runtime_error_t *self);

/**
 * @brief Return the description pointer stored in @p self.
 * @param self Error object to read from.
 * @return Current @c desc (may be null).
 */
LH_ATTRIBUTE_SYMBOL
lh_runtime_error_desc_t
lh_runtime_error_get_desc(const lh_runtime_error_t *self);

/**
 * @brief Return the description pointer or @p fallback when it is null.
 *
 * @param self     Error object to read from.
 * @param fallback Description returned when @p self has no description.
 * @return Stored description when non-null, otherwise @p fallback.
 */
LH_ATTRIBUTE_SYMBOL
lh_runtime_error_desc_t
lh_runtime_error_get_desc_or(const lh_runtime_error_t *self, lh_runtime_error_desc_t fallback);

/* ── predicates ──────────────────────────────────────────────────────────── */

/**
 * @brief Test whether @p self stores @p code.
 *
 * @param self Error object to read from.
 * @param code Error code to compare with.
 * @return ::lh_bool_true when @p self carries @p code, otherwise ::lh_bool_false.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_runtime_error_has_code(const lh_runtime_error_t *self, lh_runtime_error_code_t code);

/**
 * @brief Test whether @p self has the success code.
 * @param self Error object to read from.
 * @return ::lh_bool_true when @p self stores ::lh_runtime_error_code_ok.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_runtime_error_is_ok(const lh_runtime_error_t *self);

/**
 * @brief Test whether @p self has a non-success code.
 * @param self Error object to read from.
 * @return ::lh_bool_true when @p self does not store ::lh_runtime_error_code_ok.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_runtime_error_is_failure(const lh_runtime_error_t *self);

/**
 * @brief Test whether @p self has a non-null description.
 * @param self Error object to read from.
 * @return ::lh_bool_true when @p self stores a description pointer.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_runtime_error_has_desc(const lh_runtime_error_t *self);

/**
 * @brief Test whether @p self is the cleared success state.
 * @param self Error object to read from.
 * @return ::lh_bool_true when @p self stores ::lh_runtime_error_code_ok and no description.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_runtime_error_is_empty(const lh_runtime_error_t *self);

/**
 * @brief Test whether two runtime error objects store the same fields.
 *
 * Description equality is pointer equality.
 *
 * @param self  Error object to read from.
 * @param other Error object to compare with.
 * @return ::lh_bool_true when both store the same code and description pointer.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_runtime_error_equals(const lh_runtime_error_t *self, const lh_runtime_error_t *other);

/**
 * @brief Test whether two runtime error objects store the same code.
 *
 * @param self  Error object to read from.
 * @param other Error object to compare with.
 * @return ::lh_bool_true when both store the same code.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_runtime_error_has_same_code(const lh_runtime_error_t *self, const lh_runtime_error_t *other);

/**
 * @brief Test whether two runtime error objects store different codes.
 *
 * @param self  Error object to read from.
 * @param other Error object to compare with.
 * @return ::lh_bool_true when both store different codes.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_runtime_error_has_diff_code(const lh_runtime_error_t *self, const lh_runtime_error_t *other);

/* ── copy / clear ────────────────────────────────────────────────────────── */

/**
 * @brief Copy the error state from @p other into @p self.
 * @param self  Destination error object.
 * @param other Source error object to copy from.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_runtime_error_assign(lh_runtime_error_t *self, const lh_runtime_error_t *other);

/**
 * @brief Reset @p self to an empty success state.
 * @param self Error object to clear.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_runtime_error_clear(lh_runtime_error_t *self);

/* ── init ────────────────────────────────────────────────────────────────── */

/**
 * @brief Initialize @p self with @p code and @p desc.
 *
 * @param self Error object to initialize.
 * @param code Initial error code.
 * @param desc Initial description pointer (may be null).
 */
LH_ATTRIBUTE_SYMBOL
void
lh_runtime_error_init(lh_runtime_error_t *self, lh_runtime_error_code_t code,
                      lh_runtime_error_desc_t desc);

/**
 * @brief Initialize @p self by copying from @p other.
 * @param self  Error object to initialize.
 * @param other Source error to copy from.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_runtime_error_init_by_other(lh_runtime_error_t *self, const lh_runtime_error_t *other);

/**
 * @brief Initialize @p self to an empty success state.
 * @param self Error object to initialize.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_runtime_error_init_by_empty(lh_runtime_error_t *self);

/**
 * @brief Return the error code, then clear @p self.
 *
 * @param self Error object to read and reset.
 * @return Code value before clearing.
 */
LH_ATTRIBUTE_SYMBOL
lh_runtime_error_code_t
lh_runtime_error_get_code_and_clear(lh_runtime_error_t *self);

/* ── make ────────────────────────────────────────────────────────────────── */

/**
 * @brief Construct an ::lh_runtime_error_t with @p code and @p desc.
 *
 * @param code Error code (::lh_runtime_error_code_t).
 * @param desc Description string (may be @c lh_null).
 * @return Constructed ::lh_runtime_error_t value.
 *
 * Example usage:
 * @code{.c}
 * lh_runtime_assert(ptr, lh_runtime_error_make(lh_runtime_error_code_null_pointer, "bad ptr"));
 * @endcode
 *
 * @see lh_runtime_error_make_by_code
 */
LH_ATTRIBUTE_SYMBOL
lh_runtime_error_t
lh_runtime_error_make(lh_runtime_error_code_t code, lh_runtime_error_desc_t desc);

/**
 * @brief Construct an ::lh_runtime_error_t with @p code and no description.
 *
 * @param code Error code (::lh_runtime_error_code_t).
 * @return Constructed ::lh_runtime_error_t value.
 *
 * Example usage:
 * @code{.c}
 * lh_runtime_assert(ptr, lh_runtime_error_make_by_code(lh_runtime_error_code_null_pointer));
 * @endcode
 *
 * @see lh_runtime_error_make
 * @see lh_runtime_error_make_by_desc
 */
LH_ATTRIBUTE_SYMBOL
lh_runtime_error_t
lh_runtime_error_make_by_code(lh_runtime_error_code_t code);

/**
 * @brief Construct an ::lh_runtime_error_t with @p desc and ::lh_runtime_error_code_interrupt.
 *
 * Convenience constructor for the common case where a human-readable message
 * is the only meaningful information at the throw site.
 * The error code is fixed to ::lh_runtime_error_code_interrupt.
 *
 * @param desc Description string (must not be null).
 * @return Constructed ::lh_runtime_error_t value.
 *
 * Example usage:
 * @code{.c}
 * lh_runtime_assert(ok, lh_runtime_error_make_by_desc("operation failed"));
 * @endcode
 *
 * @see lh_runtime_error_make
 * @see lh_runtime_error_make_by_code
 */
LH_ATTRIBUTE_SYMBOL
lh_runtime_error_t
lh_runtime_error_make_by_desc(lh_runtime_error_desc_t desc);

LH_COMPILER_EXTERN_C_END

#endif /* LH_RUNTIME_ERROR_H */
