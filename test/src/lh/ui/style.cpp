#include <gtest/gtest.h>

#include <lh/ui/style.h>
#include <lh/util/addr.h>

TEST(ui_style, make_empty_and_init_are_usable)
{
    lh_ui_style_t a;

    lh_ui_style_init(lh_addr_of(a));
    lh_ui_style_t b;
    lh_ui_style_init(lh_addr_of(b));
    (void)a;
    (void)b;
}
