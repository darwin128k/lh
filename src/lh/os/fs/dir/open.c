#include <lh/os/fs/dir.h>
#include "state.h"
#include <lh/assert.h>
#include <lh/null.h>
#include <lh/os.h>
#include <lh/runtime/allocator.h>
#include <lh/str.h>
#include <lh/str/view.h>
#include <lh/str/view/initializer.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

static struct lh_os_fs_dir_state *
lh_os_fs_dir_alloc_state(void)
{
    struct lh_os_fs_dir_state *state;

    state = lh_ptr_cast(struct lh_os_fs_dir_state,
                        lh_runtime_allocator_alloc(sizeof(*state)));
    if (lh_null_eq(state))
    {
        lh_os_set_last_error(lh_os_error_make(lh_os_error_code_out_of_memory,
                             lh_os_error_desc_lit("out of memory")));
    }
    return state;
}

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
static lh_bool_t
lh_os_fs_dir_star_pattern(const lh_os_fs_path_t *path, lh_os_fs_path_t *out)
{
    lh_os_fs_path_t star;
    lh_bool_t ok;

    lh_os_fs_path_init(out);
    lh_os_fs_path_init(lh_addr_of(star));
    ok = lh_os_fs_path_set(lh_addr_of(star), lh_str_view_lit("*")) &&
         lh_os_fs_path_join(out, path, lh_addr_of(star));
    lh_os_fs_path_deinit(lh_addr_of(star));
    if (!ok)
    {
        lh_os_fs_path_deinit(out);
    }
    return ok;
}

static lh_bool_t
lh_os_fs_dir_open_native(struct lh_os_fs_dir_state *state, const lh_os_fs_path_t *path)
{
    lh_os_fs_path_t pattern;

    if (!lh_os_fs_dir_star_pattern(path, lh_addr_of(pattern)))
    {
        return lh_bool_false;
    }
    state->find = FindFirstFileA(lh_str_get_data(lh_os_fs_path_get_text_as_const(lh_addr_of(pattern))),
                                 lh_addr_of(state->data));
    lh_os_fs_path_deinit(lh_addr_of(pattern));
    if (state->find == INVALID_HANDLE_VALUE)
    {
        lh_os_system_error_capture();
        return lh_bool_false;
    }
    state->ready = lh_bool_true;
    return lh_bool_true;
}
#else
static lh_bool_t
lh_os_fs_dir_open_native(struct lh_os_fs_dir_state *state, const lh_os_fs_path_t *path)
{
    state->dir = opendir(lh_str_get_data(lh_os_fs_path_get_text_as_const(path)));
    if (lh_null_eq(state->dir))
    {
        lh_os_system_error_capture();
        return lh_bool_false;
    }
    state->ready = lh_bool_false;
    return lh_bool_true;
}
#endif

lh_bool_t
lh_os_fs_dir_open(lh_os_fs_dir_t *self, const lh_os_fs_path_t *path)
{
    struct lh_os_fs_dir_state *state;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(path);
    if (lh_os_fs_path_is_empty(path))
    {
        lh_os_set_last_error(lh_os_error_make(lh_os_error_code_path_empty,
                             lh_os_error_desc_lit("path is empty")));
        return lh_bool_false;
    }
    lh_os_fs_dir_close(self);
    state = lh_os_fs_dir_alloc_state();
    if (lh_null_eq(state))
    {
        return lh_bool_false;
    }
    if (!lh_os_fs_dir_open_native(state, path))
    {
        lh_runtime_allocator_free(state);
        return lh_bool_false;
    }
    lh_os_fs_dir_put_state(self, state);
    lh_os_fs_path_assign(lh_os_fs_dir_get_path(self), path);
    return lh_bool_true;
}
