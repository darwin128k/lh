/**
 * @file controller.c
 * @brief Implementation of `lh/os/controller.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/os/controller.h>
#include <lh/util/return.h>

/* -- Setting up ------------------------------------------------------------ */

lh_void
lh_os_controller_set_handle_cb(lh_os_controller_t *self,
                               lh_os_controller_handlers_handle_cb handle_cb)
{
    lh_assert_runtime_ref(self);
    self->handle_cb = handle_cb;
}

lh_os_controller_handlers_handle_cb
lh_os_controller_get_handle_cb(lh_os_controller_cptr self)
{
    lh_assert_runtime_ref(self);
    return self->handle_cb;
}

lh_void
lh_os_controller_set_update_cb(lh_os_controller_t *self,
                               lh_os_controller_handlers_update_cb update_cb)
{
    lh_assert_runtime_ref(self);
    self->update_cb = update_cb;
}

lh_os_controller_handlers_update_cb
lh_os_controller_get_update_cb(lh_os_controller_cptr self)
{
    lh_assert_runtime_ref(self);
    return self->update_cb;
}

lh_void
lh_os_controller_set_cancel_cb(lh_os_controller_t *self,
                               lh_os_controller_handlers_cancel_cb cancel_cb)
{
    lh_assert_runtime_ref(self);
    self->cancel_cb = cancel_cb;
}

lh_os_controller_handlers_cancel_cb
lh_os_controller_get_cancel_cb(lh_os_controller_cptr self)
{
    lh_assert_runtime_ref(self);
    return self->cancel_cb;
}

lh_void
lh_os_controller_set_context(lh_os_controller_t *self, lh_self_ptr context)
{
    lh_assert_runtime_ref(self);
    self->context = context;
}

lh_self_ptr
lh_os_controller_get_context(lh_os_controller_cptr self)
{
    lh_assert_runtime_ref(self);
    return self->context;
}

lh_os_input_recognizer_ptr
lh_os_controller_get_recognizer(lh_os_controller_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->recognizer);
}

/* -- Flags ----------------------------------------------------------------- */

lh_void
lh_os_controller_set_flags(lh_os_controller_t *self, lh_os_controller_flags_t flags)
{
    lh_assert_runtime_ref(self);
    self->flags = flags;
}

lh_os_controller_flags_t
lh_os_controller_get_flags(lh_os_controller_cptr self)
{
    lh_assert_runtime_ref(self);
    return self->flags;
}

lh_bool_t
lh_os_controller_has_flag(lh_os_controller_cptr self, lh_os_controller_flags_t flag)
{
    return lh_type_cast(lh_bool_t, lh_bit_and(lh_os_controller_get_flags(self), flag) == flag);
}

lh_bool_t
lh_os_controller_is_enabled(lh_os_controller_cptr self)
{
    return lh_os_controller_has_flag(self, lh_os_controller_flags_enabled);
}

lh_void
lh_os_controller_set_enabled(lh_os_controller_t *self, lh_bool_t enabled)
{
    const lh_os_controller_flags_t flags = lh_os_controller_get_flags(self);

    lh_os_controller_set_flags(
        self,
        lh_type_cast(
            lh_os_controller_flags_t,
            enabled ? lh_bit_or(flags, lh_os_controller_flags_enabled)
                    : lh_bit_and(flags, lh_bit_not(lh_type_cast(lh_os_controller_flags_t,
                                                                lh_os_controller_flags_enabled)))));
}

lh_void
lh_os_controller_init(lh_os_controller_t *self, lh_os_controller_handlers_handle_cb handle_cb,
                      lh_self_ptr context, lh_os_input_recognizer_config_cptr config)
{
    lh_os_controller_set_handle_cb(self, handle_cb);
    lh_os_controller_set_update_cb(self, lh_null);
    lh_os_controller_set_cancel_cb(self, lh_null);
    lh_os_controller_set_context(self, context);
    lh_os_input_recognizer_init(lh_os_controller_get_recognizer(self), config);
    lh_os_controller_set_flags(
        self, lh_type_cast(lh_os_controller_flags_t, lh_bit_or(lh_os_controller_flags_enabled,
                                                               lh_os_controller_flags_gestures)));
}

/* -- Events ---------------------------------------------------------------- */

lh_bool_t
lh_os_controller_handle(lh_os_controller_t *self, lh_os_input_event_cptr event)
{
    lh_os_controller_handlers_handle_cb handle_cb = lh_os_controller_get_handle_cb(self);

    lh_assert_runtime_ref(event);
    lh_return_if(!lh_os_controller_is_enabled(self) || !lh_ptr_is_set(handle_cb), lh_bool_false);
    return handle_cb(lh_os_controller_get_context(self), event);
}

/** What the recognizer's sink works with during one feed or update: the controller the
   events go to, and whether its handler acted on any of them. */
struct lh_os_controller_dispatch
{
    lh_os_controller_t *controller;
    lh_bool_t handled;
};
/** @copybrief lh_os_controller_dispatch */
typedef struct lh_os_controller_dispatch lh_os_controller_dispatch_t;

/** A dispatch to @p controller that nothing was handled by yet. */
#define lh_os_controller_dispatch_initializer(controller)                                          \
    lh_initializer((controller), lh_bool_false)

lh_void
lh_os_controller_dispatch_sink(lh_self_ptr self, lh_os_input_event_cptr event)
{
    lh_os_controller_dispatch_t *dispatch = lh_ptr_rcast(lh_os_controller_dispatch_t, self);

    if (lh_os_controller_handle(dispatch->controller, event))
    {
        dispatch->handled = lh_bool_true;
    }
}

lh_bool_t
lh_os_controller_feed(lh_os_controller_t *self, lh_os_input_event_cptr event)
{
    lh_os_controller_dispatch_t dispatch = lh_os_controller_dispatch_initializer(self);

    lh_assert_runtime_ref(event);
    lh_return_if(!lh_os_controller_is_enabled(self), lh_bool_false);
    lh_return_if(!lh_os_controller_has_flag(self, lh_os_controller_flags_gestures),
                 lh_os_controller_handle(self, event));
    lh_os_input_recognizer_feed(lh_os_controller_get_recognizer(self), event,
                                lh_os_controller_dispatch_sink, lh_addr_of(dispatch));
    return dispatch.handled;
}

lh_bool_t
lh_os_controller_update(lh_os_controller_t *self, lh_tick_t now)
{
    lh_os_controller_dispatch_t dispatch = lh_os_controller_dispatch_initializer(self);
    lh_os_controller_handlers_update_cb update_cb = lh_os_controller_get_update_cb(self);

    /* Disabled freezes the recognizer, as feed does. Running it here would
       change the phase and then drop the gesture in handle, so a long press
       that came due while muted would be gone once the controller is enabled. */
    if (lh_os_controller_is_enabled(self) &&
        lh_os_controller_has_flag(self, lh_os_controller_flags_gestures))
    {
        lh_os_input_recognizer_update(lh_os_controller_get_recognizer(self), now,
                                      lh_os_controller_dispatch_sink, lh_addr_of(dispatch));
    }
    if (lh_ptr_is_set(update_cb))
    {
        update_cb(lh_os_controller_get_context(self), now);
    }
    return dispatch.handled;
}

lh_void
lh_os_controller_forget_gestures(lh_os_controller_t *self)
{
    lh_os_input_recognizer_ptr recognizer = lh_os_controller_get_recognizer(self);
    const lh_os_input_recognizer_config_t config =
        lh_ptr_deref(lh_os_input_recognizer_get_config(recognizer));

    lh_os_input_recognizer_init(recognizer, lh_addr_of(config));
}

lh_void
lh_os_controller_cancel(lh_os_controller_t *self)
{
    lh_os_controller_handlers_cancel_cb cancel_cb = lh_os_controller_get_cancel_cb(self);

    lh_os_controller_forget_gestures(self);
    if (lh_ptr_is_set(cancel_cb))
    {
        cancel_cb(lh_os_controller_get_context(self));
    }
}

lh_void
lh_os_controller_sink(lh_self_ptr self, lh_os_input_event_cptr event)
{
    (void)lh_os_controller_feed(lh_ptr_rcast(lh_os_controller_t, self), event);
}
