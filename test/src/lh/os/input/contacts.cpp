#include <gtest/gtest.h>

#include <lh/os/input/contacts.h>
#include <lh/os/input/contacts/initializer.h>

namespace
{

TEST(os_input_contacts, the_empty_initializer_is_what_init_leaves)
{
    lh_os_input_contacts_t initialized = lh_os_input_contacts_empty_initializer();
    lh_os_input_contacts_t contacts;
    lh_os_input_pointer_id_t pointer = 9U;

    lh_os_input_contacts_init(lh_addr_of(contacts));
    EXPECT_EQ(lh_os_input_contacts_get_count(lh_addr_of(contacts)), 0U);
    EXPECT_EQ(lh_os_input_contacts_get_count(lh_addr_of(initialized)), 0U);
    EXPECT_FALSE(lh_os_input_contacts_find(lh_addr_of(initialized), 0U, lh_addr_of(pointer)))
        << "contact id 0 of an empty table found a pointer";
    EXPECT_EQ(pointer, 9U) << "a miss wrote the out";
}

/* Win32 pointer ids are large and sparse; the pointer ids are 1, 2, 3 in the order
   the contacts went down, and 0 stays the mouse's. */
TEST(os_input_contacts, contacts_take_the_lowest_free_ids_from_one)
{
    lh_os_input_contacts_t contacts;
    lh_os_input_pointer_id_t pointer = 0U;

    lh_os_input_contacts_init(lh_addr_of(contacts));
    ASSERT_TRUE(lh_os_input_contacts_take(lh_addr_of(contacts), 1123U,
                                          lh_os_input_device_kind_touch, lh_addr_of(pointer)));
    EXPECT_EQ(pointer, 1U);
    ASSERT_TRUE(lh_os_input_contacts_take(lh_addr_of(contacts), 7U, lh_os_input_device_kind_pen,
                                          lh_addr_of(pointer)));
    EXPECT_EQ(pointer, 2U);
    ASSERT_TRUE(lh_os_input_contacts_find(lh_addr_of(contacts), 1123U, lh_addr_of(pointer)));
    EXPECT_EQ(pointer, 1U);
    EXPECT_EQ(lh_os_input_contacts_get_device_kind_v(lh_addr_of(contacts), 2U),
              lh_os_input_device_kind_pen);
    EXPECT_EQ(lh_os_input_contacts_get_count(lh_addr_of(contacts)), 2U);
}

TEST(os_input_contacts, a_second_down_of_one_contact_keeps_its_id)
{
    lh_os_input_contacts_t contacts;
    lh_os_input_pointer_id_t pointer = 0U;

    lh_os_input_contacts_init(lh_addr_of(contacts));
    ASSERT_TRUE(lh_os_input_contacts_take(lh_addr_of(contacts), 40U, lh_os_input_device_kind_touch,
                                          lh_addr_of(pointer)));
    ASSERT_TRUE(lh_os_input_contacts_take(lh_addr_of(contacts), 40U, lh_os_input_device_kind_touch,
                                          lh_addr_of(pointer)));
    EXPECT_EQ(pointer, 1U);
    EXPECT_EQ(lh_os_input_contacts_get_count(lh_addr_of(contacts)), 1U);
}

TEST(os_input_contacts, a_released_id_goes_to_the_next_contact)
{
    lh_os_input_contacts_t contacts;
    lh_os_input_pointer_id_t pointer = 0U;

    lh_os_input_contacts_init(lh_addr_of(contacts));
    (void)lh_os_input_contacts_take(lh_addr_of(contacts), 10U, lh_os_input_device_kind_touch,
                                    lh_addr_of(pointer));
    (void)lh_os_input_contacts_take(lh_addr_of(contacts), 11U, lh_os_input_device_kind_touch,
                                    lh_addr_of(pointer));
    lh_os_input_contacts_release(lh_addr_of(contacts), 1U);
    EXPECT_FALSE(lh_os_input_contacts_find(lh_addr_of(contacts), 10U, lh_addr_of(pointer)));
    EXPECT_EQ(lh_os_input_contacts_get_device_kind_v(lh_addr_of(contacts), 1U),
              lh_os_input_device_kind_none);
    ASSERT_TRUE(lh_os_input_contacts_take(lh_addr_of(contacts), 12U, lh_os_input_device_kind_touch,
                                          lh_addr_of(pointer)));
    EXPECT_EQ(pointer, 1U);
    lh_os_input_contacts_release(lh_addr_of(contacts), 0U);
    lh_os_input_contacts_release(lh_addr_of(contacts), LH_OS_INPUT_POINTER_MAX);
    EXPECT_EQ(lh_os_input_contacts_get_count(lh_addr_of(contacts)), 2U)
        << "the mouse's id or one past the table released a contact";
}

TEST(os_input_contacts, a_contact_past_the_ids_gets_none)
{
    lh_os_input_contacts_t contacts;
    lh_os_input_pointer_id_t pointer = 0U;
    lh_os_input_contact_id_t contact;

    lh_os_input_contacts_init(lh_addr_of(contacts));
    for (contact = 0U; contact + 1U < LH_OS_INPUT_POINTER_MAX; ++contact)
    {
        ASSERT_TRUE(lh_os_input_contacts_take(lh_addr_of(contacts), 100U + contact,
                                              lh_os_input_device_kind_touch, lh_addr_of(pointer)));
    }
    pointer = 0U;
    EXPECT_FALSE(lh_os_input_contacts_take(lh_addr_of(contacts), 999U,
                                           lh_os_input_device_kind_touch, lh_addr_of(pointer)));
    EXPECT_EQ(pointer, 0U);
    EXPECT_FALSE(lh_os_input_contacts_find(lh_addr_of(contacts), 999U, lh_addr_of(pointer)));
}

} // namespace
