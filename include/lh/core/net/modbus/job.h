/**
 * @file job.h
 * @brief ::lh_mb_job_t — one request for a line: what to ask or write, and who asked.
 *
 * A line carries one request at a time (::lh_mb_bus_t), and requests come from more
 * than one place: a poll that refreshes values, a screen that writes a setting, a
 * client of a gateway. A job is such a request written down, so that it can wait in a
 * queue (::lh_mb_job_queue_t) and be sent when the line is free.
 *
 * A job names the unit, the function (::LH_MB_FC_READ_HOLDING, ::LH_MB_FC_READ_INPUT,
 * ::LH_MB_FC_WRITE_ONE or ::LH_MB_FC_WRITE_MANY), the first register and how many, and
 * for a write the words to write. ::owner and ::ticket belong to the submitter: the
 * line never reads them, and they come back with the outcome so the submitter knows
 * whose request it was -- a gateway puts the client's connection in ::owner and the
 * client's transaction id in ::ticket.
 */

#ifndef LH_NET_MODBUS_JOB_H
#define LH_NET_MODBUS_JOB_H

#include <lh/core/net/modbus.h>

/** @brief One request for a line. */
typedef struct lh_mb_job
{
    lh_u32_t owner;                   /**< who asked; not read by the line */
    lh_u32_t ticket;                  /**< what the owner knows the job by; not read by the line */
    lh_u8_t unit;                     /**< the unit the request is for */
    lh_u8_t fc;                       /**< the function code */
    lh_u16_t address;                 /**< the first register */
    lh_u16_t count;                   /**< how many registers to read or write */
    lh_u16_t values[LH_MB_WRITE_MAX]; /**< what a write writes, the first ::count of them */
} lh_mb_job_t;                        /**< One request for a line. */

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Who asked (level 1).
 *
 * @param self The job; checked by ::lh_assert_runtime_ref.
 */
lh_u32_t
lh_mb_job_get_owner(const lh_mb_job_t *self);

/**
 * @brief Store who asked (level 1).
 *
 * @param self  The job; checked by ::lh_assert_runtime_ref.
 * @param owner The new value.
 */
lh_void
lh_mb_job_set_owner(lh_mb_job_t *self, lh_u32_t owner);

/**
 * @brief What the owner knows the job by (level 1).
 *
 * @param self The job; checked by ::lh_assert_runtime_ref.
 */
lh_u32_t
lh_mb_job_get_ticket(const lh_mb_job_t *self);

/**
 * @brief Store what the owner knows the job by (level 1).
 *
 * @param self   The job; checked by ::lh_assert_runtime_ref.
 * @param ticket The new value.
 */
lh_void
lh_mb_job_set_ticket(lh_mb_job_t *self, lh_u32_t ticket);

/**
 * @brief The unit the request is for (level 1).
 *
 * @param self The job; checked by ::lh_assert_runtime_ref.
 */
lh_u8_t
lh_mb_job_get_unit(const lh_mb_job_t *self);

/**
 * @brief Store the unit the request is for (level 1).
 *
 * @param self The job; checked by ::lh_assert_runtime_ref.
 * @param unit The new value.
 */
lh_void
lh_mb_job_set_unit(lh_mb_job_t *self, lh_u8_t unit);

/**
 * @brief The function code (level 1).
 *
 * @param self The job; checked by ::lh_assert_runtime_ref.
 */
lh_u8_t
lh_mb_job_get_fc(const lh_mb_job_t *self);

/**
 * @brief Store the function code (level 1).
 *
 * @param self The job; checked by ::lh_assert_runtime_ref.
 * @param fc   The new value.
 */
lh_void
lh_mb_job_set_fc(lh_mb_job_t *self, lh_u8_t fc);

/**
 * @brief The first register (level 1).
 *
 * @param self The job; checked by ::lh_assert_runtime_ref.
 */
lh_u16_t
lh_mb_job_get_address(const lh_mb_job_t *self);

/**
 * @brief Store the first register (level 1).
 *
 * @param self    The job; checked by ::lh_assert_runtime_ref.
 * @param address The new value.
 */
lh_void
lh_mb_job_set_address(lh_mb_job_t *self, lh_u16_t address);

/**
 * @brief How many registers to read or write (level 1).
 *
 * @param self The job; checked by ::lh_assert_runtime_ref.
 */
lh_u16_t
lh_mb_job_get_count(const lh_mb_job_t *self);

/**
 * @brief Store how many registers to read or write (level 1).
 *
 * @param self  The job; checked by ::lh_assert_runtime_ref.
 * @param count The new value; at most ::LH_MB_WRITE_MAX for a write, not checked.
 */
lh_void
lh_mb_job_set_count(lh_mb_job_t *self, lh_u16_t count);

/**
 * @brief The words a write writes, ::LH_MB_WRITE_MAX of room (level 1).
 *
 * @param self The job; checked by ::lh_assert_runtime_ref.
 */
const lh_u16_t *
lh_mb_job_get_values(const lh_mb_job_t *self);

/**
 * @brief The words a write writes, to change (level 1).
 *
 * @param self The job; checked by ::lh_assert_runtime_ref.
 */
lh_u16_t *
lh_mb_job_get_values_mut(lh_mb_job_t *self);

/**
 * @brief Whether the job writes: ::LH_MB_FC_WRITE_ONE or ::LH_MB_FC_WRITE_MANY (level 2).
 *
 * @param self The job; checked by ::lh_assert_runtime_ref.
 */
lh_bool_t
lh_mb_job_is_write(const lh_mb_job_t *self);

/**
 * @brief Make @p self a read of @p count registers from @p address of @p unit, owned by
 *        nobody: owner and ticket 0 (level 2).
 *
 * @param self    The job; checked by ::lh_assert_runtime_ref.
 * @param unit    The unit.
 * @param fc      ::LH_MB_FC_READ_HOLDING or ::LH_MB_FC_READ_INPUT; not checked.
 * @param address The first register.
 * @param count   How many, 1 to ::LH_MB_READ_MAX; not checked.
 */
lh_void
lh_mb_job_init_read(lh_mb_job_t *self, lh_u8_t unit, lh_u8_t fc, lh_u16_t address, lh_u16_t count);

/**
 * @brief Make @p self a write of @p count words of @p values to @p address of @p unit,
 *        owned by nobody: owner and ticket 0 (level 3).
 *
 * @param self    The job; checked by ::lh_assert_runtime_ref.
 * @param unit    The unit.
 * @param fc      ::LH_MB_FC_WRITE_ONE (then @p count is 1) or ::LH_MB_FC_WRITE_MANY;
 *                not checked.
 * @param address The first register.
 * @param values  The words; checked by ::lh_assert_runtime_ref.
 * @param count   How many, 1 to ::LH_MB_WRITE_MAX; more are cut to ::LH_MB_WRITE_MAX.
 */
lh_void
lh_mb_job_init_write(lh_mb_job_t *self, lh_u8_t unit, lh_u8_t fc, lh_u16_t address,
                     const lh_u16_t *values, lh_u16_t count);

LH_COMPILER_EXTERN_C_END

#endif /* LH_NET_MODBUS_JOB_H */
