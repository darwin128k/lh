/* example/gx_web.c
 *
 * The controls of gx_controls.h in a browser: drawn by the gx renderer through
 * the web backend, the browser only plays the frames and sends the pointer and
 * the keys back (lh/gx/web.h).
 *
 * Run it, open http://127.0.0.1:8080/ and press the button: it darkens while
 * it is held, and a release over it clicks it. Tap the checkbox, or its
 * caption: the background turns light, and dark again. Choose "Green": the
 * button turns green, and "Blue" is unmarked. Drag the slider: the button
 * grows and shrinks around its centre as the knob follows the pointer. Click
 * the field and type: the arrows, Home, End, Backspace and Delete edit it, and
 * Enter puts its text on the button. Open a second tab: it gets the whole
 * screen, and both follow every change. The host runs on this thread and does
 * not return; stop the program with Ctrl+C.
 */

#include "gx_controls.h"

#include <stdio.h>

#include <lh/gx/web.h>
#include <lh/os/net.h>
#include <lh/os/rail/base.h>
#include <lh/os/web/host.h>
#include <lh/rail/runtime/base.h>

int
main(void)
{
    /* Everything the host serves lives as long as the program: the records point
       at each other, and the host never returns. */
    static lh_gx_web_t web;

    gx_controls_bind_runtime();
    /* The host is a rail loop, and rail runs on what the program gives it: the OS. */
    lh_rail_base_assign(lh_rail_runtime_base_get_mut(), lh_os_rail_base_get());
    if (!lh_os_net_init())
    {
        fprintf(stderr, "the network would not start\n");
        return 1;
    }
    lh_gx_web_init(&web, GX_CONTROLS_WIDTH, GX_CONTROLS_HEIGHT);
    gx_controls_build(lh_gx_web_get_screen_mut(&web));
    const lh_net_port_t port = 8080U;

    printf("open http://127.0.0.1:%u/\n", (unsigned)port);
    fflush(stdout);
    if (!lh_os_web_host_run(lh_gx_web_get_app_mut(&web), port))
    {
        fprintf(stderr, "the web host would not start on port %u\n", (unsigned)port);
    }
    lh_gx_web_deinit(&web);
    lh_os_net_deinit();
    return 1;
}
