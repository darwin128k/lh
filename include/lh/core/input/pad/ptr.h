/**
 * @file ptr.h
 * @brief Pointer aliases of ::lh_input_pad_t.
 */

#ifndef LH_INPUT_PAD_PTR_H
#define LH_INPUT_PAD_PTR_H

#include <lh/core/util/ptr.h>

struct lh_input_pad;

/**
 * @def lh_input_pad_ptr
 * @brief Pointer to ::lh_input_pad_t.
 */
#define lh_input_pad_ptr lh_ptr_of(struct lh_input_pad)

/**
 * @def lh_input_pad_cptr
 * @brief Pointer to `const` ::lh_input_pad_t, for `.c` files only.
 */
#define lh_input_pad_cptr lh_cptr_of(struct lh_input_pad)

#endif /* LH_INPUT_PAD_PTR_H */
