#include <lh/os/system/win/fs/dir/state.h>
#include <lh/os/system/fs/dir.h>
#include <lh/assert.h>
#include <lh/char/slash.h>
#include <lh/os/alloc.h>
#include <lh/os/system/error/capture.h>
#include <lh/os/system/str.h>
#include <lh/os/system/win/fs/kind.h>
#include <lh/runtime/allocator.h>
#include <lh/util/wstr/ptr.h>

LH_ATTRIBUTE_STATIC
struct lh_os_system_fs_dir_state *
lh_os_system_fs_dir_state(lh_os_system_fs_dir_handle_t handle)
{
    lh_assert_runtime_ref(handle);
    return lh_ptr_cast(struct lh_os_system_fs_dir_state, handle);
}

/* FindFirstFile lists a pattern, not a directory: "<path>\*", as OS text.
   Initializes `out` either way. */
LH_ATTRIBUTE_STATIC
lh_bool_t
lh_os_system_fs_dir_pattern(lh_str_cptr path, lh_os_str_t *out)
{
    lh_str_view_t view;

    lh_str_view_init(lh_addr_of(view), path);

    lh_str_t pattern;

    lh_str_init_by_view(lh_addr_of(pattern), view);
    if (!lh_str_view_is_empty(lh_addr_of(view)) &&
        !lh_char_is_slash(lh_str_view_get_last_char(lh_addr_of(view))) &&
        !lh_char_is_backslash(lh_str_view_get_last_char(lh_addr_of(view))))
    {
        lh_str_push_back(lh_addr_of(pattern), lh_char_map_backslash);
    }
    lh_str_push_back(lh_addr_of(pattern), lh_char_map_asterisk);
    const lh_bool_t ok = lh_os_system_str_init_by_utf8(out, lh_str_get_data(lh_addr_of(pattern)));
    lh_str_deinit(lh_addr_of(pattern));
    return ok;
}

lh_os_system_fs_dir_handle_t
lh_os_system_fs_dir_open(lh_str_cptr path)
{
    lh_os_str_t pattern;

    lh_assert_runtime_ref(path);
    struct lh_os_system_fs_dir_state *const state =
        lh_os_system_fs_dir_pattern(path, lh_addr_of(pattern))
            ? lh_ptr_cast(struct lh_os_system_fs_dir_state,
                          lh_os_alloc(sizeof(lh_ptr_deref(state))))
            : lh_null;
    if (lh_null_eq(state))
    {
        lh_os_str_deinit(lh_addr_of(pattern));
        return LH_OS_SYSTEM_FS_DIR_HANDLE_INVALID;
    }
    lh_os_system_fs_dir_state_set_find(
        state, FindFirstFileW(lh_os_str_get_data(lh_addr_of(pattern)),
                              lh_os_system_fs_dir_state_get_data_mut(state)));
    lh_os_str_deinit(lh_addr_of(pattern));
    if (lh_math_eq(lh_os_system_fs_dir_state_get_find(state), LH_OS_SYSTEM_WIN_INVALID_HANDLE))
    {
        lh_os_system_error_capture();
        lh_runtime_allocator_free(state);
        return LH_OS_SYSTEM_FS_DIR_HANDLE_INVALID;
    }
    lh_str_init(lh_os_system_fs_dir_state_get_name_mut(state));
    lh_os_system_fs_dir_state_set_ready(state, lh_bool_true);
    return state;
}

lh_void
lh_os_system_fs_dir_close(lh_os_system_fs_dir_handle_t handle)
{
    struct lh_os_system_fs_dir_state *const state = lh_os_system_fs_dir_state(handle);

    (void)FindClose(lh_os_system_fs_dir_state_get_find(state));
    lh_str_deinit(lh_os_system_fs_dir_state_get_name_mut(state));
    lh_runtime_allocator_free(state);
}

lh_ssize_t
lh_os_system_fs_dir_read(lh_os_system_fs_dir_handle_t handle, lh_str_cptr *name, lh_fs_kind_t *kind)
{
    lh_assert_runtime_ref(name);
    lh_assert_runtime_ref(kind);
    struct lh_os_system_fs_dir_state *const state = lh_os_system_fs_dir_state(handle);

    if (!lh_os_system_fs_dir_state_is_ready(state))
    {
        if (!FindNextFileW(lh_os_system_fs_dir_state_get_find(state),
                           lh_os_system_fs_dir_state_get_data_mut(state)))
        {
            if (lh_math_eq(GetLastError(), LH_OS_SYSTEM_WIN_ERROR_NO_MORE_FILES))
            {
                return 0;
            }
            lh_os_system_error_capture();
            return LH_OS_RESULT_INVALID;
        }
    }
    lh_os_system_fs_dir_state_set_ready(state, lh_bool_false);
    if (!lh_os_system_str_ptr_to_utf8(lh_os_system_fs_dir_state_get_data(state)->cFileName,
                                      lh_os_system_fs_dir_state_get_name_mut(state)))
    {
        return LH_OS_RESULT_INVALID;
    }
    lh_ptr_deref(name) = lh_str_get_data(lh_os_system_fs_dir_state_get_name_mut(state));
    lh_ptr_deref(kind) = lh_os_system_fs_kind_from_attrs(
        lh_os_system_fs_dir_state_get_data(state)->dwFileAttributes,
        lh_os_system_fs_is_symlink_tag(lh_os_system_fs_dir_state_get_data(state)->dwFileAttributes,
                                       lh_os_system_fs_dir_state_get_data(state)->dwReserved0));
    return lh_cast_static(lh_ssize_t,
                          lh_str_get_size(lh_os_system_fs_dir_state_get_name_mut(state)));
}

/* A name is at most MAX_PATH - 1 UTF-16 units; one unit is at most 3 UTF-8
   bytes (a surrogate pair, 2 units, is 4 bytes). */
lh_usize_t
lh_os_system_fs_dir_name_max(void)
{
    return lh_cast_static(lh_usize_t, lh_math_mul(lh_math_sub(LH_OS_SYSTEM_WIN_MAX_PATH, 1), 3));
}

lh_bool_t
lh_os_system_fs_dir_create(lh_str_cptr path)
{
    lh_os_str_t os_path;

    lh_assert_runtime_ref(path);
    const lh_bool_t ok = lh_os_system_str_init_by_utf8(lh_addr_of(os_path), path);
    const lh_bool_t created = lh_cast_static(
        lh_bool_t, ok && lh_math_ne(CreateDirectoryW(lh_os_str_get_data(lh_addr_of(os_path)), lh_null), 0));
    lh_os_str_deinit(lh_addr_of(os_path));
    /* A failed conversion already set its own error. */
    if (ok && !created)
    {
        lh_os_system_error_capture();
    }
    return created;
}

lh_os_system_win_handle_t
lh_os_system_fs_dir_state_get_find(const struct lh_os_system_fs_dir_state *self)
{
    lh_assert_runtime_ref(self);
    return self->find;
}

lh_void
lh_os_system_fs_dir_state_set_find(struct lh_os_system_fs_dir_state *self,
                                   lh_os_system_win_handle_t find)
{
    lh_assert_runtime_ref(self);
    self->find = find;
}

const lh_os_system_win_find_data_t *
lh_os_system_fs_dir_state_get_data(const struct lh_os_system_fs_dir_state *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->data);
}

lh_os_system_win_find_data_t *
lh_os_system_fs_dir_state_get_data_mut(struct lh_os_system_fs_dir_state *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->data);
}

const lh_str_t *
lh_os_system_fs_dir_state_get_name(const struct lh_os_system_fs_dir_state *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->name);
}

lh_str_t *
lh_os_system_fs_dir_state_get_name_mut(struct lh_os_system_fs_dir_state *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->name);
}

lh_bool_t
lh_os_system_fs_dir_state_is_ready(const struct lh_os_system_fs_dir_state *self)
{
    lh_assert_runtime_ref(self);
    return self->ready;
}

lh_void
lh_os_system_fs_dir_state_set_ready(struct lh_os_system_fs_dir_state *self, lh_bool_t ready)
{
    lh_assert_runtime_ref(self);
    self->ready = ready;
}
