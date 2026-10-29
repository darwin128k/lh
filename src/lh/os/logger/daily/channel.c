/**
 * @file channel.c
 * @brief ::lh_os_logger_daily_channel_t: its fields and its day file.
 */

#include <lh/os/logger/daily/channel.h>
#include <lh/assert/runtime.h>
#include <lh/runtime/allocator.h>
#include <lh/util/return.h>

const lh_list_node_t *
lh_os_logger_daily_channel_get_node(const lh_os_logger_daily_channel_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->node);
}

lh_list_node_t *
lh_os_logger_daily_channel_get_node_mut(lh_os_logger_daily_channel_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->node);
}

lh_void
lh_os_logger_daily_channel_set_node(lh_os_logger_daily_channel_t *self, lh_list_node_t node)
{
    lh_assert_runtime_ref(self);
    self->node = node;
}

lh_u32_t
lh_os_logger_daily_channel_get_id(const lh_os_logger_daily_channel_t *self)
{
    lh_assert_runtime_ref(self);
    return self->id;
}

lh_void
lh_os_logger_daily_channel_set_id(lh_os_logger_daily_channel_t *self, lh_u32_t id)
{
    lh_assert_runtime_ref(self);
    self->id = id;
}

const lh_fs_path_t *
lh_os_logger_daily_channel_get_dir(const lh_os_logger_daily_channel_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->dir);
}

lh_fs_path_t *
lh_os_logger_daily_channel_get_dir_mut(lh_os_logger_daily_channel_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->dir);
}

lh_s32_t
lh_os_logger_daily_channel_get_keep_days(const lh_os_logger_daily_channel_t *self)
{
    lh_assert_runtime_ref(self);
    return self->keep_days;
}

lh_void
lh_os_logger_daily_channel_set_keep_days(lh_os_logger_daily_channel_t *self, lh_s32_t keep_days)
{
    lh_assert_runtime_ref(self);
    self->keep_days = keep_days;
}

lh_bool_t
lh_os_logger_daily_channel_is_raw(const lh_os_logger_daily_channel_t *self)
{
    lh_assert_runtime_ref(self);
    return self->raw;
}

lh_void
lh_os_logger_daily_channel_set_raw(lh_os_logger_daily_channel_t *self, lh_bool_t raw)
{
    lh_assert_runtime_ref(self);
    self->raw = raw;
}

const lh_os_fs_file_t *
lh_os_logger_daily_channel_get_file(const lh_os_logger_daily_channel_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->file);
}

lh_os_fs_file_t *
lh_os_logger_daily_channel_get_file_mut(lh_os_logger_daily_channel_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->file);
}

lh_s64_t
lh_os_logger_daily_channel_get_day(const lh_os_logger_daily_channel_t *self)
{
    lh_assert_runtime_ref(self);
    return self->day;
}

lh_void
lh_os_logger_daily_channel_set_day(lh_os_logger_daily_channel_t *self, lh_s64_t day)
{
    lh_assert_runtime_ref(self);
    self->day = day;
}

lh_s64_t
lh_os_logger_daily_channel_get_pruned_day(const lh_os_logger_daily_channel_t *self)
{
    lh_assert_runtime_ref(self);
    return self->pruned_day;
}

lh_void
lh_os_logger_daily_channel_set_pruned_day(lh_os_logger_daily_channel_t *self, lh_s64_t pruned_day)
{
    lh_assert_runtime_ref(self);
    self->pruned_day = pruned_day;
}

lh_os_logger_daily_channel_t *
lh_os_logger_daily_channel_try_create(lh_u32_t id)
{
    lh_os_logger_daily_channel_t *self =
        lh_ptr_rcast(lh_os_logger_daily_channel_t,
                     lh_runtime_allocator_try_alloc_zero(1U, sizeof(lh_ptr_deref(self))));

    lh_return_if(lh_null_eq(self), lh_null);
    lh_list_node_init(lh_os_logger_daily_channel_get_node_mut(self));
    lh_os_logger_daily_channel_set_id(self, id);
    lh_fs_path_init(lh_os_logger_daily_channel_get_dir_mut(self));
    lh_os_fs_file_init(lh_os_logger_daily_channel_get_file_mut(self));
    lh_os_logger_daily_channel_set_day(self, LH_S64_T_MIN);
    lh_os_logger_daily_channel_set_pruned_day(self, LH_S64_T_MIN);
    return self;
}

lh_void
lh_os_logger_daily_channel_close_file(lh_os_logger_daily_channel_t *self)
{
    lh_os_fs_file_close(lh_os_logger_daily_channel_get_file_mut(self));
    lh_os_logger_daily_channel_set_day(self, LH_S64_T_MIN);
}

lh_void
lh_os_logger_daily_channel_destroy(lh_os_logger_daily_channel_t *self)
{
    lh_return_if(lh_null_eq(self));
    lh_os_fs_file_deinit(lh_os_logger_daily_channel_get_file_mut(self));
    lh_fs_path_deinit(lh_os_logger_daily_channel_get_dir_mut(self));
    lh_runtime_allocator_free(self);
}
