/* lh/gl.h compiled as C11: the library is C, so its umbrella must be too. */
#include <lh/gl.h>

int
lh_test_gl_umbrella_c(void);

int
lh_test_gl_umbrella_c(void)
{
    lh_byte_t mem[8];
    lh_gl_web_buf_t buf;
    lh_gl_web_buf_init(&buf, mem, sizeof(mem));
    lh_pixel_gradient_t gradient;

    lh_pixel_gradient_init(&gradient);
    return lh_gl_web_buf_ok(&buf) && lh_pixel_gradient_get_stops_count(&gradient) == 0;
}
