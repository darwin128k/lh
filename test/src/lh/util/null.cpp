#include <gtest/gtest.h>

#include <lh/null.h>

namespace
{

// What this pins is the compiler's, not a value. C++ has no implicit conversion
// from `void *` to any pointer type, and a zero typed as `void *` is not a null
// pointer constant there either, so the C spelling of `lh_null` (::LH_PTR_T_MIN)
// reaches both declarations below only through a cast - measured on this
// toolchain, every `-std` from c++11 to c++23: `invalid conversion from 'void*'
// to 'int (*)(int)'`. That is why the allocator initializers carried a
// reinterpret_cast on every callback argument, and why 27 tests had to spell
// `reinterpret_cast<T *>(lh_null)` where they now write `lh_null`. The values
// are trivially null either way; the compiler is what this test holds.
TEST(util_null, initializes_a_function_pointer_without_a_cast)
{
    int (*fn)(int) = lh_null;
    EXPECT_TRUE(lh_null_eq(fn));
}

TEST(util_null, initializes_typed_object_pointers_without_a_cast)
{
    lh_ptr anything = lh_null;
    const lh_byte_t *bytes = lh_null;
    EXPECT_TRUE(lh_null_eq(anything));
    EXPECT_TRUE(lh_null_eq(bytes));
}

} // namespace