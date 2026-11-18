/* example/gx_web_button.c
 *
 * A gx screen in the browser: a button that counts its clicks. The tree is
 * three entities — a background, and a button that is a box with its caption
 * — drawn by the gx renderer through the web backend; the browser only plays
 * the frames and sends the pointer back (lh/gx/web.h).
 *
 * Run it, open http://127.0.0.1:8080/ and press the button: it greys while it
 * is held, and a release over it clicks it. Open a second tab: it gets the
 * whole screen, and both follow every change. The host runs on this thread
 * and does not return; stop the program with Ctrl+C.
 */

#include <stdio.h>
#include <stdlib.h>

#include <lh/core/runtime/allocator.h>
#include <lh/core/runtime/terminate.h>
#include <lh/core/str/format/uint.h>
#include <lh/gx/box.h>
#include <lh/gx/button.h>
#include <lh/gl/font.h>
#include <lh/gx/font/baked.h>
#include <lh/gx/style.h>
#include <lh/gx/web.h>
#include <lh/rail/runtime/base.h>
#include <lh/os/net.h>
#include <lh/os/rail/base.h>
#include <lh/os/web/host.h>

/* The example is a host: the core starts with every runtime slot empty, so it
   binds the C library before lh allocates anything. */
static lh_ptr
example_alloc(lh_self_ptr self, lh_usize_t size)
{
    (void)self;
    return malloc(size);
}

static lh_void
example_dealloc(lh_self_ptr self, lh_ptr ptr)
{
    (void)self;
    free(ptr);
}

static lh_void
example_abort(lh_self_ptr self)
{
    (void)self;
    abort();
}

/* The caption the button shows after a click: "Clicked N". */
static lh_char_t caption[32] = "Press me";

/* The button's reaction; it runs on the host's thread, where the tree lives. */
static lh_void
on_click(lh_gx_button_t *button, lh_ptr context)
{
    lh_uint_t *clicks = (lh_uint_t *)context;
    static const lh_char_t prefix[] = "Clicked ";
    lh_usize_t n = sizeof(prefix) - 1U;

    *clicks += 1U;
    for (lh_usize_t i = 0U; i < n; ++i)
    {
        caption[i] = prefix[i];
    }
    n += lh_str_ptr_format_uint(*clicks, caption + n, sizeof(caption) - n - 1U);
    caption[n] = '\0';
    lh_gx_button_set_text(button, caption);
}

int
main(void)
{
    /* Everything the host serves lives as long as the program: the records point
       at each other, and the host never returns. */
    static lh_gx_web_t web;

    lh_memory_allocator_init(lh_runtime_allocator_get_mut(), example_alloc, example_dealloc);
    lh_runtime_terminate_handler_init(lh_runtime_terminate_get_mut(), example_abort, NULL);
    /* The host is a rail loop, and rail runs on what the program gives it: the OS. */
    lh_rail_base_assign(lh_rail_runtime_base_get_mut(), lh_os_rail_base_get());
    if (!lh_os_net_init())
    {
        fprintf(stderr, "the network would not start\n");
        return 1;
    }
    /* The built-in Roboto still lives in gl; it moves with the swap (docs/cleanup.md).
       Without it the button still works; it only has no caption. */
    const lh_font_t *table = lh_gl_font_get_default();
    static lh_gx_font_t font;
    const lh_gx_font_t *face = lh_null;

    if (table != lh_null)
    {
        lh_gx_font_baked_init(&font, table);
        face = &font;
    }

    lh_gx_web_init(&web, 320, 200);
    static lh_gx_style_t back_style;

    lh_gx_style_init(&back_style, 0xFF2B2B30U, 0U, 0, 0, 0U, lh_null);
    static lh_gx_style_t button_style;

    lh_gx_style_init(&button_style, 0xFF3D7BD9U, 0xFF2A5CA8U, 1, 0, 0xFFFFFFFFU, face);
    static lh_gx_style_t pressed_style;

    lh_gx_style_init(&pressed_style, 0xFF2A5CA8U, 0xFF1D4480U, 1, 0, 0xFFFFFFFFU, face);
    static lh_gx_entity_t back;

    lh_gx_entity_init(&back, &lh_gx_box_class, &back_style, 0, 0, 320, 200);
    static lh_gx_button_t button;
    static lh_uint_t clicks;

    lh_gx_button_init(&button, &button_style, 90, 76, 140, 48, caption, on_click, &clicks);
    lh_gx_button_set_pressed_style(&button, &pressed_style);
    lh_gx_entity_attach(lh_gx_web_get_screen_mut(&web), &back);
    lh_gx_entity_attach(&back, lh_gx_button_get_box_mut(&button));
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
