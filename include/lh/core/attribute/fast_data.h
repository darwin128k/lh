/**
 * @file fast_data.h
 * @brief Library attribute for a constant table that is read in a hot loop:
 *        ::LH_ATTRIBUTE_FAST_DATA.
 *
 * On a microcontroller, constants live in flash, and flash at full clock answers
 * with wait states: an STM32F407 at 168 MHz waits 5 cycles for every read its small
 * flash data cache misses. A 256-byte lookup table read at random (an AES S-box)
 * misses almost every time. Copied to RAM, the same read takes one or two cycles.
 *
 * The build decides where such tables go: ::LH_LIBRARY_OPTION_FAST_DATA_SECTION
 * names a section, and the board's linker script and startup code put that
 * section in RAM (a `.data.*` name is copied from flash at start like any
 * initialized variable). Without the option the attribute expands to nothing and
 * the table stays where the compiler puts constants, as on a PC.
 *
 * Mark only tables a hot loop reads by index; a table read once costs RAM for no
 * gain. Every marked object is `const`, so the section never mixes kinds.
 */

#ifndef LH_ATTRIBUTE_FAST_DATA_H
#define LH_ATTRIBUTE_FAST_DATA_H

#include <lh/config.h>
#include <lh/core/compiler/attribute/section.h>

/**
 * @def LH_ATTRIBUTE_FAST_DATA
 * @brief Put the decorated constant table in the fast data section, when the build
 *        names one (::LH_LIBRARY_OPTION_FAST_DATA_SECTION); nothing otherwise.
 *
 * @code{.c}
 * LH_ATTRIBUTE_FAST_DATA const lh_byte_t m_sbox[256] = {...};
 * @endcode
 */
#if (LH_LIBRARY_OPTION_HAVE_FAST_DATA_SECTION)
#    define LH_ATTRIBUTE_FAST_DATA                                                                 \
        LH_COMPILER_ATTRIBUTE_SECTION(LH_LIBRARY_OPTION_FAST_DATA_SECTION)
#else
#    define LH_ATTRIBUTE_FAST_DATA
#endif

#endif /* LH_ATTRIBUTE_FAST_DATA_H */
