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
 * rejected. Children are linked through their own node (::lh_list_t) and
 * each is allocated on its own, so a pointer from ::lh_os_loader_load or a
 * walk stays valid until that child is unloaded. Loading or unloading
 * other children never moves it.
 *
 * Which paths to load — a list, a directory scan, a config file — is the
 * caller's policy, not the loader's: it takes one path at a time. The
 * pieces for building such a policy are elsewhere (::lh_os_fs_dir_t,
 * ::lh_os_system_shared_get_ext, `lh/os/system/error/kind.h`).
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
#include <lh/list.h>
#include <lh/os/loader/fields.h>
#include <lh/os/module.h>
#include <lh/size.h>
#include <lh/str.h>
#include <lh/str/ptr.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/loader.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

/**
 * @struct lh_os_loader
 * @brief Children, owner, entry name. Fields via ::lh_os_loader_fields.
 */
typedef struct lh_os_loader
{
    lh_os_loader_fields(lh_list_t, lh_os_module_t *, lh_str_t);
} lh_os_loader_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief No children yet. Each child's owner is this loader, so @p owner
 *        is each child's parent (::lh_os_module_get_parent).
 *
 * Normally reached through ::lh_os_loader_create (which ::lh_os_module_grant_loader
 * calls); call it directly only for a loader in memory of your own.
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
 * @brief A new loader on the heap: ::lh_os_loader_init'ed memory from the
 *        runtime allocator. What ::lh_os_module_grant_loader uses.
 *
 * @return The loader, or ::lh_null (::lh_os_error_code_out_of_memory in
 *         ::lh_os_last_error).
 */
lh_os_loader_t *
lh_os_loader_create(lh_os_module_t *owner, lh_str_cptr entry);

/**
 * @brief ::lh_os_loader_deinit (every child unloaded), then free the memory.
 *        Only for a loader from ::lh_os_loader_create.
 */
void
lh_os_loader_destroy(lh_os_loader_t *self);

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
 * @brief Unload one child of @p self in O(1): close it (its own children
 *        first), then free it. The other children are untouched.
 *
 * @param child A module this loader loaded (its owner is @p self).
 */
void
lh_os_loader_unload_child(lh_os_loader_t *self, lh_os_module_t *child);

/**
 * @brief Number of children currently loaded. O(n).
 */
lh_usize_t
lh_os_loader_get_loaded(const lh_os_loader_t *self);

/**
 * @brief First child in load order, or ::lh_null if there is none.
 *
 * With ::lh_os_loader_get_next:
 * @code{.c}
 * for (lh_os_module_t *m = lh_os_loader_get_first(loader); m; m = lh_os_loader_get_next(loader, m))
 * @endcode
 * To unload while walking, take the next child before unloading the current one.
 */
lh_os_module_t *
lh_os_loader_get_first(const lh_os_loader_t *self);

/**
 * @brief Child loaded after @p child, or ::lh_null if @p child is the last.
 */
lh_os_module_t *
lh_os_loader_get_next(const lh_os_loader_t *self, lh_os_module_t *child);

/**
 * @brief Child number @p index in load order (0 is the first loaded), or
 *        ::lh_null if there are not that many.
 *
 * O(@p index), see ::lh_list_get_at. To visit every child, walk with
 * ::lh_os_loader_get_first / ::lh_os_loader_get_next instead.
 */
lh_os_module_t *
lh_os_loader_get_at(const lh_os_loader_t *self, lh_uindex_t index);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_LOADER_H */
