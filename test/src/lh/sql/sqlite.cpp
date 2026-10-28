#include <gtest/gtest.h>

#include <lh/sql/sqlite.h>

#include <string>

namespace
{

std::string
text(lh_str_view_t v)
{
    return std::string(lh_str_view_get_data(&v) ? lh_str_view_get_data(&v) : "",
                       lh_str_view_get_size(&v));
}

lh_str_view_t
view(const std::string &s)
{
    lh_str_view_t v;
    if (s.empty())
    {
        lh_str_view_init_empty(&v);
    }
    else
    {
        lh_str_view_init_by_size(&v, s.data(), s.size());
    }
    return v;
}

struct memory_db
{
    lh_sql_db_t db;

    memory_db()
    {
        lh_sql_db_init(&db);
        EXPECT_TRUE(lh_sql_db_open_sqlite(&db, lh_str_view_lit(":memory:")))
            << text(lh_sql_db_get_error(&db));
    }

    ~memory_db()
    {
        lh_sql_db_deinit(&db);
    }
};

TEST(sql_sqlite, opens_an_in_memory_database)
{
    memory_db m;
    EXPECT_TRUE(lh_sql_db_is_open(&m.db));
    EXPECT_EQ(std::string(lh_sql_driver_get_name(lh_sql_db_get_driver(&m.db))), "sqlite");
}

TEST(sql_sqlite, exec_runs_a_whole_script)
{
    memory_db m;
    ASSERT_TRUE(
        lh_sql_db_exec(&m.db, lh_str_view_lit("CREATE TABLE t(id INTEGER PRIMARY KEY, name TEXT);"
                                              "INSERT INTO t(name) VALUES ('a');"
                                              "INSERT INTO t(name) VALUES ('b'); -- done\n")))
        << text(lh_sql_db_get_error(&m.db));
    EXPECT_EQ(lh_sql_db_get_last_insert_id(&m.db), 2);
}

TEST(sql_sqlite, exec_reports_the_failing_statement)
{
    memory_db m;
    EXPECT_FALSE(lh_sql_db_exec(&m.db, lh_str_view_lit("SELECT * FROM nowhere")));
    EXPECT_NE(text(lh_sql_db_get_error(&m.db)).find("no such table"), std::string::npos)
        << text(lh_sql_db_get_error(&m.db));
}

TEST(sql_sqlite, exec_does_not_need_a_nul_after_the_text)
{
    memory_db m;
    /* Only the first 23 characters are SQL; the rest is garbage after the view. */
    const std::string script = "CREATE TABLE ok(x INT);THIS IS NOT SQL";
    lh_str_view_t first;
    lh_str_view_init_by_size(&first, script.data(), 23U);
    EXPECT_TRUE(lh_sql_db_exec(&m.db, first)) << text(lh_sql_db_get_error(&m.db));
}

TEST(sql_sqlite, binds_and_reads_every_kind)
{
    memory_db m;
    ASSERT_TRUE(
        lh_sql_db_exec(&m.db, lh_str_view_lit("CREATE TABLE t(n INTEGER, s TEXT, z TEXT)")));

    lh_sql_stmt_t insert;
    ASSERT_TRUE(
        lh_sql_db_prepare(&m.db, &insert, lh_str_view_lit("INSERT INTO t VALUES (?, ?, ?)")));
    const std::string with_nul("a\0b", 3);
    EXPECT_TRUE(lh_sql_stmt_bind_sint(&insert, 1, -9223372036854775807LL - 1));
    EXPECT_TRUE(lh_sql_stmt_bind_str(&insert, 2, view(with_nul)));
    EXPECT_TRUE(lh_sql_stmt_bind_null(&insert, 3));
    EXPECT_EQ(lh_sql_stmt_step(&insert), lh_sql_step_done);

    /* The same statement again, with an empty string and a big number. */
    EXPECT_TRUE(lh_sql_stmt_reset(&insert));
    EXPECT_TRUE(lh_sql_stmt_bind_sint(&insert, 1, 76561198000000000LL));
    EXPECT_TRUE(lh_sql_stmt_bind_str(&insert, 2, view("")));
    EXPECT_TRUE(lh_sql_stmt_bind_str(&insert, 3, lh_str_view_lit("\xD0\x9A")));
    EXPECT_EQ(lh_sql_stmt_step(&insert), lh_sql_step_done);
    lh_sql_stmt_finalize(&insert);

    lh_sql_stmt_t select;
    ASSERT_TRUE(
        lh_sql_db_prepare(&m.db, &select, lh_str_view_lit("SELECT n, s, z FROM t ORDER BY rowid")));
    EXPECT_EQ(lh_sql_stmt_get_column_count(&select), 3U);

    ASSERT_EQ(lh_sql_stmt_step(&select), lh_sql_step_row);
    EXPECT_EQ(lh_sql_stmt_get_column_sint(&select, 0), -9223372036854775807LL - 1);
    EXPECT_EQ(text(lh_sql_stmt_get_column_str(&select, 1)), with_nul);
    EXPECT_TRUE(lh_sql_stmt_is_column_null(&select, 2));
    EXPECT_EQ(text(lh_sql_stmt_get_column_str(&select, 2)), "");

    ASSERT_EQ(lh_sql_stmt_step(&select), lh_sql_step_row);
    EXPECT_EQ(lh_sql_stmt_get_column_sint(&select, 0), 76561198000000000LL);
    EXPECT_FALSE(lh_sql_stmt_is_column_null(&select, 1));
    EXPECT_EQ(text(lh_sql_stmt_get_column_str(&select, 1)), "");
    EXPECT_EQ(text(lh_sql_stmt_get_column_str(&select, 2)), "\xD0\x9A");

    EXPECT_EQ(lh_sql_stmt_step(&select), lh_sql_step_done);
    lh_sql_stmt_finalize(&select);
}

TEST(sql_sqlite, a_failing_step_sets_the_error)
{
    memory_db m;
    ASSERT_TRUE(lh_sql_db_exec(&m.db, lh_str_view_lit("CREATE TABLE u(x INTEGER UNIQUE)")));
    ASSERT_TRUE(lh_sql_db_exec(&m.db, lh_str_view_lit("INSERT INTO u VALUES (1)")));

    lh_sql_stmt_t stmt;
    ASSERT_TRUE(lh_sql_db_prepare(&m.db, &stmt, lh_str_view_lit("INSERT INTO u VALUES (1)")));
    EXPECT_EQ(lh_sql_stmt_step(&stmt), lh_sql_step_error);
    EXPECT_NE(text(lh_sql_db_get_error(&m.db)).find("UNIQUE"), std::string::npos)
        << text(lh_sql_db_get_error(&m.db));
    lh_sql_stmt_finalize(&stmt);
}

TEST(sql_sqlite, prepare_rejects_bad_and_empty_sql)
{
    memory_db m;
    lh_sql_stmt_t stmt;

    EXPECT_FALSE(lh_sql_db_prepare(&m.db, &stmt, lh_str_view_lit("SELEKT 1")));
    EXPECT_FALSE(text(lh_sql_db_get_error(&m.db)).empty());
    lh_sql_stmt_finalize(&stmt);

    EXPECT_FALSE(lh_sql_db_prepare(&m.db, &stmt, lh_str_view_lit("  -- nothing\n")));
    EXPECT_EQ(text(lh_sql_db_get_error(&m.db)), "no statement in the SQL text");
    lh_sql_stmt_finalize(&stmt);
}

TEST(sql_sqlite, a_file_that_cannot_be_opened_fails_cleanly)
{
    lh_sql_db_t db;
    lh_sql_db_init(&db);
    EXPECT_FALSE(lh_sql_db_open_sqlite(&db, lh_str_view_lit("")));
    EXPECT_EQ(text(lh_sql_db_get_error(&db)), "empty database path");
    EXPECT_FALSE(lh_sql_db_open_sqlite(&db, lh_str_view_lit("/no/such/dir/at/all/x.db")));
    EXPECT_FALSE(lh_sql_db_is_open(&db));
    EXPECT_FALSE(text(lh_sql_db_get_error(&db)).empty());
    lh_sql_db_deinit(&db);
}

TEST(sql_sqlite, reopening_closes_the_old_database)
{
    memory_db m;
    ASSERT_TRUE(lh_sql_db_exec(&m.db, lh_str_view_lit("CREATE TABLE gone(x)")));
    ASSERT_TRUE(lh_sql_db_open_sqlite(&m.db, lh_str_view_lit(":memory:")));
    /* A fresh in-memory database: the table is not there any more. */
    EXPECT_FALSE(lh_sql_db_exec(&m.db, lh_str_view_lit("SELECT * FROM gone")));
}

} // namespace
