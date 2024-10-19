#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "columns.h"
#include "insert.h"
#include "result.h"
#include "sql.h"
#include "statement.h"
#include "type.h"

int get_values_length(char *values[]);

result* insert(MYSQL *db, char *db_name, char *table_name, char *cols[], char *values[]) {
  int           values_length = get_values_length(values);

  MYSQL_BIND    bind[values_length];
  MYSQL_STMT    *stmt;
  result        *res = NULL;
  char          placeholders[REPO_BUFFER_SIZE / 2] = "";
  char          param[REPO_BUFFER_SIZE / 2] = "";
  char          query[REPO_BUFFER_SIZE * 2];
  int           affected_rows;
  int           i;

  i = 0;

  // append the parameters and placeholders, add a comma if not last
  while (i < values_length) {
    strcat(param, cols[i]);
    strcat(placeholders, "?");
    if (i < values_length - 1) {
      strcat(param, ",");
      strcat(placeholders, ",");
    }
    i++;
  }

  // format the query
  sprintf(query, SQL_INSERT, table_name, param, placeholders);

  printf("Database: Querying: %s\n", query);

  // create prepared statement
  stmt = create_prepared_statement(db, query);
  if (stmt == NULL) {
    fprintf(stderr, "Database Error: Cannot create statement: %s\n", mysql_stmt_error(stmt));
    return NULL;
  }

  // bind the values to their parameter
  memset(bind, 0, sizeof(bind));
  i = 0;
  while (i < values_length) {
    bind[i].buffer_type = MYSQL_TYPE_STRING;
    bind[i].buffer = (char *)values[i];
    bind[i].is_null = 0;
    bind[i].buffer_length = strlen(values[i]);
  
    i++;
  }

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
  if (affected_rows != 1) {
    fprintf(stderr, "Database Error: Invalid affected rows\n");
    return NULL;
  }

  insert_end(&res, &res, "1", true, MYSQL_TYPE_INT24);
  res->num_results = 1;
  res->num_rows = 1;
  
  mysql_stmt_free_result(stmt);
  mysql_stmt_close(stmt);

  return res;
}

int get_values_length(char *values[]) {
  int i = 0;

  while (values[i + 1] != NULL) {
    i++;
  }

  return i;
}