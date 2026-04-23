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

#include <lh/attribute.h>
#include <lh/bool.h>
#include <lh/compiler.h>
#include <lh/error/code.h>
#include <lh/error/desc.h>

/**
 * @struct lh_error
 * @brief Error code and optional human-readable description.
 */
typedef struct lh_error {
    lh_error_code_t code; /**< Discrete error kind; ::LH_ERROR_CODE_OK means success / empty. */
    lh_error_desc_t desc; /**< Optional message pointer; may be null. */
} lh_error_t;             /**< Typedef for struct ::lh_error. */

LH_COMPILER(EXTERN_C_BEGIN)

/* ── pack / unpack ───────────────────────────────────────────────────────── */

/**
 * @brief Update fields on @p self from optional input pointers.
 *
 * @param self Error object to modify.
 * @param code Input pointer for new error code, or ::lh_null to keep current value.
 * @param desc Input pointer for new description pointer, or ::lh_null to keep current value.
 */
LH_ATTRIBUTE(SYMBOL)
void lh_error_pack(lh_error_t *self, const lh_error_code_t *code, lh_error_desc_t *desc);

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
LH_ATTRIBUTE(SYMBOL)
void lh_error_unpack(const lh_error_t *self, lh_error_code_t *code, lh_error_desc_t *desc);

/**
 * @brief Replace @p self with @p code and @p desc.
 *
 * Equivalent to ::lh_error_pack with both fields provided.
 *
 * @param self Error object to modify.
 * @param code New error code.
 * @param desc New description pointer (may be null).
 */
LH_ATTRIBUTE(SYMBOL)
void lh_error_set(lh_error_t *self, lh_error_code_t code, lh_error_desc_t desc);

/* ── accessors ───────────────────────────────────────────────────────────── */

/**
 * @brief Return the error code stored in @p self.
 * @param self Error object to read from.
 * @return Current ::lh_error_code_t value.
 */
LH_ATTRIBUTE(SYMBOL)
lh_error_code_t lh_error_get_code(const lh_error_t *self);

/**
 * @brief Return the description pointer stored in @p self.
 * @param self Error object to read from.
 * @return Current @c desc (may be null).
 */
LH_ATTRIBUTE(SYMBOL)
lh_error_desc_t lh_error_get_desc(const lh_error_t *self);

/* ── copy / clear ────────────────────────────────────────────────────────── */

/**
 * @brief Copy the error state from @p other into @p self.
 * @param self  Destination error object.
 * @param other Source error object to copy from.
 */
LH_ATTRIBUTE(SYMBOL)
void lh_error_assign(lh_error_t *self, const lh_error_t *other);

/**
 * @brief Reset @p self to an empty success state (::LH_ERROR_CODE_OK, null @c desc).
 *
 * Uses ::lh_error_empty_initializer via ::lh_error_assign.
 *
 * @param self Error object to clear.
 */
LH_ATTRIBUTE(SYMBOL)
void lh_error_clear(lh_error_t *self);

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
LH_ATTRIBUTE(SYMBOL)
void lh_error_init(lh_error_t *self, lh_error_code_t code, lh_error_desc_t desc);

/**
 * @brief Initialize @p self by copying from @p other.
 * @param self  Error object to initialize.
 * @param other Source error to copy from.
 */
LH_ATTRIBUTE(SYMBOL)
void lh_error_init_by_other(lh_error_t *self, const lh_error_t *other);

/**
 * @brief Initialize @p self to an empty success state.
 *
 * Equivalent to ::lh_error_clear.
 *
 * @param self Error object to initialize.
 */
LH_ATTRIBUTE(SYMBOL)
void lh_error_init_by_empty(lh_error_t *self);

/**
 * @brief Return the error code, then clear @p self (::lh_error_clear).
 *
 * @param self Error object to read and reset.
 * @return Code value before clearing.
 */
LH_ATTRIBUTE(SYMBOL)
lh_error_code_t lh_error_get_code_and_clear(lh_error_t *self);

LH_COMPILER(EXTERN_C_END)

#endif // LH_ERROR_H
