/**
 * @file phase.h
 * @brief ::lh_mb_batch_phase_t — what a batch of writes is doing.
 */

#ifndef LH_NET_MODBUS_BATCH_PHASE_H
#define LH_NET_MODBUS_BATCH_PHASE_H

/** @brief What a batch of writes (::lh_mb_batch_t) is doing. */
typedef enum lh_mb_batch_phase
{
    lh_mb_batch_idle = 0, /**< no batch is taken; ::lh_mb_batch_try_begin takes one */
    lh_mb_batch_writing,  /**< writing the values that are pending */
    lh_mb_batch_verifying /**< reading back the values that were written */
} lh_mb_batch_phase_t;

#endif /* LH_NET_MODBUS_BATCH_PHASE_H */