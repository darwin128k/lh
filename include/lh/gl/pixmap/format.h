/**
 * @file format.h
 * @brief Pixel formats of ::lh_gl_pixmap_t.
 */

#ifndef LH_GL_PIXMAP_FORMAT_H
#define LH_GL_PIXMAP_FORMAT_H

/**
 * @enum lh_gl_pixmap_format
 * @brief How one pixel is stored.
 */
enum lh_gl_pixmap_format
{
    /** 32-bit word `0xAARRGGBB`, straight alpha (bytes B, G, R, A on a
     *  little-endian target: a Win32 32-bit DIB). */
    lh_gl_pixmap_format_argb8888,
    /** 16-bit word `rrrrrggg gggbbbbb` in native byte order (STM32 LTDC /
     *  DMA2D RGB565), opaque. */
    lh_gl_pixmap_format_rgb565,
    /** RGB565 with the two bytes of each word swapped (big-endian in memory):
     *  what most SPI display controllers take, so a frame goes out as is. */
    lh_gl_pixmap_format_rgb565_swapped
};

/** @brief How one pixel is stored. */
typedef enum lh_gl_pixmap_format lh_gl_pixmap_format_t;

#endif /* LH_GL_PIXMAP_FORMAT_H */
