/**
 * @file module.h
 * @brief Loaded shared image that may own child images.
 *
 * Handle + child table. Identity (name, ABI, …) stays in the caller.
 * A derived struct starts with these fields; ::lh_os_shared_module_init
 * takes that derived `sizeof` so the table stores the larger type.
 *
 * ::lh_os_shared_module_open owns the handle (::lh_os_shared_close on
 * unload). ::lh_os_shared_module_bind uses ::lh_os_shared_of_addr:
 * Windows does not own a `FreeLibrary` ref, POSIX does (`dlclose`).
 *
 * Requires ::LH_LIBRARY_OPTION_OS. Failure: ::lh_os_get_last_error.
 */

#ifndef LH_OS_SHARED_MODULE_H
#define LH_OS_SHARED_MODULE_H

#include <lh/attribute/symbol.h>
#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/index.h>
#include <lh/os/shared.h>
#include <lh/os/shared/module/fields.h>
#include <lh/ptr.h>
#include <lh/size.h>
#include <lh/str/ptr.h>
#include <lh/vector.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/shared/module.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

/**
 * @struct lh_os_shared_module
 * @brief One shared image and its child table.
 *
 * Fields are injected via ::lh_os_shared_module_fields.
 */
struct lh_os_shared_module
{
    lh_os_shared_module_fields(lh_os_shared_handle_t, lh_bool_t, lh_vector_t);
};

/**
 * @typedef lh_os_shared_module_t
 * @brief Alias for `struct lh_os_shared_module`.
 */
typedef struct lh_os_shared_module lh_os_shared_module_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Child table of @p self, after validating the module pointer.
 *
 * Single access to `modules`. Other functions go through this or
 * ::lh_os_shared_module_get_modules_as_const instead of `self->modules`.
 */
LH_ATTRIBUTE_SYMBOL
lh_vector_t *
lh_os_shared_module_get_modules(lh_os_shared_module_t *self);

/**
 * @brief `const` counterpart to ::lh_os_shared_module_get_modules.
 */
LH_ATTRIBUTE_SYMBOL
const lh_vector_t *
lh_os_shared_module_get_modules_as_const(const lh_os_shared_module_t *self);

/**
 * @brief OS handle of @p self, after validating the module pointer.
 *
 * Single access to `handle`. Other functions go through this or
 * ::lh_os_shared_module_get_handle_as_const instead of `self->handle`.
 */
LH_ATTRIBUTE_SYMBOL
lh_os_shared_handle_t *
lh_os_shared_module_get_handle(lh_os_shared_module_t *self);

/**
 * @brief `const` counterpart to ::lh_os_shared_module_get_handle.
 */
LH_ATTRIBUTE_SYMBOL
const lh_os_shared_handle_t *
lh_os_shared_module_get_handle_as_const(const lh_os_shared_module_t *self);

/**
 * @brief Close-ownership flag of @p self, after validating the module pointer.
 *
 * Single access to `owned`. Other functions go through this or
 * ::lh_os_shared_module_get_owned_as_const instead of `self->owned`.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t *
lh_os_shared_module_get_owned(lh_os_shared_module_t *self);

/**
 * @brief `const` counterpart to ::lh_os_shared_module_get_owned.
 */
LH_ATTRIBUTE_SYMBOL
const lh_bool_t *
lh_os_shared_module_get_owned_as_const(const lh_os_shared_module_t *self);

/**
 * @brief Filesystem path of this loaded image, filled by the OS from the handle.
 *
 * ::lh_os_shared_get_path_of. Unloaded slots fail with last-error “not loaded”.
 *
 * @param self Loaded module.
 * @param out  Destination path, filled by the OS from the handle.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_shared_module_get_path(const lh_os_shared_module_t *self, lh_os_fs_path_t *out);

/**
 * @brief Empty image: no handle, empty child table of elements sized @p type_size.
 *
 * @p type_size is `sizeof` of the derived module (at least
 * `sizeof(lh_os_shared_module_t)`).
 *
 * @param self      Module to initialize.
 * @param type_size Size of one child (and of @p self if it is derived).
 */
LH_ATTRIBUTE_SYMBOL
void
lh_os_shared_module_init(lh_os_shared_module_t *self, lh_usize_t type_size);

/**
 * @brief Unload children and this handle, then release the child table.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_os_shared_module_deinit(lh_os_shared_module_t *self);

/**
 * @brief True if @p self currently holds an OS handle.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_shared_module_is_loaded(const lh_os_shared_module_t *self);

/**
 * @brief Open @p path into an unloaded slot. Owns the handle.
 *
 * @param self Unloaded module.
 * @param path Filesystem path for ::lh_os_shared_open.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_shared_module_open(lh_os_shared_module_t *self, const lh_os_fs_path_t *path);

/**
 * @brief Bind the already-loaded image that contains @p addr.
 *
 * @param self Unloaded module.
 * @param addr Address inside that image.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_shared_module_bind(lh_os_shared_module_t *self, lh_ptr addr);

/**
 * @brief Unload every child. @p self stays loaded.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_os_shared_module_unload_all(lh_os_shared_module_t *self);

/**
 * @brief Close this handle if owned. Children are not touched.
 *
 * Unloaded slots succeed. Bind-on-Windows does not `FreeLibrary`.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_shared_module_close(lh_os_shared_module_t *self);

/**
 * @brief Unload children, then close this handle.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_shared_module_unload(lh_os_shared_module_t *self);

/**
 * @brief Copy @p child into the table (vector element size bytes).
 *
 * @p child must be the same type the table was initialized with.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_os_shared_module_push(lh_os_shared_module_t *self, const lh_os_shared_module_t *child);

/**
 * @brief Number of children (not counting @p self).
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_os_shared_module_get_loaded(const lh_os_shared_module_t *self);

/**
 * @brief Child at @p index, or ::lh_null if out of range.
 *
 * The pointer is the derived object; only the shared prefix is typed here.
 */
LH_ATTRIBUTE_SYMBOL
lh_os_shared_module_t *
lh_os_shared_module_get(const lh_os_shared_module_t *self, lh_uindex_t index);

/**
 * @brief Exported symbol @p name, or ::lh_null if unloaded / missing.
 */
LH_ATTRIBUTE_SYMBOL
lh_ptr
lh_os_shared_module_get_sym(const lh_os_shared_module_t *self, lh_str_cptr name);

/**
 * @brief True if loaded and @p name is exported.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_shared_module_has_sym(const lh_os_shared_module_t *self, lh_str_cptr name);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_SHARED_MODULE_H */
