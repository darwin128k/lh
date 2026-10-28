/**
 * @file fn.h
 * @brief ::lh_mb_link_change_fn — told when a link changes state.
 */

#ifndef LH_NET_MODBUS_LINK_CHANGE_FN_H
#define LH_NET_MODBUS_LINK_CHANGE_FN_H

#include <lh/ptr.h>

/**
 * @brief A link changed state: an answer arrived, a request failed, ...
 *
 * @param self The owner the application registered.
 * @param link The link that changed.
 */
typedef lh_void(lh_mb_link_change_fn)(lh_ptr self, lh_ptr link);

#endif /* LH_NET_MODBUS_LINK_CHANGE_FN_H */
