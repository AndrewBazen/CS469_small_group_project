#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sql.h"

typedef struct result {
  char            *buffer[BUFFER_SIZE];
  struct result   *next;
} result;

result* create_result(char *str) {
  result *new_result = (result *)malloc(sizeof(result));

  *new_result->buffer = (char *)malloc(strlen(str) + 1);
  strcpy(*new_result->buffer, str);
  new_result->next = NULL;

  return new_result;
}

void insert_end(result **head, result **tail, char *str) {
  result *new_node = create_result(str);

  if (*head == NULL) {
    *head = new_node;
    *tail = new_node;
  } else {
    result *temp = *tail;
    temp->next = new_node;
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
