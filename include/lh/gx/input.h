/**
 * @file input.h
 * @brief Where an event goes on a screen: ::lh_gx_input_take.
 *
 * Every source of input — a window, a browser, a touch panel, a test — turns
 * its reports into ::lh_gx_event_t and hands them here. The pointer's events go
 * to the pointer (`lh/gx/pointer.h`), which finds the entity under a press and
 * keeps the gesture there; a press also moves the focus (`lh/gx/focus.h`) to
 * what it landed on. A key or a text goes to the entity with the focus. So the
 * rule for what goes where is written once, and each source only translates.
 */

#ifndef LH_GX_INPUT_H
#define LH_GX_INPUT_H

#include <lh/gx/focus.h>
#include <lh/gx/pointer.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Deliver @p event on the screen @p top: a pointer event to @p pointer
 *        (::lh_gx_pointer_take) — a press also moves @p focus to what it landed
 *        on (::lh_gx_focus_press) — and a key or a text to @p focus
 *        (::lh_gx_focus_take). A focus or a blur is the focus's own and is not
 *        taken here.
 *
 * @param pointer The screen's pointer; must not be null.
 * @param focus   The screen's focus; must not be null.
 * @param top     The tree a press searches, usually the screen; must not be null.
 * @param event   The event; must not be null.
 */
lh_void
lh_gx_input_take(lh_gx_pointer_t *pointer, lh_gx_focus_t *focus, lh_gx_entity_t *top,
                 const lh_gx_event_t *event);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_INPUT_H */
