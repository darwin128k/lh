/**
 * @file desc.h
 * @brief Type alias for runtime error description pointers (::lh_runtime_error_desc_t).
 */

#ifndef LH_RUNTIME_ERROR_DESC_H
#define LH_RUNTIME_ERROR_DESC_H

#include <lh/error/desc.h>

/**
 * @typedef lh_runtime_error_desc_t
 * @brief Same representation as ::lh_error_desc_t for runtime-layer APIs.
 *
 * Distinct typedef documents intent (runtime throw / check paths) while keeping
 * binary compatibility with ::lh_error_desc_t.
 */
typedef lh_error_desc_t lh_runtime_error_desc_t;

#endif // LH_RUNTIME_ERROR_DESC_H
