/**
 * @file box.h
 * @brief The box a 2D entity is: from the local origin to its `size`, corners
 *        rounded by a radius.
 */

#ifndef LH_MATH_BOX_H
#define LH_MATH_BOX_H

#include <lh/compiler/extern/c.h>
#include <lh/float.h>
#include <lh/math/vec2.h>
#include <lh/void.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief How far @p point is from the box `0 .. size` with its corners
 *        rounded by @p radius, negative inside and zero on the rim.
 *
 * A shape that is turned on the screen cannot be drawn by testing whether a
 * pixel is in it, because the answer is "yes" or "no" and the edge the pixel
 * straddles belongs half to the shape. What a pixel needs is how far it is
 * from the edge, and a distance field is the one number that says that for
 * every shape at once: the fill, the corner radius and the outline are all
 * thresholds on the same field, and the coverage of a pixel is a threshold
 * on it in turn.
 *
 * The distance is in the units of the box. A box drawn turned is not measured
 * in those units by the screen, so the caller scales it by what one of its
 * units is worth on screen.
 *
 * @param radius Rounded off at half the shorter side, so a radius larger than
 *               the box can hold is the same as a disc, and never folds the
 *               field back on itself.
 * @param size   Zero on an axis collapses the box onto that axis and a
 *               negative one is read as that same zero, which is how
 *               ::lh_math_rect_is_empty reads a side: there is no interior to
 *               be inside of, so the answer is never negative. Zero on both
 *               leaves a corner point, at the origin.
 */
lh_float_t
lh_math_box_distance(lh_math_vec2_t point, lh_math_vec2_t size, lh_float_t radius);

LH_COMPILER_EXTERN_C_END

#endif /* LH_MATH_BOX_H */
