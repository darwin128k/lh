/**
 * @file walk.h
 * @brief ::lh_gl_view_focus_walk_t — state of ::lh_gl_view_visit_focus while looking for the next focus.
 */

#ifndef LH_GL_VIEW_FOCUS_WALK_H
#define LH_GL_VIEW_FOCUS_WALK_H

#include <lh/config.h>
#include <lh/os/input/event.h>
#include <lh/os/input/pointer/id.h>
#include <lh/os/input/wheel.h>
#include <lh/gl/button.h>
#include <lh/gl/view/fields.h>
#include <lh/gl/view/pointer.h>

/**
 * @struct lh_gl_view_focus_walk
 * @brief State of ::lh_gl_view_visit_focus while looking for the next focus.
 */
struct lh_gl_view_focus_walk
{
    const lh_gl_entity_t *current; /**< The focus the walk starts from. */
    lh_bool_t passed;              /**< True once ::current has been visited. */
    const lh_gl_entity_t *first;   /**< First focusable met: the wrap-around answer. */
    const lh_gl_entity_t *next;    /**< First focusable after ::current, once found. */
};
/** @copybrief lh_gl_view_focus_walk */
typedef struct lh_gl_view_focus_walk lh_gl_view_focus_walk_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The entity that has the focus (level 1).
 *
 * @param self The focus walk; checked by ::lh_assert_runtime_ref.
 */
const lh_gl_entity_t *
lh_gl_view_focus_walk_get_current(const lh_gl_view_focus_walk_t *self);

/**
 * @brief Store the entity that has the focus (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The focus walk; checked by ::lh_assert_runtime_ref.
 * @param current The new value.
 */
lh_void
lh_gl_view_focus_walk_set_current(lh_gl_view_focus_walk_t *self, const lh_gl_entity_t * current);

/**
 * @brief Whether the walk has passed the focused entity (level 1).
 *
 * @param self The focus walk; checked by ::lh_assert_runtime_ref.
 */
lh_bool_t
lh_gl_view_focus_walk_is_passed(const lh_gl_view_focus_walk_t *self);

/**
 * @brief Store whether the walk has passed the focused entity (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The focus walk; checked by ::lh_assert_runtime_ref.
 * @param passed The new value.
 */
lh_void
lh_gl_view_focus_walk_set_passed(lh_gl_view_focus_walk_t *self, lh_bool_t passed);

/**
 * @brief The first entity that can take the focus (level 1).
 *
 * @param self The focus walk; checked by ::lh_assert_runtime_ref.
 */
const lh_gl_entity_t *
lh_gl_view_focus_walk_get_first(const lh_gl_view_focus_walk_t *self);

/**
 * @brief Store the first entity that can take the focus (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The focus walk; checked by ::lh_assert_runtime_ref.
 * @param first The new value.
 */
lh_void
lh_gl_view_focus_walk_set_first(lh_gl_view_focus_walk_t *self, const lh_gl_entity_t * first);

/**
 * @brief The next entity that can take the focus (level 1).
 *
 * @param self The focus walk; checked by ::lh_assert_runtime_ref.
 */
const lh_gl_entity_t *
lh_gl_view_focus_walk_get_next(const lh_gl_view_focus_walk_t *self);

/**
 * @brief Store the next entity that can take the focus (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The focus walk; checked by ::lh_assert_runtime_ref.
 * @param next The new value.
 */
lh_void
lh_gl_view_focus_walk_set_next(lh_gl_view_focus_walk_t *self, const lh_gl_entity_t * next);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_VIEW_FOCUS_WALK_H */
