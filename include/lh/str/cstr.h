/**
 * @file str/cstr.h
 * @brief Two questions asked of a NUL-terminated `const char *` (::lh_str_len,
 *        ::lh_str_eq).
 *
 * **Neither of these is new logic.** `lh/util/str/ptr.h` has had
 * ::lh_str_ptr_len and ::lh_str_ptr_equals from the start, and they are where a length
 * is measured and where two strings are compared. What they take is
 * ::lh_str_ptr — a **mutable** `char *` — because most of what they are for is trimming,
 * erasing and writing: a buffer you are about to change.
 *
 * An application is not in that position. A label's text, a card's `tag`, the result of
 * `getenv`, a comparison in a `switch`: all `const char *`, all read-only, all the ones
 * where you want to ask how long something is or whether two things are the same. Handing
 * the mutable spelling a read-only pointer means a cast that says "this is const, and I am
 * not going to write through it" **backwards**, or a hand-rolled `strlen` in the
 * application, which is the same scan written again in every project that needs one.
 *
 * So these are the const spellings, and they delegate. There is one implementation of
 * "how long is this string" and one of "when are these two the same", they are the ones
 * already tested in `lh/util/str/ptr.c`, and a change to them lands here too. Reading both
 * this header and `lh/util/str/ptr.h` will show you the same two questions answered twice;
 * that is deliberate, and this is the answer written in terms of the type you have.
 *
 * @see lh_str_ptr_len
 * @see lh_str_ptr_equals
 * @see lh_str_view_init — takes a `const char *` too, and measures it the same way
 */

#ifndef LH_STR_CSTR_H
#define LH_STR_CSTR_H

#include <lh/compiler/extern/c.h>
#include <lh/bool.h>
#include <lh/size.h>
#include <lh/str/ptr.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief How many characters @p str has before its terminator.
 *
 * Delegated to ::lh_str_ptr_len, including what happens when there is no terminator: that
 * is a **contract violation and it terminates** (::lh_runtime_error_code_no_null_terminator),
 * which is the answer this library gives everywhere else it is handed a string that ends
 * nowhere. A length that quietly came back as a plausible number would be worse than a
 * crash -- a register's tag that is one byte short prints as the right row with the wrong
 * name.
 *
 * @param str NUL-terminated string. **Not** ::lh_null: a null pointer is a bug in the
 *            caller, and ::lh_str_view_init says so in the same words.
 *
 * @return Number of characters before the terminator. `0` for `""`, which is a **present**
 *         empty string and not a missing one.
 */
lh_usize_t
lh_str_len(lh_str_cptr str);

/**
 * @brief Whether @p lhs and @p rhs are the same string.
 *
 * The third parameter is ::lh_str_ptr_equals' own, kept so the two spellings are a matched
 * pair: `ignore_case` answers "are these the same name" rather than "are these the same
 * bytes", which is the question when the two strings are a tag off a device and the one
 * the user typed. Adding it here and not there would give the library two answers to one
 * question.
 *
 * @param lhs         NUL-terminated string, not ::lh_null.
 * @param rhs         NUL-terminated string, not ::lh_null.
 * @param ignore_case Non-zero to fold case.
 *
 * @return ::lh_bool_true when equal. Two empty strings are equal; a null pointer is not
 *         an empty string here, for the reason ::lh_str_len gives.
 */
lh_bool_t
lh_str_eq(lh_str_cptr lhs, lh_str_cptr rhs, lh_bool_t ignore_case);

LH_COMPILER_EXTERN_C_END

#endif /* LH_STR_CSTR_H */
