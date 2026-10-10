/**
 * @file str/cstr.cpp
 * @brief Tests for the const C-string spellings, ::lh_str_len and ::lh_str_eq.
 *
 * **These are delegation tests, and that is the claim being made.** Both functions are one
 * call into `lh/util/str/ptr.c`, where the scan and the comparison already have their own
 * tests. What can be wrong here is not "does a string have a length" -- it is:
 *
 *  - the const spelling agrees with the mutable one on every case, which is what "it
 *    delegates" means when somebody reads only the signature;
 *  - a null pointer dies rather than reading one, and `""` is a **present** empty string
 *    and answers 0 / equal-to-`""` instead of dying;
 *  - `ignore_case` is the same answer `lh_str_ptr_equals` gives, on a case where the two
 *    spellings could have drifted apart;
 *  - the cast does not launder anything: `lh_str_ptr_len` taking `const` is a lie the
 *    compiler is allowed to believe, so the tests that would catch it are the ones on
 *    non-ASCII bytes rather than on `""`.
 *
 * The last one is the reason Cyrillic is in here at all. In ASCII every byte is a
 * character and a length is a length, so a length counted in bytes and one counted in code
 * points are the same number on every ASCII input -- and a test written only in ASCII stays
 * green over a wrong answer, which is the same trap as `lh_ui_text_encoded_size` and
 * `lh_ui_text_lead_size` disagreeing for every code point below `0x800`.
 */

#include <gtest/gtest.h>

#include <lh/bool.h>
#include <lh/char/map.h>
#include <lh/expect/death.h>
#include <lh/null.h>
#include <lh/str/cstr.h>
#include <lh/util/str/ptr.h>

namespace
{

/** Cyrillic in UTF-8: five characters, ten bytes.
 *
 *  An **array**, not a bare pointer, so the byte count is `sizeof` and not a number
 *  somebody typed. The first version of this file called these same bytes "four
 *  characters, eight bytes" and asserted 8, and the test went red against a function that
 *  was right: a hand-counted constant in a test about counting is a test that reports the
 *  author's own arithmetic. The character count is still written out in words, because
 *  "bytes and characters are different numbers" is the claim and it needs both.
 */
const lh_char_t CYRILLIC[] = "\xD0\x9A\xD0\xBE\xD1\x82\xD0\xBE\xD1\x80";

/** How many characters @p s has, measured the other way -- by hand, over the bytes. */
lh_usize_t
manually(const lh_char_t *s)
{
    lh_usize_t n = 0;

    while (s[n] != lh_char_map_nul)
    {
        ++n;
    }
    return n;
}

} // namespace

/* -- the denominator, before any verdict ------------------------------------- */

TEST(str_len, agrees_with_the_mutable_spelling_on_every_case)
{
    /* The claim "it delegates" is only worth something if the two spellings cannot differ,
       and they cannot differ on a case where they agree. These are the cases an
       application actually has: a tag, a label, an environment variable, a name. */
    static const char *const cases[] = {
        "", "a", "ab", "abc", "  padded  ", "0x1F40", "device:5030", "vav0_task_pqv",
    };
    const size_t count = sizeof(cases) / sizeof(cases[0]);

    std::printf("DENOMINATOR %zu cases, both spellings\n", count);
    for (size_t i = 0; i < count; ++i)
    {
        EXPECT_EQ(lh_str_len(cases[i]), lh_str_ptr_len(cases[i]))
            << "case " << i << ": " << cases[i];
        EXPECT_EQ(lh_str_len(cases[i]), manually(cases[i])) << "case " << i;
    }
}

TEST(str_len, counts_bytes_and_not_characters)
{
    /* **Bytes**, and that is the answer on purpose: the same one ::lh_str_ptr_len gives,
       and the one every buffer in this library is sized in. A length in code points would
       be a second answer to the same question, and an application would not know which one
       it had been given. */
    const size_t bytes = sizeof(CYRILLIC) - 1u;

    EXPECT_EQ(lh_str_len(CYRILLIC), bytes);
    EXPECT_EQ(lh_str_len(CYRILLIC), lh_str_ptr_len(CYRILLIC));

    /* Five characters is not ten bytes and not four either: the second half of the claim
       is that the answer is *not* the character count, which is the one a person reading
       the string would give. */
    EXPECT_NE(lh_str_len(CYRILLIC), 5u);
}

TEST(str_len, an_empty_string_is_zero_and_a_present_value)
{
    EXPECT_EQ(lh_str_len(""), 0u);
    EXPECT_EQ(lh_str_len(""), lh_str_ptr_len(""));
}

TEST(str_len_death, a_null_pointer_is_a_bug_not_an_empty_string)
{
    /* The asymmetry is the point: `""` is 0, and null **dies**. A null that answered 0
       would be indistinguishable from an empty string, and "no device name" and "the
       device's name is empty" are two different bugs. ::lh_str_view_init says the same
       thing in the same words. */
    EXPECT_DEATH(lh_str_len(lh_null), "");
}

TEST(str_eq, agrees_with_the_mutable_spelling)
{
    static const char *const cases[][2] = {
        {"", ""},
        {"a", "a"},
        {"a", "b"},
        {"abc", "abc"},
        {"abc", "abcd"},
        {"JL205", "JL205"},
        {"vav0_status", "vav0_param"},
        {CYRILLIC, CYRILLIC},
    };
    const size_t count = sizeof(cases) / sizeof(cases[0]);

    std::printf("DENOMINATOR %zu pairs, both spellings, both cases\n", count * 2u);
    for (size_t i = 0; i < count; ++i)
    {
        for (int fold = 0; fold < 2; ++fold)
        {
            const lh_bool_t ignore_case = fold ? lh_bool_true : lh_bool_false;

            EXPECT_EQ(lh_str_eq(cases[i][0], cases[i][1], ignore_case),
                      lh_str_ptr_equals(cases[i][0], cases[i][1], ignore_case))
                << "pair " << i << ", ignore_case " << fold;
        }
    }
}

TEST(str_eq, is_symmetric)
{
    /* A comparison that is not symmetric answers different questions depending on which
       side the operands were written, and it is the kind of thing that only shows up in a
       `!=` written the other way round. */
    static const char *const pairs[][2] = {
        {"abc", "abd"}, {"", "a"}, {"VAV0", "vav0"}, {CYRILLIC, CYRILLIC},
    };
    const size_t count = sizeof(pairs) / sizeof(pairs[0]);

    for (size_t i = 0; i < count; ++i)
    {
        for (int fold = 0; fold < 2; ++fold)
        {
            const lh_bool_t ignore_case = fold ? lh_bool_true : lh_bool_false;

            EXPECT_EQ(lh_str_eq(pairs[i][0], pairs[i][1], ignore_case),
                      lh_str_eq(pairs[i][1], pairs[i][0], ignore_case))
                << "pair " << i << ", ignore_case " << fold;
        }
    }
}

TEST(str_eq, ignore_case_is_the_same_answer_the_ptr_spelling_gives)
{
    EXPECT_FALSE(lh_str_eq("VAV0_status", "vav0_status", lh_bool_false));
    EXPECT_TRUE(lh_str_eq("VAV0_status", "vav0_status", lh_bool_true));
    EXPECT_TRUE(lh_str_eq("Holding", "holding", lh_bool_true));

    /* And the two spellings still agree after the cast, which is where a drifting
       `ignore_case` would show. */
    EXPECT_EQ(lh_str_eq("VAV0_status", "vav0_status", lh_bool_true),
              lh_str_ptr_equals("VAV0_status", "vav0_status", lh_bool_true));
}

TEST(str_eq, two_empty_strings_are_equal)
{
    EXPECT_TRUE(lh_str_eq("", "", lh_bool_false));
    EXPECT_TRUE(lh_str_eq("", "", lh_bool_true));
    EXPECT_FALSE(lh_str_eq("", "a", lh_bool_false));
    EXPECT_FALSE(lh_str_eq("a", "", lh_bool_false));
}

TEST(str_eq, a_prefix_is_not_the_string)
{
    /* The one that `strncmp` gets wrong when somebody reaches for it by habit: "abc" is
       not "abcd", and a comparison that says otherwise makes a device called "JL205" equal
       to "JL2050". */
    EXPECT_FALSE(lh_str_eq("abc", "abcd", lh_bool_false));
    EXPECT_FALSE(lh_str_eq("JL205", "JL2050", lh_bool_false));
    EXPECT_TRUE(lh_str_eq("JL205", "JL205", lh_bool_false));
}

TEST(str_eq_death, a_null_operand_is_a_bug)
{
    EXPECT_DEATH(lh_str_eq(lh_null, "a", lh_bool_false), "");
    EXPECT_DEATH(lh_str_eq("a", lh_null, lh_bool_false), "");
}
