/**
 * @file js.h
 * @brief lh's own JavaScript engine: the lexer, and what comes after it.
 *
 * The engine is lh's code on lh's bricks — no other library under it. It
 * reads JavaScript source and runs it:
 *
 * 1. the lexer (:lh/js/lexer.h) — source text in, tokens out;
 * 2. the tree — the nodes the tokens make;
 * 3. the parser — the tree out of the tokens;
 * 4. the values — what a node holds;
 * 5. the interpreter — what running a node does;
 * 6. the runtime — the objects around them.
 *
 * This is the first slice: the lexer. It recognizes names, keywords, numbers,
 * strings with their escapes, comments, the operators and the brackets of
 * JavaScript, and the line terminators the parser's automatic semicolon
 * insertion needs to see. A regular expression literal is the parser's
 * business: only it knows whether a `/` divides or opens one.
 *
 * The numbers are ::lh_real_t, which is binary64 when the library is built
 * with ::LH_LIBRARY_OPTION_REAL = ::LH_REAL_DOUBLE — the default, and the
 * format the JavaScript number has.
 *
 * @see lh/js/lexer.h
 */

#ifndef LH_JS_H
#define LH_JS_H

#include <lh/js/lexer.h>

#endif /* LH_JS_H */
