/**
 * @file ptr.h
 * @brief Pointer aliases of ::lh_input_sample_t.
 */

#ifndef LH_INPUT_SAMPLE_PTR_H
#define LH_INPUT_SAMPLE_PTR_H

#include <lh/core/util/ptr.h>

struct lh_input_sample;

/**
 * @def lh_input_sample_ptr
 * @brief Pointer to ::lh_input_sample_t.
 */
#define lh_input_sample_ptr lh_ptr_of(struct lh_input_sample)

/**
 * @def lh_input_sample_cptr
 * @brief Pointer to `const` ::lh_input_sample_t, for `.c` files only.
 */
#define lh_input_sample_cptr lh_cptr_of(struct lh_input_sample)

#endif /* LH_INPUT_SAMPLE_PTR_H */
