/**
 * @file desc.h
 * @brief Type of ::lh_werror_t::desc (::lh_werror_desc_t).
 */

#ifndef LH_WERROR_DESC_H
#define LH_WERROR_DESC_H

#include <lh/wstr/view.h>

/**
 * @typedef lh_werror_desc_t
 * @brief Wide error description: a non-owning ::lh_wstr_view_t over the message.
 *
 * An empty / uninitialized view means “no description”.
 * ::lh_werror_t does not own the text.
 */
typedef lh_wstr_view_t lh_werror_desc_t;

#endif /* LH_WERROR_DESC_H */
