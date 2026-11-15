/**
 * @file event.h
 * @brief One pointer event applied to ::lh_gl_input_t.
 *
 * ::lh_gl_input_press answers a point. This answers one ::lh_os_input_event_t
 * from a driver: pointer down searches, move keeps the hold, up and cancel
 * forget it. The first pointer to go down owns the hold. Another pointer's
 * events leave it. A key, a wheel and a controller event leave it too.
 *
 * The point is whole pixels of the event's Q24.8 position, the fraction cut
 * towards zero. The caller owns the layer and the entity.
 */

#ifndef LH_GL_INPUT_EVENT_H
#define LH_GL_INPUT_EVENT_H

#include <lh/gl/input.h>
#include <lh/os/input/event.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Apply one input event to the hold.
 *
 * A pointer down searches @p layer and starts tracking that pointer, unless
 * another pointer is already tracked: then the hold stays and the result is
 * ::lh_null. A move, up or cancel of the tracked pointer is
 * ::lh_gl_input_move or ::lh_gl_input_release. Any other event returns
 * ::lh_null and changes nothing.
 *
 * @param self  Input state; must not be null.
 * @param layer Layer to search on a pointer down; must not be null.
 * @param event The event; must not be null. Not copied.
 * @return The entity the event concerns. Borrowed. ::lh_null when the event
 *         does not concern the hold, or the search missed.
 */
const lh_gl_entity_t *
lh_gl_input_apply(lh_gl_input_t *self, const lh_gl_layer_t *layer,
                  const lh_os_input_event_ptr event);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_INPUT_EVENT_H */
