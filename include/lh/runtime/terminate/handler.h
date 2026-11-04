/**
 * @file handler.h
 * @brief The terminate handler: a call and its context.
 *
 * ::lh_runtime_terminate_handler_t is what ::lh_runtime_terminate runs: one
 * ::lh_runtime_terminate_cb and the context it is handed, the same shape as an
 * ::lh_io_writer_t. It owns nothing; the context belongs to whoever filled it.
 * The runtime one is ::lh_runtime_terminate_get_mut; a host fills it with
 * ::lh_runtime_terminate_handler_init at start-up.
 */

#ifndef LH_RUNTIME_TERMINATE_HANDLER_H
#define LH_RUNTIME_TERMINATE_HANDLER_H

#include <lh/compiler/extern/c.h>
#include <lh/runtime/terminate/cb.h>
#include <lh/runtime/terminate/handler/fields.h>

/**
 * @brief A terminate call and its context.
 */
struct lh_runtime_terminate_handler
{
    /** Members: ::lh_runtime_terminate_handler_fields. */
    lh_runtime_terminate_handler_fields(lh_runtime_terminate_fn);
};

/** @copybrief lh_runtime_terminate_handler */
typedef struct lh_runtime_terminate_handler lh_runtime_terminate_handler_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The call of @p self (level 1).
 * @param self Handler; must not be null.
 * @return The stored call, or ::lh_null when none is installed.
 */
lh_runtime_terminate_cb
lh_runtime_terminate_handler_get_call_cb(const lh_runtime_terminate_handler_t *self);

/**
 * @brief Store the call of @p self (level 1).
 * @param self    Handler; must not be null.
 * @param call_cb The call (may be ::lh_null: no handler).
 */
lh_void
lh_runtime_terminate_handler_set_call_cb(lh_runtime_terminate_handler_t *self,
                                         lh_runtime_terminate_cb call_cb);

/**
 * @brief The context of @p self (level 1).
 * @param self Handler; must not be null.
 * @return The context handed to the call (may be ::lh_null).
 */
lh_ptr
lh_runtime_terminate_handler_get_context(const lh_runtime_terminate_handler_t *self);

/**
 * @brief Store the context of @p self (level 1).
 * @param self    Handler; must not be null.
 * @param context The context, borrowed (may be ::lh_null).
 */
lh_void
lh_runtime_terminate_handler_set_context(lh_runtime_terminate_handler_t *self, lh_ptr context);

/**
 * @brief Fill @p self with @p call_cb and @p context.
 * @param self    Handler; must not be null.
 * @param call_cb The call (may be ::lh_null).
 * @param context The context, borrowed (may be ::lh_null).
 */
lh_void
lh_runtime_terminate_handler_init(lh_runtime_terminate_handler_t *self,
                                  lh_runtime_terminate_cb call_cb, lh_ptr context);

/**
 * @brief Run the call of @p self with its context, if one is installed.
 *
 * Returns only when no call is installed or the call itself returns; the
 * caller (::lh_runtime_terminate) decides what happens then.
 *
 * @param self Handler; must not be null.
 */
lh_void
lh_runtime_terminate_handler_call(const lh_runtime_terminate_handler_t *self);

/**
 * @brief Copy @p other into @p self.
 * @param self  Destination; must not be null.
 * @param other Source; must not be null.
 */
lh_void
lh_runtime_terminate_handler_assign(lh_runtime_terminate_handler_t *self,
                                    const lh_runtime_terminate_handler_t *other);

LH_COMPILER_EXTERN_C_END

#endif /* LH_RUNTIME_TERMINATE_HANDLER_H */
