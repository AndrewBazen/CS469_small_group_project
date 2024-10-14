#include <stdio.h>
#include <string.h>
#include "statement.h"

MYSQL_STMT* create_prepared_statement(MYSQL *db, char *statement) {
  MYSQL_STMT    *stmt;

  // init prepared statement
  stmt = mysql_stmt_init(db);
  if (!stmt) {
    fprintf(stderr, "Database Error: Cannot create prepared statement: %s\n", mysql_stmt_error(stmt));
    return NULL;
  }
  // give the query to the prepared statement
  if (mysql_stmt_prepare(stmt, statement, strlen(statement)) != 0) {
    fprintf(stderr, "Database Error: Cannot create prepared statement: %s\n", mysql_stmt_error(stmt));
    return NULL;
  }

  return stmt;
}
