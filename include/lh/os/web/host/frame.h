/**
 * @file frame.h
 * @brief ::lh_os_web_host_frame_t — one encoded WebSocket frame waiting to be sent.
 */

#ifndef LH_OS_WEB_HOST_FRAME_H
#define LH_OS_WEB_HOST_FRAME_H

#include <lh/config.h>
#include <lh/os/net/socket.h>
#include <lh/os/web/host/client/fields.h>
#include <lh/os/web/host/frame/fields.h>
#include <lh/os/web/host/stream/fields.h>
#include <lh/core/web/ws.h>

/**
 * @struct lh_os_web_host_frame
 * @brief One encoded WebSocket frame waiting to be sent.
 */
struct lh_os_web_host_frame
{
    /** Members: ::lh_os_web_host_frame_fields. */
    lh_os_web_host_frame_fields(struct lh_os_web_host_frame *, lh_byte_ptr, lh_uint_t);
};
/** @copybrief lh_os_web_host_frame */
typedef struct lh_os_web_host_frame lh_os_web_host_frame_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The next frame in the queue (level 1).
 *
 * @param self The frame; checked by ::lh_assert_runtime_ref.
 */
struct lh_os_web_host_frame *
lh_os_web_host_frame_get_next(const lh_os_web_host_frame_t *self);

/**
 * @brief Store the next frame in the queue (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The frame; checked by ::lh_assert_runtime_ref.
 * @param next The new value.
 */
lh_void
lh_os_web_host_frame_set_next(lh_os_web_host_frame_t *self, struct lh_os_web_host_frame *next);

/**
 * @brief The bytes of the frame (level 1).
 *
 * @param self The frame; checked by ::lh_assert_runtime_ref.
 */
lh_byte_ptr
lh_os_web_host_frame_get_bytes(const lh_os_web_host_frame_t *self);

/**
 * @brief Store the bytes of the frame (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The frame; checked by ::lh_assert_runtime_ref.
 * @param bytes The new value.
 */
lh_void
lh_os_web_host_frame_set_bytes(lh_os_web_host_frame_t *self, lh_byte_ptr bytes);

/**
 * @brief How many bytes the frame has (level 1).
 *
 * @param self The frame; checked by ::lh_assert_runtime_ref.
 */
lh_uint_t
lh_os_web_host_frame_get_len(const lh_os_web_host_frame_t *self);

/**
 * @brief Store how many bytes the frame has (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The frame; checked by ::lh_assert_runtime_ref.
 * @param len The new value.
 */
lh_void
lh_os_web_host_frame_set_len(lh_os_web_host_frame_t *self, lh_uint_t len);

/**
 * @brief How many bytes of the frame are sent (level 1).
 *
 * @param self The frame; checked by ::lh_assert_runtime_ref.
 */
lh_uint_t
lh_os_web_host_frame_get_off(const lh_os_web_host_frame_t *self);

/**
 * @brief Store how many bytes of the frame are sent (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The frame; checked by ::lh_assert_runtime_ref.
 * @param off The new value.
 */
lh_void
lh_os_web_host_frame_set_off(lh_os_web_host_frame_t *self, lh_uint_t off);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_WEB_HOST_FRAME_H */
