#include <gtest/gtest.h>

#include <lh/key.h>
#include <lh/numeric/fixed/types.h>

/* Backend-private (src/lh/os/system/posix/key.h), so declared here: the test
   includes only the public tree. */
extern "C" lh_key_t
lh_os_system_posix_key_from_keysym(unsigned long keysym);
extern "C" lh_u32_t
lh_os_system_posix_text_from_keysym(unsigned long keysym);

namespace
{

TEST(os_system_posix_key, the_keys_the_ui_listens_for_are_known)
{
    EXPECT_EQ(lh_os_system_posix_key_from_keysym(0xff0dUL), lh_key_enter) << "Return";
    EXPECT_EQ(lh_os_system_posix_key_from_keysym(0xff8dUL), lh_key_enter) << "the keypad's Enter too";
    EXPECT_EQ(lh_os_system_posix_key_from_keysym(0xff08UL), lh_key_backspace);
    EXPECT_EQ(lh_os_system_posix_key_from_keysym(0xff51UL), lh_key_left);
    EXPECT_EQ(lh_os_system_posix_key_from_keysym(0xffe2UL), lh_key_shift) << "the right Shift is Shift";
    EXPECT_EQ(lh_os_system_posix_key_from_keysym(0x61UL), lh_key_other) << "a letter is text, not a key";
}

TEST(os_system_posix_key, latin_text_is_its_own_code_point)
{
    EXPECT_EQ(lh_os_system_posix_text_from_keysym(0x41UL), 0x41U);
    EXPECT_EQ(lh_os_system_posix_text_from_keysym(0xe9UL), 0xe9U) << "é, Latin-1";
    EXPECT_EQ(lh_os_system_posix_text_from_keysym(0xff0dUL), 0U) << "Return types nothing";
    EXPECT_EQ(lh_os_system_posix_text_from_keysym(0x7fUL), 0U) << "DEL is not text";
}

TEST(os_system_posix_key, russian_letters_type_russian_letters)
{
    /* X orders its Cyrillic keysyms ю а б ц ..., not by the alphabet, and that is
       where a table goes wrong without anybody noticing until a word is typed. */
    EXPECT_EQ(lh_os_system_posix_text_from_keysym(0x6c1UL), 0x0430U) << "а";
    EXPECT_EQ(lh_os_system_posix_text_from_keysym(0x6c0UL), 0x044eU) << "ю";
    EXPECT_EQ(lh_os_system_posix_text_from_keysym(0x6dfUL), 0x044aU) << "ъ";
    EXPECT_EQ(lh_os_system_posix_text_from_keysym(0x6e1UL), 0x0410U) << "А";
    EXPECT_EQ(lh_os_system_posix_text_from_keysym(0x6ffUL), 0x042aU) << "Ъ";
    EXPECT_EQ(lh_os_system_posix_text_from_keysym(0x6a3UL), 0x0451U) << "ё";
    EXPECT_EQ(lh_os_system_posix_text_from_keysym(0x6b3UL), 0x0401U) << "Ё";
    EXPECT_EQ(lh_os_system_posix_text_from_keysym(0x6b0UL), 0x2116U) << "№";
}

TEST(os_system_posix_key, a_unicode_keysym_is_its_code_point)
{
    EXPECT_EQ(lh_os_system_posix_text_from_keysym(0x010020acUL), 0x20acU) << "€";
    EXPECT_EQ(lh_os_system_posix_text_from_keysym(0x0100001fUL), 0U) << "a control character is not text";
}

} // namespace
