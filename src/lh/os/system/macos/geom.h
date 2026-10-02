/**
 * @file geom.h
 * @brief Backend-private: lh::geom ↔ Cocoa native conversions.
 *
 * Used by `src/lh/os/system/macos/window.m` and the future paint / input
 * backends. Lives under `src/` (not installed) — `lh/geom.h` is the only
 * public header.
 *
 * Implemented in `geom.m` (Objective-C) because every conversion touches
 * `NSRect` / `NSColor`, both of which are ObjC types.
 */

#ifndef LH_SRC_OS_SYSTEM_MACOS_GEOM_H
#define LH_SRC_OS_SYSTEM_MACOS_GEOM_H

#include <AppKit/AppKit.h>

#include <lh/color.h>
#include <lh/geom.h>

/**
 * @brief Convert `lh_rect_t` (origin + size) to Cocoa's `NSRect`
 *        (origin + size).
 */
NSRect
lh_os_system_macos_rect_from_lh(const lh_rect_t *self);

/**
 * @brief Convert Cocoa's `NSRect` to `lh_rect_t` (rounding down so the
 *        pixel grid stays integer).
 */
lh_rect_t
lh_os_system_macos_rect_to_lh(NSRect self);

/**
 * @brief Convert `lh_color_t` to Cocoa's `NSColor` (calibrated RGB).
 */
NSColor *
lh_os_system_macos_color_from_lh(lh_color_t self);

#endif /* LH_SRC_OS_SYSTEM_MACOS_GEOM_H */