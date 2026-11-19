/*
 * scene_gx.c -- the scenes of scene.h that gx can draw, built with gx's own API and
 * drawn by its software backend into memory.
 *
 * gx draws what it has a class for: boxes of one colour or a gradient, each corner
 * of its own radius, with a soft shadow, and one-line text -- the `cards`,
 * `corners`, `grad_*` and `shadow` scenes. The others need what gx does not have
 * yet -- wrapped paragraphs, a flex layout -- and are left out (gx_scene_has).
 * A frame is the renderer's: the damage is the area asked for, the renderer walks the
 * tree and paints it from the entity that covers it.
 */

#include "scene.h"

#include <stdio.h>

#include <lh/gl/font.h>
#include <lh/gx/backend/sw.h>
#include <lh/gx/box.h>
#include <lh/gx/font/baked.h>
#include <lh/gx/render.h>
#include <lh/gx/text.h>

#define CARDS 24

static uint32_t pixels[SCENE_W * SCENE_H];
static lh_memory_bounds_t bits;
static lh_gx_pixmap_t pixmap;
static lh_gx_backend_sw_t sw;
static lh_gx_canvas_t canvas;
static lh_gx_render_t render;
static lh_gx_font_t font;

static lh_pixel_gradient_t gradient;
static lh_pixel_gradient_table_t gradient_table;
static lh_gx_shadow_t shadow;
/* Where the backend keeps a shadow's alphas for the next one like it. */
static lh_byte_t shadow_bits[16384];
static lh_gx_backend_sw_shadow_t shadow_mask;
static lh_gx_style_t back_style;
static lh_gx_style_t card_style;
static lh_gx_entity_t back;
static lh_gx_entity_t cards[CARDS];
static lh_gx_entity_t captions[CARDS];
static char texts[CARDS][12];
static lh_gx_rect_t partial;
static lh_gx_rect_t whole;

int
gx_scene_has(int scene)
{
    return scene == SCENE_CARDS || scene == SCENE_CORNERS || scene == SCENE_GRAD_VERTICAL ||
           scene == SCENE_GRAD_DIAGONAL || scene == SCENE_GRAD_RADIAL || scene == SCENE_SHADOW;
}

void
gx_scene_setup(int scene)
{
    lh_memory_bounds_init(&bits, pixels, sizeof(pixels));
    lh_gx_pixmap_init(&pixmap, &bits, SCENE_W, SCENE_H, lh_gx_pixmap_format_argb8888);
    lh_gx_backend_sw_init(&sw, &pixmap, 0, 0);
    lh_gx_backend_sw_shadow_init(&shadow_mask, shadow_bits, sizeof shadow_bits);
    lh_gx_backend_sw_set_shadow(&sw, &shadow_mask);
    lh_gx_backend_sw_canvas_init(&canvas);
    lh_gx_render_init(&render, &canvas, &sw, SCENE_W, SCENE_H);
    lh_gx_font_baked_init(&font, lh_gl_font_get_default());

    /* The look of scene_lh.c: the same background, cards of one colour, a radius of
       8, white text 8 in from the corner. */
    lh_gx_style_init(&back_style, 0xFF14181DU, 0U, 0, 0, 0U, lh_null);
    lh_gx_style_init(&card_style, 0xFF2E6BE6U, 0U, 0, 8, 0xFFFFFFFFU, &font);
    if (scene == SCENE_GRAD_VERTICAL || scene == SCENE_GRAD_DIAGONAL || scene == SCENE_GRAD_RADIAL)
    {
        /* As scene_lh.c: the card's blue into violet, down, corner to corner, or out
           from the middle. */
        const lh_u32_t stops[2] = {0xFF2E6BE6U, 0xFF9B4DE6U};

        lh_pixel_gradient_init(&gradient);
        lh_pixel_gradient_init_stops(&gradient, stops, lh_null, 2);
        if (scene == SCENE_GRAD_RADIAL)
        {
            lh_pixel_gradient_set_radial(&gradient);
        }
        else
        {
            lh_pixel_gradient_set_linear(&gradient, scene == SCENE_GRAD_VERTICAL ? 180 : 135);
        }
        /* The theme works its colours out once, not every frame. */
        lh_pixel_gradient_table_fill(&gradient_table, &gradient);
        lh_gx_style_set_gradient(&card_style, &gradient);
        lh_gx_style_set_gradient_table(&card_style, &gradient_table);
    }
    if (scene == SCENE_SHADOW)
    {
        /* As scene_lh.c: black at half alpha, fading over 12, 4 down. */
        lh_gx_shadow_init(&shadow, 0x80000000U, 12, 0, 4);
        lh_gx_style_set_shadow(&card_style, &shadow);
    }
    if (scene == SCENE_CORNERS)
    {
        /* As scene_lh.c: rounded 12 on top, square at the bottom -- a tab. */
        lh_gx_corners_init(lh_gx_style_get_radius_mut(&card_style), 12, 12, 0, 0);
    }
    lh_gx_entity_init(&back, &lh_gx_box_class, &back_style, 0, 0, SCENE_W, SCENE_H);
    lh_gx_entity_attach(lh_gx_render_get_screen_mut(&render), &back);
    for (int i = 0; i < CARDS; ++i)
    {
        snprintf(texts[i], sizeof texts[i], "Card %d", i + 1);
        lh_gx_entity_init(&cards[i], &lh_gx_box_class, &card_style, 8 + (i % 6) * 78,
                          8 + (i / 6) * 78, 72, 66);
        lh_gx_entity_init(&captions[i], &lh_gx_text_class, &card_style, 8, 8,
                          lh_gx_text_get_width(&font, texts[i]), lh_gx_font_get_line_height(&font));
        lh_gx_entity_set_content(&captions[i], texts[i]);
        lh_gx_entity_attach(&back, &cards[i]);
        lh_gx_entity_attach(&cards[i], &captions[i]);
    }
    lh_gx_rect_init(&partial, 8, 8, 72, 66);
    lh_gx_rect_init(&whole, 0, 0, SCENE_W, SCENE_H);
    gx_scene_frame();
}

void
gx_scene_frame(void)
{
    lh_gx_damage_t drawn;

    lh_gx_render_add_damage(&render, &whole);
    lh_gx_render_draw(&render, &drawn);
}

void
gx_scene_frame_partial(void)
{
    lh_gx_damage_t drawn;

    lh_gx_render_add_damage(&render, &partial);
    lh_gx_render_draw(&render, &drawn);
}

const uint32_t *
gx_scene_pixels(void)
{
    return pixels;
}

void
gx_scene_teardown(void)
{
    lh_gx_entity_detach(&back);
}
