/* lh/gl.h compiled as C11: the library is C, so its umbrella must be too. */
#include <lh/gl.h>

int
lh_test_gl_umbrella_c(void);

int
lh_test_gl_umbrella_c(void)
{
    lh_pixel_gradient_t gradient;

    lh_pixel_gradient_init(&gradient);
    return lh_pixel_gradient_get_stops_count(&gradient) == 0;
}
