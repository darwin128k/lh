/**
 * @file phase.h
 * @brief Where one HTTP connection of the web host stands.
 */

#ifndef LH_UI_WEB_HOST_CONN_PHASE_H
#define LH_UI_WEB_HOST_CONN_PHASE_H

#include <lh/core/numeric/fixed/types.h>

/**
 * @typedef lh_ui_web_host_conn_phase_t
 * @brief Phase of an ::lh_ui_web_host_conn_t.
 *
 * Alias for ::lh_u8_t; canonical values are the `lh_ui_web_host_conn_phase_*`
 * constants below.
 */
typedef lh_u8_t lh_ui_web_host_conn_phase_t;

/** @def lh_ui_web_host_conn_phase_free
 *  @brief No connection: the slot is free. */
#define lh_ui_web_host_conn_phase_free 0U

/** @def lh_ui_web_host_conn_phase_head
 *  @brief The request head is arriving. */
#define lh_ui_web_host_conn_phase_head 1U

/** @def lh_ui_web_host_conn_phase_body
 *  @brief A buffered body (or a websocket handshake) is arriving whole. */
#define lh_ui_web_host_conn_phase_body 2U

/** @def lh_ui_web_host_conn_phase_stream
 *  @brief A streaming route takes the body as it arrives. */
#define lh_ui_web_host_conn_phase_stream 3U

/** @def lh_ui_web_host_conn_phase_upgraded
 *  @brief The connection became a websocket: hand its socket to the clients. */
#define lh_ui_web_host_conn_phase_upgraded 4U

/** @def lh_ui_web_host_conn_phase_done
 *  @brief The request is answered (or refused): close the socket. */
#define lh_ui_web_host_conn_phase_done 5U

#endif /* LH_UI_WEB_HOST_CONN_PHASE_H */
