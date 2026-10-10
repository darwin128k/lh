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
 * A link owns a transport (::lh_mb_transport_t) and does not know what one is.
 * A TCP socket, a COM port and a pair of arrays in a test are the same three
 * calls, and this is what lets the state machine be tested with no device at all.
 *
 * @see lh_mb_link_ask, lh_mb_link_poll
 */

#ifndef LH_NET_MODBUS_LINK_H
#define LH_NET_MODBUS_LINK_H

#include <lh/bool.h>
#include <lh/net/modbus.h>
#include <lh/char.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/fixed/types.h>
#include <lh/util/ptr.h>
#include <lh/void.h>

LH_COMPILER_EXTERN_C_BEGIN

/* ── Framing ─────────────────────────────────────────────────────────────── */

/**
 * @brief How a request is wrapped for the wire it goes out on.
 *
 * One PDU, two envelopes. A serial line has no header and ends a frame with a
 * CRC; a network has a seven-byte header with a length in it and no CRC. A
 * program that assumed one of them is a program that talks to half the devices
 * in the world.
 */
typedef enum lh_mb_framing
{
    lh_mb_framing_rtu = 0, /**< unit, PDU, CRC low byte first — a COM port, or RTU over TCP */
    lh_mb_framing_mbap = 1 /**< seven-byte header with a length, unit, PDU — Modbus TCP */
} lh_mb_framing_t;

/** @brief Bytes of MBAP header ahead of the unit and the PDU. */
#define LH_MB_MBAP_HEAD ((lh_u16_t)7)

/**
 * @brief Wrap @p pdu for @p framing.
 *
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
lh_mb_encode(lh_mb_framing_t framing, lh_u8_t unit, lh_u16_t tx, const lh_byte_t *pdu,
             lh_u16_t pdu_length, lh_byte_t *out, lh_u16_t cap);

/**
 * @brief How many bytes a whole frame of @p framing takes, or 0 if it cannot.
 *
 * The answer to "have I got it all yet", which for RTU means finding the end by
 * the function code and for MBAP means reading the length out of the header. Not
 * knowing this is what makes a parser guess, and a parser that guesses reads the
 * next device's frame as its own.
 */
lh_u16_t
lh_mb_frame_length(lh_mb_framing_t framing, const lh_byte_t *frame, lh_u16_t have, lh_u8_t unit,
                   lh_u8_t fc);

/**
 * @brief The PDU inside @p frame, once the framing is off and the CRC, if the
 *        framing has one, has been checked.
 *
 * @param pdu        Where the PDU goes; may be the same array as @p frame, since
 *                   only bytes in front of it are dropped.
 * @param pdu_length How many bytes the PDU is.
 *
 * @return Whether the frame is one of ours. A frame that is not is not an error
 *         to report upward: a line carries every device's traffic.
 */
lh_bool_t
lh_mb_decode(lh_mb_framing_t framing, lh_u8_t unit, const lh_byte_t *frame, lh_u16_t length,
             lh_byte_t *pdu, lh_u16_t *pdu_length);

/* ── Transport ───────────────────────────────────────────────────────────── */

/**
 * @brief Whoever moves the bytes.
 *
 * Every call is non-blocking and answers in one step: there is no "wait for it"
 * call anywhere in this interface, because a program that has one has a place
 * where a dead device stops the world. A transport that has nothing to report
 * says so — zero bytes read, or fewer written than asked — and the link comes
 * back to it later.
 *
 * @param send Bytes written, or a negative value for an error.
 * @param recv Bytes read, 0 for nothing yet, or a negative value for an error.
 */
typedef struct lh_mb_transport
{
    lh_ptr self; /**< whatever the four calls below close over */
    lh_bool_t (*open)(lh_ptr self, const lh_char_t *target, lh_u16_t port);
    lh_void (*close)(lh_ptr self);
    lh_s32_t (*send)(lh_ptr self, const lh_byte_t *bytes, lh_u16_t length);
    lh_s32_t (*recv)(lh_ptr self, lh_byte_t *bytes, lh_u16_t cap);
} lh_mb_transport_t;

/* ── Link ────────────────────────────────────────────────────────────────── */

/** @brief How many words one link holds; the most a single read may ask for. */
#define LH_MB_LINK_VALUES ((lh_u16_t)LH_MB_READ_MAX)

/**
 * @brief Where a link is with the device behind it.
 *
 * @note ::lh_mb_link_failed is a **result, not a resting place**. A question that
 *       timed out, came back with a bad CRC or came back as an exception puts the
 *       link straight back to ::lh_mb_link_idle and this poll reports the failure
 *       on its way. A link stayed in `failed` for the rest of the session, which
 *       meant a device that answers "illegal function" for one register bank was
 *       dropped for good, and a poll loop that keeps finding the link busy spins
 *       for ever instead of moving on to the next device. Whether a device is
 *       *off* rather than *slow* is the app's decision — only it knows how many
 *       failures in a row mean that — and ::lh_mb_link_counters_t.failed is what
 *       it counts them from.
 */
typedef enum lh_mb_link_state
{
    lh_mb_link_idle = 0,  /**< nothing in flight */
    lh_mb_link_waiting,   /**< a question is out and no answer has come back yet */
    lh_mb_link_answered,  /**< an answer arrived and was parsed; ::lh_mb_link_take takes it */
    lh_mb_link_failed     /**< the transport gave up, or the answer was not ours */
} lh_mb_link_state_t;

/**
 * @brief The numbers a bench reads and a status bar shows.
 *
 * Counted because "it is fast" is a claim and these are the evidence for it: how
 * many questions went out, how many answers came back whole, how many devices
 * stopped answering, and how long the answers took once they were in.
 */
typedef struct lh_mb_link_counters
{
    lh_u32_t sent;    /**< requests written to the transport */
    lh_u32_t done;    /**< answers parsed in full */
    lh_u32_t failed;  /**< timeouts and transport errors, and nothing else */
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
    lh_mb_transport_t transport;
    lh_mb_framing_t framing;
    lh_u8_t unit;
    lh_u8_t fc;
    /** @brief The function code of the question in flight: ::fc for a read,
     *  ::LH_MB_FC_WRITE_MANY for a write. The answer is read by this and not by
     *  ::fc, because a write does not change what the link reads next. */
    lh_u8_t asked_fc;
    lh_u8_t state;
    /** @brief Why the last poll ended the way it did, as a ::lh_mb_status_t.
     *
     *  The state says *whether* a question produced data; this says *why not*. The
     *  two questions have different answers and an app needs both: "the device is
     *  gone" and "the device is here and this address is not on it" leave the same
     *  state behind and mean opposite things to a device that has to be polled
     *  every ten seconds for the next eight hours.
     */
    lh_mb_status_t reason;
    lh_u16_t address;   /**< what the question in flight asked for */
    lh_u16_t expected;  /**< how many words it asked for */
    lh_u16_t got;       /**< how many the answer carried */
    lh_u16_t held;      /**< bytes of a frame that has not all arrived */
    lh_u16_t tx;        /**< next transaction id, for MBAP */
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
    lh_u16_t values[LH_MB_LINK_VALUES];
    lh_byte_t frame[LH_MB_ADU_MAX];
    lh_mb_link_counters_t counters;
    /* The clock, in microseconds. A link does not read the system clock itself:
       a test has to be able to move time by hand, and an app has to be able to
       decide what a microsecond is worth. */
    lh_u64_t (*now_us)(lh_ptr self);
    lh_ptr clock;
    lh_void (*on_change)(lh_ptr self, lh_void *link);
    lh_ptr owner;
} lh_mb_link_t;

/**
 * @brief Point @p self at @p transport and start empty.
 *
 * @param self     Link to set up.
 * @param transport The four calls that move the bytes; not owned.
 * @param framing  What the line carries.
 * @param unit     Slave address, 1..247.
 * @param now_us   The link's clock; `lh_null` means the link counts nothing.
 */
lh_void
lh_mb_link_init(lh_mb_link_t *self, const lh_mb_transport_t *transport, lh_mb_framing_t framing,
                lh_u8_t unit, lh_u64_t (*now_us)(lh_ptr self), lh_ptr clock);

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

/**
 * @brief Open the transport. False when the device is not there.
 *
 * A device that is off is a state this program has, not an error: a configurator
 * with six devices on a panel has to keep drawing the five that answer.
 */
lh_bool_t
lh_mb_link_open(lh_mb_link_t *self, const lh_char_t *target, lh_u16_t port);

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
 * @brief Look for an answer without waiting for one.
 *
 * One pass over the transport, then a decision: whole frame, not yet, or not
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