/**
 * @file buf.h
 * @brief Byte sink for web page serialization (HTML and JS text).
 *
 * Construct markup / JS with their own entities, then write through this API.
 * Callers do not touch the fields — use ::lh_ui_web_buf_init / lit / raw / fail.
 *
 * Once space is checked, writes go straight to ::lh_memory_std_copy, avoiding
 * a second size clamp. On the Windows GCC 16.1 Release/LTO benchmark, 32
 * one-byte writes took a median 137 ns before and 123 ns after this change;
 * player page construction went from 18.63 to 18.04 microseconds. These are
 * machine-specific measurements, not a guarantee for every workload.
 *
 * Requires ::LH_LIBRARY_OPTION_UI.
 */

#ifndef LH_UI_WEB_BUF_H
#define LH_UI_WEB_BUF_H

#include <lh/core/bool.h>
#include <lh/core/byte/ptr.h>
#include <lh/core/str/ptr.h>
#include <lh/core/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/core/size.h>

#if !LH_LIBRARY_OPTION_UI
#    error "lh/ui/web/buf.h requires LH_LIBRARY_OPTION_UI"
#endif

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
} lh_ui_web_buf_t;   /**< Scratch that accumulates text bytes. */

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The bytes of the buffer (level 1).
 *
 * @param self The caller-owned buffer; must be valid.
 * @return The stored borrowed byte pointer; ownership stays with the caller.
 */
lh_byte_ptr
lh_ui_web_buf_get_mem(const lh_ui_web_buf_t *self);

/**
 * @brief Store the bytes of the buffer (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The caller-owned buffer; must be valid.
 * @param mem The new value.
 */
lh_void
lh_ui_web_buf_set_mem(lh_ui_web_buf_t *self, lh_byte_ptr mem);

/**
 * @brief How many bytes the buffer holds (level 1).
 *
 * @param self The caller-owned buffer; must be valid.
 * @return The stored capacity in bytes.
 */
lh_usize_t
lh_ui_web_buf_get_cap(const lh_ui_web_buf_t *self);

/**
 * @brief Store how many bytes the buffer holds (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The caller-owned buffer; must be valid.
 * @param cap The new value.
 */
lh_void
lh_ui_web_buf_set_cap(lh_ui_web_buf_t *self, lh_usize_t cap);

/**
 * @brief How many bytes are in use (level 1).
 *
 * @param self The caller-owned buffer; must be valid.
 * @return The stored number of written bytes.
 */
lh_usize_t
lh_ui_web_buf_get_len(const lh_ui_web_buf_t *self);

/**
 * @brief Store how many bytes are in use (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The caller-owned buffer; must be valid.
 * @param len The new value.
 */
lh_void
lh_ui_web_buf_set_len(lh_ui_web_buf_t *self, lh_usize_t len);

/**
 * @brief Whether every write so far fitted (level 1).
 *
 * @param self The caller-owned buffer; must be valid.
 * @return The stored success flag.
 */
lh_bool_t
lh_ui_web_buf_is_ok(const lh_ui_web_buf_t *self);

/**
 * @brief Store whether every write so far fitted (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The caller-owned buffer; must be valid.
 * @param ok The new value.
 */
lh_void
lh_ui_web_buf_set_ok(lh_ui_web_buf_t *self, lh_bool_t ok);

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

/**
 * @brief Append @p n bytes while reserving space for a trailing NUL.
 *
 * After checking available space, copies through ::lh_memory_std_copy. A
 * failed buffer, a null buffer, a null source or zero bytes writes nothing.
 * A write that does not fit marks the buffer failed and preserves its bytes
 * and length. Input and storage remain owned by the caller.
 *
 * @param self Buffer to append to, or null.
 * @param p Source bytes, or null to append nothing.
 * @param n Source byte count; @p p must contain this many readable bytes when
 *          the write fits. Source and destination must not overlap.
 */
lh_void
lh_ui_web_buf_lit(lh_ui_web_buf_t *self, const lh_str_ptr p, lh_usize_t n);

/** Write a trailing NUL without counting it in ::lh_ui_web_buf_len. */
lh_void
lh_ui_web_buf_finish(lh_ui_web_buf_t *self);

/** Append a NUL-terminated C string. */
lh_void
lh_ui_web_buf_raw(lh_ui_web_buf_t *self, const lh_str_ptr s);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_WEB_BUF_H */
