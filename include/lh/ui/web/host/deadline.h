/**
 * @file deadline.h
 * @brief Socket-backed LH reader with a total web-host request deadline.
 *
 * Waits for readability through LH's socket API before each read. Its socket
 * is borrowed; the deadline is measured from the caller-provided start tick,
 * so reading a head and streaming a body can share one total timeout.
 */

#ifndef LH_UI_WEB_HOST_DEADLINE_H
#define LH_UI_WEB_HOST_DEADLINE_H

#include <lh/os/net/socket.h>
#include <lh/ui/web/host/deadline/fields.h>

/** @brief A borrowed socket and its total deadline. */
struct lh_ui_web_host_deadline
{
    /** Members: ::lh_ui_web_host_deadline_fields. */
    lh_ui_web_host_deadline_fields(lh_os_net_socket_t *, lh_u64_t, lh_u32_t);
};
/** @copybrief lh_ui_web_host_deadline */
typedef struct lh_ui_web_host_deadline lh_ui_web_host_deadline_t;

LH_COMPILER_EXTERN_C_BEGIN

/** Return the stored socket; @p self must be valid. */
lh_os_net_socket_t *
lh_ui_web_host_deadline_get_socket(const lh_ui_web_host_deadline_t *self);

/** Store socket without validating it; @p self must be valid. */
lh_void
lh_ui_web_host_deadline_set_socket(lh_ui_web_host_deadline_t *self, lh_os_net_socket_t *value);

/** Return the stored start; @p self must be valid. */
lh_u64_t
lh_ui_web_host_deadline_get_start(const lh_ui_web_host_deadline_t *self);

/** Store start without validating it; @p self must be valid. */
lh_void
lh_ui_web_host_deadline_set_start(lh_ui_web_host_deadline_t *self, lh_u64_t value);

/** Return the stored timeout; @p self must be valid. */
lh_u32_t
lh_ui_web_host_deadline_get_timeout(const lh_ui_web_host_deadline_t *self);

/** Store timeout without validating it; @p self must be valid. */
lh_void
lh_ui_web_host_deadline_set_timeout(lh_ui_web_host_deadline_t *self, lh_u32_t value);

/**
 * @brief Initialize a reader using a caller-owned socket and start tick.
 * @param self Reader state to initialize.
 * @param socket Borrowed connected socket; it must outlive reads.
 * @param start Monotonic millisecond tick when the request began.
 * @param timeout Total allowed milliseconds from @p start.
 * @pre @p self and @p socket are valid.
 */
lh_void
lh_ui_web_host_deadline_init(lh_ui_web_host_deadline_t *self, lh_os_net_socket_t *socket,
                             lh_u64_t start, lh_u32_t timeout);

/**
 * @brief Read from the socket before the total deadline; an LH reader callback.
 * @param context Initialized deadline state, borrowed.
 * @param out Caller-owned writable output.
 * @param size Output capacity in bytes.
 * @return Bytes read, 0 on peer EOF, ::LH_IO_PENDING if the socket reports it,
 *         or ::LH_IO_ERROR on timeout or I/O failure.
 * @pre Context is valid and nonzero @p size has that many writable bytes.
 */
lh_ssize_t
lh_ui_web_host_deadline_read(lh_self_ptr context, lh_ptr out, lh_usize_t size);

/**
 * @brief Construct an LH reader borrowing the deadline state.
 * @param self Initialized deadline state, which must outlive @p out.
 * @param out Caller-owned reader to initialize.
 */
lh_void
lh_ui_web_host_deadline_get_reader(lh_ui_web_host_deadline_t *self, lh_io_reader_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_WEB_HOST_DEADLINE_H */
