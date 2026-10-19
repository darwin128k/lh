/**
 * @file once.h
 * @brief Cut a text in two at the first occurrence of a delimiter.
 *
 * For `key=value`, `host:port`, `Name: value` — anything that is one head,
 * one delimiter, and the rest. The rest may hold the delimiter again:
 * only the first one cuts. Nothing is trimmed; that is the caller's call.
 */

#ifndef LH_STR_SPLIT_ONCE_H
#define LH_STR_SPLIT_ONCE_H

#include <lh/bool.h>
#include <lh/char.h>
#include <lh/compiler/extern/c.h>
#include <lh/str/view.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Split @p self at the first @p delim into @p head and @p tail.
 *
 * Example usage:
 * @code{.c}
 * lh_str_view_t text = lh_str_view_lit("a=b=c");
 * lh_str_view_t head;
 * lh_str_view_t tail;
 * lh_str_view_split_once(&text, '=', &head, &tail); // "a", "b=c"
 * @endcode
 *
 * @param self  View to split.
 * @param delim Delimiter character.
 * @param head  Out: everything before the first @p delim (empty if
 *              @p self starts with it).
 * @param tail  Out: everything after it (empty if @p self ends with it).
 *
 * @return ::lh_bool_true if @p delim occurs in @p self; ::lh_bool_false
 *         otherwise, with @p head and @p tail untouched.
 */
lh_bool_t
lh_str_view_split_once(const lh_str_view_t *self, lh_char_t delim, lh_str_view_t *head,
                       lh_str_view_t *tail);

LH_COMPILER_EXTERN_C_END

#endif /* LH_STR_SPLIT_ONCE_H */
