#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "./sql.h"
#include "./connector.h"
#include <string.h>  // For strlen and strcmp functions
#include <mysql/mysql.h>

#define MAX_INPUT_SIZE 256

void repl(MYSQL *db) {
    char input[MAX_INPUT_SIZE];
    while (1) {
        printf("DATABASE QUERIES> ");
        if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL) {
            break;
        }

        //Remove trailing newline character from input
        size_t length = strlen(input);
        if (length > 0 && input[length - 1] == '\n') {
            input[length - 1] = '\0';
        }

        //use "exit" to quit repl
        if (strcmp(input, "exit") == 0) {
            printf("Goodbye\n");
            break;
        }

        // Placeholder: Later, process commands and interact with the database
        if (mysql_query(db, input)) {
            fprintf(stderr, "MySQL query failed: %s\n", mysql_error(db));
        } else {
            MYSQL_RES *result = mysql_store_result(db);
            if (result) {
                MYSQL_ROW row;
                while ((row = mysql_fetch_row(result))) {
                    for (int i = 0; i < mysql_num_fields(result); i++) {
                        printf("%s\t", row[i] ? row[i] : "NULL");
                    }
                    printf("\n");
                }
                mysql_free_result(result);
            } else {
                if (mysql_field_count(db) == 0) {
                    printf("%lu rows affected\n", (unsigned long)mysql_affected_rows(db));
                } else {
                    fprintf(stderr, "Could not retrieve result set: %s\n", mysql_error(db));
                }
            }
        }
    }
}

int main(int argc, char **argv, char **envp) {
  char *user = getenv("MYSQL_USER"),
       *password = getenv("MYSQL_PASSWORD"),
       *database = getenv("MYSQL_DATABASE");
  MYSQL *db;

  db = db_connect("127.0.0.1", "root", "root", "dogs", 3306);

  if (!db) {
    db_close(db);
    return EXIT_FAILURE;
  }

  printf("Welcome to the database REPL. Type 'exit' to quit.\n");
    repl(db);

  db_close(db);

  return EXIT_SUCCESS;
}
