#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "repository/sql.h"
#include "connector.h"
#include <string.h>
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

        //process MySQL inputs
        process_input(input, db);
    }
}

void process_input(char * input, MYSQL * db) {
  //process "tables" query
  if (strcmp(input, "tables") == 0) {
    if (mysql_query(db, "SHOW TABLES")) { //if non zero value -> error
      fprintf(stderr, "MySQL query failed: %s\n", mysql_error(db));
    } else { //if zero value -> fetch and print result
      MYSQL_RES * result = mysql_store_result(db); //retrieve rows
      MYSQL_ROW row;
      while ((row = mysql_fetch_row(result))) { //loop through each row
        printf("%s\n", row[0]); //output each row
      }
      mysql_free_result(result); //free allocated memory for next query
    }
    //process "columns" query given a table_name
  } else if (strncmp(input, "columns", 7) == 0) {
    char table_name[MAX_INPUT_SIZE];
    char query[MAX_INPUT_SIZE];

    sscanf(input, "columns %s", table_name); //get table_name

    snprintf(query, MAX_INPUT_SIZE, "SHOW COLUMNS FROM %s", table_name); //construct query

    if (mysql_query(db, query)) {
      fprintf(stderr, "MySQL query failed: %s\n", mysql_error(db));
    } else {
      MYSQL_RES * result = mysql_store_result(db);
      MYSQL_ROW row;

      while ((row = mysql_fetch_row(result))) { //loop through each row
        printf("%s\t%s\n", row[0], row[1]); //output column and column name
      }
      mysql_free_result(result);
    }
    //process "select" query given an input to select
  } else if (strncmp(input, "select", 6) == 0) {
    char query[MAX_INPUT_SIZE];

    snprintf(query, MAX_INPUT_SIZE, "%s", input);

    if (mysql_query(db, query)) {
      fprintf(stderr, "MySQL query failed: %s\n", mysql_error(db));
    } else {
      MYSQL_RES * result = mysql_store_result(db);
      MYSQL_ROW row;

      while ((row = mysql_fetch_row(result))) { //loop through each row
        for (int i = 0; i < mysql_num_fields(result); i++) {
          printf("%s\t", row[i] ? row[i] : "empty"); //prints the field or "empty"
        }
        printf("\n");
      }
      mysql_free_result(result);
    }
    //process "insert" query given an input to insert
  } else if (strncmp(input, "insert", 6) == 0) {
    char query[MAX_INPUT_SIZE];

    snprintf(query, MAX_INPUT_SIZE, "%s", input);

    if (mysql_query(db, query)) {
      fprintf(stderr, "MySQL query failed: %s\n", mysql_error(db));
    } else {
        //print inserted rows
      printf("%lu row has been inserted\n", (unsigned long) mysql_affected_rows(db));
    }
    //process "update" query
  } else if (strncmp(input, "update", 6) == 0) {
    char query[MAX_INPUT_SIZE];

    snprintf(query, MAX_INPUT_SIZE, "%s", input);

    if (mysql_query(db, query)) {
      fprintf(stderr, "MySQL query failed: %s\n", mysql_error(db));
    } else {
        //print updated rows
      printf("%lu row(s) updated\n", (unsigned long) mysql_affected_rows(db));
    }
    //process "delete" query
  } else if (strncmp(input, "delete", 6) == 0) {
    char query[MAX_INPUT_SIZE];

    snprintf(query, MAX_INPUT_SIZE, "%s", input);

    if (mysql_query(db, query)) {
      fprintf(stderr, "MySQL query failed: %s\n", mysql_error(db));
    } else {
      //print deleted rows
      printf("%lu row(s) deleted\n", (unsigned long) mysql_affected_rows(db));
    }
  } else {
    printf("Unknown command or incorrect syntax.\n");
    printf("Known commands: tables, columns, select, insert, update, and delete\n");
  }
}


int main(int argc, char **argv, char **envp) {
    MYSQL *db;
    char *user = getenv("MYSQL_USER"),
         *password = getenv("MYSQL_PASSWORD"),
         *database = getenv("MYSQL_DATABASE");

    db = db_connect("mysql", user, password, database, 3306);

    if (!db) {
        db_close(db);
        printf("Server: Unable to connect to database\n");
        return EXIT_FAILURE;
    }

    repl(db);

    db_close(db);

    return EXIT_SUCCESS;
}