/**
 * @file bmp.c
 * @brief Store a pixmap as an uncompressed 32-bit bitmap.
 */
#include <lh/gl/bmp.h>

#include <lh/core/bit/endian.h>
#include <lh/core/util/interval.h>

lh_usize_t
lh_gl_bmp_byte_count(const lh_gl_pixmap_t *pixmap)
{
    const lh_gl_size_t *size = lh_gl_pixmap_get_size(pixmap);
    lh_gl_scalar_t width = lh_gl_size_get_width(size);
    lh_gl_scalar_t height = lh_gl_size_get_height(size);

    if (width <= 0 || height <= 0)
    {
        return 0U;
    }
    lh_gl_scalar_t limit = lh_cast_static(lh_gl_scalar_t, (LH_UINT_T_MAX - LH_GL_BMP_HEADER_BYTES) /
                                                             LH_GL_BMP_BYTES_PER_PIXEL);

    if (lh_interval_closed_is_mul_overflow(width, height, 1, limit))
    {
        return 0U;
    }
    return lh_cast_static(lh_usize_t, width) * lh_cast_static(lh_usize_t, height) *
               LH_GL_BMP_BYTES_PER_PIXEL +
           LH_GL_BMP_HEADER_BYTES;
}

lh_bool_t
lh_gl_bmp_try_store(const lh_gl_pixmap_t *pixmap, lh_byte_ptr out, lh_usize_t out_size)
{
    lh_usize_t count = lh_gl_bmp_byte_count(pixmap);

    if (count == 0U || lh_ptr_is_null(out) || out_size < count ||
        lh_ptr_is_null(lh_gl_pixmap_get_pixels(pixmap)))
    {
        return lh_bool_false;
    }
    const lh_gl_size_t *size = lh_gl_pixmap_get_size(pixmap);
    lh_gl_scalar_t width = lh_gl_size_get_width(size);

    lh_bit_pack_le32(lh_cast_static(lh_u32_t, width), lh_ptr_add_by_offset(lh_byte_t, out, 18U));
    lh_gl_scalar_t height = lh_gl_size_get_height(size);

    lh_bit_pack_le32(lh_cast_static(lh_u32_t, height), lh_ptr_add_by_offset(lh_byte_t, out, 22U));
    lh_bit_pack_le16(lh_cast_static(lh_u16_t, LH_GL_BMP_SIGNATURE), out);
    lh_bit_pack_le32(lh_cast_static(lh_u32_t, count), lh_ptr_add_by_offset(lh_byte_t, out, 2U));
    lh_bit_pack_le16(0U, lh_ptr_add_by_offset(lh_byte_t, out, 6U));
    lh_bit_pack_le16(0U, lh_ptr_add_by_offset(lh_byte_t, out, 8U));
    lh_bit_pack_le32(LH_GL_BMP_HEADER_BYTES, lh_ptr_add_by_offset(lh_byte_t, out, 10U));
    lh_bit_pack_le32(LH_GL_BMP_INFO_HEADER_BYTES, lh_ptr_add_by_offset(lh_byte_t, out, 14U));
    lh_bit_pack_le16(1U, lh_ptr_add_by_offset(lh_byte_t, out, 26U));
    lh_bit_pack_le16(lh_cast_static(lh_u16_t, LH_GL_BMP_BYTES_PER_PIXEL * 8U),
                     lh_ptr_add_by_offset(lh_byte_t, out, 28U));
    lh_bit_pack_le32(0U, lh_ptr_add_by_offset(lh_byte_t, out, 30U));
    lh_bit_pack_le32(lh_cast_static(lh_u32_t, count - LH_GL_BMP_HEADER_BYTES),
                     lh_ptr_add_by_offset(lh_byte_t, out, 34U));
    lh_bit_pack_le32(0U, lh_ptr_add_by_offset(lh_byte_t, out, 38U));
    lh_bit_pack_le32(0U, lh_ptr_add_by_offset(lh_byte_t, out, 42U));
    lh_bit_pack_le32(0U, lh_ptr_add_by_offset(lh_byte_t, out, 46U));
    lh_bit_pack_le32(0U, lh_ptr_add_by_offset(lh_byte_t, out, 50U));
    lh_usize_t at = LH_GL_BMP_HEADER_BYTES;

    for (lh_gl_scalar_t y = height - 1; y >= 0; --y)
    {
        for (lh_gl_scalar_t x = 0; x < width; ++x)
        {
            const lh_gl_color_t *pixel = lh_gl_pixmap_get_pixel(pixmap, x, y);

            out[at] = lh_gl_color_get_b(pixel);
            out[at + 1U] = lh_gl_color_get_g(pixel);
            out[at + 2U] = lh_gl_color_get_r(pixel);
            out[at + 3U] = lh_gl_color_get_a(pixel);
            at += LH_GL_BMP_BYTES_PER_PIXEL;
        }
    }
    return lh_bool_true;
}

lh_void
lh_gl_bmp_store(const lh_gl_pixmap_t *pixmap, lh_byte_ptr out, lh_usize_t out_size)
{
    lh_runtime_check_ifn(lh_gl_bmp_try_store(pixmap, out, out_size),
                         lh_runtime_error_code_out_of_range);
}
