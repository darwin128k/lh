/**
 * @file werror.h
 * @brief Wide error value type (::lh_werror_t) and field accessors.
 *
 * ::lh_werror_t is binary-compatible with ::lh_error_t — both carry a numeric
 * code and a description pointer in the same field order.
 * The description is NUL-terminated ::lh_werror_desc_ptr (::lh_wchar_t units).
 * All functions delegate to the corresponding ::lh_error_* counterparts.
 *
 * The wide-namespaced type documents intent: text is wide, not
 * ::lh_error_desc_ptr. The pointer does not own text.
 *
 * @see lh_error_t
 * @see lh_werror_desc_ptr
 */

#ifndef LH_WERROR_H
#define LH_WERROR_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/error/code.h>
#include <lh/error/fields.h>
#include <lh/werror/desc.h>

/**
 * @struct lh_werror
 * Binary-compatible with ::lh_error_t; description field is wide text.
 */
struct lh_werror
{
    /** Members: ::lh_error_fields. */
    lh_error_fields(lh_error_code_t, const lh_werror_desc_ptr);
};
/** @copybrief lh_werror */
typedef struct lh_werror lh_werror_t;

LH_COMPILER_EXTERN_C_BEGIN

/* -- set ------------------------------------------------------------------- */

/**
 * @brief Replace @p self with @p code and @p desc.
 *
 * @param self Wide error object to modify.
 * @param code New error code.
 * @param desc New description (null or empty = no description).
 */
lh_void
lh_werror_set(lh_werror_t *self, lh_error_code_t code, const lh_werror_desc_ptr desc);

/**
 * @brief Replace only the error code stored in @p self.
 *
 * @param self Wide error object to modify.
 * @param code New error code.
 */
lh_void
lh_werror_set_code(lh_werror_t *self, lh_error_code_t code);

/**
 * @brief Replace only the description stored in @p self.
 *
 * @param self Wide error object to modify.
 * @param desc New description (null or empty = no description).
 */
lh_void
lh_werror_set_desc(lh_werror_t *self, const lh_werror_desc_ptr desc);

/* -- accessors ------------------------------------------------------------- */

/**
 * @brief Return the error code stored in @p self.
 * @param self Wide error object to read from.
 * @return Current ::lh_error_code_t value.
 */
lh_error_code_t
lh_werror_get_code(const lh_werror_t *self);

/**
 * @brief Return the description stored in @p self.
 * @param self Wide error object to read from.
 * @return Current @c desc (null or empty when there is none).
 */
const lh_werror_desc_ptr
lh_werror_get_desc(const lh_werror_t *self);

/**
 * @brief Return the description or @p fallback when it is null or empty.
 *
 * @param self Wide error object to read from.
 * @param fallback Description returned when @p self has no description.
 * @return Stored description when non-empty, otherwise @p fallback.
 */
const lh_werror_desc_ptr
lh_werror_get_desc_or(const lh_werror_t *self, const lh_werror_desc_ptr fallback);

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
 * A null pointer and empty text both mean “no description”.
 *
 * @param self Wide error object to read from.
 * @return ::lh_bool_true when @p self stores a non-null, non-empty description.
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
 * Description equality is pointer identity;
 * ::lh_werror_t does not own or copy description text.
 *
 * @param self Wide error object to read from.
 * @param other Wide error object to compare with.
 * @return ::lh_bool_true when both objects store the same code and description pointer.
 */
lh_bool_t
lh_werror_equals(const lh_werror_t *self, const lh_werror_t *other);

/**
 * @brief Test whether two wide error objects store the same code.
 *
 * Descriptions are ignored.
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
 * Descriptions are ignored.
 *
 * @param self Wide error object to read from.
 * @param other Wide error object to compare with.
 * @return ::lh_bool_true when both objects store different codes.
 */
lh_bool_t
lh_werror_has_diff_code(const lh_werror_t *self, const lh_werror_t *other);

/* -- copy / clear ---------------------------------------------------------- */

/**
 * @brief Copy the error state from @p other into @p self.
 * @param self  Destination wide error object.
 * @param other Source wide error object to copy from.
 */
lh_void
lh_werror_assign(lh_werror_t *self, const lh_werror_t *other);

/**
 * @brief Reset @p self to an empty success state (::lh_error_code_ok, empty @c desc).
 *
 * Uses ::lh_werror_empty_initializer via ::lh_werror_assign.
 *
 * @param self Wide error object to clear.
 */
lh_void
lh_werror_clear(lh_werror_t *self);

/* -- init ------------------------------------------------------------------ */

/**
 * @brief Initialize @p self with @p code and @p desc.
 *
 * @param self Wide error object to initialize.
 * @param code Initial error code.
 * @param desc Initial description (null or empty = no description).
 */
lh_void
lh_werror_init(lh_werror_t *self, lh_error_code_t code, const lh_werror_desc_ptr desc);

/**
 * @brief Initialize @p self by copying from @p other.
 * @param self  Wide error object to initialize.
 * @param other Source error to copy from.
 */
lh_void
lh_werror_init_by_other(lh_werror_t *self, const lh_werror_t *other);

/**
 * @brief Initialize @p self to an empty success state.
 *
 * Equivalent to ::lh_werror_clear.
 *
 * @param self Wide error object to initialize.
 */
lh_void
lh_werror_init_by_empty(lh_werror_t *self);

/**
 * @brief Initialize @p self with @p code and an empty description.
 */
lh_void
lh_werror_init_by_code(lh_werror_t *self, lh_error_code_t code);

/**
 * @brief Return the error code, then clear @p self (::lh_werror_clear).
 *
 * @param self Wide error object to read and reset.
 * @return Code value before clearing.
 */
lh_error_code_t
lh_werror_get_code_and_clear(lh_werror_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_WERROR_H */
