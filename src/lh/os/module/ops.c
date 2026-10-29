/**
 * @file ops.c
 * @brief Level-1 accessors of ::lh_os_module_ops_t.
 */

#include <lh/os/module/ops.h>

#include <lh/assert/runtime.h>

lh_os_module_start_cb
lh_os_module_ops_get_start_fn(const lh_os_module_ops_t *self)
{
    lh_assert_runtime_ref(self);
    return self->start;
}

lh_void
lh_os_module_ops_set_start_fn(lh_os_module_ops_t *self, lh_os_module_start_cb start)
{
    lh_assert_runtime_ref(self);
    self->start = start;
}

lh_os_module_stop_cb
lh_os_module_ops_get_stop_fn(const lh_os_module_ops_t *self)
{
    lh_assert_runtime_ref(self);
    return self->stop;
}

lh_void
lh_os_module_ops_set_stop_fn(lh_os_module_ops_t *self, lh_os_module_stop_cb stop)
{
    lh_assert_runtime_ref(self);
    self->stop = stop;
}
