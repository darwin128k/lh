/**
 * @file desc.h
 * @brief Type of ::lh_error_t::desc (::lh_error_desc_t).
 */

#ifndef LH_ERROR_DESC_H
#define LH_ERROR_DESC_H

#include <lh/str/view.h>

/**
 * @typedef lh_error_desc_t
 * @brief Error description: a non-owning ::lh_str_view_t over the message.
 *
 * An empty / uninitialized view means “no description”.
 * ::lh_error_t does not own the text.
 */
typedef lh_str_view_t lh_error_desc_t;

#endif /* LH_ERROR_DESC_H */
