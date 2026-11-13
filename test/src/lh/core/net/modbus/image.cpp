#include <gtest/gtest.h>

#include <lh/core/net/modbus/image.h>

#include <vector>

/* -- The image: values that survive a failed read -----------------------------
   A master keeps what it asks for. These wants are a card in miniature: two
   one-register values, a gap, a two-register value (a 32-bit one or an io_data), and
   the last register of the address space, whose end (65536) does not fit 16 bits. */
namespace
{

const lh_mb_want_t kWants[] = {{10, 1}, {11, 1}, {14, 2}, {65535, 1}};

struct planned
{
    lh_mb_image_t image;
    std::vector<lh_mb_read_t> reads;

    planned()
    {
        lh_mb_image_init(&image);
        EXPECT_TRUE(lh_mb_image_try_assign(&image, kWants, 4));
        reads.resize(4);
        reads.resize(lh_mb_plan_reads(kWants, 4, reads.data(), 4));
    }

    ~planned()
    {
        lh_mb_image_deinit(&image);
    }
};

} // namespace

TEST(lh_mb_image, starts_with_every_value_unread)
{
    planned p;

    ASSERT_EQ(lh_mb_image_get_count(&p.image), 4);
    for (lh_u16_t i = 0; i < 4; ++i)
    {
        EXPECT_EQ(lh_mb_image_get_value_status(&p.image, i), lh_mb_value_unread);
        EXPECT_EQ(lh_mb_image_get_updated_us(&p.image, i), 0U);
    }
    ASSERT_EQ(p.reads.size(), 2U) << "10..15 in one read, 65535 in another";
}

TEST(lh_mb_image, finds_a_want_by_any_of_its_registers)
{
    planned p;

    EXPECT_EQ(lh_mb_image_find(&p.image, 10), 0);
    EXPECT_EQ(lh_mb_image_find(&p.image, 11), 1);
    EXPECT_EQ(lh_mb_image_find(&p.image, 14), 2);
    EXPECT_EQ(lh_mb_image_find(&p.image, 15), 2) << "the second register of a 32-bit value";
    EXPECT_EQ(lh_mb_image_find(&p.image, 65535), 3);
    EXPECT_EQ(lh_mb_image_find(&p.image, 12), 4) << "in the gap: no want";
    EXPECT_EQ(lh_mb_image_find(&p.image, 9), 4);
    EXPECT_EQ(lh_mb_image_find(&p.image, 16), 4);
}

TEST(lh_mb_image, an_answer_lands_in_the_wants_it_covers)
{
    planned p;
    /* The answer to 10..15: the gap at 12..13 is read and thrown away. */
    const lh_u16_t answer[] = {100, 101, 0xEEEE, 0xEEEE, 0x5678, 0x1234};

    lh_mb_image_store(&p.image, &p.reads[0], answer, 6, 5000);
    EXPECT_EQ(lh_mb_image_get_value(&p.image, 0)[0], 100);
    EXPECT_EQ(lh_mb_image_get_value(&p.image, 1)[0], 101);
    EXPECT_EQ(lh_mb_image_get_value(&p.image, 2)[0], 0x5678);
    EXPECT_EQ(lh_mb_image_get_value(&p.image, 2)[1], 0x1234);
    for (lh_u16_t i = 0; i < 3; ++i)
    {
        EXPECT_EQ(lh_mb_image_get_value_status(&p.image, i), lh_mb_value_fresh);
        EXPECT_EQ(lh_mb_image_get_updated_us(&p.image, i), 5000U);
    }
    EXPECT_EQ(lh_mb_image_get_value_status(&p.image, 3), lh_mb_value_unread) << "not in this read";

    const lh_u16_t top[] = {0xBEEF};
    lh_mb_image_store(&p.image, &p.reads[1], top, 1, 6000);
    EXPECT_EQ(lh_mb_image_get_value(&p.image, 3)[0], 0xBEEF) << "register 65535";
}

TEST(lh_mb_image, a_failed_read_keeps_the_values_and_says_they_are_old)
{
    planned p;
    const lh_u16_t answer[] = {100, 101, 0, 0, 7, 8};

    lh_mb_image_store(&p.image, &p.reads[0], answer, 6, 5000);
    /* The controller held the next request for a second and the read timed out. */
    lh_mb_image_fail(&p.image, &p.reads[0], lh_mb_value_stale);
    EXPECT_EQ(lh_mb_image_get_value_status(&p.image, 0), lh_mb_value_stale);
    EXPECT_EQ(lh_mb_image_get_value(&p.image, 0)[0], 100) << "the value is kept";
    EXPECT_EQ(lh_mb_image_get_updated_us(&p.image, 0), 5000U) << "and says how old it is";
    /* A value never read has nothing to be old. */
    lh_mb_image_fail(&p.image, &p.reads[1], lh_mb_value_stale);
    EXPECT_EQ(lh_mb_image_get_value_status(&p.image, 3), lh_mb_value_unread);
    /* The next read succeeds: fresh again. */
    lh_mb_image_store(&p.image, &p.reads[0], answer, 6, 7000);
    EXPECT_EQ(lh_mb_image_get_value_status(&p.image, 0), lh_mb_value_fresh);
    EXPECT_EQ(lh_mb_image_get_updated_us(&p.image, 0), 7000U);
}

TEST(lh_mb_image, a_refusal_marks_the_wants_refused)
{
    planned p;

    lh_mb_image_fail(&p.image, &p.reads[1], lh_mb_value_refused);
    EXPECT_EQ(lh_mb_image_get_value_status(&p.image, 3), lh_mb_value_refused);
}

TEST(lh_mb_image, an_answer_too_short_for_a_want_does_not_store_it)
{
    planned p;
    const lh_u16_t answer[] = {100, 101, 0, 0, 7, 8};
    const lh_u16_t partial[] = {200, 201, 0, 0, 9};

    lh_mb_image_store(&p.image, &p.reads[0], answer, 6, 5000);
    lh_mb_image_store(&p.image, &p.reads[0], partial, 5, 6000);
    EXPECT_EQ(lh_mb_image_get_value(&p.image, 0)[0], 200);
    EXPECT_EQ(lh_mb_image_get_value_status(&p.image, 2), lh_mb_value_stale)
        << "half a 32-bit value is not a value";
    EXPECT_EQ(lh_mb_image_get_value(&p.image, 2)[0], 7);
    EXPECT_EQ(lh_mb_image_get_updated_us(&p.image, 2), 5000U);
}

TEST(lh_mb_image, assigning_again_starts_over)
{
    planned p;
    const lh_mb_want_t one[] = {{3, 1}};
    const lh_u16_t answer[] = {100, 101, 0, 0, 7, 8};

    lh_mb_image_store(&p.image, &p.reads[0], answer, 6, 5000);
    ASSERT_TRUE(lh_mb_image_try_assign(&p.image, one, 1));
    EXPECT_EQ(lh_mb_image_get_count(&p.image), 1);
    EXPECT_EQ(lh_mb_image_get_value_status(&p.image, 0), lh_mb_value_unread);
    EXPECT_EQ(lh_mb_image_find(&p.image, 3), 0);
}

/* -- What changed --------------------------------------------------------------
   A master tells a screen "this value changed" from what the image says moved: the
   first answer, a register that is different, a value that went stale or came back.
   An answer that brings the same words again changes nothing. */

TEST(lh_mb_image, the_first_answer_changes_every_want_it_brings)
{
    planned p;
    const lh_u16_t answer[] = {0, 0, 0, 0, 0, 0};

    EXPECT_EQ(lh_mb_image_store(&p.image, &p.reads[0], answer, 6, 5000), 3)
        << "zeros are a value too: unread to fresh is a change";
    for (lh_u16_t i = 0; i < 3; ++i)
    {
        EXPECT_TRUE(lh_mb_image_is_value_changed(&p.image, i));
        lh_mb_image_clear_value_changed(&p.image, i);
    }
    EXPECT_FALSE(lh_mb_image_is_value_changed(&p.image, 3)) << "not in the read";
}

TEST(lh_mb_image, the_same_words_again_change_nothing_and_one_new_word_changes_its_want)
{
    planned p;
    const lh_u16_t first[] = {100, 101, 0, 0, 7, 8};
    const lh_u16_t again[] = {100, 101, 0xEEEE, 0xEEEE, 7, 8};
    const lh_u16_t moved[] = {100, 101, 0, 0, 7, 9};

    lh_mb_image_store(&p.image, &p.reads[0], first, 6, 5000);
    for (lh_u16_t i = 0; i < 3; ++i)
    {
        lh_mb_image_clear_value_changed(&p.image, i);
    }
    EXPECT_EQ(lh_mb_image_store(&p.image, &p.reads[0], again, 6, 6000), 0)
        << "the gap is not a want: its words do not count";
    EXPECT_EQ(lh_mb_image_get_updated_us(&p.image, 0), 6000U) << "still stamped: it is fresh";
    EXPECT_EQ(lh_mb_image_store(&p.image, &p.reads[0], moved, 6, 7000), 1);
    EXPECT_FALSE(lh_mb_image_is_value_changed(&p.image, 0));
    EXPECT_FALSE(lh_mb_image_is_value_changed(&p.image, 1));
    EXPECT_TRUE(lh_mb_image_is_value_changed(&p.image, 2)) << "the high word of the 32-bit value";
}

TEST(lh_mb_image, going_stale_and_coming_back_are_changes)
{
    planned p;
    const lh_u16_t answer[] = {100, 101, 0, 0, 7, 8};

    lh_mb_image_store(&p.image, &p.reads[0], answer, 6, 5000);
    lh_mb_image_clear_value_changed(&p.image, 0);
    EXPECT_EQ(lh_mb_image_fail(&p.image, &p.reads[0], lh_mb_value_stale), 3);
    EXPECT_TRUE(lh_mb_image_is_value_changed(&p.image, 0));
    lh_mb_image_clear_value_changed(&p.image, 0);
    EXPECT_EQ(lh_mb_image_fail(&p.image, &p.reads[0], lh_mb_value_stale), 0) << "stale again";
    EXPECT_EQ(lh_mb_image_store(&p.image, &p.reads[0], answer, 6, 6000), 3)
        << "the same words, but fresh again";
    EXPECT_EQ(lh_mb_image_fail(&p.image, &p.reads[1], lh_mb_value_stale), 0)
        << "never read: nothing to go stale";
}

/* -- A read for an answer outside the plan ---------------------------------------
   The read-back after a write, the answer to a gateway client: the image takes what
   lies wholly inside, and half a 32-bit value is not a value. */

TEST(lh_mb_image, an_answer_outside_the_plan_stores_the_wants_wholly_inside_it)
{
    planned p;
    lh_mb_read_t read;

    ASSERT_TRUE(lh_mb_image_try_copy_read(&p.image, 11, 5, &read)) << "11..15";
    EXPECT_EQ(lh_mb_read_get_first(&read), 1);
    EXPECT_EQ(lh_mb_read_get_last(&read), 2);
    const lh_u16_t answer[] = {55, 0, 0, 0x1111, 0x2222};
    EXPECT_EQ(lh_mb_image_store(&p.image, &read, answer, 5, 9000), 2);
    EXPECT_EQ(lh_mb_image_get_value(&p.image, 1)[0], 55);
    EXPECT_EQ(lh_mb_image_get_value(&p.image, 2)[1], 0x2222);
    EXPECT_EQ(lh_mb_image_get_value_status(&p.image, 0), lh_mb_value_unread) << "10 is not in it";
}

TEST(lh_mb_image, half_a_value_or_a_gap_is_no_read)
{
    planned p;
    lh_mb_read_t read;

    EXPECT_FALSE(lh_mb_image_try_copy_read(&p.image, 15, 1, &read)) << "the high word alone";
    EXPECT_FALSE(lh_mb_image_try_copy_read(&p.image, 14, 1, &read)) << "the low word alone";
    EXPECT_FALSE(lh_mb_image_try_copy_read(&p.image, 12, 2, &read)) << "the gap";
    EXPECT_FALSE(lh_mb_image_try_copy_read(&p.image, 100, 10, &read)) << "nothing there";
    ASSERT_TRUE(lh_mb_image_try_copy_read(&p.image, 65535, 1, &read)) << "the last register";
    EXPECT_EQ(lh_mb_read_get_first(&read), 3);
    EXPECT_EQ(lh_mb_image_find_from(&p.image, 12), 2) << "the next want after the gap";
}
