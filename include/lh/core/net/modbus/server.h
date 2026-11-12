/**
 * @file server.h
 * @brief ::lh_mb_server_t — a Modbus device: it reads requests off a stream, has
 *        the application do them, and answers.
 *
 * The other side of ::lh_mb_link_t. A link asks and a server answers, over the
 * same streams and the same two framings: RTU on a serial line (unit, PDU, CRC)
 * and Modbus TCP (the seven-byte MBAP header, PDU). It speaks the four codes the
 * link speaks -- read holding and input registers (0x03, 0x04), write one and many
 * registers (0x06, 0x10) -- and refuses every other with "illegal function".
 *
 * The server owns the wire and nothing else. Which registers exist, what is in
 * them and what a write does is the application's, behind two callbacks
 * (::lh_mb_server_read_fn, ::lh_mb_server_write_fn). Before either is called the
 * request has passed every check the protocol makes: a whole frame, a good CRC, a
 * unit that is this one, a count in range, a byte count that matches it, an
 * address range inside the 65536 registers. What fails a check is refused with the
 * exception the specification names for it, and the application is never asked.
 *
 * A server never blocks. ::lh_mb_server_poll reads what has arrived, and when a
 * whole request is held it serves it: one request per poll, so an application that
 * polls in a loop serves requests as fast as they come and can do its own work
 * between them. Over RTU a request for another unit is stepped over (a serial line
 * carries every unit's traffic), and a write to unit 0 is done by every server and
 * answered by none. Over Modbus TCP the answer carries the request's transaction
 * id, and requests that arrive together are served one after another.
 *
 * A frame is found by its length: RTU requests say how long they are by their
 * function code, MBAP by its header. RTU also ends a frame with silence (three and
 * a half characters); with a clock and ::lh_mb_server_set_gap_us, bytes after a
 * longer silence start a new frame, and a request with a code the server does not
 * know is taken whole at the silence and refused. Without the gap such a request
 * cannot be sized and is dropped.
 *
 * Lifecycle: ::lh_mb_server_init, then ::lh_mb_server_poll as often as the
 * application likes. On ::lh_mb_server_lost the stream is gone or out of step: the
 * application closes it and hands the server a new one (::lh_mb_server_set_stream).
 * The server holds no memory of its own beyond the struct.
 *
 * @see lh_mb_link_t, lh_mb_server_poll
 */

#ifndef LH_NET_MODBUS_SERVER_H
#define LH_NET_MODBUS_SERVER_H

#include <lh/core/net/modbus/link.h>
#include <lh/core/net/modbus/server/counters.h>
#include <lh/core/net/modbus/server/read/cb.h>
#include <lh/core/net/modbus/server/result.h>
#include <lh/core/net/modbus/server/write/cb.h>

/** @brief A Modbus device on one stream. */
typedef struct lh_mb_server
{
    lh_io_stream_t stream;            /**< what the bytes go through (::lh_mb_server_set_stream) */
    lh_mb_framing_t framing;          /**< what the line carries */
    lh_u8_t unit;                     /**< the unit this server answers as, 1..247 */
    lh_mb_server_read_cb read;        /**< the application's read; ::lh_null refuses reads */
    lh_mb_server_write_cb write;      /**< the application's write; ::lh_null refuses writes */
    lh_ptr owner;                     /**< passed to ::read and ::write */
    lh_mb_clock_cb now_us;            /**< the clock; ::lh_null counts no time */
    lh_ptr clock;                     /**< passed to every ::now_us call */
    lh_u32_t gap_us;                  /**< RTU: silence that ends a frame; 0 does not look */
    lh_u64_t heard_us;                /**< when bytes last arrived */
    lh_u16_t held;                    /**< bytes received and not yet consumed */
    lh_byte_t frame[LH_MB_ADU_MAX];   /**< bytes received, the first ::held of them live */
    lh_mb_server_counters_t counters; /**< what the server has done so far */
} lh_mb_server_t;                     /**< A Modbus device on one stream. */

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief What the line carries (level 1).
 *
 * @param self The server; checked by ::lh_assert_runtime_ref.
 */
lh_mb_framing_t
lh_mb_server_get_framing(const lh_mb_server_t *self);

/**
 * @brief Store what the line carries (level 1).
 *
 * @param self The server; checked by ::lh_assert_runtime_ref.
 * @param framing The new value.
 */
lh_void
lh_mb_server_set_framing(lh_mb_server_t *self, lh_mb_framing_t framing);

/**
 * @brief The unit this server answers as, 1..247 (level 1).
 *
 * @param self The server; checked by ::lh_assert_runtime_ref.
 */
lh_u8_t
lh_mb_server_get_unit(const lh_mb_server_t *self);

/**
 * @brief Store the unit this server answers as, 1..247 (level 1).
 *
 * @param self The server; checked by ::lh_assert_runtime_ref.
 * @param unit The new value.
 */
lh_void
lh_mb_server_set_unit(lh_mb_server_t *self, lh_u8_t unit);

/**
 * @brief The application's read; ::lh_null refuses reads (level 1).
 *
 * @param self The server; checked by ::lh_assert_runtime_ref.
 */
lh_mb_server_read_cb
lh_mb_server_get_read(const lh_mb_server_t *self);

/**
 * @brief Store the application's read; ::lh_null refuses reads (level 1).
 *
 * @param self The server; checked by ::lh_assert_runtime_ref.
 * @param read The new value.
 */
lh_void
lh_mb_server_set_read(lh_mb_server_t *self, lh_mb_server_read_cb read);

/**
 * @brief The application's write; ::lh_null refuses writes (level 1).
 *
 * @param self The server; checked by ::lh_assert_runtime_ref.
 */
lh_mb_server_write_cb
lh_mb_server_get_write(const lh_mb_server_t *self);

/**
 * @brief Store the application's write; ::lh_null refuses writes (level 1).
 *
 * @param self The server; checked by ::lh_assert_runtime_ref.
 * @param write The new value.
 */
lh_void
lh_mb_server_set_write(lh_mb_server_t *self, lh_mb_server_write_cb write);

/**
 * @brief Passed to ::read and ::write: the application's own pointer (level 1).
 *
 * @param self The server; checked by ::lh_assert_runtime_ref.
 */
lh_ptr
lh_mb_server_get_owner(const lh_mb_server_t *self);

/**
 * @brief Store passed to ::read and ::write: the application's own pointer (level 1).
 *
 * @param self The server; checked by ::lh_assert_runtime_ref.
 * @param owner The new value.
 */
lh_void
lh_mb_server_set_owner(lh_mb_server_t *self, lh_ptr owner);

/**
 * @brief The clock; ::lh_null counts no time (level 1).
 *
 * @param self The server; checked by ::lh_assert_runtime_ref.
 */
lh_mb_clock_cb
lh_mb_server_get_now_us(const lh_mb_server_t *self);

/**
 * @brief Store the clock; ::lh_null counts no time (level 1).
 *
 * @param self The server; checked by ::lh_assert_runtime_ref.
 * @param now_us The new value.
 */
lh_void
lh_mb_server_set_now_us(lh_mb_server_t *self, lh_mb_clock_cb now_us);

/**
 * @brief Passed to every ::now_us call (level 1).
 *
 * @param self The server; checked by ::lh_assert_runtime_ref.
 */
lh_ptr
lh_mb_server_get_clock(const lh_mb_server_t *self);

/**
 * @brief Store passed to every ::now_us call (level 1).
 *
 * @param self The server; checked by ::lh_assert_runtime_ref.
 * @param clock The new value.
 */
lh_void
lh_mb_server_set_clock(lh_mb_server_t *self, lh_ptr clock);

/**
 * @brief RTU: silence in microseconds that ends a frame; 0 does not look (level 1).
 *
 * @param self The server; checked by ::lh_assert_runtime_ref.
 */
lh_u32_t
lh_mb_server_get_gap_us(const lh_mb_server_t *self);

/**
 * @brief Store RTU: silence in microseconds that ends a frame; 0 does not look (level 1).
 *
 * @param self The server; checked by ::lh_assert_runtime_ref.
 * @param gap_us The new value.
 */
lh_void
lh_mb_server_set_gap_us(lh_mb_server_t *self, lh_u32_t gap_us);

/**
 * @brief When bytes last arrived, on the server's clock (level 1).
 *
 * @param self The server; checked by ::lh_assert_runtime_ref.
 */
lh_u64_t
lh_mb_server_get_heard_us(const lh_mb_server_t *self);

/**
 * @brief Store when bytes last arrived, on the server's clock (level 1).
 *
 * @param self The server; checked by ::lh_assert_runtime_ref.
 * @param heard_us The new value.
 */
lh_void
lh_mb_server_set_heard_us(lh_mb_server_t *self, lh_u64_t heard_us);

/**
 * @brief Bytes received and not yet consumed (level 1).
 *
 * @param self The server; checked by ::lh_assert_runtime_ref.
 */
lh_u16_t
lh_mb_server_get_held(const lh_mb_server_t *self);

/**
 * @brief Store bytes received and not yet consumed (level 1).
 *
 * @param self The server; checked by ::lh_assert_runtime_ref.
 * @param held The new value.
 */
lh_void
lh_mb_server_set_held(lh_mb_server_t *self, lh_u16_t held);

/**
 * @brief The stream the server reads and writes (level 1).
 *
 * @param self The server; checked by ::lh_assert_runtime_ref.
 */
const lh_io_stream_t *
lh_mb_server_get_stream(const lh_mb_server_t *self);

/**
 * @brief The stream the server reads and writes, to change (level 1).
 *
 * @param self The server; checked by ::lh_assert_runtime_ref.
 */
lh_io_stream_t *
lh_mb_server_get_stream_mut(lh_mb_server_t *self);

/**
 * @brief What the server has done so far (level 1).
 *
 * @param self The server; checked by ::lh_assert_runtime_ref.
 */
const lh_mb_server_counters_t *
lh_mb_server_get_counters(const lh_mb_server_t *self);

/**
 * @brief What the server has done so far, to change (level 1).
 *
 * @param self The server; checked by ::lh_assert_runtime_ref.
 */
lh_mb_server_counters_t *
lh_mb_server_get_counters_mut(lh_mb_server_t *self);

/**
 * @brief The bytes received, the first ::lh_mb_server_get_held of them live (level 1).
 *
 * @param self The server; checked by ::lh_assert_runtime_ref.
 */
const lh_byte_t *
lh_mb_server_get_frame(const lh_mb_server_t *self);

/**
 * @brief The bytes received, to fill (level 1).
 *
 * @param self The server; checked by ::lh_assert_runtime_ref.
 */
lh_byte_t *
lh_mb_server_get_frame_mut(lh_mb_server_t *self);

/**
 * @brief The server's clock now, in microseconds; 0 without a clock (level 2).
 *
 * @param self The server; checked by ::lh_assert_runtime_ref.
 */
lh_u64_t
lh_mb_server_get_time_us(const lh_mb_server_t *self);

/**
 * @brief Whether a frame for @p unit is one this server serves (level 2).
 *
 * Its own unit, and unit 0 -- over RTU the broadcast, over Modbus TCP the address
 * a server answers as itself, as it does 255.
 *
 * @param self The server; checked by ::lh_assert_runtime_ref.
 * @param unit The unit the frame carries.
 */
lh_bool_t
lh_mb_server_is_for_unit(const lh_mb_server_t *self, lh_u8_t unit);

/**
 * @brief Whether a frame for @p unit is a broadcast: RTU and unit 0 (level 2).
 *
 * @param self The server; checked by ::lh_assert_runtime_ref.
 * @param unit The unit the frame carries.
 */
lh_bool_t
lh_mb_server_is_broadcast(const lh_mb_server_t *self, lh_u8_t unit);

/**
 * @brief How long the request at the start of what is held is (level 2).
 *
 * ::lh_mb_frame_length for Modbus TCP, ::lh_mb_rtu_request_length for RTU.
 *
 * @param self The server; checked by ::lh_assert_runtime_ref.
 *
 * @return The length, or 0 when it cannot be told from what is held.
 */
lh_u16_t
lh_mb_server_get_request_length(const lh_mb_server_t *self);

/**
 * @brief Drop the first @p n bytes held and keep what follows: the start of the
 *        next request. @p n at or past ::lh_mb_server_get_held empties it.
 *
 * @param self The server; checked by ::lh_assert_runtime_ref.
 * @param n    How many bytes.
 */
lh_void
lh_mb_server_consume(lh_mb_server_t *self, lh_u16_t n);

/**
 * @brief The stream is gone or out of step: forget what is held.
 *
 * @param self The server; checked by ::lh_assert_runtime_ref.
 *
 * @return ::lh_mb_server_lost, for the caller to return.
 */
lh_mb_server_result_t
lh_mb_server_lose(lh_mb_server_t *self);

/**
 * @brief Point @p self at @p stream and start empty: a new connection, or the
 *        port opened again.
 *
 * @param self   The server; checked by ::lh_assert_runtime_ref.
 * @param stream What the bytes go through, non-blocking; copied.
 */
lh_void
lh_mb_server_set_stream(lh_mb_server_t *self, const lh_io_stream_t *stream);

/**
 * @brief Answer a read (0x03 or 0x04) **PDU**: the words, or the refusal.
 *
 * Refused with ::LH_MB_EXCEPTION_ILLEGAL_VALUE when the PDU is not five bytes or
 * the count is not 1..::LH_MB_READ_MAX, with ::LH_MB_EXCEPTION_ILLEGAL_ADDRESS when
 * the registers run past 65535, with ::LH_MB_EXCEPTION_ILLEGAL_FUNCTION when the
 * server has no read, and with whatever the read returns when it refuses.
 *
 * @param self The server; checked by ::lh_assert_runtime_ref.
 * @param pdu  The request: function code and data.
 * @param length How many bytes @p pdu holds, at least 1.
 * @param out  Where the answer PDU goes.
 * @param cap  How much @p out has; 2 + 2 · ::LH_MB_READ_MAX is always enough.
 *
 * @return The length of the answer PDU; 0 when a pointer is `lh_null` or the
 *         answer does not fit @p out.
 */
lh_u16_t
lh_mb_server_answer_read(lh_mb_server_t *self, const lh_byte_ptr pdu, lh_u16_t length,
                         lh_byte_ptr out, lh_u16_t cap);

/**
 * @brief Answer a "write one register" (0x06) **PDU**: the request echoed, or the
 *        refusal.
 *
 * Refused with ::LH_MB_EXCEPTION_ILLEGAL_VALUE when the PDU is not five bytes,
 * ::LH_MB_EXCEPTION_ILLEGAL_FUNCTION when the server has no write, and with
 * whatever the write returns when it refuses.
 *
 * @param self The server; checked by ::lh_assert_runtime_ref.
 * @param pdu  The request: function code and data.
 * @param length How many bytes @p pdu holds, at least 1.
 * @param out  Where the answer PDU goes; 5 bytes.
 * @param cap  How much @p out has.
 *
 * @return The length of the answer PDU; 0 when a pointer is `lh_null` or it does
 *         not fit.
 */
lh_u16_t
lh_mb_server_answer_write_one(lh_mb_server_t *self, const lh_byte_ptr pdu, lh_u16_t length,
                              lh_byte_ptr out, lh_u16_t cap);

/**
 * @brief Answer a "write many registers" (0x10) **PDU**: its address and count
 *        echoed, or the refusal.
 *
 * Refused with ::LH_MB_EXCEPTION_ILLEGAL_VALUE when the count is not
 * 1..::LH_MB_WRITE_MAX, the byte count is not twice the count or the PDU is not as
 * long as the byte count says; ::LH_MB_EXCEPTION_ILLEGAL_ADDRESS when the registers
 * run past 65535; ::LH_MB_EXCEPTION_ILLEGAL_FUNCTION when the server has no write;
 * and with whatever the write returns when it refuses.
 *
 * @param self The server; checked by ::lh_assert_runtime_ref.
 * @param pdu  The request: function code and data.
 * @param length How many bytes @p pdu holds, at least 1.
 * @param out  Where the answer PDU goes; 5 bytes.
 * @param cap  How much @p out has.
 *
 * @return The length of the answer PDU; 0 when a pointer is `lh_null` or it does
 *         not fit.
 */
lh_u16_t
lh_mb_server_answer_write_many(lh_mb_server_t *self, const lh_byte_ptr pdu, lh_u16_t length,
                               lh_byte_ptr out, lh_u16_t cap);

/**
 * @brief Set up @p self as unit @p unit on @p stream, with nothing held and every
 *        counter at zero.
 *
 * @param self    The server; checked by ::lh_assert_runtime_ref.
 * @param stream  What the bytes go through, non-blocking; copied.
 * @param framing What the line carries.
 * @param unit    The unit to answer as, 1..247.
 * @param read    The application's read; ::lh_null refuses reads.
 * @param write   The application's write; ::lh_null refuses writes.
 * @param owner   Passed to @p read and @p write.
 * @param now_us  The clock; ::lh_null counts no time (and RTU silence is not seen).
 * @param clock   Passed to every @p now_us call.
 */
lh_void
lh_mb_server_init(lh_mb_server_t *self, const lh_io_stream_t *stream, lh_mb_framing_t framing,
                  lh_u8_t unit, lh_mb_server_read_cb read, lh_mb_server_write_cb write,
                  lh_ptr owner, lh_mb_clock_cb now_us, lh_ptr clock);

/**
 * @brief Answer any request **PDU**: the four codes by their own answers, every
 *        other code with ::LH_MB_EXCEPTION_ILLEGAL_FUNCTION.
 *
 * @param self The server; checked by ::lh_assert_runtime_ref.
 * @param pdu  The request: function code and data.
 * @param length How many bytes @p pdu holds, at least 1.
 * @param out  Where the answer PDU goes.
 * @param cap  How much @p out has.
 *
 * @return The length of the answer PDU; 0 when it cannot be built.
 */
lh_u16_t
lh_mb_server_answer(lh_mb_server_t *self, const lh_byte_ptr pdu, lh_u16_t length, lh_byte_ptr out,
                    lh_u16_t cap);

/**
 * @brief Put the answer @p pdu to @p unit on the stream, in the server's framing.
 *
 * @param self The server; checked by ::lh_assert_runtime_ref.
 * @param unit The unit the request was for.
 * @param tx   The request's transaction id (Modbus TCP); ignored over RTU.
 * @param pdu  The answer PDU.
 * @param length Its length.
 *
 * @return ::lh_bool_true when the whole answer went out. ::lh_bool_false when it
 *         could not be framed, or the stream took less than all of it: half an
 *         answer on the line is a frame nobody sent, so the stream is lost
 *         (::lh_mb_server_lose).
 */
lh_bool_t
lh_mb_server_try_reply(lh_mb_server_t *self, lh_u8_t unit, lh_u16_t tx, const lh_byte_ptr pdu,
                       lh_u16_t length);

/**
 * @brief Serve one whole frame of @p length bytes at the start of what is held.
 *
 * Checks the CRC (RTU) and the unit, has the request answered
 * (::lh_mb_server_answer) and the answer sent (::lh_mb_server_try_reply), and counts
 * what happened. Consumes nothing: ::lh_mb_server_poll does.
 *
 * @param self   The server; checked by ::lh_assert_runtime_ref.
 * @param length The frame's length, as ::lh_mb_server_get_request_length gave it.
 *
 * @return What was done with the frame.
 */
lh_mb_server_result_t
lh_mb_server_serve(lh_mb_server_t *self, lh_u16_t length);

/**
 * @brief What is held cannot be sized: serve it, drop it, or wait for more.
 *
 * Over Modbus TCP a header that gives no length is no Modbus header, and the
 * stream is out of step: ::lh_mb_server_lost. Over RTU a "write many" whose byte
 * count has not arrived waits; a function code the server does not know is taken
 * whole once the line has been silent for the gap (::lh_mb_server_set_gap_us) and
 * refused as an illegal function, and without a gap or a clock it is dropped.
 *
 * @param self The server; checked by ::lh_assert_runtime_ref.
 *
 * @return What was done; ::lh_mb_server_idle while waiting.
 */
lh_mb_server_result_t
lh_mb_server_serve_unsized(lh_mb_server_t *self);

/**
 * @brief Read what has arrived and serve the first whole request held.
 *
 * Never blocks: one read of the stream, at most one request served.
 *
 * @param self The server; checked by ::lh_assert_runtime_ref.
 *
 * @return What this poll did; ::lh_mb_server_idle when there is no whole request
 *         yet.
 */
lh_mb_server_result_t
lh_mb_server_poll(lh_mb_server_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_NET_MODBUS_SERVER_H */
