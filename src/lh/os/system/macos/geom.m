/**
 * @file geom.m
 * @brief Cocoa native ↔ lh::geom conversions for the macOS backend.
 *
 * `NSRect` and `NSColor` are both value types in Cocoa; conversions are
 * `O(1)` arithmetic, no allocations. The header (`geom.h`) is plain C so
 * the public side never names `<AppKit>`; this `.m` file is the only place
 * where Cocoa types appear in the conversion path.
 *
 * Coordinate convention: `NSRect` is `{ NSPoint origin; NSSize size; }`
 * which lines up directly with `lh_rect_t` — no +1/-1 dance needed
 * (Cocoa is half-open on both ends).
 */

#import <AppKit/AppKit.h>

#include <lh/cast/static.h>
#include <lh/color.h>
#include <lh/geom.h>
#include <lh/null.h>
#include <lh/os/system/macos/geom.h>

NSRect
lh_os_system_macos_rect_from_lh(const lh_rect_t *self)
{
    if (lh_null_eq(self))
    {
        return NSMakeRect(0.0, 0.0, 0.0, 0.0);
    }
    return NSMakeRect((CGFloat)self->origin.x, (CGFloat)self->origin.y,
                       (CGFloat)self->size.width, (CGFloat)self->size.height);
}

lh_rect_t
lh_os_system_macos_rect_to_lh(NSRect self)
{
    /* Round sub-pixel coordinates down — UI coords are integer in our
       public API, and over-rounding can produce an off-by-one when
       Cocoa tracks the cursor at fractional positions. */
    return lh_rect_make((lh_coord_t)self.origin.x, (lh_coord_t)self.origin.y,
                         (lh_coord_t)self.size.width, (lh_coord_t)self.size.height);
}

NSColor *
lh_os_system_macos_color_from_lh(lh_color_t self)
{
    return [NSColor
        colorWithCalibratedRed:(CGFloat)self.r / 255.0
                         green:(CGFloat)self.g / 255.0
                          blue:(CGFloat)self.b / 255.0
                         alpha:(CGFloat)self.a / 255.0];
}