#include <gtest/gtest.h>

#include <lh/byte.h>
#include <lh/config.h>
#include <lh/null.h>
#include <lh/ui/color.h>
#include <lh/ui/gradient.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

namespace
{

TEST(ui_gradient, make_empty_has_no_stops)
{
    lh_ui_gradient_t gradient;

    lh_ui_gradient_init(lh_addr_of(gradient));
    EXPECT_EQ(lh_ui_gradient_get_stop_count(lh_addr_of(gradient)), 0);
}

TEST(ui_gradient, init_stops_even_fracs_default_capacity)
{
    const lh_ui_color_t colors[2] = {
        ({ lh_ui_color_t _lh_tmp; lh_ui_color_init(lh_addr_of(_lh_tmp), 255, 0, 0, 255); _lh_tmp; }),
        ({ lh_ui_color_t _lh_tmp; lh_ui_color_init(lh_addr_of(_lh_tmp), 0, 0, 255, 255); _lh_tmp; }),
    };
    lh_ui_gradient_t gradient;
    lh_ui_gradient_init_stops(lh_addr_of(gradient), colors, lh_ptr_rcast(const lh_byte_t, lh_null),
                              2);
    ASSERT_EQ(lh_ui_gradient_get_stop_count(lh_addr_of(gradient)), 2);
    EXPECT_EQ(lh_ui_gradient_stop_get_frac(lh_ui_gradient_get_stop_as_const(lh_addr_of(gradient), 0)),
              0);
    EXPECT_EQ(lh_ui_gradient_stop_get_frac(lh_ui_gradient_get_stop_as_const(lh_addr_of(gradient), 1)),
              255);
    EXPECT_EQ(lh_ui_color_get_r(lh_ui_gradient_stop_get_color_as_const(
                  lh_ui_gradient_get_stop_as_const(lh_addr_of(gradient), 0))),
              255);
    EXPECT_EQ(lh_ui_color_get_b(lh_ui_gradient_stop_get_color_as_const(
                  lh_ui_gradient_get_stop_as_const(lh_addr_of(gradient), 1))),
              255);
}

TEST(ui_gradient, init_stops_uses_explicit_fracs)
{
    const lh_ui_color_t colors[2] = {
        ({ lh_ui_color_t _lh_tmp; lh_ui_color_init(lh_addr_of(_lh_tmp), 0, 0, 0, 255); _lh_tmp; }),
        ({ lh_ui_color_t _lh_tmp; lh_ui_color_init(lh_addr_of(_lh_tmp), 255, 255, 255, 255); _lh_tmp; }),
    };
    const lh_byte_t fracs[2] = {40, 200};
    lh_ui_gradient_t gradient;
    lh_ui_gradient_init_stops(lh_addr_of(gradient), colors, fracs, 2);
    EXPECT_EQ(lh_ui_gradient_stop_get_frac(lh_ui_gradient_get_stop_as_const(lh_addr_of(gradient), 0)),
              40);
    EXPECT_EQ(lh_ui_gradient_stop_get_frac(lh_ui_gradient_get_stop_as_const(lh_addr_of(gradient), 1)),
              200);
}

TEST(ui_gradient, max_stops_option_is_at_least_one)
{
    EXPECT_GE(LH_LIBRARY_OPTION_UI_GRADIENT_MAX_STOPS, 1);
}

} // namespace
