#include <gtest/gtest.h>

#include <lh/ui/paint.h>
#include <lh/ui/pen.h>
#include <lh/util/addr.h>

#include <type_traits>

TEST(ui_pen, is_the_same_type_as_paint)
{
    EXPECT_TRUE((std::is_same<lh_ui_pen_t, lh_ui_paint_t>::value));
}

TEST(ui_pen, make_empty_and_init_are_usable)
{
    lh_ui_pen_t a;

    lh_ui_paint_init(lh_addr_of(a));
    lh_ui_pen_t b;
    lh_ui_pen_init(lh_addr_of(b));
    (void)a;
    (void)b;
}
