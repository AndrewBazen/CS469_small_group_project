#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "delete.h"
#include "sql.h"
#include "statement.h"

result* update(MYSQL *db, char *table_name, char *set_field, char *set_value, char *where_field, char *where_value) {
  MYSQL_BIND    bind[2];
  MYSQL_STMT    *stmt;
  result        *res = NULL;
  char          query[REPO_BUFFER_SIZE];
  char          affected_rows_str[64];
  int           affected_rows;

  // format the query
  sprintf(query, SQL_UPDATE, table_name, set_field, where_field);

  printf("Database: Querying: %s\n", query);

  // create prepared statement
  stmt = create_prepared_statement(db, query);
  if (stmt == NULL) {
    return NULL;
  }
  
  // bind the value to parameter
  memset(bind, 0, sizeof(bind));
  bind[0].buffer_type = MYSQL_TYPE_STRING;
  bind[0].buffer = set_value;
  bind[0].is_null = 0;
  bind[0].buffer_length = strlen(set_value);
  bind[1].buffer_type = MYSQL_TYPE_STRING;
  bind[1].buffer = where_value;
  bind[1].is_null = 0;
  bind[1].buffer_length = strlen(where_value);

  // give the bound params to the prepared statement
  if (mysql_stmt_bind_param(stmt, bind)) {
    fprintf(stderr, "Database Error: Cannot bind params: %s\n", mysql_stmt_error(stmt));
    return NULL;
  }

  // execute the statement
  if (mysql_stmt_execute(stmt)) {
    fprintf(stderr, "Database Error: Cannot execute statement: %s\n", mysql_stmt_error(stmt));
    return NULL;
  }

  affected_rows = mysql_stmt_affected_rows(stmt);
  sprintf(affected_rows_str, "%d", affected_rows);

  insert_end(&res, &res, affected_rows_str, true, MYSQL_TYPE_INT24);
  res->num_results = 1;
  res->num_rows = 1;
  
  mysql_stmt_free_result(stmt);
  mysql_stmt_close(stmt);

  return res;
}
