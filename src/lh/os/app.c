/**
 * @file app.c
 * @brief Portable ::lh_os_app_t — top-level windows and the message pump.
 */

#include <lh/assert/runtime.h>
#include <lh/compiler/os.h>
#include <lh/list.h>
#include <lh/null.h>
#include <lh/os/app.h>
#include <lh/os/window.h>
#include <lh/util/addr.h>

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
#    include <lh/os/system/window.h>
#endif

lh_void
lh_os_app_init(lh_os_app_t *self)
{
    lh_assert_runtime_ref(self);
    lh_list_init(lh_addr_of(self->windows));
    self->quit = lh_bool_false;
}

lh_void
lh_os_app_deinit(lh_os_app_t *self)
{
    lh_os_window_t *window;

    lh_assert_runtime_ref(self);
    for (;;)
    {
        window = lh_os_app_get_first_window(self);
        if (lh_null_eq(window))
        {
            break;
        }
        lh_os_window_close(window);
    }
    self->quit = lh_bool_false;
}

lh_bool_t
lh_os_app_is_quit(const lh_os_app_t *self)
{
    lh_assert_runtime_ref(self);
    return self->quit;
}

lh_void
lh_os_app_quit(lh_os_app_t *self)
{
    lh_assert_runtime_ref(self);
    self->quit = lh_bool_true;
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    lh_os_system_window_post_quit();
#endif
}

lh_os_window_t *
lh_os_app_get_first_window(const lh_os_app_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_list_entry(lh_os_window_t, link, lh_list_get_first(lh_addr_of(self->windows)));
}

lh_os_window_t *
lh_os_app_get_next_window(const lh_os_app_t *self, const lh_os_window_t *window)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(window);
    return lh_list_entry(lh_os_window_t, link,
                         lh_list_get_next(lh_addr_of(self->windows), lh_addr_of(window->link)));
}

lh_void
lh_os_app_run(lh_os_app_t *self)
{
    lh_assert_runtime_ref(self);
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    while (!self->quit && !lh_list_is_empty(lh_addr_of(self->windows)))
    {
        if (!lh_os_system_window_pump())
        {
            self->quit = lh_bool_true;
            break;
        }
    }
#else
    (void)self;
#endif
}
