/**
 * @file surface.c
 * @brief X11 off-screen surface: client memory, presented with `XPutImage`.
 *
 * The surface is memory in the 0xAARRGGBB words the rest of lh draws in, and a
 * present describes that memory to Xlib as an `XImage` on the spot — nothing is
 * allocated for it and nothing has to be freed. Xlib converts byte order if the
 * server's differs from ours.
 *
 * Where the server is on this machine and has MIT-SHM, a 32-bit surface is a
 * System V shared-memory segment the server reads directly, and a present sends a
 * request instead of the pixels: an 800x600 frame is 1.9 MB that does not go
 * through the socket. The server reads it later, so a write to the surface waits
 * for that first (::lh_os_system_surface_get_pixels) — the same promise GdiFlush
 * keeps on Windows. Anywhere else, the plain path: the pixels in the request.
 */
#include <lh/assert.h>

#include <lh/byte/ptr.h>
#include <lh/memory.h>
#include <lh/os/alloc.h>
#include <lh/os/system/posix/connection.h>
#include <lh/os/system/posix/paint.h>
#include <lh/os/system/posix/surface.h>
#include <lh/os/system/posix/surface/rgb565.h>
#include <lh/os/system/surface.h>
#include <lh/runtime/allocator.h>
#include <lh/util/return.h>

#include <sys/ipc.h>
#include <sys/shm.h>

/** -1 not tried yet, 0 the server cannot read our memory, 1 it can. Learned once:
   a server that refused one segment refuses them all, and every refusal costs a
   round trip. */
static int lh_os_system_posix_surface_shm = -1;

lh_bool_t
lh_os_system_posix_surface_share(lh_os_system_posix_surface_t *surface, lh_usize_t bytes)
{
    lh_os_system_posix_display_t *display = lh_os_system_posix_connection_get_display();
    const lh_os_system_posix_x11_t *x = lh_os_system_posix_x11_load();
    const lh_os_system_posix_xext_t *ext = lh_os_system_posix_xext_load();
    lh_bool_t refused;
    void *address;
    int id;

    lh_return_if(lh_os_system_posix_surface_shm == 0, lh_bool_false);
    lh_return_if(lh_null_eq(display) || lh_null_eq(x) || lh_null_eq(ext), lh_bool_false);
    if (lh_os_system_posix_surface_shm < 0 && ext->XShmQueryExtension(display) == 0)
    {
        lh_os_system_posix_surface_shm = 0;
        return lh_bool_false;
    }
    id = shmget(IPC_PRIVATE, lh_cast_static(size_t, bytes), IPC_CREAT | 0600);
    lh_return_if(id < 0, lh_bool_false);
    address = shmat(id, lh_null, 0);
    if (address == LH_PTR_T_MAX)
    {
        (void)shmctl(id, IPC_RMID, lh_null);
        return lh_bool_false;
    }
    lh_memory_set(lh_os_system_posix_surface_get_segment_mut(surface), sizeof(surface->segment), 0);
    lh_os_system_posix_surface_get_segment_mut(surface)->shmid = id;
    lh_os_system_posix_surface_get_segment_mut(surface)->shmaddr = lh_ptr_rcast(char, address);
    lh_os_system_posix_surface_get_segment_mut(surface)->read_only = 0;
    /* The attach is answered asynchronously; a server on another machine answers it
       with an error, and the sync is what makes the answer arrive now. */
    (void)lh_os_system_posix_connection_take_error();
    (void)ext->XShmAttach(display, lh_os_system_posix_surface_get_segment_mut(surface));
    (void)x->XSync(display, 0);
    refused = lh_os_system_posix_connection_take_error();
    (void)shmctl(id, IPC_RMID, lh_null);
    if (refused)
    {
        (void)shmdt(address);
        lh_os_system_posix_surface_shm = 0;
        return lh_bool_false;
    }
    lh_os_system_posix_surface_shm = 1;
    lh_os_system_posix_surface_set_bits(surface, lh_ptr_rcast(lh_byte_t, address));
    lh_os_system_posix_surface_set_display(surface, display);
    lh_os_system_posix_surface_set_shared(surface, lh_bool_true);
    return lh_bool_true;
}

lh_void
lh_os_system_posix_surface_settle(lh_os_system_posix_surface_t *surface)
{
    if (lh_os_system_posix_surface_is_pending(surface))
    {
        (void)lh_os_system_posix_x11_load()->XSync(lh_os_system_posix_surface_get_display(surface),
                                                   0);
        lh_os_system_posix_surface_set_pending(surface, lh_bool_false);
    }
}

lh_os_system_surface_handle_t
lh_os_system_surface_create(int width, int height, int bits)
{
    lh_os_system_posix_surface_t *surface;

    lh_return_if(width <= 0 || height <= 0, LH_OS_SYSTEM_SURFACE_HANDLE_INVALID);
    lh_return_if(bits != 16 && bits != 32, LH_OS_SYSTEM_SURFACE_HANDLE_INVALID);
    surface =
        lh_ptr_rcast(lh_os_system_posix_surface_t, lh_os_alloc(sizeof(lh_ptr_deref(surface))));
    lh_return_if(lh_null_eq(surface), LH_OS_SYSTEM_SURFACE_HANDLE_INVALID);
    lh_memory_set(surface, sizeof(lh_ptr_deref(surface)), 0);
    /* Only 32 bits are shared: a 16-bit surface is widened on its way out, so the
       server never reads it as it is. */
    if (bits != 32 ||
        !lh_os_system_posix_surface_share(surface, lh_cast_static(lh_usize_t, width) *
                                                       lh_cast_static(lh_usize_t, height) * 4U))
    {
        lh_os_system_posix_surface_set_bits(
            surface, lh_ptr_rcast(lh_byte_t, lh_os_alloc(lh_cast_static(lh_usize_t, width) *
                                                         lh_cast_static(lh_usize_t, height) *
                                                         lh_cast_static(lh_usize_t, (bits / 8)))));
        if (lh_null_eq(lh_os_system_posix_surface_get_bits(surface)))
        {
            lh_runtime_allocator_free(surface);
            return LH_OS_SYSTEM_SURFACE_HANDLE_INVALID;
        }
    }
    lh_os_system_posix_surface_set_width(surface, width);
    lh_os_system_posix_surface_set_height(surface, height);
    lh_os_system_posix_surface_set_pixel_bits(surface, bits);
    return surface;
}

lh_void
lh_os_system_surface_destroy(lh_os_system_surface_handle_t handle)
{
    lh_os_system_posix_surface_t *surface;

    lh_return_if(lh_null_eq(handle));
    surface = lh_ptr_rcast(lh_os_system_posix_surface_t, handle);
    if (lh_null_ne(lh_os_system_posix_surface_get_staging(surface)))
    {
        lh_runtime_allocator_free(lh_os_system_posix_surface_get_staging(surface));
    }
    if (lh_os_system_posix_surface_is_shared(surface))
    {
        /* Detached and synced before the memory goes: the server may still be
           reading the last present out of it. */
        lh_os_system_posix_surface_settle(surface);
        (void)lh_os_system_posix_xext_load()->XShmDetach(
            lh_os_system_posix_surface_get_display(surface),
            lh_os_system_posix_surface_get_segment_mut(surface));
        (void)lh_os_system_posix_x11_load()->XSync(lh_os_system_posix_surface_get_display(surface),
                                                   0);
        (void)shmdt(lh_os_system_posix_surface_get_segment_mut(surface)->shmaddr);
    }
    else
    {
        lh_runtime_allocator_free(lh_os_system_posix_surface_get_bits(surface));
    }
    lh_runtime_allocator_free(surface);
}

int
lh_os_system_surface_get_width(lh_os_system_surface_handle_t handle)
{
    lh_return_if(lh_null_eq(handle), 0);
    return lh_ptr_rcast(const lh_os_system_posix_surface_t, handle)->width;
}

int
lh_os_system_surface_get_height(lh_os_system_surface_handle_t handle)
{
    lh_return_if(lh_null_eq(handle), 0);
    return lh_ptr_rcast(const lh_os_system_posix_surface_t, handle)->height;
}

lh_ptr
lh_os_system_surface_get_pixels(lh_os_system_surface_handle_t handle)
{
    lh_os_system_posix_surface_t *surface;

    lh_return_if(lh_null_eq(handle), lh_null);
    surface = lh_ptr_rcast(lh_os_system_posix_surface_t, handle);
    /* Whoever asks for the pixels is about to write them, and the server may not
       have read the last frame out of them yet. */
    lh_os_system_posix_surface_settle(surface);
    return lh_os_system_posix_surface_get_bits(surface);
}

lh_ptr
lh_os_system_surface_get_dc(lh_os_system_surface_handle_t handle)
{
    /* X has no memory device context: nothing draws into a surface through the
       window system here, it is drawn into as memory and presented. */
    (void)handle;
    return lh_null;
}

lh_u32_t *
lh_os_system_posix_surface_widen(lh_os_system_posix_surface_t *surface, int src_x, int src_y,
                                 int width, int height)
{
    const lh_usize_t words = lh_cast_static(lh_usize_t, width) * lh_cast_static(lh_usize_t, height);

    if (lh_os_system_posix_surface_get_staging_words(surface) < words)
    {
        lh_u32_t *grown = lh_ptr_rcast(lh_u32_t, lh_os_alloc(words * sizeof(lh_u32_t)));

        lh_return_if(lh_null_eq(grown), lh_null);
        if (lh_null_ne(lh_os_system_posix_surface_get_staging(surface)))
        {
            lh_runtime_allocator_free(lh_os_system_posix_surface_get_staging(surface));
        }
        lh_os_system_posix_surface_set_staging(surface, grown);
        lh_os_system_posix_surface_set_staging_words(surface, words);
    }
    for (lh_int_t y = 0; y < height; ++y)
    {
        const lh_u16_t *row =
            lh_ptr_rcast(const lh_u16_t, lh_os_system_posix_surface_get_bits(surface)) +
            lh_cast_static(lh_usize_t, (src_y + y)) *
                lh_cast_static(lh_usize_t, lh_os_system_posix_surface_get_width(surface));

        for (lh_int_t x = 0; x < width; ++x)
        {
            lh_os_system_posix_surface_get_staging(
                surface)[lh_cast_static(lh_usize_t, y) * lh_cast_static(lh_usize_t, width) +
                         lh_cast_static(lh_usize_t, x)] =
                lh_os_system_posix_surface_from_565(row[src_x + x]);
        }
    }
    return lh_os_system_posix_surface_get_staging(surface);
}

lh_bool_t
lh_os_system_surface_present_part(lh_os_system_surface_handle_t handle, lh_ptr dest, int x, int y,
                                  int src_x, int src_y, int width, int height)
{
    lh_os_system_posix_surface_t *surface;
    const lh_os_system_posix_paint_t *paint;
    lh_os_system_posix_ximage_t image;
    const lh_u32_t one = 1U;

    lh_return_if(lh_null_eq(handle) || lh_null_eq(dest), lh_bool_false);
    surface = lh_ptr_rcast(lh_os_system_posix_surface_t, handle);
    paint = lh_ptr_rcast(const lh_os_system_posix_paint_t, dest);
    /* The part has to be inside the surface: XPutImage reads what it is told to,
       and a part past the edge is memory that is not ours. */
    lh_return_if(width <= 0 || height <= 0 || src_x < 0 || src_y < 0, lh_bool_false);
    lh_return_if(src_x + width > lh_os_system_posix_surface_get_width(surface) ||
                     src_y + height > lh_os_system_posix_surface_get_height(surface),
                 lh_bool_false);
    /* Only TrueColor 24 and 32: the words are 0x00RRGGBB, which is what those
       visuals take. A paletted or 16-bit screen is not one this backend draws on. */
    lh_return_if(lh_os_system_posix_paint_get_depth(paint) != 24 &&
                     lh_os_system_posix_paint_get_depth(paint) != 32,
                 lh_bool_false);

    {
        lh_byte_ptr data;
        int image_width;
        int image_height;

        if (lh_os_system_posix_surface_get_pixel_bits(surface) == 16)
        {
            data = lh_ptr_rcast(lh_byte_t,
                                lh_os_system_posix_surface_widen(surface, src_x, src_y, width,
                                                                 height));
            lh_return_if(lh_null_eq(data), lh_bool_false);
            image_width = width;
            image_height = height;
            src_x = 0;
            src_y = 0;
        }
        else
        {
            data = lh_os_system_posix_surface_get_bits(surface);
            image_width = lh_os_system_posix_surface_get_width(surface);
            image_height = lh_os_system_posix_surface_get_height(surface);
        }

        lh_memory_set(lh_addr_of(image), sizeof(image), 0);
        image.width = image_width;
        image.height = image_height;
        image.format = LH_OS_SYSTEM_POSIX_X_ZPIXMAP;
        image.data = lh_ptr_rcast(char, data);
        /* The words are in this machine's byte order; Xlib swaps them if the server's
           is the other one. */
        image.byte_order = *lh_ptr_rcast(const lh_byte_t, lh_addr_of(one)) == 1U
                               ? LH_OS_SYSTEM_POSIX_X_LSB_FIRST
                               : LH_OS_SYSTEM_POSIX_X_MSB_FIRST;
        image.bitmap_unit = 32;
        image.bitmap_bit_order = image.byte_order;
        image.bitmap_pad = 32;
        image.depth = lh_os_system_posix_paint_get_depth(paint);
        image.bytes_per_line = image_width * 4;
        image.bits_per_pixel = 32;
        image.red_mask = 0x00ff0000UL;
        image.green_mask = 0x0000ff00UL;
        image.blue_mask = 0x000000ffUL;
    }
    if (lh_os_system_posix_surface_is_shared(surface))
    {
        /* XShmPutImage finds the segment through obdata. */
        image.obdata = lh_ptr_rcast(char, lh_os_system_posix_surface_get_segment_mut(surface));
    }
    lh_return_if(lh_os_system_posix_paint_get_x11(paint)->XInitImage(lh_addr_of(image)) == 0,
                 lh_bool_false);
    if (lh_os_system_posix_surface_is_shared(surface) &&
        lh_os_system_posix_surface_get_display(surface) ==
            lh_os_system_posix_paint_get_display(paint))
    {
        (void)lh_os_system_posix_xext_load()->XShmPutImage(
            lh_os_system_posix_paint_get_display(paint), lh_os_system_posix_paint_get_window(paint),
            lh_os_system_posix_paint_get_gc(paint), lh_addr_of(image), src_x, src_y, x, y,
            lh_cast_static(unsigned int, width), lh_cast_static(unsigned int, height), 0);
        lh_os_system_posix_surface_set_pending(surface, lh_bool_true);
        return lh_bool_true;
    }
    lh_os_system_posix_paint_get_x11(paint)->XPutImage(
        lh_os_system_posix_paint_get_display(paint), lh_os_system_posix_paint_get_window(paint),
        lh_os_system_posix_paint_get_gc(paint), lh_addr_of(image), src_x, src_y, x, y,
        lh_cast_static(unsigned int, width), lh_cast_static(unsigned int, height));
    return lh_bool_true;
}

lh_bool_t
lh_os_system_surface_present_at(lh_os_system_surface_handle_t handle, lh_ptr dest, int x, int y)
{
    const lh_os_system_posix_surface_t *surface;

    lh_return_if(lh_null_eq(handle) || lh_null_eq(dest), lh_bool_false);
    surface = lh_ptr_rcast(const lh_os_system_posix_surface_t, handle);
    return lh_os_system_surface_present_part(handle, dest, x, y, 0, 0,
                                             lh_os_system_posix_surface_get_width(surface),
                                             lh_os_system_posix_surface_get_height(surface));
}

lh_bool_t
lh_os_system_surface_present_fit(lh_os_system_surface_handle_t handle, lh_ptr dest, int dest_w,
                                 int dest_h, lh_u32_t fill)
{
    (void)dest_w;
    (void)dest_h;
    (void)fill;
    return lh_os_system_surface_present_at(handle, dest, 0, 0);
}

lh_byte_ptr
lh_os_system_posix_surface_get_bits(const lh_os_system_posix_surface_t *self)
{
    lh_assert_runtime_ref(self);
    return self->bits;
}

lh_void
lh_os_system_posix_surface_set_bits(lh_os_system_posix_surface_t *self, lh_byte_ptr bits)
{
    lh_assert_runtime_ref(self);
    self->bits = bits;
}

int
lh_os_system_posix_surface_get_width(const lh_os_system_posix_surface_t *self)
{
    lh_assert_runtime_ref(self);
    return self->width;
}

lh_void
lh_os_system_posix_surface_set_width(lh_os_system_posix_surface_t *self, int width)
{
    lh_assert_runtime_ref(self);
    self->width = width;
}

int
lh_os_system_posix_surface_get_height(const lh_os_system_posix_surface_t *self)
{
    lh_assert_runtime_ref(self);
    return self->height;
}

lh_void
lh_os_system_posix_surface_set_height(lh_os_system_posix_surface_t *self, int height)
{
    lh_assert_runtime_ref(self);
    self->height = height;
}

int
lh_os_system_posix_surface_get_pixel_bits(const lh_os_system_posix_surface_t *self)
{
    lh_assert_runtime_ref(self);
    return self->pixel_bits;
}

lh_void
lh_os_system_posix_surface_set_pixel_bits(lh_os_system_posix_surface_t *self, int pixel_bits)
{
    lh_assert_runtime_ref(self);
    self->pixel_bits = pixel_bits;
}

lh_u32_t *
lh_os_system_posix_surface_get_staging(const lh_os_system_posix_surface_t *self)
{
    lh_assert_runtime_ref(self);
    return self->staging;
}

lh_void
lh_os_system_posix_surface_set_staging(lh_os_system_posix_surface_t *self, lh_u32_t *staging)
{
    lh_assert_runtime_ref(self);
    self->staging = staging;
}

lh_usize_t
lh_os_system_posix_surface_get_staging_words(const lh_os_system_posix_surface_t *self)
{
    lh_assert_runtime_ref(self);
    return self->staging_words;
}

lh_void
lh_os_system_posix_surface_set_staging_words(lh_os_system_posix_surface_t *self,
                                             lh_usize_t staging_words)
{
    lh_assert_runtime_ref(self);
    self->staging_words = staging_words;
}

const lh_os_system_posix_xshm_segment_info_t *
lh_os_system_posix_surface_get_segment(const lh_os_system_posix_surface_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->segment);
}

lh_os_system_posix_xshm_segment_info_t *
lh_os_system_posix_surface_get_segment_mut(lh_os_system_posix_surface_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->segment);
}

lh_os_system_posix_display_t *
lh_os_system_posix_surface_get_display(const lh_os_system_posix_surface_t *self)
{
    lh_assert_runtime_ref(self);
    return self->display;
}

lh_void
lh_os_system_posix_surface_set_display(lh_os_system_posix_surface_t *self,
                                       lh_os_system_posix_display_t *display)
{
    lh_assert_runtime_ref(self);
    self->display = display;
}

lh_bool_t
lh_os_system_posix_surface_is_shared(const lh_os_system_posix_surface_t *self)
{
    lh_assert_runtime_ref(self);
    return self->shared;
}

lh_void
lh_os_system_posix_surface_set_shared(lh_os_system_posix_surface_t *self, lh_bool_t shared)
{
    lh_assert_runtime_ref(self);
    self->shared = shared;
}

lh_bool_t
lh_os_system_posix_surface_is_pending(const lh_os_system_posix_surface_t *self)
{
    lh_assert_runtime_ref(self);
    return self->pending;
}

lh_void
lh_os_system_posix_surface_set_pending(lh_os_system_posix_surface_t *self, lh_bool_t pending)
{
    lh_assert_runtime_ref(self);
    self->pending = pending;
}
