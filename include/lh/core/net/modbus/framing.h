/**
 * @file framing.h
 * @brief ::lh_mb_framing_t — how a request is wrapped for the wire it goes out on.
 */

#ifndef LH_NET_MODBUS_FRAMING_H
#define LH_NET_MODBUS_FRAMING_H

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

#endif /* LH_NET_MODBUS_FRAMING_H */
