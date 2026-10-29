/**
 * @file record.c
 * @brief ::lh_os_logger_record_t: its fields and how a record is filled.
 */

#include <lh/os/logger/record.h>
#include <lh/assert/runtime.h>
#include <lh/char/map.h>
#include <lh/util/str/ptr.h>

lh_u32_t
lh_os_logger_record_get_channel(const lh_os_logger_record_t *self)
{
    lh_assert_runtime_ref(self);
    return self->channel;
}

lh_void
lh_os_logger_record_set_channel(lh_os_logger_record_t *self, lh_u32_t channel)
{
    lh_assert_runtime_ref(self);
    self->channel = channel;
}

lh_u8_t
lh_os_logger_record_get_kind(const lh_os_logger_record_t *self)
{
    lh_assert_runtime_ref(self);
    return self->kind;
}

lh_void
lh_os_logger_record_set_kind(lh_os_logger_record_t *self, lh_u8_t kind)
{
    lh_assert_runtime_ref(self);
    self->kind = kind;
}

lh_logger_level_t
lh_os_logger_record_get_level(const lh_os_logger_record_t *self)
{
    lh_assert_runtime_ref(self);
    return self->level;
}

lh_void
lh_os_logger_record_set_level(lh_os_logger_record_t *self, lh_logger_level_t level)
{
    lh_assert_runtime_ref(self);
    self->level = level;
}

lh_bool_t
lh_os_logger_record_is_raw(const lh_os_logger_record_t *self)
{
    lh_assert_runtime_ref(self);
    return self->raw;
}

lh_void
lh_os_logger_record_set_raw(lh_os_logger_record_t *self, lh_bool_t raw)
{
    lh_assert_runtime_ref(self);
    self->raw = raw;
}

lh_s32_t
lh_os_logger_record_get_keep_days(const lh_os_logger_record_t *self)
{
    lh_assert_runtime_ref(self);
    return self->keep_days;
}

lh_void
lh_os_logger_record_set_keep_days(lh_os_logger_record_t *self, lh_s32_t keep_days)
{
    lh_assert_runtime_ref(self);
    self->keep_days = keep_days;
}

lh_timestamp_t
lh_os_logger_record_get_time(const lh_os_logger_record_t *self)
{
    lh_assert_runtime_ref(self);
    return self->time;
}

lh_void
lh_os_logger_record_set_time(lh_os_logger_record_t *self, lh_timestamp_t time)
{
    lh_assert_runtime_ref(self);
    self->time = time;
}

const lh_char_t *
lh_os_logger_record_get_tag(const lh_os_logger_record_t *self)
{
    lh_assert_runtime_ref(self);
    return self->tag;
}

lh_char_t *
lh_os_logger_record_get_tag_mut(lh_os_logger_record_t *self)
{
    lh_assert_runtime_ref(self);
    return self->tag;
}

const lh_char_t *
lh_os_logger_record_get_text(const lh_os_logger_record_t *self)
{
    lh_assert_runtime_ref(self);
    return self->text;
}

lh_char_t *
lh_os_logger_record_get_text_mut(lh_os_logger_record_t *self)
{
    lh_assert_runtime_ref(self);
    return self->text;
}

lh_void
lh_os_logger_record_assign_tag(lh_os_logger_record_t *self, const lh_char_t *text)
{
    const lh_usize_t size = lh_null_eq(text) ? 0U : lh_str_ptr_len(text);

    lh_ptr_deref(lh_str_ptr_copy(lh_os_logger_record_get_tag_mut(self),
                                 LH_OS_LOGGER_RECORD_TAG_SIZE - 1U, text, size)) = lh_char_map_nul;
}

lh_void
lh_os_logger_record_assign_text(lh_os_logger_record_t *self, const lh_char_t *text)
{
    const lh_usize_t size = lh_null_eq(text) ? 0U : lh_str_ptr_len(text);

    lh_ptr_deref(lh_str_ptr_copy(lh_os_logger_record_get_text_mut(self),
                                 LH_OS_LOGGER_RECORD_TEXT_SIZE - 1U, text, size)) = lh_char_map_nul;
}

lh_void
lh_os_logger_record_init_line(lh_os_logger_record_t *self, lh_u32_t channel,
                              lh_logger_level_t level, lh_timestamp_t time, const lh_char_t *tag,
                              const lh_char_t *text)
{
    lh_os_logger_record_set_channel(self, channel);
    lh_os_logger_record_set_kind(self, lh_os_logger_record_kind_line);
    lh_os_logger_record_set_level(self, level);
    lh_os_logger_record_set_raw(self, lh_bool_false);
    lh_os_logger_record_set_keep_days(self, 0);
    lh_os_logger_record_set_time(self, time);
    lh_os_logger_record_assign_tag(self, tag);
    lh_os_logger_record_assign_text(self, text);
}

lh_void
lh_os_logger_record_init_open(lh_os_logger_record_t *self, lh_u32_t channel, const lh_char_t *dir,
                              lh_s32_t keep_days, lh_bool_t raw)
{
    lh_os_logger_record_set_channel(self, channel);
    lh_os_logger_record_set_kind(self, lh_os_logger_record_kind_open);
    lh_os_logger_record_set_level(self, lh_logger_level_debug);
    lh_os_logger_record_set_raw(self, raw);
    lh_os_logger_record_set_keep_days(self, keep_days);
    lh_os_logger_record_set_time(self, 0);
    lh_os_logger_record_assign_tag(self, lh_null);
    lh_os_logger_record_assign_text(self, dir);
}

lh_void
lh_os_logger_record_init_close(lh_os_logger_record_t *self, lh_u32_t channel)
{
    lh_os_logger_record_init_open(self, channel, lh_null, 0, lh_bool_false);
    lh_os_logger_record_set_kind(self, lh_os_logger_record_kind_close);
}
