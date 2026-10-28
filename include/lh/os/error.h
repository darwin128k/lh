/**
 * @file error.h
 * @brief OS-layer error value type (::lh_os_error_t) and field accessors.
 *
 * Same shape as ::lh_error_t (code + description), but the description
 * is ::lh_os_error_desc_ptr — ::lh_wstr_ptr or ::lh_str_ptr, whichever
 * ::LH_LIBRARY_OPTION_OS_WERROR selects. Either is one pointer, so every
 * function here delegates to the ::lh_error_* counterpart through a pointer
 * cast, the same way ::lh_werror_t does; only the description-content
 * checks use ::lh_os_error_desc_is_empty.
 *
 * @see lh_error_t
 * @see lh_os_error_desc_ptr
 */

#ifndef LH_OS_ERROR_H
#define LH_OS_ERROR_H

#include <lh/void.h>
#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/error/code.h>
#include <lh/error/fields.h>
#include <lh/os/error/desc.h>

/**
 * @struct lh_os_error
 * @brief OS-layer error code plus a description in ::lh_os_error_desc_ptr.
 */
struct lh_os_error
{
    /** Members: ::lh_error_fields. */
    lh_error_fields(lh_error_code_t, const lh_os_error_desc_ptr);
};
/** @copybrief lh_os_error */
typedef struct lh_os_error lh_os_error_t;

/* -- set ------------------------------------------------------------------- */

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Replace @p self with @p code and @p desc.
 */
lh_void
lh_os_error_set(lh_os_error_t *self, lh_error_code_t code, const lh_os_error_desc_ptr desc);

/**
 * @brief Replace only the error code stored in @p self.
 */
lh_void
lh_os_error_set_code(lh_os_error_t *self, lh_error_code_t code);

/**
 * @brief Replace only the description stored in @p self.
 */
lh_void
lh_os_error_set_desc(lh_os_error_t *self, const lh_os_error_desc_ptr desc);

/* -- accessors ------------------------------------------------------------- */

/**
 * @brief Return the error code stored in @p self.
 */
lh_error_code_t
lh_os_error_get_code(const lh_os_error_t *self);

/**
 * @brief Return the description stored in @p self.
 */
const lh_os_error_desc_ptr
lh_os_error_get_desc(const lh_os_error_t *self);

/**
 * @brief Return the description stored in @p self, or @p fallback when empty.
 */
const lh_os_error_desc_ptr
lh_os_error_get_desc_or(const lh_os_error_t *self, const lh_os_error_desc_ptr fallback);

/**
 * @brief Test whether @p self stores @p code.
 */
lh_bool_t
lh_os_error_has_code(const lh_os_error_t *self, lh_error_code_t code);

/**
 * @brief Test whether @p self has the success code.
 */
lh_bool_t
lh_os_error_is_ok(const lh_os_error_t *self);

/**
 * @brief Test whether @p self has a non-success code.
 */
lh_bool_t
lh_os_error_is_failure(const lh_os_error_t *self);

/**
 * @brief Test whether @p self has a non-empty description.
 */
lh_bool_t
lh_os_error_has_desc(const lh_os_error_t *self);

/**
 * @brief Test whether @p self is the cleared success state.
 */
lh_bool_t
lh_os_error_is_empty(const lh_os_error_t *self);

/**
 * @brief Test whether two OS errors store the same code and description pointer.
 */
lh_bool_t
lh_os_error_equals(const lh_os_error_t *self, const lh_os_error_t *other);

/**
 * @brief Test whether two OS errors store the same code.
 */
lh_bool_t
lh_os_error_has_same_code(const lh_os_error_t *self, const lh_os_error_t *other);

/**
 * @brief Test whether two OS errors store different codes.
 */
lh_bool_t
lh_os_error_has_diff_code(const lh_os_error_t *self, const lh_os_error_t *other);

/* -- copy / clear ---------------------------------------------------------- */

/**
 * @brief Copy the error state from @p other into @p self.
 */
lh_void
lh_os_error_assign(lh_os_error_t *self, const lh_os_error_t *other);

/**
 * @brief Reset @p self to an empty success state.
 */
lh_void
lh_os_error_clear(lh_os_error_t *self);

/* -- init ------------------------------------------------------------------ */

/**
 * @brief Initialize @p self with @p code and @p desc.
 */
lh_void
lh_os_error_init(lh_os_error_t *self, lh_error_code_t code, const lh_os_error_desc_ptr desc);

/**
 * @brief Initialize @p self by copying from @p other.
 */
lh_void
lh_os_error_init_by_other(lh_os_error_t *self, const lh_os_error_t *other);

/**
 * @brief Initialize @p self to an empty success state.
 */
lh_void
lh_os_error_init_by_empty(lh_os_error_t *self);

/**
 * @brief Initialize @p self with @p code and an empty description.
 */
lh_void
lh_os_error_init_by_code(lh_os_error_t *self, lh_error_code_t code);

/**
 * @brief Return the error code, then clear @p self.
 */
lh_error_code_t
lh_os_error_get_code_and_clear(lh_os_error_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_ERROR_H */
