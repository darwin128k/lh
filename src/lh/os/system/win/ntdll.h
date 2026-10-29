/**
 * @file ntdll.h
 * @brief Backend-private: the few native NT calls the Win32 API has no equal for.
 *
 * `NtCreateFile` opens a name relative to an open directory handle, which Win32
 * cannot: that is how a file is looked up inside a trusted root without a path
 * string that could escape it. Both calls are resolved from `ntdll.dll` at use,
 * the module every Windows process has loaded. Not installed, not part of the API.
 */

#ifndef LH_SRC_OS_SYSTEM_WIN_NTDLL_H
#define LH_SRC_OS_SYSTEM_WIN_NTDLL_H

#include <lh/os/system/win/kernel32.h>

/** @{ */
/** `NtCreateFile`: CreateDisposition and CreateOptions. */
#define LH_OS_SYSTEM_WIN_NT_FILE_OPEN 1UL
#define LH_OS_SYSTEM_WIN_NT_FILE_DIRECTORY_FILE 0x00000001UL
#define LH_OS_SYSTEM_WIN_NT_FILE_SYNCHRONOUS_IO_NONALERT 0x00000020UL
#define LH_OS_SYSTEM_WIN_NT_FILE_NON_DIRECTORY_FILE 0x00000040UL
#define LH_OS_SYSTEM_WIN_NT_FILE_OPEN_REPARSE_POINT 0x00200000UL
/** @} */

/** @{ */
/** Access rights of a directory step and of the file at the end. */
#define LH_OS_SYSTEM_WIN_FILE_LIST_DIRECTORY 0x00000001UL
#define LH_OS_SYSTEM_WIN_FILE_TRAVERSE 0x00000020UL
#define LH_OS_SYSTEM_WIN_FILE_READ_ATTRIBUTES 0x00000080UL
#define LH_OS_SYSTEM_WIN_SYNCHRONIZE 0x00100000UL
/** @} */

/** `OBJ_CASE_INSENSITIVE`: names compare as the Win32 API compares them. */
#define LH_OS_SYSTEM_WIN_OBJ_CASE_INSENSITIVE 0x00000040UL

/** @brief `UNICODE_STRING`: counted UTF-16, lengths in bytes. */
typedef struct lh_os_system_win_unicode_string
{
    lh_os_system_win_word_t Length;        /**< Bytes of text, no NUL. */
    lh_os_system_win_word_t MaximumLength; /**< Bytes of the buffer. */
    lh_wstr_ptr Buffer;                    /**< The text. */
} lh_os_system_win_unicode_string_t;

/** @brief `OBJECT_ATTRIBUTES`: what to open, relative to what. */
typedef struct lh_os_system_win_object_attributes
{
    lh_os_system_win_dword_t Length;               /**< Size of this structure. */
    lh_os_system_win_handle_t RootDirectory;       /**< The directory the name is in. */
    lh_os_system_win_unicode_string_t *ObjectName; /**< One name, no separator. */
    lh_os_system_win_dword_t Attributes;           /**< `OBJ_*` flags. */
    lh_ptr SecurityDescriptor;                     /**< Always ::lh_null here. */
    lh_ptr SecurityQualityOfService;               /**< Always ::lh_null here. */
} lh_os_system_win_object_attributes_t;

/** @brief `IO_STATUS_BLOCK`: how an NT I/O call ended. */
typedef struct lh_os_system_win_io_status_block
{
    lh_ptr Status;          /**< The status, as wide as a pointer. */
    lh_usize_t Information; /**< Call-specific detail. */
} lh_os_system_win_io_status_block_t;

/** `NtCreateFile`. Present since Windows NT 3.1. */
typedef lh_long_t(LH_OS_SYSTEM_WIN_CALL *lh_os_system_win_nt_create_file_fn)(
    lh_os_system_win_handle_t *FileHandle, lh_os_system_win_dword_t DesiredAccess,
    lh_os_system_win_object_attributes_t *ObjectAttributes,
    lh_os_system_win_io_status_block_t *IoStatusBlock,
    lh_os_system_win_large_integer_t *AllocationSize, lh_os_system_win_dword_t FileAttributes,
    lh_os_system_win_dword_t ShareAccess, lh_os_system_win_dword_t CreateDisposition,
    lh_os_system_win_dword_t CreateOptions, lh_ptr EaBuffer, lh_os_system_win_dword_t EaLength);

/** `RtlNtStatusToDosError`: an NT status as a `GetLastError` code. Present since NT 3.1. */
typedef lh_os_system_win_dword_t(
    LH_OS_SYSTEM_WIN_CALL *lh_os_system_win_rtl_nt_status_to_dos_error_fn)(lh_long_t Status);

#endif /* LH_SRC_OS_SYSTEM_WIN_NTDLL_H */
