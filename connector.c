/******************************************************************************
/ PROGRAM:  connector.c
/ SYNOPSIS: Program exposes a connection to the MySQL database
******************************************************************************/

#include "connector.h"

MYSQL *db_connect(
    char *host, char *user, char *password, char *db, unsigned int port) {
  MYSQL *connection;
  MYSQL_RES *result;

  if ((connection = mysql_init(NULL)) == NULL) {
    fprintf(stderr, "Could not initialize mysql: %s\n", mysql_error(connection));
    return NULL;
  }

   mysql_ssl_set(connection, NULL, NULL, "/etc/mysql/certs/rootCA.pem", NULL, NULL);

  if (mysql_real_connect(connection, host, user, password, db, port, NULL, CLIENT_SSL) == NULL) {
    fprintf(stderr, "Could not connect to MySQL database: %s\n", mysql_error(connection));
    mysql_close(connection);
    return NULL;
  }

  if (mysql_query(connection, "SELECT 1+1")) {
    fprintf(stderr, "MySQL query failed: %s\n", mysql_error(connection));
    mysql_close(connection);
    return NULL;
  }

  if ((result = mysql_store_result(connection)) == NULL) {
    fprintf(stderr, "%s\n", mysql_error(connection));
    mysql_close(connection);
    return NULL;
  }

  printf("Database Connection Established on port %d\n", port);

  return connection;
}

void db_close(MYSQL *db) {
  mysql_close(db);
}
