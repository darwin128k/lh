#include <gtest/gtest.h>

#include <lh/gl.h>

extern "C" int
lh_test_gl_umbrella_c(void);

namespace
{

// Each of these comes from a header that lh/gl.h did not reach before it was
// made a complete umbrella; the test compiling at all is most of the check.
TEST(ui_umbrella, reaches_gradient_sine)
{
    EXPECT_EQ(lh_gl_gradient_sine(0), 0);
    EXPECT_GT(lh_gl_gradient_sine(90), 0);
}

TEST(ui_umbrella, reaches_web_buf)
{
    lh_byte_t mem[16];
    lh_gl_web_buf_t buf;
    lh_gl_web_buf_init(&buf, mem, sizeof(mem));
    EXPECT_TRUE(lh_gl_web_buf_ok(&buf));
    EXPECT_EQ(lh_gl_web_buf_len(&buf), 0u);
}

TEST(ui_umbrella, reaches_types_from_new_headers)
{
    EXPECT_GT(sizeof(lh_gl_canvas_box_frame_t), 0u);
    EXPECT_GT(sizeof(lh_gl_present_sink_t), 0u);
}

// The same header, compiled as C11 in umbrella.c.
TEST(ui_umbrella, compiles_as_c)
{
    EXPECT_EQ(lh_test_gl_umbrella_c(), 1);
}

} // namespace
