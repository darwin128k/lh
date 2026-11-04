/**
 * @file handler.h
 * @brief The failed-check handler: a report call and its context.
 *
 * ::lh_runtime_check_handler_t is what ::lh_runtime_check_fail reports to: one
 * ::lh_runtime_check_fail_cb and the context it is handed, the same shape as
 * an ::lh_io_writer_t. It owns nothing; the context belongs to whoever filled
 * it. The runtime one is ::lh_runtime_check_get_mut; a host fills it with
 * ::lh_runtime_check_handler_init at start-up.
 */

#ifndef LH_RUNTIME_CHECK_HANDLER_H
#define LH_RUNTIME_CHECK_HANDLER_H

#include <lh/compiler/extern/c.h>
#include <lh/runtime/check/fail/cb.h>
#include <lh/runtime/check/handler/fields.h>

/**
 * @brief A failed-check report call and its context.
 */
struct lh_runtime_check_handler
{
    /** Members: ::lh_runtime_check_handler_fields. */
    lh_runtime_check_handler_fields(lh_runtime_check_fail_fn);
};

/** @copybrief lh_runtime_check_handler */
typedef struct lh_runtime_check_handler lh_runtime_check_handler_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The report call of @p self (level 1).
 * @param self Handler; must not be null.
 * @return The stored call, or ::lh_null when none is installed.
 */
lh_runtime_check_fail_cb
lh_runtime_check_handler_get_fail_cb(const lh_runtime_check_handler_t *self);

/**
 * @brief Store the report call of @p self (level 1).
 * @param self    Handler; must not be null.
 * @param fail_cb The call (may be ::lh_null: nothing is reported).
 */
lh_void
lh_runtime_check_handler_set_fail_cb(lh_runtime_check_handler_t *self,
                                     lh_runtime_check_fail_cb fail_cb);

/**
 * @brief The context of @p self (level 1).
 * @param self Handler; must not be null.
 * @return The context handed to the call (may be ::lh_null).
 */
lh_ptr
lh_runtime_check_handler_get_context(const lh_runtime_check_handler_t *self);

/**
 * @brief Store the context of @p self (level 1).
 * @param self    Handler; must not be null.
 * @param context The context, borrowed (may be ::lh_null).
 */
lh_void
lh_runtime_check_handler_set_context(lh_runtime_check_handler_t *self, lh_ptr context);

/**
 * @brief Fill @p self with @p fail_cb and @p context.
 * @param self    Handler; must not be null.
 * @param fail_cb The call (may be ::lh_null).
 * @param context The context, borrowed (may be ::lh_null).
 */
lh_void
lh_runtime_check_handler_init(lh_runtime_check_handler_t *self, lh_runtime_check_fail_cb fail_cb,
                              lh_ptr context);

/**
 * @brief Report @p exception through the call of @p self, if one is installed.
 * @param self      Handler; must not be null.
 * @param exception The failed check; must not be null, lives for the call only.
 */
lh_void
lh_runtime_check_handler_call(const lh_runtime_check_handler_t *self,
                              const lh_exception_t *exception);

/**
 * @brief Copy @p other into @p self.
 * @param self  Destination; must not be null.
 * @param other Source; must not be null.
 */
lh_void
lh_runtime_check_handler_assign(lh_runtime_check_handler_t *self,
                                const lh_runtime_check_handler_t *other);

LH_COMPILER_EXTERN_C_END

#endif /* LH_RUNTIME_CHECK_HANDLER_H */
