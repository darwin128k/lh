/**
 * @file time.h
 * @brief Filesystem timestamp representation (::lh_os_fs_time_t).
 *
 * Unix seconds since the epoch — the same ::lh_timestamp_t used everywhere
 * else in `lh`, not a second, fs-private notion of "time". Not
 * milliseconds, not Windows' 100ns ticks since 1601 — those are converted
 * to this on the way in (see `lh_os_fs_time_from_filetime_ticks` in
 * stat.c). Signed so a value before 1970 (rare, but some filesystems
 * allow it) is representable rather than wrapping.
 *
 * Purely a documentation typedef (alias for ::lh_timestamp_t, not a
 * distinct type) — it exists so `lh_os_fs_stat_t`'s atime/mtime/ctime
 * fields and ::lh_os_fs_mtime read as "a filesystem timestamp", not "some
 * timestamp". Bridges to ::lh_datetime_t the same way any other
 * ::lh_timestamp_t does, via ::lh_timestamp_to_datetime.
 */

#ifndef LH_OS_FS_TIME_H
#define LH_OS_FS_TIME_H

#include <lh/timestamp.h>

/**
 * @typedef lh_os_fs_time_t
 * @brief Unix seconds since the epoch. Alias for ::lh_timestamp_t.
 */
typedef lh_timestamp_t lh_os_fs_time_t;

#endif /* LH_OS_FS_TIME_H */
