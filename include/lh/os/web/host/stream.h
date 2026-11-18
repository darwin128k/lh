/**
 * @file stream.h
 * @brief ::lh_os_web_host_stream_t — what the stream of WS frames shares between iterations.
 */

#ifndef LH_OS_WEB_HOST_STREAM_H
#define LH_OS_WEB_HOST_STREAM_H

#include <lh/config.h>
#include <lh/os/web/host/client.h>

/**
 * @struct lh_os_web_host_stream
 * @brief What the stream of WS frames shares between iterations.
 */
struct lh_os_web_host_stream
{
    /** Members: ::lh_os_web_host_stream_fields. */
    lh_os_web_host_stream_fields(lh_str_ptr, lh_usize_t);
};
/** @copybrief lh_os_web_host_stream */
typedef struct lh_os_web_host_stream lh_os_web_host_stream_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The last message sent, kept to skip repeats (level 1).
 *
 * @param self The stream; checked by ::lh_assert_runtime_ref.
 */
lh_str_ptr
lh_os_web_host_stream_get_last(const lh_os_web_host_stream_t *self);

/**
 * @brief Store the last message sent, kept to skip repeats (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The stream; checked by ::lh_assert_runtime_ref.
 * @param last The new value.
 */
lh_void
lh_os_web_host_stream_set_last(lh_os_web_host_stream_t *self, lh_str_ptr last);

/**
 * @brief How long the last message is (level 1).
 *
 * @param self The stream; checked by ::lh_assert_runtime_ref.
 */
lh_usize_t
lh_os_web_host_stream_get_last_len(const lh_os_web_host_stream_t *self);

/**
 * @brief Store how long the last message is (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The stream; checked by ::lh_assert_runtime_ref.
 * @param last_len The new value.
 */
lh_void
lh_os_web_host_stream_set_last_len(lh_os_web_host_stream_t *self, lh_usize_t last_len);

/**
 * @brief How many bytes the last-message buffer holds (level 1).
 *
 * @param self The stream; checked by ::lh_assert_runtime_ref.
 */
lh_usize_t
lh_os_web_host_stream_get_last_cap(const lh_os_web_host_stream_t *self);

/**
 * @brief Store how many bytes the last-message buffer holds (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The stream; checked by ::lh_assert_runtime_ref.
 * @param last_cap The new value.
 */
lh_void
lh_os_web_host_stream_set_last_cap(lh_os_web_host_stream_t *self, lh_usize_t last_cap);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_WEB_HOST_STREAM_H */
