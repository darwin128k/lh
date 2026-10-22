/**
 * @file query.h
 * @brief Put bound values into a MySQL statement's text, where its `?`
 *        placeholders are.
 *
 * The MySQL backend runs prepared statements as text: the C client's
 * `MYSQL_BIND` changes layout between client versions, and a library
 * loaded at run time gives no header to check it against. So every bound
 * value is turned into an SQL literal when it is bound (`NULL`, `-12`,
 * `'it\'s'`) and the literals replace the placeholders when the statement
 * runs. Only a `?` that MySQL itself would read as a placeholder is
 * replaced: not one inside `'...'`, `"..."` or `` `...` ``, nor in a
 * `-- `, `#` or `/ * * /` comment.
 *
 * No connection is needed here, so this part works — and is tested — on
 * any target; lh/sql/mysql.h makes the literals.
 */

#ifndef LH_SQL_MYSQL_QUERY_H
#define LH_SQL_MYSQL_QUERY_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/size.h>
#include <lh/str.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The position right after the quoted run that starts at @p pos —
 *        `'...'`, `"..."` or `` `...` `` — or @p size when it never closes.
 *
 * Inside, a doubled quote is the quote itself, and in `'` and `"` a
 * backslash escapes the next character.
 */
lh_usize_t
lh_sql_mysql_query_skip_quoted(const lh_str_ptr text, lh_usize_t size, lh_usize_t pos);

/**
 * @brief The position right after the comment that starts at @p pos, or
 *        @p pos when none does: `#` and `-- ` (a space or control after the
 *        dashes, as MySQL wants) run to the line end, `/ *` to `* /`.
 */
lh_usize_t
lh_sql_mysql_query_skip_comment(const lh_str_ptr text, lh_usize_t size, lh_usize_t pos);

/**
 * @brief The position of the first placeholder at or after @p pos, skipping
 *        quoted runs and comments; @p size when there is none.
 */
lh_usize_t
lh_sql_mysql_query_find_param(const lh_str_ptr text, lh_usize_t size, lh_usize_t pos);

/** How many placeholders @p sql has. */
lh_usize_t
lh_sql_mysql_query_count_params(lh_str_view_t sql);

/**
 * @brief Append @p sql to @p out with its `n`-th placeholder replaced by
 *        `literals[n - 1]`.
 *
 * @return ::lh_bool_false, with @p out holding what was written so far,
 *         when a placeholder has no literal: past @p count, or an empty one
 *         (a parameter that was never bound).
 */
lh_bool_t
lh_sql_mysql_query_substitute(lh_str_view_t sql, const lh_str_t *literals, lh_usize_t count,
                              lh_str_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_SQL_MYSQL_QUERY_H */
