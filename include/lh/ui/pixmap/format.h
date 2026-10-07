/**
 * @file format.h
 * @brief Pixel formats of ::lh_ui_pixmap_t.
 */

#ifndef LH_UI_PIXMAP_FORMAT_H
#define LH_UI_PIXMAP_FORMAT_H

/**
 * @enum lh_ui_pixmap_format
 * @typedef lh_ui_pixmap_format_t
 * @brief How one pixel is stored.
 */
enum lh_ui_pixmap_format
{
    /** 32-bit word `0xAARRGGBB`, straight alpha (bytes B, G, R, A on a
     *  little-endian target: a Win32 32-bit DIB). */
    lh_ui_pixmap_format_argb8888,
    /** 16-bit word `rrrrrggg gggbbbbb` in native byte order (STM32 LTDC /
     *  DMA2D RGB565), opaque. */
    lh_ui_pixmap_format_rgb565
};
typedef enum lh_ui_pixmap_format lh_ui_pixmap_format_t;

#endif /* LH_UI_PIXMAP_FORMAT_H */
