/**
 * @file critical.h
 * @brief ::lh_critical_t — a critical section, as the platform makes one.
 *
 * Some code must run while nothing else touches the same data: a counter
 * shared with an interrupt handler, the atomics of a core without atomic
 * instructions (::LH_LIBRARY_OPTION_ATOMIC = ::LH_ATOMIC_CRITICAL). The core
 * cannot shut the others out by itself — that is masking interrupts on a
 * single core, a lock on a host with threads — so it calls the two calls of
 * this record, which the platform fills (lh/core/critical/enter/fn.h says
 * what they promise).
 *
 * A record with no calls is a platform where nothing can come between: one
 * thread and no interrupt handler touching the data. There entering and
 * leaving do nothing. The core uses the one in the slot
 * (lh/core/runtime/critical.h); the host fills it before it starts a second
 * thread or enables an interrupt that touches guarded data.
 *
 * Keep a section short and never wait inside it: on a single core it holds
 * every interrupt back.
 */

#ifndef LH_CRITICAL_H
#define LH_CRITICAL_H

#include <lh/core/bool.h>
#include <lh/core/critical/enter/cb.h>
#include <lh/core/critical/fields.h>
#include <lh/core/critical/leave/cb.h>

/**
 * @brief The two calls of a critical section.
 */
struct lh_critical
{
    /** Members: ::lh_critical_fields. */
    lh_critical_fields(lh_critical_enter_fn, lh_critical_leave_fn);
};

/** @copybrief lh_critical */
typedef struct lh_critical lh_critical_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The call that begins a section (level 1).
 * @param self Critical section; must not be null.
 * @return The call, or ::lh_null.
 */
lh_critical_enter_cb
lh_critical_get_enter_cb(const lh_critical_t *self);

/**
 * @brief Store the call that begins a section (level 1).
 * @param self     Critical section; must not be null.
 * @param enter_cb The call (may be ::lh_null).
 */
lh_void
lh_critical_set_enter_cb(lh_critical_t *self, lh_critical_enter_cb enter_cb);

/**
 * @brief The call that ends a section (level 1).
 * @param self Critical section; must not be null.
 * @return The call, or ::lh_null.
 */
lh_critical_leave_cb
lh_critical_get_leave_cb(const lh_critical_t *self);

/**
 * @brief Store the call that ends a section (level 1).
 * @param self     Critical section; must not be null.
 * @param leave_cb The call (may be ::lh_null).
 */
lh_void
lh_critical_set_leave_cb(lh_critical_t *self, lh_critical_leave_cb leave_cb);

/**
 * @brief Whether @p self lacks a call: then nothing can come between, and
 *        entering and leaving do nothing.
 * @param self Critical section; must not be null.
 * @return ::lh_bool_true when either call is ::lh_null.
 */
lh_bool_t
lh_critical_is_empty(const lh_critical_t *self);

/**
 * @brief Copy both calls of @p other into @p self.
 * @param self  Destination; must not be null.
 * @param other Source; must not be null. May be @p self.
 */
lh_void
lh_critical_assign(lh_critical_t *self, const lh_critical_t *other);

/**
 * @brief Begin a section (::lh_critical_enter_fn); an empty @p self does nothing.
 * @param self Critical section; must not be null.
 * @return The token for ::lh_critical_leave; 0 for an empty @p self.
 */
lh_usize_t
lh_critical_enter(const lh_critical_t *self);

/**
 * @brief End the section that returned @p state; an empty @p self does nothing.
 * @param self  The critical section it was entered with; must not be null.
 * @param state The token ::lh_critical_enter returned.
 */
lh_void
lh_critical_leave(const lh_critical_t *self, lh_usize_t state);

LH_COMPILER_EXTERN_C_END

#endif /* LH_CRITICAL_H */
