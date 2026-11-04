/**
 * @file initializer.h
 * @brief Brace-enclosed initializer for ::lh_runtime_terminate_handler_t.
 */

#ifndef LH_RUNTIME_TERMINATE_HANDLER_INITIALIZER_H
#define LH_RUNTIME_TERMINATE_HANDLER_INITIALIZER_H

#include <lh/core/runtime/terminate/cb.h>

/**
 * @def lh_runtime_terminate_handler_initializer(call_fn, context)
 * @brief Produces a brace-enclosed initializer for
 *        ::lh_runtime_terminate_handler_t.
 *
 * For a static object:
 *
 * @code{.c}
 * static void board_reset(lh_self_ptr self) { (void)self; NVIC_SystemReset(); }
 * static const lh_runtime_terminate_handler_t reset =
 *     lh_runtime_terminate_handler_initializer(board_reset, lh_null);
 * @endcode
 *
 * @param call_fn A function of type ::lh_runtime_terminate_fn, a pointer to
 *                one, or ::lh_null.
 * @param context The context handed to @p call_fn (may be ::lh_null).
 */
#define lh_runtime_terminate_handler_initializer(call_fn, context)                                 \
    lh_initializer(lh_cast_static(lh_runtime_terminate_cb, call_fn), context)

#endif /* LH_RUNTIME_TERMINATE_HANDLER_INITIALIZER_H */
