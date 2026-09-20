/**
 * @file time.h
 * @brief Filesystem timestamp representation (::lh_os_fs_time_t).
 *
 * Unix seconds since the epoch (matches `time_t`'s usual meaning), not
 * milliseconds, not Windows' 100ns ticks since 1601 — those are converted
 * to this on the way in (see `lh_os_fs_filetime_to_unix` in stat.c).
 * Signed so a value before 1970 (rare, but some filesystems allow it) is
 * representable rather than wrapping.
 *
 * Purely a documentation typedef (alias for ::lh_s64_t, not a distinct
 * type) — it exists so `lh_os_fs_stat_t`'s atime/mtime/ctime fields and
 * ::lh_os_fs_mtime read as "a filesystem timestamp", not "some s64".
 */

#ifndef LH_OS_FS_TIME_H
#define LH_OS_FS_TIME_H

#include <lh/numeric/fixed/types.h>

/**
 * @typedef lh_os_fs_time_t
 * @brief Unix seconds since the epoch. Alias for ::lh_s64_t.
 */
typedef lh_s64_t lh_os_fs_time_t;

#endif /* LH_OS_FS_TIME_H */
