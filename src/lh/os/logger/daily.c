/**
 * @file daily.c
 * @brief ::lh_os_logger_daily_t: a sink of one file per channel and local day.
 *
 * Runs on a log queue's worker thread only: no lock, and a channel's state is
 * touched by nothing else.
 */

#include <lh/os/logger/daily.h>
#include <lh/assert/runtime.h>
#include <lh/attribute/unused.h>
#include <lh/char/map.h>
#include <lh/os.h>
#include <lh/os/logger/daily/private.h>
#include <lh/runtime/allocator.h>
#include <lh/runtime/logger.h>
#include <lh/util/return.h>

/* `YYYY-MM-DD.log`: the date, then the extension. */
#define LH_OS_LOGGER_DAILY_EXTENSION ".log"
#define LH_OS_LOGGER_DAILY_NAME_SIZE 14U
#define LH_OS_LOGGER_DAILY_DATE_SIZE 10U

struct lh_os_logger_daily
{
    lh_memory_allocator_t allocator;
    lh_list_t channels;
    lh_os_logger_daily_format_cb format;
    lh_os_logger_daily_clock_cb clock;
    lh_ptr user;
    lh_str_t line;
    lh_bool_t reported;
};

/* -- Level 1: the fields ------------------------------------------------------ */

lh_memory_allocator_t *
lh_os_logger_daily_get_allocator_mut(lh_os_logger_daily_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->allocator);
}

const lh_list_t *
lh_os_logger_daily_get_channels(const lh_os_logger_daily_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->channels);
}

lh_list_t *
lh_os_logger_daily_get_channels_mut(lh_os_logger_daily_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->channels);
}

lh_os_logger_daily_format_cb
lh_os_logger_daily_get_format_fn(const lh_os_logger_daily_t *self)
{
    lh_assert_runtime_ref(self);
    return self->format;
}

lh_void
lh_os_logger_daily_set_format_fn(lh_os_logger_daily_t *self, lh_os_logger_daily_format_cb format)
{
    lh_assert_runtime_ref(self);
    self->format = format;
}

lh_os_logger_daily_clock_cb
lh_os_logger_daily_get_clock_fn(const lh_os_logger_daily_t *self)
{
    lh_assert_runtime_ref(self);
    return self->clock;
}

lh_void
lh_os_logger_daily_set_clock_fn(lh_os_logger_daily_t *self, lh_os_logger_daily_clock_cb clock)
{
    lh_assert_runtime_ref(self);
    self->clock = clock;
}

lh_ptr
lh_os_logger_daily_get_user(const lh_os_logger_daily_t *self)
{
    lh_assert_runtime_ref(self);
    return self->user;
}

lh_void
lh_os_logger_daily_set_user(lh_os_logger_daily_t *self, lh_ptr user)
{
    lh_assert_runtime_ref(self);
    self->user = user;
}

lh_str_t *
lh_os_logger_daily_get_line_mut(lh_os_logger_daily_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->line);
}

lh_bool_t
lh_os_logger_daily_is_reported(const lh_os_logger_daily_t *self)
{
    lh_assert_runtime_ref(self);
    return self->reported;
}

lh_void
lh_os_logger_daily_set_reported(lh_os_logger_daily_t *self, lh_bool_t reported)
{
    lh_assert_runtime_ref(self);
    self->reported = reported;
}

/* -- Level 2 and up ----------------------------------------------------------- */

lh_os_logger_daily_channel_t *
lh_os_logger_daily_get_channel(lh_list_node_t *node)
{
    return lh_list_entry(lh_os_logger_daily_channel_t, node, node);
}

/* The local day of @p utc, in days since the epoch; UTC's when the zone is unknown. */
lh_s64_t
lh_os_logger_daily_get_local_day(lh_timestamp_t utc)
{
    lh_timestamp_t local;

    return lh_timestamp_get_days(lh_os_timestamp_try_get_local(utc, lh_addr_of(local)) ? local
                                                                                       : utc);
}

/* The sink's clock when none is given: the system's. */
lh_bool_t
lh_os_logger_daily_get_system_now(lh_ptr user, lh_timestamp_t *out)
{
    LH_ATTRIBUTE_UNUSED(user);
    return lh_os_timestamp_try_get_now(out);
}

/* The day of a `YYYY-MM-DD.log` name, in days since the epoch. */
lh_bool_t
lh_os_logger_daily_try_get_day(lh_str_view_t name, lh_s64_t *out)
{
    lh_str_view_t extension;
    lh_date_t date;

    lh_str_view_init(lh_addr_of(extension), LH_OS_LOGGER_DAILY_EXTENSION);
    lh_return_if(
        lh_str_view_get_size(lh_addr_of(name)) != LH_OS_LOGGER_DAILY_NAME_SIZE ||
            !lh_str_view_ends_with(lh_addr_of(name), lh_addr_of(extension), lh_bool_false) ||
            !lh_date_parse_by(lh_str_view_get_data(lh_addr_of(name)), LH_OS_LOGGER_DAILY_DATE_SIZE,
                              lh_char_map_minus, lh_addr_of(date)),
        lh_bool_false);
    lh_ptr_deref(out) = lh_date_days_since_epoch(lh_addr_of(date));
    return lh_bool_true;
}

/* @p dir joined with the file name of @p day. */
lh_bool_t
lh_os_logger_daily_get_path(const lh_fs_path_t *dir, lh_s64_t day, lh_fs_path_t *out)
{
    lh_char_t text[LH_DATE_TEXT_MAX + sizeof(LH_OS_LOGGER_DAILY_EXTENSION)];
    lh_fs_path_t name;
    lh_str_view_t view;
    lh_date_t date;
    lh_usize_t size;
    lh_bool_t joined;

    lh_date_from_epoch_days(day, lh_addr_of(date));
    size = lh_date_format_by(lh_addr_of(date), text, sizeof(text), lh_char_map_minus);
    lh_return_if(size == 0U, lh_bool_false);
    lh_ptr_deref(lh_str_ptr_copy(text + size, sizeof(text) - size - 1U,
                                 LH_OS_LOGGER_DAILY_EXTENSION,
                                 sizeof(LH_OS_LOGGER_DAILY_EXTENSION) - 1U)) = lh_char_map_nul;
    lh_fs_path_init(lh_addr_of(name));
    lh_str_view_init(lh_addr_of(view), text);
    lh_fs_path_set(lh_addr_of(name), view, lh_fs_path_style_posix);
    joined = lh_fs_path_join(out, dir, lh_addr_of(name));
    lh_fs_path_deinit(lh_addr_of(name));
    return joined;
}

lh_void
lh_os_logger_daily_append_text(lh_str_t *out, const lh_char_t *text)
{
    const lh_usize_t size = lh_null_eq(text) ? 0U : lh_str_ptr_len(text);
    const lh_char_t newline[1] = {lh_char_map_lf};

    if (size > 0U)
    {
        lh_str_append(out, text, size);
    }
    if (size == 0U || text[size - 1U] != lh_char_map_lf)
    {
        lh_str_append(out, newline, 1U);
    }
}

/* Report the first file failure; later ones would only repeat it. */
lh_void
lh_os_logger_daily_report(lh_os_logger_daily_t *self, const lh_char_t *what,
                          const lh_fs_path_t *path)
{
    const lh_char_t *desc = lh_cast_reinterpret(const lh_char_t *, lh_os_get_last_error_desc());

    lh_return_if(lh_os_logger_daily_is_reported(self));
    lh_os_logger_daily_set_reported(self, lh_bool_true);
    lh_logger_error(lh_runtime_logger(), "lh logger: %s failed, path='%s': %s", what,
                    lh_str_get_data(lh_fs_path_get_text(path)),
                    lh_null_ne(desc) ? desc : "unknown error");
}

lh_os_logger_daily_channel_t *
lh_os_logger_daily_find(const lh_os_logger_daily_t *self, lh_u32_t id)
{
    const lh_list_t *list = lh_os_logger_daily_get_channels(self);

    for (lh_list_node_t *node = lh_list_get_first(list); lh_null_ne(node);
         node = lh_list_get_next(list, node))
    {
        lh_os_logger_daily_channel_t *channel = lh_os_logger_daily_get_channel(node);

        lh_return_if(lh_os_logger_daily_channel_get_id(channel) == id, channel);
    }
    return lh_null;
}

/* -- The API ------------------------------------------------------------------ */

lh_os_logger_daily_t *
lh_os_logger_daily_try_create(lh_os_logger_daily_format_cb format,
                              lh_os_logger_daily_clock_cb clock, lh_ptr user)
{
    lh_memory_allocator_t allocator = lh_ptr_deref(lh_runtime_allocator());
    lh_os_logger_daily_t *self = lh_ptr_rcast(
        lh_os_logger_daily_t,
        lh_memory_allocator_try_alloc_zero(lh_addr_of(allocator), 1U, sizeof(lh_ptr_deref(self))));

    lh_return_if(lh_null_eq(self), lh_null);
    lh_memory_allocator_assign(lh_os_logger_daily_get_allocator_mut(self), lh_addr_of(allocator));
    lh_list_init(lh_os_logger_daily_get_channels_mut(self));
    lh_os_logger_daily_set_format_fn(self, lh_null_ne(format) ? format
                                                              : lh_os_logger_daily_format_standard);
    lh_os_logger_daily_set_clock_fn(self,
                                    lh_null_ne(clock) ? clock : lh_os_logger_daily_get_system_now);
    lh_os_logger_daily_set_user(self, user);
    lh_str_init(lh_os_logger_daily_get_line_mut(self));
    lh_os_logger_daily_set_reported(self, lh_bool_false);
    return self;
}

lh_void
lh_os_logger_daily_format_standard(lh_ptr user, const lh_os_logger_record_t *record,
                                   lh_timestamp_t local, lh_str_t *out)
{
    const lh_char_t *tag = lh_os_logger_record_get_tag(record);
    lh_str_t head;

    LH_ATTRIBUTE_UNUSED(user);
    lh_str_init(lh_addr_of(head));
    LH_ATTRIBUTE_UNUSED(
        lh_str_format(lh_addr_of(head), "%04d-%02d-%02d %02d:%02d:%02d [%s] ",
                      lh_cast_static(lh_int_t, lh_timestamp_get_year(local)),
                      lh_cast_static(lh_int_t, lh_timestamp_get_month(local)),
                      lh_cast_static(lh_int_t, lh_timestamp_get_day(local)),
                      lh_cast_static(lh_int_t, lh_timestamp_get_hour(local)),
                      lh_cast_static(lh_int_t, lh_timestamp_get_minute(local)),
                      lh_cast_static(lh_int_t, lh_timestamp_get_second(local)),
                      lh_logger_level_get_name(lh_os_logger_record_get_level(record))));
    lh_str_append_str(out, lh_addr_of(head));
    if (tag[0] != lh_char_map_nul)
    {
        LH_ATTRIBUTE_UNUSED(lh_str_format(lh_addr_of(head), "[%s] ", tag));
        lh_str_append_str(out, lh_addr_of(head));
    }
    lh_str_deinit(lh_addr_of(head));
    lh_os_logger_daily_append_text(out, lh_os_logger_record_get_text(record));
}

/* Delete the channel's daily files older than its retention allows on @p today. */
lh_void
lh_os_logger_daily_prune(lh_os_logger_daily_t *self, lh_os_logger_daily_channel_t *channel,
                         lh_s64_t today)
{
    const lh_s32_t keep = lh_os_logger_daily_channel_get_keep_days(channel);
    const lh_fs_path_t *dir = lh_os_logger_daily_channel_get_dir(channel);
    lh_os_fs_dir_entry_kind_t kind;
    lh_os_fs_dir_t listing;
    lh_str_view_t entry;
    lh_fs_path_t path;

    lh_os_logger_daily_channel_set_pruned_day(channel, today);
    lh_return_if(keep <= 0);
    lh_os_fs_dir_init(lh_addr_of(listing));
    lh_return_ifn(lh_os_fs_dir_open(lh_addr_of(listing), dir));
    lh_fs_path_init(lh_addr_of(path));
    while (lh_os_fs_dir_read(lh_addr_of(listing), lh_addr_of(entry), lh_addr_of(kind)) > 0)
    {
        lh_s64_t day;

        if (kind == lh_fs_kind_dir || !lh_os_logger_daily_try_get_day(entry, lh_addr_of(day)) ||
            day >= today - keep || day == lh_os_logger_daily_channel_get_day(channel) ||
            !lh_os_logger_daily_get_path(dir, day, lh_addr_of(path)))
        {
            continue;
        }
        if (!lh_os_fs_file_remove(lh_addr_of(path)))
        {
            lh_os_logger_daily_report(self, "removing an old file", lh_addr_of(path));
        }
    }
    lh_fs_path_deinit(lh_addr_of(path));
    lh_os_fs_dir_close(lh_addr_of(listing));
}

lh_void
lh_os_logger_daily_close_channel(lh_os_logger_daily_t *self, const lh_os_logger_record_t *record)
{
    lh_os_logger_daily_channel_t *channel =
        lh_os_logger_daily_find(self, lh_os_logger_record_get_channel(record));

    lh_return_if(lh_null_eq(channel));
    lh_list_node_unlink(lh_os_logger_daily_channel_get_node_mut(channel));
    lh_os_logger_daily_channel_destroy(channel);
}

/* The channel's file for @p day: the open one, or the next one opened. */
lh_bool_t
lh_os_logger_daily_open_day(lh_os_logger_daily_t *self, lh_os_logger_daily_channel_t *channel,
                            lh_s64_t day)
{
    const lh_fs_path_t *dir = lh_os_logger_daily_channel_get_dir(channel);
    lh_fs_path_t path;
    lh_bool_t opened;

    lh_return_if(lh_os_fs_file_is_valid(lh_os_logger_daily_channel_get_file(channel)) &&
                     lh_os_logger_daily_channel_get_day(channel) == day,
                 lh_bool_true);
    lh_os_logger_daily_channel_close_file(channel);
    if (!lh_os_fs_dir_create_tree(dir))
    {
        lh_os_logger_daily_report(self, "creating the directory", dir);
        return lh_bool_false;
    }
    if (lh_os_logger_daily_channel_get_pruned_day(channel) != day)
    {
        lh_os_logger_daily_prune(self, channel, day);
    }
    lh_fs_path_init(lh_addr_of(path));
    opened = lh_os_logger_daily_get_path(dir, day, lh_addr_of(path)) &&
             lh_os_fs_file_open(lh_os_logger_daily_channel_get_file_mut(channel), lh_addr_of(path),
                                lh_fs_file_mode_append);
    if (opened)
    {
        lh_os_logger_daily_channel_set_day(channel, day);
    }
    else
    {
        lh_os_logger_daily_report(self, "opening the file", lh_addr_of(path));
    }
    lh_fs_path_deinit(lh_addr_of(path));
    return opened;
}

lh_void
lh_os_logger_daily_open(lh_os_logger_daily_t *self, const lh_os_logger_record_t *record)
{
    lh_os_logger_daily_channel_t *channel =
        lh_os_logger_daily_find(self, lh_os_logger_record_get_channel(record));
    lh_str_view_t dir;

    if (lh_null_eq(channel))
    {
        channel = lh_os_logger_daily_channel_try_create(lh_os_logger_daily_get_allocator_mut(self),
                                                        lh_os_logger_record_get_channel(record));
        lh_return_if(lh_null_eq(channel));
        lh_list_push_back(lh_os_logger_daily_get_channels_mut(self),
                          lh_os_logger_daily_channel_get_node_mut(channel));
    }
    lh_os_logger_daily_channel_close_file(channel);
    lh_str_view_init(lh_addr_of(dir), lh_os_logger_record_get_text(record));
    lh_fs_path_set(lh_os_logger_daily_channel_get_dir_mut(channel), dir, lh_fs_path_style_posix);
    lh_os_logger_daily_channel_set_keep_days(channel, lh_os_logger_record_get_keep_days(record));
    lh_os_logger_daily_channel_set_raw(channel, lh_os_logger_record_is_raw(record));
    lh_os_logger_daily_channel_set_pruned_day(channel, LH_S64_T_MIN);
    /* Retention runs now, not at the first line: a quiet channel is cleaned too. */
    if (lh_os_fs_dir_create_tree(lh_os_logger_daily_channel_get_dir(channel)))
    {
        lh_os_logger_daily_prune(
            self, channel, lh_os_logger_daily_get_local_day(lh_os_logger_record_get_time(record)));
    }
}

lh_void
lh_os_logger_daily_write(lh_os_logger_daily_t *self, const lh_os_logger_record_t *record)
{
    lh_os_logger_daily_channel_t *channel =
        lh_os_logger_daily_find(self, lh_os_logger_record_get_channel(record));
    lh_str_t *line = lh_os_logger_daily_get_line_mut(self);
    lh_timestamp_t local;

    lh_return_if(lh_null_eq(channel));
    if (!lh_os_timestamp_try_get_local(lh_os_logger_record_get_time(record), lh_addr_of(local)))
    {
        local = lh_os_logger_record_get_time(record);
    }
    lh_return_ifn(lh_os_logger_daily_open_day(self, channel, lh_timestamp_get_days(local)));
    lh_str_clear(line);
    if (lh_os_logger_daily_channel_is_raw(channel))
    {
        lh_os_logger_daily_append_text(line, lh_os_logger_record_get_text(record));
    }
    else
    {
        lh_os_logger_daily_get_format_fn(self)(lh_os_logger_daily_get_user(self), record, local,
                                               line);
    }
    if (!lh_os_fs_file_write_all(lh_os_logger_daily_channel_get_file_mut(channel),
                                 lh_str_get_data(line), lh_str_get_size(line)))
    {
        lh_os_logger_daily_report(self, "writing", lh_os_logger_daily_channel_get_dir(channel));
    }
}

/* -- The sink callbacks ------------------------------------------------------- */

lh_void
lh_os_logger_daily_emit(lh_ptr user, const lh_os_logger_record_t *record)
{
    lh_os_logger_daily_t *self = lh_ptr_rcast(lh_os_logger_daily_t, user);

    switch (lh_os_logger_record_get_kind(record))
    {
    case lh_os_logger_record_kind_open:
        lh_os_logger_daily_open(self, record);
        break;
    case lh_os_logger_record_kind_close:
        lh_os_logger_daily_close_channel(self, record);
        break;
    default:
        lh_os_logger_daily_write(self, record);
        break;
    }
}

/* Lines are written straight to the files: there is no buffer of ours to flush.
   A flush is when a quiet channel notices a new day and cleans up. */
lh_void
lh_os_logger_daily_flush(lh_ptr user)
{
    lh_os_logger_daily_t *self = lh_ptr_rcast(lh_os_logger_daily_t, user);
    const lh_list_t *list = lh_os_logger_daily_get_channels(self);
    lh_timestamp_t now;
    lh_s64_t today;

    lh_return_ifn(
        lh_os_logger_daily_get_clock_fn(self)(lh_os_logger_daily_get_user(self), lh_addr_of(now)));
    today = lh_os_logger_daily_get_local_day(now);
    for (lh_list_node_t *node = lh_list_get_first(list); lh_null_ne(node);
         node = lh_list_get_next(list, node))
    {
        lh_os_logger_daily_channel_t *channel = lh_os_logger_daily_get_channel(node);

        if (lh_os_logger_daily_channel_get_pruned_day(channel) != today)
        {
            lh_os_logger_daily_prune(self, channel, today);
        }
    }
}

lh_void
lh_os_logger_daily_close(lh_ptr user)
{
    lh_os_logger_daily_t *self = lh_ptr_rcast(lh_os_logger_daily_t, user);
    lh_list_t *list = lh_os_logger_daily_get_channels_mut(self);

    while (!lh_list_is_empty(list))
    {
        lh_list_node_t *node = lh_list_pop_front(list);

        lh_os_logger_daily_channel_destroy(lh_os_logger_daily_get_channel(node));
    }
}

lh_void
lh_os_logger_daily_copy_sink(lh_os_logger_daily_t *self, lh_os_logger_sink_t *out)
{
    lh_assert_runtime_ref(self);
    lh_os_logger_sink_set_emit_fn(out, lh_os_logger_daily_emit);
    lh_os_logger_sink_set_flush_fn(out, lh_os_logger_daily_flush);
    lh_os_logger_sink_set_close_fn(out, lh_os_logger_daily_close);
    lh_os_logger_sink_set_dropped_fn(out, lh_null);
    lh_os_logger_sink_set_user(out, self);
}

lh_void
lh_os_logger_daily_destroy(lh_os_logger_daily_t *self)
{
    lh_return_if(lh_null_eq(self));
    lh_os_logger_daily_close(self);
    lh_str_deinit(lh_os_logger_daily_get_line_mut(self));
    lh_memory_allocator_dealloc(lh_os_logger_daily_get_allocator_mut(self), self);
}
