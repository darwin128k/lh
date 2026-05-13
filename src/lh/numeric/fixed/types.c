#include <lh/numeric/fixed/types.h>
#include <lh/assert/static.h>
#include <lh/util/type.h>

lh_assert_static(lh_type_size(lh_u8_t) == 1, "lh_u8_t must be 1 byte");
lh_assert_static(lh_type_size(lh_u16_t) == 2, "lh_u16_t must be 2 bytes");
lh_assert_static(lh_type_size(lh_u32_t) == 4, "lh_u32_t must be 4 bytes");
lh_assert_static(lh_type_size(lh_u64_t) == 8, "lh_u64_t must be 8 bytes");
lh_assert_static(lh_type_size(lh_s8_t) == 1, "lh_s8_t must be 1 byte");
lh_assert_static(lh_type_size(lh_s16_t) == 2, "lh_s16_t must be 2 bytes");
lh_assert_static(lh_type_size(lh_s32_t) == 4, "lh_s32_t must be 4 bytes");
lh_assert_static(lh_type_size(lh_s64_t) == 8, "lh_s64_t must be 8 bytes");
