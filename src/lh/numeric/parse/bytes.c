/**
 * @file bytes.c
 * @brief Implementation of `lh/numeric/parse/bytes.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/numeric/parse/bytes.h>

lh_void
lh_numeric_parse_bytes(lh_uint_t value, lh_byte_ptr out, lh_usize_t count)
{
    lh_assert_runtime_ref(out);
    lh_assert_runtime_if(count < 1u || count > sizeof(lh_uint_t),
                         lh_runtime_error_code_invalid_argument);
    for (lh_usize_t i = 0U; i < count; ++i)
    {
        const lh_usize_t shift = 8u * (count - 1u - i);
        out[i] = lh_cast_static(lh_byte_t, (value >> shift) & 0xFFu);
    }
}
