/**
 * @file kernel32.h
 * @brief Backend-private: the part of kernel32.dll the Windows backend uses,
 *        declared by us instead of `<windows.h>`.
 *
 * Only what the backend actually calls is here; add a declaration together
 * with its first caller. Names of types and constants carry the `lh` prefix;
 * function names and struct member names stay the Win32 ones (the linker
 * resolves the former, MSDN documents the latter). See types.h.
 */

#ifndef LH_SRC_OS_SYSTEM_WIN_KERNEL32_H
#define LH_SRC_OS_SYSTEM_WIN_KERNEL32_H

#include <lh/cast/reinterpret.h>
#include <lh/cast/static.h>
#include <lh/char.h>
#include <lh/numeric/fixed/types.h>
#include <lh/os/system/win/kernel32/file_attribute_data/fields.h>
#include <lh/os/system/win/kernel32/filetime/fields.h>
#include <lh/os/system/win/kernel32/find_data/fields.h>
#include <lh/os/system/win/types.h>
#include <lh/str/ptr.h>
#include <lh/void.h>
#include <lh/wchar.h>
#include <lh/wstr/ptr.h>

/* `INVALID_HANDLE_VALUE`: (HANDLE)(LONG_PTR)-1. */
#define LH_OS_SYSTEM_WIN_INVALID_HANDLE                                                            \
    (lh_cast_reinterpret(lh_os_system_win_handle_t, lh_cast_static(lh_ssize_t, -1)))

#define LH_OS_SYSTEM_WIN_MAXDWORD 0xFFFFFFFFUL

#define LH_OS_SYSTEM_WIN_ERROR_FILE_NOT_FOUND 2UL
#define LH_OS_SYSTEM_WIN_ERROR_PATH_NOT_FOUND 3UL
#define LH_OS_SYSTEM_WIN_ERROR_NO_MORE_FILES 18UL
#define LH_OS_SYSTEM_WIN_ERROR_DIRECTORY 267UL

/* MultiByteToWideChar / WideCharToMultiByte. */
#define LH_OS_SYSTEM_WIN_CP_UTF8 65001U
#define LH_OS_SYSTEM_WIN_MB_ERR_INVALID_CHARS 0x00000008UL

/* CreateFile: dwDesiredAccess, dwShareMode, dwCreationDisposition. */
#define LH_OS_SYSTEM_WIN_GENERIC_READ 0x80000000UL
#define LH_OS_SYSTEM_WIN_GENERIC_WRITE 0x40000000UL
#define LH_OS_SYSTEM_WIN_FILE_SHARE_READ 0x00000001UL
#define LH_OS_SYSTEM_WIN_CREATE_ALWAYS 2UL
#define LH_OS_SYSTEM_WIN_OPEN_EXISTING 3UL
#define LH_OS_SYSTEM_WIN_OPEN_ALWAYS 4UL

/* FILE_ATTRIBUTE_*. */
#define LH_OS_SYSTEM_WIN_FILE_ATTRIBUTE_READONLY 0x00000001UL
#define LH_OS_SYSTEM_WIN_FILE_ATTRIBUTE_HIDDEN 0x00000002UL
#define LH_OS_SYSTEM_WIN_FILE_ATTRIBUTE_SYSTEM 0x00000004UL
#define LH_OS_SYSTEM_WIN_FILE_ATTRIBUTE_DIRECTORY 0x00000010UL
#define LH_OS_SYSTEM_WIN_FILE_ATTRIBUTE_ARCHIVE 0x00000020UL
#define LH_OS_SYSTEM_WIN_FILE_ATTRIBUTE_NORMAL 0x00000080UL
#define LH_OS_SYSTEM_WIN_FILE_ATTRIBUTE_TEMPORARY 0x00000100UL
#define LH_OS_SYSTEM_WIN_FILE_ATTRIBUTE_REPARSE_POINT 0x00000400UL
#define LH_OS_SYSTEM_WIN_FILE_ATTRIBUTE_COMPRESSED 0x00000800UL
#define LH_OS_SYSTEM_WIN_FILE_ATTRIBUTE_ENCRYPTED 0x00004000UL

#define LH_OS_SYSTEM_WIN_IO_REPARSE_TAG_SYMLINK 0xA000000CUL

/* FormatMessage: dwFlags. */
#define LH_OS_SYSTEM_WIN_FORMAT_MESSAGE_IGNORE_INSERTS 0x00000200UL
#define LH_OS_SYSTEM_WIN_FORMAT_MESSAGE_FROM_SYSTEM 0x00001000UL

/** @brief `FILETIME`. */
struct lh_os_system_win_filetime
{
    lh_os_system_win_filetime_fields(lh_os_system_win_dword_t);
};
typedef struct lh_os_system_win_filetime lh_os_system_win_filetime_t;

/** @brief `WIN32_FIND_DATAW`. */
struct lh_os_system_win_find_data
{
    lh_os_system_win_find_data_fields(lh_os_system_win_dword_t, lh_os_system_win_filetime_t,
                                      lh_wchar_t);
};
typedef struct lh_os_system_win_find_data lh_os_system_win_find_data_t;

/** @brief `WIN32_FILE_ATTRIBUTE_DATA`. */
struct lh_os_system_win_file_attribute_data
{
    lh_os_system_win_file_attribute_data_fields(lh_os_system_win_dword_t, lh_os_system_win_filetime_t);
};
typedef struct lh_os_system_win_file_attribute_data lh_os_system_win_file_attribute_data_t;

/** @brief `GET_FILEEX_INFO_LEVELS`. */
enum lh_os_system_win_get_file_ex_info_level
{
    lh_os_system_win_get_file_ex_info_standard = 0
};
typedef enum lh_os_system_win_get_file_ex_info_level lh_os_system_win_get_file_ex_info_level_t;

/* Text. Every path and name crosses the kernel boundary as UTF-16 (`...W`
   functions); lh itself speaks UTF-8. The `...A` functions are not used:
   they take the ANSI code page, not UTF-8. See lh/os/str.h. */

LH_OS_SYSTEM_WIN_IMPORT lh_int_t LH_OS_SYSTEM_WIN_CALL
MultiByteToWideChar(lh_os_system_win_uint_t CodePage, lh_os_system_win_dword_t dwFlags,
                    lh_str_cptr lpMultiByteStr, lh_int_t cbMultiByte, lh_wstr_ptr lpWideCharStr,
                    lh_int_t cchWideChar);

LH_OS_SYSTEM_WIN_IMPORT lh_int_t LH_OS_SYSTEM_WIN_CALL
WideCharToMultiByte(lh_os_system_win_uint_t CodePage, lh_os_system_win_dword_t dwFlags,
                    lh_wstr_cptr lpWideCharStr, lh_int_t cchWideChar, lh_str_ptr lpMultiByteStr,
                    lh_int_t cbMultiByte, lh_str_cptr lpDefaultChar,
                    lh_os_system_win_bool_t *lpUsedDefaultChar);

/* Time. */

LH_OS_SYSTEM_WIN_IMPORT void LH_OS_SYSTEM_WIN_CALL
GetSystemTimeAsFileTime(lh_os_system_win_filetime_t *lpSystemTimeAsFileTime);

/* Errors. `Arguments` is a `va_list *`; we always pass null with
   FORMAT_MESSAGE_IGNORE_INSERTS. */

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_dword_t LH_OS_SYSTEM_WIN_CALL
GetLastError(void);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_dword_t LH_OS_SYSTEM_WIN_CALL
FormatMessageW(lh_os_system_win_dword_t dwFlags, const lh_ptr lpSource, lh_os_system_win_dword_t dwMessageId,
               lh_os_system_win_dword_t dwLanguageId, lh_wstr_ptr lpBuffer, lh_os_system_win_dword_t nSize,
               lh_ptr Arguments);

/* Files. `lpSecurityAttributes` / `lpOverlapped` are always null here, so
   they are plain pointers rather than mirrored structs. */

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_handle_t LH_OS_SYSTEM_WIN_CALL
CreateFileW(lh_wstr_cptr lpFileName, lh_os_system_win_dword_t dwDesiredAccess,
            lh_os_system_win_dword_t dwShareMode, lh_ptr lpSecurityAttributes,
            lh_os_system_win_dword_t dwCreationDisposition,
            lh_os_system_win_dword_t dwFlagsAndAttributes, lh_os_system_win_handle_t hTemplateFile);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
CloseHandle(lh_os_system_win_handle_t hObject);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
ReadFile(lh_os_system_win_handle_t hFile, lh_ptr lpBuffer, lh_os_system_win_dword_t nNumberOfBytesToRead,
         lh_os_system_win_dword_t *lpNumberOfBytesRead, lh_ptr lpOverlapped);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
WriteFile(lh_os_system_win_handle_t hFile, const lh_ptr lpBuffer, lh_os_system_win_dword_t nNumberOfBytesToWrite,
          lh_os_system_win_dword_t *lpNumberOfBytesWritten, lh_ptr lpOverlapped);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
GetFileAttributesExW(lh_wstr_cptr lpFileName,
                     lh_os_system_win_get_file_ex_info_level_t fInfoLevelId,
                     lh_ptr lpFileInformation);

/* Directories. */

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_handle_t LH_OS_SYSTEM_WIN_CALL
FindFirstFileW(lh_wstr_cptr lpFileName, lh_os_system_win_find_data_t *lpFindFileData);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
FindNextFileW(lh_os_system_win_handle_t hFindFile, lh_os_system_win_find_data_t *lpFindFileData);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
FindClose(lh_os_system_win_handle_t hFindFile);

/* Shared images. `lpModuleName` is a path for LoadLibrary and an address
   inside an image for GetModuleHandleEx FROM_ADDRESS — one pointer either way. */

#define LH_OS_SYSTEM_WIN_GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS 0x00000004UL

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_handle_t LH_OS_SYSTEM_WIN_CALL
LoadLibraryW(lh_wstr_cptr lpLibFileName);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
FreeLibrary(lh_os_system_win_handle_t hLibModule);

LH_OS_SYSTEM_WIN_IMPORT lh_ptr LH_OS_SYSTEM_WIN_CALL
GetProcAddress(lh_os_system_win_handle_t hModule, lh_str_cptr lpProcName);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
GetModuleHandleExW(lh_os_system_win_dword_t dwFlags, const lh_ptr lpModuleName,
                   lh_os_system_win_handle_t *phModule);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_dword_t LH_OS_SYSTEM_WIN_CALL
GetModuleFileNameW(lh_os_system_win_handle_t hModule, lh_wstr_ptr lpFilename,
                   lh_os_system_win_dword_t nSize);

/* Module handle by ANSI name. Present since Windows 95. Used only for the
   window class `hInstance` (null → this EXE); paths still go through `...W`. */
LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_handle_t LH_OS_SYSTEM_WIN_CALL
GetModuleHandleA(lh_str_cptr lpModuleName);

/* Millisecond tick. Present since Windows 95. Wraps ~49.7 days; fine for the
   app pump. (`GetTickCount64` is Vista+ — do not call it.) */
LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_dword_t LH_OS_SYSTEM_WIN_CALL
GetTickCount(lh_void);

/**
 * @brief `LARGE_INTEGER`: QPC counter / frequency. Present since Windows XP
 *        (and NT 3.1 for the type). QuadPart is ::lh_s64_t.
 */
typedef union lh_os_system_win_large_integer
{
    struct
    {
        lh_os_system_win_dword_t LowPart;
        lh_long_t HighPart;
    } u;
    lh_s64_t QuadPart;
} lh_os_system_win_large_integer_t;

/* High-resolution tick. Present since Windows XP (NT 3.5+). */
LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
QueryPerformanceCounter(lh_os_system_win_large_integer_t *lpPerformanceCount);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
QueryPerformanceFrequency(lh_os_system_win_large_integer_t *lpFrequency);

#endif /* LH_SRC_OS_SYSTEM_WIN_KERNEL32_H */
