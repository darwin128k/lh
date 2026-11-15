/**
 * @file ptr.h
 * @brief Pointer aliases of ::lh_os_input_sample_t.
 */

#ifndef LH_OS_INPUT_SAMPLE_PTR_H
#define LH_OS_INPUT_SAMPLE_PTR_H

#include <lh/core/util/ptr.h>

struct lh_os_input_sample;

/**
 * @def lh_os_input_sample_ptr
 * @brief Pointer to ::lh_os_input_sample_t.
 */
#define lh_os_input_sample_ptr lh_ptr_of(struct lh_os_input_sample)

/**
 * @def lh_os_input_sample_cptr
 * @brief Pointer to `const` ::lh_os_input_sample_t, for `.c` files only.
 */
#define lh_os_input_sample_cptr lh_cptr_of(struct lh_os_input_sample)

#endif /* LH_OS_INPUT_SAMPLE_PTR_H */
