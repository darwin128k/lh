/**
 * @file module.h
 * @brief One module (::lh_os_module_t): an image, its own methods, its state.
 *
 * The module is the main object. It carries its lifecycle
 * (::lh_os_module_ops_t: start / stop), its private state (`data`), and
 * who holds it (`owner`: the loader that loaded it, null for a root).
 * Loading other modules is a
 * privilege a module may be granted (::lh_os_module_grant_loader), not
 * something every module is.
 *
 * Two layers, always torn down in this order:
 *  - lifecycle: ::lh_os_module_start / ::lh_os_module_stop (children
 *    first, then the module's own stop);
 *  - image: ::lh_os_module_open or ::lh_os_module_bind / ::lh_os_module_close.
 * ::lh_os_module_close stops first: the methods live inside the image and
 * must never run after it is unmapped.
 *
 * The image is an ::lh_os_shared_t (::lh_os_module_get_image_as_const):
 * opening it, holding its one OS reference and giving that back is its job.
 * The image functions here (open, bind, get_sym, …) hand over to it and add
 * only what the lifecycle needs — close stops first.
 *
 * On failure the reason is in ::lh_os_last_error (our own checks, e.g. an
 * empty path or an already-open slot) or ::lh_os_system_last_error (the
 * native call failed) — see `lh/os.h` / `lh/os/system.h`.
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_MODULE_H
#define LH_OS_MODULE_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/fs/path.h>
#include <lh/list/node.h>
#include <lh/os/module/fields.h>
#include <lh/os/module/ops.h>
#include <lh/os/shared.h>
#include <lh/os/system/shared/handle.h>
#include <lh/ptr.h>
#include <lh/str/ptr.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/module.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

struct lh_os_loader;

/**
 * @struct lh_os_module
 * @brief Image, lifecycle, state, place in the tree. Fields via ::lh_os_module_fields.
 */
struct lh_os_module
{
    lh_os_module_fields(lh_os_shared_t, lh_bool_t, const lh_os_module_ops_t *, lh_ptr,
                        struct lh_os_loader *, lh_list_node_t);
};
typedef struct lh_os_module lh_os_module_t;

LH_COMPILER_EXTERN_C_BEGIN

/* ── init / deinit ───────────────────────────────────────────────────────── */

/**
 * @brief Empty module: no image, no methods, no owner, no loader. Does not
 *        touch the OS.
 */
void
lh_os_module_init(lh_os_module_t *self);

/**
 * @brief ::lh_os_module_close, then release the stored path.
 *
 * Only for a module nobody holds: a root, or one the caller initialized
 * itself. A child a loader loaded is the loader's to free
 * (::lh_os_loader_unload_child / ::lh_os_loader_unload); deinitializing it
 * directly is a checked error.
 */
void
lh_os_module_deinit(lh_os_module_t *self);

/**
 * @brief A new empty module on the heap: ::lh_os_module_init'ed memory from
 *        the runtime allocator.
 *
 * For modules that must not move and outlive a scope — a loader's
 * children. Release with ::lh_os_module_destroy, never plain `free`.
 *
 * @return The module, or ::lh_null (::lh_os_error_code_out_of_memory in
 *         ::lh_os_last_error).
 */
lh_os_module_t *
lh_os_module_create(void);

/**
 * @brief ::lh_os_module_deinit, then free the memory. Only for a module from
 *        ::lh_os_module_create; same "nobody holds it" rule as deinit.
 */
void
lh_os_module_destroy(lh_os_module_t *self);

/* ── accessors ───────────────────────────────────────────────────────────── */

/**
 * @brief The module's image. Read-only: the module opens and closes it,
 *        so the image never changes under a started module.
 */
const lh_os_shared_t *
lh_os_module_get_image_as_const(const lh_os_module_t *self);

/**
 * @brief Path of the module's image (::lh_os_shared_get_path_as_const).
 */
const lh_fs_path_t *
lh_os_module_get_path_as_const(const lh_os_module_t *self);

/**
 * @brief Raw handle, or ::LH_OS_SYSTEM_SHARED_HANDLE_INVALID when no image is open.
 */
lh_os_system_shared_handle_t
lh_os_module_get_handle(const lh_os_module_t *self);

/**
 * @brief True when @p self holds an image.
 */
lh_bool_t
lh_os_module_is_loaded(const lh_os_module_t *self);

/**
 * @brief True between a successful ::lh_os_module_start and ::lh_os_module_stop.
 */
lh_bool_t
lh_os_module_is_started(const lh_os_module_t *self);

/**
 * @brief The module's own methods, or ::lh_null if it has none.
 */
const lh_os_module_ops_t *
lh_os_module_get_ops(const lh_os_module_t *self);

/**
 * @brief Give @p self its methods. Only while it is not started.
 *
 * A loader does this from the image; the program does it for its own root
 * module. The table must outlive the module (normally it is static data
 * inside the image).
 */
void
lh_os_module_set_ops(lh_os_module_t *self, const lh_os_module_ops_t *ops);

/**
 * @brief The module's private state, as its start left it.
 */
lh_ptr
lh_os_module_get_data(const lh_os_module_t *self);

/**
 * @brief Keep @p data as the module's private state. The module owns it.
 */
void
lh_os_module_set_data(lh_os_module_t *self, lh_ptr data);

/**
 * @brief The loader holding @p self, or ::lh_null for a root.
 *
 * The owner loaded the module and is the one that unloads and frees it.
 */
struct lh_os_loader *
lh_os_module_get_owner(const lh_os_module_t *self);

/**
 * @brief Record which loader holds @p self. Set by ::lh_os_loader_load.
 */
void
lh_os_module_set_owner(lh_os_module_t *self, struct lh_os_loader *owner);

/**
 * @brief The link of @p self among its owner's children.
 *
 * For ::lh_os_loader_t, which strings its children through it; a module
 * never touches its own link.
 */
lh_list_node_t *
lh_os_module_get_node(lh_os_module_t *self);

/**
 * @brief The module whose link is @p node, or ::lh_null if @p node is
 *        ::lh_null. The reverse of ::lh_os_module_get_node.
 *
 * For code that keeps modules in its own ::lh_list_t — a loader of its
 * own, say — and walks them:
 * @code{.c}
 * for (lh_list_node_t *n = lh_list_get_first(&mine); n; n = lh_list_get_next(&mine, n))
 * {
 *     lh_os_module_t *m = lh_os_module_get_by_node(n);
 * }
 * @endcode
 */
lh_os_module_t *
lh_os_module_get_by_node(lh_list_node_t *node);

/**
 * @brief The module whose loader holds @p self, or ::lh_null for a root.
 *
 * Not stored: it is the owner's own owner (::lh_os_loader_get_owner).
 */
lh_os_module_t *
lh_os_module_get_parent(const lh_os_module_t *self);

/**
 * @brief The loader privilege of @p self, or ::lh_null for a leaf.
 */
struct lh_os_loader *
lh_os_module_get_loader(const lh_os_module_t *self);

/* ── privilege ───────────────────────────────────────────────────────────── */

/**
 * @brief Give @p self a loader for children.
 *
 * Children are images that export an ::lh_os_module_ops_t named @p entry.
 * The module owns the loader: ::lh_os_module_stop unloads every child and
 * releases it, before the module's own stop runs.
 *
 * @return The loader, or ::lh_null (already granted, or out of memory).
 */
struct lh_os_loader *
lh_os_module_grant_loader(lh_os_module_t *self, lh_str_cptr entry);

/* ── image ───────────────────────────────────────────────────────────────── */

/**
 * @brief Open @p path into an empty module (::lh_os_shared_open).
 *
 * A module that already holds an image fails with
 * ::lh_os_error_code_already_open. Empty @p path is an error.
 */
lh_bool_t
lh_os_module_open(lh_os_module_t *self, const lh_fs_path_t *path);

/**
 * @brief Bind the already-loaded image that contains @p addr.
 *
 * A module that already holds an image fails with
 * ::lh_os_error_code_already_open. The stored path is filled from the OS.
 */
lh_bool_t
lh_os_module_bind(lh_os_module_t *self, lh_ptr addr);

/**
 * @brief Bind the program itself (the executable) as a root module.
 *
 * Same as ::lh_os_module_bind, without needing an address inside it.
 */
lh_bool_t
lh_os_module_bind_executable(lh_os_module_t *self);

/**
 * @brief ::lh_os_module_stop, then drop the image. The stored path is kept.
 *
 * The method table is dropped too (it normally lives in the image); a root
 * module that is reopened gets it again with ::lh_os_module_set_ops.
 *
 * The image gives back its one OS reference (::lh_os_shared_close). Safe
 * on an already-closed module.
 *
 * @return ::lh_bool_false if the OS close failed. The handle is dropped
 *         either way.
 */
lh_bool_t
lh_os_module_close(lh_os_module_t *self);

/* ── lifecycle ───────────────────────────────────────────────────────────── */

/**
 * @brief Run the module's start. No methods, or no start: started as is.
 *
 * Starting twice is a no-op.
 *
 * @return What start returned.
 */
lh_bool_t
lh_os_module_start(lh_os_module_t *self);

/**
 * @brief Unload the children (if any), then run the module's stop.
 *
 * Safe on a module that is not started.
 */
void
lh_os_module_stop(lh_os_module_t *self);

/* ── symbols ─────────────────────────────────────────────────────────────── */

/**
 * @brief Exported symbol @p name, or ::lh_null if no image / missing.
 *
 * No image: ::lh_os_error_code_not_open. Missing: the native slot.
 */
lh_ptr
lh_os_module_get_sym(const lh_os_module_t *self, lh_str_cptr name);

/**
 * @brief True if an image is open and @p name is exported.
 */
lh_bool_t
lh_os_module_has_sym(const lh_os_module_t *self, lh_str_cptr name);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_MODULE_H */
