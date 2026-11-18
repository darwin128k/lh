/**
 * @file fn.h
 * @brief Function type ::lh_gx_radio_group_change_fn.
 */

#ifndef LH_GX_RADIO_GROUP_CHANGE_FN_H
#define LH_GX_RADIO_GROUP_CHANGE_FN_H

#include <lh/core/compiler/extern/c.h>
#include <lh/core/ptr.h>

struct lh_gx_radio;
struct lh_gx_radio_group;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @typedef lh_gx_radio_group_change_fn
 * @brief The application's reaction to the user choosing @p radio in @p group.
 *
 * Called once per tap on a radio that was not the one chosen, after the
 * group is drawn with the new choice. Not called when the application chooses
 * itself (::lh_gx_radio_group_select), nor for a tap on the radio already
 * chosen. The pointer is already free, so the reaction may change the screen.
 *
 * @param group   The group.
 * @param radio   The radio now chosen.
 * @param context What the application gave the group with the reaction.
 */
typedef lh_void(lh_gx_radio_group_change_fn)(struct lh_gx_radio_group *group,
                                             struct lh_gx_radio *radio, lh_ptr context);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_RADIO_GROUP_CHANGE_FN_H */
