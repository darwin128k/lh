#include <gtest/gtest.h>

#include <lh/core/sql/address.h>

namespace
{

/* A driver table only to be pointed at: an address compares drivers by address. */
const lh_sql_driver_t k_driver_a = {};
const lh_sql_driver_t k_driver_b = {};

void
fill(lh_sql_address_t *address)
{
    lh_sql_address_set_driver(address, &k_driver_a);
    lh_str_assign_view(lh_sql_address_get_host_mut(address), lh_str_view_lit("db.local"));
    lh_str_assign_view(lh_sql_address_get_user_mut(address), lh_str_view_lit("game"));
    lh_str_assign_view(lh_sql_address_get_password_mut(address), lh_str_view_lit("secret"));
    lh_str_assign_view(lh_sql_address_get_database_mut(address), lh_str_view_lit("stats"));
    lh_sql_address_set_port(address, 3307U);
}

TEST(sql_address, init_names_nothing)
{
    lh_sql_address_t address;
    lh_sql_address_init(&address);
    EXPECT_FALSE(lh_sql_address_is_set(&address));
    EXPECT_TRUE(lh_str_is_empty(lh_sql_address_get_host(&address)));
    EXPECT_EQ(lh_sql_address_get_port(&address), 0U);
    lh_sql_address_deinit(&address);
}

TEST(sql_address, assign_copies_and_is_equal_compares_every_field)
{
    lh_sql_address_t a;
    lh_sql_address_init(&a);
    lh_sql_address_t b;
    lh_sql_address_init(&b);
    fill(&a);
    EXPECT_TRUE(lh_sql_address_is_set(&a));
    EXPECT_FALSE(lh_sql_address_is_equal(&a, &b));

    lh_sql_address_assign(&b, &a);
    EXPECT_TRUE(lh_sql_address_is_equal(&a, &b));
    EXPECT_STREQ(lh_str_get_data(lh_sql_address_get_password(&b)), "secret");

    lh_str_assign_view(lh_sql_address_get_password_mut(&b), lh_str_view_lit("other"));
    EXPECT_FALSE(lh_sql_address_is_equal(&a, &b));
    lh_sql_address_assign(&b, &a);
    lh_sql_address_set_port(&b, 0U);
    EXPECT_FALSE(lh_sql_address_is_equal(&a, &b));
    lh_sql_address_assign(&b, &a);
    lh_sql_address_set_driver(&b, &k_driver_b);
    EXPECT_FALSE(lh_sql_address_is_equal(&a, &b));

    lh_sql_address_deinit(&a);
    lh_sql_address_deinit(&b);
}

TEST(sql_address, clear_forgets_everything)
{
    lh_sql_address_t address;
    lh_sql_address_init(&address);
    fill(&address);
    lh_sql_address_clear(&address);
    EXPECT_FALSE(lh_sql_address_is_set(&address));
    EXPECT_TRUE(lh_str_is_empty(lh_sql_address_get_password(&address)));
    EXPECT_TRUE(lh_str_is_empty(lh_sql_address_get_database(&address)));
    EXPECT_EQ(lh_sql_address_get_port(&address), 0U);
    lh_sql_address_deinit(&address);
}

} // namespace
