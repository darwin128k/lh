/* example/gx_controls.h
 *
 * The screen both gx examples show — in a browser (gx_web.c) and in a desktop
 * window (gx_window.c): one tree of controls, built once, drawn by the same
 * renderer; only the backend and the present differ. That is what "LH is
 * contracts" means on a screen.
 */

#ifndef LH_EXAMPLE_GX_CONTROLS_H
#define LH_EXAMPLE_GX_CONTROLS_H

#include <lh/gx/entity.h>

/* The screen's size, in pixels. */
#define GX_CONTROLS_WIDTH 320
#define GX_CONTROLS_HEIGHT 200

/* Bind the C library to the core's runtime slots — the allocator and the
   terminate handler — before lh allocates anything: the core starts with every
   slot empty, and the example is the host that fills them. */
void
gx_controls_bind_runtime(void);

/* Build the controls and attach them to @p screen: a background, a button that
   counts its clicks, a checkbox that lights the background, two radios that
   colour the button, a slider that sizes it and a text field whose Enter names
   it. Everything lives in static storage: the records point at each other. */
void
gx_controls_build(lh_gx_entity_t *screen);

#endif /* LH_EXAMPLE_GX_CONTROLS_H */
