/* The GDI backend is the one target that has to widen a buffer on the way out, so
   its default format is a measured decision and not a preference. The whole point
   of these two tests is that the number they pin was nearly lost: RGB565 draws an
   800x600 frame measurably faster (137 750 ns against 94 100 on this machine) and
   presents it far slower (382 400 ns against 839 050, because a 32-bit window DC
   cannot be copied into from 16 bits and GDI expands every pixel). Half the bytes
   on the way in is not half the frame. */

#include <gtest/gtest.h>

#include <lh/compiler/os.h>
#include <lh/null.h>
#include <lh/ui/pixmap.h>
#include <lh/util/addr.h>

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS

#    include <lh/os/render/backend/gdi.h>

namespace
{

TEST(os_render_backend_gdi, a_fresh_context_draws_into_argb8888)
{
    lh_os_render_backend_gdi_context_t gdi;

    lh_os_render_backend_gdi_context_init(lh_addr_of(gdi));
    EXPECT_EQ(lh_os_render_backend_gdi_context_get_format(lh_addr_of(gdi)), lh_ui_pixmap_format_argb8888);
    lh_os_render_backend_gdi_context_deinit(lh_addr_of(gdi));
}

TEST(os_render_backend_gdi, the_format_is_reachable_before_the_first_frame)
{
    lh_os_render_backend_gdi_context_t gdi;

    /* What an MCU-sized target wants, and the only thing that costs it here is
       that the DIB it asks for is 16-bit. Reaching it is one call. */
    lh_os_render_backend_gdi_context_init(lh_addr_of(gdi));
    lh_os_render_backend_gdi_context_set_format(lh_addr_of(gdi), lh_ui_pixmap_format_rgb565);
    EXPECT_EQ(lh_os_render_backend_gdi_context_get_format(lh_addr_of(gdi)), lh_ui_pixmap_format_rgb565);
    EXPECT_EQ(lh_ui_pixmap_format_get_bytes(lh_os_render_backend_gdi_context_get_format(lh_addr_of(gdi))), 2);
    lh_os_render_backend_gdi_context_deinit(lh_addr_of(gdi));
}

} // namespace

#endif // LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS