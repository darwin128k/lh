#include <lh/sql/mysql/query.h>
#include <lh/assert.h>
#include <lh/char/map.h>

lh_usize_t
lh_sql_mysql_query_skip_quoted(const lh_str_ptr text, lh_usize_t size, lh_usize_t pos)
{
    const lh_char_t quote = text[pos];
    /* Backticks quote a name: no backslash escapes there. */
    const lh_bool_t escapes = lh_cast_static(lh_bool_t, lh_math_ne(quote, lh_char_map_grave));

    for (pos = lh_math_add_one(pos); lh_math_lt(pos, size); pos = lh_math_add_one(pos))
    {
        if (escapes && lh_math_eq(text[pos], lh_char_map_backslash))
        {
            pos = lh_math_add_one(pos);
        }
        else if (lh_math_eq(text[pos], quote))
        {
            /* A doubled quote stands for itself and does not end the run. */
            if (lh_math_lt(lh_math_add_one(pos), size) &&
                lh_math_eq(text[lh_math_add_one(pos)], quote))
            {
                pos = lh_math_add_one(pos);
                continue;
            }
            return lh_math_add_one(pos);
        }
    }
    return size;
}

lh_usize_t
lh_sql_mysql_query_skip_comment(const lh_str_ptr text, lh_usize_t size, lh_usize_t pos)
{
    const lh_char_t ch = text[pos];
    const lh_char_t next =
        lh_math_lt(lh_math_add_one(pos), size) ? text[lh_math_add_one(pos)] : lh_char_map_nul;
    const lh_char_t after =
        lh_math_lt(lh_math_add(pos, 2U), size) ? text[lh_math_add(pos, 2U)] : lh_char_map_nul;
    lh_usize_t at;

    if (lh_math_eq(ch, lh_char_map_hash) ||
        (lh_math_eq(ch, lh_char_map_minus) && lh_math_eq(next, lh_char_map_minus) &&
         lh_math_le(lh_cast_static(lh_byte_t, after),
                    lh_cast_static(lh_byte_t, lh_char_map_space))))
    {
        for (at = pos; lh_math_lt(at, size) && lh_math_ne(text[at], lh_char_map_lf);
             at = lh_math_add_one(at))
        {
        }
        return at;
    }
    if (lh_math_eq(ch, lh_char_map_slash) && lh_math_eq(next, lh_char_map_asterisk))
    {
        for (at = lh_math_add(pos, 2U); lh_math_lt(lh_math_add_one(at), size);
             at = lh_math_add_one(at))
        {
            if (lh_math_eq(text[at], lh_char_map_asterisk) &&
                lh_math_eq(text[lh_math_add_one(at)], lh_char_map_slash))
            {
                return lh_math_add(at, 2U);
            }
        }
        return size;
    }
    return pos;
}

lh_usize_t
lh_sql_mysql_query_find_param(const lh_str_ptr text, lh_usize_t size, lh_usize_t pos)
{
    while (lh_math_lt(pos, size))
    {
        const lh_char_t ch = text[pos];
        const lh_usize_t past = lh_sql_mysql_query_skip_comment(text, size, pos);

        if (lh_math_ne(past, pos))
        {
            pos = past;
        }
        else if (lh_math_eq(ch, lh_char_map_squote) || lh_math_eq(ch, lh_char_map_dquot) ||
                 lh_math_eq(ch, lh_char_map_grave))
        {
            pos = lh_sql_mysql_query_skip_quoted(text, size, pos);
        }
        else if (lh_math_eq(ch, lh_char_map_question))
        {
            return pos;
        }
        else
        {
            pos = lh_math_add_one(pos);
        }
    }
    return size;
}

lh_usize_t
lh_sql_mysql_query_count_params(lh_str_view_t sql)
{
    const lh_str_cptr text = lh_str_view_get_begin(lh_addr_of(sql));
    const lh_usize_t size = lh_str_view_get_size(lh_addr_of(sql));
    lh_usize_t count = 0U;

    for (lh_usize_t pos = lh_sql_mysql_query_find_param(text, size, 0U); lh_math_lt(pos, size);
         pos = lh_sql_mysql_query_find_param(text, size, lh_math_add_one(pos)))
    {
        count = lh_math_add_one(count);
    }
    return count;
}

lh_bool_t
lh_sql_mysql_query_substitute(lh_str_view_t sql, const lh_str_t *literals, lh_usize_t count,
                              lh_str_t *out)
{
    const lh_str_cptr text = lh_str_view_get_begin(lh_addr_of(sql));
    const lh_usize_t size = lh_str_view_get_size(lh_addr_of(sql));
    lh_usize_t param = 0U;
    lh_usize_t start = 0U;
    lh_usize_t pos;

    lh_assert_runtime_ref(out);
    for (pos = lh_sql_mysql_query_find_param(text, size, 0U); lh_math_lt(pos, size);
         pos = lh_sql_mysql_query_find_param(text, size, lh_math_add_one(pos)))
    {
        if (lh_math_ge(param, count) || lh_str_is_empty(lh_addr_of(literals[param])))
        {
            return lh_bool_false;
        }
        /* Everything since the last placeholder, then this one's literal. */
        lh_str_append(out, text + start, lh_math_sub(pos, start));
        lh_str_append_str(out, lh_addr_of(literals[param]));
        param = lh_math_add_one(param);
        start = lh_math_add_one(pos);
    }
    if (lh_math_gt(size, start))
    {
        lh_str_append(out, text + start, lh_math_sub(size, start));
    }
    return lh_bool_true;
}
