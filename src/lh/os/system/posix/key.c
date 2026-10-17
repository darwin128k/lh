/**
 * @file key.c
 * @brief X keysyms as portable keys and as text.
 */

#include <lh/os/system/posix/key.h>

#include <lh/util/return.h>

/** Keysym and portable key, side by side; both Shift, Control and Alt keys are
   the one modifier, as on Windows. */
static const unsigned long lh_os_system_posix_key_keysyms[] = {
    0xff09UL, 0xff0dUL, 0xff8dUL, 0xff1bUL, 0xff08UL, 0xffffUL, 0xff9fUL, 0x0020UL,
    0xff51UL, 0xff53UL, 0xff52UL, 0xff54UL, 0xff50UL, 0xff57UL, 0xff55UL, 0xff56UL,
    0xffe1UL, 0xffe2UL, 0xffe3UL, 0xffe4UL, 0xffe9UL, 0xffeaUL,
};
/** The portable key of each entry of ::lh_os_system_posix_key_keysyms, in the same order. */
static const lh_key_t lh_os_system_posix_key_keys[] = {
    lh_key_tab,       lh_key_enter,  lh_key_enter, lh_key_escape,  lh_key_backspace,
    lh_key_delete,    lh_key_delete, lh_key_space, lh_key_left,    lh_key_right,
    lh_key_up,        lh_key_down,   lh_key_home,  lh_key_end,     lh_key_page_up,
    lh_key_page_down, lh_key_shift,  lh_key_shift, lh_key_control, lh_key_control,
    lh_key_alt,       lh_key_alt,
};

/** keysymdef.h, 0x6a1..0x6bf: the letters of Serbian, Macedonian, Ukrainian and
   Belarusian that are not in the Russian alphabet, Ё, and the numero sign. */
static const lh_u16_t lh_os_system_posix_key_cyrillic_extra[] = {
    0x0452, 0x0453, 0x0451, 0x0454, 0x0455, 0x0456, 0x0457, 0x0458, 0x0459, 0x045a, 0x045b,
    0x045c, 0x0491, 0x045e, 0x045f, 0x2116, 0x0402, 0x0403, 0x0401, 0x0404, 0x0405, 0x0406,
    0x0407, 0x0408, 0x0409, 0x040a, 0x040b, 0x040c, 0x0490, 0x040e, 0x040f,
};

/** keysymdef.h, 0x6c0..0x6df: the lower-case Russian alphabet in the order X has
   it, which is not the alphabet's -- ю а б ц д е ф г х и й ... -- and 0x6e0..0x6ff
   is the same letters in upper case, 0x20 below in Unicode. */
static const lh_u16_t lh_os_system_posix_key_cyrillic[] = {
    0x044e, 0x0430, 0x0431, 0x0446, 0x0434, 0x0435, 0x0444, 0x0433, 0x0445, 0x0438, 0x0439,
    0x043a, 0x043b, 0x043c, 0x043d, 0x043e, 0x043f, 0x044f, 0x0440, 0x0441, 0x0442, 0x0443,
    0x0436, 0x0432, 0x044c, 0x044b, 0x0437, 0x0448, 0x044d, 0x0449, 0x0447, 0x044a,
};

lh_key_t
lh_os_system_posix_key_from_keysym(unsigned long keysym)
{
    lh_u32_t i;

    for (i = 0U;
         i < sizeof(lh_os_system_posix_key_keysyms) / sizeof(lh_os_system_posix_key_keysyms[0]);
         ++i)
    {
        lh_return_if(lh_os_system_posix_key_keysyms[i] == keysym, lh_os_system_posix_key_keys[i]);
    }
    return lh_key_other;
}

lh_u32_t
lh_os_system_posix_text_from_keysym(unsigned long keysym)
{
    /* Latin-1 keysyms are their own code points, the control range excepted. */
    if ((keysym >= 0x20UL && keysym <= 0x7eUL) || (keysym >= 0xa0UL && keysym <= 0xffUL))
    {
        return (lh_u32_t)keysym;
    }
    /* A keysym for any code point at all: 0x01000000 + the code point. */
    if (keysym >= 0x01000020UL && keysym <= 0x0110ffffUL && keysym != 0x0100007fUL)
    {
        return (lh_u32_t)(keysym - 0x01000000UL);
    }
    if (keysym >= 0x6a1UL && keysym <= 0x6bfUL)
    {
        return lh_os_system_posix_key_cyrillic_extra[keysym - 0x6a1UL];
    }
    if (keysym >= 0x6c0UL && keysym <= 0x6dfUL)
    {
        return lh_os_system_posix_key_cyrillic[keysym - 0x6c0UL];
    }
    if (keysym >= 0x6e0UL && keysym <= 0x6ffUL)
    {
        return (lh_u32_t)(lh_os_system_posix_key_cyrillic[keysym - 0x6e0UL] - 0x20U);
    }
    return 0U;
}
