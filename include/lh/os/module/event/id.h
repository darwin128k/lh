/**
 * @file id.h
 * @brief Bus-local numeric event identity; zero is invalid.
 *
 * IDs belong to one initialized bus. They are never reused within that lifetime
 * and remain valid when subscriptions are cleared; do not persist or exchange
 * them between independent buses.
 */

#ifndef LH_OS_MODULE_EVENT_ID_H
#define LH_OS_MODULE_EVENT_ID_H

#include <lh/size.h>

/** @brief One-based registry index, valid until the owning bus is deinitialized. */
typedef lh_usize_t lh_os_module_event_id_t;

#endif /* LH_OS_MODULE_EVENT_ID_H */
