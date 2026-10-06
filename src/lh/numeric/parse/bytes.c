/**
 * @file bytes.c
 * @brief Implementation of `lh/numeric/parse/bytes.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/cast/static.h>
#include <lh/numeric/parse/bytes.h>
#include <lh/runtime/error/code.h>

lh_void
lh_numeric_parse_bytes(lh_uint_t value, lh_byte_t *out, lh_usize_t count)
{
    lh_usize_t i;
    lh_assert_runtime_ref(out);
    lh_assert_runtime_if(count < 1u || count > sizeof(lh_uint_t),
                         lh_runtime_error_code_invalid_argument);
    for (i = 0; i < count; ++i)
    {
        const lh_usize_t shift = 8u * (count - 1u - i);
        out[i] = lh_cast_static(lh_byte_t, (value >> shift) & 0xFFu);
    }
}
