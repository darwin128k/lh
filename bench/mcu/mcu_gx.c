/* bench/mcu/mcu_gx.c -- a gx screen on a board with no operating system.
 *
 * A 320 by 240 panel in RGB565: a background and a button with a caption in the
 * built-in font, drawn through a band of 20 rows (lh/gx/strip.h) and sent to the
 * panel's data register a pixel at a time; then a press and a release on the
 * button, and the frame that redraws it. Linked for size only: the register
 * address is an SPI data register of a Cortex-M4 part, and nothing runs.
 */

#include <lh/gl/font.h>
#include <lh/gx/box.h>
#include <lh/gx/button.h>
#include <lh/gx/font/baked.h>
#include <lh/gx/input.h>
#include <lh/gx/strip.h>
#include <lh/gx/style.h>

#define PANEL_W 320
#define PANEL_H 240
#define BAND_ROWS 20

/* The panel's data register: where a display controller takes its pixels. */
#define PANEL_DATA (*(volatile lh_u16_t *)0x4001300CU)

static lh_u16_t band_words[PANEL_W * BAND_ROWS];
static lh_memory_bounds_t band_bits;
static lh_gx_pixmap_t band;
static lh_gx_backend_sw_t sw;
static lh_gx_canvas_t canvas;
static lh_gx_render_t render;
static lh_gx_present_t present;
static lh_gx_font_t font;
static lh_gx_style_t back_style;
static lh_gx_style_t button_style;
static lh_gx_style_t pressed_style;
static lh_gx_entity_t back;
static lh_gx_button_t button;
static lh_gx_pointer_t pointer;
static lh_gx_focus_t focus;
static volatile lh_u32_t clicks;

/* Send each run the band holds to the panel, row by row. A real driver sets
   the panel's window to the run first and lets a DMA do the copy. */
static lh_void
panel_show(lh_ptr context, const lh_gx_pixmap_t *pixels, lh_gx_point_t at,
           const lh_gx_damage_t *drawn)
{
    (void)context;
    for (lh_u32_t i = 0U; i < lh_gx_damage_get_count(drawn); ++i)
    {
        const lh_gx_rect_t *run = lh_gx_damage_get_rect(drawn, i);
        const lh_s32_t x = lh_gx_point_get_x(lh_gx_rect_get_origin(run));
        const lh_s32_t y = lh_gx_point_get_y(lh_gx_rect_get_origin(run));
        const lh_s32_t w = lh_gx_size_get_width(lh_gx_rect_get_size(run));
        const lh_s32_t h = lh_gx_size_get_height(lh_gx_rect_get_size(run));

        for (lh_s32_t row = 0; row < h; ++row)
        {
            const lh_u16_t *src = (const lh_u16_t *)lh_gx_pixmap_get_address(
                pixels, (lh_u32_t)(x - lh_gx_point_get_x(&at)),
                (lh_u32_t)(y + row - lh_gx_point_get_y(&at)));

            for (lh_s32_t col = 0; col < w; ++col)
            {
                PANEL_DATA = src[col];
            }
        }
    }
}

static lh_void
on_click(lh_gx_button_t *pressed, lh_ptr context)
{
    (void)pressed;
    (void)context;
    clicks = clicks + 1U;
}

static lh_void
tap(lh_gx_scalar_t x, lh_gx_scalar_t y)
{
    lh_gx_point_t at;
    lh_gx_event_t event;

    lh_gx_point_init(&at, x, y);
    lh_gx_event_init(&event, lh_gx_event_kind_press, at);
    lh_gx_input_take(&pointer, &focus, lh_gx_render_get_screen_mut(&render), &event);
    lh_gx_event_init(&event, lh_gx_event_kind_release, at);
    lh_gx_input_take(&pointer, &focus, lh_gx_render_get_screen_mut(&render), &event);
}

int
main(void)
{
    lh_memory_bounds_init(&band_bits, band_words, sizeof(band_words));
    lh_gx_pixmap_init(&band, &band_bits, PANEL_W, BAND_ROWS, lh_gx_pixmap_format_rgb565);
    lh_gx_backend_sw_init(&sw, &band, 0, 0);
    /* A board with no gradient: the canvas that draws colours only, so nothing of
       the gradient is linked. */
    lh_gx_backend_sw_canvas_init_solid(&canvas);
    lh_gx_render_init(&render, &canvas, &sw, PANEL_W, PANEL_H);
    lh_gx_present_init(&present);
    lh_gx_present_set_show(&present, panel_show);
    lh_gx_pointer_init(&pointer);
    lh_gx_focus_init(&focus);
    lh_gx_font_baked_init(&font, lh_gl_font_get_default());

    lh_gx_style_init(&back_style, 0xFF2B2B30U, 0U, 0, 0, 0U, lh_null);
    lh_gx_style_init(&button_style, 0xFF3D7BD9U, 0xFF2A5CA8U, 1, 6, 0xFFFFFFFFU, &font);
    lh_gx_style_init(&pressed_style, 0xFF2A5CA8U, 0xFF1D4480U, 1, 6, 0xFFFFFFFFU, &font);
    lh_gx_entity_init(&back, &lh_gx_box_class, &back_style, 0, 0, PANEL_W, PANEL_H);
    lh_gx_button_init(&button, &button_style, 90, 96, 140, 48, "Press me", on_click, lh_null);
    lh_gx_button_set_pressed_style(&button, &pressed_style);
    lh_gx_entity_attach(lh_gx_render_get_screen_mut(&render), &back);
    lh_gx_entity_attach(&back, lh_gx_button_get_box_mut(&button));

    (void)lh_gx_strip_draw(&render, &sw, &band, &present, lh_null);
    tap(160, 120);
    (void)lh_gx_strip_draw(&render, &sw, &band, &present, lh_null);
    for (;;)
    {
    }
}
