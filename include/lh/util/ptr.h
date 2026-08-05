/**
 * @file ptr.h
 * @brief Pointer helpers: type construction, casts, `memmove`-style layout tests,
 * byte offsets, and raw ::lh_uaddr_t address arithmetic.
 *
 * Includes `lh/util/type.h` so ::lh_ptr_cast
 * does not depend on transitive includes from `lh/ptr.h`.
 */

#ifndef LH_UTIL_PTR_H
#define LH_UTIL_PTR_H

#include <lh/null.h>
#include <lh/util/addr.h>

/**
 * @def lh_ptr_of(T)
 * @brief Pointer type to @p T.
 *
 * Expands to `T *`.
 * Used to construct pointer type expressions where a typedef is not applicable.
 *
 * @param T Base type.
 *
 * Example usage:
 * @code{.c}
 * lh_ptr_of(int)      // → int *
 * lh_ptr_of(lh_void)  // → void *
 * @endcode
 */
#define lh_ptr_of(T) T *

/**
 * @def lh_cptr_of(T)
 * @brief Pointer type to `const` @p T.
 *
 * Expands to `const T *` — the same expansion as `lh_ptr_of(const T)`.
 *
 * @param T Base type (without `const` or `*`).
 *
 * Example usage:
 * @code{.c}
 * lh_cptr_of(int)           // → const int *
 * lh_cptr_of(lh_schar_t)    // → const lh_schar_t *
 * @endcode
 */
#define lh_cptr_of(T) lh_ptr_of(const T)

/**
 * @def lh_ptr_cast(T, ptr)
 * @brief Cast @p ptr to `T *`.
 *
 * @param T   Target pointee type (without `*`).
 * @param ptr Pointer expression to cast.
 *
 * Example usage:
 * @code{.c}
 * lh_ptr_cast(int, p)   // → (int *)(p)
 * @endcode
 */
#define lh_ptr_cast(T, ptr) lh_type_cast(T *, ptr)

/**
 * @def lh_ptr_ccast(T, ptr)
 * @brief Cast @p ptr to `const T *`.
 *
 * @param T   Target pointee type (without `const` or `*`).
 * @param ptr Pointer expression to cast.
 *
 * Example usage:
 * @code{.c}
 * lh_ptr_ccast(char, p)   // → (const char *)(p)
 * @endcode
 */
#define lh_ptr_ccast(T, ptr) lh_ptr_cast(const T, ptr)

/**
 * @def lh_ptr_rcast(T, ptr)
 * @brief Reinterpret @p ptr as `T *`.
 *
 * Uses ::lh_cast_reinterpret: expands to `reinterpret_cast<T *>`
 * in C++ and to a C-style cast in C.
 *
 * @param T   Target pointee type (without `*`).
 * @param ptr Pointer expression to reinterpret.
 *
 * Example usage:
 * @code{.c}
 * lh_ptr_rcast(lh_uchar_t, &x)   // → (lh_uchar_t *)(&x)
 * @endcode
 */
#define lh_ptr_rcast(T, ptr) lh_cast_reinterpret(T *, ptr)

/**
 * @def lh_ptr_deref(ptr)
 * @brief Dereference @p ptr.
 *
 * Expands to `*ptr`.
 * The result type is the pointee type of @p ptr per C pointer semantics.
 *
 * @param ptr Pointer expression.
 *
 * Example usage:
 * @code{.c}
 * lh_ptr_deref(p)   // → (*p)
 * @endcode
 */
#define lh_ptr_deref(ptr) (*ptr)

/**
 * @def lh_ptr_deref_of(T, ptr)
 * @brief Dereference @p ptr as type @p T.
 *
 * Expands to ::lh_ptr_deref(::lh_ptr_cast(T, ptr)): casts @p ptr to `T *`, then dereferences.
 * The pointer subexpression is evaluated once.
 *
 * @param T   Type to dereference as.
 * @param ptr Pointer expression.
 *
 * Example usage:
 * @code{.c}
 * lh_ptr_deref_of(int, p)   // → *((int *)(p))
 * @endcode
 */
#define lh_ptr_deref_of(T, ptr) lh_ptr_deref(lh_ptr_cast(T, ptr))

/**
 * @def lh_ptr_is_null(ptr)
 * @brief Non-zero if @p ptr compares equal to ::LH_PTR_T_MIN (null ::lh_ptr).
 *
 * Example usage:
 * @code{.c}
 * lh_ptr p = LH_PTR_T_MIN;
 * if (lh_ptr_is_null(p)) {
 *     return;
 * }
 * @endcode
 */
#define lh_ptr_is_null(ptr) lh_null_eq(ptr)

/**
 * @def lh_ptr_is_set(ptr)
 * @brief Check whether @p ptr is not a null pointer (same as ::lh_null_ne).
 *
 * @param ptr Pointer expression.
 *
 * Example usage:
 * @code{.c}
 * if (lh_ptr_is_set(p)) { ... }
 * @endcode
 */
#define lh_ptr_is_set(ptr) lh_null_ne(ptr)

/**
 * @def lh_addr_to_ptr(T, addr)
 * @brief Reinterpret `addr` as a pointer to `T` (::lh_cast_reinterpret).
 *
 * @param T    Pointee type name (not parenthesized).
 * @param addr Integral or pointer expression stored in address-sized form.
 *
 * @return Expression of type `T *` after reinterpretation.
 *
 * @code{.c}
 * lh_uaddr_t raw = (lh_uaddr_t)0x1000;
 * int *p = lh_addr_to_ptr(int, raw);
 * @endcode
 */
#define lh_addr_to_ptr(T, addr) lh_cast_reinterpret(T *, addr)

/**
 * @def lh_addr_to_void(addr)
 * @brief Reinterpret `addr` as ::lh_void * (`void *`).
 *
 * @param addr Integral or pointer expression.
 *
 * @return Expression of type `void *`.
 *
 * @code{.c}
 * lh_uaddr_t raw = (lh_uaddr_t)0x2000;
 * void *vp = lh_addr_to_void(raw);
 * @endcode
 */
#define lh_addr_to_void(addr) lh_addr_to_ptr(lh_void, addr)

/**
 * @def lh_ptr_to_uaddr(ptr)
 * @brief Reinterpret @p ptr as ::lh_uaddr_t (::lh_cast_reinterpret).
 *
 * @see lh_ptr_to_saddr(), lh_ptr_to_uaddr() in `lh/util/addr.h`
 *      (same idea at the address layer).
 *
 * Example usage:
 * @code{.c}
 * int x;
 * lh_uaddr_t u = lh_ptr_to_uaddr(&x);
 * @endcode
 */
#define lh_ptr_to_uaddr(ptr) lh_cast_reinterpret(lh_uaddr_t, (ptr))

/**
 * @def lh_ptr_to_saddr(ptr)
 * @brief Reinterpret @p ptr as ::lh_saddr_t (::lh_cast_reinterpret).
 *
 * @see lh_ptr_to_uaddr()
 *
 * Example usage:
 * @code{.c}
 * double y;
 * lh_saddr_t s = lh_ptr_to_saddr(&y);
 * @endcode
 */
#define lh_ptr_to_saddr(ptr) lh_cast_reinterpret(lh_saddr_t, (ptr))

/**
 * @def lh_ptr_sdiff(ptr1, ptr2)
 * @brief Signed difference of two pointers
 *        after converting both to ::lh_saddr_t (::lh_addr_diff).
 *
 * Example usage:
 * @code{.c}
 * char buf[64];
 * char *a = &buf[10];
 * char *b = &buf[25];
 * lh_saddr_t n = lh_ptr_sdiff(b, a); // 15 (bytes on typical platforms)
 * @endcode
 */
#define lh_ptr_sdiff(ptr1, ptr2) lh_addr_diff(lh_ptr_to_saddr(ptr1), lh_ptr_to_saddr(ptr2))

/**
 * @def lh_ptr_udiff(ptr1, ptr2)
 * @brief Unsigned difference of two pointers
 * after converting both to ::lh_uaddr_t (::lh_addr_diff).
 *
 * @note Unsigned subtraction wraps modulo `2^width`;
 *       use ::lh_ptr_sdiff when you need a signed span.
 *
 * Example usage:
 * @code{.c}
 * unsigned char blob[32];
 * unsigned char *lo = &blob[4];
 * unsigned char *hi = &blob[12];
 * lh_uaddr_t d = lh_ptr_udiff(hi, lo); // 8
 * @endcode
 */
#define lh_ptr_udiff(ptr1, ptr2) lh_addr_diff(lh_ptr_to_uaddr(ptr1), lh_ptr_to_uaddr(ptr2))

/**
 * @def lh_ptr_eq(a, b)
 * @brief Non-zero if @p a and @p b compare equal (`a == b`).
 *
 * Example usage:
 * @code{.c}
 * unsigned char buf[8];
 * if (lh_ptr_eq(&buf[2], &buf[2])) {
 *     // same address
 * }
 * @endcode
 */
#define lh_ptr_eq(a, b) lh_addr_eq(lh_ptr_to_uaddr(a), lh_ptr_to_uaddr(b))

/**
 * @def lh_ptr_ne(a, b)
 * @brief Non-zero if @p a and @p b compare unequal (`a != b`).
 *
 * Example usage:
 * @code{.c}
 * unsigned char buf[8];
 * if (lh_ptr_ne(&buf[1], &buf[3])) {
 *     // different addresses
 * }
 * @endcode
 */
#define lh_ptr_ne(a, b) lh_addr_ne(lh_ptr_to_uaddr(a), lh_ptr_to_uaddr(b))

/**
 * @def lh_ptr_lt(a, b)
 * @brief Non-zero if @p a compares less than @p b (`a < b`).
 *
 * @note In ISO C, `<` / `>` on pointers is only defined
 *       when both point into the same array object (or one past its end).
 *       Otherwise the behavior is undefined.
 *
 * Example usage:
 * @code{.c}
 * unsigned char buf[8];
 * if (lh_ptr_lt(&buf[1], &buf[4])) {
 *     // buf + 1 is before buf + 4
 * }
 * @endcode
 */
#define lh_ptr_lt(a, b) lh_addr_lt(lh_ptr_to_uaddr(a), lh_ptr_to_uaddr(b))

/**
 * @def lh_ptr_gt(a, b)
 * @brief Non-zero if @p a compares greater than @p b (`a > b`).
 *
 * @note Same pointer comparison restrictions as ::lh_ptr_lt().
 *
 * Example usage:
 * @code{.c}
 * unsigned char buf[8];
 * if (lh_ptr_gt(&buf[6], &buf[2])) {
 *     // buf + 6 is after buf + 2
 * }
 * @endcode
 */
#define lh_ptr_gt(a, b) lh_addr_gt(lh_ptr_to_uaddr(a), lh_ptr_to_uaddr(b))

/**
 * @def lh_ptr_le(a, b)
 * @brief Non-zero if @p a compares less than or equal to @p b (`a <= b`).
 *
 * @note Same pointer comparison restrictions as ::lh_ptr_lt().
 *
 * Example usage:
 * @code{.c}
 * unsigned char buf[8];
 * if (lh_ptr_le(&buf[2], &buf[2])) {
 *     // equal addresses are accepted
 * }
 * @endcode
 */
#define lh_ptr_le(a, b) lh_addr_le(lh_ptr_to_uaddr(a), lh_ptr_to_uaddr(b))

/**
 * @def lh_ptr_ge(a, b)
 * @brief Non-zero if @p a compares greater than or equal to @p b (`a >= b`).
 *
 * @note Same pointer comparison restrictions as ::lh_ptr_lt().
 *
 * Example usage:
 * @code{.c}
 * unsigned char buf[8];
 * if (lh_ptr_ge(&buf[7], &buf[3])) {
 *     // buf + 7 is not before buf + 3
 * }
 * @endcode
 */
#define lh_ptr_ge(a, b) lh_addr_ge(lh_ptr_to_uaddr(a), lh_ptr_to_uaddr(b))

/**
 * @def lh_ptr_is_aligned(ptr, align)
 * @brief Non-zero if the address bits of @p ptr are aligned to @p align (::lh_addr_is_aligned).
 *
 * @param ptr   Pointer expression.
 * @param align Power-of-two alignment (integer, same rules as ::lh_addr_is_aligned).
 *
 * Example usage:
 * @code{.c}
 * struct object {
 *     unsigned char pad[64];
 * } obj;
 * if (lh_ptr_is_aligned(&obj, 16)) {
 *     // ...
 * }
 * @endcode
 */
#define lh_ptr_is_aligned(ptr, align) lh_addr_is_aligned(lh_ptr_to_uaddr(ptr), align)

/**
 * @def lh_ptr_is_forward_copy(dst, src, src_end)
 * @brief `memmove`-style branch: non-zero when a naive **low-to-high** byte copy is safe.
 *
 * Expands to `(dst <= src) || (src_end <= dst)` — i.e.
 * `dst <= src || src + n <= dst` when @p src_end is `src + n`.
 * Same predicate as in many `memmove` / `memcpy` sources when choosing direction.
 * Arguments: destination start @p dst, source start @p src, exclusive source end @p src_end.
 *
 * @note Pointer `<` / `<=` is only fully specified for pointers into the same object (or one past);
 *       this assumes a flat ordering where the comparison is meaningful (typical libc style).
 *
 * Example usage:
 * @code{.c}
 * char buf[64];
 * char *dst = &buf[10];
 * char *src = &buf[0];
 * char *src_end = &buf[32]; // src + 32 bytes
 * if (lh_ptr_is_forward_copy(dst, src, src_end)) {
 *     // forward copy OK for this layout
 * }
 * @endcode
 */
#define lh_ptr_is_forward_copy(dst, src, src_end) (lh_ptr_le(dst, src) || lh_ptr_le(src_end, dst))

/**
 * @def lh_ptr_is_backward_copy(dst, src, src_end)
 * @brief Complement of ::lh_ptr_is_forward_copy — non-zero
 *        when forward copy can clobber unread source (use backward copy or `memmove`).
 *
 * Example usage:
 * @code{.c}
 * char buf[64];
 * char *dst = &buf[4];
 * char *src = &buf[8];
 * char *src_end = &buf[24];
 * if (lh_ptr_is_backward_copy(dst, src, src_end)) {
 *     // copy high-to-low or call memmove
 * }
 * @endcode
 */
#define lh_ptr_is_backward_copy(dst, src, src_end) (!lh_ptr_is_forward_copy(dst, src, src_end))

/**
 * @def lh_ptr_add_by_offset_unsafe(T, ptr, offset)
 * @brief `ptr` advanced by @p offset bytes (via ::lh_uaddr_t); result has type `T *`.
 *
 * @note @p offset counts **bytes**, not elements. For `T` with `alignof(T) > 1`,
 *       the result may be misaligned for `T` unless @p offset is a multiple of the alignment
 *       — prefer `unsigned char *` for raw byte walking, or only use offsets you have proven safe.
 *
 * @warning Does not treat null specially; combining a null @p ptr
 *          with a non-zero @p offset is still undefined for downstream use.
 *
 * Example usage:
 * @code{.c}
 * unsigned char block[64];
 * unsigned char *p = &block[8];
 * unsigned char *q = lh_ptr_add_by_offset_unsafe(unsigned char, p, (lh_uaddr_t)16);
 * @endcode
 */
#define lh_ptr_add_by_offset_unsafe(T, ptr, offset)                                                \
    lh_addr_to_ptr(T, lh_math_add(lh_ptr_to_uaddr(ptr), offset))

/**
 * @def lh_ptr_sub_by_offset_unsafe(T, ptr, offset)
 * @brief `ptr` moved back by @p offset bytes (via ::lh_uaddr_t); result has type `T *`.
 *
 * @warning Same null / validity caveats as ::lh_ptr_add_by_offset_unsafe.
 *
 * Example usage:
 * @code{.c}
 * unsigned char block[64];
 * unsigned char *p = &block[40];
 * unsigned char *q = lh_ptr_sub_by_offset_unsafe(unsigned char, p, (lh_uaddr_t)8);
 * @endcode
 */
#define lh_ptr_sub_by_offset_unsafe(T, ptr, offset)                                                \
    lh_addr_to_ptr(T, lh_math_sub(lh_ptr_to_uaddr(ptr), offset))

/**
 * @def lh_ptr_add_by_offset(T, ptr, offset)
 * @brief Like ::lh_ptr_add_by_offset_unsafe,
 *        but if @p ptr is null (::lh_ptr_is_null),
 *        returns @p ptr unchanged.
 *
 * @note The ternary preserves @p ptr’s type when it is null;
 *        both branches should be compatible as a single pointer type
 *        in the calling context (same as any macro ternary).
 *
 * Example usage:
 * @code{.c}
 * lh_ptr z = LH_PTR_T_MIN;
 * lh_ptr r = lh_ptr_add_by_offset(lh_void, z, (lh_uaddr_t)8); // still null
 * @endcode
 */
#define lh_ptr_add_by_offset(T, ptr, offset)                                                       \
    (lh_ptr_is_null(ptr) ? ptr : lh_ptr_add_by_offset_unsafe(T, ptr, offset))

/**
 * @def lh_ptr_sub_by_offset(T, ptr, offset)
 * @brief Like ::lh_ptr_sub_by_offset_unsafe, but if @p ptr is null,
 *        returns @p ptr unchanged.
 *
 * Example usage:
 * @code{.c}
 * lh_ptr z = LH_PTR_T_MIN;
 * lh_ptr r = lh_ptr_sub_by_offset(lh_void, z, (lh_uaddr_t)4); // still null
 * @endcode
 */
#define lh_ptr_sub_by_offset(T, ptr, offset)                                                       \
    (lh_ptr_is_null(ptr) ? ptr : lh_ptr_sub_by_offset_unsafe(T, ptr, offset))

/**
 * @def lh_ptr_add_unsafe(T, ptr, rhs)
 * @brief `T *` from `::lh_ptr_to_uaddr(ptr) + ::lh_ptr_to_uaddr(rhs)`
 *        (::lh_ptr_add_by_offset_unsafe).
 *
 * This is **integer address** addition, not “pointer + index”.
 * Rare; know the layout you need.
 *
 * Example usage:
 * @code{.c}
 * // lh_addr_to_ptr(T, lh_ptr_to_uaddr(ptr) + lh_ptr_to_uaddr(rhs))
 * int x, y;
 * int *p = lh_ptr_add_unsafe(int, &x, &y);
 * @endcode
 */
#define lh_ptr_add_unsafe(T, ptr, rhs) lh_ptr_add_by_offset_unsafe(T, ptr, lh_ptr_to_uaddr(rhs))

/**
 * @def lh_ptr_sub_unsafe(T, ptr, rhs)
 * @brief `T *` from `::lh_ptr_to_uaddr(ptr) - ::lh_ptr_to_uaddr(rhs)`
 *        (::lh_ptr_sub_by_offset_unsafe).
 *
 * Example usage:
 * @code{.c}
 * // lh_addr_to_ptr(T, lh_ptr_to_uaddr(ptr) - lh_ptr_to_uaddr(rhs))
 * int x, y;
 * int *p = lh_ptr_sub_unsafe(int, &x, &y);
 * @endcode
 */
#define lh_ptr_sub_unsafe(T, ptr, rhs) lh_ptr_sub_by_offset_unsafe(T, ptr, lh_ptr_to_uaddr(rhs))

/**
 * @def lh_ptr_add(T, ptr, rhs)
 * @brief Like ::lh_ptr_add_unsafe, but if @p ptr is null (::lh_ptr_is_null), returns @p ptr.
 *
 * @note When @p ptr is null, @p rhs is not applied — there is no “null + address” path;
 *       only the null-preserving short-circuit runs.
 *
 * Example usage:
 * @code{.c}
 * int cell;
 * int *q = &cell;
 * lh_ptr z = LH_PTR_T_MIN;
 * int *r = lh_ptr_add(int, q, z); // z null → offset 0 → still &cell
 * @endcode
 */
#define lh_ptr_add(T, ptr, rhs) lh_ptr_add_by_offset(T, ptr, lh_ptr_to_uaddr(rhs))

/**
 * @def lh_ptr_sub(T, ptr, rhs)
 * @brief Like ::lh_ptr_sub_unsafe, but if @p ptr is null, returns @p ptr.
 *
 * @note Same short-circuit as ::lh_ptr_add:
 *       a null @p ptr returns immediately; @p rhs is ignored.
 *
 * Example usage:
 * @code{.c}
 * int cell;
 * int *q = &cell;
 * lh_ptr z = LH_PTR_T_MIN;
 * int *r = lh_ptr_sub(int, q, z); // z null → offset 0 → still &cell
 * @endcode
 */
#define lh_ptr_sub(T, ptr, rhs) lh_ptr_sub_by_offset(T, ptr, lh_ptr_to_uaddr(rhs))

/**
 * @def lh_ptr_align_up_grid(T, ptr, origin, grid)
 * @brief Advance @p ptr (by a non-negative byte delta) so the unsigned byte offset
 *        from @p origin to the result is a multiple of @p grid — i.e. align to a
 *        lattice anchored at @p origin, not at address zero.
 *
 * @param T       Result pointee type.
 * @param ptr     Pointer expression; for a meaningful unsigned span from @p origin,
 *                @p ptr is usually not below @p origin within the same object
 *                (see ::lh_ptr_udiff).
 * @param origin  Anchor for the grid (same pointer comparison caveats as ::lh_ptr_udiff).
 * @param grid    Step size; must be non-zero (division in ::lh_math_mod).
 *
 * @note Unlike ::lh_ptr_align_up, @p grid need not be a power of two.
 *
 * Example usage:
 * @code{.c}
 * lh_wchar_t buf[16];
 * lh_wchar_t *base = &buf[0];
 * unsigned char *p = lh_ptr_rcast(lh_uchar_t, base) + 3;
 * unsigned char *q = lh_ptr_align_up_grid(lh_uchar_t, p, base, (lh_uaddr_t)sizeof(lh_wchar_t));
 * @endcode
 */
#define lh_ptr_align_up_grid(T, ptr, origin, grid)                                                 \
    lh_ptr_add_by_offset_unsafe(                                                                   \
        T, (ptr),                                                                                  \
        lh_math_mod(lh_math_sub((grid), lh_math_mod(lh_ptr_udiff((ptr), (origin)), (grid))),       \
                    (grid)))

/**
 * @def lh_ptr_align_down_grid(T, ptr, origin, grid)
 * @brief Largest address `<= ptr` that lies on the same @p grid lattice as @p origin:
 *        the unsigned byte offset from @p origin to the result is a multiple of @p grid.
 *
 * @param T       Result pointee type.
 * @param ptr     Pointer expression (see ::lh_ptr_align_up_grid for ::lh_ptr_udiff caveats).
 * @param origin  Anchor for the grid.
 * @param grid    Step size; must be non-zero.
 *
 * @note Unlike ::lh_ptr_align_down, @p grid need not be a power of two.
 *
 * Example usage:
 * @code{.c}
 * lh_wchar_t buf[16];
 * lh_wchar_t *base = &buf[0];
 * unsigned char *p = lh_ptr_rcast(lh_uchar_t, base) + 11;
 * unsigned char *q = lh_ptr_align_down_grid(lh_uchar_t, p, base, (lh_uaddr_t)sizeof(lh_wchar_t));
 * @endcode
 */
#define lh_ptr_align_down_grid(T, ptr, origin, grid)                                               \
    lh_ptr_add_by_offset_unsafe(T, (origin),                                                       \
                                lh_math_sub(lh_ptr_udiff((ptr), (origin)),                         \
                                            lh_math_mod(lh_ptr_udiff((ptr), (origin)), (grid))))

/**
 * @def lh_ptr_align_up(T, ptr, align)
 * @brief Round the address of @p ptr up to a multiple of @p align;
 *        result is `T *` (::lh_addr_align_up).
 *
 * @param T      Result pointee type.
 * @param ptr    Pointer expression.
 * @param align  Power-of-two alignment (see `lh/util/addr.h`).
 *
 * Example usage:
 * @code{.c}
 * unsigned char blob[128];
 * unsigned char *p = &blob[5];
 * unsigned char *q = lh_ptr_align_up(unsigned char, p, (lh_uaddr_t)16);
 * @endcode
 */
#define lh_ptr_align_up(T, ptr, align)                                                             \
    lh_addr_to_ptr(T, lh_addr_align_up(lh_ptr_to_uaddr(ptr), align))

/**
 * @def lh_ptr_align_down(T, ptr, align)
 * @brief Round the address of @p ptr down to a multiple of @p align;
 *        result is `T *` (::lh_addr_align_down).
 *
 * @param T      Result pointee type.
 * @param ptr    Pointer expression.
 * @param align  Power-of-two alignment (see `lh/util/addr.h`).
 *
 * Example usage:
 * @code{.c}
 * unsigned char blob[128];
 * unsigned char *p = &blob[90];
 * unsigned char *q = lh_ptr_align_down(unsigned char, p, (lh_uaddr_t)32);
 * @endcode
 */
#define lh_ptr_align_down(T, ptr, align)                                                           \
    lh_addr_to_ptr(T, lh_addr_align_down(lh_ptr_to_uaddr(ptr), align))

#endif /* LH_UTIL_PTR_H */
