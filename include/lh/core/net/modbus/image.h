/**
 * @file image.h
 * @brief ::lh_mb_image_t — what a master knows of a device: the registers it reads,
 *        how much each value can be trusted and when it was last updated.
 *
 * A master does not keep all 65536 registers of a device: it keeps what it asks for.
 * The image is built from the same wants ::lh_mb_plan_reads plans the requests from
 * (address-ordered, non-overlapping), and holds, per want, its registers, a status
 * (::lh_mb_value_status_t) and the time of its last update on the master's clock.
 *
 * An answer is stored by the request it answers (::lh_mb_image_store): every want the
 * request covers takes its registers from the answer and becomes fresh. A request that
 * failed (::lh_mb_image_fail) **keeps the values** and marks them stale -- or refused,
 * when the device answered with an exception. A reader finds a want once, by address
 * (::lh_mb_image_find), and reads it by index from then on: no search per read.
 *
 * Storing and failing say what they changed: a want whose registers or status moved
 * is marked changed (::lh_mb_image_is_value_changed) until the reader clears it
 * (::lh_mb_image_clear_value_changed), and both return how many wants they marked. A
 * master that tells a screen "this value changed" reads that, instead of keeping a
 * second copy of every value to compare with.
 *
 * Ownership: the image owns its arrays (runtime allocator); ::lh_mb_image_try_assign
 * fills them, ::lh_mb_image_deinit frees them. Storing and reading never allocate.
 *
 * @see lh_mb_plan_reads, lh_mb_read_t, lh_mb_health_t
 */

#ifndef LH_NET_MODBUS_IMAGE_H
#define LH_NET_MODBUS_IMAGE_H

#include <lh/core/array.h>
#include <lh/core/net/modbus.h>
#include <lh/core/net/modbus/value/status.h>

/** @brief What a master knows of a device, want by want. */
typedef struct lh_mb_image
{
    lh_array_t wants;   /**< ::lh_mb_want_t: the registers asked for, address-ordered */
    lh_array_t first;   /**< lh_u32_t per want: index of its first register in ::values */
    lh_array_t values;  /**< lh_u16_t: the registers of every want, one after another */
    lh_array_t status;  /**< lh_u8_t per want: an ::lh_mb_value_status_t */
    lh_array_t updated; /**< lh_u64_t per want: when it was last stored, master's clock, µs */
    lh_array_t changed; /**< lh_u8_t per want: 1 when its value or status moved, until cleared */
} lh_mb_image_t;        /**< What a master knows of a device, want by want. */

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The wants, an array of ::lh_mb_want_t (level 1).
 *
 * @param self The image; checked by ::lh_assert_runtime_ref.
 */
const lh_array_t *
lh_mb_image_get_wants(const lh_mb_image_t *self);

/**
 * @brief The wants, to change (level 1).
 *
 * @param self The image; checked by ::lh_assert_runtime_ref.
 */
lh_array_t *
lh_mb_image_get_wants_mut(lh_mb_image_t *self);

/**
 * @brief Index of each want's first register in the values, lh_u32_t per want (level 1).
 *
 * @param self The image; checked by ::lh_assert_runtime_ref.
 */
const lh_array_t *
lh_mb_image_get_first(const lh_mb_image_t *self);

/**
 * @brief Index of each want's first register, to change (level 1).
 *
 * @param self The image; checked by ::lh_assert_runtime_ref.
 */
lh_array_t *
lh_mb_image_get_first_mut(lh_mb_image_t *self);

/**
 * @brief The registers of every want, lh_u16_t, one want after another (level 1).
 *
 * @param self The image; checked by ::lh_assert_runtime_ref.
 */
const lh_array_t *
lh_mb_image_get_values(const lh_mb_image_t *self);

/**
 * @brief The registers of every want, to change (level 1).
 *
 * @param self The image; checked by ::lh_assert_runtime_ref.
 */
lh_array_t *
lh_mb_image_get_values_mut(lh_mb_image_t *self);

/**
 * @brief The status of each want, lh_u8_t holding an ::lh_mb_value_status_t (level 1).
 *
 * @param self The image; checked by ::lh_assert_runtime_ref.
 */
const lh_array_t *
lh_mb_image_get_status(const lh_mb_image_t *self);

/**
 * @brief The status of each want, to change (level 1).
 *
 * @param self The image; checked by ::lh_assert_runtime_ref.
 */
lh_array_t *
lh_mb_image_get_status_mut(lh_mb_image_t *self);

/**
 * @brief When each want was last stored, lh_u64_t microseconds on the master's clock
 *        (level 1).
 *
 * @param self The image; checked by ::lh_assert_runtime_ref.
 */
const lh_array_t *
lh_mb_image_get_updated(const lh_mb_image_t *self);

/**
 * @brief When each want was last stored, to change (level 1).
 *
 * @param self The image; checked by ::lh_assert_runtime_ref.
 */
lh_array_t *
lh_mb_image_get_updated_mut(lh_mb_image_t *self);

/**
 * @brief Whether each want changed, lh_u8_t per want (level 1).
 *
 * @param self The image; checked by ::lh_assert_runtime_ref.
 */
const lh_array_t *
lh_mb_image_get_changed(const lh_mb_image_t *self);

/**
 * @brief Whether each want changed, to change (level 1).
 *
 * @param self The image; checked by ::lh_assert_runtime_ref.
 */
lh_array_t *
lh_mb_image_get_changed_mut(lh_mb_image_t *self);

/**
 * @brief How many wants the image holds (level 2).
 *
 * @param self The image; checked by ::lh_assert_runtime_ref.
 */
lh_u16_t
lh_mb_image_get_count(const lh_mb_image_t *self);

/**
 * @brief Want @p index (level 2).
 *
 * @param self  The image; checked by ::lh_assert_runtime_ref.
 * @param index Below ::lh_mb_image_get_count; not checked.
 */
const lh_mb_want_t *
lh_mb_image_get_want(const lh_mb_image_t *self, lh_u16_t index);

/**
 * @brief The registers of want @p index, as many as it has (level 2).
 *
 * @param self  The image; checked by ::lh_assert_runtime_ref.
 * @param index Below ::lh_mb_image_get_count; not checked.
 *
 * @return Its first register; the rest follow it. Meaningful only when the status
 *         is ::lh_mb_value_fresh or ::lh_mb_value_stale.
 */
const lh_u16_t *
lh_mb_image_get_value(const lh_mb_image_t *self, lh_u16_t index);

/**
 * @brief The status of want @p index (level 2).
 *
 * @param self  The image; checked by ::lh_assert_runtime_ref.
 * @param index Below ::lh_mb_image_get_count; not checked.
 */
lh_mb_value_status_t
lh_mb_image_get_value_status(const lh_mb_image_t *self, lh_u16_t index);

/**
 * @brief When want @p index was last stored, microseconds on the master's clock; 0
 *        when never (level 2).
 *
 * @param self  The image; checked by ::lh_assert_runtime_ref.
 * @param index Below ::lh_mb_image_get_count; not checked.
 */
lh_u64_t
lh_mb_image_get_updated_us(const lh_mb_image_t *self, lh_u16_t index);

/**
 * @brief Whether the registers or the status of want @p index moved since it was last
 *        cleared (level 2).
 *
 * @param self  The image; checked by ::lh_assert_runtime_ref.
 * @param index Below ::lh_mb_image_get_count; not checked.
 */
lh_bool_t
lh_mb_image_is_value_changed(const lh_mb_image_t *self, lh_u16_t index);

/**
 * @brief The change of want @p index is seen: it is not changed until it moves again
 *        (level 2).
 *
 * @param self  The image; checked by ::lh_assert_runtime_ref.
 * @param index Below ::lh_mb_image_get_count; not checked.
 */
lh_void
lh_mb_image_clear_value_changed(lh_mb_image_t *self, lh_u16_t index);

/**
 * @brief Mark want @p index changed, whatever it was (level 2).
 *
 * @param self  The image; checked by ::lh_assert_runtime_ref.
 * @param index Below ::lh_mb_image_get_count; not checked.
 */
lh_void
lh_mb_image_mark_value_changed(lh_mb_image_t *self, lh_u16_t index);

/**
 * @brief Mark want @p index changed when its status becomes @p status, and set it
 *        (level 3).
 *
 * @param self   The image; checked by ::lh_assert_runtime_ref.
 * @param index  Below ::lh_mb_image_get_count; not checked.
 * @param status The new status.
 *
 * @return 1 when the status moved, 0 when it was @p status already.
 */
lh_u16_t
lh_mb_image_set_value_status(lh_mb_image_t *self, lh_u16_t index, lh_mb_value_status_t status);

/**
 * @brief Request @p read failed: every want it covers keeps its value and becomes
 *        stale, or refused.
 *
 * With ::lh_mb_value_stale a want that holds a value (fresh or stale) is stale; one
 * that was never read stays unread -- there is no value to be old. With
 * ::lh_mb_value_refused every want becomes refused: the device answered that it will
 * not give these registers.
 *
 * @param self   The image; checked by ::lh_assert_runtime_ref.
 * @param read   The request, planned over this image's wants.
 * @param status ::lh_mb_value_stale or ::lh_mb_value_refused.
 *
 * @return How many wants changed status (::lh_mb_image_is_value_changed).
 */
lh_u16_t
lh_mb_image_fail(lh_mb_image_t *self, const lh_mb_read_t *read, lh_mb_value_status_t status);

/**
 * @brief Start empty: no wants, nothing allocated.
 *
 * @param self The image; checked by ::lh_assert_runtime_ref.
 */
lh_void
lh_mb_image_init(lh_mb_image_t *self);

/**
 * @brief Free what the image holds; it is empty and may be assigned again.
 *
 * @param self The image; checked by ::lh_assert_runtime_ref.
 */
lh_void
lh_mb_image_deinit(lh_mb_image_t *self);

/**
 * @brief The first want that ends past @p address: the one that covers it, or the
 *        next one after it (level 3: binary search).
 *
 * @param self    The image; checked by ::lh_assert_runtime_ref.
 * @param address A register address.
 *
 * @return Its index, or ::lh_mb_image_get_count when every want ends at or before
 *         @p address.
 */
lh_u16_t
lh_mb_image_find_from(const lh_mb_image_t *self, lh_u16_t address);

/**
 * @brief The want whose registers include @p address (level 4).
 *
 * @param self    The image; checked by ::lh_assert_runtime_ref.
 * @param address A register address.
 *
 * @return Its index, or ::lh_mb_image_get_count when no want covers @p address.
 */
lh_u16_t
lh_mb_image_find(const lh_mb_image_t *self, lh_u16_t address);

/**
 * @brief Store the answer @p values to request @p read: every want it covers takes its
 *        registers from the answer, becomes ::lh_mb_value_fresh and is stamped @p now_us.
 *
 * A want the answer is too short for (@p count less than the request asked) is not
 * stored; it is marked as by ::lh_mb_image_fail with ::lh_mb_value_stale.
 *
 * @param self   The image; checked by ::lh_assert_runtime_ref.
 * @param read   The request, planned by ::lh_mb_plan_reads over this image's wants.
 * @param values The words of the answer, first register of the request first.
 * @param count  How many words the answer carried.
 * @param now_us The master's clock.
 *
 * @return How many wants changed, registers or status (::lh_mb_image_is_value_changed).
 */
lh_u16_t
lh_mb_image_store(lh_mb_image_t *self, const lh_mb_read_t *read, const lh_u16_t *values,
                  lh_u16_t count, lh_u64_t now_us);

/**
 * @brief The request that stores @p count registers from @p address: it covers every
 *        want lying wholly inside them (level 4).
 *
 * What a master read or wrote outside its plan -- the read-back that checks a write,
 * the answer to a gateway client -- refreshes the image too: with this request,
 * ::lh_mb_image_store takes the words of that answer. A want only partly inside is
 * left alone: half a 32-bit value is not a value.
 *
 * @param self    The image; checked by ::lh_assert_runtime_ref.
 * @param address The first register of the answer.
 * @param count   How many registers it carries.
 * @param read    Where the request goes; checked by ::lh_assert_runtime_ref.
 *
 * @return ::lh_bool_false when no want lies wholly inside; @p read is untouched then.
 */
lh_bool_t
lh_mb_image_try_copy_read(const lh_mb_image_t *self, lh_u16_t address, lh_u16_t count,
                          lh_mb_read_t *read);

/**
 * @brief Make @p self an image of @p count wants: copies them, lays their registers out
 *        one after another, every value unread and not changed.
 *
 * @param self  The image, initialized; checked by ::lh_assert_runtime_ref.
 * @param wants Address-ordered, non-overlapping, as ::lh_mb_plan_reads takes them.
 * @param count How many.
 *
 * @return ::lh_bool_false when memory runs out; @p self is then empty.
 */
lh_bool_t
lh_mb_image_try_assign(lh_mb_image_t *self, const lh_mb_want_t *wants, lh_u16_t count);

LH_COMPILER_EXTERN_C_END

#endif /* LH_NET_MODBUS_IMAGE_H */
