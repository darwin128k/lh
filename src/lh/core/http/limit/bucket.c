/**
 * @file bucket.c
 * @brief Refilling a token bucket.
 */

#include <lh/core/http/limit/bucket.h>
#include <lh/core/math.h>

lh_real_t
lh_http_limit_bucket_get_refilled(lh_real_t tokens, lh_s64_t elapsed_ms, lh_real_t per_second,
                                  lh_real_t burst)
{
    const lh_real_t gained =
        lh_math_gt(elapsed_ms, 0)
            ? lh_math_div(lh_math_mul(lh_cast_static(lh_real_t, elapsed_ms), per_second), 1000.0)
            : 0.0;

    return lh_math_min(lh_math_add(tokens, gained), burst);
}
