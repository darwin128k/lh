/*
 * scene.h -- the scenes lh and LVGL both draw, so their frame times can be compared.
 *
 * Every scene is 480 x 320 ARGB8888, drawn by each library's own software renderer into
 * memory, with nothing on screen and no driver: what is timed is the renderer and the
 * work around it (layout, style lookups, damage), not a display.
 */

#ifndef LH_BENCH_LVGL_SCENE_H
#define LH_BENCH_LVGL_SCENE_H

#include <stdint.h>

#define SCENE_W 480
#define SCENE_H 320

#ifdef __cplusplus
extern "C" {
#endif

enum scene_id
{
    SCENE_CARDS = 0,     /* 24 rounded cards of one colour, a caption each */
    SCENE_GRAD_VERTICAL, /* the same cards, a two-stop gradient top to bottom */
    SCENE_GRAD_DIAGONAL, /* a two-stop gradient corner to corner */
    SCENE_GRAD_RADIAL,   /* a two-stop gradient from the middle out */
    SCENE_SHADOW,        /* the cards casting a soft shadow */
    SCENE_TEXT,          /* four paragraphs wrapped at their boxes */
    SCENE_FLEX,          /* 32 cells in flex rows: layout on every frame or not */
    SCENE_CORNERS,       /* a radius per corner: lh only, LVGL has one radius */
    SCENE_COUNT
};

/* What each library implements. `has` is false for a scene the library cannot draw. */
int lh_scene_has(int scene);
void lh_scene_setup(int scene);
void lh_scene_frame(void);         /* the whole screen, redrawn */
void lh_scene_frame_partial(void); /* one card's area, redrawn */
const uint32_t *lh_scene_pixels(void);
void lh_scene_teardown(void);

/* One 72 x 66 box drawn straight on a canvas, no entities: what a card's fill costs
   on its own. @p paint is 0 solid, 1 down, 2 corner to corner, 3 radial. */
void lh_micro_box(int paint, int radius);

/* "Card 12" drawn straight on the same canvas in the default font: what a caption's
   glyphs cost on their own. */
void lh_micro_text(void);

int lvgl_scene_has(int scene);
void lvgl_scene_setup(int scene);
void lvgl_scene_frame(void);
void lvgl_scene_frame_partial(void);
const uint32_t *lvgl_scene_pixels(void);
void lvgl_scene_teardown(void);

/* The text both libraries write, so a paragraph is the same length in each. */
extern const char *const scene_paragraph;

#ifdef __cplusplus
}
#endif

#endif /* LH_BENCH_LVGL_SCENE_H */
