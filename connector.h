/******************************************************************************
/ PROGRAM:  connector.h
/ SYNOPSIS: Program exposes a connection to the MySQL database
******************************************************************************/
#ifndef CONNECTOR_H
#define CONNECTOR_H

#include "repository/sql.h"
#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>

MYSQL *db_connect(char *host, char *user, char *password, char *db, unsigned int port);
void db_close(MYSQL *db);

#endif