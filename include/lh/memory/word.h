/**
 * @file word.h
 * @brief Read and write 2-, 4- and 8-byte words over memory of any type.
 *
 * Word-wide copies, fills and string scans treat a run of bytes as `u16` /
 * `u32` / `u64` / ::lh_usize_t. Done as `*(lh_u64_t *)p`, that breaks two C
 * rules: the bytes have another type (strict aliasing), and `p` may not be
 * aligned for the word. Compilers act on both — type-based alias analysis may
 * reorder or drop accesses it believes cannot overlap, and an aligned type
 * lets it assume alignment it does not have. These functions are the one
 * place that says "this access may alias anything, at any address", so the
 * rest of `lh` can be compiled with the compiler's full alias analysis.
 *
 * Two kinds, both one machine instruction on x86:
 *
 * | Function                         | Address        | Use                         |
 * |----------------------------------|----------------|-----------------------------|
 * | ::lh_memory_load_u16 … `_usize`  | any            | tails, unaligned copies     |
 * | ::lh_memory_word_load / `_store` | word-aligned   | scan loops after aligning   |
 *
 * The aligned pair matters on strict-alignment CPUs (ARM, RISC-V), where an
 * any-address load is several byte loads.
 *
 * MSVC does no type-based alias analysis and its x86/x64 targets load any
 * alignment, so there the types are the plain ones.
 */

#ifndef LH_MEMORY_WORD_H
#define LH_MEMORY_WORD_H

#include <lh/attribute/force_inline.h>
#include <lh/compiler/extern/c.h>
#include <lh/size.h>
#include <lh/util/ptr.h>

#if (LH_COMPILER_TYPE_IS_GCC_LIKE)
/* may_alias: an access through this type may touch an object of any type.
   aligned(1): the address need not be aligned for the word. */

/** @brief A 2-byte word type for accessing memory of any type at any address. */
typedef lh_u16_t lh_memory_word_u16_t __attribute__((may_alias, aligned(1)));

/** @brief A 4-byte word type for accessing memory of any type at any address. */
typedef lh_u32_t lh_memory_word_u32_t __attribute__((may_alias, aligned(1)));

/** @brief An 8-byte word type for accessing memory of any type at any address. */
typedef lh_u64_t lh_memory_word_u64_t __attribute__((may_alias, aligned(1)));

/** @brief A machine-word type for accessing memory of any type at any address. */
typedef lh_usize_t lh_memory_word_usize_t __attribute__((may_alias, aligned(1)));

/** @brief A machine-word type for accessing memory of any type at a word-aligned address. */
typedef lh_usize_t lh_memory_word_t __attribute__((may_alias));
#else
/** @brief A 2-byte word type for accessing memory of any type at any address. */
typedef lh_u16_t lh_memory_word_u16_t;

/** @brief A 4-byte word type for accessing memory of any type at any address. */
typedef lh_u32_t lh_memory_word_u32_t;

/** @brief An 8-byte word type for accessing memory of any type at any address. */
typedef lh_u64_t lh_memory_word_u64_t;

/** @brief A machine-word type for accessing memory of any type at any address. */
typedef lh_usize_t lh_memory_word_usize_t;

/** @brief A machine-word type for accessing memory of any type at a word-aligned address. */
typedef lh_usize_t lh_memory_word_t;
#endif

LH_COMPILER_EXTERN_C_BEGIN

/** @brief The 2 bytes at @p p, any address, any type of memory. */
LH_ATTRIBUTE_FORCE_INLINE
lh_u16_t
lh_memory_load_u16(const lh_ptr p)
{
    return lh_ptr_deref(lh_ptr_rcast(const lh_memory_word_u16_t, p));
}

/** @brief The 4 bytes at @p p, any address, any type of memory. */
LH_ATTRIBUTE_FORCE_INLINE
lh_u32_t
lh_memory_load_u32(const lh_ptr p)
{
    return lh_ptr_deref(lh_ptr_rcast(const lh_memory_word_u32_t, p));
}

/** @brief The 8 bytes at @p p, any address, any type of memory. */
LH_ATTRIBUTE_FORCE_INLINE
lh_u64_t
lh_memory_load_u64(const lh_ptr p)
{
    return lh_ptr_deref(lh_ptr_rcast(const lh_memory_word_u64_t, p));
}

/** @brief The `sizeof(lh_usize_t)` bytes at @p p, any address, any type of memory. */
LH_ATTRIBUTE_FORCE_INLINE
lh_usize_t
lh_memory_load_usize(const lh_ptr p)
{
    return lh_ptr_deref(lh_ptr_rcast(const lh_memory_word_usize_t, p));
}

/** @brief Write @p v as the 2 bytes at @p p, any address, any type of memory. */
LH_ATTRIBUTE_FORCE_INLINE
lh_void
lh_memory_store_u16(lh_ptr p, lh_u16_t v)
{
    lh_ptr_deref(lh_ptr_rcast(lh_memory_word_u16_t, p)) = v;
}

/** @brief Write @p v as the 4 bytes at @p p, any address, any type of memory. */
LH_ATTRIBUTE_FORCE_INLINE
lh_void
lh_memory_store_u32(lh_ptr p, lh_u32_t v)
{
    lh_ptr_deref(lh_ptr_rcast(lh_memory_word_u32_t, p)) = v;
}

/** @brief Write @p v as the 8 bytes at @p p, any address, any type of memory. */
LH_ATTRIBUTE_FORCE_INLINE
lh_void
lh_memory_store_u64(lh_ptr p, lh_u64_t v)
{
    lh_ptr_deref(lh_ptr_rcast(lh_memory_word_u64_t, p)) = v;
}

/** @brief Write @p v as the `sizeof(lh_usize_t)` bytes at @p p, any address. */
LH_ATTRIBUTE_FORCE_INLINE
lh_void
lh_memory_store_usize(lh_ptr p, lh_usize_t v)
{
    lh_ptr_deref(lh_ptr_rcast(lh_memory_word_usize_t, p)) = v;
}

/**
 * @brief The machine word at word-aligned @p p, over memory of any type.
 *
 * @param p Address aligned to `sizeof(lh_usize_t)`.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_usize_t
lh_memory_word_load(const lh_ptr p)
{
    return lh_ptr_deref(lh_ptr_rcast(const lh_memory_word_t, p));
}

/**
 * @brief Write @p v as the machine word at word-aligned @p p.
 *
 * @param p Address aligned to `sizeof(lh_usize_t)`.
 * @param v Word to write.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_void
lh_memory_word_store(lh_ptr p, lh_usize_t v)
{
    lh_ptr_deref(lh_ptr_rcast(lh_memory_word_t, p)) = v;
}

LH_COMPILER_EXTERN_C_END

#endif /* LH_MEMORY_WORD_H */
