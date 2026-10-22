#include <gtest/gtest.h>

#include <lh/sql.h>

#include <string>
#include <vector>

namespace
{

/* A backend that records what it was asked and answers from a script. */
struct fake
{
    std::vector<std::string> calls;
    bool fail_exec = false;
    int rows = 0;
};

fake *g_fake = nullptr;
int g_handle = 0;

std::string
text(lh_str_view_t v)
{
    return std::string(lh_str_view_get_data(&v) ? lh_str_view_get_data(&v) : "",
                       lh_str_view_get_size(&v));
}

void
fake_close(lh_sql_db_t *db)
{
    g_fake->calls.push_back("close");
    lh_sql_db_set_handle(db, nullptr);
}

lh_bool_t
fake_exec(lh_sql_db_t *db, lh_str_view_t sql)
{
    g_fake->calls.push_back("exec " + text(sql));
    return g_fake->fail_exec ? lh_sql_db_fail(db, lh_str_view_lit("no such table")) : lh_bool_true;
}

lh_sllong_t
fake_last_insert_id(lh_sql_db_t *)
{
    return 42;
}

lh_bool_t
fake_prepare(lh_sql_db_t *, lh_sql_stmt_t *stmt, lh_str_view_t sql)
{
    g_fake->calls.push_back("prepare " + text(sql));
    lh_sql_stmt_set_handle(stmt, &g_handle);
    return lh_bool_true;
}

void
fake_finalize(lh_sql_stmt_t *)
{
    g_fake->calls.push_back("finalize");
}

lh_bool_t
fake_bind_null(lh_sql_stmt_t *, lh_usize_t index)
{
    g_fake->calls.push_back("null " + std::to_string(index));
    return lh_bool_true;
}

lh_bool_t
fake_bind_sint(lh_sql_stmt_t *, lh_usize_t index, lh_sllong_t value)
{
    g_fake->calls.push_back("sint " + std::to_string(index) + "=" + std::to_string(value));
    return lh_bool_true;
}

lh_bool_t
fake_bind_str(lh_sql_stmt_t *, lh_usize_t index, lh_str_view_t value)
{
    g_fake->calls.push_back("str " + std::to_string(index) + "=" + text(value));
    return lh_bool_true;
}

lh_sql_step_t
fake_step(lh_sql_stmt_t *)
{
    return g_fake->rows-- > 0 ? lh_sql_step_row : lh_sql_step_done;
}

lh_bool_t
fake_reset(lh_sql_stmt_t *)
{
    g_fake->calls.push_back("reset");
    return lh_bool_true;
}

lh_usize_t
fake_column_count(const lh_sql_stmt_t *)
{
    return 2U;
}

lh_bool_t
fake_column_is_null(const lh_sql_stmt_t *, lh_usize_t index)
{
    return index == 1U ? lh_bool_true : lh_bool_false;
}

lh_sllong_t
fake_column_sint(const lh_sql_stmt_t *, lh_usize_t index)
{
    return static_cast<lh_sllong_t>(index) + 100;
}

lh_str_view_t
fake_column_str(const lh_sql_stmt_t *, lh_usize_t)
{
    return lh_str_view_lit("cell");
}

const lh_sql_driver_t k_fake_driver = {
    "fake",        fake_close,        fake_exec,           fake_last_insert_id, fake_prepare,
    fake_finalize, fake_bind_null,    fake_bind_sint,      fake_bind_str,       fake_step,
    fake_reset,    fake_column_count, fake_column_is_null, fake_column_sint,    fake_column_str,
};

struct open_db
{
    fake backend;
    lh_sql_db_t db;

    open_db()
    {
        g_fake = &backend;
        lh_sql_db_init(&db);
        lh_sql_db_set_handle(&db, &g_handle);
        lh_sql_db_set_driver(&db, &k_fake_driver);
    }

    ~open_db()
    {
        lh_sql_db_deinit(&db);
        g_fake = nullptr;
    }
};

TEST(sql_db, a_closed_database_refuses_and_says_why)
{
    lh_sql_db_t db;
    lh_sql_stmt_t stmt;

    lh_sql_db_init(&db);
    EXPECT_FALSE(lh_sql_db_is_open(&db));
    EXPECT_FALSE(lh_sql_db_exec(&db, lh_str_view_lit("SELECT 1")));
    EXPECT_EQ(text(lh_sql_db_get_error(&db)), "database is not open");
    EXPECT_EQ(lh_sql_db_get_last_insert_id(&db), 0);

    EXPECT_FALSE(lh_sql_db_prepare(&db, &stmt, lh_str_view_lit("SELECT 1")));
    EXPECT_FALSE(lh_sql_stmt_is_prepared(&stmt));
    EXPECT_EQ(lh_sql_stmt_step(&stmt), lh_sql_step_error);
    EXPECT_EQ(text(lh_sql_db_get_error(&db)), "statement is not prepared");
    EXPECT_FALSE(lh_sql_stmt_bind_sint(&stmt, 1, 1));
    EXPECT_EQ(lh_sql_stmt_get_column_count(&stmt), 0U);
    EXPECT_TRUE(lh_sql_stmt_is_column_null(&stmt, 0));
    EXPECT_EQ(text(lh_sql_stmt_get_column_str(&stmt, 0)), "");
    lh_sql_stmt_finalize(&stmt);
    lh_sql_db_deinit(&db);
}

TEST(sql_db, operations_go_through_the_driver)
{
    open_db o;

    EXPECT_EQ(std::string(lh_sql_driver_get_name(lh_sql_db_get_driver(&o.db))), "fake");
    EXPECT_TRUE(lh_sql_db_exec(&o.db, lh_str_view_lit("CREATE TABLE t(x)")));
    EXPECT_EQ(lh_sql_db_get_last_insert_id(&o.db), 42);

    o.backend.fail_exec = true;
    EXPECT_FALSE(lh_sql_db_exec(&o.db, lh_str_view_lit("DROP TABLE nope")));
    EXPECT_EQ(text(lh_sql_db_get_error(&o.db)), "no such table");
    /* The next success forgets the old error. */
    o.backend.fail_exec = false;
    EXPECT_TRUE(lh_sql_db_exec(&o.db, lh_str_view_lit("SELECT 1")));
    EXPECT_EQ(text(lh_sql_db_get_error(&o.db)), "");

    EXPECT_EQ(o.backend.calls, (std::vector<std::string>{"exec CREATE TABLE t(x)",
                                                         "exec DROP TABLE nope", "exec SELECT 1"}));
}

TEST(sql_db, a_statement_binds_steps_and_reads_through_the_driver)
{
    open_db o;
    lh_sql_stmt_t stmt;

    ASSERT_TRUE(lh_sql_db_prepare(&o.db, &stmt, lh_str_view_lit("SELECT ?, ?, ?")));
    EXPECT_TRUE(lh_sql_stmt_is_prepared(&stmt));
    EXPECT_TRUE(lh_sql_stmt_bind_sint(&stmt, 1, -7));
    EXPECT_TRUE(lh_sql_stmt_bind_str(&stmt, 2, lh_str_view_lit("a b")));
    EXPECT_TRUE(lh_sql_stmt_bind_null(&stmt, 3));

    o.backend.rows = 2;
    int seen = 0;
    while (lh_sql_stmt_step(&stmt) == lh_sql_step_row)
    {
        ++seen;
        EXPECT_EQ(lh_sql_stmt_get_column_count(&stmt), 2U);
        EXPECT_EQ(lh_sql_stmt_get_column_sint(&stmt, 0), 100);
        EXPECT_FALSE(lh_sql_stmt_is_column_null(&stmt, 0));
        EXPECT_TRUE(lh_sql_stmt_is_column_null(&stmt, 1));
        EXPECT_EQ(text(lh_sql_stmt_get_column_str(&stmt, 0)), "cell");
    }
    EXPECT_EQ(seen, 2);
    EXPECT_TRUE(lh_sql_stmt_reset(&stmt));
    lh_sql_stmt_finalize(&stmt);
    EXPECT_FALSE(lh_sql_stmt_is_prepared(&stmt));
    lh_sql_stmt_finalize(&stmt); /* a second finalize does nothing */

    EXPECT_EQ(o.backend.calls,
              (std::vector<std::string>{"prepare SELECT ?, ?, ?", "sint 1=-7", "str 2=a b",
                                        "null 3", "reset", "finalize"}));
}

TEST(sql_db, close_calls_the_driver_once_and_leaves_it_closed)
{
    open_db o;

    lh_sql_db_close(&o.db);
    EXPECT_FALSE(lh_sql_db_is_open(&o.db));
    EXPECT_EQ(lh_sql_db_get_driver(&o.db), nullptr);
    lh_sql_db_close(&o.db);
    EXPECT_EQ(o.backend.calls, (std::vector<std::string>{"close"}));
}

} // namespace
