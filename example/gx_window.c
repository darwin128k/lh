/* example/gx_window.c
 *
 * The controls of gx_controls.h in a desktop window: the same tree and the
 * same renderer as gx_web.c, drawn by the software backend into the window's
 * surface; a frame asks the system to repaint only what changed
 * (lh/gx/window.h).
 *
 * Run it: the window shows the button, the checkbox, the radios, the slider
 * and the field, and the mouse and the keyboard work them as in the browser.
 * Closing the window ends the program.
 */

#include "gx_controls.h"

#include <stdio.h>

#include <lh/gx/window.h>
#include <lh/os/app.h>
#include <lh/os/app/backend/system.h>
#include <lh/os/window/backend/system.h>

/* Closing the window ends the application's loop. */
static lh_void
on_close(lh_os_window_t *window, lh_os_window_close_reason_t reason, lh_ptr context)
{
    struct lh_os_app *app = lh_os_window_get_app(window);

    (void)reason;
    (void)context;
    if (app != NULL)
    {
        lh_os_app_quit(app);
    }
}

int
main(void)
{
    /* The records point at each other and live as long as the program. */
    static lh_os_app_t app;

    gx_controls_bind_runtime();
    lh_os_app_init(&app);
    /* The application runs on contracts; on a desktop the system fills both. */
    static lh_os_app_backend_t loop;

    lh_os_app_backend_system_init(&loop);
    static lh_os_window_backend_t windows;

    lh_os_window_backend_system_init(&windows);
    lh_os_app_install_backend(&app, &loop, lh_null);
    lh_os_app_install_window_backend(&app, &windows, lh_null);
    static lh_gx_window_t window;

    lh_gx_window_init(&window);
    lh_os_window_set_on_close(lh_gx_window_get_window_mut(&window), on_close, NULL);
    gx_controls_build(lh_gx_window_get_screen_mut(&window));
    if (!lh_gx_window_try_open(&window, &app, "lh gx", GX_CONTROLS_WIDTH, GX_CONTROLS_HEIGHT))
    {
        fprintf(stderr, "the window would not open\n");
        return 1;
    }
    lh_os_app_run(&app);

    lh_gx_window_deinit(&window);
    lh_os_app_deinit(&app);
    return 0;
}
