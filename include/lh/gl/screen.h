/**
 * @file screen.h
 * @brief The screens of one layer: open and back.
 *
 * A screen is an ordinary ::lh_gl_layer_t. The layer you pass holds them as
 * its children, and that list is the whole stack: the first child is the
 * back screen, the last child is the one in front. There is no second array
 * and nothing is allocated. The window and the surface stay outside this
 * call; they already draw this layer.
 *
 * ::lh_gl_screen_open links a parentless layer as the new front child.
 * ::lh_gl_screen_back unlinks that front layer and hands it back. The caller
 * keeps the memory either way. A widget belongs on a screen, not in this
 * list. When the front child is not a layer, it is not a screen: back leaves
 * the list as it is. No gesture is read here. A button, a key or a swipe
 * only decides to call open or back.
 */

#ifndef LH_GL_SCREEN_H
#define LH_GL_SCREEN_H

#include <lh/gl/layer.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The front screen of @p self, the last child when that child is a layer.
 *
 * @param self Layer whose children are the screens; must not be null
 *             (asserted in debug builds).
 * @return The front layer, still owned by its creator. ::lh_null when @p self
 *         has no children, or when the front child is not a layer.
 */
lh_gl_layer_t *
lh_gl_screen_get_front(const lh_gl_layer_t *self);

/**
 * @brief Link @p screen as the front child of @p self.
 *
 * The refusal is ::lh_gl_layer_try_add: @p screen already has a parent,
 * @p screen is @p self or one of its ancestors, or @p screen is attached to
 * a surface. Nothing changes on refusal.
 *
 * @param self   Layer that holds the screens; must not be null (asserted in
 *               debug builds).
 * @param screen Parentless layer to show; must not be null (asserted in
 *               debug builds). It must outlive its membership.
 * @return ::lh_bool_true when @p screen is now the front child;
 *         ::lh_bool_false when refused.
 */
lh_bool_t
lh_gl_screen_try_open(lh_gl_layer_t *self, lh_gl_layer_t *screen);

/**
 * @brief Unlink the front screen of @p self.
 *
 * The layer is not freed. Its parent is cleared. An empty list, or a front
 * child that is not a layer, leaves @p self unchanged.
 *
 * @param self Layer that holds the screens; must not be null (asserted in
 *             debug builds).
 * @return The layer that was in front, now parentless. ::lh_null when there
 *         was no screen to remove.
 */
lh_gl_layer_t *
lh_gl_screen_try_back(lh_gl_layer_t *self);

/**
 * @brief ::lh_gl_screen_try_open, and a refusal is a failed runtime check.
 *
 * @param self   Layer that holds the screens; must not be null (asserted in
 *               debug builds).
 * @param screen Parentless layer to show; must not be null (asserted in
 *               debug builds).
 *
 * @fails ::lh_runtime_error_code_not_accepted when ::lh_gl_screen_try_open refuses.
 */
lh_void
lh_gl_screen_open(lh_gl_layer_t *self, lh_gl_layer_t *screen);

/**
 * @brief ::lh_gl_screen_try_back, and no screen to remove is a failed runtime check.
 *
 * @param self Layer that holds the screens; must not be null (asserted in
 *             debug builds).
 * @return The layer that was in front, now parentless. Not null.
 *
 * @fails ::lh_runtime_error_code_not_accepted when ::lh_gl_screen_try_back
 *        returns ::lh_null.
 */
lh_gl_layer_t *
lh_gl_screen_back(lh_gl_layer_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_SCREEN_H */
