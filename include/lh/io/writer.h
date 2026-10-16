/**
 * @file writer.h
 * @brief Stream write sink: a callback plus whatever context it needs.
 *
 * Mirror of ::lh_io_reader_t for the write direction — see its doc comment
 * for why this carries a `context` where ::lh_memory_allocator_t does not.
 */

#ifndef LH_IO_WRITER_H
#define LH_IO_WRITER_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/io/pending.h>
#include <lh/io/writer/cb.h>
#include <lh/io/writer/fields.h>
#include <lh/str/ptr.h>

#include <stdarg.h>

/**
 * @def LH_IO_WRITER_PRINT_CAPACITY
 * @brief Stack buffer for ::lh_io_writer_print / ::lh_runtime_writer_print.
 *
 * ::lh_str_ptr_format_text is all-or-nothing: if the formatted line does not
 * fit, nothing is written and the call fails. Override before including this
 * header when a host needs a larger line.
 */
#ifndef LH_IO_WRITER_PRINT_CAPACITY
#    define LH_IO_WRITER_PRINT_CAPACITY 256u
#endif

/**
 * @struct lh_io_writer
 * @brief A write callback paired with the context it needs to do its job.
 *
 * The concrete fields are expanded by ::lh_io_writer_fields.
 */
struct lh_io_writer
{
    /** Members: ::lh_io_writer_fields. */
    lh_io_writer_fields(lh_io_writer_write_fn);
};
/** @copybrief lh_io_writer */
typedef struct lh_io_writer lh_io_writer_t;

LH_COMPILER_EXTERN_C_BEGIN

/* ── set / init ──────────────────────────────────────────────────────────── */

/**
 * @brief Copy the writer state from @p other into @p self.
 * @param self  Destination writer.
 * @param other Source writer.
 */
lh_void
lh_io_writer_assign(lh_io_writer_t *self, const lh_io_writer_t *other);

/**
 * @brief Set both the callback and the context at once.
 *
 * @param self     Writer object to modify.
 * @param write_cb New write callback.
 * @param context  New context.
 */
lh_void
lh_io_writer_set(lh_io_writer_t *self, lh_io_writer_write_cb write_cb, lh_ptr context);

/**
 * @brief Write only the write callback.
 */
lh_void
lh_io_writer_set_write_cb(lh_io_writer_t *self, lh_io_writer_write_cb write_cb);

/**
 * @brief Write only the context.
 */
lh_void
lh_io_writer_set_context(lh_io_writer_t *self, lh_ptr context);

/**
 * @brief Initialize a writer's callback and context.
 *
 * Call this (or ::lh_io_writer_set) before first use of ::lh_io_writer_write.
 *
 * @param self     Writer object to initialize.
 * @param write_cb Write callback (may be ::lh_null to leave writing disabled).
 * @param context  Context passed to @p write_cb on every call.
 */
lh_void
lh_io_writer_init(lh_io_writer_t *self, lh_io_writer_write_cb write_cb, lh_ptr context);

/**
 * @brief Reset a writer to empty state (callback and context both ::lh_null).
 * @param self Writer object to reset.
 */
lh_void
lh_io_writer_deinit(lh_io_writer_t *self);

/* ── accessors ───────────────────────────────────────────────────────────── */

/**
 * @brief Get the current write callback from @p self.
 * @param self Writer object to read from.
 * @return Current write callback (may be ::lh_null).
 */
lh_io_writer_write_cb
lh_io_writer_get_write_cb(const lh_io_writer_t *self);

/**
 * @brief Get the current context from @p self.
 * @param self Writer object to read from.
 * @return Current context (may be ::lh_null).
 */
lh_ptr
lh_io_writer_get_context(const lh_io_writer_t *self);

/* ── operation ───────────────────────────────────────────────────────────── */

/**
 * @brief Write @p size bytes from @p buf via @p self's callback.
 *
 * Fails with a runtime check when the callback is not initialized.
 *
 * @param self Writer to write to.
 * @param buf  Source buffer.
 * @param size Number of bytes from @p buf to write.
 *
 * @return Bytes actually written, ::LH_IO_PENDING when a non-blocking sink can
 *         take nothing yet, or another negative value if the underlying callback
 *         reports a failure.
 */
lh_ssize_t
lh_io_writer_write(lh_io_writer_t *self, const lh_ptr buf, lh_usize_t size);

/**
 * @brief Write every byte of @p buf by repeating ::lh_io_writer_write.
 *
 * The one-shot write may take less than @p size (a socket, a short OS
 * write). This is that loop, once, so a file dump and a stream do not each
 * grow their own. ::LH_IO_PENDING and any other non-positive result stop
 * it: a blocking dump has no "try later".
 *
 * @return ::lh_bool_true when all @p size bytes were taken (including
 *         @p size of 0).
 */
lh_bool_t
lh_io_writer_write_all(lh_io_writer_t *self, const lh_ptr buf, lh_usize_t size);

/**
 * @brief Format @p fmt with @p args via ::lh_str_ptr_format_text_v and write
 *        the result through @p self.
 *
 * Same conversions as ::lh_str_ptr_format_text. Uses a
 * ::LH_IO_WRITER_PRINT_CAPACITY stack buffer — oversized or malformed masks
 * fail without writing.
 *
 * @return Characters written, or ::LH_IO_ERROR on format / write failure.
 */
lh_ssize_t
lh_io_writer_print_v(lh_io_writer_t *self, const lh_str_ptr fmt, va_list args);

/**
 * @brief Variadic wrapper for ::lh_io_writer_print_v.
 */
lh_ssize_t
lh_io_writer_print(lh_io_writer_t *self, const lh_str_ptr fmt, ...);

LH_COMPILER_EXTERN_C_END

#endif /* LH_IO_WRITER_H */
