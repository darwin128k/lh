/**
 * @file origin.h
 * @brief Where an exception comes from (::lh_exception_origin_t) — the
 *        runtime check that failed.
 *
 * Each check owns one `static const` origin built entirely from
 * compile-time constants, so passing it costs nothing until the check
 * fails. Which members are filled depends on
 * ::LH_LIBRARY_OPTION_RUNTIME_CHECK_REPORT; the others are ::lh_null / `0`.
 * The error code is not part of it: it is passed next to the
 * origin, so it is reported at every level and may be computed at run time.
 */

#ifndef LH_EXCEPTION_ORIGIN_H
#define LH_EXCEPTION_ORIGIN_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/types.h>
#include <lh/str/ptr.h>

/**
 * @struct lh_exception_origin
 * @brief Location, message and (at the full level) text of the runtime
 *        check an exception comes from.
 */
struct lh_exception_origin
{
    const lh_str_ptr file;      /**< Source file (`__FILE__`); LOCATION and FULL. */
    const lh_str_ptr function;  /**< Enclosing function (`__func__`); FULL only. */
    const lh_str_ptr condition; /**< Text of the condition that held; FULL only. */
    const lh_str_ptr message;   /**< Message given at the check, or ::lh_null; LOCATION and FULL. */
    lh_uint_t line;             /**< Source line (`__LINE__`); LOCATION and FULL. */
};
/** @copybrief lh_exception_origin */
typedef struct lh_exception_origin lh_exception_origin_t;

LH_COMPILER_EXTERN_C_BEGIN

/* oo accessors ooooooooooooooooooooooooooooooooooooooooooo */

/**
 * @brief Return the source file stored in @p self.
 * @param self Origin to read from.
 * @return Current @c file (::lh_null when the report level drops it).
 */
const lh_str_ptr
lh_exception_origin_get_file(const lh_exception_origin_t *self);

/**
 * @brief Return the enclosing function stored in @p self.
 * @param self Origin to read from.
 * @return Current @c function (::lh_null below the full report level).
 */
const lh_str_ptr
lh_exception_origin_get_function(const lh_exception_origin_t *self);

/**
 * @brief Return the condition text stored in @p self.
 * @param self Origin to read from.
 * @return Current @c condition (::lh_null below the full report level).
 */
const lh_str_ptr
lh_exception_origin_get_condition(const lh_exception_origin_t *self);

/**
 * @brief Return the message stored in @p self.
 * @param self Origin to read from.
 * @return Current @c message (::lh_null when the check gives none or the
 *         report level drops it).
 */
const lh_str_ptr
lh_exception_origin_get_message(const lh_exception_origin_t *self);

/**
 * @brief Return the source line stored in @p self.
 * @param self Origin to read from.
 * @return Current @c line (`0` when the report level drops it).
 */
lh_uint_t
lh_exception_origin_get_line(const lh_exception_origin_t *self);

/* oo predicates oooooooooooooooooooooooooooooooooooooooooo */

/**
 * @brief Test whether @p self has a non-empty source file.
 * @param self Origin to read from.
 * @return ::lh_bool_true when @p self stores a non-null, non-empty file.
 */
lh_bool_t
lh_exception_origin_has_file(const lh_exception_origin_t *self);

/**
 * @brief Test whether @p self has a non-empty enclosing function.
 * @param self Origin to read from.
 * @return ::lh_bool_true when @p self stores a non-null, non-empty function.
 */
lh_bool_t
lh_exception_origin_has_function(const lh_exception_origin_t *self);

/**
 * @brief Test whether @p self has a non-empty condition text.
 * @param self Origin to read from.
 * @return ::lh_bool_true when @p self stores a non-null, non-empty condition.
 */
lh_bool_t
lh_exception_origin_has_condition(const lh_exception_origin_t *self);

/**
 * @brief Test whether @p self has a non-empty message.
 * @param self Origin to read from.
 * @return ::lh_bool_true when @p self stores a non-null, non-empty message.
 */
lh_bool_t
lh_exception_origin_has_message(const lh_exception_origin_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_EXCEPTION_ORIGIN_H */
