#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sql.h"
#include "result.h"
#include "statement.h"

result* get_table_names(MYSQL *db, char *db_name) {
  const int     num_params = 1;
  const int     num_cols = 1;

  MYSQL_BIND    bind[num_params];
  MYSQL_BIND    result_bind[num_cols];
  MYSQL_STMT    *stmt;
  int           status;
  int           num_results;
  char          result_name[REPO_BUFFER_SIZE];
  unsigned long name_length = strlen(db_name);
  result        *head = NULL;
  result        *tail = NULL;

  printf("Database: Querying: %s\n", SQL_TABLE_NAMES_FROM_DB);

  // create prepared statement
  stmt = create_prepared_statement(db, SQL_TABLE_NAMES_FROM_DB);
  if (stmt == NULL) {
    fprintf(stderr, "Database Error: Cannot create statement: %s\n", mysql_stmt_error(stmt));
    return NULL;
  }

  // bind parameters
  memset(bind, 0, sizeof(bind));
  bind[0].buffer_type = MYSQL_TYPE_STRING;
  bind[0].buffer = db_name;
  bind[0].buffer_length = REPO_BUFFER_SIZE;
  bind[0].is_null = 0;
  bind[0].length = &name_length;
  
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
  
  // bind the result
  memset(result_bind, 0, sizeof(result_bind));
  result_bind[0].buffer_type = MYSQL_TYPE_STRING;
  result_bind[0].buffer = (char *)result_name;
  result_bind[0].buffer_length = REPO_BUFFER_SIZE;
  result_bind[0].is_null = 0;
  result_bind[0].length = &name_length;

  if (mysql_stmt_bind_result(stmt, result_bind)) {
    fprintf(stderr, "Database Error: Cannot bind result: %s\n", mysql_stmt_error(stmt));
    return NULL;
  }

  if (mysql_stmt_store_result(stmt)) {
    fprintf(stderr, "Database Error: Cannot store result: %s\n", mysql_stmt_error(stmt));
    return NULL;
  }

  num_results = 0;
  status = mysql_stmt_fetch(stmt);
  if (status == MYSQL_NO_DATA) {
    printf("Database: No results found\n");
  }
  while (status == 0) {
    insert_end(&head, &tail, &result_name[0], true, MYSQL_TYPE_STRING);
    status = mysql_stmt_fetch(stmt);
    num_results++;
  }
  if (status != MYSQL_NO_DATA) {
    fprintf(stderr, "Database Error: Fetch error: %s\n", mysql_stmt_error(stmt));
  }
  if (head != NULL) {
    head->num_results = num_results;
    head->num_rows = num_results;
  }
  
  mysql_stmt_free_result(stmt);
  mysql_stmt_close(stmt);

  return head;
}
