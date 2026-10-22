#include <lh/str/parse/real.h>

#if LH_REAL_ENABLED

#    include <lh/real/decimal.h>

lh_bool_t
lh_str_ptr_parse_real(const lh_str_ptr str, lh_usize_t str_size, lh_real_t *out)
{
    lh_real_decimal_t decimal;

    return lh_real_decimal_parse(lh_addr_of(decimal), str, str_size) &&
           lh_real_decimal_to_real(lh_addr_of(decimal), out);
}

#endif /* LH_REAL_ENABLED */
