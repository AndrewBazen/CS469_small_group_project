#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "columns.h"
#include "result.h"
#include "sql.h"
#include "statement.h"
#include "type.h"

result* select_all(MYSQL *db, char *db_name, char *table_name) {
  const int     num_params = 2;

  MYSQL_ROW     row;
  MYSQL_RES     *res;
  char          query[REPO_BUFFER_SIZE];
  char          param[REPO_BUFFER_SIZE - 56] = "";
  int           i = 0;
  int           j = 0;
  int           num_rows = 0;
  result        *head = NULL;
  result        *tail = NULL;
  result        *col_result_head;
  result        *col_result_curr;

  col_result_head = get_columns(db, db_name, table_name);
  col_result_curr = col_result_head;
  while (col_result_curr != NULL) {
    if ((i % (col_result_head->num_rows + 1)) == 0) {
      strcat(param, *col_result_curr->buffer);
      strcat(param, ",");
    }
    col_result_curr = col_result_curr->next;
    i++;
  }
  // drop the last comma
  param[strlen(param)-1] = '\0';
  printf("%s\n", param);

  sprintf(query, SQL_SELECT_FROM_TABLE, param, table_name);

  int  types[col_result_head->num_rows];
  memset(types,  0, sizeof(types));

  printf("Database: Querying: %s\n", query);

  // execute the statement
  if (mysql_query(db, query)) {
    fprintf(stderr, "MySQL query failed: %s\n", mysql_error(db));
    return NULL;
  }

  if ((res = mysql_store_result(db)) == NULL) {
    fprintf(stderr, "Database Error: Cannot store result: %s\n", mysql_error(db));
    return NULL;
  }

  // get rows
  row = mysql_fetch_row(res);
  if (row == NULL) {
    printf("Database: No results found\n");
    return NULL;
  }

  // map types to columns
  col_result_curr = col_result_head;
  while (col_result_curr != NULL) {
    if ((i % col_result_head->num_rows) == 2) {
      types[j] = convert_type(*col_result_curr->buffer);
      j++;
    }
    i++;
    col_result_curr = col_result_curr->next;
  }

  j = 0;
  while (row != NULL) {
    i = 0;
    col_result_curr = col_result_head;

    while (i < col_result_head->num_rows) {
      insert_end(&head, &tail, row[i], i == col_result_head->num_rows - 1, types[i]);
      i++;
      j++;
    }

    row = mysql_fetch_row(res);
    num_rows++;
  }

  if (head != NULL) {
    head->num_results = j;
    head->num_rows = num_rows;
  }

  return head;
}
