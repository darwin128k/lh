/**
 * @file buf.h
 * @brief Byte sink for web page serialization (HTML and JS text).
 *
 * Construct markup / JS with their own entities, then write through this API.
 * Callers do not touch the fields вЂ” use ::lh_ui_web_buf_init / lit / raw / fail.
 *
 * Requires ::LH_LIBRARY_OPTION_UI.
 */

#ifndef LH_UI_WEB_BUF_H
#define LH_UI_WEB_BUF_H

#include <lh/bool.h>
#include <lh/str/ptr.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/size.h>

#if !LH_LIBRARY_OPTION_UI
#    error "lh/ui/web/buf.h requires LH_LIBRARY_OPTION_UI"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Scratch that accumulates text bytes.
 *
 * One byte of @p cap is reserved for a trailing NUL when ::lh_ui_web_buf_finish
 * is used; writes that would not leave that byte set the buffer failed.
 */
typedef struct lh_ui_web_buf
{
    lh_byte_ptr mem; /**< The caller's bytes. Not owned. */
    lh_usize_t cap;  /**< Size of ::mem. */
    lh_usize_t len;  /**< Bytes written so far. */
    lh_bool_t ok;    /**< False once a write did not fit; later writes are no-ops. */
} lh_ui_web_buf_t; /**< Scratch that accumulates text bytes. */

/**
 * @brief Start an empty buffer over @p mem.
 * @param self Buffer to set up.
 * @param mem  Bytes to write into. Not owned.
 * @param cap  Size of @p mem.
 */
lh_void
lh_ui_web_buf_init(lh_ui_web_buf_t *self, lh_byte_ptr mem, lh_usize_t cap);

/**
 * @brief Whether every write so far fitted.
 * @param self Buffer to ask.
 * @return ::lh_bool_false once a write did not fit.
 */
lh_bool_t
lh_ui_web_buf_ok(const lh_ui_web_buf_t *self);

/**
 * @brief Bytes written so far.
 * @param self Buffer to ask.
 * @return The length, without a trailing NUL.
 */
lh_usize_t
lh_ui_web_buf_len(const lh_ui_web_buf_t *self);

/**
 * @brief The bytes written so far.
 * @param self Buffer to ask.
 * @return Start of the buffer.
 */
const lh_byte_ptr
lh_ui_web_buf_bytes(const lh_ui_web_buf_t *self);

/** Mark the buffer failed (further writes are no-ops). */
lh_void
lh_ui_web_buf_fail(lh_ui_web_buf_t *self);

/** Append @p n bytes of @p p. */
lh_void
lh_ui_web_buf_lit(lh_ui_web_buf_t *self, const lh_str_ptr p, lh_usize_t n);

/** Append a NUL-terminated C string. */
lh_void
lh_ui_web_buf_raw(lh_ui_web_buf_t *self, const lh_str_ptr s);

/** Write a trailing NUL without counting it in ::lh_ui_web_buf_len. */
lh_void
lh_ui_web_buf_finish(lh_ui_web_buf_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_WEB_BUF_H */
