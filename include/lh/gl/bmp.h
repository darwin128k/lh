/**
 * @file bmp.h
 * @brief A pixmap stored as one uncompressed 32-bit bitmap.
 *
 * The encoding is the Windows bitmap a picture viewer opens: a 14-byte file
 * header, a 40-byte info header, then pixels bottom-up. Each pixel is four
 * bytes, blue, green, red, alpha. Alpha is copied and not blended. The file
 * size field is 32 bits, so a grid that does not fit returns no encoding.
 *
 * The caller owns the destination bytes. Nothing here opens a file.
 */

#ifndef LH_GL_BMP_H
#define LH_GL_BMP_H

#include <lh/core/size.h>
#include <lh/core/util/byte/ptr.h>
#include <lh/gl/pixmap.h>

/**
 * @def LH_GL_BMP_SIGNATURE
 * @brief Little-endian file mark. The first stored byte is `B`.
 */
#define LH_GL_BMP_SIGNATURE 0x4D42U

/**
 * @def LH_GL_BMP_FILE_HEADER_BYTES
 * @brief Bytes before the info header.
 */
#define LH_GL_BMP_FILE_HEADER_BYTES 14U

/**
 * @def LH_GL_BMP_INFO_HEADER_BYTES
 * @brief Bytes of the info header.
 */
#define LH_GL_BMP_INFO_HEADER_BYTES 40U

/**
 * @def LH_GL_BMP_HEADER_BYTES
 * @brief Byte offset of the first pixel.
 */
#define LH_GL_BMP_HEADER_BYTES (LH_GL_BMP_FILE_HEADER_BYTES + LH_GL_BMP_INFO_HEADER_BYTES)

/**
 * @def LH_GL_BMP_BYTES_PER_PIXEL
 * @brief Blue, green, red, alpha.
 */
#define LH_GL_BMP_BYTES_PER_PIXEL 4U

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Bytes ::lh_gl_bmp_try_store writes for @p pixmap, or 0.
 *
 * Zero means the width or the height is not positive, or the 32-bit file
 * would not hold the header plus the pixels. @p pixmap is not null.
 */
lh_usize_t
lh_gl_bmp_byte_count(const lh_gl_pixmap_t *pixmap);

/**
 * @brief Write @p pixmap into @p out.
 *
 * @return False when ::lh_gl_bmp_byte_count is 0, @p out is null, @p out_size
 *         is shorter than that count, or the pixmap has no pixels.
 */
lh_bool_t
lh_gl_bmp_try_store(const lh_gl_pixmap_t *pixmap, lh_byte_ptr out, lh_usize_t out_size);

/**
 * @brief ::lh_gl_bmp_try_store, then a check.
 *
 * The check fires when the store would return false.
 */
lh_void
lh_gl_bmp_store(const lh_gl_pixmap_t *pixmap, lh_byte_ptr out, lh_usize_t out_size);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_BMP_H */
