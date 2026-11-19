/*
 * scene_lh.c -- the scenes of scene.h, built with lh's own API and drawn by its
 * software canvas into memory.
 */

#include "scene.h"

#include <stdio.h>

#include <lh/gl/canvas/box.h>
#include <lh/gl/canvas/sw.h>
#include <lh/gl/label.h>
#include <lh/gl/text.h>
#include <lh/gl/view.h>

#define CARDS 24
#define ROWS 8
#define CELLS 32
#define PARAGRAPHS 4

static uint32_t pixels[SCENE_W * SCENE_H];
static lh_gl_pixmap_t pixmap;
static lh_gl_canvas_sw_t sw;
static lh_gl_canvas_t canvas;
static lh_gl_view_t view;

static lh_gl_container_t root;
static lh_gl_label_t cards[CARDS];
static lh_gl_container_t rows[ROWS];
static lh_gl_label_t cells[CELLS];
static lh_gl_label_t paragraphs[PARAGRAPHS];
static lh_gl_layout_t down;
static lh_gl_layout_t across;

static lh_gl_style_t root_style;
static lh_gl_style_t card_style;
static lh_gl_style_t row_style;
static lh_pixel_gradient_t gradient;
static lh_gl_shadow_t shadow;
static lh_gl_rect_t partial;
static lh_gl_color_t clear_color;
static char captions[CELLS][12];

static lh_gl_color_t
rgb(int r, int g, int b, int a)
{
    lh_gl_color_t c;
    lh_gl_color_init(&c, (lh_gl_color_channel_t)r, (lh_gl_color_channel_t)g, (lh_gl_color_channel_t)b,
                     (lh_gl_color_channel_t)a);
    return c;
}

static lh_gl_rect_t
rect(int x, int y, int w, int h)
{
    lh_gl_rect_t r;
    lh_gl_rect_init(&r, lh_gl_scalar(x), lh_gl_scalar(y), lh_gl_scalar(w), lh_gl_scalar(h));
    return r;
}

/* The card look every scene starts from: one colour, a radius of 8, 8 of padding,
   light text. */
static void
card_look(void)
{
    lh_gl_color_t fill = rgb(46, 107, 230, 255);
    lh_gl_color_t ink = rgb(255, 255, 255, 255);
    lh_gl_paint_t paint;

    lh_gl_style_init(&card_style);
    lh_gl_paint_init_color(&paint, &fill);
    lh_gl_style_assign_fill(&card_style, &paint);
    lh_gl_paint_init_color(&paint, &ink);
    lh_gl_style_assign_text(&card_style, &paint);
    lh_gl_style_set_radius_v(&card_style, lh_gl_scalar(8));
    lh_gl_style_set_padding_all(&card_style, lh_gl_scalar(8));
}

static void
gradient_fill(int linear, int degrees)
{
    lh_gl_color_t stops[2];
    lh_gl_paint_t paint;

    stops[0] = rgb(46, 107, 230, 255);
    stops[1] = rgb(155, 77, 230, 255);
    lh_pixel_gradient_init(&gradient);
    lh_gl_gradient_init_stops(&gradient, stops, NULL, 2);
    if (linear)
    {
        lh_pixel_gradient_set_linear(&gradient, degrees);
    }
    else
    {
        lh_pixel_gradient_set_radial(&gradient);
    }
    lh_gl_paint_init_gradient(&paint, &gradient);
    lh_gl_style_assign_fill(&card_style, &paint);
}

int
lh_scene_has(int scene)
{
    return scene >= 0 && scene < SCENE_COUNT;
}

void
lh_scene_setup(int scene)
{
    lh_gl_size_t size;
    lh_gl_color_t bg = rgb(20, 24, 29, 255);
    lh_gl_paint_t paint;
    int i;

    lh_gl_pixmap_init(&pixmap, (lh_byte_t *)pixels, SCENE_W, SCENE_H, SCENE_W * 4, lh_gl_pixmap_format_argb8888);
    lh_gl_canvas_sw_init(&sw);
    lh_gl_canvas_sw_use_pixmap(&sw, &pixmap);
    lh_gl_canvas_init(&canvas, &lh_gl_canvas_backend_sw, &sw);
    lh_gl_size_init(&size, lh_gl_scalar(SCENE_W), lh_gl_scalar(SCENE_H));
    lh_gl_canvas_set_size(&canvas, size);

    lh_gl_style_init(&root_style);
    lh_gl_paint_init_color(&paint, &bg);
    lh_gl_style_assign_fill(&root_style, &paint);
    lh_gl_container_init(&root, rect(0, 0, SCENE_W, SCENE_H));
    lh_gl_entity_set_style_v(lh_gl_container_as_entity(&root), &root_style);
    card_look();

    switch (scene)
    {
    case SCENE_GRAD_VERTICAL:
        gradient_fill(1, 180);
        break;
    case SCENE_GRAD_DIAGONAL:
        gradient_fill(1, 135);
        break;
    case SCENE_GRAD_RADIAL:
        gradient_fill(0, 0);
        break;
    case SCENE_SHADOW:
        lh_gl_shadow_init(&shadow);
        lh_gl_shadow_set_color(&shadow, rgb(0, 0, 0, 128));
        lh_gl_shadow_set_spread_v(&shadow, lh_gl_scalar(12));
        lh_gl_shadow_set_offset(&shadow, lh_gl_scalar(0), lh_gl_scalar(4));
        lh_gl_style_set_shadow_v(&card_style, &shadow);
        break;
    case SCENE_CORNERS:
    {
        lh_gl_radii_t radii;
        lh_gl_radii_init(&radii, lh_gl_scalar(12), lh_gl_scalar(12), lh_gl_scalar(0), lh_gl_scalar(0));
        lh_gl_style_set_radii_v(&card_style, &radii);
        break;
    }
    default:
        break;
    }

    if (scene == SCENE_TEXT)
    {
        lh_gl_color_t panel = rgb(42, 49, 59, 255);
        lh_gl_paint_init_color(&paint, &panel);
        lh_gl_style_assign_fill(&card_style, &paint);
        lh_gl_style_set_radius_v(&card_style, lh_gl_scalar(6));
        lh_gl_style_set_wrap(&card_style, lh_gl_text_wrap_words);
        for (i = 0; i < PARAGRAPHS; ++i)
        {
            const lh_gl_rect_t r = rect(8 + (i % 2) * 236, 8 + (i / 2) * 156, 228, 148);
            lh_gl_label_init(&paragraphs[i], r, scene_paragraph);
            lh_gl_entity_set_style_v(lh_gl_label_as_entity(&paragraphs[i]), &card_style);
            lh_gl_entity_add_child(lh_gl_container_as_entity(&root), lh_gl_label_as_entity(&paragraphs[i]));
        }
        partial = rect(8, 8, 228, 148);
    }
    else if (scene == SCENE_FLEX)
    {
        lh_gl_place_t fill;

        lh_gl_style_init(&row_style);
        lh_gl_style_set_padding_all(&root_style, lh_gl_scalar(8));
        lh_gl_layout_init(&down, lh_gl_axis_vertical, lh_gl_scalar(8));
        lh_gl_layout_init(&across, lh_gl_axis_horizontal, lh_gl_scalar(8));
        lh_gl_container_set_layout(&root, &down);
        lh_gl_place_init(&fill, lh_gl_place_size_fill, lh_gl_scalar(1));
        lh_gl_place_set_align(&fill, lh_gl_place_align_fill);
        for (i = 0; i < ROWS; ++i)
        {
            lh_gl_container_init(&rows[i], rect(0, 0, 0, 0));
            lh_gl_container_set_layout(&rows[i], &across);
            lh_gl_entity_set_place_v(lh_gl_container_as_entity(&rows[i]), &fill);
            lh_gl_entity_add_child(lh_gl_container_as_entity(&root), lh_gl_container_as_entity(&rows[i]));
        }
        for (i = 0; i < CELLS; ++i)
        {
            snprintf(captions[i], sizeof captions[i], "Item %d", i + 1);
            lh_gl_label_init(&cells[i], rect(0, 0, 0, 0), captions[i]);
            lh_gl_entity_set_style_v(lh_gl_label_as_entity(&cells[i]), &card_style);
            lh_gl_entity_set_place_v(lh_gl_label_as_entity(&cells[i]), &fill);
            lh_gl_entity_add_child(lh_gl_container_as_entity(&rows[i / 4]), lh_gl_label_as_entity(&cells[i]));
        }
        partial = rect(8, 8, 108, 26);
    }
    else
    {
        for (i = 0; i < CARDS; ++i)
        {
            const lh_gl_rect_t r = rect(8 + (i % 6) * 78, 8 + (i / 6) * 78, 72, 66);
            snprintf(captions[i], sizeof captions[i], "Card %d", i + 1);
            lh_gl_label_init(&cards[i], r, captions[i]);
            lh_gl_entity_set_style_v(lh_gl_label_as_entity(&cards[i]), &card_style);
            lh_gl_entity_add_child(lh_gl_container_as_entity(&root), lh_gl_label_as_entity(&cards[i]));
        }
        partial = rect(8, 8, 72, 66);
    }

    lh_gl_view_init(&view);
    lh_gl_view_set_canvas(&view, &canvas);
    lh_gl_view_set_root_v(&view, lh_gl_container_as_entity(&root));
    clear_color = bg;
    lh_gl_view_set_clear(&view, &clear_color);
    lh_gl_view_paint(&view);
}

void
lh_scene_frame(void)
{
    lh_gl_view_paint(&view);
}

void
lh_scene_frame_partial(void)
{
    lh_gl_view_draw(&view, &partial);
}

const uint32_t *
lh_scene_pixels(void)
{
    return pixels;
}

void
lh_scene_teardown(void)
{
    lh_gl_view_deinit(&view);
}

void
lh_micro_box(int kind, int radius)
{
    static int ready;
    static lh_pixel_gradient_t micro_gradient;
    lh_gl_paint_t paint;
    lh_gl_radii_t radii;
    lh_gl_rect_t box = rect(8, 8, 72, 66);
    lh_gl_color_t fill = rgb(46, 107, 230, 255);

    if (!ready)
    {
        lh_gl_pixmap_init(&pixmap, (lh_byte_t *)pixels, SCENE_W, SCENE_H, SCENE_W * 4, lh_gl_pixmap_format_argb8888);
        lh_gl_canvas_sw_init(&sw);
        lh_gl_canvas_sw_use_pixmap(&sw, &pixmap);
        lh_gl_canvas_init(&canvas, &lh_gl_canvas_backend_sw, &sw);
        ready = 1;
    }
    if (kind == 0)
    {
        lh_gl_paint_init_color(&paint, &fill);
    }
    else
    {
        lh_gl_color_t stops[2];

        stops[0] = fill;
        stops[1] = rgb(155, 77, 230, 255);
        lh_pixel_gradient_init(&micro_gradient);
        lh_gl_gradient_init_stops(&micro_gradient, stops, NULL, 2);
        if (kind == 3)
        {
            lh_pixel_gradient_set_radial(&micro_gradient);
        }
        else
        {
            lh_pixel_gradient_set_linear(&micro_gradient, kind == 1 ? 180 : 135);
        }
        lh_gl_paint_init_gradient(&paint, &micro_gradient);
    }
    lh_gl_radii_init_all(&radii, lh_gl_scalar(radius));
    lh_gl_canvas_fill_box(&canvas, &box, &radii, &paint);
}

void
lh_micro_text(void)
{
    lh_gl_color_t ink = rgb(255, 255, 255, 255);
    lh_gl_point_t at;

    lh_micro_box(0, 0);
    lh_gl_point_init(&at, lh_gl_scalar(16), lh_gl_scalar(16));
    lh_gl_text_draw(&canvas, lh_gl_font_get_default(), "Card 12", at, &ink);
}
