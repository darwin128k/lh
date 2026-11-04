/**
 * @file initializer.h
 * @brief Brace-enclosed initializer for ::lh_runtime_check_handler_t.
 */

#ifndef LH_RUNTIME_CHECK_HANDLER_INITIALIZER_H
#define LH_RUNTIME_CHECK_HANDLER_INITIALIZER_H

#include <lh/cast/static.h>
#include <lh/initializer.h>
#include <lh/runtime/check/fail/cb.h>

/**
 * @def lh_runtime_check_handler_initializer(fail_fn, context)
 * @brief Produces a brace-enclosed initializer for ::lh_runtime_check_handler_t.
 *
 * For a static object:
 *
 * @code{.c}
 * static void report(lh_self_ptr self, const lh_exception_t *e) { uart_put_exception(self, e); }
 * static const lh_runtime_check_handler_t on_check =
 *     lh_runtime_check_handler_initializer(report, &uart1);
 * @endcode
 *
 * @param fail_fn A function of type ::lh_runtime_check_fail_fn, a pointer to
 *                one, or ::lh_null.
 * @param context The context handed to @p fail_fn (may be ::lh_null).
 */
#define lh_runtime_check_handler_initializer(fail_fn, context)                                     \
    lh_initializer(lh_cast_static(lh_runtime_check_fail_cb, fail_fn), context)

#endif /* LH_RUNTIME_CHECK_HANDLER_INITIALIZER_H */
