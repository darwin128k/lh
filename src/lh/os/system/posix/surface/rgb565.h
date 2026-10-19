/**
 * @file rgb565.h
 * @brief Backend-private: RGB565 → 0x00RRGGBB for an X PutImage of a 16-bit surface.
 */

#ifndef LH_SRC_OS_SYSTEM_POSIX_SURFACE_RGB565_H
#define LH_SRC_OS_SYSTEM_POSIX_SURFACE_RGB565_H

#include <lh/attribute/force_inline.h>
#include <lh/cast/static.h>
#include <lh/numeric/fixed/types.h>

/**
 * @brief RGB565 @p pixel as 0x00RRGGBB, top bits of each channel repeated into
 *        the bottom so full scale stays full scale.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_u32_t
lh_os_system_posix_surface_from_565(lh_u16_t pixel)
{
    const lh_u32_t r = lh_cast_static(lh_u32_t, (pixel >> 11)) & 0x1fU;
    const lh_u32_t g = lh_cast_static(lh_u32_t, (pixel >> 5)) & 0x3fU;
    const lh_u32_t b = lh_cast_static(lh_u32_t, pixel) & 0x1fU;

    return ((r << 3 | r >> 2) << 16) | ((g << 2 | g >> 4) << 8) | (b << 3 | b >> 2);
}

#endif /* LH_SRC_OS_SYSTEM_POSIX_SURFACE_RGB565_H */
