#include <lh/numeric/fixed/limits.h>
#include <lh/assert/static.h>

/*
 * LH_*_T_MIN / LH_*_T_MAX from lh/numeric/limits.h read interval struct members and
 * are not C constant expressions, so they cannot appear in _Static_assert under
 * GCC. Interval bounds are filled via lh_numeric_limit_*; these checks verify
 * the underlying lh_* types match the widths assumed by lh_u8_t … lh_s64_t.
 */

lh_assert_static(lh_numeric_limit_umin(lh_uchar_t) == 0, "lh_uchar_t minimum must be 0");
lh_assert_static(lh_numeric_limit_umax(lh_uchar_t) == 255, "lh_uchar_t must be 8-bit unsigned");
lh_assert_static(lh_numeric_limit_umin(lh_ushort_t) == 0, "lh_ushort_t minimum must be 0");
lh_assert_static(lh_numeric_limit_umax(lh_ushort_t) == 65535,
                 "lh_ushort_t must be 16-bit unsigned");
lh_assert_static(lh_numeric_limit_umin(lh_uint_t) == 0, "lh_uint_t minimum must be 0");
lh_assert_static(lh_numeric_limit_umax(lh_uint_t) == 4294967295U,
                 "lh_uint_t must be 32-bit unsigned");
lh_assert_static(lh_numeric_limit_umin(lh_ullong_t) == 0, "lh_ullong_t minimum must be 0");
lh_assert_static(lh_numeric_limit_umax(lh_ullong_t) == 18446744073709551615ULL,
                 "lh_ullong_t must be 64-bit unsigned");

lh_assert_static(lh_numeric_limit_smin(lh_schar_t) == -128, "lh_schar_t must be 8-bit signed");
lh_assert_static(lh_numeric_limit_smax(lh_schar_t) == 127, "lh_schar_t must be 8-bit signed");
lh_assert_static(lh_numeric_limit_smin(lh_sshort_t) == -32767 - 1,
                 "lh_sshort_t must be 16-bit signed");
lh_assert_static(lh_numeric_limit_smax(lh_sshort_t) == 32767, "lh_sshort_t must be 16-bit signed");
lh_assert_static(lh_numeric_limit_smin(lh_sint_t) == -2147483647 - 1,
                 "lh_sint_t must be 32-bit signed");
lh_assert_static(lh_numeric_limit_smax(lh_sint_t) == 2147483647, "lh_sint_t must be 32-bit signed");
lh_assert_static(lh_numeric_limit_smin(lh_sllong_t) == -9223372036854775807LL - 1,
                 "lh_sllong_t must be 64-bit signed");
lh_assert_static(lh_numeric_limit_smax(lh_sllong_t) == 9223372036854775807LL,
                 "lh_sllong_t must be 64-bit signed");
