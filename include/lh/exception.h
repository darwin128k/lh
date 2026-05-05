/**
 * @file exception.h
 * @brief Value thrown through the `lh` jump-based exception path.
 *
 * An ::lh_exception_t is the payload moved when you “throw”:
 * always an ::lh_error_t (`code`, message, etc.).
 *
 * When **`NDEBUG` is not defined**,
 * the struct also stores ::lh_exception_origin_t
 * so you can log **where** the throw was raised.
 *
 * Defining `NDEBUG` removes `origin` entirely
 * for smaller structs and no extra string pointers in release builds.
 *
 * Catch sites hold a copy in ::lh_exception_catch_t;
 * the runtime stack in `lh/runtime/exception/catch/stack.h`
 * copies into `exception` on throw.
 *
 * Construction helpers: `lh/exception/initializer.h`.
 */

#ifndef LH_EXCEPTION_H
#define LH_EXCEPTION_H

#include <lh/error.h>

#ifndef NDEBUG
#    include <lh/exception/origin.h>
#endif // NDEBUG

/**
 * @struct lh_exception
 * @brief Error value plus optional debug-only provenance.
 *
 * @note Layout depends on `NDEBUG`: the `origin` member exists only in debug builds.
 *       Portable code should not rely on `sizeof(lh_exception_t)`
 *       or binary layout across build modes.
 *
 * @see lh_exception_catch_t
 * @see lh_exception_initializer()
 * @see lh_exception_empty_initializer()
 */
typedef struct lh_exception {
    lh_error_t error; /**< Primary payload (code and related error fields). */
#ifndef NDEBUG
    lh_exception_origin_t origin; /**< Raise site metadata; absent when `NDEBUG` is defined. */
#endif
} lh_exception_t; /**< Typedef for struct ::lh_exception. */

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Mutable pointer to the embedded ::lh_error_t inside @p self.
 *
 * Layout guarantees the error is the first member, so this is equivalent to casting @p self to
 * ::lh_error_t * in supported configurations.
 *
 * @param self Exception value (not null).
 * @return Pointer to @p self->error.
 */
LH_ATTRIBUTE(SYMBOL)
lh_error_t *lh_exception_get_error(const lh_exception_t *self);

LH_COMPILER_EXTERN_C_END

#endif // LH_EXCEPTION_H