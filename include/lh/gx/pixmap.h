/**
 * @file pixmap.h
 * @brief The pixels a thing is drawn into: ::lh_gx_pixmap_t.
 *
 * A pixmap is a **view of memory**, not a picture of its own: the bytes
 * belong to whoever provided them, and gx neither allocates them nor frees
 * them. It says how wide and how high the picture is, how a row steps through
 * the memory, and how one pixel is stored.
 *
 * That is what makes the same drawing work with and without an operating
 * system: on a desktop the bytes are the surface behind a native window, on a
 * board they are the buffer the application owns, and in both cases the
 * canvas paints into a pixmap and the platform shows it.
 *
 * A pixmap is a value: the application owns it, it is copied when it is
 * passed, and no part of gx allocates one or frees one.
 *
 * @see lh_gx_damage_t, what the canvas remembers about a pixmap's changes.
 */

#ifndef LH_GX_PIXMAP_H
#define LH_GX_PIXMAP_H

#include <lh/core/memory/bounds.h>
#include <lh/gx/pixmap/fields.h>
#include <lh/gx/pixmap/format.h>

/**
 * @struct lh_gx_pixmap
 * @brief A view of pixels in one format.
 */
struct lh_gx_pixmap
{
    /** Members: ::lh_gx_pixmap_fields. */
    lh_gx_pixmap_fields(lh_memory_bounds_t, lh_u32_t, lh_gx_pixmap_format_t);
};
/** @copybrief lh_gx_pixmap */
typedef struct lh_gx_pixmap lh_gx_pixmap_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief How many bytes one pixel of @p format takes.
 *
 * @param format The format.
 * @return The size of one pixel, 4 or 2.
 */
lh_u32_t
lh_gx_pixmap_bytes_per_pixel(lh_gx_pixmap_format_t format);

/**
 * @brief Fill @p self with a picture of @p width by @p height over @p bits.
 *
 * The row steps through the memory by the width times the size of a pixel.
 *
 * @param self   The pixmap; must not be null.
 * @param bits   The pixel bytes; not owned by the pixmap.
 * @param width  The width in pixels.
 * @param height The height in pixels.
 * @param format How one pixel is stored.
 */
lh_void
lh_gx_pixmap_init(lh_gx_pixmap_t *self, const lh_memory_bounds_t *bits, lh_u32_t width,
                  lh_u32_t height, lh_gx_pixmap_format_t format);

/**
 * @brief Fill @p self with a picture whose rows step by @p stride.
 *
 * For a buffer whose rows are padded, or a window into a bigger picture.
 *
 * @param self   The pixmap; must not be null.
 * @param bits   The pixel bytes; not owned by the pixmap.
 * @param width  The width in pixels.
 * @param height The height in pixels.
 * @param stride The bytes one row steps by.
 * @param format How one pixel is stored.
 */
lh_void
lh_gx_pixmap_init_stride(lh_gx_pixmap_t *self, const lh_memory_bounds_t *bits, lh_u32_t width,
                         lh_u32_t height, lh_u32_t stride, lh_gx_pixmap_format_t format);

/**
 * @brief The pixel bytes of @p self.
 *
 * @param self The pixmap; must not be null.
 * @return The bytes; not owned by the pixmap.
 */
const lh_memory_bounds_t *
lh_gx_pixmap_get_bits(const lh_gx_pixmap_t *self);

/**
 * @brief Store the pixel bytes of @p self.
 *
 * @param self The pixmap; must not be null.
 * @param bits The bytes; not owned by the pixmap.
 */
lh_void
lh_gx_pixmap_set_bits(lh_gx_pixmap_t *self, const lh_memory_bounds_t *bits);

/**
 * @brief The width of @p self in pixels.
 *
 * @param self The pixmap; must not be null.
 * @return The width.
 */
lh_u32_t
lh_gx_pixmap_get_width(const lh_gx_pixmap_t *self);

/**
 * @brief Store the width of @p self, and the row step that follows from it.
 *
 * @param self  The pixmap; must not be null.
 * @param width The width in pixels.
 */
lh_void
lh_gx_pixmap_set_width(lh_gx_pixmap_t *self, lh_u32_t width);

/**
 * @brief The height of @p self in pixels.
 *
 * @param self The pixmap; must not be null.
 * @return The height.
 */
lh_u32_t
lh_gx_pixmap_get_height(const lh_gx_pixmap_t *self);

/**
 * @brief Store the height of @p self.
 *
 * @param self   The pixmap; must not be null.
 * @param height The height in pixels.
 */
lh_void
lh_gx_pixmap_set_height(lh_gx_pixmap_t *self, lh_u32_t height);

/**
 * @brief How many bytes one row of @p self steps by.
 *
 * @param self The pixmap; must not be null.
 * @return The stride in bytes.
 */
lh_u32_t
lh_gx_pixmap_get_stride(const lh_gx_pixmap_t *self);

/**
 * @brief Store how many bytes one row of @p self steps by.
 *
 * @param self   The pixmap; must not be null.
 * @param stride The stride in bytes.
 */
lh_void
lh_gx_pixmap_set_stride(lh_gx_pixmap_t *self, lh_u32_t stride);

/**
 * @brief How one pixel of @p self is stored.
 *
 * @param self The pixmap; must not be null.
 * @return The format.
 */
lh_gx_pixmap_format_t
lh_gx_pixmap_get_format(const lh_gx_pixmap_t *self);

/**
 * @brief Store how one pixel of @p self is stored.
 *
 * @param self   The pixmap; must not be null.
 * @param format The format.
 */
lh_void
lh_gx_pixmap_set_format(lh_gx_pixmap_t *self, lh_gx_pixmap_format_t format);

/**
 * @brief How many bytes the whole picture of @p self takes.
 *
 * @param self The pixmap; must not be null.
 * @return The size of the picture in bytes.
 */
lh_u64_t
lh_gx_pixmap_size_bytes(const lh_gx_pixmap_t *self);

/**
 * @brief Whether the bytes of @p self hold the whole picture.
 *
 * True when the memory holds at least a stride times the height, and the
 * picture is not empty.
 *
 * @param self The pixmap; must not be null.
 * @return True when the picture fits in the bytes.
 */
lh_bool_t
lh_gx_pixmap_is_valid(const lh_gx_pixmap_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_PIXMAP_H */
