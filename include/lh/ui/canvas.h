/**
 * @file canvas.h
 * @brief A pixel buffer to draw into, with a clip rectangle.
 *
 * The canvas does not own its pixels: it draws into memory the caller
 * provides (a window's back buffer, a framebuffer on a microcontroller, a
 * texture). Pixels are ::lh_ui_color_t (RGBA, 8 bits per channel); whoever
 * shows the image converts to the device's format.
 *
 * Every drawing call stays inside the clip rectangle, which is how entities
 * are kept inside their parents and how only the changed parts of a screen
 * are redrawn.
 *
 * When a depth plane is set, a fragment is kept only when its
 * ::lh_ui_canvas_set_draw_z is closer than or equal to the value already
 * there. Closer means a larger z: the view looks along -z. Equal z keeps
 * the order of the calls, so a flat interface stacks as before.
 */

#ifndef LH_UI_CANVAS_H
#define LH_UI_CANVAS_H

#include <lh/compiler/extern/c.h>
#include <lh/float.h>
#include <lh/ui/canvas/fields.h>
#include <lh/ui/color.h>
#include <lh/math/rect.h>
#include <lh/void.h>

/**
 * @struct lh_ui_canvas
 * @brief Fields via ::lh_ui_canvas_fields.
 */
struct lh_ui_canvas
{
    lh_ui_canvas_fields(lh_ui_color_t, lh_math_coord_t, lh_math_rect_t, lh_float_t);
};
typedef struct lh_ui_canvas lh_ui_canvas_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Draw into @p width x @p height pixels at @p pixels, rows @p stride
 *        pixels apart. The clip starts as the whole image.
 */
lh_void
lh_ui_canvas_init(lh_ui_canvas_t *self, lh_ui_color_t *pixels, lh_math_coord_t width,
                  lh_math_coord_t height, lh_math_coord_t stride);

/**
 * @brief Image width in pixels.
 */
lh_math_coord_t
lh_ui_canvas_get_width(const lh_ui_canvas_t *self);

/**
 * @brief Image height in pixels.
 */
lh_math_coord_t
lh_ui_canvas_get_height(const lh_ui_canvas_t *self);

/**
 * @brief Where drawing is currently allowed.
 */
lh_math_rect_t
lh_ui_canvas_get_clip(const lh_ui_canvas_t *self);

/**
 * @brief Allow drawing only inside @p clip (cut to the image).
 */
lh_void
lh_ui_canvas_set_clip(lh_ui_canvas_t *self, lh_math_rect_t clip);

/**
 * @brief Depth samples in the same layout as the pixels, or ::lh_null to
 *        ignore depth. Not owned: it must stay valid and hold `stride *
 *        height` samples.
 */
lh_void
lh_ui_canvas_set_depth(lh_ui_canvas_t *self, lh_float_t *depth);

/**
 * @brief Depth of the next ::lh_ui_canvas_fill_rect or
 *        ::lh_ui_canvas_blend_pixel. Ignored when no depth plane is set.
 */
lh_void
lh_ui_canvas_set_draw_z(lh_ui_canvas_t *self, lh_float_t z);

/**
 * @brief Write the farthest depth into @p area (cut to the image), so the
 *        next fragments in that area all pass. No depth plane: nothing.
 */
lh_void
lh_ui_canvas_clear_depth(lh_ui_canvas_t *self, lh_math_rect_t area);

/**
 * @brief The pixel at (@p x, @p y), which must be inside the image.
 */
lh_ui_color_t
lh_ui_canvas_get_pixel(const lh_ui_canvas_t *self, lh_math_coord_t x, lh_math_coord_t y);

/**
 * @brief Draw @p color over the pixel at (@p x, @p y), blended by its alpha;
 *        nothing outside the clip.
 */
lh_void
lh_ui_canvas_blend_pixel(lh_ui_canvas_t *self, lh_math_coord_t x, lh_math_coord_t y,
                         lh_ui_color_t color);

/**
 * @brief Draw @p color over every pixel of @p rect, blended by its alpha;
 *        nothing outside the clip.
 */
lh_void
lh_ui_canvas_fill_rect(lh_ui_canvas_t *self, lh_math_rect_t rect, lh_ui_color_t color);

/**
 * @brief How much of a pixel a rim covers, 0..255, from how far the pixel is
 *        from it. The number every other coverage here is built on, and the
 *        only one a shape that is not axis-aligned on screen can use.
 *
 * @p signed_dist is a distance in 1/256 of a pixel, negative inside the shape
 * and positive outside, in the sense of ::lh_math_box_distance. A pixel whose
 * middle is a half pixel inside is fully covered, one a half pixel outside is
 * not covered at all, and the width of the ramp between them is
 * ::LH_LIBRARY_OPTION_UI_COVER, so an edge looks the same here as it does in
 * ::lh_ui_canvas_disc_coverage. A span of 0 asks for the hard edge.
 */
lh_byte_t
lh_ui_canvas_coverage_from(lh_int_t signed_dist);

/**
 * @brief Blend @p color over the pixel (@p x, @p y) as far as @p coverage
 *        reaches, and not at all when it is 0.
 *
 * Goes through the clip and the depth of ::lh_ui_canvas_blend_pixel, so a
 * covered edge is subject to the same two as a whole one.
 */
lh_void
lh_ui_canvas_blend_coverage(lh_ui_canvas_t *self, lh_int_t x, lh_int_t y, lh_ui_color_t color,
                            lh_byte_t coverage);

/**
 * @brief How much of the pixel (@p x, @p y) a disc covers, 0..255.
 *
 * The center is the point (@p cx, @p cy) and the radius is in pixels. The
 * fade width is ::LH_LIBRARY_OPTION_UI_COVER (256 is one pixel, 0 is a hard
 * edge) and is computed here, so the edge looks the same on Windows XP and
 * on a framebuffer with no OS drawing at all.
 */
lh_byte_t
lh_ui_canvas_disc_coverage(lh_int_t x, lh_int_t y, lh_float_t cx, lh_float_t cy, lh_int_t radius);

/**
 * @brief How much of the pixel (@p x, @p y) a rounded box covers, 0..255.
 *
 * The box is half-open, [@p left, @p right) by [@p top, @p bottom).
 * @p radius 0 is that rectangle with a hard edge, because an axis-aligned
 * edge that sits on a pixel boundary has no partial pixel.
 */
lh_byte_t
lh_ui_canvas_round_coverage(lh_int_t x, lh_int_t y, lh_int_t left, lh_int_t top, lh_int_t right,
                            lh_int_t bottom, lh_int_t radius);

/**
 * @brief How much of the pixel (@p x, @p y) the ring between @p inner and
 *        @p outer covers, 0..255.
 *
 * The darker of the two rims wins, so the ring is solid where it is between
 * them and fades on both edges. An @p inner of 0 or less is a full disc.
 */
lh_byte_t
lh_ui_canvas_ring_coverage(lh_int_t x, lh_int_t y, lh_float_t cx, lh_float_t cy, lh_int_t outer,
                           lh_int_t inner);

/**
 * @brief How much of the pixel (@p x, @p y) a border of @p width covers, 0..255.
 *
 * The border is inside the box it outlines, so the box does not grow: a border
 * of 2 on a 10x10 box is the ring between a 6x6 hole and the 10x10 edge. A
 * @p radius of 0 is a square border, and the corner radius shrinks by the
 * width so the outline keeps its thickness all the way round.
 */
lh_byte_t
lh_ui_canvas_box_stroke_coverage(lh_int_t x, lh_int_t y, lh_int_t left, lh_int_t top, lh_int_t right,
                                 lh_int_t bottom, lh_int_t radius, lh_int_t width);

/**
 * @brief How much of the pixel (@p x, @p y) a line covers, 0..255.
 *
 * The line runs from (@p x0, @p y0) to (@p x1, @p y1) and is @p width wide,
 * centred on the segment and square at the ends. Endpoints in the same place
 * are a disc of that width.
 */
lh_byte_t
lh_ui_canvas_line_coverage(lh_int_t x, lh_int_t y, lh_int_t x0, lh_int_t y0, lh_int_t x1,
                           lh_int_t y1, lh_int_t width);

/**
 * @brief Fill a disc. The rim is ::lh_ui_canvas_disc_coverage.
 */
lh_void
lh_ui_canvas_fill_disc(lh_ui_canvas_t *self, lh_float_t cx, lh_float_t cy, lh_int_t radius,
                       lh_ui_color_t color);

/**
 * @brief Fill a rounded box. The rim is ::lh_ui_canvas_round_coverage.
 */
lh_void
lh_ui_canvas_fill_round(lh_ui_canvas_t *self, lh_math_rect_t rect, lh_int_t radius,
                        lh_ui_color_t color);

/**
 * @brief Fill the ring between @p inner and @p outer, from @p start to @p end.
 *
 * Angles are radians with y growing downward, the same sense as `atan2`.
 * The fill runs as the angle grows and stops after one full turn. Both rims
 * are ::lh_ui_canvas_disc_coverage.
 */
lh_void
lh_ui_canvas_fill_arc(lh_ui_canvas_t *self, lh_int_t cx, lh_int_t cy, lh_int_t outer,
                      lh_int_t inner, lh_float_t start, lh_float_t end, lh_ui_color_t color);

/**
 * @brief Fill the ring between @p inner and @p outer, all the way round.
 *
 * ::lh_ui_canvas_fill_arc for the case where the sweep does not matter, which
 * is also the cheapest way to draw a disc with a hole in it: the inner radius
 * is subtracted, not painted over.
 */
lh_void
lh_ui_canvas_fill_ring(lh_ui_canvas_t *self, lh_float_t cx, lh_float_t cy, lh_int_t outer,
                       lh_int_t inner, lh_ui_color_t color);

/**
 * @brief Outline a box with @p width pixels of @p color, inside the box.
 *
 * The box does not grow, so lay out for the outer size and let the outline
 * eat into the fill. Square corners.
 */
lh_void
lh_ui_canvas_stroke_rect(lh_ui_canvas_t *self, lh_math_rect_t rect, lh_int_t width,
                         lh_ui_color_t color);

/**
 * @brief Outline a box with @p width pixels of @p color, rounded by @p radius.
 *
 * As ::lh_ui_canvas_stroke_rect, and the corner radius shrinks by @p width so
 * the outline keeps the same thickness all the way round.
 */
lh_void
lh_ui_canvas_stroke_round(lh_ui_canvas_t *self, lh_math_rect_t rect, lh_int_t radius, lh_int_t width,
                          lh_ui_color_t color);

/**
 * @brief Outline a disc of @p radius with @p width pixels of @p color.
 *
 * The width is centred on the radius, so a disc of 10 with a 2 wide outline is
 * the ring from 9 to 11 and covers 22 pixels across. This is the whole of "a
 * circle with a border": one call, and the hole is not painted twice.
 */
lh_void
lh_ui_canvas_stroke_disc(lh_ui_canvas_t *self, lh_float_t cx, lh_float_t cy, lh_int_t radius,
                         lh_int_t width, lh_ui_color_t color);

/**
 * @brief Draw a line of @p width pixels of @p color from (@p x0, @p y0) to
 *        (@p x1, @p y1).
 *
 * The width is centred on the segment and the ends are flat. Endpoints in the
 * same place are a disc of that width.
 */
lh_void
lh_ui_canvas_stroke_line(lh_ui_canvas_t *self, lh_int_t x0, lh_int_t y0, lh_int_t x1, lh_int_t y1,
                         lh_int_t width, lh_ui_color_t color);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_CANVAS_H */
