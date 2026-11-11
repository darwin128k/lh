/**
 * @file list.h
 * @brief A module list: one module per line, in sections per operating system.
 *
 * The text a host reads to know which modules to load, and in what order:
 *
 * @code{.ini}
 * ; comment          # comment too
 * qproto             ; before any section: every system
 * [any]
 * stats/stats.so
 * [win32]
 * winonly
 * [linux]
 * /opt/mods/native.so
 * @endcode
 *
 * A line is trimmed; a blank line and a line starting with `;` or `#` are
 * skipped. `[name]` opens a section. Entries before the first section and in
 * `[any]`, `[modules]` or the section named after this system
 * (::lh_os_system_get_name) are taken; the other sections are passed over. A
 * leading UTF-8 byte order mark is skipped. What an entry means — a folder
 * name, an image path — is the caller's (::lh_os_loader_image_try_resolve).
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_LOADER_LIST_H
#define LH_OS_LOADER_LIST_H

#include <lh/config.h>
#include <lh/os/loader/list/cb.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/loader/list.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Whether a trimmed @p line carries nothing: it is empty or starts with
 *        `;` or `#`.
 *
 * @param line The line, trimmed; checked by ::lh_assert_runtime_ref.
 */
lh_bool_t
lh_os_loader_list_is_skipped(const lh_str_view_t *line);

/**
 * @brief Read the section name of a trimmed `[name]` line.
 *
 * @param line The line, trimmed; checked by ::lh_assert_runtime_ref.
 * @param out  Receives the name, trimmed, as a view into @p line.
 * @return ::lh_bool_false when @p line is not `[...]`, the name is empty or the
 *         name holds `[` or `]`; @p out is untouched then.
 */
lh_bool_t
lh_os_loader_list_try_get_section(const lh_str_view_t *line, lh_str_view_t *out);

/**
 * @brief Whether entries under @p section are for this system: no section
 *        yet (empty), `any`, `modules` or this system's name, in any case.
 *
 * @param section The section name; checked by ::lh_assert_runtime_ref.
 */
lh_bool_t
lh_os_loader_list_is_section_active(const lh_str_view_t *section);

/**
 * @brief Call @p cb for every entry of @p text that is for this system, in
 *        order.
 *
 * @param text     The whole list; checked by ::lh_assert_runtime_ref.
 * @param cb       Called once per entry; checked by ::lh_assert_runtime_ref.
 * @param user     Handed to @p cb as it is.
 * @param bad_line Receives the number (from 1) of a malformed section line;
 *                 checked by ::lh_assert_runtime_ref, written only on failure.
 * @return ::lh_bool_false at the first malformed `[` line: reading stops there,
 *         and the entries before it have been handed to @p cb.
 */
lh_bool_t
lh_os_loader_list_try_read(const lh_str_view_t *text, lh_os_loader_list_cb cb, lh_ptr user,
                           lh_usize_t *bad_line);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_LOADER_LIST_H */
