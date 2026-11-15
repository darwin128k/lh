/**
 * @file controller.h
 * @brief ::lh_gl_input_t as the handler of an ::lh_os_controller_t.
 *
 * The controller knows nothing of a layer. These two have the shapes of its
 * handlers and take an ::lh_gl_input_binding_t as `self`. A pointer event
 * searches or keeps the hold. A click, a drag or a swipe is handled when a
 * press is already held. A key is not.
 */

#ifndef LH_GL_INPUT_CONTROLLER_H
#define LH_GL_INPUT_CONTROLLER_H

#include <lh/gl/input/binding/fields.h>
#include <lh/gl/input/event.h>
#include <lh/os/controller/handlers/cancel/fn.h>
#include <lh/os/controller/handlers/handle/fn.h>

/**
 * @struct lh_gl_input_binding
 * @brief The hold and the layer handed to the controller. Fields via
 *        ::lh_gl_input_binding_fields.
 */
struct lh_gl_input_binding
{
    /** Members: ::lh_gl_input_binding_fields. */
    lh_gl_input_binding_fields(lh_gl_input_t, lh_gl_layer_t);
};
/** @copybrief lh_gl_input_binding */
typedef struct lh_gl_input_binding lh_gl_input_binding_t;

LH_COMPILER_EXTERN_C_BEGIN

/** @brief The hold. Borrowed. */
lh_gl_input_t *
lh_gl_input_binding_get_input(const lh_gl_input_binding_t *self);

/** @brief Store the hold. @p input may be ::lh_null. */
lh_void
lh_gl_input_binding_set_input(lh_gl_input_binding_t *self, lh_gl_input_t *input);

/** @brief The layer that is searched. Borrowed. */
const lh_gl_layer_t *
lh_gl_input_binding_get_layer(const lh_gl_input_binding_t *self);

/** @brief Store the layer. @p layer may be ::lh_null. */
lh_void
lh_gl_input_binding_set_layer(lh_gl_input_binding_t *self, const lh_gl_layer_t *layer);

/** @brief Hold nothing over no layer. */
lh_void
lh_gl_input_binding_init(lh_gl_input_binding_t *self, lh_gl_input_t *input,
                         const lh_gl_layer_t *layer);

/**
 * @brief An ::lh_os_controller_handlers_handle_fn.
 *
 * @p self is an ::lh_gl_input_binding_t. True when the event concerns the hold.
 */
lh_bool_t
lh_gl_input_controller_handle_event_is_handled(lh_self_ptr self, const lh_os_input_event_ptr event);

/** @brief An ::lh_os_controller_handlers_cancel_fn. Forgets the hold. */
lh_void
lh_gl_input_controller_cancel(lh_self_ptr self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_INPUT_CONTROLLER_H */
