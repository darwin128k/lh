/**
 * @file self.h
 * @brief Pointer to the object an abstract callback acts on.
 *
 * An abstract callback (an allocator's alloc/dealloc/realloc, an io reader's
 * read, a logger's emit, ...) is a method: it runs on some object — an
 * allocator's state, a stream, a socket, a log sink — whose type only the
 * concrete implementation knows. ::lh_self_ptr names that parameter's type,
 * so the signature says "the object this method runs on" rather than "some
 * pointer".
 *
 * Only for such abstract callbacks. A concrete API that knows its struct
 * takes a typed pointer to it instead (`lh_array_t *self`, ...).
 */

#ifndef LH_SELF_H
#define LH_SELF_H

#include <lh/ptr.h>

/**
 * @def lh_self_ptr
 * @brief Pointer to some object, i.e. some struct, an abstract callback runs on.
 *
 * Untyped (expands to ::lh_ptr): the callback's implementation casts it to
 * its own struct.
 *
 * Example usage:
 * @code{.c}
 * static lh_ssize_t
 * my_read(lh_self_ptr self, lh_ptr buf, lh_usize_t size)
 * {
 *     struct my_stream *stream = (struct my_stream *)self;
 *     ...
 * }
 * @endcode
 */
#define lh_self_ptr lh_ptr

#endif /* LH_SELF_H */
