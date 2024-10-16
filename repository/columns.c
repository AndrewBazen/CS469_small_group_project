#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "columns.h"
#include "result.h"
#include "sql.h"
#include "statement.h"
#include <ctype.h>

/**
 * @return result order will be
 * 1-column_name,
 * 2-column_type,
 * 3-data_type,
 * 4-character_maximum_length
 */
result* get_columns(MYSQL *db, char *db_name, char *table_name) {
  const int     num_params = 2;
  const int     num_cols = 4;

  MYSQL_BIND    bind[num_params];
  MYSQL_BIND    result_bind[num_cols];
  MYSQL_STMT    *stmt;
  char          column_name[REPO_BUFFER_SIZE];
  char          column_type[REPO_BUFFER_SIZE];
  char          data_type[REPO_BUFFER_SIZE];
  char          character_maximum_length[REPO_BUFFER_SIZE];
  int           num_results;
  int           status;
  unsigned long db_name_length = strlen(db_name);
  unsigned long table_name_length = strlen(table_name);
  result        *head = NULL;
  result        *tail = NULL;

  printf("Database: Querying: %s\n", SQL_COLUMN_NAMES_FROM_TABLE);

  // create prepared statement
  stmt = create_prepared_statement(db, SQL_COLUMN_NAMES_FROM_TABLE);
  if (stmt == NULL) {
    fprintf(stderr, "Database Error: Cannot create statement: %s\n", mysql_stmt_error(stmt));
    return NULL;
  }

  memset(bind, 0, sizeof(bind));

  // bind table_schema
  bind[0].buffer_type = MYSQL_TYPE_STRING;
  bind[0].buffer = db_name;
  bind[0].buffer_length = REPO_BUFFER_SIZE;
  bind[0].is_null = 0;
  bind[0].length = &db_name_length;

  // bind table_name
  bind[1].buffer_type = MYSQL_TYPE_STRING;
  bind[1].buffer = table_name;
  bind[1].buffer_length = REPO_BUFFER_SIZE;
  bind[1].is_null = 0;
  bind[1].length = &table_name_length;
  
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

  memset(result_bind, 0, sizeof(result_bind));

  result_bind[0].buffer_type = MYSQL_TYPE_STRING;
  result_bind[0].buffer = (char *)column_name;
  result_bind[0].buffer_length = REPO_BUFFER_SIZE;
  result_bind[0].is_null = 0;

  result_bind[1].buffer_type = MYSQL_TYPE_STRING;
  result_bind[1].buffer = (char *)column_type;
  result_bind[1].buffer_length = REPO_BUFFER_SIZE;
  result_bind[1].is_null = 0;

  result_bind[2].buffer_type = MYSQL_TYPE_STRING;
  result_bind[2].buffer = (char *)data_type;
  result_bind[2].buffer_length = REPO_BUFFER_SIZE;
  result_bind[2].is_null = 0;

  result_bind[3].buffer_type = MYSQL_TYPE_INT24;
  result_bind[3].buffer = (char *)character_maximum_length;
  result_bind[3].buffer_length = REPO_BUFFER_SIZE;
  result_bind[3].is_null = 0;

  if (mysql_stmt_bind_result(stmt, result_bind)) {
    fprintf(stderr, "Database Error: Cannot bind result: %s\n", mysql_stmt_error(stmt));
    return NULL;
  }

  if (mysql_stmt_store_result(stmt)) {
    fprintf(stderr, "Database Error: Cannot store result: %s\n", mysql_stmt_error(stmt));
    return NULL;
  }

  status = mysql_stmt_fetch(stmt);
  if (status == MYSQL_NO_DATA) {
    printf("Database: No results found\n");
  }

  int i;
  num_results = 0;
  while (status == 0) {
    i = 0;
    while (i < num_cols) {
      switch(i) {
        case 0:
          insert_end(&head, &tail, &column_name[0], false, MYSQL_TYPE_STRING);
          break;
        case 1:
          insert_end(&head, &tail, &column_type[0], false, MYSQL_TYPE_STRING);
          break;
        case 2:
          insert_end(&head, &tail, &data_type[0], false, MYSQL_TYPE_STRING);
          break;
        case 3:
          if (isdigit(character_maximum_length[0])) {
            insert_end(&head, &tail, &character_maximum_length[0], true, MYSQL_TYPE_INT24);
          } else {
            insert_end(&head, &tail, "0", true, MYSQL_TYPE_INT24);
          }
          break;
      }
      i++;
      num_results++;
    }
    status = mysql_stmt_fetch(stmt);
  }
  if (status != MYSQL_NO_DATA) {
    fprintf(stderr, "Database Error: Fetch error: %s\n", mysql_stmt_error(stmt));
  }
  if (head != NULL) {
    head->num_results = num_results;
    head->num_rows = num_results / num_cols;
  }
  
  mysql_stmt_free_result(stmt);
  mysql_stmt_close(stmt);

  return head;
}