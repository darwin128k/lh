/**
 * @file serial.c
 * @brief POSIX backend for `lh/os/system/serial.h`: a tty in raw mode.
 */

/* termios' CRTSCTS and the high baud rates are not POSIX; glibc shows them with
   its default feature set, which a project built with -std=c11 does not get. */
#if !defined(_DEFAULT_SOURCE)
/** glibc feature macro: shows `CRTSCTS` and the high baud rates under `-std=c11`. */
#    define _DEFAULT_SOURCE 1
#endif

#include <lh/os/system/serial.h>

#include <lh/assert/runtime.h>
#include <lh/os/system/error/capture.h>
#include <lh/os/system/posix/serial.h>

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

speed_t
lh_os_system_serial_speed(lh_u32_t baud)
{
    switch (baud)
    {
    case 1200U:
        return B1200;
    case 2400U:
        return B2400;
    case 4800U:
        return B4800;
    case 9600U:
        return B9600;
    case 19200U:
        return B19200;
    case 38400U:
        return B38400;
#if defined(B57600)
    case 57600U:
        return B57600;
#endif
#if defined(B115200)
    case 115200U:
        return B115200;
#endif
#if defined(B230400)
    case 230400U:
        return B230400;
#endif
#if defined(B460800)
    case 460800U:
        return B460800;
#endif
#if defined(B921600)
    case 921600U:
        return B921600;
#endif
    default:
        return B0;
    }
}

lh_os_system_serial_handle_t
lh_os_system_serial_open(lh_str_cptr name, lh_u32_t baud, lh_os_serial_parity_t parity,
                         lh_u8_t stop_bits)
{
    const speed_t speed = lh_os_system_serial_speed(baud);

    lh_assert_runtime_ref(name);
    if (speed == B0 || (stop_bits != 1U && stop_bits != 2U))
    {
        errno = EINVAL;
        lh_os_system_error_capture();
        return LH_OS_SYSTEM_SERIAL_HANDLE_INVALID;
    }
    /* O_NOCTTY: a port is not this process's terminal, and must not become one --
       a line that drops would otherwise send it SIGHUP. O_NONBLOCK: open does not
       wait for the modem lines, and every read after it does not wait either. */
    int fd = open(name, O_RDWR | O_NOCTTY | O_NONBLOCK);

    if (fd < 0)
    {
        lh_os_system_error_capture();
        return LH_OS_SYSTEM_SERIAL_HANDLE_INVALID;
    }
    struct termios tty;

    if (tcgetattr(fd, lh_addr_of(tty)) != 0)
    {
        lh_os_system_error_capture();
        (void)close(fd);
        return LH_OS_SYSTEM_SERIAL_HANDLE_INVALID;
    }
    /* Raw: every byte as it came, nothing translated, nothing echoed, no line
       editing and no signals -- a Modbus frame is binary, and a 0x0D in it is a
       byte of a register, not the end of a line. */
    tty.c_iflag &= ~(tcflag_t)(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON |
                               IXOFF | IXANY);
    tty.c_oflag &= ~(tcflag_t)OPOST;
    tty.c_lflag &= ~(tcflag_t)(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tty.c_cflag &= ~(tcflag_t)(CSIZE | PARENB | PARODD | CSTOPB);
#if defined(CRTSCTS)
    tty.c_cflag &= ~(tcflag_t)CRTSCTS; /* no hardware flow control: RS-485 has none */
#endif
    tty.c_cflag |= (tcflag_t)(CS8 | CLOCAL | CREAD);
    if (parity != lh_os_serial_parity_none)
    {
        tty.c_cflag |= (tcflag_t)PARENB;
        tty.c_iflag |= (tcflag_t)INPCK;
        if (parity == lh_os_serial_parity_odd)
        {
            tty.c_cflag |= (tcflag_t)PARODD;
        }
    }
    if (stop_bits == 2U)
    {
        tty.c_cflag |= (tcflag_t)CSTOPB;
    }
    /* A read returns what there is, at once: no minimum count, no wait. */
    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 0;
    if (cfsetispeed(lh_addr_of(tty), speed) != 0 || cfsetospeed(lh_addr_of(tty), speed) != 0 ||
        tcsetattr(fd, TCSANOW, lh_addr_of(tty)) != 0)
    {
        lh_os_system_error_capture();
        (void)close(fd);
        return LH_OS_SYSTEM_SERIAL_HANDLE_INVALID;
    }
    /* What arrived before the port was set up was read at another rate, if at all. */
    (void)tcflush(fd, TCIOFLUSH);
    return lh_cast_static(lh_os_system_serial_handle_t, fd);
}

lh_void
lh_os_system_serial_close(lh_os_system_serial_handle_t handle)
{
    if (handle != LH_OS_SYSTEM_SERIAL_HANDLE_INVALID)
    {
        (void)close(lh_cast_static(int, handle));
    }
}

lh_ssize_t
lh_os_system_serial_read(lh_os_system_serial_handle_t handle, lh_ptr buf, lh_usize_t size)
{
    const ssize_t got = read(lh_cast_static(int, handle), buf, size);

    if (got > 0)
    {
        return lh_cast_static(lh_ssize_t, got);
    }
    /* 0 with VMIN 0 is "nothing has arrived": a tty has no end of stream. */
    if (got == 0 || errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
    {
        return LH_OS_RESULT_PENDING;
    }
    lh_os_system_error_capture();
    return LH_OS_RESULT_INVALID;
}

lh_ssize_t
lh_os_system_serial_write(lh_os_system_serial_handle_t handle, const lh_ptr buf, lh_usize_t size)
{
    const ssize_t put = write(lh_cast_static(int, handle), buf, size);

    if (put >= 0)
    {
        return lh_cast_static(lh_ssize_t, put);
    }
    if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
    {
        return LH_OS_RESULT_PENDING;
    }
    lh_os_system_error_capture();
    return LH_OS_RESULT_INVALID;
}
