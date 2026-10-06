/**
 * @file fields.h
 * @brief Member fields of ::lh_timer_group_t.
 */

#ifndef LH_TIMER_GROUP_FIELDS_H
#define LH_TIMER_GROUP_FIELDS_H

#include <lh/list.h>

/**
 * @def lh_timer_group_fields(list_type)
 * @brief Intrusive list of running timers. Owns no timer memory.
 *
 * @param list_type Type of the timer list (::lh_list_t).
 */
#define lh_timer_group_fields(list_type) list_type timers

#endif /* LH_TIMER_GROUP_FIELDS_H */
