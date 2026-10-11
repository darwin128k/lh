/*
 * scene_lvgl.c -- the scenes of scene.h, built with LVGL and drawn by its software
 * renderer into a buffer the size of the screen (LV_DISPLAY_RENDER_MODE_DIRECT, so a
 * partial redraw redraws only what was invalidated).
 */

#include "scene.h"

#include <stdio.h>

#include "lvgl.h"

#define CARDS 24
#define ROWS 8
#define CELLS 32
#define PARAGRAPHS 4

static uint32_t pixels[SCENE_W * SCENE_H];
static lv_display_t *display;
static lv_obj_t *partial;
static lv_style_t card_style;
static lv_style_t root_style;
static lv_style_t row_style;
static lv_grad_dsc_t gradient;
static char captions[CELLS][12];
static int initialised;

static void
flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px)
{
    (void)area;
    (void)px;
    lv_display_flush_ready(disp);
}

static uint32_t
ticks(void)
{
    return 0;
}

int
lvgl_scene_has(int scene)
{
    return scene >= 0 && scene < SCENE_COUNT && scene != SCENE_CORNERS;
}

/* A bare object: no scrolling, no scrollbar, nothing a theme would have added. */
static lv_obj_t *
plain(lv_obj_t *parent)
{
    lv_obj_t *obj = lv_obj_create(parent);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
    return obj;
}

static lv_obj_t *
card(lv_obj_t *parent, const char *text)
{
    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text_static(label, text);
    /* One line, as a label in lh is: a caption wider than its card is cut, not
       wrapped onto a second line lh does not draw. Paragraphs ask for WRAP. */
    lv_label_set_long_mode(label, LV_LABEL_LONG_MODE_CLIP);
    lv_obj_add_style(label, &card_style, 0);
    return label;
}

void
lvgl_scene_setup(int scene)
{
    lv_obj_t *screen;
    int i;

    if (!initialised)
    {
        lv_init();
        lv_tick_set_cb(ticks);
        initialised = 1;
    }
    display = lv_display_create(SCENE_W, SCENE_H);
    lv_display_set_color_format(display, LV_COLOR_FORMAT_ARGB8888);
    lv_display_set_buffers(display, pixels, NULL, sizeof pixels, LV_DISPLAY_RENDER_MODE_DIRECT);
    lv_display_set_flush_cb(display, flush);
    lv_display_set_default(display);
    screen = lv_screen_active();
    lv_obj_remove_flag(screen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(screen, LV_SCROLLBAR_MODE_OFF);

    lv_style_init(&root_style);
    lv_style_set_bg_color(&root_style, lv_color_make(20, 24, 29));
    lv_style_set_bg_opa(&root_style, LV_OPA_COVER);
    lv_obj_add_style(screen, &root_style, 0);

    lv_style_init(&card_style);
    lv_style_set_bg_opa(&card_style, LV_OPA_COVER);
    lv_style_set_bg_color(&card_style, lv_color_make(46, 107, 230));
    lv_style_set_text_color(&card_style, lv_color_make(255, 255, 255));
    lv_style_set_text_font(&card_style, &lv_font_montserrat_16);
    lv_style_set_radius(&card_style, 8);
    lv_style_set_pad_all(&card_style, 8);

    if (scene == SCENE_GRAD_VERTICAL || scene == SCENE_GRAD_DIAGONAL || scene == SCENE_GRAD_RADIAL)
    {
        const lv_color_t stops[2] = {lv_color_make(46, 107, 230), lv_color_make(155, 77, 230)};

        lv_memzero(&gradient, sizeof gradient);
        lv_grad_init_stops(&gradient, stops, NULL, NULL, 2);
        if (scene == SCENE_GRAD_VERTICAL)
        {
            lv_grad_vertical_init(&gradient);
        }
        else if (scene == SCENE_GRAD_DIAGONAL)
        {
            lv_grad_linear_init(&gradient, lv_pct(0), lv_pct(0), lv_pct(100), lv_pct(100), LV_GRAD_EXTEND_PAD);
        }
        else
        {
            lv_grad_radial_init(&gradient, lv_pct(50), lv_pct(50), lv_pct(100), lv_pct(100), LV_GRAD_EXTEND_PAD);
        }
        lv_style_set_bg_grad(&card_style, &gradient);
    }
    if (scene == SCENE_SHADOW)
    {
        lv_style_set_shadow_width(&card_style, 12);
        lv_style_set_shadow_offset_y(&card_style, 4);
        lv_style_set_shadow_color(&card_style, lv_color_make(0, 0, 0));
        lv_style_set_shadow_opa(&card_style, 128);
    }

    if (scene == SCENE_TEXT)
    {
        lv_style_set_bg_color(&card_style, lv_color_make(42, 49, 59));
        lv_style_set_radius(&card_style, 6);
        for (i = 0; i < PARAGRAPHS; ++i)
        {
            lv_obj_t *label = card(screen, scene_paragraph);
            lv_label_set_long_mode(label, LV_LABEL_LONG_MODE_WRAP);
            lv_obj_set_pos(label, 8 + (i % 2) * 236, 8 + (i / 2) * 156);
            lv_obj_set_size(label, 228, 148);
            if (i == 0)
            {
                partial = label;
            }
        }
    }
    else if (scene == SCENE_FLEX)
    {
        lv_obj_t *column = plain(screen);

        lv_style_init(&row_style);
        lv_style_set_pad_all(&row_style, 0);
        lv_style_set_pad_column(&row_style, 8);
        lv_style_set_pad_row(&row_style, 8);
        lv_style_set_border_width(&row_style, 0);
        lv_style_set_bg_opa(&row_style, LV_OPA_TRANSP);
        lv_obj_add_style(column, &row_style, 0);
        lv_obj_set_style_pad_all(column, 8, 0);
        lv_obj_set_size(column, SCENE_W, SCENE_H);
        lv_obj_set_flex_flow(column, LV_FLEX_FLOW_COLUMN);
        for (i = 0; i < ROWS; ++i)
        {
            lv_obj_t *row = plain(column);
            int j;

            lv_obj_add_style(row, &row_style, 0);
            lv_obj_set_width(row, lv_pct(100));
            lv_obj_set_flex_grow(row, 1);
            lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
            for (j = 0; j < 4; ++j)
            {
                lv_obj_t *cell;

                snprintf(captions[i * 4 + j], sizeof captions[0], "Item %d", i * 4 + j + 1);
                cell = card(row, captions[i * 4 + j]);
                lv_obj_set_height(cell, lv_pct(100));
                lv_obj_set_flex_grow(cell, 1);
                if (i == 0 && j == 0)
                {
                    partial = cell;
                }
            }
        }
    }
    else
    {
        for (i = 0; i < CARDS; ++i)
        {
            lv_obj_t *label;

            snprintf(captions[i], sizeof captions[0], "Card %d", i + 1);
            label = card(screen, captions[i]);
            lv_obj_set_pos(label, 8 + (i % 6) * 78, 8 + (i / 6) * 78);
            lv_obj_set_size(label, 72, 66);
            if (i == 0)
            {
                partial = label;
            }
        }
    }
    lv_obj_invalidate(screen);
    lv_refr_now(display);
}

void
lvgl_scene_frame(void)
{
    lv_obj_invalidate(lv_screen_active());
    lv_refr_now(display);
}

void
lvgl_scene_frame_partial(void)
{
    lv_obj_invalidate(partial);
    lv_refr_now(display);
}

const uint32_t *
lvgl_scene_pixels(void)
{
    return pixels;
}

void
lvgl_scene_teardown(void)
{
    lv_display_delete(display);
    display = NULL;
}
