#ifndef REPO_REQUEST_H
#define REPO_REQUEST_H

#define REPO_BUFFER_SIZE 512

#include <stdbool.h>

typedef struct result {
  char            *buffer[REPO_BUFFER_SIZE];
  struct result   *next;
  bool            end_of_row;
  int             type;
  int             num_results;
  int             num_rows;
} result;

result* create_result(char *str, bool end_of_row, int type);
void insert_end(result **head, result **tail, char *str, bool end_of_row, int type);
void free_results(result *head);

#endif
