#include <gtest/gtest.h>

#include <lh/core/net/modbus/server.h>
#include <lh/os/net.h>
#include <lh/os/net/socket.h>
#include <lh/os/serial.h>
#include <lh/os/tick.h>
#include <lh/test/out.h>

#include <chrono>
#include <cstdlib>
#include <cstring>
#include <random>
#include <string>
#include <thread>
#include <vector>

/* -- The link against devices it did not write ------------------------------
   Every other Modbus test in LH plays the device itself, in the same file as the
   link. These do not: the device is a separate process on the other end of a real
   line -- a pseudo-terminal pair made by socat for RTU, a TCP socket for Modbus TCP
   -- and it is one of two programs:

   - `test/emu/modbus/reference.py`, a pymodbus server: somebody else's reading of
     the specification, the one the link has to agree with;
   - `test/emu/modbus/device.py`, a device that breaks one answer on request: a bad
     CRC, another unit's frame, an answer that comes late, twice, short, long, in
     pieces, or not at all.

   They need the processes, so they run only when `test/emu/modbus/run.sh` started
   them and named them in the environment; otherwise every test here is skipped.
   The variables are

       LH_TEST_MB_EMU_RTU   the port that reaches device.py over RTU
       LH_TEST_MB_EMU_TCP   the TCP port device.py listens on
       LH_TEST_MB_REF_RTU   the port that reaches reference.py over RTU
       LH_TEST_MB_REF_BAUD  its rate (and LH_TEST_MB_REF_STOP its stop bits)
       LH_TEST_MB_REF_TCP   the TCP port reference.py listens on
       LH_TEST_MB_SERVE_RTU the port client.py talks to the LH server through
       LH_TEST_MB_SERVE_TCP the TCP port the LH server listens on for client.py */
namespace
{

const lh_u8_t kUnit = 17;       /* device.py */
const lh_u8_t kControl = 247;   /* device.py's own switchboard: a write arms a fault */
const lh_u8_t kRefUnit = 1;     /* reference.py */
const lh_u16_t kRefSize = 1000; /* registers reference.py has */
const lh_u32_t kTimeoutUs = 200000;

lh_u64_t
tick_us(lh_ptr self)
{
    (void)self;
    return lh_os_tick_us();
}

const char *
env(const char *name)
{
    const char *value = std::getenv(name);

    return value != nullptr && value[0] != '\0' ? value : nullptr;
}

void
sleep_ms(int ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

lh_u16_t
emu_holding(lh_u32_t a)
{
    return (lh_u16_t)((a * 3U) & 0xFFFFU);
}

lh_u16_t
emu_input(lh_u32_t a)
{
    return (lh_u16_t)(a ^ 0x5A5AU);
}

/* One line to a device: an open port or a connected socket, and the stream a link
   reads. */
struct line
{
    bool is_tcp = false;
    bool open = false;
    lh_u16_t tcp_port = 0;
    lh_os_serial_t port;
    lh_os_net_socket_t socket;
    lh_io_stream_t stream;

    line()
    {
        lh_os_serial_init(&port);
        lh_os_net_socket_init(&socket);
    }

    ~line()
    {
        close();
    }

    void
    close()
    {
        lh_os_serial_close(&port);
        lh_os_net_socket_close(&socket);
        open = false;
    }

    bool
    open_rtu(const char *name, lh_u32_t baud, lh_u8_t stop)
    {
        is_tcp = false;
        open = lh_os_serial_open(&port, name, baud, lh_os_serial_parity_none, stop);
        if (open)
        {
            lh_os_serial_get_stream(&port, &stream);
        }
        return open;
    }

    bool
    open_tcp(lh_u16_t at)
    {
        is_tcp = true;
        tcp_port = at;
        lh_os_net_socket_close(&socket);
        lh_net_ip4_t ip;
        lh_net_ip4_init(&ip, 127, 0, 0, 1);
        lh_net_ip4_socket_addr_t addr;
        lh_net_ip4_socket_addr_init(&addr, &ip, at);
        open = lh_os_net_socket_open(&socket, lh_net_socket_type_tcp) &&
               lh_os_net_socket_connect_within(&socket, &addr, 2000);
        if (open)
        {
            stream = lh_test::out(lh_os_net_socket_get_stream, &socket);
        }
        return open;
    }

    lh_mb_framing_t
    framing() const
    {
        return is_tcp ? lh_mb_framing_mbap : lh_mb_framing_rtu;
    }

    /* Whatever is still on its way: the late tail of an answer the last test broke. */
    void
    drain()
    {
        lh_byte_t sink[512];

        for (int quiet = 0; quiet < 50;)
        {
            if (lh_io_stream_read(&stream, sink, sizeof(sink)) > 0)
            {
                quiet = 0;
            }
            else
            {
                ++quiet;
                sleep_ms(2);
            }
        }
    }
};

/* Poll @p link until it stops waiting; the wall clock bounds the loop in case the
   link never decides. */
lh_mb_link_state_t
settle(lh_mb_link_t *link, lh_u32_t timeout_us = kTimeoutUs)
{
    lh_mb_link_state_t state = lh_mb_link_waiting;
    const lh_u64_t until = lh_os_tick_us() + (lh_u64_t)timeout_us * 4U + 1000000U;

    while (state == lh_mb_link_waiting && lh_os_tick_us() < until)
    {
        state = lh_mb_link_poll(link, timeout_us);
        if (state == lh_mb_link_waiting)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(300));
        }
    }
    return state;
}

/* A read, start to end: the state it ends in and the words it brought. */
struct reading
{
    lh_mb_link_state_t state = lh_mb_link_idle;
    lh_mb_status_t reason = lh_mb_status_ok;
    std::vector<lh_u16_t> words;
};

reading
read(lh_mb_link_t *link, lh_u8_t fc, lh_u16_t address, lh_u16_t count)
{
    reading out;
    lh_u16_t got = 0;

    lh_mb_link_set_fc(link, fc);
    if (!lh_mb_link_ask(link, address, count))
    {
        out.state = lh_mb_link_idle;
        return out;
    }
    out.state = settle(link);
    out.reason = lh_mb_link_get_reason(link);
    const lh_u16_t *values = lh_mb_link_take(link, &got);
    if (values != lh_null)
    {
        out.words.assign(values, values + got);
    }
    lh_mb_link_release(link);
    return out;
}

reading
write_one(lh_mb_link_t *link, lh_u16_t address, lh_u16_t value)
{
    reading out;

    if (!lh_mb_link_write_one(link, address, value))
    {
        return out;
    }
    out.state = settle(link);
    out.reason = lh_mb_link_get_reason(link);
    lh_mb_link_release(link);
    return out;
}

reading
write_many(lh_mb_link_t *link, lh_u16_t address, const std::vector<lh_u16_t> &values)
{
    reading out;

    if (!lh_mb_link_write(link, address, values.data(), (lh_u16_t)values.size()))
    {
        return out;
    }
    out.state = settle(link);
    out.reason = lh_mb_link_get_reason(link);
    lh_mb_link_release(link);
    return out;
}

/* Whether @p link answers again within @p tries questions. A TCP stream that carried
   a broken frame has lost its place: the link says so (garbage) and finds it again,
   but not necessarily on the very next question. */
bool
recovers(lh_mb_link_t *link, int tries)
{
    for (int i = 0; i < tries; ++i)
    {
        const reading r = read(link, LH_MB_FC_READ_HOLDING, 10, 3);
        if (r.state == lh_mb_link_answered)
        {
            return r.words[0] == emu_holding(10);
        }
        sleep_ms(30);
    }
    return false;
}

/* -- device.py -------------------------------------------------------------- */

class emu : public ::testing::TestWithParam<bool>
{
  protected:
    line wire;
    lh_mb_link_t link;
    lh_mb_link_t control;

    void
    SetUp() override
    {
        const bool tcp = GetParam();
        const char *where = env(tcp ? "LH_TEST_MB_EMU_TCP" : "LH_TEST_MB_EMU_RTU");

        if (where == nullptr)
        {
            GTEST_SKIP() << "device.py is not running (test/emu/modbus/run.sh starts it)";
        }
        ASSERT_TRUE(tcp ? wire.open_tcp((lh_u16_t)std::atoi(where))
                        : wire.open_rtu(where, 9600, 1));
        wire.drain();
        lh_mb_link_init(&link, &wire.stream, wire.framing(), kUnit, tick_us, lh_null);
        lh_mb_link_init(&control, &wire.stream, wire.framing(), kControl, tick_us, lh_null);
        /* Two links on one line: their transaction ids must not collide, or a late
           answer to one would be taken for the other's. */
        lh_mb_link_set_tx(&control, 0x8000);
    }

    /* The next question to unit 17 gets fault @p n (see FAULTS in device.py). */
    void
    arm(lh_u16_t n)
    {
        const reading r = write_one(&control, 0, n);

        ASSERT_EQ(r.state, lh_mb_link_answered) << "the switchboard did not answer";
    }

    bool
    tcp() const
    {
        return GetParam();
    }
};

std::string
framing_name(const ::testing::TestParamInfo<bool> &info)
{
    return info.param ? "tcp" : "rtu";
}

INSTANTIATE_TEST_SUITE_P(lh_mb_emu, emu, ::testing::Values(false, true), framing_name);

/* -- What a device that behaves is asked ------------------------------------ */

TEST_P(emu, reads_every_width_at_both_ends_of_the_map)
{
    const struct
    {
        lh_u16_t address;
        lh_u16_t count;
    } cases[] = {{0, 1}, {10, 3}, {0, 125}, {65411, 125}, {65535, 1}, {32767, 2}, {255, 124}};

    for (const auto &c : cases)
    {
        for (const lh_u8_t fc : {LH_MB_FC_READ_HOLDING, LH_MB_FC_READ_INPUT})
        {
            const reading r = read(&link, fc, c.address, c.count);

            ASSERT_EQ(r.state, lh_mb_link_answered)
                << c.address << "+" << c.count << " fc " << (int)fc;
            ASSERT_EQ(r.words.size(), c.count);
            for (lh_u16_t i = 0; i < c.count; ++i)
            {
                const lh_u32_t a = (lh_u32_t)c.address + i;

                ASSERT_EQ(r.words[i], fc == LH_MB_FC_READ_HOLDING ? emu_holding(a) : emu_input(a))
                    << "register " << a;
            }
        }
    }
}

TEST_P(emu, a_read_past_the_end_of_the_map_is_refused_by_the_device)
{
    const lh_u32_t refused = lh_mb_link_counters_get_refused(lh_mb_link_get_counters(&link));
    const reading r = read(&link, LH_MB_FC_READ_HOLDING, 65535, 2);

    EXPECT_EQ(r.state, lh_mb_link_failed);
    EXPECT_EQ(r.reason, lh_mb_status_exception);
    EXPECT_EQ(lh_mb_link_counters_get_refused(lh_mb_link_get_counters(&link)), refused + 1);
    /* And the device is still there: the next question is answered. */
    EXPECT_EQ(read(&link, LH_MB_FC_READ_HOLDING, 0, 1).state, lh_mb_link_answered);
}

TEST_P(emu, a_request_modbus_forbids_never_leaves)
{
    const lh_u32_t sent = lh_mb_link_counters_get_sent(lh_mb_link_get_counters(&link));
    const std::vector<lh_u16_t> too_many(124, 1);

    lh_mb_link_set_fc(&link, LH_MB_FC_READ_HOLDING);
    EXPECT_FALSE(lh_mb_link_ask(&link, 0, 0));
    EXPECT_FALSE(lh_mb_link_ask(&link, 0, 126));
    EXPECT_FALSE(lh_mb_link_write(&link, 0, too_many.data(), 124));
    EXPECT_FALSE(lh_mb_link_write(&link, 0, too_many.data(), 0));
    EXPECT_EQ(lh_mb_link_counters_get_sent(lh_mb_link_get_counters(&link)), sent);
    EXPECT_EQ(lh_mb_link_get_state(&link), (lh_u8_t)lh_mb_link_idle);
}

TEST_P(emu, writes_land_and_read_back)
{
    const lh_u16_t base = tcp() ? 40000 : 30000;
    std::vector<lh_u16_t> words(123);

    for (lh_u16_t i = 0; i < 123; ++i)
    {
        words[i] = (lh_u16_t)(0xA000U + i * 7U);
    }
    ASSERT_EQ(write_many(&link, base, words).state, lh_mb_link_answered);
    const reading back = read(&link, LH_MB_FC_READ_HOLDING, base, 123);
    ASSERT_EQ(back.state, lh_mb_link_answered);
    EXPECT_EQ(back.words, words);

    ASSERT_EQ(write_one(&link, 65535, 0xBEEF).state, lh_mb_link_answered);
    ASSERT_EQ(write_one(&link, (lh_u16_t)(base + 200), 0).state, lh_mb_link_answered);
    EXPECT_EQ(read(&link, LH_MB_FC_READ_HOLDING, 65535, 1).words, std::vector<lh_u16_t>{0xBEEF});
    EXPECT_EQ(read(&link, LH_MB_FC_READ_HOLDING, (lh_u16_t)(base + 200), 1).words,
              std::vector<lh_u16_t>{0});
    ASSERT_EQ(write_one(&link, 65535, emu_holding(65535)).state, lh_mb_link_answered);
}

TEST_P(emu, a_write_past_the_end_of_the_map_is_refused_and_nothing_lands)
{
    const std::vector<lh_u16_t> words(40, 0x1111);
    const reading r = write_many(&link, 65500, words);

    EXPECT_EQ(r.state, lh_mb_link_failed);
    EXPECT_EQ(r.reason, lh_mb_status_exception);
    EXPECT_EQ(read(&link, LH_MB_FC_READ_HOLDING, 65500, 1).words,
              std::vector<lh_u16_t>{emu_holding(65500)});
}

TEST_P(emu, a_broadcast_write_is_done_once_sent_and_reaches_the_device)
{
    if (tcp())
    {
        GTEST_SKIP() << "Modbus TCP has no broadcast: unit 0 is the server's own address";
    }
    lh_mb_link_t everyone;

    lh_mb_link_init(&everyone, &wire.stream, wire.framing(), LH_MB_UNIT_BROADCAST, tick_us,
                    lh_null);
    lh_mb_link_set_tx(&everyone, 0x4000);
    const lh_u64_t start = lh_os_tick_us();
    const reading r = write_one(&everyone, 21000, 0x0B0C);
    /* Nobody answers a broadcast. A link that waits for an answer waits its whole
       timeout and then calls a write that every device did a failure. */
    EXPECT_EQ(r.state, lh_mb_link_answered);
    EXPECT_LT(lh_os_tick_us() - start, (lh_u64_t)kTimeoutUs);
    sleep_ms(50);
    EXPECT_EQ(read(&link, LH_MB_FC_READ_HOLDING, 21000, 1).words, std::vector<lh_u16_t>{0x0B0C});
    /* A broadcast read has no answer to wait for, and does not go out. */
    lh_mb_link_set_fc(&everyone, LH_MB_FC_READ_HOLDING);
    EXPECT_FALSE(lh_mb_link_ask(&everyone, 0, 1));
}

/* -- Faults on the line ----------------------------------------------------- */

TEST_P(emu, an_answer_with_a_bad_crc_is_a_crc_failure_and_not_a_timeout)
{
    if (tcp())
    {
        GTEST_SKIP() << "Modbus TCP has no CRC";
    }
    arm(1);
    const lh_u64_t start = lh_os_tick_us();
    const reading r = read(&link, LH_MB_FC_READ_HOLDING, 10, 3);
    EXPECT_EQ(r.state, lh_mb_link_failed);
    EXPECT_EQ(r.reason, lh_mb_status_crc);
    EXPECT_LT(lh_os_tick_us() - start, (lh_u64_t)kTimeoutUs) << "the answer had arrived whole";
    EXPECT_EQ(read(&link, LH_MB_FC_READ_HOLDING, 10, 3).state, lh_mb_link_answered);
}

TEST_P(emu, another_units_answer_alone_is_a_timeout)
{
    arm(2);
    const reading r = read(&link, LH_MB_FC_READ_HOLDING, 10, 3);
    EXPECT_EQ(r.state, lh_mb_link_failed);
    EXPECT_EQ(r.reason, lh_mb_status_short);
    EXPECT_EQ(read(&link, LH_MB_FC_READ_HOLDING, 10, 3).state, lh_mb_link_answered);
}

TEST_P(emu, another_units_answer_then_ours_after_silence)
{
    arm(3);
    const reading r = read(&link, LH_MB_FC_READ_INPUT, 100, 4);
    ASSERT_EQ(r.state, lh_mb_link_answered);
    EXPECT_EQ(r.words[3], emu_input(103));
}

TEST_P(emu, another_units_answer_then_ours_back_to_back)
{
    arm(4);
    const reading r = read(&link, LH_MB_FC_READ_INPUT, 100, 4);
    ASSERT_EQ(r.state, lh_mb_link_answered) << "ours was right behind the other one";
    EXPECT_EQ(r.words[0], emu_input(100));
}

TEST_P(emu, an_answer_one_byte_at_a_time)
{
    arm(5);
    const reading r = read(&link, LH_MB_FC_READ_HOLDING, 1000, 60);
    ASSERT_EQ(r.state, lh_mb_link_answered);
    EXPECT_EQ(r.words[59], emu_holding(1059));
}

TEST_P(emu, silence_is_a_timeout_of_the_length_asked)
{
    arm(6);
    const lh_u64_t start = lh_os_tick_us();
    const reading r = read(&link, LH_MB_FC_READ_HOLDING, 10, 3);
    const lh_u64_t took = lh_os_tick_us() - start;
    EXPECT_EQ(r.state, lh_mb_link_failed);
    EXPECT_EQ(r.reason, lh_mb_status_short);
    EXPECT_GE(took, (lh_u64_t)kTimeoutUs);
    EXPECT_LT(took, (lh_u64_t)kTimeoutUs + 100000U);
    EXPECT_EQ(read(&link, LH_MB_FC_READ_HOLDING, 10, 3).state, lh_mb_link_answered);
}

TEST_P(emu, a_late_answer_is_never_taken_for_the_next_one)
{
    arm(7);
    EXPECT_EQ(read(&link, LH_MB_FC_READ_HOLDING, 10, 3).state, lh_mb_link_failed);
    /* The late answer arrives while nothing is being asked. The next question asks
       the same function and the same width at another address: the stale answer
       parses perfectly as its answer, and only its values say which it is. */
    sleep_ms(700);
    const reading r = read(&link, LH_MB_FC_READ_HOLDING, 2000, 3);
    ASSERT_EQ(r.state, lh_mb_link_answered);
    EXPECT_EQ(r.words[0], emu_holding(2000)) << "the answer to the question before";
}

TEST_P(emu, junk_after_an_answer_does_not_spoil_the_next)
{
    arm(8);
    ASSERT_EQ(read(&link, LH_MB_FC_READ_HOLDING, 10, 3).state, lh_mb_link_answered);
    sleep_ms(30);
    if (tcp())
    {
        /* Three bytes that are no frame on a stream: the next header is read three
           bytes early, which is garbage, and the link must say so and come back. */
        EXPECT_TRUE(recovers(&link, 3));
        return;
    }
    const reading r = read(&link, LH_MB_FC_READ_HOLDING, 3000, 2);
    ASSERT_EQ(r.state, lh_mb_link_answered);
    EXPECT_EQ(r.words[0], emu_holding(3000));
}

TEST_P(emu, noise_in_front_of_an_answer_fails_it_without_a_stuck_link)
{
    arm(9);
    const reading r = read(&link, LH_MB_FC_READ_HOLDING, 10, 3);
    EXPECT_EQ(r.state, lh_mb_link_failed);
    sleep_ms(30);
    EXPECT_EQ(read(&link, LH_MB_FC_READ_HOLDING, 10, 3).state, lh_mb_link_answered);
}

TEST_P(emu, a_truncated_answer_is_short)
{
    arm(10);
    const reading r = read(&link, LH_MB_FC_READ_HOLDING, 10, 3);
    EXPECT_EQ(r.state, lh_mb_link_failed);
    EXPECT_EQ(r.reason, lh_mb_status_short);
    EXPECT_TRUE(recovers(&link, tcp() ? 3 : 1));
}

TEST_P(emu, more_words_than_asked_is_not_an_answer)
{
    arm(11);
    const reading r = read(&link, LH_MB_FC_READ_HOLDING, 10, 3);
    EXPECT_EQ(r.state, lh_mb_link_failed);
    EXPECT_EQ(r.reason, lh_mb_status_garbage);
}

TEST_P(emu, fewer_words_than_asked_is_partial)
{
    arm(12);
    const reading r = read(&link, LH_MB_FC_READ_HOLDING, 10, 3);
    EXPECT_EQ(r.state, lh_mb_link_answered);
    EXPECT_EQ(r.reason, lh_mb_status_partial);
    EXPECT_EQ(r.words.size(), 2U);
}

TEST_P(emu, an_odd_byte_count_is_garbage)
{
    arm(13);
    const reading r = read(&link, LH_MB_FC_READ_HOLDING, 10, 3);
    EXPECT_EQ(r.state, lh_mb_link_failed);
    EXPECT_EQ(r.reason, lh_mb_status_garbage);
    sleep_ms(30);
    EXPECT_EQ(read(&link, LH_MB_FC_READ_HOLDING, 10, 3).state, lh_mb_link_answered);
}

TEST_P(emu, a_refusal_says_why)
{
    for (const lh_u16_t fault : {14, 15})
    {
        arm(fault);
        const reading r = read(&link, LH_MB_FC_READ_HOLDING, 10, 3);
        EXPECT_EQ(r.state, lh_mb_link_failed);
        EXPECT_EQ(r.reason, lh_mb_status_exception);
        EXPECT_EQ(lh_mb_link_get_exception(&link), fault == 14 ? 0x04 : 0x06);
    }
    arm(15);
    const reading w = write_one(&link, 22000, 1);
    EXPECT_EQ(w.reason, lh_mb_status_exception);
    EXPECT_EQ(lh_mb_link_get_exception(&link), 0x06);
}

TEST_P(emu, the_other_reads_function_code_is_wrong_code)
{
    arm(16);
    const reading r = read(&link, LH_MB_FC_READ_HOLDING, 10, 3);
    EXPECT_EQ(r.state, lh_mb_link_failed);
    EXPECT_EQ(r.reason, lh_mb_status_wrong_code);
}

TEST_P(emu, an_echo_of_another_write_is_garbage)
{
    arm(17);
    EXPECT_EQ(write_one(&link, 22000, 5).reason, lh_mb_status_garbage);
    arm(17);
    EXPECT_EQ(write_many(&link, 22000, {1, 2, 3}).reason, lh_mb_status_garbage);
    arm(18);
    EXPECT_EQ(write_one(&link, 22000, 5).reason, lh_mb_status_garbage);
}

TEST_P(emu, an_answer_sent_twice_is_taken_once)
{
    arm(19);
    ASSERT_EQ(read(&link, LH_MB_FC_READ_HOLDING, 10, 3).state, lh_mb_link_answered);
    sleep_ms(60);
    const reading r = read(&link, LH_MB_FC_READ_HOLDING, 4000, 3);
    ASSERT_EQ(r.state, lh_mb_link_answered);
    EXPECT_EQ(r.words[0], emu_holding(4000)) << "the duplicate of the answer before";
}

TEST_P(emu, an_answer_with_silence_in_its_middle_breaks_rtu_framing)
{
    if (tcp())
    {
        GTEST_SKIP() << "a TCP stream has no silence that means anything";
    }
    /* 50 ms is more than three and a half characters at any rate Modbus runs at: by
       the specification the two halves are two broken frames, not one answer. */
    lh_mb_link_set_gap_us(&link, 20000);
    arm(20);
    const reading r = read(&link, LH_MB_FC_READ_HOLDING, 10, 3);
    EXPECT_EQ(r.state, lh_mb_link_failed);
    sleep_ms(30);
    EXPECT_EQ(read(&link, LH_MB_FC_READ_HOLDING, 10, 3).state, lh_mb_link_answered);
}

TEST_P(emu, a_byte_count_that_promises_more_is_short)
{
    arm(21);
    const reading r = read(&link, LH_MB_FC_READ_HOLDING, 10, 3);
    EXPECT_EQ(r.state, lh_mb_link_failed);
    EXPECT_EQ(r.reason, lh_mb_status_short);
    sleep_ms(30);
    EXPECT_EQ(read(&link, LH_MB_FC_READ_HOLDING, 10, 3).state, lh_mb_link_answered);
}

TEST_P(emu, mbap_another_transaction_is_stepped_over)
{
    if (!tcp())
    {
        GTEST_SKIP() << "RTU has no transaction id";
    }
    arm(22);
    const reading r = read(&link, LH_MB_FC_READ_HOLDING, 10, 3);
    EXPECT_EQ(r.state, lh_mb_link_failed);
    EXPECT_EQ(r.reason, lh_mb_status_short);
    EXPECT_EQ(read(&link, LH_MB_FC_READ_HOLDING, 10, 3).state, lh_mb_link_answered);
}

TEST_P(emu, mbap_a_header_that_is_not_modbus_is_garbage_at_once)
{
    if (!tcp())
    {
        GTEST_SKIP() << "RTU has no protocol id";
    }
    arm(23);
    const lh_u64_t start = lh_os_tick_us();
    const reading r = read(&link, LH_MB_FC_READ_HOLDING, 10, 3);
    EXPECT_EQ(r.state, lh_mb_link_failed);
    EXPECT_EQ(r.reason, lh_mb_status_garbage);
    EXPECT_LT(lh_os_tick_us() - start, (lh_u64_t)kTimeoutUs);
}

TEST_P(emu, mbap_a_late_answer_in_front_of_ours_is_stepped_over)
{
    if (!tcp())
    {
        GTEST_SKIP() << "RTU cannot tell a late answer by anything but its values";
    }
    arm(24);
    EXPECT_EQ(read(&link, LH_MB_FC_READ_HOLDING, 10, 3).state, lh_mb_link_failed);
    const reading r = read(&link, LH_MB_FC_READ_HOLDING, 5000, 3);
    ASSERT_EQ(r.state, lh_mb_link_answered);
    EXPECT_EQ(r.words[0], emu_holding(5000));
}

TEST_P(emu, mbap_a_closed_connection_is_lost_and_a_new_one_works)
{
    if (!tcp())
    {
        GTEST_SKIP() << "a serial line does not close";
    }
    arm(25);
    const reading r = read(&link, LH_MB_FC_READ_HOLDING, 10, 3);
    EXPECT_EQ(r.state, lh_mb_link_failed);
    EXPECT_EQ(r.reason, lh_mb_status_lost);
    ASSERT_TRUE(wire.open_tcp(wire.tcp_port));
    lh_mb_link_set_stream(&link, &wire.stream);
    EXPECT_EQ(read(&link, LH_MB_FC_READ_HOLDING, 10, 3).state, lh_mb_link_answered);
}

/* -- reference.py: pymodbus --------------------------------------------------- */

class ref : public ::testing::TestWithParam<bool>
{
  protected:
    line wire;
    lh_mb_link_t link;

    void
    SetUp() override
    {
        const bool tcp = GetParam();
        const char *where = env(tcp ? "LH_TEST_MB_REF_TCP" : "LH_TEST_MB_REF_RTU");

        if (where == nullptr)
        {
            GTEST_SKIP() << "reference.py is not running (test/emu/modbus/run.sh starts it)";
        }
        if (tcp)
        {
            ASSERT_TRUE(wire.open_tcp((lh_u16_t)std::atoi(where)));
        }
        else
        {
            const char *baud = env("LH_TEST_MB_REF_BAUD");
            const char *stop = env("LH_TEST_MB_REF_STOP");

            ASSERT_TRUE(wire.open_rtu(where, baud ? (lh_u32_t)std::atoi(baud) : 9600,
                                      stop ? (lh_u8_t)std::atoi(stop) : 1));
        }
        wire.drain();
        lh_mb_link_init(&link, &wire.stream, wire.framing(), kRefUnit, tick_us, lh_null);
    }
};

INSTANTIATE_TEST_SUITE_P(lh_mb_ref, ref, ::testing::Values(false, true), framing_name);

TEST_P(ref, reads_what_pymodbus_holds)
{
    reading r = read(&link, LH_MB_FC_READ_INPUT, 0, 125);
    ASSERT_EQ(r.state, lh_mb_link_answered);
    for (lh_u16_t i = 0; i < 125; ++i)
    {
        ASSERT_EQ(r.words[i], (lh_u16_t)(i ^ 0x00FF));
    }
    r = read(&link, LH_MB_FC_READ_INPUT, kRefSize - 1, 1);
    ASSERT_EQ(r.state, lh_mb_link_answered);
    EXPECT_EQ(r.words[0], (lh_u16_t)((kRefSize - 1) ^ 0x00FF));
}

TEST_P(ref, pymodbus_refuses_what_it_does_not_have)
{
    reading r = read(&link, LH_MB_FC_READ_HOLDING, kRefSize - 1, 2);
    EXPECT_EQ(r.state, lh_mb_link_failed);
    EXPECT_EQ(r.reason, lh_mb_status_exception);
    EXPECT_EQ(lh_mb_link_get_exception(&link), 0x02);
    r = write_many(&link, kRefSize - 1, {1, 2});
    EXPECT_EQ(r.reason, lh_mb_status_exception);
    EXPECT_EQ(lh_mb_link_get_exception(&link), 0x02);
    EXPECT_EQ(read(&link, LH_MB_FC_READ_HOLDING, 0, 1).state, lh_mb_link_answered);
}

TEST_P(ref, a_unit_pymodbus_does_not_serve_is_silence)
{
    lh_mb_link_t other;

    lh_mb_link_init(&other, &wire.stream, wire.framing(), 2, tick_us, lh_null);
    lh_mb_link_set_tx(&other, 0x6000);
    const reading r = read(&other, LH_MB_FC_READ_HOLDING, 0, 1);
    EXPECT_EQ(r.state, lh_mb_link_failed);
    wire.drain();
    EXPECT_EQ(read(&link, LH_MB_FC_READ_HOLDING, 0, 1).state, lh_mb_link_answered);
}

/* Two thousand questions picked at random against a model of what pymodbus holds:
   every read is checked word for word against every write that came before it. */
TEST_P(ref, two_thousand_random_questions_agree_with_a_model)
{
    std::vector<lh_u16_t> model(kRefSize);
    std::mt19937 rng(GetParam() ? 7U : 11U);

    {
        const reading first = read(&link, LH_MB_FC_READ_HOLDING, 0, 125);
        ASSERT_EQ(first.state, lh_mb_link_answered);
    }
    for (lh_u16_t at = 0; at < kRefSize; at = (lh_u16_t)(at + 125))
    {
        const lh_u16_t n = (lh_u16_t)std::min<int>(125, kRefSize - at);
        const reading r = read(&link, LH_MB_FC_READ_HOLDING, at, n);
        ASSERT_EQ(r.state, lh_mb_link_answered);
        std::copy(r.words.begin(), r.words.end(), model.begin() + at);
    }
    for (int step = 0; step < 2000; ++step)
    {
        const int what = (int)(rng() % 3U);
        const lh_u16_t count =
            (lh_u16_t)(1U + rng() % (what == 1 ? 1U : (what == 2 ? 123U : 125U)));
        const lh_u16_t address = (lh_u16_t)(rng() % (kRefSize - count + 1U));

        if (what == 0)
        {
            const reading r = read(&link, LH_MB_FC_READ_HOLDING, address, count);
            ASSERT_EQ(r.state, lh_mb_link_answered) << "step " << step;
            ASSERT_TRUE(std::equal(r.words.begin(), r.words.end(), model.begin() + address))
                << "step " << step << " at " << address << "+" << count;
        }
        else if (what == 1)
        {
            const lh_u16_t value = (lh_u16_t)rng();
            ASSERT_EQ(write_one(&link, address, value).state, lh_mb_link_answered)
                << "step " << step;
            model[address] = value;
        }
        else
        {
            std::vector<lh_u16_t> words(count);
            for (auto &w : words)
            {
                w = (lh_u16_t)rng();
            }
            ASSERT_EQ(write_many(&link, address, words).state, lh_mb_link_answered)
                << "step " << step;
            std::copy(words.begin(), words.end(), model.begin() + address);
        }
    }
    const lh_mb_link_counters_t *c = lh_mb_link_get_counters(&link);
    EXPECT_EQ(lh_mb_link_counters_get_failed(c), 0U);
    EXPECT_EQ(lh_mb_link_counters_get_sent(c), lh_mb_link_counters_get_done(c));
}

/* -- The LH server, for client.py -------------------------------------------
   The other direction: LH is the device, and the masters are pymodbus and a
   master that sends raw, broken bytes (test/emu/modbus/client.py). This test
   serves until client.py writes 0xDEAD to register 0xFFFE; client.py checks the
   answers, and this checks what the server counted. */

struct served_map
{
    std::vector<lh_u16_t> holding = std::vector<lh_u16_t>(65536);
    bool stop = false;

    served_map()
    {
        for (lh_u32_t a = 0; a < 65536; ++a)
        {
            holding[a] = emu_holding(a);
        }
    }
};

lh_u8_t
served_read(lh_ptr owner, lh_u8_t fc, lh_u16_t address, lh_u16_t count, lh_u16_t *values)
{
    served_map *m = (served_map *)owner;

    for (lh_u16_t i = 0; i < count; ++i)
    {
        const lh_u32_t a = (lh_u32_t)address + i;
        values[i] = fc == LH_MB_FC_READ_HOLDING ? m->holding[a] : emu_input(a);
    }
    return 0;
}

lh_u8_t
served_write(lh_ptr owner, lh_u16_t address, lh_u16_t count, const lh_u16_t *values)
{
    served_map *m = (served_map *)owner;

    if (count == 1 && address == 0xFFFE && values[0] == 0xDEAD)
    {
        m->stop = true;
    }
    for (lh_u16_t i = 0; i < count; ++i)
    {
        m->holding[(lh_u32_t)address + i] = values[i];
    }
    return 0;
}

class serve : public ::testing::TestWithParam<bool>
{
};

INSTANTIATE_TEST_SUITE_P(lh_mb_serve, serve, ::testing::Values(false, true), framing_name);

TEST_P(serve, serves_client_py_until_it_says_stop)
{
    const bool tcp = GetParam();
    const char *where = env(tcp ? "LH_TEST_MB_SERVE_TCP" : "LH_TEST_MB_SERVE_RTU");

    if (where == nullptr)
    {
        GTEST_SKIP() << "client.py is not running (test/emu/modbus/run.sh starts it)";
    }
    lh_os_net_socket_t listener;
    lh_os_net_socket_init(&listener);
    line wire;
    if (tcp)
    {
        lh_net_ip4_t ip;
        lh_net_ip4_socket_addr_t addr;

        lh_net_ip4_init(&ip, 127, 0, 0, 1);
        lh_net_ip4_socket_addr_init(&addr, &ip, (lh_u16_t)std::atoi(where));
        ASSERT_TRUE(lh_os_net_socket_open_nonblocking(&listener, lh_net_socket_type_tcp));
        ASSERT_TRUE(lh_os_net_socket_set_reuse_addr(&listener, lh_bool_true));
        ASSERT_TRUE(lh_os_net_socket_bind(&listener, &addr));
        ASSERT_TRUE(lh_os_net_socket_listen(&listener, 4));
        wire.is_tcp = true;
    }
    else
    {
        ASSERT_TRUE(wire.open_rtu(where, 9600, 1));
    }
    served_map map;
    lh_mb_server_t server;
    lh_mb_server_init(&server, &wire.stream, wire.framing(), kUnit, served_read, served_write, &map,
                      tick_us, lh_null);
    /* The silence that ends a frame, measured on a pseudo-terminal under WSL rather
       than on a wire: generous, because a pty delivers a writer's bytes when the
       scheduler gets round to it. */
    lh_mb_server_set_gap_us(&server, 15000);

    const lh_u64_t until = lh_os_tick_us() + 180U * 1000000U;
    int connections = 0;
    int lost = 0;
    bool connected = !tcp;
    while (!map.stop && lh_os_tick_us() < until)
    {
        if (!connected)
        {
            lh_net_ip4_socket_addr_t peer;

            lh_os_net_socket_close(&wire.socket);
            lh_os_net_socket_init(&wire.socket);
            if (lh_os_net_socket_accept_nonblocking(&listener, &wire.socket, &peer))
            {
                ASSERT_TRUE(lh_os_net_socket_set_blocking(&wire.socket, lh_bool_false));
                wire.stream = lh_test::out(lh_os_net_socket_get_stream, &wire.socket);
                lh_mb_server_set_stream(&server, &wire.stream);
                connected = true;
                ++connections;
            }
            else
            {
                sleep_ms(1);
            }
            continue;
        }
        const lh_mb_server_result_t r = lh_mb_server_poll(&server);
        if (r == lh_mb_server_lost)
        {
            ++lost;
            ASSERT_TRUE(tcp) << "a serial line does not end";
            connected = false;
        }
        else if (r == lh_mb_server_idle)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(200));
        }
    }
    lh_os_net_socket_close(&listener);
    ASSERT_TRUE(map.stop) << "client.py never said stop";

    const lh_mb_server_counters_t *c = lh_mb_server_get_counters(&server);
    EXPECT_GT(lh_mb_server_counters_get_answered(c), 2000U);
    EXPECT_GE(lh_mb_server_counters_get_refused(c), tcp ? 1U : 4U);
    EXPECT_GE(lh_mb_server_counters_get_ignored(c), tcp ? 1U : 2U);
    if (tcp)
    {
        /* Three connections: the raw one closed by the bad header, the raw one after
           it, and pymodbus. Two of them ended on the client's side. */
        EXPECT_EQ(connections, 3);
        EXPECT_EQ(lost, 2);
    }
    else
    {
        EXPECT_GE(lh_mb_server_counters_get_broadcast(c), 1U);
        EXPECT_GE(lh_mb_server_counters_get_broken(c), 3U);
    }
}

} // namespace
