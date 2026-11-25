/**
 * @file ptr.h
 * @brief Pointer aliases of ::lh_input_touch_t.
 */

#ifndef LH_INPUT_TOUCH_PTR_H
#define LH_INPUT_TOUCH_PTR_H

#include <lh/core/util/ptr.h>

struct lh_input_touch;

/**
 * @def lh_input_touch_ptr
 * @brief Pointer to ::lh_input_touch_t.
 */
#define lh_input_touch_ptr lh_ptr_of(struct lh_input_touch)

/**
 * @def lh_input_touch_cptr
 * @brief Pointer to `const` ::lh_input_touch_t, for `.c` files only.
 */
#define lh_input_touch_cptr lh_cptr_of(struct lh_input_touch)

#endif /* LH_INPUT_TOUCH_PTR_H */
