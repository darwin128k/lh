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

#include <lh/bool.h>
#include <lh/byte.h>
#include <lh/memory.h>
#include <lh/null.h>
#include <lh/os/alloc.h>
#include <lh/os/system/posix/connection.h>
#include <lh/os/system/posix/paint.h>
#include <lh/os/system/posix/surface/fields.h>
#include <lh/os/system/surface.h>
#include <lh/runtime/allocator.h>
#include <lh/size.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>
#include <lh/util/return.h>

#include <sys/ipc.h>
#include <sys/shm.h>

/** @brief A surface: its pixels and what they look like. */
struct lh_os_system_posix_surface
{
    lh_os_system_posix_surface_fields;
};
typedef struct lh_os_system_posix_surface lh_os_system_posix_surface_t;

/* -1 not tried yet, 0 the server cannot read our memory, 1 it can. Learned once:
   a server that refused one segment refuses them all, and every refusal costs a
   round trip. */
static int lh_os_system_posix_surface_shm = -1;

/* Put @p surface's pixels in a segment the server is attached to, or say it
   cannot be done. The segment is marked for removal as soon as both sides hold
   it, so it goes away with the last of them even if this process does not end
   cleanly -- a leaked segment outlives the program that made it. */
static lh_bool_t
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
    id = shmget(IPC_PRIVATE, (size_t)bytes, IPC_CREAT | 0600);
    lh_return_if(id < 0, lh_bool_false);
    address = shmat(id, lh_null, 0);
    if (address == (void *)-1)
    {
        (void)shmctl(id, IPC_RMID, lh_null);
        return lh_bool_false;
    }
    lh_memory_set(lh_addr_of(surface->segment), sizeof(surface->segment), 0);
    surface->segment.shmid = id;
    surface->segment.shmaddr = lh_ptr_rcast(char, address);
    surface->segment.read_only = 0;
    /* The attach is answered asynchronously; a server on another machine answers it
       with an error, and the sync is what makes the answer arrive now. */
    (void)lh_os_system_posix_connection_take_error();
    (void)ext->XShmAttach(display, lh_addr_of(surface->segment));
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
    surface->bits = lh_ptr_rcast(lh_byte_t, address);
    surface->display = display;
    surface->shared = lh_bool_true;
    return lh_bool_true;
}

/* Wait until the server has read what the last present gave it. */
static lh_void
lh_os_system_posix_surface_settle(lh_os_system_posix_surface_t *surface)
{
    if (surface->pending)
    {
        (void)lh_os_system_posix_x11_load()->XSync(surface->display, 0);
        surface->pending = lh_bool_false;
    }
}

lh_os_system_surface_handle_t
lh_os_system_surface_create(int width, int height, int bits)
{
    lh_os_system_posix_surface_t *surface;

    lh_return_if(width <= 0 || height <= 0, LH_OS_SYSTEM_SURFACE_HANDLE_INVALID);
    lh_return_if(bits != 16 && bits != 32, LH_OS_SYSTEM_SURFACE_HANDLE_INVALID);
    surface = lh_ptr_rcast(lh_os_system_posix_surface_t, lh_os_alloc(sizeof(*surface)));
    lh_return_if(lh_null_eq(surface), LH_OS_SYSTEM_SURFACE_HANDLE_INVALID);
    lh_memory_set(surface, sizeof(*surface), 0);
    /* Only 32 bits are shared: a 16-bit surface is widened on its way out, so the
       server never reads it as it is. */
    if (bits != 32 ||
        !lh_os_system_posix_surface_share(surface, (lh_usize_t)width * (lh_usize_t)height * 4U))
    {
        surface->bits =
            lh_ptr_rcast(lh_byte_t, lh_os_alloc((lh_usize_t)width * (lh_usize_t)height * (lh_usize_t)(bits / 8)));
        if (lh_null_eq(surface->bits))
        {
            lh_runtime_allocator_free(surface);
            return LH_OS_SYSTEM_SURFACE_HANDLE_INVALID;
        }
    }
    surface->width = width;
    surface->height = height;
    surface->pixel_bits = bits;
    return surface;
}

lh_void
lh_os_system_surface_destroy(lh_os_system_surface_handle_t handle)
{
    lh_os_system_posix_surface_t *surface;

    lh_return_if(lh_null_eq(handle));
    surface = lh_ptr_rcast(lh_os_system_posix_surface_t, handle);
    if (lh_null_ne(surface->staging))
    {
        lh_runtime_allocator_free(surface->staging);
    }
    if (surface->shared)
    {
        /* Detached and synced before the memory goes: the server may still be
           reading the last present out of it. */
        lh_os_system_posix_surface_settle(surface);
        (void)lh_os_system_posix_xext_load()->XShmDetach(surface->display, lh_addr_of(surface->segment));
        (void)lh_os_system_posix_x11_load()->XSync(surface->display, 0);
        (void)shmdt(surface->segment.shmaddr);
    }
    else
    {
        lh_runtime_allocator_free(surface->bits);
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
    return surface->bits;
}

lh_ptr
lh_os_system_surface_get_dc(lh_os_system_surface_handle_t handle)
{
    /* X has no memory device context: nothing draws into a surface through the
       window system here, it is drawn into as memory and presented. */
    (void)handle;
    return lh_null;
}

/* RGB565 to 0xFFRRGGBB, with each channel's top bits repeated into its bottom
   ones so that full scale stays full scale. Opaque: a 24-bit window ignores the
   top byte, and a 32-bit one -- a window with round corners -- reads it as alpha. */
static lh_u32_t
lh_os_system_posix_surface_from_565(lh_u16_t pixel)
{
    const lh_u32_t r = (lh_u32_t)(pixel >> 11) & 0x1fU;
    const lh_u32_t g = (lh_u32_t)(pixel >> 5) & 0x3fU;
    const lh_u32_t b = (lh_u32_t)pixel & 0x1fU;

    return 0xff000000U | ((r << 3 | r >> 2) << 16) | ((g << 2 | g >> 4) << 8) | (b << 3 | b >> 2);
}

/* The rows of a 16-bit surface that a present covers, widened into the staging
   buffer. Grown, never shrunk: a window presents the same sizes frame after frame. */
static lh_u32_t *
lh_os_system_posix_surface_widen(lh_os_system_posix_surface_t *surface, int src_x, int src_y, int width,
                                 int height)
{
    const lh_usize_t words = (lh_usize_t)width * (lh_usize_t)height;
    int x;
    int y;

    if (surface->staging_words < words)
    {
        lh_u32_t *grown = lh_ptr_rcast(lh_u32_t, lh_os_alloc(words * sizeof(lh_u32_t)));

        lh_return_if(lh_null_eq(grown), lh_null);
        if (lh_null_ne(surface->staging))
        {
            lh_runtime_allocator_free(surface->staging);
        }
        surface->staging = grown;
        surface->staging_words = words;
    }
    for (y = 0; y < height; ++y)
    {
        const lh_u16_t *row = lh_ptr_rcast(const lh_u16_t, surface->bits) + (lh_usize_t)(src_y + y) * (lh_usize_t)surface->width;

        for (x = 0; x < width; ++x)
        {
            surface->staging[(lh_usize_t)y * (lh_usize_t)width + (lh_usize_t)x] =
                lh_os_system_posix_surface_from_565(row[src_x + x]);
        }
    }
    return surface->staging;
}

lh_bool_t
lh_os_system_surface_present_part(lh_os_system_surface_handle_t handle, lh_ptr dest, int x, int y,
                                  int src_x, int src_y, int width, int height)
{
    lh_os_system_posix_surface_t *surface;
    const lh_os_system_posix_paint_t *paint;
    lh_os_system_posix_ximage_t image;
    const lh_u32_t one = 1U;
    lh_byte_t *data;
    int image_width;
    int image_height;

    lh_return_if(lh_null_eq(handle) || lh_null_eq(dest), lh_bool_false);
    surface = lh_ptr_rcast(lh_os_system_posix_surface_t, handle);
    paint = lh_ptr_rcast(const lh_os_system_posix_paint_t, dest);
    /* The part has to be inside the surface: XPutImage reads what it is told to,
       and a part past the edge is memory that is not ours. */
    lh_return_if(width <= 0 || height <= 0 || src_x < 0 || src_y < 0, lh_bool_false);
    lh_return_if(src_x + width > surface->width || src_y + height > surface->height, lh_bool_false);
    /* Only TrueColor 24 and 32: the words are 0x00RRGGBB, which is what those
       visuals take. A paletted or 16-bit screen is not one this backend draws on. */
    lh_return_if(paint->depth != 24 && paint->depth != 32, lh_bool_false);

    if (surface->pixel_bits == 16)
    {
        data = lh_ptr_rcast(lh_byte_t, lh_os_system_posix_surface_widen(surface, src_x, src_y, width, height));
        lh_return_if(lh_null_eq(data), lh_bool_false);
        image_width = width;
        image_height = height;
        src_x = 0;
        src_y = 0;
    }
    else
    {
        data = surface->bits;
        image_width = surface->width;
        image_height = surface->height;
    }

    lh_memory_set(lh_addr_of(image), sizeof(image), 0);
    image.width = image_width;
    image.height = image_height;
    image.format = LH_OS_SYSTEM_POSIX_X_ZPIXMAP;
    image.data = lh_ptr_rcast(char, data);
    /* The words are in this machine's byte order; Xlib swaps them if the server's
       is the other one. */
    image.byte_order = *lh_ptr_rcast(const lh_byte_t, lh_addr_of(one)) == 1U ? LH_OS_SYSTEM_POSIX_X_LSB_FIRST
                                                                            : LH_OS_SYSTEM_POSIX_X_MSB_FIRST;
    image.bitmap_unit = 32;
    image.bitmap_bit_order = image.byte_order;
    image.bitmap_pad = 32;
    image.depth = paint->depth;
    image.bytes_per_line = image_width * 4;
    image.bits_per_pixel = 32;
    image.red_mask = 0x00ff0000UL;
    image.green_mask = 0x0000ff00UL;
    image.blue_mask = 0x000000ffUL;
    if (surface->shared)
    {
        /* XShmPutImage finds the segment through obdata. */
        image.obdata = lh_ptr_rcast(char, lh_addr_of(surface->segment));
    }
    lh_return_if(paint->x11->XInitImage(lh_addr_of(image)) == 0, lh_bool_false);
    if (surface->shared && surface->display == paint->display)
    {
        (void)lh_os_system_posix_xext_load()->XShmPutImage(paint->display, paint->window, paint->gc, lh_addr_of(image),
                                                           src_x, src_y, x, y, (unsigned int)width,
                                                           (unsigned int)height, 0);
        surface->pending = lh_bool_true;
        return lh_bool_true;
    }
    paint->x11->XPutImage(paint->display, paint->window, paint->gc, lh_addr_of(image), src_x, src_y, x, y,
                          (unsigned int)width, (unsigned int)height);
    return lh_bool_true;
}

lh_bool_t
lh_os_system_surface_present_at(lh_os_system_surface_handle_t handle, lh_ptr dest, int x, int y)
{
    const lh_os_system_posix_surface_t *surface;

    lh_return_if(lh_null_eq(handle) || lh_null_eq(dest), lh_bool_false);
    surface = lh_ptr_rcast(const lh_os_system_posix_surface_t, handle);
    return lh_os_system_surface_present_part(handle, dest, x, y, 0, 0, surface->width, surface->height);
}
