/**
 * @file error.h
 * @brief OS-layer error value type (::lh_os_error_t) and field accessors.
 *
 * Same shape as ::lh_error_t (code + description), but the description
 * type is ::lh_os_error_desc_t — ::lh_wstr_view_t or ::lh_str_view_t,
 * whichever ::LH_LIBRARY_OPTION_OS_WERROR selects. Both are layout-compatible
 * with ::lh_memory_view_t, so every function here delegates to the
 * ::lh_error_* counterpart through a pointer cast, the same way ::lh_werror_t
 * delegates today.
 *
 * @see lh_error_t
 * @see lh_os_error_desc_t
 */

#ifndef LH_OS_ERROR_H
#define LH_OS_ERROR_H

#include <lh/attribute/symbol.h>
#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/error/code.h>
#include <lh/error/fields.h>
#include <lh/os/error/desc.h>

/**
 * @struct lh_os_error
 * @brief OS-layer error code plus a description in ::lh_os_error_desc_t.
 */
typedef struct lh_os_error
{
    lh_error_fields(lh_error_code_t, lh_os_error_desc_t);
} lh_os_error_t;

LH_COMPILER_EXTERN_C_BEGIN

/* ── set ─────────────────────────────────────────────────────────────────── */

/**
 * @brief Replace @p self with @p code and @p desc.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_os_error_set(lh_os_error_t *self, lh_error_code_t code, lh_os_error_desc_t desc);

/**
 * @brief Replace only the error code stored in @p self.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_os_error_set_code(lh_os_error_t *self, lh_error_code_t code);

/**
 * @brief Replace only the description stored in @p self.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_os_error_set_desc(lh_os_error_t *self, lh_os_error_desc_t desc);

/* ── accessors ───────────────────────────────────────────────────────────── */

/**
 * @brief Return the error code stored in @p self.
 */
LH_ATTRIBUTE_SYMBOL
lh_error_code_t
lh_os_error_get_code(const lh_os_error_t *self);

/**
 * @brief Return the description stored in @p self.
 */
LH_ATTRIBUTE_SYMBOL
lh_os_error_desc_t
lh_os_error_get_desc(const lh_os_error_t *self);

/**
 * @brief Return the description stored in @p self, or @p fallback when empty.
 */
LH_ATTRIBUTE_SYMBOL
lh_os_error_desc_t
lh_os_error_get_desc_or(const lh_os_error_t *self, lh_os_error_desc_t fallback);

/**
 * @brief Test whether @p self stores @p code.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_error_has_code(const lh_os_error_t *self, lh_error_code_t code);

/**
 * @brief Test whether @p self has the success code.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_error_is_ok(const lh_os_error_t *self);

/**
 * @brief Test whether @p self has a non-success code.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_error_is_failure(const lh_os_error_t *self);

/**
 * @brief Test whether @p self has a non-empty description.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_error_has_desc(const lh_os_error_t *self);

/**
 * @brief Test whether @p self is the cleared success state.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_error_is_empty(const lh_os_error_t *self);

/**
 * @brief Test whether two OS errors store the same code and description span.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_error_equals(const lh_os_error_t *self, const lh_os_error_t *other);

/**
 * @brief Test whether two OS errors store the same code.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_error_has_same_code(const lh_os_error_t *self, const lh_os_error_t *other);

/**
 * @brief Test whether two OS errors store different codes.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_error_has_diff_code(const lh_os_error_t *self, const lh_os_error_t *other);

/* ── copy / clear ────────────────────────────────────────────────────────── */

/**
 * @brief Copy the error state from @p other into @p self.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_os_error_assign(lh_os_error_t *self, const lh_os_error_t *other);

/**
 * @brief Reset @p self to an empty success state.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_os_error_clear(lh_os_error_t *self);

/* ── init ────────────────────────────────────────────────────────────────── */

/**
 * @brief Initialize @p self with @p code and @p desc.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_os_error_init(lh_os_error_t *self, lh_error_code_t code, lh_os_error_desc_t desc);

/**
 * @brief Initialize @p self by copying from @p other.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_os_error_init_by_other(lh_os_error_t *self, const lh_os_error_t *other);

/**
 * @brief Initialize @p self to an empty success state.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_os_error_init_by_empty(lh_os_error_t *self);

/**
 * @brief Return the error code, then clear @p self.
 */
LH_ATTRIBUTE_SYMBOL
lh_error_code_t
lh_os_error_get_code_and_clear(lh_os_error_t *self);

/* ── make ────────────────────────────────────────────────────────────────── */

/**
 * @brief Construct an ::lh_os_error_t with @p code and @p desc.
 */
LH_ATTRIBUTE_SYMBOL
lh_os_error_t
lh_os_error_make(lh_error_code_t code, lh_os_error_desc_t desc);

/**
 * @brief Construct an ::lh_os_error_t with @p code and no description.
 */
LH_ATTRIBUTE_SYMBOL
lh_os_error_t
lh_os_error_make_by_code(lh_error_code_t code);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_ERROR_H */
