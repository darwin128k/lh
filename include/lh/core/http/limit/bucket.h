/**
 * @file bucket.h
 * @brief A token bucket for a byte rate: what is spent comes back at the rate,
 *        up to a burst.
 */

#ifndef LH_HTTP_LIMIT_BUCKET_H
#define LH_HTTP_LIMIT_BUCKET_H

#include <lh/core/compiler/extern/c.h>
#include <lh/core/real.h>
#include <lh/core/numeric/fixed/types.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The tokens a bucket holds @p elapsed_ms after it held @p tokens.
 *
 * @param tokens     What the bucket held; may be negative after an overdraft.
 * @param elapsed_ms Milliseconds since then; a negative time adds nothing.
 * @param per_second Tokens that come back each second.
 * @param burst      The most the bucket holds.
 */
lh_real_t
lh_http_limit_bucket_get_refilled(lh_real_t tokens, lh_s64_t elapsed_ms, lh_real_t per_second,
                                  lh_real_t burst);

LH_COMPILER_EXTERN_C_END

#endif /* LH_HTTP_LIMIT_BUCKET_H */
