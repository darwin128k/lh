/**
 * @file data.h
 * @brief Storage of ::lh_value_t: one member per kind, sharing memory.
 */

#ifndef LH_VALUE_DATA_H
#define LH_VALUE_DATA_H

#include <lh/real.h>
#include <lh/str/view.h>

/**
 * @union lh_value_data
 * @brief What an ::lh_value_t holds; which member is live is its `kind`.
 *
 * Read only through the `lh_value_get_*` functions; all but the raw
 * ::lh_value_get_data check the kind.
 */
union lh_value_data
{
    lh_bool_t b;   /**< ::lh_value_kind_bool */
    lh_sllong_t i; /**< ::lh_value_kind_sint */
    lh_ullong_t u; /**< ::lh_value_kind_uint */
#if LH_REAL_ENABLED
    lh_real_t r; /**< ::lh_value_kind_real */
#endif
    lh_str_view_t s; /**< ::lh_value_kind_str */
    lh_ptr p;        /**< ::lh_value_kind_ptr */
};
/** @copybrief lh_value_data */
typedef union lh_value_data lh_value_data_t;

#endif /* LH_VALUE_DATA_H */
