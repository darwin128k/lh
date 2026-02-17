/**
 * @file jump_buffer.h
 * @brief Definition of a data type
 *        for using a jump buffer with setjmp/longjmp.
 *
 * This file contains the definition of the `lh_jump_buffer_t` type,
 * which is an alias for the standard `jmp_buf` type used to save
 * the program's execution state when working with setjmp and longjmp functions.
 */

#ifndef LH_JUMP_BUFFER_H
#define LH_JUMP_BUFFER_H

#include <setjmp.h>

/**
 * @typedef lh_jump_buffer_t
 * @brief Type used to store the program's execution state.
 *
 * The `lh_jump_buffer_t` type is an alias for `jmp_buf`,
 * which is used to save and restore the program's execution state
 * when working with setjmp() and longjmp() functions.
 */
typedef jmp_buf lh_jump_buffer_t;

#endif // LH_JUMP_BUFFER_H