/**
 * @file sql.h
 * @brief SQL databases: a connection, prepared statements, and the backend
 *        table they dispatch through.
 *
 * A backend is included on its own: lh/sql/sqlite.h (with
 * ::LH_LIBRARY_OPTION_SQL_SQLITE).
 *
 * @see lh/sql/db.h
 * @see lh/sql/stmt.h
 * @see lh/sql/driver.h
 */

#ifndef LH_SQL_H
#define LH_SQL_H

#include <lh/sql/db.h>
#include <lh/sql/driver.h>
#include <lh/sql/step.h>
#include <lh/sql/stmt.h>

#endif /* LH_SQL_H */
