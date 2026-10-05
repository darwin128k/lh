#include <gtest/gtest.h>

#include <lh/entity/3d.h>
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

lh_entity_t *g_pointer_current;
lh_math_vec2_t g_pointer_at;

/* Every event of a press, in the order the screen delivered it, so a test can
   read the whole of one press rather than a single event of it. */
lh_uint_t g_held_codes[8];
lh_int_t g_held_count = 0;

lh_void
screen_test_pointer_handler(lh_entity_event_t *event, lh_ptr)
{
    if (lh_entity_event_get_code(event) != LH_ENTITY_EVENT_POINTER_DOWN)
    {
        return; // e.g. DELETE at teardown, which has no position
    }
    g_pointer_current = lh_entity_event_get_current(event);
    g_pointer_at = *static_cast<const lh_math_vec2_t *>(lh_entity_event_get_param(event));
}

lh_void
screen_test_hold_handler(lh_entity_event_t *event, lh_ptr)
{
    const lh_uint_t code = lh_entity_event_get_code(event);
    if (code != LH_ENTITY_EVENT_POINTER_DOWN && code != LH_ENTITY_EVENT_POINTER_MOVE &&
        code != LH_ENTITY_EVENT_POINTER_UP && code != LH_ENTITY_EVENT_POINTER_CANCEL)
    {
        return; // DRAW, DELETE and the rest are not part of a press
    }
    if (g_held_count < 8)
    {
        g_held_codes[g_held_count] = code;
        g_held_count += 1;
    }
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
        style_count = 0;
        sized = lh_memory_allocator_initializer_with_context(screen_test_alloc, screen_test_dealloc,
                                                             lh_null, lh_null);
        screen = reinterpret_cast<lh_entity_screen_t *>(
            lh_entity_create_root(&lh_entity_screen_class, &sized));
        lh_entity_2d_set_size(reinterpret_cast<lh_entity_2d_t *>(screen),
                              lh_math_vec2_make(k_width, k_height));
        pixels.assign(k_width * k_height, lh_ui_color_make(1, 2, 3, 4));
        depth.assign(k_width * k_height, 0.0f);
        lh_ui_canvas_init(&canvas, pixels.data(), k_width, k_height, k_width);
        lh_ui_canvas_set_depth(&canvas, depth.data());
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

    lh_entity_2d_t *
    make_box(lh_entity_t *parent, lh_float_t x, lh_float_t y, lh_float_t w, lh_float_t h,
             lh_ui_color_t color)
    {
        lh_ui_style_t *style = &styles[style_count++];
        lh_ui_style_init(style);
        lh_ui_style_set_bg_color(style, color);
        lh_entity_2d_t *box =
            reinterpret_cast<lh_entity_2d_t *>(lh_entity_create(&lh_entity_2d_class, parent));
        lh_entity_2d_set_position(box, lh_math_vec2_make(x, y));
        lh_entity_2d_set_size(box, lh_math_vec2_make(w, h));
        lh_entity_2d_set_style(box, style);
        return box;
    }

    lh_entity_t *
    make_layer(lh_float_t x, lh_float_t y, lh_float_t z, lh_float_t w, lh_float_t h,
               lh_ui_color_t color)
    {
        lh_ui_style_t *style = &styles[style_count++];
        lh_ui_style_init(style);
        lh_ui_style_set_bg_color(style, color);
        lh_entity_3d_t *box =
            reinterpret_cast<lh_entity_3d_t *>(lh_entity_create(&lh_entity_3d_class, root()));
        lh_entity_3d_set_position(box, lh_math_vec3_make(x, y, z));
        lh_entity_2d_set_size(reinterpret_cast<lh_entity_2d_t *>(box), lh_math_vec2_make(w, h));
        lh_entity_2d_set_style(reinterpret_cast<lh_entity_2d_t *>(box), style);
        return reinterpret_cast<lh_entity_t *>(box);
    }

    lh_ui_color_t
    at(int x, int y)
    {
        return lh_ui_canvas_get_pixel(&canvas, x, y);
    }

    lh_math_rect_t
    render()
    {
        return lh_entity_screen_render(screen, &canvas);
    }

    lh_memory_sized_allocator_t sized;
    lh_entity_screen_t *screen;
    std::vector<lh_ui_color_t> pixels;
    std::vector<lh_float_t> depth;
    lh_ui_canvas_t canvas;
    lh_ui_style_t styles[8];
    int style_count = 0;
};

TEST_F(Screen, first_render_fills_the_whole_display)
{
    ASSERT_EQ(lh_entity_screen_get_dirty_count(screen), 1u);
    const lh_math_rect_t drawn = render();
    EXPECT_EQ(drawn.size.width, k_width);
    EXPECT_EQ(drawn.size.height, k_height);
    EXPECT_TRUE(same(at(0, 0), k_black));
    EXPECT_TRUE(same(at(k_width - 1, k_height - 1), k_black));
    EXPECT_EQ(lh_entity_screen_get_dirty_count(screen), 0u);
}

TEST_F(Screen, rect_covers_the_pixels_whose_centers_it_contains)
{
    make_box(root(), 2, 1, 3, 2, k_red);
    render();
    EXPECT_TRUE(same(at(2, 1), k_red));
    EXPECT_TRUE(same(at(4, 2), k_red));
    EXPECT_TRUE(same(at(5, 1), k_black));
    EXPECT_TRUE(same(at(2, 3), k_black));
    EXPECT_TRUE(same(at(1, 1), k_black));
}

TEST_F(Screen, only_dirty_areas_are_redrawn)
{
    lh_entity_2d_t *r = make_box(root(), 0, 0, 2, 2, k_red);
    render();
    pixels[10 * k_width + 10] = k_green; // a mark far from any change

    lh_entity_2d_set_position(reinterpret_cast<lh_entity_2d_t *>(r), lh_math_vec2_make(4, 0));
    ASSERT_GE(lh_entity_screen_get_dirty_count(screen), 1u);
    const lh_math_rect_t drawn = render();

    EXPECT_TRUE(same(at(0, 0), k_black));   // where it was: background again
    EXPECT_TRUE(same(at(4, 0), k_red));     // where it is now
    EXPECT_TRUE(same(at(10, 10), k_green)); // untouched: not redrawn
    EXPECT_LE(drawn.origin.x + drawn.size.width, 6);
    EXPECT_LE(drawn.origin.y + drawn.size.height, 2);
}

TEST_F(Screen, children_are_cut_to_the_parent_unless_overflow_visible)
{
    lh_entity_2d_t *panel = make_box(root(), 2, 2, 4, 4, k_black);
    lh_entity_t *panel_entity = reinterpret_cast<lh_entity_t *>(panel);
    make_box(panel_entity, 2, 2, 6, 1, k_red); // reaches past the panel's right edge (x 4..10)
    render();
    EXPECT_TRUE(same(at(5, 4), k_red));
    EXPECT_TRUE(same(at(7, 4), k_black)); // cut

    // A child outside its parent cannot be clicked either.
    EXPECT_EQ(lh_entity_2d_find_at(root(), lh_math_vec2_make(7.5f, 4.5f)), root());

    lh_entity_add_flags(panel_entity, lh_entity_flags_overflow_visible);
    lh_entity_screen_invalidate_area(screen, lh_math_rect_make(0, 0, k_width, k_height));
    render();
    EXPECT_TRUE(same(at(7, 4), k_red));
}

TEST_F(Screen, hidden_entities_are_not_drawn_or_hit)
{
    lh_entity_2d_t *r = make_box(root(), 0, 0, 3, 3, k_red);
    lh_entity_t *e = reinterpret_cast<lh_entity_t *>(r);
    render();
    ASSERT_TRUE(same(at(1, 1), k_red));

    lh_entity_add_flags(e, lh_entity_flags_hidden);
    lh_entity_invalidate(e);
    render();
    EXPECT_TRUE(same(at(1, 1), k_black));
    EXPECT_EQ(lh_entity_2d_find_at(root(), lh_math_vec2_make(1, 1)), root());
}

TEST_F(Screen, deleting_redraws_what_was_under)
{
    lh_entity_2d_t *r = make_box(root(), 5, 5, 2, 2, k_red);
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
    lh_entity_2d_t *r = make_box(root(), 8, 2, 4, 4, k_red);
    lh_entity_2d_set_angle(reinterpret_cast<lh_entity_2d_t *>(r), k_pi / 4);
    render();
    EXPECT_TRUE(same(at(7, 4), k_red));    // well inside, left of the corner
    EXPECT_TRUE(same(at(8, 5), k_red));    // the middle of the diamond
    EXPECT_TRUE(same(at(10, 2), k_black)); // where the unrotated square would be
}

TEST_F(Screen, rotated_rect_has_an_antialiased_edge)
{
    // The same diamond. A turned edge crosses pixels instead of running along
    // their boundaries, so a pixel it passes through is partly covered: not
    // the colour, and not nothing. Before the edge was measured the answer was
    // one or the other, which is what made a turned widget look chewed.
    lh_entity_2d_t *r = make_box(root(), 8, 2, 4, 4, k_red);
    lh_entity_2d_set_angle(r, k_pi / 4);
    render();
    EXPECT_GT(at(7, 2).r, 0);   // the top vertex
    EXPECT_LT(at(7, 2).r, 255);
    EXPECT_GT(at(5, 4).r, 0);   // the left vertex
    EXPECT_LT(at(5, 4).r, 255);
}

TEST_F(Screen, rotated_rect_keeps_its_corner_radius)
{
    // A square turned 45 degrees whose radius is half its side, so the shape is
    // a disc and its corners are nowhere. The radius is a property of the box,
    // not of the screen it lands on, so it survives the turn: the pixel at the
    // corner the unturned square would have reaches nothing, while a square
    // corner fills half of it.
    lh_ui_style_t *style = &styles[style_count++];
    lh_ui_style_init(style);
    lh_ui_style_set_bg_color(style, k_red);
    lh_ui_style_set_radius(style, 4);
    lh_entity_2d_t *r = lh_ptr_rcast(lh_entity_2d_t, lh_entity_create(&lh_entity_2d_class, root()));
    lh_entity_2d_set_position(r, lh_math_vec2_make(8, 2));
    lh_entity_2d_set_size(r, lh_math_vec2_make(8, 8));
    lh_entity_2d_set_style(r, style);
    lh_entity_2d_set_angle(r, k_pi / 4);
    render();
    EXPECT_TRUE(same(at(8, 2), k_black)); // the corner the disc does not reach
    EXPECT_TRUE(same(at(8, 6), k_red));  // and the middle, which it does
}

TEST_F(Screen, rotated_rect_keeps_its_outline)
{
    // An outline is a band around the same rim, so it turns with the box. It
    // used to be dropped outright on a turned box, which left a shape that
    // declared a border and drew none.
    lh_ui_style_t *style = &styles[style_count++];
    lh_ui_style_init(style);
    lh_ui_style_set_bg_color(style, k_red);
    lh_ui_style_set_border_color(style, k_green);
    lh_ui_style_set_border_width(style, 1);
    lh_entity_2d_t *r = lh_ptr_rcast(lh_entity_2d_t, lh_entity_create(&lh_entity_2d_class, root()));
    lh_entity_2d_set_position(r, lh_math_vec2_make(8, 2));
    lh_entity_2d_set_size(r, lh_math_vec2_make(6, 6));
    lh_entity_2d_set_style(r, style);
    lh_entity_2d_set_angle(r, k_pi / 4);
    render();
    // On the rim, the outline is over the fill, so green wins; with the border
    // dropped the same pixel is the plain red of the fill underneath.
    EXPECT_GT(at(4, 5).g, at(4, 5).r);
    EXPECT_TRUE(same(at(8, 6), k_red)); // the middle is still the fill
}

TEST_F(Screen, younger_siblings_draw_over_older_ones)
{
    make_box(root(), 0, 0, 4, 4, k_red);
    make_box(root(), 2, 2, 4, 4, k_green);
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
                                         lh_math_rect_make(i % k_width, (i * 3) % k_height, 1, 1));
    }
    EXPECT_LE(lh_entity_screen_get_dirty_count(screen),
              static_cast<lh_usize_t>(LH_ENTITY_SCREEN_DIRTY_MAX));
    render();
    EXPECT_EQ(lh_entity_screen_get_dirty_count(screen), 0u);
}

TEST_F(Screen, pointer_goes_to_the_rect_on_top_and_bubbles)
{
    lh_entity_2d_t *panel = make_box(root(), 0, 0, 10, 10, k_black);
    lh_entity_2d_t *button = make_box(reinterpret_cast<lh_entity_t *>(panel), 2, 2, 3, 3, k_red);
    lh_entity_t *panel_entity = reinterpret_cast<lh_entity_t *>(panel);
    lh_entity_t *button_entity = reinterpret_cast<lh_entity_t *>(button);
    lh_entity_add_handler(panel_entity, screen_test_pointer_handler, lh_null);

    // Not bubbling: the button gets it, the panel's handler does not run.
    g_pointer_current = nullptr;
    EXPECT_EQ(
        lh_entity_screen_send_pointer(screen, LH_ENTITY_EVENT_POINTER_DOWN, lh_math_vec2_make(3, 3)),
        button_entity);
    EXPECT_EQ(g_pointer_current, nullptr);

    // Bubbling: the panel sees the button's click with its position.
    lh_entity_add_flags(button_entity, lh_entity_flags_event_bubble);
    lh_entity_screen_send_pointer(screen, LH_ENTITY_EVENT_POINTER_DOWN, lh_math_vec2_make(3.5f, 4));
    EXPECT_EQ(g_pointer_current, panel_entity);
    EXPECT_FLOAT_EQ(g_pointer_at.x, 3.5f);
    EXPECT_FLOAT_EQ(g_pointer_at.y, 4.0f);

    EXPECT_EQ(
        lh_entity_screen_send_pointer(screen, LH_ENTITY_EVENT_POINTER_UP, lh_math_vec2_make(12, 1)),
        root()); // only the screen itself is there
}

TEST_F(Screen, nearer_surface_covers_a_farther_one)
{
    // Older and closer. The younger one is on the screen plane.
    lh_entity_t *near = make_layer(2, 2, 1, 4, 4, k_red);
    lh_entity_t *level = make_layer(2, 2, 0, 4, 4, k_green);
    render();
    EXPECT_TRUE(same(at(3, 3), k_red));
    EXPECT_EQ(lh_entity_2d_find_at(root(), lh_math_vec2_make(3.5f, 3.5f)), near);

    // Behind the screen's own surface the background stays in front.
    lh_entity_delete(near);
    lh_entity_delete(level);
    make_layer(2, 2, -1, 4, 4, k_green);
    render();
    EXPECT_TRUE(same(at(3, 3), k_black));
    EXPECT_EQ(lh_entity_2d_find_at(root(), lh_math_vec2_make(3.5f, 3.5f)), root());
}

TEST_F(Screen, tilted_plane_is_covered_only_where_it_is_farther)
{
    // Turned 45 degrees about x: world z equals the screen y, so the top of
    // the plane is closer than a flat box sitting at z = 1.
    lh_entity_3d_t *plane = reinterpret_cast<lh_entity_3d_t *>(make_layer(0, 0, 0, 8, 8, k_red));
    lh_entity_3d_set_rotation(
        plane, lh_math_quat_from_axis_angle(lh_math_vec3_make(1, 0, 0), k_pi / 4));
    lh_entity_t *flat = make_layer(0, 0, 1, 8, 8, k_green);
    render();
    EXPECT_TRUE(same(at(1, 0), k_green)); // plane z = 0.5, behind the flat box
    EXPECT_TRUE(same(at(1, 1), k_red));   // plane z = 1.5, in front of it
    EXPECT_EQ(lh_entity_2d_find_at(root(), lh_math_vec2_make(1.5f, 0.5f)), flat);
    EXPECT_EQ(lh_entity_2d_find_at(root(), lh_math_vec2_make(1.5f, 1.5f)),
              reinterpret_cast<lh_entity_t *>(plane));
}

/* A press is one event or several, and which it was is the whole question. These
   read the screen's answer as a sequence, so a test can say "a press, a move
   and a release" or "a press and a cancel" and mean it. */

TEST_F(Screen, a_press_ends_at_a_release_and_nothing_else)
{
    lh_entity_t *box = reinterpret_cast<lh_entity_t *>(make_box(root(), 0, 0, 4, 4, k_red));
    lh_entity_add_handler(box, screen_test_hold_handler, lh_null);
    g_held_count = 0;

    lh_entity_screen_send_pointer(screen, LH_ENTITY_EVENT_POINTER_DOWN, lh_math_vec2_make(1, 1));
    EXPECT_EQ(lh_entity_screen_get_pressed(screen), box);
    lh_entity_screen_send_pointer(screen, LH_ENTITY_EVENT_POINTER_UP, lh_math_vec2_make(1, 1));

    ASSERT_EQ(g_held_count, 2);
    EXPECT_EQ(g_held_codes[0], LH_ENTITY_EVENT_POINTER_DOWN);
    EXPECT_EQ(g_held_codes[1], LH_ENTITY_EVENT_POINTER_UP);
    EXPECT_EQ(lh_entity_screen_get_pressed(screen), nullptr);
}

TEST_F(Screen, a_move_is_a_drag_while_the_pointer_is_held_and_a_hover_otherwise)
{
    lh_entity_t *box = reinterpret_cast<lh_entity_t *>(make_box(root(), 0, 0, 4, 4, k_red));
    lh_entity_add_handler(box, screen_test_hold_handler, lh_null);
    g_held_count = 0;

    // Nothing held: a move belongs to whatever is under the pointer, and this
    // one is not under the box.
    lh_entity_screen_send_pointer(screen, LH_ENTITY_EVENT_POINTER_MOVE, lh_math_vec2_make(9, 9));
    EXPECT_EQ(g_held_count, 0);

    // Held: the same move at the same place belongs to what is holding it, or a
    // drag would end the moment the pointer left the thing being dragged.
    lh_entity_screen_send_pointer(screen, LH_ENTITY_EVENT_POINTER_DOWN, lh_math_vec2_make(1, 1));
    EXPECT_EQ(lh_entity_screen_send_pointer(screen, LH_ENTITY_EVENT_POINTER_MOVE,
                                            lh_math_vec2_make(9, 9)),
              box);
    ASSERT_EQ(g_held_count, 2);
    EXPECT_EQ(g_held_codes[1], LH_ENTITY_EVENT_POINTER_MOVE);
}

TEST_F(Screen, a_held_pointer_is_told_the_hold_was_taken_away)
{
    lh_entity_t *box = reinterpret_cast<lh_entity_t *>(make_box(root(), 0, 0, 4, 4, k_red));
    lh_entity_add_handler(box, screen_test_hold_handler, lh_null);
    g_held_count = 0;

    lh_entity_screen_send_pointer(screen, LH_ENTITY_EVENT_POINTER_DOWN, lh_math_vec2_make(1, 1));
    lh_entity_screen_send_pointer(screen, LH_ENTITY_EVENT_POINTER_MOVE, lh_math_vec2_make(3, 3));
    lh_entity_screen_cancel_pointer(screen);

    // A press, a move, and a cancel that is not a release: the entity is told
    // the hold is over, and told apart from a release, and is not left holding
    // a press that has already ended.
    ASSERT_EQ(g_held_count, 3);
    EXPECT_EQ(g_held_codes[0], LH_ENTITY_EVENT_POINTER_DOWN);
    EXPECT_EQ(g_held_codes[1], LH_ENTITY_EVENT_POINTER_MOVE);
    EXPECT_EQ(g_held_codes[2], LH_ENTITY_EVENT_POINTER_CANCEL);
    EXPECT_EQ(lh_entity_screen_get_pressed(screen), nullptr);
}

TEST_F(Screen, a_cancel_that_has_nothing_behind_it_is_nothing)
{
    lh_entity_t *box = reinterpret_cast<lh_entity_t *>(make_box(root(), 0, 0, 4, 4, k_red));
    lh_entity_add_handler(box, screen_test_hold_handler, lh_null);
    g_held_count = 0;

    // A window cannot tell which capture changes were its own releasing, so it
    // reports all of them and the screen decides.
    lh_entity_screen_cancel_pointer(screen);
    EXPECT_EQ(g_held_count, 0);

    // The same after a press that was released properly: the release already
    // ended it, and the capture change that follows it has nothing left to end.
    lh_entity_screen_send_pointer(screen, LH_ENTITY_EVENT_POINTER_DOWN, lh_math_vec2_make(1, 1));
    lh_entity_screen_send_pointer(screen, LH_ENTITY_EVENT_POINTER_UP, lh_math_vec2_make(1, 1));
    g_held_count = 0;
    lh_entity_screen_cancel_pointer(screen);
    EXPECT_EQ(g_held_count, 0);
}

} // namespace
