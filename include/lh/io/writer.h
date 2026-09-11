/**
 * @file writer.h
 * @brief Stream write sink: a callback plus whatever context it needs.
 *
 * Mirror of ::lh_io_reader_t for the write direction — see its doc comment
 * for why this carries a `context` where ::lh_memory_allocator_t does not.
 */

#ifndef LH_IO_WRITER_H
#define LH_IO_WRITER_H

#include <lh/attribute/symbol.h>
#include <lh/compiler/extern/c.h>
#include <lh/io/writer/cb.h>
#include <lh/io/writer/fields.h>
#include <lh/ptr.h>
#include <lh/size.h>

/**
 * @struct lh_io_writer
 * @brief A write callback paired with the context it needs to do its job.
 *
 * The concrete fields are expanded by ::lh_io_writer_fields.
 */
typedef struct lh_io_writer
{
    lh_io_writer_fields(lh_io_writer_write_fn);
} lh_io_writer_t;

LH_COMPILER_EXTERN_C_BEGIN

/* ── pack / unpack ───────────────────────────────────────────────────────── */

/**
 * @brief Pack optional callback/context pointers into writer fields.
 *
 * Only fields with non-null input pointers are updated.
 *
 * @param self     Writer object to modify.
 * @param write_cb Optional pointer to the write callback value (or ::lh_null to skip).
 * @param context  Optional pointer to the context value (or ::lh_null to skip).
 */
LH_ATTRIBUTE_SYMBOL
void
lh_io_writer_pack(lh_io_writer_t *self, lh_io_writer_write_cb *write_cb, lh_ptr *context);

/**
 * @brief Unpack writer fields into optional output pointers.
 *
 * @param self     Writer object to read from.
 * @param write_cb Optional output for the write callback (or ::lh_null to skip).
 * @param context  Optional output for the context (or ::lh_null to skip).
 */
LH_ATTRIBUTE_SYMBOL
void
lh_io_writer_unpack(const lh_io_writer_t *self, lh_io_writer_write_cb *write_cb, lh_ptr *context);

/**
 * @brief Copy the writer state from @p other into @p self.
 * @param self  Destination writer.
 * @param other Source writer.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_io_writer_assign(lh_io_writer_t *self, const lh_io_writer_t *other);

/**
 * @brief Set both the callback and the context at once.
 *
 * @param self     Writer object to modify.
 * @param write_cb New write callback.
 * @param context  New context.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_io_writer_set(lh_io_writer_t *self, lh_io_writer_write_cb write_cb, lh_ptr context);

/**
 * @brief Initialize a writer's callback and context.
 *
 * Call this (or ::lh_io_writer_set) before first use of ::lh_io_writer_write.
 *
 * @param self     Writer object to initialize.
 * @param write_cb Write callback (may be ::lh_null to leave writing disabled).
 * @param context  Context passed to @p write_cb on every call.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_io_writer_init(lh_io_writer_t *self, lh_io_writer_write_cb write_cb, lh_ptr context);

/**
 * @brief Reset a writer to empty state (callback and context both ::lh_null).
 * @param self Writer object to reset.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_io_writer_deinit(lh_io_writer_t *self);

/* ── accessors ───────────────────────────────────────────────────────────── */

/**
 * @brief Get the current write callback from @p self.
 * @param self Writer object to read from.
 * @return Current write callback (may be ::lh_null).
 */
LH_ATTRIBUTE_SYMBOL
lh_io_writer_write_cb
lh_io_writer_get_write_cb(const lh_io_writer_t *self);

/**
 * @brief Get the current context from @p self.
 * @param self Writer object to read from.
 * @return Current context (may be ::lh_null).
 */
LH_ATTRIBUTE_SYMBOL
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
 * @return Bytes actually written, or a negative value if the underlying
 *         callback reports a failure.
 */
LH_ATTRIBUTE_SYMBOL
lh_ssize_t
lh_io_writer_write(lh_io_writer_t *self, const lh_ptr buf, lh_usize_t size);

LH_COMPILER_EXTERN_C_END

#endif /* LH_IO_WRITER_H */
