/**
 * @file link.h
 * @brief One device on one line: a request in flight, and an answer when it comes.
 *
 * The protocol in `lh/net/modbus.h` knows how to build a request and read an
 * answer. This knows the third thing, which is the one a configurator lives or
 * dies by: **nothing here ever waits.**
 *
 * A poll over eight devices on a slow line is not eight round trips in a row and
 * it is not a thread per device either. It is one loop that hands every idle
 * device something to do, then goes and looks at all of them again: each is asked
 * at most one question at a time, a device that has not answered yet is simply
 * still not answered, and a device that has gone quiet costs a timeout rather
 * than the whole pass. That is what makes the thing scale with the number of
 * devices instead of with the number of round trips.
 *
 * A link reads and writes an ::lh_io_stream_t and does not know what is behind it.
 * A TCP socket (::lh_os_net_socket_get_stream), a COM port
 * (::lh_os_serial_get_stream) and a pair of arrays in a test are the same two
 * calls, and this is what lets the state machine be tested with no device at all.
 *
 * The stream has to be **non-blocking**: a read answers ::LH_IO_PENDING when
 * nothing has arrived, and `0` only when the stream has ended. A blocking stream
 * works, but a read on it waits for the device, which is the one thing a link
 * exists not to do.
 *
 * @see lh_mb_link_ask, lh_mb_link_poll
 */

#ifndef LH_NET_MODBUS_LINK_H
#define LH_NET_MODBUS_LINK_H

#include <lh/byte/ptr.h>
#include <lh/net/modbus.h>
#include <lh/io/stream.h>
#include <lh/net/modbus/framing.h>
#include <lh/net/modbus/link/state.h>

LH_COMPILER_EXTERN_C_BEGIN

/* -- Framing --------------------------------------------------------------- */

/** @brief Bytes of MBAP header ahead of the unit and the PDU. */
#define LH_MB_MBAP_HEAD (lh_cast_static(lh_u16_t, 7))

/**
 * @brief Wrap @p pdu for @p framing.
 *
 * @param framing What the line carries.
 * @param unit Slave address.
 * @param pdu  Function code and data, as `lh_mb_build_pdu` left them.
 * @param pdu_length How many bytes @p pdu is.
 * @param out  Where the frame goes.
 * @param cap  How much @p out really has.
 * @param tx   Transaction id for MBAP; ignored for RTU, where there is nowhere to
 *             put one.
 *
 * @return Length of the frame, 0 when it does not fit.
 */
lh_u16_t
lh_mb_encode(lh_mb_framing_t framing, lh_u8_t unit, lh_u16_t tx, const lh_byte_ptr pdu,
             lh_u16_t pdu_length, lh_byte_ptr out, lh_u16_t cap);

/** How long an RTU answer is, from its function code: a read says how many bytes
   in its third byte, a write echoes its request, and an exception is five. 125
   registers is the most, so 255 bytes is the most a read answer can be. */
lh_u16_t
lh_mb_rtu_frame_length(const lh_byte_ptr frame, lh_u16_t have, lh_u8_t fc);

/**
 * @brief How many bytes a whole frame of @p framing takes, or 0 if it cannot.
 *
 * The answer to "have I got it all yet", which for RTU means finding the end by
 * the function code and for MBAP means reading the length out of the header. Not
 * knowing this is what makes a parser guess, and a parser that guesses reads the
 * next device's frame as its own.
 *
 * It does **not** ask whose frame it is. Over TCP a frame for another unit, or an
 * answer to a question that has already timed out, is still a frame of a known
 * length, and knowing that length is the only way to step over it to the one
 * behind it. Whose it is is ::lh_mb_decode's question.
 *
 * @return For MBAP, 0 also when the header is not a Modbus header at all — a
 *         protocol id other than 0, or a length no frame can have. With seven bytes
 *         in hand that is not "not yet": it is a stream that has lost its place.
 */
lh_u16_t
lh_mb_frame_length(lh_mb_framing_t framing, const lh_byte_ptr frame, lh_u16_t have, lh_u8_t fc);

/**
 * @brief The PDU inside @p frame, once the framing is off and the CRC, if the
 *        framing has one, has been checked.
 *
 * @param framing    What the line carries.
 * @param unit       Slave address the frame must be for.
 * @param frame      The frame as it came off the wire.
 * @param length     How many bytes @p frame holds.
 * @param pdu        Where the PDU goes; may be the same array as @p frame, since
 *                   only bytes in front of it are dropped.
 * @param pdu_length How many bytes the PDU is.
 *
 * @return Whether the frame is one of ours: @p unit's, and for MBAP with protocol
 *         id 0. A frame that is not is not an error to report upward: a line
 *         carries every device's traffic. The transaction id is not looked at here
 *         — only the link knows which question is in flight.
 */
lh_bool_t
lh_mb_decode(lh_mb_framing_t framing, lh_u8_t unit, const lh_byte_ptr frame, lh_u16_t length,
             lh_byte_ptr pdu, lh_u16_t *pdu_length);

/* -- Link ------------------------------------------------------------------ */

/** @brief How many words one link holds; the most a single read may ask for. */
#define LH_MB_LINK_VALUES (lh_cast_static(lh_u16_t, LH_MB_READ_MAX))

/**
 * @brief The numbers a bench reads and a status bar shows.
 *
 * Counted because "it is fast" is a claim and these are the evidence for it: how
 * many questions went out, how many answers came back whole, how many devices
 * stopped answering, and how long the answers took once they were in.
 */
typedef struct lh_mb_link_counters
{
    lh_u32_t sent;   /**< requests written to the stream */
    lh_u32_t done;   /**< answers parsed in full */
    lh_u32_t failed; /**< timeouts and stream errors, and nothing else */
    /** @brief Answers the device understood and refused: a Modbus exception.
     *
     *  **This is an answer, and it is the most useful one a device can give.** It
     *  says the line is good, the framing is right, the function code was
     *  understood, and this address is not on the device.
     *
     *  It used to be counted in ::failed, and that was a lie the app could not see
     *  through: the counter's own words said "timeouts and transport errors", the
     *  code counted exceptions there too, and a card that asked for one register
     *  the device did not have looked exactly like a device that had stopped
     *  answering. That is not a small difference here — the client's rule for
     *  "this device is off line" is a count of failures in a row, so a **healthy**
     *  controller with a card slightly wider than its firmware would be taken off
     *  line by its own app while every other device kept being polled.
     *
     *  Against a real controller on a panel this is not a corner: a card is a
     *  union of everything the family ever had, and asking a JL211 for a JL204C7
     *  register is refused, politely, forever.
     */
    lh_u32_t refused; /**< answers that were Modbus exceptions: the device is talking */
    lh_u32_t bytes;   /**< request and answer bytes both ways */
    lh_u64_t us_busy; /**< microseconds a request spent in flight */
} lh_mb_link_counters_t;

/**
 * @brief One device: the question in flight, the answer, and what it cost.
 */
typedef struct lh_mb_link
{
    lh_io_stream_t stream;   /**< what the bytes go through (::lh_mb_link_set_stream) */
    lh_mb_framing_t framing; /**< what the line carries */
    lh_u8_t unit;            /**< slave address of the device */
    lh_u8_t fc;              /**< function code a read uses (::lh_mb_link_set_fc) */
    /** @brief The function code of the question in flight: ::fc for a read,
     *  ::LH_MB_FC_WRITE_MANY for a write. The answer is read by this and not by
     *  ::fc, because a write does not change what the link reads next. */
    lh_u8_t asked_fc;
    lh_u8_t state; /**< where the question is, as a ::lh_mb_link_state_t */
    /** @brief Why the last poll ended the way it did, as a ::lh_mb_status_t.
     *
     *  The state says *whether* a question produced data; this says *why not*. The
     *  two questions have different answers and an app needs both: "the device is
     *  gone" and "the device is here and this address is not on it" leave the same
     *  state behind and mean opposite things to a device that has to be polled
     *  every ten seconds for the next eight hours.
     */
    lh_mb_status_t reason;
    lh_u16_t address;  /**< what the question in flight asked for */
    lh_u16_t expected; /**< how many words it asked for */
    lh_u16_t got;      /**< how many the answer carried */
    /** @brief The word a "write one register" sent, which its echo has to carry back. */
    lh_u16_t written;
    /** @brief Bytes received and not yet consumed.
     *
     *  Over MBAP this is a window on a byte stream, and it outlives a question: what
     *  comes after an answer, and what is left of an answer to a question that timed
     *  out, are the start of the next frame, and dropping them puts every later
     *  frame off by that many bytes. Only a new stream (::lh_mb_link_set_stream) or a
     *  stream that has lost its place empties it. Over RTU there is no length to
     *  find the next frame by, and it is emptied whenever a question ends. */
    lh_u16_t held;
    lh_u16_t tx; /**< next transaction id, for MBAP */
    /** @brief The transaction id of the question in flight, for MBAP.
     *
     *  The only thing that tells an answer to *this* question from a late answer
     *  to the last one. Without it, a device that answers just after the timeout
     *  has its answer read as the answer to the next question — and when the next
     *  question asks the same registers, which a poll loop does every pass, the
     *  stale values are accepted with nothing anywhere to say so. */
    lh_u16_t asked_tx;
    /** @brief When the question went out, on the link's own clock.
     *
     * 64 bits for the same reason the clock is 64: it is **microseconds since
     * boot**, and 32 bits of those run out after 4294 seconds. A machine that had
     * been up for seventy-one minutes stored a wrapped value here, and
     * `now - since_us` then came out at exactly 5 · 2³² microseconds — a question
     * that had been in flight for four and a half hours, so **every** request timed
     * out the instant it was sent and not one answer was ever parsed.
     *
     * It is not a failure that shows up in a test. A bench that runs for ninety
     * seconds on a machine that has been up for hours is a bench that never sees a
     * whole number, and a link that is asked and never answers looks exactly like a
     * device that is not there.
     */
    lh_u64_t since_us;
    lh_u16_t values[LH_MB_LINK_VALUES]; /**< words of the last answer (::lh_mb_link_take) */
    lh_byte_t frame[LH_MB_ADU_MAX];     /**< bytes received, the first ::held of them live */
    lh_mb_link_counters_t counters;     /**< what this link has cost so far */
    /* The clock, in microseconds. A link does not read the system clock itself:
       a test has to be able to move time by hand, and an app has to be able to
       decide what a microsecond is worth. */
    lh_u64_t (*now_us)(lh_ptr self); /**< the clock; ::lh_null counts nothing */
    lh_ptr clock;                    /**< passed to every ::now_us call */
    /** Not called by the link: a slot for the application's own notification. */
    lh_void (*on_change)(lh_ptr self, lh_ptr link);
    lh_ptr owner; /**< not read by the link: the application's own back pointer */
} lh_mb_link_t;   /**< One device: the question in flight, the answer, and what it cost. */

/**
 * @brief Point @p self at @p stream and start empty.
 *
 * @param self     Link to set up.
 * @param stream   What the bytes go through, non-blocking; copied, and what it
 *                 reads and writes must outlive the link.
 * @param framing  What the line carries.
 * @param unit     Slave address, 1..247.
 * @param now_us   The link's clock; `lh_null` means the link counts nothing.
 * @param clock    Passed to every @p now_us call.
 */
lh_void
lh_mb_link_init(lh_mb_link_t *self, const lh_io_stream_t *stream, lh_mb_framing_t framing,
                lh_u8_t unit, lh_u64_t (*now_us)(lh_ptr self), lh_ptr clock);

/**
 * @brief Carry on over a new @p stream: the connection was made again.
 *
 * Whatever was held belonged to the old stream and is dropped, and any question in
 * flight is forgotten -- its answer, if it comes, comes on the old one. This is
 * what an app does when ::lh_mb_link_t.reason says ::lh_mb_status_lost: open a new
 * socket or port, and hand it here. The counters are kept; they are the device's,
 * not the connection's.
 */
lh_void
lh_mb_link_set_stream(lh_mb_link_t *self, const lh_io_stream_t *stream);

/**
 * @brief Ask ::LH_MB_FC_READ_INPUT or ::LH_MB_FC_READ_HOLDING on this link.
 *
 * Modbus has two read functions and they read two different spaces: 0x03 reads
 * holding registers (4xxxx) and 0x04 reads input registers (3xxxx). A device that
 * has both is **two conversations**, not one — the answer frames differ by a byte
 * of function code, and a link that asked 0x03 can only be answered by a device
 * that has holding registers at that address.
 *
 * The cards in the firmware need exactly that: JL204C5M names 761 input registers
 * against 646 holding, and JL208VAV 133 against none. A link that could only ask
 * holding would be unable to poll most of a real device, so the code is a field an
 * app sets rather than a constant in the library.
 *
 * @param self Link to aim.
 * @param fc   ::LH_MB_FC_READ_HOLDING or ::LH_MB_FC_READ_INPUT.
 *
 * @note Ignored unless the link is idle. A link with a question already out cannot
 *       be re-aimed: the answer in flight belongs to the question that was asked,
 *       and reading it as the other function's frame is exactly the mistake the
 *       field exists to prevent.
 */
lh_void
lh_mb_link_set_fc(lh_mb_link_t *self, lh_u8_t fc);

/** @brief The function code this link asks, ::LH_MB_FC_READ_HOLDING by default. */
lh_u8_t
lh_mb_link_get_fc(const lh_mb_link_t *self);

/** The stream is gone: the peer closed it, it failed, or it took half a request. A
   frame boundary cannot be found on it again, so nothing held is kept, and the
   reason says so -- the app opens a new stream and hands it to
   ::lh_mb_link_set_stream. */
lh_mb_link_state_t
lh_mb_link_lose(lh_mb_link_t *self);

/** Drop the first @p n bytes held. Over MBAP what follows them is the start of the
   next frame and is kept: TCP is a stream, a recv hands over whatever has arrived,
   and an answer and the frame behind it come in one piece as often as not. Over RTU
   there is no length to find the next frame by, so everything goes. */
lh_void
lh_mb_link_consume(lh_mb_link_t *self, lh_u16_t n);

/** Everything after the PDU is the same for a read and a write: the envelope, the send,
   and the bookkeeping that makes the answer this question's. @p fc is the code the
   answer will carry, and ::lh_mb_link_poll reads it by that. */
lh_bool_t
lh_mb_link_send(lh_mb_link_t *self, const lh_byte_ptr pdu, lh_u16_t pdu_length, lh_u16_t address,
                lh_u16_t count, lh_u8_t fc);

/**
 * @brief Ask for @p count registers at @p address, if nothing else is in flight.
 *
 * @return Whether a question went out. False means the link is busy, which is
 *         what keeps one device from being asked two things at once: a second
 *         request while the first is unanswered would arrive after its own answer
 *         and be answered as somebody else's frame.
 */
lh_bool_t
lh_mb_link_ask(lh_mb_link_t *self, lh_u16_t address, lh_u16_t count);

/**
 * @brief Write @p count registers from @p values at @p address ("write many
 *        registers", ::LH_MB_FC_WRITE_MANY), if nothing else is in flight.
 *
 * The same one-question-at-a-time rule as ::lh_mb_link_ask, and the same answer path:
 * ::lh_mb_link_poll says when the device has confirmed it, and an answered write
 * carries no values (::lh_mb_link_take gives a count of 0). A refusal is a
 * ::lh_mb_status_exception in ::lh_mb_link_t.reason, as for a read.
 *
 * @return Whether the request went out.
 */
lh_bool_t
lh_mb_link_write(lh_mb_link_t *self, lh_u16_t address, const lh_u16_t *values, lh_u16_t count);

/**
 * @brief Write one register with @p value at @p address ("write one register",
 *        ::LH_MB_FC_WRITE_ONE), if nothing else is in flight.
 *
 * The same rules and the same answer path as ::lh_mb_link_write: one question at a
 * time, ::lh_mb_link_poll says when the device confirmed it, an answered write
 * carries no values, and a refusal is a ::lh_mb_status_exception. For the devices
 * that take 0x06 and refuse 0x10, and for one word anywhere.
 *
 * @return Whether the request went out.
 */
lh_bool_t
lh_mb_link_write_one(lh_mb_link_t *self, lh_u16_t address, lh_u16_t value);

/**
 * @brief Look for an answer without waiting for one.
 *
 * One read of the stream, then a decision: whole frame, not yet, or not
 * ours. Never blocks, and never asks for anything new — sending is
 * ::lh_mb_link_ask's job, so that a pass cannot turn into a stall.
 *
 * @param self Link to look at.
 * @param timeout_us How long a question may stay unanswered.
 *
 * @return The state the link is in afterwards.
 */
lh_mb_link_state_t
lh_mb_link_poll(lh_mb_link_t *self, lh_u32_t timeout_us);

/**
 * @brief Take the answer, or say there is none.
 *
 * The words stay where they are and are not copied out: a poll of a few hundred
 * registers per device per pass is a lot of copying, and the caller is reading
 * them before the next pass anyway.
 *
 * @param self  Link to take from.
 * @param count How many words arrived.
 *
 * @return The words, or `lh_null` when there is no answer to take.
 */
const lh_u16_t *
lh_mb_link_take(lh_mb_link_t *self, lh_u16_t *count);

/**
 * @brief Hand the answer back and go idle.
 *
 * Separate from ::lh_mb_link_take on purpose: taking a value and giving the link
 * back are different moments, and a caller that reads one register out of a
 * hundred and then wants another device's turn should not have to give the link
 * up first to get it.
 */
lh_void
lh_mb_link_release(lh_mb_link_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_NET_MODBUS_LINK_H */
