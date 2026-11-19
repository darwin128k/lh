/* example/gx_panel.c
 *
 * The controls of gx_controls.h on a display panel, the way a board with no
 * operating system shows them: no frame buffer, only a band of 20 rows; each
 * frame the damage is drawn into the band a run of rows at a time and sent to
 * the panel, which keeps its own frame (lh/gx/strip.h). The same tree and the
 * same renderer as gx_web.c and gx_window.c; only the present differs.
 *
 * The panel here is memory — 320 by 200 RGB565 pixels, what a display
 * controller holds — and the input is a script in place of a touch screen: a
 * click on the button, the checkbox, the green radio, the slider dragged, a
 * name typed into the field and Enter. Each step prints how many pixels went
 * to the panel; at the end the panel is written to gx_panel.ppm, the picture
 * a real panel would show. It needs gx and nothing of the operating system:
 * it builds with LH_LIBRARY_OPTION_OS off as well.
 */

#include "gx_controls.h"

#include <stdio.h>

#include <lh/gx/input.h>
#include <lh/gx/render.h>
#include <lh/gx/strip.h>

/* The band the frames are drawn through: 20 rows, an eighth of a frame. */
#define BAND_ROWS 20

/* The display controller: the frame it keeps, and what it was sent. */
struct panel
{
    lh_u16_t frame[GX_CONTROLS_WIDTH * GX_CONTROLS_HEIGHT];
    long bands;
    long pixels;
};

/* The present: copy each run of the band into the panel's frame. A driver on
   a board sets the controller's window to the run and lets a DMA send it. */
static lh_void
panel_show(lh_ptr context, const lh_gx_pixmap_t *band, lh_gx_point_t at,
           const lh_gx_damage_t *drawn)
{
    struct panel *panel = (struct panel *)context;

    panel->bands += 1;
    for (lh_u32_t i = 0U; i < lh_gx_damage_get_count(drawn); ++i)
    {
        const lh_gx_rect_t *run = lh_gx_damage_get_rect(drawn, i);
        const lh_s32_t x = lh_gx_point_get_x(lh_gx_rect_get_origin(run));
        const lh_s32_t y = lh_gx_point_get_y(lh_gx_rect_get_origin(run));
        const lh_s32_t w = lh_gx_size_get_width(lh_gx_rect_get_size(run));
        const lh_s32_t h = lh_gx_size_get_height(lh_gx_rect_get_size(run));

        for (lh_s32_t row = y; row < y + h; ++row)
        {
            const lh_u16_t *src = (const lh_u16_t *)lh_gx_pixmap_get_address(
                band, (lh_u32_t)(x - lh_gx_point_get_x(&at)),
                (lh_u32_t)(row - lh_gx_point_get_y(&at)));
            lh_u16_t *dst = panel->frame + row * GX_CONTROLS_WIDTH + x;

            for (lh_s32_t col = 0; col < w; ++col)
            {
                dst[col] = src[col];
            }
        }
        panel->pixels += (long)w * h;
    }
}

/* Everything the screen is made of; static, for the records point at each other. */
static lh_u16_t band_words[GX_CONTROLS_WIDTH * BAND_ROWS];
static lh_memory_bounds_t band_bits;
static lh_gx_pixmap_t band;
static lh_gx_backend_sw_t sw;
static lh_gx_canvas_t canvas;
static lh_gx_render_t render;
static lh_gx_present_t present;
static lh_gx_pointer_t pointer;
static lh_gx_focus_t focus;
static struct panel panel;

/* One frame through the band, and a line saying what it sent. */
static lh_void
frame(const char *what)
{
    panel.bands = 0;
    panel.pixels = 0;
    (void)lh_gx_strip_draw(&render, &sw, &band, &present, &panel);
    printf("%-22s %6ld pixels in %2ld bands (%3ld%% of the screen)\n", what, panel.pixels,
           panel.bands, panel.pixels * 100L / (GX_CONTROLS_WIDTH * GX_CONTROLS_HEIGHT));
}

/* An event to the screen, the way a touch or keyboard driver hands it over. */
static lh_void
take(lh_gx_event_t *event)
{
    lh_gx_input_take(&pointer, &focus, lh_gx_render_get_screen_mut(&render), event);
}

static lh_void
pointer_at(lh_gx_event_kind_t kind, lh_gx_scalar_t x, lh_gx_scalar_t y)
{
    lh_gx_point_t at;
    lh_gx_event_t event;

    lh_gx_point_init(&at, x, y);
    lh_gx_event_init(&event, kind, at);
    take(&event);
}

static lh_void
tap(lh_gx_scalar_t x, lh_gx_scalar_t y)
{
    pointer_at(lh_gx_event_kind_press, x, y);
    pointer_at(lh_gx_event_kind_release, x, y);
}

static lh_void
type(const char *text)
{
    lh_gx_event_t event;

    for (; *text != '\0'; ++text)
    {
        lh_gx_event_init_text(&event, (lh_u32_t)(unsigned char)*text);
        take(&event);
    }
}

static lh_void
press_key(lh_gx_key_t key)
{
    lh_gx_event_t event;

    lh_gx_event_init_key(&event, key);
    take(&event);
}

/* The panel's frame as a binary PPM: RGB565 widened to eight bits a channel. */
static int
write_ppm(const char *path)
{
    FILE *file = fopen(path, "wb");

    if (file == NULL)
    {
        return 0;
    }
    fprintf(file, "P6\n%d %d\n255\n", GX_CONTROLS_WIDTH, GX_CONTROLS_HEIGHT);
    for (int i = 0; i < GX_CONTROLS_WIDTH * GX_CONTROLS_HEIGHT; ++i)
    {
        const unsigned p = panel.frame[i];
        const unsigned char rgb[3] = {
            (unsigned char)(((p >> 11) & 0x1FU) * 255U / 31U),
            (unsigned char)(((p >> 5) & 0x3FU) * 255U / 63U),
            (unsigned char)((p & 0x1FU) * 255U / 31U),
        };

        fwrite(rgb, 1, sizeof(rgb), file);
    }
    return fclose(file) == 0;
}

int
main(void)
{
    gx_controls_bind_runtime();
    lh_memory_bounds_init(&band_bits, band_words, sizeof(band_words));
    lh_gx_pixmap_init(&band, &band_bits, GX_CONTROLS_WIDTH, BAND_ROWS,
                      lh_gx_pixmap_format_rgb565);
    lh_gx_backend_sw_init(&sw, &band, 0, 0);
    lh_gx_backend_sw_canvas_init(&canvas);
    lh_gx_render_init(&render, &canvas, &sw, GX_CONTROLS_WIDTH, GX_CONTROLS_HEIGHT);
    lh_gx_present_init(&present);
    lh_gx_present_set_show(&present, panel_show);
    lh_gx_pointer_init(&pointer);
    lh_gx_focus_init(&focus);
    gx_controls_build(lh_gx_render_get_screen_mut(&render));

    frame("the first frame");
    tap(160, 100);
    frame("the button clicked");
    tap(100, 150);
    frame("Light checked");
    tap(26, 52);
    frame("Green chosen");
    pointer_at(lh_gx_event_kind_press, 200, 182);
    pointer_at(lh_gx_event_kind_move, 230, 182);
    pointer_at(lh_gx_event_kind_release, 230, 182);
    frame("the slider dragged");
    tap(215, 37);
    type("Panel");
    frame("a name typed");
    press_key(lh_gx_key_enter);
    frame("Enter");

    if (!write_ppm("gx_panel.ppm"))
    {
        fprintf(stderr, "gx_panel.ppm could not be written\n");
        return 1;
    }
    printf("the panel is in gx_panel.ppm\n");
    return 0;
}
