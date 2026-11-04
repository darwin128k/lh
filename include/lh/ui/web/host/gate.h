/**
 * @file gate.h
 * @brief ::lh_ui_web_host_gate_t — a socket reader that receives once per readiness.
 *
 * The web host waits for its sockets with one poll and then lets each ready
 * connection take one step. A connection's socket stays blocking (its replies
 * are written whole), so it must be received from only when the poll said it
 * is readable — then one receive returns at once. The gate is that rule as an
 * ::lh_io_reader_t: after ::lh_ui_web_host_gate_set_ready it lets one receive
 * through; every other read answers ::LH_IO_PENDING. Readers built on it (the
 * HTTP body reader, the gzip reader) keep their state across pending reads, so a
 * request body is decoded piece by piece as it arrives.
 *
 * The record and the socket belong to the caller; the gate allocates nothing.
 */

#ifndef LH_UI_WEB_HOST_GATE_H
#define LH_UI_WEB_HOST_GATE_H

#include <lh/os/net/socket.h>
#include <lh/ui/web/host/gate/fields.h>

/**
 * @brief A socket reader that receives once per readiness.
 */
struct lh_ui_web_host_gate
{
    /** Members: ::lh_ui_web_host_gate_fields. */
    lh_ui_web_host_gate_fields(lh_os_net_socket_t, lh_bool_t);
};

/** @copybrief lh_ui_web_host_gate */
typedef struct lh_ui_web_host_gate lh_ui_web_host_gate_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The socket read from, borrowed (level 1).
 * @param self Must not be null.
 * @return The value.
 */
lh_os_net_socket_t *
lh_ui_web_host_gate_get_socket(const lh_ui_web_host_gate_t *self);

/**
 * @brief Store the socket read from, borrowed (level 1).
 * @param self  Must not be null.
 * @param value The new value.
 */
lh_void
lh_ui_web_host_gate_set_socket(lh_ui_web_host_gate_t *self, lh_os_net_socket_t *value);

/**
 * @brief Whether one receive may run now (level 1).
 * @param self Must not be null.
 * @return The value.
 */
lh_bool_t
lh_ui_web_host_gate_is_ready(const lh_ui_web_host_gate_t *self);

/**
 * @brief Store whether one receive may run now (level 1).
 * @param self  Must not be null.
 * @param value The new value.
 */
lh_void
lh_ui_web_host_gate_set_ready(lh_ui_web_host_gate_t *self, lh_bool_t value);

/**
 * @brief The gate as an ::lh_io_reader_t.
 * @param self Gate; must not be null. It must outlive the reader.
 * @param out  Receives the reader.
 */
lh_void
lh_ui_web_host_gate_get_reader(lh_ui_web_host_gate_t *self, lh_io_reader_t *out);

/**
 * @brief Gate @p socket: no receive until the first readiness.
 * @param self   Gate; must not be null.
 * @param socket Connected socket, borrowed; must outlive the gate's use.
 */
lh_void
lh_ui_web_host_gate_init(lh_ui_web_host_gate_t *self, lh_os_net_socket_t *socket);

/**
 * @brief Read from the gated socket (an ::lh_io_reader_read_fn).
 *
 * @param context The ::lh_ui_web_host_gate_t.
 * @param out     Destination.
 * @param size    Capacity of @p out.
 * @return Bytes received; 0 when the peer closed; ::LH_IO_PENDING when the
 *         gate is not ready (or the socket had nothing after all); a negative
 *         error otherwise. A positive @p size consumes the readiness.
 */
lh_ssize_t
lh_ui_web_host_gate_read(lh_self_ptr context, lh_ptr out, lh_usize_t size);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_WEB_HOST_GATE_H */
