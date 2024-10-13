#include <mysql/mysql.h>
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

result* insert(MYSQL *db, char *db_name, char *table_name, char *values[]) {
  int           values_length = get_values_length(values);

  MYSQL_BIND    bind[values_length];
  MYSQL_STMT    *stmt;
  result        *col_result_head;
  result        *col_result_curr;
  result        *res = NULL;
  unsigned long table_name_length = strlen(table_name);
  char          placeholders[BUFFER_SIZE / 2] = "";
  char          param[BUFFER_SIZE / 2] = "";
  char          query[BUFFER_SIZE * 2];
  int           affected_rows;
  int           num_results;
  int           status;
  int           i;

  // get columns in order to format the query
  col_result_head = get_columns(db, db_name, table_name);
  if (col_result_head == NULL) {
    fprintf(stderr, "MySQL query failed: Could not get the columns\n");
    return NULL;
  }

  i = 0;
  col_result_curr = col_result_head;

  // append the parameters and placeholders, add a comma if not last
  while (col_result_curr != NULL) {
    if ((i % (col_result_head->num_rows + 1)) == 0) {
      strcat(param, *col_result_curr->buffer);
      strcat(placeholders, "?");
      if (col_result_head->num_rows < col_result_head->num_results - i - 1) {
        strcat(param, ",");
        strcat(placeholders, ",");
      }
    }
    col_result_curr = col_result_curr->next;
    i++;
  }

  // validate the columns and values are of same length
  if (values_length != col_result_head->num_rows) {
    fprintf(stderr, "Database Error: Number of values (%d) mismatch number of columns (%d)\n", values_length, col_result_head->num_rows);
    return NULL;
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