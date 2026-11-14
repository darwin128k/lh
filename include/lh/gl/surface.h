/**
 * @file surface.h
 * @brief Display-sized owner of fixed dirty-region storage.
 *
 * A surface borrows one root layer. Its size is independent of the layer tree;
 * attach and resize synchronize the root rectangle and invalidate the whole
 * surface. Detach does not change the root rectangle. Neither side allocates.
 */

#ifndef LH_GL_SURFACE_H
#define LH_GL_SURFACE_H

#include <lh/core/index.h>
#include <lh/gl/rect.h>
#include <lh/gl/layer/decl.h>
#include <lh/gl/surface/decl.h>
#include <lh/gl/surface/fields.h>

/** @brief Surface size, borrowed root, and fixed dirty-region storage. */
struct lh_gl_surface
{
    lh_gl_surface_fields(lh_gl_size_t, lh_gl_layer_t, lh_gl_rect_t, lh_usize_t);
};

LH_COMPILER_EXTERN_C_BEGIN

/** @brief Mutable surface size; pointer remains valid for the surface lifetime. */
lh_gl_size_t *
lh_gl_surface_get_size_mut(lh_gl_surface_t *self);

/** @brief Surface size; pointer remains valid for the surface lifetime. */
const lh_gl_size_t *
lh_gl_surface_get_size(const lh_gl_surface_t *self);

/** @brief Borrowed root layer, or ::lh_null. */
lh_gl_layer_t *
lh_gl_surface_get_root(const lh_gl_surface_t *self);

/** @brief Store a borrowed root; attach/detach maintain the reciprocal link. */
lh_void
lh_gl_surface_set_root(lh_gl_surface_t *self, lh_gl_layer_t *root);

/** @brief Mutable dirty storage slot; index must be below the configured capacity. */
lh_gl_rect_t *
lh_gl_surface_get_dirty_mut(lh_gl_surface_t *self, lh_uindex_t index);

/** @brief Number of occupied dirty slots. */
lh_usize_t
lh_gl_surface_get_dirty_count(const lh_gl_surface_t *self);

/** @brief Store the number of occupied slots; count must fit the capacity. */
lh_void
lh_gl_surface_set_dirty_count(lh_gl_surface_t *self, lh_usize_t count);

/** @brief Dirty area at index below the count; valid until the list changes. */
const lh_gl_rect_t *
lh_gl_surface_get_dirty(const lh_gl_surface_t *self, lh_uindex_t index);

/** @brief Initialize an unattached surface of size, with no dirty areas. */
lh_void
lh_gl_surface_init(lh_gl_surface_t *self, lh_gl_size_t size);

/** @brief Attach an unattached root, set its rectangle to the surface and invalidate all.
 * The layer must have no parent or surface; both objects must outlive attachment.
 * Any previous root is detached. Both arguments must be non-null.
 */
lh_void
lh_gl_surface_attach(lh_gl_surface_t *self, lh_gl_layer_t *root);

/** @brief Detach the root if present; its rectangle remains unchanged. */
lh_void
lh_gl_surface_detach(lh_gl_surface_t *self);

/** @brief Set size, synchronize the attached root and invalidate the whole surface. */
lh_void
lh_gl_surface_resize(lh_gl_surface_t *self, lh_gl_size_t size);

/** @brief Remove all dirty areas. */
lh_void
lh_gl_surface_clear_dirty(lh_gl_surface_t *self);

/** @brief Clip an area to the surface and accumulate it without allocation.
 * Contained areas are dropped; a union is merged when its area does not exceed
 * the sum of its inputs. A full array collapses to one bounding rectangle.
 * Both pointers must be non-null; the area is copied.
 */
lh_void
lh_gl_surface_invalidate(lh_gl_surface_t *self, const lh_gl_rect_t *area);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_SURFACE_H */
