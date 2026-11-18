/**
 * @file fields.h
 * @brief Fields of the socket reader with a total deadline.
 */

#ifndef LH_OS_WEB_HOST_DEADLINE_FIELDS_H
#define LH_OS_WEB_HOST_DEADLINE_FIELDS_H

/** @brief Borrowed socket, start tick and total timeout. */
#define lh_os_web_host_deadline_fields(socket_type, tick_type, timeout_type)                       \
    socket_type socket;                                                                            \
    tick_type start;                                                                               \
    timeout_type timeout

#endif /* LH_OS_WEB_HOST_DEADLINE_FIELDS_H */
