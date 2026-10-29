/*
 * screen.h -- the three screens the UI benchmarks draw (see view.cpp).
 */

#ifndef LH_BENCH_UI_SCREEN_H
#define LH_BENCH_UI_SCREEN_H

#include <benchmark/benchmark.h>

#include "fixture.h"

#include <lh/ui/label.h>
#include <lh/ui/text.h>
#include <lh/ui/view.h>

#include <cstdio>
#include <string>

namespace lh_bench_ui
{
enum screen_id
{
    screen_cards = 0,
    screen_flex = 1,
    screen_text = 2
};

static const char *const screen_paragraph =
    "The renderer draws every primitive itself, row by row, into the pixmap: straight runs as "
    "one span, anti-aliased edges per pixel with the coverage of the shape.";

/** One screen: its widgets, its styles and the view that draws them on a surface. */
struct screen
{
    surface surf;
    lh_ui_view_t view;
    lh_ui_container_t root;
    lh_ui_label_t labels[32];
    lh_ui_container_t rows[8];
    lh_ui_layout_t down;
    lh_ui_layout_t across;
    lh_ui_place_t fill;
    lh_ui_style_t root_style;
    lh_ui_style_t card_style;
    lh_ui_color_t clear;
    char captions[32][12];
    lh_ui_rect_t partial;

    screen(int id, int format) : surf(format)
    {
        const lh_ui_color_t bg = rgb(20, 24, 29);
        const lh_ui_color_t card = rgb(46, 107, 230);
        const lh_ui_color_t ink = rgb(255, 255, 255);
        lh_ui_paint_t paint;

        lh_ui_style_init(&root_style);
        lh_ui_paint_init_color(&paint, &bg);
        lh_ui_style_assign_fill(&root_style, &paint);
        lh_ui_style_init(&card_style);
        lh_ui_paint_init_color(&paint, &card);
        lh_ui_style_assign_fill(&card_style, &paint);
        lh_ui_paint_init_color(&paint, &ink);
        lh_ui_style_assign_text(&card_style, &paint);
        lh_ui_style_set_radius(&card_style, lh_ui_scalar(8));
        lh_ui_style_set_padding_all(&card_style, lh_ui_scalar(8));

        lh_ui_container_init(&root, rect(0, 0, width, height));
        lh_ui_entity_set_style(lh_ui_container_as_entity(&root), &root_style);

        if (id == screen_cards)
        {
            for (int i = 0; i < 24; ++i)
            {
                std::snprintf(captions[i], sizeof captions[i], "Card %d", i + 1);
                lh_ui_label_init(&labels[i], rect(8 + (i % 6) * 78, 8 + (i / 6) * 78, 72, 66), captions[i]);
                lh_ui_entity_set_style(lh_ui_label_as_entity(&labels[i]), &card_style);
                lh_ui_entity_add_child(lh_ui_container_as_entity(&root), lh_ui_label_as_entity(&labels[i]));
            }
            partial = rect(8, 8, 72, 66);
        }
        else if (id == screen_flex)
        {
            lh_ui_style_set_padding_all(&root_style, lh_ui_scalar(8));
            lh_ui_layout_init(&down, lh_ui_axis_vertical, lh_ui_scalar(8));
            lh_ui_layout_init(&across, lh_ui_axis_horizontal, lh_ui_scalar(8));
            lh_ui_container_set_layout(&root, &down);
            lh_ui_place_init(&fill, lh_ui_place_size_fill, lh_ui_scalar(1));
            lh_ui_place_set_align(&fill, lh_ui_place_align_fill);
            for (int i = 0; i < 8; ++i)
            {
                lh_ui_container_init(&rows[i], rect(0, 0, 0, 0));
                lh_ui_container_set_layout(&rows[i], &across);
                lh_ui_entity_set_place_v(lh_ui_container_as_entity(&rows[i]), &fill);
                lh_ui_entity_add_child(lh_ui_container_as_entity(&root), lh_ui_container_as_entity(&rows[i]));
            }
            for (int i = 0; i < 32; ++i)
            {
                std::snprintf(captions[i], sizeof captions[i], "Item %d", i + 1);
                lh_ui_label_init(&labels[i], rect(0, 0, 0, 0), captions[i]);
                lh_ui_entity_set_style(lh_ui_label_as_entity(&labels[i]), &card_style);
                lh_ui_entity_set_place_v(lh_ui_label_as_entity(&labels[i]), &fill);
                lh_ui_entity_add_child(lh_ui_container_as_entity(&rows[i / 4]), lh_ui_label_as_entity(&labels[i]));
            }
            partial = rect(8, 8, 108, 26);
        }
        else
        {
            lh_ui_style_set_wrap(&card_style, lh_ui_text_wrap_words);
            for (int i = 0; i < 4; ++i)
            {
                lh_ui_label_init(&labels[i], rect(8 + (i % 2) * 236, 8 + (i / 2) * 156, 228, 148),
                                 screen_paragraph);
                lh_ui_entity_set_style(lh_ui_label_as_entity(&labels[i]), &card_style);
                lh_ui_entity_add_child(lh_ui_container_as_entity(&root), lh_ui_label_as_entity(&labels[i]));
            }
            partial = rect(8, 8, 228, 148);
        }

        lh_ui_view_init(&view);
        lh_ui_view_set_canvas(&view, &surf.canvas);
        lh_ui_view_set_root(&view, lh_ui_container_as_entity(&root));
        clear = bg;
        lh_ui_view_set_clear(&view, &clear);
        lh_ui_view_paint(&view);
    }

    ~screen()
    {
        lh_ui_view_deinit(&view);
    }

    screen(const screen &) = delete;
    screen &operator=(const screen &) = delete;
};

inline const char *
screen_name(long id)
{
    return id == screen_cards ? "cards" : (id == screen_flex ? "flex" : "text");
}

inline void
screen_label(benchmark::State &state)
{
    std::string s = screen_name(state.range(0));
    s += '/';
    s += format_name(static_cast<int>(state.range(1)));
    state.SetLabel(s);
}


} // namespace lh_bench_ui

#endif /* LH_BENCH_UI_SCREEN_H */
