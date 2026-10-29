/**
 * @file dispatch.h
 * @brief ::lh_os_controller_dispatch_t — one feed or update of a controller.
 *
 * What the recognizer's sink (::lh_os_controller_dispatch_sink) works with while
 * a controller feeds it one event or updates it once: the controller the events
 * go to, and whether its handler acted on any of them. It lives on the stack of
 * ::lh_os_controller_feed and ::lh_os_controller_update, for one call.
 */

#ifndef LH_OS_CONTROLLER_DISPATCH_H
#define LH_OS_CONTROLLER_DISPATCH_H

#include <lh/initializer.h>
#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/void.h>

struct lh_os_controller;

/**
 * @struct lh_os_controller_dispatch
 * @brief The controller events go to, and whether its handler acted.
 */
struct lh_os_controller_dispatch
{
    struct lh_os_controller *controller; /**< Where the events go. */
    lh_bool_t handled;                   /**< True once the handler acted on one. */
};
/** @copybrief lh_os_controller_dispatch */
typedef struct lh_os_controller_dispatch lh_os_controller_dispatch_t;

/** A dispatch to @p controller that nothing was handled by yet. */
#define lh_os_controller_dispatch_initializer(controller)                                          \
    lh_initializer((controller), lh_bool_false)

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The controller the events go to (level 1).
 *
 * @param self The dispatch; checked by ::lh_assert_runtime_ref.
 */
struct lh_os_controller *
lh_os_controller_dispatch_get_controller(const lh_os_controller_dispatch_t *self);

/**
 * @brief Store the controller the events go to (level 1).
 *
 * A level-1 setter: the pointer is stored as given and is not validated.
 *
 * @param self       The dispatch; checked by ::lh_assert_runtime_ref.
 * @param controller The new controller.
 */
lh_void
lh_os_controller_dispatch_set_controller(lh_os_controller_dispatch_t *self,
                                         struct lh_os_controller *controller);

/**
 * @brief Whether the handler acted on any event of this dispatch (level 1).
 *
 * @param self The dispatch; checked by ::lh_assert_runtime_ref.
 */
lh_bool_t
lh_os_controller_dispatch_is_handled(const lh_os_controller_dispatch_t *self);

/**
 * @brief Store whether the handler acted (level 1).
 *
 * A level-1 setter: the flag is stored as given.
 *
 * @param self    The dispatch; checked by ::lh_assert_runtime_ref.
 * @param handled The new flag.
 */
lh_void
lh_os_controller_dispatch_set_handled(lh_os_controller_dispatch_t *self, lh_bool_t handled);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_CONTROLLER_DISPATCH_H */
