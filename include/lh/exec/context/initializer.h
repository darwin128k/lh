/**
 * @file initializer.h
 * @brief Brace-enclosed initializer for ::lh_exec_context_t.
 */

#ifndef LH_EXEC_CONTEXT_INITIALIZER_H
#define LH_EXEC_CONTEXT_INITIALIZER_H

#include <lh/exec/context/close/cb.h>
#include <lh/exec/context/open/cb.h>

/**
 * @def lh_exec_context_initializer(open_fn, close_fn, user)
 * @brief Produces a brace-enclosed initializer for ::lh_exec_context_t.
 *
 * `lh_exec_context_initializer(lh_null, lh_null, lh_null)` is the empty
 * context: nothing opened, every `work` receives `local` = ::lh_null.
 *
 * @param open_fn  ::lh_exec_context_open_fn, or ::lh_null.
 * @param close_fn ::lh_exec_context_close_fn, or ::lh_null.
 * @param user     The pointer both calls receive.
 */
#define lh_exec_context_initializer(open_fn, close_fn, user)                                       \
    lh_initializer(lh_cast_static(lh_exec_context_open_cb, open_fn),                               \
                   lh_cast_static(lh_exec_context_close_cb, close_fn), user)

#endif /* LH_EXEC_CONTEXT_INITIALIZER_H */
