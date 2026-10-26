#include <lh/ui/renderer3d.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#define CHECK(x)                                                                                   \
    do                                                                                             \
    {                                                                                              \
        if (!(x))                                                                                  \
        {                                                                                          \
            fprintf(stderr, "renderer3d line %d: %s\n", __LINE__, #x);                             \
            exit(1);                                                                               \
        }                                                                                          \
    } while (0)
static lh_math_vec3_t
v(float x, float y, float z)
{
    lh_math_vec3_t a;
    lh_math_vec3_init(&a, x, y, z);
    return a;
}
int
main(void)
{
    lh_u32_t bits[64 * 64];
    float depth[64 * 64];
    lh_ui_pixmap_t p;
    lh_ui_renderer3d_camera_t cam;
    lh_ui_renderer3d_t r;
    lh_ui_pixmap_init(&p, (lh_byte_ptr)bits, 64, 64, 64 * 4, lh_ui_pixmap_format_argb8888);
    CHECK(!lh_ui_renderer3d_camera_init(&cam, v(0, 0, 0), v(0, 0, 0), v(0, 1, 0), 1, .1, 100));
    CHECK(lh_ui_renderer3d_camera_init(&cam, v(0, 0, 0), v(0, 0, 1), v(0, 1, 0), 1, .1, 100));
    CHECK(!lh_ui_renderer3d_begin(&r, &p, depth, 2, &cam, 0));
    CHECK(lh_ui_renderer3d_begin(&r, &p, depth, 64 * 64, &cam, 0));
    lh_ui_renderer3d_triangle(&r, v(-1, -1, 2), v(1, -1, 2), v(0, 1, 2), 0xff00ff00u);
    CHECK(bits[32 * 64 + 32] == 0xff00ff00u);
    lh_ui_renderer3d_triangle(&r, v(-2, -2, 4), v(2, -2, 4), v(0, 2, 4), 0xffff0000u);
    CHECK(bits[32 * 64 + 32] == 0xff00ff00u);
    lh_ui_renderer3d_triangle(&r, v(-.5f, -.5f, 1), v(.5f, -.5f, 1), v(0, .5f, 1), 0xff0000ffu);
    CHECK(bits[32 * 64 + 32] == 0xff0000ffu);
    lh_ui_renderer3d_triangle(&r, v(NAN, 0, 1), v(0, 1, 1), v(1, 1, 1), 0);
    CHECK(bits[32 * 64 + 32] == 0xff0000ffu);
    CHECK(lh_ui_renderer3d_begin(&r, &p, depth, 64 * 64, &cam, 0));
    lh_ui_renderer3d_triangle(&r, v(-1, -1, -2), v(1, -1, -2), v(0, 1, -2), 1);
    for (int i = 0; i < 64 * 64; i++)
    {
        CHECK(bits[i] == 0);
    }
    lh_ui_renderer3d_triangle(&r, v(-10, -10, .01f), v(10, -10, 2), v(0, 10, 2), 0xffabcdefu);
    int covered = 0;
    for (int i = 0; i < 64 * 64; i++)
    {
        covered += bits[i] != 0;
    }
    CHECK(covered > 0);
    CHECK(lh_ui_renderer3d_get_triangles(&r) > 0);
    puts("PASS: camera validation, buffer bounds, depth, behind-camera rejection and frustum "
         "clipping");
    return 0;
}
