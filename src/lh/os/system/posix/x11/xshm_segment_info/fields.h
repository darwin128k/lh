/**
 * @file fields.h
 * @brief Member fields of ::lh_os_system_posix_xshm_segment_info_t.
 */

#ifndef LH_SRC_OS_SYSTEM_POSIX_X11_XSHM_SEGMENT_INFO_FIELDS_H
#define LH_SRC_OS_SYSTEM_POSIX_X11_XSHM_SEGMENT_INFO_FIELDS_H

/**
 * @def lh_os_system_posix_xshm_segment_info_fields
 * @brief `XShmSegmentInfo`: a System V shared-memory segment as both sides know it --
 *        the server by `shmseg`, this process by `shmid` and `shmaddr`.
 */
#define lh_os_system_posix_xshm_segment_info_fields \
    lh_os_system_posix_xid_t shmseg;                                                               \
    int shmid;                                                                                     \
    char *shmaddr;                                                                                 \
    int read_only

#endif /* LH_SRC_OS_SYSTEM_POSIX_X11_XSHM_SEGMENT_INFO_FIELDS_H */
