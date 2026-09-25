/**
 * @file error.h
 * @brief Native OS error value type (::lh_os_system_error_t).
 *
 * Same shape as ::lh_os_error_t (code + description), but @c code is
 * ::lh_os_system_error_code_t — the OS's own namespace (`GetLastError()` /
 * `errno`), not ours. See `lh/os/system/error/code.h` for why the two must
 * stay separate types instead of sharing a slot.
 *
 * Not layout-compatible with ::lh_error_t (the code field is a different
 * type and width), so — unlike ::lh_os_error_t / ::lh_werror_t — this type
 * does not delegate through a pointer cast; every function here is its own
 * implementation.
 *
 * @see lh_os_error_t
 * @see lh_os_system_error_code_t
 */

#ifndef LH_OS_SYSTEM_ERROR_H
#define LH_OS_SYSTEM_ERROR_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/error/fields.h>
#include <lh/os/error/desc.h>
#include <lh/os/system/error/code.h>

/**
 * @struct lh_os_system_error
 * @brief Native OS error code plus a description in ::lh_os_error_desc_t.
 */
typedef struct lh_os_system_error
{
    lh_error_fields(lh_os_system_error_code_t, lh_os_error_desc_t);
} lh_os_system_error_t;

LH_COMPILER_EXTERN_C_BEGIN

/* ── set ─────────────────────────────────────────────────────────────────── */

/**
 * @brief Replace @p self with @p code and @p desc.
 */
void
lh_os_system_error_set(lh_os_system_error_t *self, lh_os_system_error_code_t code, lh_os_error_desc_t desc);

/**
 * @brief Replace only the error code stored in @p self.
 */
void
lh_os_system_error_set_code(lh_os_system_error_t *self, lh_os_system_error_code_t code);

/**
 * @brief Replace only the description stored in @p self.
 */
void
lh_os_system_error_set_desc(lh_os_system_error_t *self, lh_os_error_desc_t desc);

/* ── accessors ───────────────────────────────────────────────────────────── */

/**
 * @brief Return the error code stored in @p self.
 */
lh_os_system_error_code_t
lh_os_system_error_get_code(const lh_os_system_error_t *self);

/**
 * @brief Return the description stored in @p self.
 */
lh_os_error_desc_t
lh_os_system_error_get_desc(const lh_os_system_error_t *self);

/**
 * @brief Return the description stored in @p self, or @p fallback when empty.
 */
lh_os_error_desc_t
lh_os_system_error_get_desc_or(const lh_os_system_error_t *self, lh_os_error_desc_t fallback);

/**
 * @brief Test whether @p self stores @p code.
 */
lh_bool_t
lh_os_system_error_has_code(const lh_os_system_error_t *self, lh_os_system_error_code_t code);

/**
 * @brief Test whether @p self has the success code.
 */
lh_bool_t
lh_os_system_error_is_ok(const lh_os_system_error_t *self);

/**
 * @brief Test whether @p self has a non-success code.
 */
lh_bool_t
lh_os_system_error_is_failure(const lh_os_system_error_t *self);

/**
 * @brief Test whether @p self has a non-empty description.
 */
lh_bool_t
lh_os_system_error_has_desc(const lh_os_system_error_t *self);

/**
 * @brief Test whether @p self is the cleared success state.
 */
lh_bool_t
lh_os_system_error_is_empty(const lh_os_system_error_t *self);

/**
 * @brief Test whether two native errors store the same code and description span.
 */
lh_bool_t
lh_os_system_error_equals(const lh_os_system_error_t *self, const lh_os_system_error_t *other);

/**
 * @brief Test whether two native errors store the same code.
 */
lh_bool_t
lh_os_system_error_has_same_code(const lh_os_system_error_t *self, const lh_os_system_error_t *other);

/**
 * @brief Test whether two native errors store different codes.
 */
lh_bool_t
lh_os_system_error_has_diff_code(const lh_os_system_error_t *self, const lh_os_system_error_t *other);

/* ── copy / clear ────────────────────────────────────────────────────────── */

/**
 * @brief Copy the error state from @p other into @p self.
 */
void
lh_os_system_error_assign(lh_os_system_error_t *self, const lh_os_system_error_t *other);

/**
 * @brief Reset @p self to an empty success state.
 */
void
lh_os_system_error_clear(lh_os_system_error_t *self);

/* ── init ────────────────────────────────────────────────────────────────── */

/**
 * @brief Initialize @p self with @p code and @p desc.
 */
void
lh_os_system_error_init(lh_os_system_error_t *self, lh_os_system_error_code_t code, lh_os_error_desc_t desc);

/**
 * @brief Initialize @p self by copying from @p other.
 */
void
lh_os_system_error_init_by_other(lh_os_system_error_t *self, const lh_os_system_error_t *other);

/**
 * @brief Initialize @p self to an empty success state.
 */
void
lh_os_system_error_init_by_empty(lh_os_system_error_t *self);

/**
 * @brief Return the error code, then clear @p self.
 */
lh_os_system_error_code_t
lh_os_system_error_get_code_and_clear(lh_os_system_error_t *self);

/* ── make ────────────────────────────────────────────────────────────────── */

/**
 * @brief Construct an ::lh_os_system_error_t with @p code and @p desc.
 */
lh_os_system_error_t
lh_os_system_error_make(lh_os_system_error_code_t code, lh_os_error_desc_t desc);

/**
 * @brief Construct an ::lh_os_system_error_t with @p code and no description.
 */
lh_os_system_error_t
lh_os_system_error_make_by_code(lh_os_system_error_code_t code);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_SYSTEM_ERROR_H */
