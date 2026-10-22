#include <gtest/gtest.h>

#include <lh/real.h>

#if LH_REAL_ENABLED

#    include <charconv>
#    include <cmath>
#    include <cstdio>
#    include <cstdlib>
#    include <cstring>
#    include <limits>
#    include <random>
#    include <string>

#    include <lh/real/bits.h>
#    include <lh/real/decimal.h>
#    include <lh/str/format/real.h>
#    include <lh/str/parse/real.h>

namespace
{

/* The reference: libc's correctly rounded conversion for this build's type. */
lh_real_t
reference(const std::string &text)
{
#    if LH_LIBRARY_OPTION_REAL == LH_REAL_DOUBLE
    return std::strtod(text.c_str(), nullptr);
#    else
    return std::strtof(text.c_str(), nullptr);
#    endif
}

bool
parse(const std::string &text, lh_real_t *out)
{
    return lh_str_ptr_parse_real(text.data(), text.size(), out);
}

std::string
format(lh_real_t value)
{
    char buf[LH_STR_FORMAT_REAL_SIZE_MAX];
    const lh_usize_t n = lh_str_ptr_format_real(value, buf, sizeof(buf));
    return std::string(buf, n);
}

/* What the parser gives, or "fail", as bits for exact comparison. */
std::string
parsed_bits(const std::string &text)
{
    lh_real_t value = 0;
    if (!parse(text, &value))
    {
        return "fail";
    }
    return std::to_string(lh_real_to_bits(value));
}

std::string
reference_bits(const std::string &text)
{
    const lh_real_t value = reference(text);
    if (std::isinf(value))
    {
        return "fail";
    }
    return std::to_string(lh_real_to_bits(value));
}

/* ── decimal ─────────────────────────────────────────────────────────────── */

TEST(real_decimal, takes_the_text_apart)
{
    lh_real_decimal_t d;

    ASSERT_TRUE(lh_real_decimal_parse(&d, "-12.5e3", 7));
    EXPECT_EQ(lh_real_decimal_get_mantissa(&d), 125U);
    EXPECT_EQ(lh_real_decimal_get_exponent(&d), 2);
    EXPECT_TRUE(lh_real_decimal_is_negative(&d));
    EXPECT_FALSE(lh_real_decimal_is_truncated(&d));

    ASSERT_TRUE(lh_real_decimal_parse(&d, "0.00120", 7));
    EXPECT_EQ(lh_real_decimal_get_mantissa(&d), 120U);
    EXPECT_EQ(lh_real_decimal_get_exponent(&d), -5);
}

TEST(real_decimal, keeps_nineteen_digits_and_notes_the_rest)
{
    lh_real_decimal_t d;
    const std::string many = "12345678901234567890123";

    ASSERT_TRUE(lh_real_decimal_parse(&d, many.data(), many.size()));
    EXPECT_EQ(lh_real_decimal_get_mantissa(&d), 1234567890123456789ULL);
    EXPECT_EQ(lh_real_decimal_get_exponent(&d), 4);
    EXPECT_TRUE(lh_real_decimal_is_truncated(&d));

    const std::string zeros = "1000000000000000000000000";
    ASSERT_TRUE(lh_real_decimal_parse(&d, zeros.data(), zeros.size()));
    EXPECT_FALSE(lh_real_decimal_is_truncated(&d));
}

TEST(real_decimal, accepts_and_rejects_forms)
{
    lh_real_decimal_t d;
    for (const char *ok : {"0", "-0", ".5", "5.", "1e5", "1E+5", "1e-5", "007", "0.000"})
    {
        EXPECT_TRUE(lh_real_decimal_parse(&d, ok, std::strlen(ok))) << ok;
    }
    for (const char *bad :
         {"", "-", ".", "e5", "1e", "1e+", "+1", " 1", "1 ", "1.2.3", "inf", "nan", "0x10", "1,5"})
    {
        EXPECT_FALSE(lh_real_decimal_parse(&d, bad, std::strlen(bad))) << bad;
    }
}

/* ── parse ───────────────────────────────────────────────────────────────── */

TEST(str_ptr_parse_real, agrees_with_libc_on_hard_cases)
{
    for (const char *text : {
             "0",
             "-0",
             "1",
             "0.1",
             "0.2",
             "0.3",
             "1.5",
             "3.14159",
             "100",
             "1e23",
             "8.5e-5",
             "9007199254740993",        /* 2^53 + 1: a tie, to even */
             "9007199254740995",        /* 2^53 + 3: a tie, up to even */
             "2.2250738585072011e-308", /* the largest subnormal, almost */
             "2.2250738585072014e-308", /* the smallest normal */
             "4.9406564584124654e-324", /* the smallest subnormal */
             "2.4703282292062327e-324", /* just under half of it: zero */
             "2.4703282292062328e-324", /* just over half: the smallest */
             "1.7976931348623157e308",  /* the largest double */
             "1.7976931348623158e308",  /* still rounds to it */
             "1.7976931348623159e308",  /* infinity: fails */
             "1e-400",
             "1e400",
             "-1e400",
             "123456789012345678",
             "0.000000000000000000000001",
             "3.4028234663852886e38", /* the largest float */
             "3.4028235677973366e38", /* float infinity */
             "1.401298464324817e-45", /* the smallest float */
             "7.006492321624085e-46", /* half of it */
             "1.17549435e-38",
             "16777217",
             "33554435",
             "1e38",
             "1e39",
             "1e-46",
             "7e-46",
         })
    {
        EXPECT_EQ(parsed_bits(text), reference_bits(text)) << text;
    }
}

TEST(str_ptr_parse_real, agrees_with_libc_on_random_numbers)
{
    std::mt19937_64 rng(20261022U);
    char buf[64];

    for (int i = 0; i < 200000; ++i)
    {
        /* A random finite value, printed with a random number of digits. */
        lh_u64_t bits = rng();
#    if LH_LIBRARY_OPTION_REAL == LH_REAL_FLOAT
        bits &= 0xFFFFFFFFU;
#    endif
        const lh_real_t value = lh_real_from_bits(bits);
        if (!std::isfinite(value))
        {
            continue;
        }
        const int digits = 1 + static_cast<int>(rng() % 19U);
        std::snprintf(buf, sizeof(buf), "%.*e", digits - 1, static_cast<double>(value));
        ASSERT_EQ(parsed_bits(buf), reference_bits(buf)) << buf;
    }
}

TEST(str_ptr_parse_real, agrees_with_libc_on_random_digit_strings)
{
    std::mt19937_64 rng(42U);

    for (int i = 0; i < 200000; ++i)
    {
        /* Up to 19 digits (exact territory), any exponent the type can hold. */
        std::string text = std::to_string(rng() % 10000000000000000000ULL);
        text += "e" + std::to_string(static_cast<int>(rng() % 700U) - 360);
        ASSERT_EQ(parsed_bits(text), reference_bits(text)) << text;
    }
}

TEST(str_ptr_parse_real, rejects_what_is_not_a_number)
{
    lh_real_t value = 7;
    EXPECT_FALSE(parse("abc", &value));
    EXPECT_FALSE(parse("1.0x", &value));
    EXPECT_FALSE(parse("", &value));
    EXPECT_EQ(value, 7);
}

/* ── format ──────────────────────────────────────────────────────────────── */

#    if LH_LIBRARY_OPTION_REAL == LH_REAL_DOUBLE
TEST(str_ptr_format_real, lays_out_like_javascript)
{
    EXPECT_EQ(format(0.0), "0");
    EXPECT_EQ(format(-0.0), "-0");
    EXPECT_EQ(format(1.0), "1");
    EXPECT_EQ(format(-1.5), "-1.5");
    EXPECT_EQ(format(0.1), "0.1");
    EXPECT_EQ(format(0.1 + 0.2), "0.30000000000000004");
    EXPECT_EQ(format(100.0), "100");
    EXPECT_EQ(format(123.456), "123.456");
    EXPECT_EQ(format(0.000001), "0.000001");
    EXPECT_EQ(format(1e-7), "1e-7");
    EXPECT_EQ(format(1.5e-7), "1.5e-7");
    EXPECT_EQ(format(1e20), "100000000000000000000");
    EXPECT_EQ(format(1e21), "1e+21");
    EXPECT_EQ(format(1.7976931348623157e308), "1.7976931348623157e+308");
    EXPECT_EQ(format(5e-324), "5e-324");
    EXPECT_EQ(format(-2.2250738585072014e-308), "-2.2250738585072014e-308");
}
#    else
TEST(str_ptr_format_real, gives_the_shortest_float_digits)
{
    EXPECT_EQ(format(0.1f), "0.1");
    EXPECT_EQ(format(16777216.0f), "16777216");
    EXPECT_EQ(format(3.4028235e38f), "3.4028235e+38");
    EXPECT_EQ(format(1e-45f), "1e-45");
}
#    endif

TEST(str_ptr_format_real, refuses_infinity_and_nan_and_a_short_buffer)
{
    char buf[4];
    EXPECT_EQ(format(std::numeric_limits<lh_real_t>::infinity()), "");
    EXPECT_EQ(format(-std::numeric_limits<lh_real_t>::infinity()), "");
    EXPECT_EQ(format(std::numeric_limits<lh_real_t>::quiet_NaN()), "");
    EXPECT_EQ(lh_str_ptr_format_real(static_cast<lh_real_t>(123.5), buf, sizeof(buf)), 0U);
    EXPECT_EQ(lh_str_ptr_format_real(static_cast<lh_real_t>(12.5), buf, sizeof(buf)), 4U);
}

TEST(str_ptr_format_real, every_number_reads_back_and_is_nearly_always_shortest)
{
    std::mt19937_64 rng(7U);
    int longer = 0;
    int total = 0;

    for (int i = 0; i < 300000; ++i)
    {
        lh_u64_t bits = rng();
#    if LH_LIBRARY_OPTION_REAL == LH_REAL_FLOAT
        bits &= 0xFFFFFFFFU;
#    endif
        const lh_real_t value = lh_real_from_bits(bits);
        if (!std::isfinite(value))
        {
            continue;
        }
        const std::string text = format(value);
        ASSERT_FALSE(text.empty());
        /* Exactly the same number back, through libc. */
        ASSERT_EQ(lh_real_to_bits(reference(text)), lh_real_to_bits(value)) << text;
        /* And through our own parser. */
        ASSERT_EQ(parsed_bits(text), std::to_string(lh_real_to_bits(value))) << text;

        char shortest[64];
        const auto result = std::to_chars(shortest, shortest + sizeof(shortest), value,
                                          std::chars_format::scientific);
        /* Digits only: drop the sign, the point and the exponent. */
        int reference_digits = 0;
        for (char *p = shortest; p < result.ptr && *p != 'e'; ++p)
        {
            reference_digits += (*p >= '0' && *p <= '9') ? 1 : 0;
        }
        const std::string mantissa = text.substr(0, text.find('e'));
        std::string sig;
        for (char c : mantissa)
        {
            if (c >= '0' && c <= '9')
            {
                sig += c;
            }
        }
        /* Leading and trailing zeros are layout, not digits. */
        while (!sig.empty() && sig.front() == '0')
        {
            sig.erase(sig.begin());
        }
        while (!sig.empty() && sig.back() == '0')
        {
            sig.pop_back();
        }
        const int ours = static_cast<int>(sig.size());
        ASSERT_GE(ours, reference_digits) << text;
        longer += ours > reference_digits ? 1 : 0;
        ++total;
    }
    /* Grisu2 misses the shortest digits for a fraction of a percent of inputs. */
    EXPECT_LT(longer * 100, total) << longer << " of " << total << " not shortest";
}

} // namespace

#endif /* LH_REAL_ENABLED */
