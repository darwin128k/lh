#include <gtest/gtest.h>

#include <cstdio>

#include <lh/os/sql/keep.h>
#include <lh/os/sql/sqlite.h>

namespace
{

lh_usize_t
error_size(const lh_sql_db_t *db)
{
    const lh_str_view_t error = lh_sql_db_get_error(db);
    return lh_str_view_get_size(&error);
}

void
sqlite_at(lh_sql_address_t *address, lh_str_view_t path)
{
    lh_sql_address_set_driver(address, lh_os_sql_sqlite_get_driver());
    lh_str_assign_view(lh_sql_address_get_path_mut(address), path);
}

TEST(os_sql_keep, uses_the_kept_connection_again_for_the_same_address)
{
    lh_os_sql_keep_t keep;
    lh_os_sql_keep_init(&keep);
    lh_sql_address_t address;
    lh_sql_address_init(&address);
    sqlite_at(&address, lh_str_view_lit(":memory:"));

    lh_sql_db_t *first = lh_os_sql_keep_try_get_db(&keep, &address);
    ASSERT_NE(first, nullptr);
    ASSERT_TRUE(lh_sql_db_exec(first, lh_str_view_lit("CREATE TABLE t(id INTEGER)")));
    EXPECT_FALSE(lh_sql_db_exec(first, lh_str_view_lit("SELECT nope FROM t")));

    /* The same in-memory database: the table is still there, the error is gone. */
    lh_sql_db_t *again = lh_os_sql_keep_try_get_db(&keep, &address);
    ASSERT_EQ(again, first);
    EXPECT_EQ(error_size(again), 0U);
    EXPECT_TRUE(lh_sql_db_exec(again, lh_str_view_lit("INSERT INTO t VALUES(1)")));
    EXPECT_FALSE(lh_os_sql_keep_needs_release(&keep));

    lh_sql_address_deinit(&address);
    lh_os_sql_keep_deinit(&keep);
}

TEST(os_sql_keep, opens_anew_for_another_address)
{
    const char *const file = "lh_test_os_sql_keep.db";
    lh_os_sql_keep_t keep;
    lh_os_sql_keep_init(&keep);
    lh_sql_address_t memory;
    lh_sql_address_init(&memory);
    lh_sql_address_t disk;
    lh_sql_address_init(&disk);
    sqlite_at(&memory, lh_str_view_lit(":memory:"));
    sqlite_at(&disk, lh_str_view_lit("lh_test_os_sql_keep.db"));

    lh_sql_db_t *db = lh_os_sql_keep_try_get_db(&keep, &memory);
    ASSERT_NE(db, nullptr);
    ASSERT_TRUE(lh_sql_db_exec(db, lh_str_view_lit("CREATE TABLE only_in_memory(id INTEGER)")));

    db = lh_os_sql_keep_try_get_db(&keep, &disk);
    ASSERT_NE(db, nullptr);
    EXPECT_TRUE(lh_sql_address_is_equal(lh_os_sql_keep_get_address(&keep), &disk));
    EXPECT_FALSE(lh_sql_db_exec(db, lh_str_view_lit("INSERT INTO only_in_memory VALUES(1)")));

    lh_os_sql_keep_deinit(&keep);
    lh_sql_address_deinit(&memory);
    lh_sql_address_deinit(&disk);
    std::remove(file);
}

TEST(os_sql_keep, a_failed_open_keeps_nothing_and_says_why)
{
    lh_os_sql_keep_t keep;
    lh_os_sql_keep_init(&keep);
    lh_sql_address_t nowhere;
    lh_sql_address_init(&nowhere);

    EXPECT_EQ(lh_os_sql_keep_try_get_db(&keep, &nowhere), nullptr);
    EXPECT_GT(error_size(lh_os_sql_keep_get_db(&keep)), 0U);
    EXPECT_EQ(lh_sql_db_get_error_kind(lh_os_sql_keep_get_db(&keep)),
              lh_sql_error_kind_unsupported);
    EXPECT_FALSE(lh_sql_address_is_set(lh_os_sql_keep_get_address(&keep)));
    EXPECT_FALSE(lh_sql_db_is_open(lh_os_sql_keep_get_db(&keep)));

    lh_sql_address_deinit(&nowhere);
    lh_os_sql_keep_deinit(&keep);
}

} // namespace
