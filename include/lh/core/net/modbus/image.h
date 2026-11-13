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
 */
lh_void
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
 * @brief The want whose registers include @p address (level 3: binary search).
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
 */
lh_void
lh_mb_image_store(lh_mb_image_t *self, const lh_mb_read_t *read, const lh_u16_t *values,
                  lh_u16_t count, lh_u64_t now_us);

/**
 * @brief Make @p self an image of @p count wants: copies them, lays their registers out
 *        one after another, every value unread.
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
