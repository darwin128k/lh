/**
 * @file fn.h
 * @brief ::lh_str_template_lookup_fn — the value of one `{name}` of a template.
 */

#ifndef LH_STR_TEMPLATE_LOOKUP_FN_H
#define LH_STR_TEMPLATE_LOOKUP_FN_H

#include <lh/core/str/view.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The value of @p name, written into @p value.
 *
 * @param user  The value ::lh_str_template_try_expand was given.
 * @param name  The text between the braces; may be empty or hold any character.
 * @param value Receives the value; it must stay valid until the expansion returns.
 * @return ::lh_bool_false when @p name has no value: the expansion fails.
 */
typedef lh_bool_t(lh_str_template_lookup_fn)(lh_ptr user, const lh_str_view_t *name,
                                             lh_str_view_t *value);

LH_COMPILER_EXTERN_C_END

#endif /* LH_STR_TEMPLATE_LOOKUP_FN_H */
