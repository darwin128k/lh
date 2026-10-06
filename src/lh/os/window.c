/**
 * @file window.c
 * @brief Portable ::lh_os_window_t on top of `lh/os/system/window.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/compiler/os.h>
#include <lh/list.h>
#include <lh/list/node.h>
#include <lh/null.h>
#include <lh/os/app.h>
#include <lh/os/window.h>
#include <lh/runtime/error/code.h>
#include <lh/util/addr.h>

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
#    include <lh/os/system/window.h>
#endif

static lh_void
lh_os_window_unlink(lh_os_window_t *self)
{
    if (lh_list_node_is_linked(lh_addr_of(self->link)))
    {
        lh_list_node_unlink(lh_addr_of(self->link));
    }
    self->app = lh_null;
    self->parent = lh_null;
    self->modal = lh_bool_false;
}

lh_void
lh_os_window_init(lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    self->handle = LH_OS_SYSTEM_WINDOW_HANDLE_INVALID;
    self->app = lh_null;
    self->parent = lh_null;
    lh_list_init(lh_addr_of(self->children));
    lh_list_node_init(lh_addr_of(self->link));
    self->modal = lh_bool_false;
}

lh_bool_t
lh_os_window_open(lh_os_app_t *app, lh_os_window_t *self, const lh_char_t *title, int width,
                  int height)
{
    lh_assert_runtime_ref(app);
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(title);
    lh_assert_runtime_if(lh_null_ne(self->handle), lh_runtime_error_code_invalid_argument);
    lh_assert_runtime_if(lh_list_node_is_linked(lh_addr_of(self->link)),
                         lh_runtime_error_code_invalid_argument);
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    self->handle = lh_os_system_window_open(title, width, height, self,
                                            LH_OS_SYSTEM_WINDOW_HANDLE_INVALID);
    if (!lh_os_system_window_is_valid(self->handle))
    {
        return lh_bool_false;
    }
    self->app = app;
    self->parent = lh_null;
    self->modal = lh_bool_false;
    lh_list_push_back(lh_addr_of(app->windows), lh_addr_of(self->link));
    return lh_bool_true;
#else
    (void)width;
    (void)height;
    return lh_bool_false;
#endif
}

lh_bool_t
lh_os_window_open_modal(lh_os_window_t *parent, lh_os_window_t *self, const lh_char_t *title,
                        int width, int height)
{
    lh_assert_runtime_ref(parent);
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(title);
    lh_assert_runtime_if(self == parent, lh_runtime_error_code_invalid_argument);
    lh_assert_runtime_if(lh_null_ne(self->handle), lh_runtime_error_code_invalid_argument);
    lh_assert_runtime_if(lh_list_node_is_linked(lh_addr_of(self->link)),
                         lh_runtime_error_code_invalid_argument);
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    lh_assert_runtime_ifn(lh_os_system_window_is_valid(parent->handle),
                          lh_runtime_error_code_invalid_argument);
    self->handle =
        lh_os_system_window_open(title, width, height, self, parent->handle);
    if (!lh_os_system_window_is_valid(self->handle))
    {
        return lh_bool_false;
    }
    self->app = lh_null;
    self->parent = parent;
    self->modal = lh_bool_true;
    lh_list_push_back(lh_addr_of(parent->children), lh_addr_of(self->link));
    lh_os_system_window_set_enabled(parent->handle, lh_bool_false);
    return lh_bool_true;
#else
    (void)width;
    (void)height;
    return lh_bool_false;
#endif
}

lh_bool_t
lh_os_window_is_open(const lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    return lh_os_system_window_is_valid(self->handle);
#else
    return lh_bool_false;
#endif
}

lh_os_system_window_handle_t
lh_os_window_get_handle(const lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    return self->handle;
}

lh_os_app_t *
lh_os_window_get_app(const lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    return self->app;
}

lh_os_window_t *
lh_os_window_get_parent(const lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    return self->parent;
}

lh_os_window_t *
lh_os_window_get_first_child(const lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_list_entry(lh_os_window_t, link, lh_list_get_first(lh_addr_of(self->children)));
}

lh_os_window_t *
lh_os_window_get_next_child(const lh_os_window_t *self, const lh_os_window_t *child)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(child);
    return lh_list_entry(lh_os_window_t, link,
                         lh_list_get_next(lh_addr_of(self->children), lh_addr_of(child->link)));
}

lh_void
lh_os_window_close(lh_os_window_t *self)
{
    lh_os_window_t *child;

    lh_assert_runtime_ref(self);
    for (;;)
    {
        child = lh_os_window_get_first_child(self);
        if (lh_null_eq(child))
        {
            break;
        }
        lh_os_window_close(child);
    }
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    if (lh_os_system_window_is_valid(self->handle))
    {
        lh_os_system_window_close(self->handle);
    }
#endif
    if (lh_null_ne(self->handle) || lh_list_node_is_linked(lh_addr_of(self->link)))
    {
        lh_os_window_on_native_destroy(self);
    }
}

lh_void
lh_os_window_deinit(lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    lh_os_window_close(self);
}

lh_void
lh_os_window_on_native_destroy(lh_os_window_t *self)
{
    lh_os_app_t *app;
    lh_os_window_t *parent;
    lh_bool_t modal;

    lh_assert_runtime_ref(self);
    app = self->app;
    parent = self->parent;
    modal = self->modal;
    self->handle = LH_OS_SYSTEM_WINDOW_HANDLE_INVALID;
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    if (modal && lh_null_ne(parent) && lh_os_system_window_is_valid(parent->handle))
    {
        lh_os_system_window_set_enabled(parent->handle, lh_bool_true);
    }
#else
    (void)modal;
    (void)parent;
#endif
    lh_os_window_unlink(self);
    if (lh_null_ne(app) && lh_list_is_empty(lh_addr_of(app->windows)))
    {
        lh_os_app_quit(app);
    }
}
