/**
 * @file loader.h
 * @brief The privilege to load child modules (::lh_os_loader_t).
 *
 * Not an object of its own: a module is granted one
 * (::lh_os_module_grant_loader) and owns it. The loader is the mechanism —
 * open an image, find the method table it exports, run its start; on
 * unload, close children in reverse order. What a child does is the
 * child's business (::lh_os_module_ops_t).
 *
 * A child is an image that exports an ::lh_os_module_ops_t under the
 * loader's `entry` name. An image without it is not a module and is
 * rejected. Each child is allocated on its own, so a pointer from
 * ::lh_os_loader_load or ::lh_os_loader_get stays valid until the child is
 * unloaded.
 *
 * Directory scan is one level, shared images only (the platform suffix).
 * Subdirectories and symbolic links are skipped. A missing directory, or a
 * path that is not a directory, succeeds with nothing loaded. One rejected
 * child does not fail the scan.
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_LOADER_H
#define LH_OS_LOADER_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/fs/path.h>
#include <lh/index.h>
#include <lh/os/loader/fields.h>
#include <lh/os/module.h>
#include <lh/size.h>
#include <lh/str.h>
#include <lh/str/ptr.h>
#include <lh/vector.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/loader.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

/**
 * @struct lh_os_loader
 * @brief Children, owner, entry name. Fields via ::lh_os_loader_fields.
 */
typedef struct lh_os_loader
{
    lh_os_loader_fields(lh_vector_t, lh_os_module_t *, lh_str_t);
} lh_os_loader_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief No children yet. Each child's owner is this loader, so @p owner
 *        is each child's parent (::lh_os_module_get_parent).
 *
 * Normally called by ::lh_os_module_grant_loader, not directly.
 *
 * @param owner Module holding this privilege.
 * @param entry Name a child exports its ::lh_os_module_ops_t under. Copied.
 */
void
lh_os_loader_init(lh_os_loader_t *self, lh_os_module_t *owner, lh_str_cptr entry);

/**
 * @brief Unload every child, then release the loader's own storage.
 */
void
lh_os_loader_deinit(lh_os_loader_t *self);

/**
 * @brief Close every child, last loaded first. The loader stays usable.
 */
void
lh_os_loader_unload(lh_os_loader_t *self);

/**
 * @brief The module holding this privilege.
 */
lh_os_module_t *
lh_os_loader_get_owner(const lh_os_loader_t *self);

/**
 * @brief Name a child exports its method table under.
 */
lh_str_cptr
lh_os_loader_get_entry(const lh_os_loader_t *self);

/**
 * @brief Open @p path, take its method table, start it, keep it.
 *
 * @return The child, or ::lh_null if the image did not open, exports no
 *         method table, or its start refused. Nothing is kept then.
 */
lh_os_module_t *
lh_os_loader_load(lh_os_loader_t *self, const lh_fs_path_t *path);

/**
 * @brief Load every shared image in @p dir (one level).
 *
 * Skips the image at @p skip when it is not ::lh_null (the file whose
 * sibling directory is being scanned). Empty @p dir is an error.
 */
lh_bool_t
lh_os_loader_load_dir(lh_os_loader_t *self, const lh_fs_path_t *dir, const lh_fs_path_t *skip);

/**
 * @brief Load shared images from @p subdir beside @p image.
 *
 * @p image is the file, not the directory. @p subdir is a relative name
 * (`modules`). The image file itself is skipped.
 */
lh_bool_t
lh_os_loader_load_beside(lh_os_loader_t *self, const lh_fs_path_t *image, const lh_fs_path_t *subdir);

/**
 * @brief Number of children currently loaded.
 */
lh_usize_t
lh_os_loader_get_loaded(const lh_os_loader_t *self);

/**
 * @brief Child at @p index (load order), or ::lh_null if out of range.
 */
lh_os_module_t *
lh_os_loader_get(const lh_os_loader_t *self, lh_uindex_t index);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_LOADER_H */
