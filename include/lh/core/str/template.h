/**
 * @file template.h
 * @brief Text with `{name}` tokens, each replaced by the value a lookup gives.
 *
 * A template is plain text; `{` opens a token and the next `}` closes it. There
 * is no escape: a `}` outside a token, a `{` without its `}` and a name the lookup
 * does not know all fail — nothing is guessed or left in place. Where the values
 * come from is the caller's: a callback, or a registry of named values
 * (`lh/core/str/template/registry.h`).
 */

#ifndef LH_STR_TEMPLATE_H
#define LH_STR_TEMPLATE_H

#include <lh/core/str.h>
#include <lh/core/str/template/lookup/cb.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Whether @p name is a token name a registry accepts: one or more word
 *        characters (::lh_char_is_word).
 *
 * @param name The name; checked by ::lh_assert_runtime_ref.
 */
lh_bool_t
lh_str_template_is_name(const lh_str_view_t *name);

/**
 * @brief Expand @p pattern into @p out: every `{name}` replaced by its value.
 *
 * @param pattern The template; checked by ::lh_assert_runtime_ref.
 * @param lookup  Gives the value of each name; checked by ::lh_assert_runtime_ref.
 * @param user    Passed to @p lookup.
 * @param out     Receives the text, replacing what it held; checked by
 *                ::lh_assert_runtime_ref.
 * @return ::lh_bool_false for a stray brace or a name without a value; @p out is
 *         empty then.
 */
lh_bool_t
lh_str_template_try_expand(const lh_str_view_t *pattern, lh_str_template_lookup_cb lookup,
                           lh_ptr user, lh_str_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_STR_TEMPLATE_H */
