/**
 * @file key.h
 * @brief Backend-private: X keysyms as ::lh_key_t and as text.
 *
 * A keysym is what X says a key means once the keyboard layout and Shift have
 * been applied (`XLookupString`): `0xff0d` is Return, `0x6c1` is Cyrillic `а`.
 * The portable keys come from the first table; the text a key types comes from
 * its keysym as well, because core X has no other place to get it from.
 */

#ifndef LH_SRC_OS_SYSTEM_POSIX_KEY_H
#define LH_SRC_OS_SYSTEM_POSIX_KEY_H

#include <lh/compiler/extern/c.h>
#include <lh/key.h>
#include <lh/numeric/fixed/types.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The portable key for @p keysym, or ::lh_key_other.
 */
lh_key_t
lh_os_system_posix_key_from_keysym(unsigned long keysym);

/**
 * @brief The code point @p keysym types, or `0` when it types nothing.
 *
 * Latin-1, the Unicode keysyms (`0x01000000` + code point) and the Cyrillic
 * block. Control characters are not text: Return, Tab and Backspace are keys.
 */
lh_u32_t
lh_os_system_posix_text_from_keysym(unsigned long keysym);

LH_COMPILER_EXTERN_C_END

#endif /* LH_SRC_OS_SYSTEM_POSIX_KEY_H */
