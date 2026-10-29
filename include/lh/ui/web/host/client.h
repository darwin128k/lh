/**
 * @file client.h
 * @brief ::lh_ui_web_host_client_t — one WebSocket client: its socket, what it sent, and what is queued for it.
 */

#ifndef LH_UI_WEB_HOST_CLIENT_H
#define LH_UI_WEB_HOST_CLIENT_H

#include <lh/config.h>
#include <lh/ui/web/host/frame.h>

/**
 * @struct lh_ui_web_host_client
 * @brief One WebSocket client: its socket, what it sent, and what is queued for it.
 */
struct lh_ui_web_host_client
{
    /** Members: ::lh_ui_web_host_client_fields. */
    lh_ui_web_host_client_fields(lh_os_net_socket_t, lh_byte_ptr, lh_uint_t,
                                 lh_ui_web_host_frame_t *, lh_tick_t, lh_bool_t);
};
/** @copybrief lh_ui_web_host_client */
typedef struct lh_ui_web_host_client lh_ui_web_host_client_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The socket of the client (level 1).
 *
 * @param self The client; checked by ::lh_assert_runtime_ref.
 */
const lh_os_net_socket_t *
lh_ui_web_host_client_get_sock(const lh_ui_web_host_client_t *self);

/**
 * @brief The socket of the client, to change in place (level 1).
 *
 * @param self The client; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_os_net_socket_t *
lh_ui_web_host_client_get_sock_mut(lh_ui_web_host_client_t *self);

/**
 * @brief Store the socket of the client (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The client; checked by ::lh_assert_runtime_ref.
 * @param sock The new value.
 */
lh_void
lh_ui_web_host_client_set_sock(lh_ui_web_host_client_t *self, lh_os_net_socket_t sock);

/**
 * @brief The bytes received and not parsed yet (level 1).
 *
 * @param self The client; checked by ::lh_assert_runtime_ref.
 */
lh_byte_ptr
lh_ui_web_host_client_get_rx(const lh_ui_web_host_client_t *self);

/**
 * @brief Store the bytes received and not parsed yet (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The client; checked by ::lh_assert_runtime_ref.
 * @param rx The new value.
 */
lh_void
lh_ui_web_host_client_set_rx(lh_ui_web_host_client_t *self, lh_byte_ptr rx);

/**
 * @brief How many bytes are received and not parsed yet (level 1).
 *
 * @param self The client; checked by ::lh_assert_runtime_ref.
 */
lh_uint_t
lh_ui_web_host_client_get_rx_len(const lh_ui_web_host_client_t *self);

/**
 * @brief Store how many bytes are received and not parsed yet (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The client; checked by ::lh_assert_runtime_ref.
 * @param rx_len The new value.
 */
lh_void
lh_ui_web_host_client_set_rx_len(lh_ui_web_host_client_t *self, lh_uint_t rx_len);

/**
 * @brief The message being reassembled (level 1).
 *
 * @param self The client; checked by ::lh_assert_runtime_ref.
 */
lh_byte_ptr
lh_ui_web_host_client_get_msg(const lh_ui_web_host_client_t *self);

/**
 * @brief Store the message being reassembled (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The client; checked by ::lh_assert_runtime_ref.
 * @param msg The new value.
 */
lh_void
lh_ui_web_host_client_set_msg(lh_ui_web_host_client_t *self, lh_byte_ptr msg);

/**
 * @brief How many bytes of the message are in (level 1).
 *
 * @param self The client; checked by ::lh_assert_runtime_ref.
 */
lh_uint_t
lh_ui_web_host_client_get_msg_len(const lh_ui_web_host_client_t *self);

/**
 * @brief Store how many bytes of the message are in (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The client; checked by ::lh_assert_runtime_ref.
 * @param msg_len The new value.
 */
lh_void
lh_ui_web_host_client_set_msg_len(lh_ui_web_host_client_t *self, lh_uint_t msg_len);

/**
 * @brief How many bytes the message buffer holds (level 1).
 *
 * @param self The client; checked by ::lh_assert_runtime_ref.
 */
lh_uint_t
lh_ui_web_host_client_get_msg_cap(const lh_ui_web_host_client_t *self);

/**
 * @brief Store how many bytes the message buffer holds (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The client; checked by ::lh_assert_runtime_ref.
 * @param msg_cap The new value.
 */
lh_void
lh_ui_web_host_client_set_msg_cap(lh_ui_web_host_client_t *self, lh_uint_t msg_cap);

/**
 * @brief The WebSocket opcode of the message (level 1).
 *
 * @param self The client; checked by ::lh_assert_runtime_ref.
 */
lh_uint_t
lh_ui_web_host_client_get_msg_op(const lh_ui_web_host_client_t *self);

/**
 * @brief Store the WebSocket opcode of the message (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The client; checked by ::lh_assert_runtime_ref.
 * @param msg_op The new value.
 */
lh_void
lh_ui_web_host_client_set_msg_op(lh_ui_web_host_client_t *self, lh_uint_t msg_op);

/**
 * @brief Whether a fragmented message is being reassembled (level 1).
 *
 * @param self The client; checked by ::lh_assert_runtime_ref.
 */
lh_bool_t
lh_ui_web_host_client_is_msg_on(const lh_ui_web_host_client_t *self);

/**
 * @brief Store whether a fragmented message is being reassembled (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The client; checked by ::lh_assert_runtime_ref.
 * @param msg_on The new value.
 */
lh_void
lh_ui_web_host_client_set_msg_on(lh_ui_web_host_client_t *self, lh_bool_t msg_on);

/**
 * @brief The first frame waiting to be sent (level 1).
 *
 * @param self The client; checked by ::lh_assert_runtime_ref.
 */
lh_ui_web_host_frame_t *
lh_ui_web_host_client_get_qhead(const lh_ui_web_host_client_t *self);

/**
 * @brief Store the first frame waiting to be sent (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The client; checked by ::lh_assert_runtime_ref.
 * @param qhead The new value.
 */
lh_void
lh_ui_web_host_client_set_qhead(lh_ui_web_host_client_t *self, lh_ui_web_host_frame_t * qhead);

/**
 * @brief The last frame waiting to be sent (level 1).
 *
 * @param self The client; checked by ::lh_assert_runtime_ref.
 */
lh_ui_web_host_frame_t *
lh_ui_web_host_client_get_qtail(const lh_ui_web_host_client_t *self);

/**
 * @brief Store the last frame waiting to be sent (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The client; checked by ::lh_assert_runtime_ref.
 * @param qtail The new value.
 */
lh_void
lh_ui_web_host_client_set_qtail(lh_ui_web_host_client_t *self, lh_ui_web_host_frame_t * qtail);

/**
 * @brief How many bytes wait to be sent (level 1).
 *
 * @param self The client; checked by ::lh_assert_runtime_ref.
 */
lh_uint_t
lh_ui_web_host_client_get_queued(const lh_ui_web_host_client_t *self);

/**
 * @brief Store how many bytes wait to be sent (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The client; checked by ::lh_assert_runtime_ref.
 * @param queued The new value.
 */
lh_void
lh_ui_web_host_client_set_queued(lh_ui_web_host_client_t *self, lh_uint_t queued);

/**
 * @brief How many frames the client has acknowledged (level 1).
 *
 * @param self The client; checked by ::lh_assert_runtime_ref.
 */
lh_uint_t
lh_ui_web_host_client_get_seen(const lh_ui_web_host_client_t *self);

/**
 * @brief Store how many frames the client has acknowledged (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The client; checked by ::lh_assert_runtime_ref.
 * @param seen The new value.
 */
lh_void
lh_ui_web_host_client_set_seen(lh_ui_web_host_client_t *self, lh_uint_t seen);

/**
 * @brief When the client last sent anything (level 1).
 *
 * @param self The client; checked by ::lh_assert_runtime_ref.
 */
lh_tick_t
lh_ui_web_host_client_get_last_rx_ms(const lh_ui_web_host_client_t *self);

/**
 * @brief Store when the client last sent anything (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The client; checked by ::lh_assert_runtime_ref.
 * @param last_rx_ms The new value.
 */
lh_void
lh_ui_web_host_client_set_last_rx_ms(lh_ui_web_host_client_t *self, lh_tick_t last_rx_ms);

/**
 * @brief When the client was last pinged (level 1).
 *
 * @param self The client; checked by ::lh_assert_runtime_ref.
 */
lh_tick_t
lh_ui_web_host_client_get_last_ping_ms(const lh_ui_web_host_client_t *self);

/**
 * @brief Store when the client was last pinged (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The client; checked by ::lh_assert_runtime_ref.
 * @param last_ping_ms The new value.
 */
lh_void
lh_ui_web_host_client_set_last_ping_ms(lh_ui_web_host_client_t *self, lh_tick_t last_ping_ms);

/**
 * @brief Whether the slot holds a client (level 1).
 *
 * @param self The client; checked by ::lh_assert_runtime_ref.
 */
lh_bool_t
lh_ui_web_host_client_is_used(const lh_ui_web_host_client_t *self);

/**
 * @brief Store whether the slot holds a client (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The client; checked by ::lh_assert_runtime_ref.
 * @param used The new value.
 */
lh_void
lh_ui_web_host_client_set_used(lh_ui_web_host_client_t *self, lh_bool_t used);

/**
 * @brief Whether the client is being closed (level 1).
 *
 * @param self The client; checked by ::lh_assert_runtime_ref.
 */
lh_bool_t
lh_ui_web_host_client_is_closing(const lh_ui_web_host_client_t *self);

/**
 * @brief Store whether the client is being closed (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The client; checked by ::lh_assert_runtime_ref.
 * @param closing The new value.
 */
lh_void
lh_ui_web_host_client_set_closing(lh_ui_web_host_client_t *self, lh_bool_t closing);

/**
 * @brief Whether the client has the whole page (level 1).
 *
 * @param self The client; checked by ::lh_assert_runtime_ref.
 */
lh_bool_t
lh_ui_web_host_client_is_synced(const lh_ui_web_host_client_t *self);

/**
 * @brief Store whether the client has the whole page (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The client; checked by ::lh_assert_runtime_ref.
 * @param synced The new value.
 */
lh_void
lh_ui_web_host_client_set_synced(lh_ui_web_host_client_t *self, lh_bool_t synced);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_WEB_HOST_CLIENT_H */
