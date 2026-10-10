#include <gtest/gtest.h>

#include <lh/net/modbus.h>
#include <lh/null.h>
#include <lh/util/addr.h>

#include <cstdio>

/* ── CRC ─────────────────────────────────────────────────────────────────────
   One external anchor: the check value the specification publishes, for the nine
   digits and nothing else. The string has to be exactly "123456789" — an "ABCD"
   in front of it is still a perfectly good string to checksum and a completely
   different answer, and a test that puts one there stops testing the vector. */

TEST(lh_mb, crc16_matches_the_vector_of_the_specification)
{
    const lh_byte_t digits[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
    const lh_byte_t empty[1] = {0};

    EXPECT_EQ(lh_mb_crc16(digits, (lh_u16_t)sizeof(digits)), (lh_u16_t)0x4B37);
    EXPECT_EQ(lh_mb_crc16(empty, 0), (lh_u16_t)0xFFFF);
}

TEST(lh_mb, crc16_of_nothing_is_the_seed_and_not_zero)
{
    /* A CRC that starts at zero reads a line that never sent anything as a frame
       that sent exactly the right things. */
    EXPECT_EQ(lh_mb_crc16(lh_null, 8), (lh_u16_t)0xFFFF);
}

/* ── Frames ────────────────────────────────────────────────────────────────────
   The bytes of a request are asserted directly. Its CRC is **not** asserted as a
   constant, because a constant here is a number remembered rather than derived —
   and a remembered number is how this file came to claim a frame was 0xF5C8 when
   both halves of the code agreed it was 0x0E04. The CRC is anchored once, above,
   and every frame below is checked by the same function a device's frame goes
   through, which is the check that actually matters. */

TEST(lh_mb, a_read_is_the_request_and_its_crc_low_byte_first)
{
    lh_byte_t adu[LH_MB_ADU_MAX];
    const lh_u16_t length = lh_mb_build_read(adu, LH_MB_ADU_MAX, 1, LH_MB_FC_READ_HOLDING, 0x0010, 3);
    const lh_byte_t request[] = {0x01, 0x03, 0x00, 0x10, 0x00, 0x03};
    const lh_u16_t crc = lh_mb_crc16(adu, (lh_u16_t)sizeof(request));

    EXPECT_EQ(length, (lh_u16_t)8) << "unit, code, address, count, CRC";
    for (lh_u16_t i = 0; i < sizeof(request); ++i)
    {
        EXPECT_EQ(adu[i], request[i]) << "byte " << i << " of the request";
    }
    /* Low byte first: the CRC goes out least significant byte first, and a frame
       that gets this backwards is refused by every device and by every parser
       that checks. */
    EXPECT_EQ(adu[6], (lh_byte_t)(crc & 0xFF));
    EXPECT_EQ(adu[7], (lh_byte_t)(crc >> 8));
}

TEST(lh_mb, a_read_that_the_protocol_forbids_is_not_built)
{
    lh_byte_t adu[LH_MB_ADU_MAX];

    EXPECT_EQ(lh_mb_build_read(adu, LH_MB_ADU_MAX, 1, LH_MB_FC_READ_HOLDING, 0, 0),
              (lh_u16_t)0) << "zero registers is not a question";
    EXPECT_EQ(lh_mb_build_read(adu, LH_MB_ADU_MAX, 1, LH_MB_FC_READ_HOLDING, 0,
                               LH_MB_READ_MAX + 1),
              (lh_u16_t)0) << "126 registers is past what one read may ask for";
    EXPECT_EQ(lh_mb_build_read(adu, 4, 1, LH_MB_FC_READ_HOLDING, 0, 1), (lh_u16_t)0)
        << "a buffer that cannot hold the frame gets no frame";
    EXPECT_EQ(lh_mb_build_read(adu, LH_MB_ADU_MAX, 1, 0x2B, 0, 1), (lh_u16_t)0)
        << "a function code that is not a read is not a read";
}

TEST(lh_mb, a_write_carries_the_words_and_stays_inside_the_frame)
{
    lh_byte_t adu[LH_MB_ADU_MAX];
    const lh_u16_t values[2] = {0x1234, 0xABCD};
    const lh_u16_t length = lh_mb_build_write(adu, LH_MB_ADU_MAX, 1, 0x0020,
                                              values, 2);

    EXPECT_EQ(length, (lh_u16_t)13) << "unit, code, address, count, bytes, two words, CRC";
    EXPECT_EQ(adu[0], 0x01);
    EXPECT_EQ(adu[1], LH_MB_FC_WRITE_MANY);
    EXPECT_EQ(adu[6], 4) << "four bytes for two registers";
    EXPECT_EQ(adu[7], 0x12);
    EXPECT_EQ(adu[8], 0x34);
    EXPECT_EQ(adu[10], 0xCD) << "the second word sits at bytes 9 and 10";
    /* The frame it built has to pass the same check every frame from a device
       passes, or a device that answers us correctly would be the only one whose
       words we trusted. */
    EXPECT_EQ(lh_mb_crc16(adu, (lh_u16_t)(length - 2)),
              (lh_u16_t)((lh_u16_t)adu[length - 1] << 8 | adu[length - 2]));
}

/* ── Answers ──────────────────────────────────────────────────────────────────
   The frame a device sends back for the read above: unit 1, code 3, two words. */

static void
mb_answer(lh_byte_t *adu, lh_u8_t unit, lh_u8_t fc, const lh_u16_t *values, lh_u16_t count,
          lh_u16_t *length)
{
    lh_u16_t i;

    adu[0] = unit;
    adu[1] = fc;
    adu[2] = (lh_byte_t)(count * 2);
    for (i = 0; i < count; ++i)
    {
        adu[3 + i * 2] = (lh_byte_t)(values[i] >> 8);
        adu[4 + i * 2] = (lh_byte_t)(values[i] & 0xFF);
    }
    {
        const lh_u16_t crc = lh_mb_crc16(adu, (lh_u16_t)(3 + count * 2));
        const lh_u16_t at = (lh_u16_t)(3 + count * 2);

        adu[at] = (lh_byte_t)(crc & 0xFF);
        adu[at + 1] = (lh_byte_t)(crc >> 8);
        *length = (lh_u16_t)(at + 2);
    }
}

TEST(lh_mb, an_answer_comes_back_as_the_words_it_carried)
{
    lh_byte_t adu[LH_MB_ADU_MAX];
    lh_u16_t values[4] = {0, 0, 0, 0};
    const lh_u16_t sent[2] = {0x1234, 0xABCD};
    lh_u16_t length = 0;
    lh_u16_t got = 0;

    mb_answer(adu, 1, LH_MB_FC_READ_HOLDING, sent, 2, lh_addr_of(length));

    EXPECT_EQ(lh_mb_parse_read(adu, length, 1, LH_MB_FC_READ_HOLDING, 2, values, 4,
                               lh_addr_of(got), lh_null),
              lh_mb_status_ok);
    EXPECT_EQ(got, (lh_u16_t)2) << "words read: " << got;
    EXPECT_EQ(values[0], 0x1234);
    EXPECT_EQ(values[1], 0xABCD);
}

TEST(lh_mb, a_frame_that_has_not_all_arrived_asks_for_the_rest)
{
    lh_byte_t adu[LH_MB_ADU_MAX];
    lh_u16_t values[4];
    const lh_u16_t sent[2] = {1, 2};
    lh_u16_t length = 0;

    mb_answer(adu, 1, LH_MB_FC_READ_HOLDING, sent, 2, lh_addr_of(length));

    /* Every prefix of a frame is a frame that has not finished arriving, and the
       last one in particular: the byte count inside a frame whose CRC has not
       been read yet cannot be trusted to be a length. */
    for (lh_u16_t i = 0; i < length; ++i)
    {
        const lh_mb_status_t status =
            lh_mb_parse_read(adu, i, 1, LH_MB_FC_READ_HOLDING, 2, values, 4, lh_null,
                             lh_null);
        EXPECT_NE(status, lh_mb_status_ok) << "a " << i << " byte prefix was taken for an answer";
    }
}

TEST(lh_mb, a_frame_for_another_unit_is_not_ours_and_not_a_fault)
{
    lh_byte_t adu[LH_MB_ADU_MAX];
    lh_u16_t values[4];
    const lh_u16_t sent[2] = {1, 2};
    lh_u16_t length = 0;

    mb_answer(adu, 7, LH_MB_FC_READ_HOLDING, sent, 2, lh_addr_of(length));

    EXPECT_EQ(lh_mb_parse_read(adu, length, 1, LH_MB_FC_READ_HOLDING, 2, values, 4,
                               lh_null, lh_null),
              lh_mb_status_wrong_unit)
        << "a bus carries every unit's traffic; that is the bus working";
}

TEST(lh_mb, a_device_that_refuses_says_which_way_and_by_how_much)
{
    lh_byte_t adu[LH_MB_ADU_MAX];
    lh_u8_t exception = 0;
    const lh_byte_t body[] = {0x01, 0x83, 0x02};
    const lh_u16_t crc = lh_mb_crc16(body, 3);

    adu[0] = body[0];
    adu[1] = body[1];
    adu[2] = body[2];
    adu[3] = (lh_byte_t)(crc & 0xFF);
    adu[4] = (lh_byte_t)(crc >> 8);

    EXPECT_EQ(lh_mb_parse_read(adu, 5, 1, LH_MB_FC_READ_HOLDING, 1, lh_null, 0, lh_null,
                               lh_addr_of(exception)),
              lh_mb_status_exception);
    EXPECT_EQ(exception, 2) << "illegal data address: the register is not there";
}

TEST(lh_mb, a_tcp_exception_is_two_bytes_and_that_is_a_whole_answer)
{
    /* The PDU out of a Modbus TCP refusal, taken off a controller: asked for 126
       holding registers at 30259, which the protocol does not allow, and what came
       back inside the MBAP header was `83 03`. Two bytes.

       The test above is the same refusal over RTU, where the unit byte and the CRC
       make it five. **Only the five-byte shape was ever tested**, because only RTU
       was tested with one, and the four-byte "function code, byte count, one word"
       check that ::lh_mb_parse_pdu did before looking for an exception is right for
       an answer and wrong for a refusal: it called this device a sentence with the
       last word missing.

       Which is the whole ninth failure in one line: the client could not tell a
       device that said no from a device that had not finished talking, and on a
       panel those two are the difference between an address to stop asking and a
       device to open a ticket about. */
    lh_u16_t values[4] = {0, 0, 0, 0};
    lh_u8_t exception = 0;
    lh_u16_t got = 99;
    const lh_byte_t pdu[] = {0x83, 0x03};

    EXPECT_EQ(lh_mb_parse_pdu(pdu, (lh_u16_t)sizeof(pdu), LH_MB_FC_READ_HOLDING, 125, values,
                              (lh_u16_t)(sizeof(values) / sizeof(values[0])), lh_addr_of(got),
                              lh_addr_of(exception)),
              lh_mb_status_exception)
        << "0x03 is illegal data value: the question was well formed and the answer is no";
    EXPECT_EQ(exception, 3);
    EXPECT_EQ(got, 0) << "and it carried no registers, which is the whole of what a refusal says";

    /* One byte is not an answer at all: not even a reason fits. */
    EXPECT_EQ(lh_mb_parse_pdu(pdu, 1, LH_MB_FC_READ_HOLDING, 125, values, 4, lh_addr_of(got),
                              lh_addr_of(exception)),
              lh_mb_status_short)
        << "under two bytes there is no reason to read, so 'not yet' is the honest answer";
}

TEST(lh_mb, an_exception_to_another_function_is_not_our_answer)
{
    /* We asked 0x04 and something answered 0x83 — which is 0x03 with the high bit
       set, i.e. somebody else's refusal. Reading its reason as ours would report
       another unit's complaint about its own registers as this device's. */

    lh_u8_t exception = 0;
    lh_u16_t values[4] = {0, 0, 0, 0};
    const lh_byte_t pdu[] = {0x83, 0x02, 0x00, 0x00};

    EXPECT_EQ(lh_mb_parse_pdu(pdu, (lh_u16_t)sizeof(pdu), LH_MB_FC_READ_INPUT, 1, values,
                              (lh_u16_t)(sizeof(values) / sizeof(values[0])), lh_null,
                              lh_addr_of(exception)),
              lh_mb_status_wrong_code);
    EXPECT_EQ(exception, 0) << "and its reason is not ours to report";

    /* The same two bytes, but asking the function it actually refuses: now it is
       ours, and the order of the two checks is what tells the two apart. */
    EXPECT_EQ(lh_mb_parse_pdu(pdu, 2, LH_MB_FC_READ_HOLDING, 1, values, 4, lh_null,
                              lh_addr_of(exception)),
              lh_mb_status_exception)
        << "0x83 | 0x03 is the high bit on the function we sent, and it arrived complete";
    EXPECT_EQ(exception, 2);
}

TEST(lh_mb, a_frame_with_a_broken_crc_is_not_read_as_its_bytes_say)
{
    lh_byte_t adu[LH_MB_ADU_MAX];
    lh_u16_t values[4] = {0, 0, 0, 0};
    const lh_u16_t sent[2] = {0x1234, 0xABCD};
    lh_u16_t length = 0;

    mb_answer(adu, 1, LH_MB_FC_READ_HOLDING, sent, 2, lh_addr_of(length));
    adu[3] ^= 0x01; /* one bit, in the first word */

    EXPECT_EQ(lh_mb_parse_read(adu, length, 1, LH_MB_FC_READ_HOLDING, 2, values, 4,
                               lh_null, lh_null),
              lh_mb_status_crc);
    EXPECT_EQ(values[0], (lh_u16_t)0) << "the words were taken out of a frame nobody should believe";
}

/* ── The plan ─────────────────────────────────────────────────────────────────
   This is the number the speed claim is made of, so every test here says how many
   requests a list costs — and one of them compares the planner against the
   counter, because a slide and the code that produces it disagreeing is the whole
   failure this pair exists to catch. */

TEST(lh_mb, a_run_of_neighbours_is_one_request)
{
    const lh_mb_want_t wants[4] = {{0x8010, 1}, {0x8011, 1}, {0x8012, 1}, {0x8013, 1}};
    lh_mb_read_t reads[4];

    EXPECT_EQ(lh_mb_plan_reads(wants, 4, reads, 4), (lh_u16_t)1)
        << "four adjacent registers are one question";
    EXPECT_EQ(reads[0].start, 0x8010);
    EXPECT_EQ(reads[0].count, 4);
    EXPECT_EQ(reads[0].first, 0);
    EXPECT_EQ(reads[0].last, 3);
}

TEST(lh_mb, a_gap_bigger_than_it_is_worth_crossing_costs_a_second_request)
{
    /* The gap has to be one the **gap** rule rejects and the 125-register rule
       does not: a gap of a thousand is rejected by both, so a test that used one
       proved nothing about the rule it was written for — it passed with the gap
       cap set to anything at all, because the read limit had already split the
       list on its own. 0x40 is 47 registers past the first want and the whole run
       is 49, which one read may still ask for. */
    const lh_mb_want_t wants[2] = {{0x0010, 1}, {0x0040, 1}};
    lh_mb_read_t reads[4];

    EXPECT_EQ(lh_mb_plan_reads(wants, 2, reads, 4), (lh_u16_t)2)
        << "crossing 47 registers nobody asked for is not free";
}

TEST(lh_mb, a_gap_of_a_thousand_registers_is_split_by_the_read_limit_too)
{
    /* The other rule, with the same care: this one is not about the gap at all,
       and a test that only used small gaps would pass with the read limit gone. */
    const lh_mb_want_t wants[2] = {{0x0010, 1}, {0x0400, 1}};
    lh_mb_read_t reads[4];

    EXPECT_EQ(lh_mb_plan_reads(wants, 2, reads, 4), (lh_u16_t)2)
        << "one read may not ask for a thousand registers";
}

TEST(lh_mb, a_gap_inside_the_worth_crossing_is_crossed)
{
    const lh_mb_want_t wants[2] = {{0x0010, 1}, {0x0016, 1}};
    lh_mb_read_t reads[4];

    EXPECT_EQ(lh_mb_plan_reads(wants, 2, reads, 4), (lh_u16_t)1);
    EXPECT_EQ(reads[0].start, 0x0010);
    EXPECT_EQ(reads[0].count, 7) << "0x10..0x16 is seven registers, and the gap is asked for "
                                       "too: the device sends it either way";
}

TEST(lh_mb, a_run_longer_than_one_read_may_ask_for_is_cut_where_the_protocol_says)
{
    lh_mb_want_t wants[LH_MB_READ_MAX + 4];
    lh_mb_read_t reads[4];
    lh_u16_t i;

    for (i = 0; i < LH_MB_READ_MAX + 4; ++i)
    {
        wants[i].address = (lh_u16_t)(0x1000 + i);
        wants[i].registers = 1;
    }

    EXPECT_EQ(lh_mb_plan_reads(wants, (lh_u16_t)(LH_MB_READ_MAX + 4), reads, 4),
              (lh_u16_t)2);
    EXPECT_EQ(reads[0].count, LH_MB_READ_MAX);
    EXPECT_EQ(reads[1].start, (lh_u16_t)(0x1000 + LH_MB_READ_MAX));
    EXPECT_EQ(reads[1].count, (lh_u16_t)4);
}

TEST(lh_mb, the_counter_agrees_with_the_planner_on_every_shape)
{
    /* The two are the same walk written twice on purpose, so the check is that
       they never part company: a count that says 3 while the plan builds 5 means
       one of them is dropping wants, and nobody finds out until a device is sent
       a question about a register it was never asked. */
    struct shape
    {
        const char *name;
        lh_u16_t count;
        lh_u16_t stride;
        lh_u16_t width;
    };
    const struct shape shapes[] = {
        {"one", 1, 1, 1},
        {"dense", 200, 1, 1},
        {"every eighth", 40, 8, 1},
        {"every fortieth", 40, 40, 1},
        {"pairs", 60, 6, 2},
        {"past the read limit", 300, 1, 1},
    };
    lh_mb_want_t wants[300];
    lh_mb_read_t reads[300];

    for (const struct shape *shape = shapes; shape != shapes + 6; ++shape)
    {
        for (lh_u16_t i = 0; i < shape->count; ++i)
        {
            wants[i].address = (lh_u16_t)(0x2000 + i * shape->stride);
            wants[i].registers = shape->width;
        }
        const lh_u16_t planned = lh_mb_plan_reads(wants, shape->count, reads, 300);
        const lh_u16_t counted = lh_mb_count_requests(wants, shape->count);

        fprintf(stderr, "        %-20s %u wants -> %u requests\n", shape->name, shape->count,
                planned);
        EXPECT_EQ(planned, counted) << "planner and counter disagree on " << shape->name;

        /* And the plan has to actually cover every want: no gaps, no overlap. */
        lh_u16_t next = 0;
        for (lh_u16_t r = 0; r < planned; ++r)
        {
            EXPECT_EQ(reads[r].first, next) << "a want went missing after " << shape->name;
            EXPECT_EQ(reads[r].last, (lh_u16_t)(reads[r].last)) ;
            EXPECT_GT(reads[r].last, reads[r].first - 1);
            EXPECT_LE(reads[r].count, LH_MB_READ_MAX);
            next = (lh_u16_t)(reads[r].last + 1);
        }
        EXPECT_EQ(next, shape->count) << "the last want was not covered by " << shape->name;
    }
}

TEST(lh_mb, a_scattered_card_costs_far_fewer_requests_than_registers)
{
    /* What the claim is: a card of a few hundred scattered registers is not a few
       hundred questions. The number is printed so the claim and the measurement
       are the same number and not two numbers from two runs. */
    lh_mb_want_t wants[128];
    lh_mb_read_t reads[128];
    lh_u16_t i;

    for (i = 0; i < 128; ++i)
    {
        wants[i].address = (lh_u16_t)(0x8000 + i * 6);
        wants[i].registers = 2;
    }
    const lh_u16_t planned = lh_mb_plan_reads(wants, 128, reads, 128);

    fprintf(stderr, "        128 registers, 6 apart: %u requests\n", planned);
    EXPECT_LT(planned, (lh_u16_t)40) << "a register at a time is what this avoids";
    EXPECT_EQ(planned, lh_mb_count_requests(wants, 128));
}

TEST(lh_mb, nothing_to_ask_is_no_requests_rather_than_one_empty_one)
{
    lh_mb_read_t reads[4];

    EXPECT_EQ(lh_mb_plan_reads(lh_null, 4, reads, 4), (lh_u16_t)0);
    EXPECT_EQ(lh_mb_count_requests(lh_null, 4), (lh_u16_t)0);
}