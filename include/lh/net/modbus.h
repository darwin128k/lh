/**
 * @file modbus.h
 * @brief Modbus RTU: the wire, and how to ask for a lot of it at once.
 *
 * A configurator does not read a handful of registers. A device card is a few
 * hundred of them, several devices answer at once, and there is no real device on
 * the desk to develop against — so the two things worth getting right here are the
 * ones that do not need one:
 *
 * - **the wire** — build a request, parse a response, check the CRC. All of it
 *   arithmetic over bytes, all of it testable with a device that never existed;
 * - **the plan** — a card is address-ordered, and consecutive registers that are
 *   wanted with the same function code can be asked for in **one** request
 *   instead of one each (::lh_mb_plan_reads). This is where the speed comes from
 *   and it is decided by the map, not by the loop: a hundred scattered registers
 *   become tens of requests, and a device is asked less often than a naive
 *   register-at-a-time reader asks it.
 *
 * Nothing here talks to anything. A transport (::lh_mb_transport_t) hands bytes in
 * and gets bytes out, so the same plan runs over TCP, over a COM port, or over a
 * pair of arrays in a test.
 *
 * @see lh_mb_plan_reads, lh_mb_crc16, lh_mb_link
 */

#ifndef LH_NET_MODBUS_H
#define LH_NET_MODBUS_H

#include <lh/bool.h>
#include <lh/byte.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/fixed/types.h>
#include <lh/void.h>

LH_COMPILER_EXTERN_C_BEGIN

/* ── The wire ────────────────────────────────────────────────────────────── */

/** @brief Function code: read holding registers. */
#define LH_MB_FC_READ_HOLDING ((lh_u8_t)0x03)
/** @brief Function code: read input registers. */
#define LH_MB_FC_READ_INPUT ((lh_u8_t)0x04)
/** @brief Function code: write one register. */
#define LH_MB_FC_WRITE_ONE ((lh_u8_t)0x06)
/** @brief Function code: write many registers. */
#define LH_MB_FC_WRITE_MANY ((lh_u8_t)0x10)

/** @brief Unit 0: every unit on the line is meant, and nobody answers. */
#define LH_MB_UNIT_BROADCAST ((lh_u8_t)0x00)

/**
 * @brief Longest ADU, the whole frame in whichever envelope it travels.
 *
 * The widest answer a link can ask for is a read of ::LH_MB_READ_MAX registers, and
 * its two framings do **not** come out the same size:
 *
 * - **RTU** — unit, function code, byte count, 250 data bytes, CRC low byte first:
 *   1 + 1 + 1 + 250 + 2 = **255**;
 * - **Modbus TCP** — transaction id, protocol id, length, unit, then the same PDU
 *   with no CRC at all: 7 + 1 + 1 + 250 = **259**.
 *
 * The larger one is what a buffer has to be. It used to be 256, which is the RTU
 * answer with a round number added and is **too small for TCP**: a full 125-register
 * read over a network is a 259-byte frame, `recv` is asked for at most 256 of it, the
 * last three bytes are never requested and so never arrive, and the request waits out
 * its timeout on a device that had already answered perfectly. Nothing crashes and
 * nothing is logged: the answer is simply never completed, which looks exactly like a
 * device that stopped answering halfway.
 *
 * So 260, with the arithmetic above rather than a remembered number — the figure 256
 * is the number that was remembered, and it was wrong for one of the two framings this
 * library speaks.
 */
#define LH_MB_ADU_MAX ((lh_u16_t)260)

/**
 * @brief Most registers one read may ask for.
 *
 * 125 is the protocol's own limit (0x7D), and it is also the widest answer that
 * fits a 256-byte frame once the unit, the code, the byte count and the CRC are
 * taken off it.
 */
#define LH_MB_READ_MAX ((lh_u16_t)125)

/**
 * @brief CRC-16 as Modbus uses it: reflected, polynomial 0xA001.
 *
 * The one piece of arithmetic every frame depends on, and the one a device that
 * answers anyway will notice first.
 */
lh_u16_t
lh_mb_crc16(const lh_byte_t *bytes, lh_u16_t length);

/**
 * @brief Build a read request into @p adu.
 *
 * @param adu   Buffer for the whole frame, CRC included.
 * @param cap   How much @p adu really has.
 * @param unit  Slave address, 1..247, or ::LH_MB_UNIT_BROADCAST.
 * @param fc    ::LH_MB_FC_READ_HOLDING or ::LH_MB_FC_READ_INPUT.
 * @param address First register.
 * @param count How many, 1..::LH_MB_READ_MAX.
 *
 * @return Length of the frame in bytes, 0 when it does not fit or the request is
 *         not one Modbus allows. A request that cannot be sent is not an error the
 *         caller has to distinguish from one that can: both mean "do not send
 *         this", and the plan below never builds one.
 */
lh_u16_t
lh_mb_build_read(lh_byte_t *adu, lh_u16_t cap, lh_u8_t unit, lh_u8_t fc, lh_u16_t address,
                 lh_u16_t count);

/**
 * @brief Build a "write many registers" request into @p adu.
 *
 * @param adu    Buffer for the whole frame, CRC included.
 * @param cap    How much @p adu really has.
 * @param unit   Slave address, 1..247.
 * @param address First register.
 * @param values What to write, first register first.
 * @param count  How many.
 *
 * @return Length of the frame in bytes, 0 when it does not fit.
 */
lh_u16_t
lh_mb_build_write(lh_byte_t *adu, lh_u16_t cap, lh_u8_t unit, lh_u16_t address,
                  const lh_u16_t *values, lh_u16_t count);

/** @brief What came back. */
typedef enum lh_mb_status
{
    lh_mb_status_ok = 0,     /**< the frame is an answer to what was asked */
    lh_mb_status_partial,    /**< a good frame, but with fewer words than were asked for */
    lh_mb_status_short,      /**< not a whole frame yet: ask for the rest */
    lh_mb_status_crc,        /**< a whole frame with the wrong CRC: the line is noisy */
    lh_mb_status_exception,  /**< the device understood and refused; `exception` says how */
    lh_mb_status_wrong_unit, /**< somebody else's answer on a shared line */
    lh_mb_status_wrong_code, /**< a different question than the one that was sent */
    lh_mb_status_garbage     /**< a whole frame that is not an answer at all */
} lh_mb_status_t;

/**
 * @brief Read what a device answered into @p values.
 *
 * @param adu      The frame as it came off the wire.
 * @param length   How many bytes @p adu holds.
 * @param unit     The unit that was asked.
 * @param fc       The function code that was sent.
 * @param expect   How many registers were asked for.
 * @param values   Where the words go.
 * @param cap      How many words @p values really holds.
 * @param out      How many words arrived; `lh_null` to not care.
 * @param exception The device's exception code, when it refused.
 *
 * @return What came back.
 *
 * @note A frame for another unit is not an error and not garbage: a line carries
 *       every unit's traffic, and a device that is not ours answering means
 *       somebody else is on the bus. Treating it as a failure is what makes a bus
 *       with eight devices look like eight broken ones.
 */
/* The PDU — function code and data, no unit and no CRC — is the part of a frame
   that is the same on every line, and it is what `lh/net/modbus/link.h` hands to
   whichever framing it has. The three below are the PDU on its own; the three after
   them are the same work with the envelope still on. */

/**
 * @brief Build a read request **PDU** — function code and data, no unit, no CRC.
 *
 * @param pdu    Where the PDU goes.
 * @param cap    How much @p pdu really has; 6 bytes is enough for a read.
 * @param fc     ::LH_MB_FC_READ_HOLDING or ::LH_MB_FC_READ_INPUT.
 * @param address First register.
 * @param count  How many, 1..::LH_MB_READ_MAX.
 *
 * @return Length of the PDU, 0 when it does not fit or is not one Modbus allows.
 */
lh_u16_t
lh_mb_build_pdu_read(lh_byte_t *pdu, lh_u16_t cap, lh_u8_t fc, lh_u16_t address, lh_u16_t count);

/**
 * @brief Build a "write many registers" **PDU** — no unit, no CRC.
 */
lh_u16_t
lh_mb_build_pdu_write(lh_byte_t *pdu, lh_u16_t cap, lh_u16_t address, const lh_u16_t *values,
                      lh_u16_t count);

/**
 * @brief Read what a device answered, from the **PDU** of its answer.
 *
 * No unit and no CRC here: those belong to the framing and have been checked by the
 * time a PDU gets this far (::lh_mb_decode). ::lh_mb_parse_read is this plus the
 * envelope, and takes a frame that still has both.
 *
 * @param pdu       Function code and data, as it came off the wire.
 * @param length    How many bytes @p pdu holds.
 * @param fc        The function code that was sent.
 * @param expect    How many registers were asked for.
 * @param values    Where the words go.
 * @param cap       How many words @p values really holds.
 * @param out       How many words arrived; `lh_null` to not care.
 * @param exception The device's exception code, when it refused.
 *
 * @return What came back. @see lh_mb_status_t
 */
lh_mb_status_t
lh_mb_parse_pdu(const lh_byte_t *pdu, lh_u16_t length, lh_u8_t fc, lh_u16_t expect, lh_u16_t *values,
                lh_u16_t cap, lh_u16_t *out, lh_u8_t *exception);

lh_mb_status_t
lh_mb_parse_read(const lh_byte_t *adu, lh_u16_t length, lh_u8_t unit, lh_u8_t fc, lh_u16_t expect,
                 lh_u16_t *values, lh_u16_t cap, lh_u16_t *out, lh_u8_t *exception);

/**
 * @brief Read what a device answered to "write many registers", from the **PDU**.
 *
 * The answer to a write is its own header echoed: the function code, the address and
 * the count, five bytes. It is checked against what was sent, because an echo of a
 * different address is not this write being done -- or a refusal, two bytes, exactly
 * as for a read.
 *
 * @param pdu       Function code and data, as it came off the wire.
 * @param length    How many bytes @p pdu holds.
 * @param address   The address that was written.
 * @param count     How many registers were written.
 * @param exception The device's exception code, when it refused; `lh_null` to not care.
 *
 * @return ::lh_mb_status_ok when the echo is this write's. @see lh_mb_status_t
 */
lh_mb_status_t
lh_mb_parse_pdu_write(const lh_byte_t *pdu, lh_u16_t length, lh_u16_t address, lh_u16_t count,
                      lh_u8_t *exception);

/* ── The plan ────────────────────────────────────────────────────────────── */

/**
 * @brief One register somebody wants.
 *
 * Address-ordered, which is what makes coalescing a single pass and what
 * ::lh_cfg_map_t already gives: a device map is sorted by address because a poll
 * reads it in order.
 */
typedef struct lh_mb_want
{
    lh_u16_t address;   /**< first register wanted */
    lh_u16_t registers; /**< how many from there */
} lh_mb_want_t;

/**
 * @brief One request that answers for a run of wants.
 */
typedef struct lh_mb_read
{
    lh_u16_t start; /**< first register this request asks for */
    lh_u16_t count; /**< how many registers it asks for */
    lh_u16_t first; /**< index into the wants this one starts at */
    lh_u16_t last;  /**< index of the last want it covers */
} lh_mb_read_t;

/**
 * @brief How many registers of a gap to cross for the sake of a wanted one.
 *
 * A device sends every register between two ends whether or not anybody asked for
 * it, so a run of two registers is one request either way and crossing a gap of
 * eight is free. Crossing two hundred to save one request is not: the words are
 * received, copied, checked and thrown away, and that is the whole cost this
 * program is trying to avoid.
 *
 * `8` is where it was worth measuring, and the measurement is the number to move
 * if a card turns out to be denser than the ones in the firmware.
 */
#define LH_MB_GAP_MAX ((lh_u16_t)8)

/**
 * @brief Turn a sorted list of wants into the fewest requests that cover them.
 *
 * A run of wants merges into one request when the run does not cross more than
 * ::LH_MB_GAP_MAX unwanted registers and does not go over ::LH_MB_READ_MAX. Both
 * ends are the protocol's and the wire's, and a plan that broke either of them
 * would be a plan that sends a request the device refuses.
 *
 * @param wants The registers, address-ordered and non-overlapping.
 * @param count How many there are.
 * @param reads Where the requests go.
 * @param cap   How many @p reads really holds.
 *
 * @return How many requests were produced, 0 when there is nothing to ask or no
 *         room to ask it in.
 *
 * @note Overlapping wants are the caller's problem to avoid: a merged request that
 *       covers two wants for the same register would store the second answer over
 *       the first, and the value that survives is the one that was asked for
 *       twice.
 */
lh_u16_t
lh_mb_plan_reads(const lh_mb_want_t *wants, lh_u16_t count, lh_mb_read_t *reads, lh_u16_t cap);

/**
 * @brief How many requests a list of wants would need, without building them.
 *
 * The number to put in front of a customer, and the one to check against
 * ::lh_mb_plan_reads — a plan that returns more reads than this says are two paths
 * that disagree, and a plan that returns fewer is dropping something.
 */
lh_u16_t
lh_mb_count_requests(const lh_mb_want_t *wants, lh_u16_t count);

LH_COMPILER_EXTERN_C_END

#endif /* LH_NET_MODBUS_H */