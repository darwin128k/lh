/**
 * @file walk.h
 * @brief ::lh_ui_view_focus_walk_t — state of ::lh_ui_view_visit_focus while looking for the next focus.
 */

#ifndef LH_UI_VIEW_FOCUS_WALK_H
#define LH_UI_VIEW_FOCUS_WALK_H

#include <lh/config.h>
#include <lh/os/input/event.h>
#include <lh/os/input/pointer/id.h>
#include <lh/os/input/wheel.h>
#include <lh/ui/button.h>
#include <lh/ui/view/fields.h>
#include <lh/ui/view/pointer.h>

/**
 * @struct lh_ui_view_focus_walk
 * @brief State of ::lh_ui_view_visit_focus while looking for the next focus.
 */
struct lh_ui_view_focus_walk
{
    const lh_ui_entity_t *current; /**< The focus the walk starts from. */
    lh_bool_t passed;              /**< True once ::current has been visited. */
    const lh_ui_entity_t *first;   /**< First focusable met: the wrap-around answer. */
    const lh_ui_entity_t *next;    /**< First focusable after ::current, once found. */
};
/** @copybrief lh_ui_view_focus_walk */
typedef struct lh_ui_view_focus_walk lh_ui_view_focus_walk_t;

#endif /* LH_UI_VIEW_FOCUS_WALK_H */
