#include <gtest/gtest.h>

#include <lh/entity/screen.h>
#include <lh/memory/allocator/initializer.h>
#include <lh/null.h>
#include <lh/self.h>

#include <cstdlib>
#include <vector>

namespace
{

const lh_float_t k_pi = 3.14159265f;
const int k_width = 16;
const int k_height = 12;
const lh_ui_color_t k_black = lh_ui_color_make(0, 0, 0, 255);
const lh_ui_color_t k_red = lh_ui_color_make(255, 0, 0, 255);
const lh_ui_color_t k_green = lh_ui_color_make(0, 255, 0, 255);

#include <lh/compiler/extern/c.h>

LH_COMPILER_EXTERN_C_BEGIN

lh_ptr
screen_test_alloc(lh_self_ptr, lh_usize_t size)
{
    return std::malloc(static_cast<std::size_t>(size));
}

lh_void
screen_test_dealloc(lh_self_ptr, lh_ptr ptr)
{
    std::free(ptr);
}

LH_COMPILER_EXTERN_C_END

bool
same(lh_ui_color_t a, lh_ui_color_t b)
{
    return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
}

class Screen : public ::testing::Test
{
  protected:
    void
    SetUp() override
    {
        sized = lh_memory_allocator_initializer_with_context(screen_test_alloc, screen_test_dealloc,
                                                             lh_null, lh_null);
        screen = reinterpret_cast<lh_entity_screen_t *>(
            lh_entity_create_root(&lh_entity_screen_class, &sized));
        lh_entity_rect_set_size(reinterpret_cast<lh_entity_rect_t *>(screen),
                                lh_vec2_make(k_width, k_height));
        pixels.assign(k_width * k_height, lh_ui_color_make(1, 2, 3, 4));
        lh_ui_canvas_init(&canvas, pixels.data(), k_width, k_height, k_width);
    }

    void
    TearDown() override
    {
        lh_entity_delete(root());
    }

    lh_entity_t *
    root()
    {
        return reinterpret_cast<lh_entity_t *>(screen);
    }

    lh_entity_rect_t *
    make_rect(lh_entity_t *parent, lh_float_t x, lh_float_t y, lh_float_t w, lh_float_t h,
              lh_ui_color_t color)
    {
        lh_entity_rect_t *r =
            reinterpret_cast<lh_entity_rect_t *>(lh_entity_create(&lh_entity_rect_class, parent));
        lh_entity_2d_set_position(reinterpret_cast<lh_entity_2d_t *>(r), lh_vec2_make(x, y));
        lh_entity_rect_set_size(r, lh_vec2_make(w, h));
        lh_entity_rect_set_color(r, color);
        return r;
    }

    lh_ui_color_t
    at(int x, int y)
    {
        return lh_ui_canvas_get_pixel(&canvas, x, y);
    }

    lh_ui_rect_t
    render()
    {
        return lh_entity_screen_render(screen, &canvas);
    }

    lh_memory_sized_allocator_t sized;
    lh_entity_screen_t *screen;
    std::vector<lh_ui_color_t> pixels;
    lh_ui_canvas_t canvas;
};

TEST_F(Screen, first_render_fills_the_whole_display)
{
    ASSERT_EQ(lh_entity_screen_get_dirty_count(screen), 1u);
    const lh_ui_rect_t drawn = render();
    EXPECT_EQ(drawn.size.width, k_width);
    EXPECT_EQ(drawn.size.height, k_height);
    EXPECT_TRUE(same(at(0, 0), k_black));
    EXPECT_TRUE(same(at(k_width - 1, k_height - 1), k_black));
    EXPECT_EQ(lh_entity_screen_get_dirty_count(screen), 0u);
}

TEST_F(Screen, rect_covers_the_pixels_whose_centers_it_contains)
{
    make_rect(root(), 2, 1, 3, 2, k_red);
    render();
    EXPECT_TRUE(same(at(2, 1), k_red));
    EXPECT_TRUE(same(at(4, 2), k_red));
    EXPECT_TRUE(same(at(5, 1), k_black));
    EXPECT_TRUE(same(at(2, 3), k_black));
    EXPECT_TRUE(same(at(1, 1), k_black));
}

TEST_F(Screen, only_dirty_areas_are_redrawn)
{
    lh_entity_rect_t *r = make_rect(root(), 0, 0, 2, 2, k_red);
    render();
    pixels[10 * k_width + 10] = k_green; // a mark far from any change

    lh_entity_2d_set_position(reinterpret_cast<lh_entity_2d_t *>(r), lh_vec2_make(4, 0));
    ASSERT_GE(lh_entity_screen_get_dirty_count(screen), 1u);
    const lh_ui_rect_t drawn = render();

    EXPECT_TRUE(same(at(0, 0), k_black));   // where it was: background again
    EXPECT_TRUE(same(at(4, 0), k_red));     // where it is now
    EXPECT_TRUE(same(at(10, 10), k_green)); // untouched: not redrawn
    EXPECT_LE(drawn.origin.x + drawn.size.width, 6);
    EXPECT_LE(drawn.origin.y + drawn.size.height, 2);
}

TEST_F(Screen, children_are_cut_to_the_parent_unless_overflow_visible)
{
    lh_entity_rect_t *panel = make_rect(root(), 2, 2, 4, 4, k_black);
    lh_entity_t *panel_entity = reinterpret_cast<lh_entity_t *>(panel);
    make_rect(panel_entity, 2, 2, 6, 1, k_red); // reaches past the panel's right edge (x 4..10)
    render();
    EXPECT_TRUE(same(at(5, 4), k_red));
    EXPECT_TRUE(same(at(7, 4), k_black)); // cut

    // A child outside its parent cannot be clicked either.
    EXPECT_EQ(lh_entity_rect_find_at(root(), lh_vec2_make(7.5f, 4.5f)), root());

    lh_entity_add_flags(panel_entity, lh_entity_flags_overflow_visible);
    lh_entity_screen_invalidate_area(screen, lh_ui_rect_make(0, 0, k_width, k_height));
    render();
    EXPECT_TRUE(same(at(7, 4), k_red));
}

TEST_F(Screen, hidden_entities_are_not_drawn_or_hit)
{
    lh_entity_rect_t *r = make_rect(root(), 0, 0, 3, 3, k_red);
    lh_entity_t *e = reinterpret_cast<lh_entity_t *>(r);
    render();
    ASSERT_TRUE(same(at(1, 1), k_red));

    lh_entity_add_flags(e, lh_entity_flags_hidden);
    lh_entity_invalidate(e);
    render();
    EXPECT_TRUE(same(at(1, 1), k_black));
    EXPECT_EQ(lh_entity_rect_find_at(root(), lh_vec2_make(1, 1)), root());
}

TEST_F(Screen, deleting_redraws_what_was_under)
{
    lh_entity_rect_t *r = make_rect(root(), 5, 5, 2, 2, k_red);
    render();
    ASSERT_TRUE(same(at(5, 5), k_red));

    lh_entity_delete(reinterpret_cast<lh_entity_t *>(r));
    EXPECT_GE(lh_entity_screen_get_dirty_count(screen), 1u);
    render();
    EXPECT_TRUE(same(at(5, 5), k_black));
}

TEST_F(Screen, rotated_rect_is_drawn_rotated)
{
    // A 4x4 square turned 45 degrees about its corner at (8,2): a diamond.
    lh_entity_rect_t *r = make_rect(root(), 8, 2, 4, 4, k_red);
    lh_entity_2d_set_angle(reinterpret_cast<lh_entity_2d_t *>(r), k_pi / 4);
    render();
    EXPECT_TRUE(same(at(7, 4), k_red));    // inside the diamond, left of the corner
    EXPECT_TRUE(same(at(8, 6), k_red));    // further down, below the middle
    EXPECT_TRUE(same(at(10, 2), k_black)); // where the unrotated square would be
}

TEST_F(Screen, younger_siblings_draw_over_older_ones)
{
    make_rect(root(), 0, 0, 4, 4, k_red);
    make_rect(root(), 2, 2, 4, 4, k_green);
    render();
    EXPECT_TRUE(same(at(1, 1), k_red));
    EXPECT_TRUE(same(at(3, 3), k_green));
}

TEST_F(Screen, many_small_changes_merge_into_one_area)
{
    render();
    for (int i = 0; i < LH_ENTITY_SCREEN_DIRTY_MAX + 3; ++i)
    {
        lh_entity_screen_invalidate_area(screen,
                                         lh_ui_rect_make(i % k_width, (i * 3) % k_height, 1, 1));
    }
    EXPECT_LE(lh_entity_screen_get_dirty_count(screen),
              static_cast<lh_usize_t>(LH_ENTITY_SCREEN_DIRTY_MAX));
    render();
    EXPECT_EQ(lh_entity_screen_get_dirty_count(screen), 0u);
}

} // namespace
