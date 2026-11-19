/**
 * @file render.cpp
 * @brief Tests for `lh/gx/render.h`, `lh/gx/class.h` and `lh/gx/paint.h`.
 *
 * The first check of the pipeline: a card changes its colour, and only its
 * rectangle is drawn again and handed to the present. Around it: the first frame
 * draws everything, an opaque card hides what is under it from the walk, a
 * hidden entity is not drawn, a child is drawn inside its owner, a move reports
 * where it was and where it is, and a strip draws its own rows. The class here is
 * the test's own — a solid card whose colour is its content.
 */

#include <gtest/gtest.h>

#include <lh/gx/backend/sw.h>
#include <lh/gx/class/initializer.h>
#include <lh/gx/render.h>

namespace
{

/* The content of a solid card: its colour, and how many times it was drawn. */
struct Solid
{
    lh_gx_color_t color;
    int paints;
};

lh_void
solid_paint(const lh_gx_entity_t *entity, const lh_gx_paint_t *paint)
{
    auto *solid = static_cast<Solid *>(lh_gx_entity_get_content(entity));

    ++solid->paints;
    lh_gx_paint_fill_rect_color(paint, lh_gx_paint_get_place(paint), solid->color);
}

lh_bool_t
solid_covers(const lh_gx_entity_t *entity, const lh_gx_rect_t *, const lh_gx_rect_t *)
{
    const auto *solid = static_cast<const Solid *>(lh_gx_entity_get_content(entity));

    return lh_cast_static(lh_bool_t, (solid->color >> 24) == 0xFFU);
}

const lh_gx_class_t solid_class =
    lh_gx_class_initializer(solid_paint, solid_covers, lh_null, lh_null, lh_null, lh_null, lh_null);

/* A 16 by 8 screen in ARGB, drawn by the software backend. */
struct Screen
{
    lh_u32_t words[16 * 8];
    lh_memory_bounds_t bits;
    lh_gx_pixmap_t pixmap;
    lh_gx_backend_sw_t sw;
    lh_gx_canvas_t canvas;
    lh_gx_render_t render;
    Solid back_solid;
    Solid card_solid;
    lh_gx_entity_t back;
    lh_gx_entity_t card;

    Screen() : words{}, back_solid{0xFF000000U, 0}, card_solid{0xFFFF0000U, 0}
    {
        lh_memory_bounds_init(&bits, words, sizeof(words));
        lh_gx_pixmap_init(&pixmap, &bits, 16U, 8U, lh_gx_pixmap_format_argb8888);
        lh_gx_backend_sw_init(&sw, &pixmap, 0, 0);
        lh_gx_backend_sw_canvas_init(&canvas);
        lh_gx_render_init(&render, &canvas, &sw, 16, 8);
        lh_gx_entity_init(&back, &solid_class, nullptr, 0, 0, 16, 8);
        lh_gx_entity_set_content(&back, &back_solid);
        lh_gx_entity_init(&card, &solid_class, nullptr, 2, 2, 4, 3);
        lh_gx_entity_set_content(&card, &card_solid);
        lh_gx_entity_attach(lh_gx_render_get_screen_mut(&render), &back);
        lh_gx_entity_attach(&back, &card);
    }

    lh_u32_t
    at(int x, int y) const
    {
        return words[y * 16 + x];
    }

    lh_gx_damage_t
    draw()
    {
        lh_gx_damage_t drawn;

        lh_gx_render_draw(&render, &drawn);
        return drawn;
    }
};

lh_gx_rect_t
rect(lh_gx_scalar_t x, lh_gx_scalar_t y, lh_gx_scalar_t w, lh_gx_scalar_t h)
{
    lh_gx_rect_t r;

    lh_gx_rect_init(&r, x, y, w, h);
    return r;
}

TEST(gx_class, a_new_table_has_every_slot_empty)
{
    lh_gx_class_t klass;

    lh_gx_class_init(&klass);
    EXPECT_TRUE(lh_gx_class_get_paint(&klass) == nullptr);
    EXPECT_TRUE(lh_gx_class_get_covers(&klass) == nullptr);
    EXPECT_TRUE(lh_gx_class_get_hit(&klass) == nullptr);
    EXPECT_TRUE(lh_gx_class_get_input(&klass) == nullptr);
    EXPECT_TRUE(lh_gx_class_get_key(&klass) == nullptr);
    EXPECT_TRUE(lh_gx_class_get_report(&klass) == nullptr);
    lh_gx_class_set_paint(&klass, solid_paint);
    EXPECT_TRUE(lh_gx_class_get_paint(&klass) == solid_paint);
    EXPECT_TRUE(lh_gx_class_get_covers(&solid_class) == solid_covers) << "a constant table";
}

TEST(gx_render, the_first_frame_draws_everything)
{
    Screen screen;
    const lh_gx_damage_t drawn = screen.draw();

    EXPECT_EQ(lh_gx_damage_get_count(&drawn), 1U);
    EXPECT_EQ(lh_gx_damage_area(&drawn), 16U * 8U);
    EXPECT_EQ(screen.at(0, 0), 0xFF000000U);
    EXPECT_EQ(screen.at(2, 2), 0xFFFF0000U);
    EXPECT_EQ(screen.at(5, 4), 0xFFFF0000U);
    EXPECT_EQ(screen.at(6, 4), 0xFF000000U);
    EXPECT_EQ(lh_gx_render_has_damage(&screen.render), lh_bool_false);
}

/* The check the pipeline was built for. */
TEST(gx_render, a_card_that_changes_colour_is_the_only_thing_drawn)
{
    Screen screen;

    (void)screen.draw();
    screen.words[6 * 16 + 10] = 0x12345678U; /* a pixel nothing should touch now */
    screen.card_solid.color = 0xFF00FF00U;
    lh_gx_entity_invalidate(&screen.card);
    const lh_gx_rect_t card = rect(2, 2, 4, 3);
    const lh_gx_damage_t drawn = screen.draw();

    ASSERT_EQ(lh_gx_damage_get_count(&drawn), 1U);
    EXPECT_EQ(lh_gx_rect_eq(lh_gx_damage_get_rect(&drawn, 0U), &card), lh_bool_true);
    EXPECT_EQ(screen.at(3, 3), 0xFF00FF00U);
    EXPECT_EQ(screen.at(10, 6), 0x12345678U) << "outside the card nothing was drawn";
}

/* The card is opaque over its own rectangle: the walk starts from it. */
TEST(gx_render, an_opaque_card_hides_what_is_under_it_from_the_walk)
{
    Screen screen;

    (void)screen.draw();
    const int back_paints = screen.back_solid.paints;

    lh_gx_entity_invalidate(&screen.card);
    (void)screen.draw();
    EXPECT_EQ(screen.back_solid.paints, back_paints) << "the background was not drawn";
    EXPECT_EQ(screen.card_solid.paints, 2);

    screen.card_solid.color = 0x80FFFFFFU; /* translucent: no longer covers */
    lh_gx_entity_invalidate(&screen.card);
    (void)screen.draw();
    EXPECT_EQ(screen.back_solid.paints, back_paints + 1);
}

TEST(gx_render, a_hidden_card_is_not_drawn_and_what_it_hid_comes_back)
{
    Screen screen;

    (void)screen.draw();
    lh_gx_entity_set_shown(&screen.card, lh_bool_false);
    const lh_gx_damage_t drawn = screen.draw();

    EXPECT_EQ(lh_gx_damage_area(&drawn), 4U * 3U);
    EXPECT_EQ(screen.at(3, 3), 0xFF000000U);
    lh_gx_entity_invalidate(&screen.card);
    EXPECT_EQ(lh_gx_render_has_damage(&screen.render), lh_bool_false) << "hidden reports nothing";
}

TEST(gx_render, a_move_reports_where_it_was_and_where_it_is)
{
    Screen screen;

    (void)screen.draw();
    lh_gx_entity_move(&screen.card, rect(10, 2, 4, 3));
    const lh_gx_damage_t drawn = screen.draw();

    EXPECT_EQ(lh_gx_damage_area(&drawn), 2U * 4U * 3U);
    EXPECT_EQ(screen.at(3, 3), 0xFF000000U) << "the old place is the background again";
    EXPECT_EQ(screen.at(11, 3), 0xFFFF0000U);
}

TEST(gx_render, a_child_is_drawn_inside_its_owner)
{
    Screen screen;
    Solid inner_solid{0xFF0000FFU, 0};
    lh_gx_entity_t inner;

    lh_gx_entity_init(&inner, &solid_class, nullptr, 2, 1, 10, 1); /* far past the card */
    lh_gx_entity_set_content(&inner, &inner_solid);
    lh_gx_entity_attach(&screen.card, &inner);
    (void)screen.draw();

    EXPECT_EQ(screen.at(4, 3), 0xFF0000FFU) << "card (2, 2) + inner (2, 1)";
    EXPECT_EQ(screen.at(5, 3), 0xFF0000FFU);
    EXPECT_EQ(screen.at(6, 3), 0xFF000000U) << "past the card's right edge";
    lh_gx_entity_detach(&inner);
}

TEST(gx_render, a_tree_not_on_a_screen_reports_nowhere)
{
    Solid solid{0xFFFFFFFFU, 0};
    lh_gx_entity_t alone;

    lh_gx_entity_init(&alone, &solid_class, nullptr, 0, 0, 4, 4);
    lh_gx_entity_set_content(&alone, &solid);
    lh_gx_entity_invalidate(&alone);
    lh_gx_entity_move(&alone, rect(1, 1, 4, 4));
    SUCCEED();
}

/* A strip: the backend's origin moved down, the area cut to its rows. */
TEST(gx_render, a_strip_draws_its_own_rows)
{
    Screen screen;
    lh_u32_t strip_words[16 * 2] = {};
    lh_memory_bounds_t strip_bits;
    lh_gx_pixmap_t strip;

    lh_memory_bounds_init(&strip_bits, strip_words, sizeof(strip_words));
    lh_gx_pixmap_init(&strip, &strip_bits, 16U, 2U, lh_gx_pixmap_format_argb8888);
    lh_gx_backend_sw_init(&screen.sw, &strip, 0, 3);
    const lh_gx_rect_t rows = rect(0, 3, 16, 2);

    lh_gx_render_paint_area(&screen.render, &rows);
    EXPECT_EQ(strip_words[0], 0xFF000000U) << "screen (0, 3)";
    EXPECT_EQ(strip_words[2], 0xFFFF0000U) << "screen (2, 3) is the card";
    EXPECT_EQ(strip_words[16 + 2], 0xFFFF0000U) << "screen (2, 4) is the card";
}

/* The walks keep no stack of their own: a chain far deeper than any recursion
   would like is drawn, and its innermost entity — inside every owner — is reached. */
TEST(gx_render, a_very_deep_tree_is_walked_without_recursion)
{
    Screen screen;
    static lh_gx_entity_t chain[4096];
    Solid leaf{0xFF00FFFFU, 0};

    lh_gx_entity_t *owner = &screen.card;
    for (auto &link : chain)
    {
        lh_gx_entity_init(&link, nullptr, nullptr, 0, 0, 4, 3);
        lh_gx_node_attach(lh_gx_entity_get_node_mut(owner), lh_gx_entity_get_node_mut(&link));
        owner = &link;
    }
    lh_gx_entity_set_class(owner, &solid_class);
    lh_gx_entity_set_content(owner, &leaf);
    (void)screen.draw();

    EXPECT_EQ(leaf.paints, 1);
    EXPECT_EQ(screen.at(3, 3), 0xFF00FFFFU);
    lh_gx_entity_detach(&chain[0]);
}

/* Up and across: after a deep subtree the next sibling of an owner is drawn with
   its own owner's clip and position, found again from the owners. */
TEST(gx_render, the_walk_climbs_back_with_the_right_clip)
{
    Screen screen;
    Solid deep_solid{0xFF0000FFU, 0};
    Solid after_solid{0xFF00FF00U, 0};
    lh_gx_entity_t layer;
    lh_gx_entity_t deep;
    lh_gx_entity_t after;

    lh_gx_entity_init(&layer, nullptr, nullptr, 1, 1, 2, 2);
    lh_gx_entity_init(&deep, &solid_class, nullptr, 0, 0, 2, 2);
    lh_gx_entity_set_content(&deep, &deep_solid);
    lh_gx_entity_init(&after, &solid_class, nullptr, 0, 0, 20, 1); /* wider than the card */
    lh_gx_entity_set_content(&after, &after_solid);
    lh_gx_entity_attach(&screen.card, &layer);
    lh_gx_entity_attach(&layer, &deep);
    lh_gx_entity_attach(&screen.card, &after);
    (void)screen.draw();

    EXPECT_EQ(screen.at(3, 3), 0xFF0000FFU) << "card (2, 2) + layer (1, 1)";
    EXPECT_EQ(screen.at(5, 2), 0xFF00FF00U) << "after, the card's last column";
    EXPECT_EQ(screen.at(6, 2), 0xFF000000U) << "after is cut by the card, not by the layer";
    lh_gx_entity_detach(&layer);
    lh_gx_entity_detach(&after);
}

TEST(gx_render, the_owner_clip_is_every_owner_on_the_way_up)
{
    Screen screen;
    lh_gx_entity_t inner;
    lh_gx_rect_t clip;
    lh_gx_point_t origin;

    lh_gx_entity_init(&inner, nullptr, nullptr, 1, 1, 10, 10);
    lh_gx_entity_attach(&screen.card, &inner);
    const lh_gx_rect_t everything = rect(0, 0, 16, 8);
    const lh_gx_rect_t card = rect(2, 2, 4, 3);

    lh_gx_entity_get_owner_clip(&inner, &everything, &clip);
    EXPECT_EQ(lh_gx_rect_eq(&clip, &card), lh_bool_true)
        << "cut by the card, the back and the screen";
    lh_gx_entity_get_owner_origin(&inner, &origin);
    EXPECT_EQ(lh_gx_point_get_x(&origin), 2);
    EXPECT_EQ(lh_gx_point_get_y(&origin), 2);
    lh_gx_entity_detach(&inner);
}

/* A canvas without fill_mask: the paint brings the mask down to one-pixel rects. */
TEST(gx_paint, a_mask_without_fill_mask_is_drawn_through_fill_rect)
{
    lh_u32_t words[4] = {0xFF000000U, 0xFF000000U, 0xFF000000U, 0xFF000000U};
    lh_memory_bounds_t bits;
    lh_gx_pixmap_t target;
    lh_byte_t cover[2] = {255U, 0U};
    lh_memory_bounds_t cover_bits;
    lh_gx_pixmap_t mask;
    lh_gx_backend_sw_t sw;
    lh_gx_canvas_t canvas;
    lh_gx_paint_t paint;
    lh_gx_point_t at;

    lh_memory_bounds_init(&bits, words, sizeof(words));
    lh_gx_pixmap_init(&target, &bits, 4U, 1U, lh_gx_pixmap_format_argb8888);
    lh_memory_bounds_init(&cover_bits, cover, sizeof(cover));
    lh_gx_pixmap_init(&mask, &cover_bits, 2U, 1U, lh_gx_pixmap_format_a8);
    lh_gx_backend_sw_init(&sw, &target, 0, 0);
    lh_gx_canvas_init(&canvas);
    lh_gx_canvas_set_fill_rect(&canvas, lh_gx_backend_sw_fill_rect);
    lh_gx_paint_init(&paint, &canvas, &sw, rect(0, 0, 4, 1), rect(0, 0, 4, 1));
    lh_gx_point_init(&at, 1, 0);
    lh_gx_paint_fill_mask_color(&paint, &mask, at, 0xFFFFFFFFU);

    EXPECT_EQ(words[0], 0xFF000000U);
    EXPECT_EQ(words[1], 0xFFFFFFFFU);
    EXPECT_EQ(words[2], 0xFF000000U) << "no coverage";
}

} // namespace
