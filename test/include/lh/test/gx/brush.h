/**
 * @file brush.h
 * @brief Test helper: ::lh_test::gx_brush, a one-colour brush made in an
 *        expression.
 *
 * The canvas slots take a pointer to a brush (`lh/gx/brush.h`). A test that fills
 * with one colour writes `gx_brush(0xFF112233U).get()`: the brush is a temporary
 * that lives until the end of the call it is passed to.
 */

#ifndef LH_TEST_GX_BRUSH_H
#define LH_TEST_GX_BRUSH_H

#include <lh/gx/brush.h>

namespace lh_test
{

/* A brush of one colour. */
struct gx_brush
{
    lh_gx_brush_t brush;

    explicit gx_brush(lh_gx_color_t color)
    {
        lh_gx_brush_init_color(&brush, color);
    }

    const lh_gx_brush_t *
    get() const
    {
        return &brush;
    }
};

} // namespace lh_test

#endif /* LH_TEST_GX_BRUSH_H */
