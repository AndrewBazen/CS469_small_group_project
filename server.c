#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mysql/mysql.h>
#include "./connector.h"

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

  return EXIT_SUCCESS;
}