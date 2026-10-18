/**
 * @file ops.h
 * @brief ::lh_usize_t arithmetic that fails instead of wrapping.
 */

#ifndef LH_SIZE_OPS_H
#define LH_SIZE_OPS_H

#include <lh/compiler/extern/c.h>
#include <lh/size.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief @p a + @p b.
 *
 * The brick block sizes are computed with: a payload plus the header in
 * front of it.
 *
 * @param a Addend.
 * @param b Addend.
 * @return The sum.
 * @fails ::lh_runtime_error_code_overflow
 *        The sum does not fit in ::lh_usize_t.
 */
lh_usize_t
lh_size_add(lh_usize_t a, lh_usize_t b);

LH_COMPILER_EXTERN_C_END

#endif /* LH_SIZE_OPS_H */
