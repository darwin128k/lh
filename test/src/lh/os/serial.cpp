#include <gtest/gtest.h>

#include <lh/io/pending.h>
#include <lh/net/modbus.h>
#include <lh/net/modbus/link.h>
#include <lh/null.h>
#include <lh/os/serial.h>
#include <lh/util/addr.h>

#include <cstring>

#if !defined(_WIN32)
#    include <fcntl.h>
#    include <stdlib.h>
#    include <unistd.h>
#endif

namespace
{

TEST(os_serial, a_port_that_does_not_exist_is_not_opened)
{
    lh_os_serial_t port;

    lh_os_serial_init(&port);
#if defined(_WIN32)
    EXPECT_FALSE(lh_os_serial_open(&port, "COM250", 9600, lh_os_serial_parity_even, 1));
#else
    EXPECT_FALSE(lh_os_serial_open(&port, "/dev/lh-no-such-tty", 9600, lh_os_serial_parity_even, 1));
#endif
    EXPECT_FALSE(lh_os_serial_is_open(&port));
    lh_os_serial_close(&port); /* safe on a port that never opened */
}

#if !defined(_WIN32)

/* ── A cable on this machine ─────────────────────────────────────────────────────
   A pseudo-terminal is a pair of ends that behave as the two ends of a serial cable:
   what one writes the other reads, and the "port" end is a tty that takes termios
   like any /dev/ttyUSB0. The port end is opened through lh as a serial port; the
   other end plays the device. */
struct cable
{
    int device;
    const char *port_name;

    cable() : device(-1), port_name(nullptr)
    {
        device = posix_openpt(O_RDWR | O_NOCTTY);
        if (device >= 0 && grantpt(device) == 0 && unlockpt(device) == 0)
        {
            port_name = ptsname(device);
        }
    }

    ~cable()
    {
        if (device >= 0)
        {
            close(device);
        }
    }

    /* Read exactly @p size bytes the port sent, giving it a second. */
    bool
    read_exactly(lh_byte_t *out, size_t size) const
    {
        size_t have = 0;

        for (int i = 0; i < 1000 && have < size; ++i)
        {
            const ssize_t got = read(device, out + have, size - have);

            if (got > 0)
            {
                have += (size_t)got;
            }
            else
            {
                usleep(1000);
            }
        }
        return have == size;
    }
};

TEST(os_serial, a_quiet_line_is_pending_and_not_the_end)
{
    cable line;
    lh_os_serial_t port;
    lh_byte_t buf[16];

    ASSERT_NE(line.port_name, nullptr);
    lh_os_serial_init(&port);
    ASSERT_TRUE(lh_os_serial_open(&port, line.port_name, 9600, lh_os_serial_parity_even, 1));
    /* Nothing sent: not 0 -- a serial line has no end -- but "nothing yet". */
    EXPECT_EQ(lh_os_serial_read(&port, buf, sizeof(buf)), LH_IO_PENDING);
    lh_os_serial_close(&port);
}

TEST(os_serial, bytes_go_both_ways_through_the_stream)
{
    cable line;
    lh_os_serial_t port;
    lh_io_stream_t stream;
    const lh_byte_t hello[] = {0x01, 0x0D, 0x0A, 0x00, 0xFF}; /* a CR and an LF stay bytes */
    lh_byte_t got[sizeof(hello)] = {};
    lh_ssize_t n = LH_IO_PENDING;

    ASSERT_NE(line.port_name, nullptr);
    lh_os_serial_init(&port);
    ASSERT_TRUE(lh_os_serial_open(&port, line.port_name, 115200, lh_os_serial_parity_none, 2));
    lh_os_serial_get_stream(&port, &stream);

    ASSERT_EQ(lh_io_stream_write(&stream, (lh_ptr)hello, sizeof(hello)), (lh_ssize_t)sizeof(hello));
    ASSERT_TRUE(line.read_exactly(got, sizeof(got)));
    EXPECT_EQ(memcmp(got, hello, sizeof(hello)), 0) << "raw: nothing translated on the way out";

    ASSERT_EQ(write(line.device, hello, sizeof(hello)), (ssize_t)sizeof(hello));
    memset(got, 0, sizeof(got));
    for (int i = 0; i < 1000 && n == LH_IO_PENDING; ++i)
    {
        n = lh_io_stream_read(&stream, got, sizeof(got));
        if (n == LH_IO_PENDING)
        {
            usleep(1000);
        }
    }
    ASSERT_EQ(n, (lh_ssize_t)sizeof(hello));
    EXPECT_EQ(memcmp(got, hello, sizeof(hello)), 0) << "and nothing translated on the way in";
    lh_os_serial_close(&port);
}

TEST(os_serial, a_modbus_rtu_question_is_answered_over_the_port)
{
    cable line;
    lh_os_serial_t port;
    lh_io_stream_t stream;
    lh_mb_link_t link;
    lh_byte_t request[8];
    lh_byte_t answer[LH_MB_ADU_MAX];
    /* The PDU of three holding registers, 1, 2 and 3: code, byte count, six bytes. */
    const lh_byte_t pdu[] = {0x03, 0x06, 0x00, 0x01, 0x00, 0x02, 0x00, 0x03};
    lh_u16_t answer_length;
    lh_mb_link_state_t state = lh_mb_link_waiting;
    const lh_u16_t *values;
    lh_u16_t count = 0;

    ASSERT_NE(line.port_name, nullptr);
    lh_os_serial_init(&port);
    ASSERT_TRUE(lh_os_serial_open(&port, line.port_name, 9600, lh_os_serial_parity_even, 1));
    lh_os_serial_get_stream(&port, &stream);
    lh_mb_link_init(&link, &stream, lh_mb_framing_rtu, 1, lh_null, lh_null);

    ASSERT_TRUE(lh_mb_link_ask(&link, 10, 3));
    /* What arrives at the device is an RTU request: unit, code, address, count and a
       CRC that checks. */
    ASSERT_TRUE(line.read_exactly(request, sizeof(request)));
    EXPECT_EQ(request[0], 0x01);
    EXPECT_EQ(request[1], LH_MB_FC_READ_HOLDING);
    EXPECT_EQ(request[3], 10);
    EXPECT_EQ(request[5], 3);
    EXPECT_EQ(lh_mb_crc16(request, 6), (lh_u16_t)((lh_u16_t)request[7] << 8 | request[6]));

    answer_length = lh_mb_encode(lh_mb_framing_rtu, 1, 0, pdu, (lh_u16_t)sizeof(pdu), answer, (lh_u16_t)sizeof(answer));
    ASSERT_EQ(write(line.device, answer, answer_length), (ssize_t)answer_length);
    for (int i = 0; i < 1000 && state == lh_mb_link_waiting; ++i)
    {
        state = lh_mb_link_poll(&link, 1000000);
        if (state == lh_mb_link_waiting)
        {
            usleep(1000);
        }
    }
    ASSERT_EQ(state, lh_mb_link_answered);
    values = lh_mb_link_take(&link, &count);
    ASSERT_NE(values, lh_null);
    ASSERT_EQ(count, 3);
    EXPECT_EQ(values[0], 1);
    EXPECT_EQ(values[2], 3);
    lh_os_serial_close(&port);
}

#endif /* !_WIN32 */

} // namespace
