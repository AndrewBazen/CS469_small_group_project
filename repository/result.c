#include <mysql/mysql.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sql.h"
#include "result.h"

result* create_result(char *str, bool end_of_row, int type) {
  result *new_result = (result *)malloc(sizeof(result));

  *new_result->buffer = (char *)malloc(strlen(str) + 1);
  strcpy(*new_result->buffer, str);
  new_result->next = NULL;

  return new_result;
}

void insert_end(result **head, result **tail, char *str, bool end_of_row, int type) {
  result *new_node = create_result(str, end_of_row, type);

  if (*head == NULL) {
    *head = new_node;
    *tail = new_node;
  } else {
    result *temp = *tail;
    temp->next = new_node;
    *tail = new_node;
  }
}

void free_results(result *head) {
  result *temp;

  while (head != NULL) {
    temp = head;
    head = head->next;
    free(temp->buffer);
    free(temp);
  }
}