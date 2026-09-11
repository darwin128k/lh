/**
 * @file reader.h
 * @brief Stream read source: a callback plus whatever context it needs,
 *        wired up so any concrete byte source (socket, file, ...) can hand
 *        callers an ::lh_io_reader_t without those callers knowing which.
 *
 * Same shape as ::lh_memory_allocator_t (pack/unpack/set/init/deinit/get_*),
 * except the callback itself carries a `context` — a memory allocator is
 * global/stateless like `malloc`, but a reader has to say *which* stream
 * it's reading, since there can be many open at once.
 */

#ifndef LH_IO_READER_H
#define LH_IO_READER_H

#include <lh/attribute/symbol.h>
#include <lh/compiler/extern/c.h>
#include <lh/io/reader/cb.h>
#include <lh/io/reader/fields.h>
#include <lh/ptr.h>
#include <lh/size.h>

/**
 * @struct lh_io_reader
 * @brief A read callback paired with the context it needs to do its job.
 *
 * The concrete fields are expanded by ::lh_io_reader_fields.
 */
typedef struct lh_io_reader
{
    lh_io_reader_fields(lh_io_reader_read_fn);
} lh_io_reader_t;

LH_COMPILER_EXTERN_C_BEGIN

/* ── pack / unpack ───────────────────────────────────────────────────────── */

/**
 * @brief Pack optional callback/context pointers into reader fields.
 *
 * Only fields with non-null input pointers are updated.
 *
 * @param self    Reader object to modify.
 * @param read_cb Optional pointer to the read callback value (or ::lh_null to skip).
 * @param context Optional pointer to the context value (or ::lh_null to skip).
 */
LH_ATTRIBUTE_SYMBOL
void
lh_io_reader_pack(lh_io_reader_t *self, lh_io_reader_read_cb *read_cb, lh_ptr *context);

/**
 * @brief Unpack reader fields into optional output pointers.
 *
 * @param self    Reader object to read from.
 * @param read_cb Optional output for the read callback (or ::lh_null to skip).
 * @param context Optional output for the context (or ::lh_null to skip).
 */
LH_ATTRIBUTE_SYMBOL
void
lh_io_reader_unpack(const lh_io_reader_t *self, lh_io_reader_read_cb *read_cb, lh_ptr *context);

/**
 * @brief Copy the reader state from @p other into @p self.
 * @param self  Destination reader.
 * @param other Source reader.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_io_reader_assign(lh_io_reader_t *self, const lh_io_reader_t *other);

/**
 * @brief Set both the callback and the context at once.
 *
 * @param self    Reader object to modify.
 * @param read_cb New read callback.
 * @param context New context.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_io_reader_set(lh_io_reader_t *self, lh_io_reader_read_cb read_cb, lh_ptr context);

/**
 * @brief Initialize a reader's callback and context.
 *
 * Call this (or ::lh_io_reader_set) before first use of ::lh_io_reader_read.
 *
 * @param self    Reader object to initialize.
 * @param read_cb Read callback (may be ::lh_null to leave reading disabled).
 * @param context Context passed to @p read_cb on every call.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_io_reader_init(lh_io_reader_t *self, lh_io_reader_read_cb read_cb, lh_ptr context);

/**
 * @brief Reset a reader to empty state (callback and context both ::lh_null).
 * @param self Reader object to reset.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_io_reader_deinit(lh_io_reader_t *self);

/* ── accessors ───────────────────────────────────────────────────────────── */

/**
 * @brief Get the current read callback from @p self.
 * @param self Reader object to read from.
 * @return Current read callback (may be ::lh_null).
 */
LH_ATTRIBUTE_SYMBOL
lh_io_reader_read_cb
lh_io_reader_get_read_cb(const lh_io_reader_t *self);

/**
 * @brief Get the current context from @p self.
 * @param self Reader object to read from.
 * @return Current context (may be ::lh_null).
 */
LH_ATTRIBUTE_SYMBOL
lh_ptr
lh_io_reader_get_context(const lh_io_reader_t *self);

/* ── operation ───────────────────────────────────────────────────────────── */

/**
 * @brief Read up to @p size bytes into @p buf via @p self's callback.
 *
 * Fails with a runtime check when the callback is not initialized.
 *
 * @param self Reader to read from.
 * @param buf  Destination buffer.
 * @param size Maximum bytes to read into @p buf.
 *
 * @return Bytes actually read (`0` at end of stream), or a negative value
 *         if the underlying callback reports a failure.
 */
LH_ATTRIBUTE_SYMBOL
lh_ssize_t
lh_io_reader_read(lh_io_reader_t *self, lh_ptr buf, lh_usize_t size);

LH_COMPILER_EXTERN_C_END

#endif /* LH_IO_READER_H */
