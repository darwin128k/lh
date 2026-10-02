/**
 * @file str.h
 * @brief The OS's own text (::lh_os_str_t): what the kernel reads and writes.
 *
 * lh text is UTF-8 everywhere. The kernel's is not always:
 *
 *   Windows — UTF-16 (::lh_wstr_t, `...W` calls). The `...A` calls take the
 *             ANSI code page, not UTF-8, so lh never uses them.
 *   POSIX   — bytes, UTF-8 by convention (::lh_str_t): no conversion.
 *
 * ::lh_os_str_t names that text so code that crosses into the kernel does
 * not hardcode wide vs. narrow — same idea as ::lh_os_error_desc_t
 * (`lh/os/error/desc.h`). The two representations share one API shape
 * (both are an ::lh_array_t of characters), so the `lh_os_str_*` names map
 * one to one onto `lh_wstr_*` / `lh_str_*`.
 *
 * Converting to and from UTF-8 is the kernel backend's job:
 * `lh/os/system/str.h`.
 */

#ifndef LH_OS_STR_H
#define LH_OS_STR_H

#include <lh/compiler/os.h>

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
#    include <lh/util/wstr/ptr.h>
#    include <lh/wchar.h>
#    include <lh/wstr.h>
#    include <lh/wstr/ptr.h>
#    include <lh/wstr/view.h>

/**
 * @def LH_OS_STR_WIDE
 * @brief `1` when ::lh_os_str_t is UTF-16 (Windows), `0` when it is UTF-8.
 */
#    define LH_OS_STR_WIDE 1

/**
 * @typedef lh_os_char_t
 * @brief One code unit of OS text (::lh_wchar_t / ::lh_char_t).
 */
typedef lh_wchar_t lh_os_char_t;

/**
 * @typedef lh_os_str_t
 * @brief Owning OS text (::lh_wstr_t / ::lh_str_t).
 */
typedef lh_wstr_t lh_os_str_t;

/**
 * @typedef lh_os_str_view_t
 * @brief View of OS text (::lh_wstr_view_t / ::lh_str_view_t).
 */
typedef lh_wstr_view_t lh_os_str_view_t;

/** @brief Mutable pointer to OS text (::lh_wstr_ptr / ::lh_str_ptr). */
#    define lh_os_str_ptr lh_wstr_ptr
/** @brief Const pointer to OS text (::lh_wstr_cptr / ::lh_str_cptr). */
#    define lh_os_str_cptr lh_wstr_cptr

#    define lh_os_str_init(self) lh_wstr_init(self)
#    define lh_os_str_deinit(self) lh_wstr_deinit(self)
#    define lh_os_str_get_data(self) lh_wstr_get_data(self)
#    define lh_os_str_get_size(self) lh_wstr_get_size(self)
#    define lh_os_str_clear(self) lh_wstr_clear(self)
#    define lh_os_str_append(self, text, count) lh_wstr_append(self, text, count)
#    define lh_os_str_as_view(self) lh_wstr_as_view(self)
/** @brief Length of NUL-terminated OS text, in code units. */
#    define lh_os_str_ptr_len(text) lh_wstr_ptr_len(text)
#else
#    include <lh/char.h>
#    include <lh/str.h>
#    include <lh/str/ptr.h>
#    include <lh/str/view.h>
#    include <lh/util/str/ptr.h>

#    define LH_OS_STR_WIDE 0

typedef lh_char_t lh_os_char_t;
typedef lh_str_t lh_os_str_t;
typedef lh_str_view_t lh_os_str_view_t;

#    define lh_os_str_ptr lh_str_ptr
#    define lh_os_str_cptr lh_str_cptr

#    define lh_os_str_init(self) lh_str_init(self)
#    define lh_os_str_deinit(self) lh_str_deinit(self)
#    define lh_os_str_get_data(self) lh_str_get_data(self)
#    define lh_os_str_get_size(self) lh_str_get_size(self)
#    define lh_os_str_clear(self) lh_str_clear(self)
#    define lh_os_str_append(self, text, count) lh_str_append(self, text, count)
#    define lh_os_str_as_view(self) lh_str_as_view(self)
#    define lh_os_str_ptr_len(text) lh_str_ptr_len(text)
#endif

#endif /* LH_OS_STR_H */
