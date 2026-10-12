/**
 * @file fields.h
 * @brief Member fields of ::lh_os_system_win_dcb_t.
 */

#ifndef LH_SRC_OS_SYSTEM_WIN_KERNEL32_DCB_FIELDS_H
#define LH_SRC_OS_SYSTEM_WIN_KERNEL32_DCB_FIELDS_H

/**
 * @def lh_os_system_win_dcb_fields(dword_type, word_type, byte_type, char_type)
 * @brief `DCB`, a serial port's settings. Win32 declares the third DWORD as fifteen
 *        bit fields (fBinary, fParity, fDtrControl, ...); they are mirrored as the
 *        one `flags` word they occupy, and set with the `LH_OS_SYSTEM_WIN_DCB_*` bits.
 *
 * @param dword_type `DWORD` (::lh_os_system_win_dword_t).
 * @param word_type  `WORD` (::lh_os_system_win_word_t).
 * @param byte_type  `BYTE` (::lh_byte_t).
 * @param char_type  `char` (::lh_char_t).
 */
#define lh_os_system_win_dcb_fields(dword_type, word_type, byte_type, char_type) \
    dword_type DCBlength;                                                                          \
    dword_type BaudRate;                                                                           \
    dword_type flags;                                                                              \
    word_type wReserved;                                                                           \
    word_type XonLim;                                                                              \
    word_type XoffLim;                                                                             \
    byte_type ByteSize;                                                                            \
    byte_type Parity;                                                                              \
    byte_type StopBits;                                                                            \
    char_type XonChar;                                                                             \
    char_type XoffChar;                                                                            \
    char_type ErrorChar;                                                                           \
    char_type EofChar;                                                                             \
    char_type EvtChar;                                                                             \
    word_type wReserved1

#endif /* LH_SRC_OS_SYSTEM_WIN_KERNEL32_DCB_FIELDS_H */
