/**
 * @file ws.h
 * @brief WebSocket frames (RFC 6455), no TCP and no TLS.
 *
 * The host speaks these bytes on a socket. `wss` is the same codec on a TLS
 * stream; this file does not do TLS. XP never loads the player.
 *
 * Requires ::LH_LIBRARY_OPTION_UI.
 */

#ifndef LH_UI_WEB_WS_H
#define LH_UI_WEB_WS_H

#include <lh/bool.h>
#include <lh/byte/ptr.h>
#include <lh/compiler/extern/c.h>
#include <lh/util/ptr.h>
#include <lh/config.h>
#include <lh/size.h>

#if !LH_LIBRARY_OPTION_UI
#    error "lh/ui/web/ws.h requires LH_LIBRARY_OPTION_UI"
#endif

/** @brief WebSocket opcode: continuation of a fragmented message. */
#define LH_UI_WEB_WS_CONT 0u
/** @brief WebSocket opcode: text frame. */
#define LH_UI_WEB_WS_TEXT 1u
/** @brief WebSocket opcode: binary frame. */
#define LH_UI_WEB_WS_BIN 2u
/** @brief WebSocket opcode: close. */
#define LH_UI_WEB_WS_CLOSE 8u
/** @brief WebSocket opcode: ping. */
#define LH_UI_WEB_WS_PING 9u
/** @brief WebSocket opcode: pong. */
#define LH_UI_WEB_WS_PONG 10u

/** Largest payload the decoder will assemble (1 MiB). */
#define LH_UI_WEB_WS_MAX_MSG (lh_cast_static(lh_usize_t, (1024u * 1024u)))

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Bytes of an unmasked server frame with @p payload_len bytes of data.
 */
lh_usize_t
lh_ui_web_ws_frame_size(lh_usize_t payload_len);

/**
 * @brief Write an unmasked frame (server → client) into @p out.
 *
 * @return Bytes written, or 0 if @p cap is too small or @p opcode is not a frame.
 */
lh_usize_t
lh_ui_web_ws_write(lh_byte_ptr out, lh_usize_t cap, unsigned opcode, lh_bool_t fin,
                   const void *payload, lh_usize_t payload_len);

/**
 * @brief Parse one frame at @p p.
 *
 * @return Frame size in bytes; 0 if @p n is a prefix; ::lh_usize_t max (all bits)
 *         encoded as @c (lh_usize_t)-1 on error (too big, truncated length, bad opcode).
 *
 * @p payload is inside @p p (after the header / mask). If the frame is masked,
 * call ::lh_ui_web_ws_unmask on that span. Continuation frames have
 * opcode ::LH_UI_WEB_WS_CONT.
 */
lh_usize_t
lh_ui_web_ws_read(const lh_byte_ptr p, lh_usize_t n, unsigned *opcode, lh_bool_t *fin,
                  const lh_byte_ptr *payload, lh_usize_t *payload_len, const lh_byte_ptr *mask);

/**
 * @brief XOR @p len bytes of @p data with the 4-byte @p mask (RFC masking).
 */
lh_void
lh_ui_web_ws_unmask(lh_byte_ptr data, lh_usize_t len, const lh_byte_t mask[4]);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_WEB_WS_H */
