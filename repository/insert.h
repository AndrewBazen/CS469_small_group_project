#ifndef REPO_INSERT_H
#define REPO_INSERT_H

#include <mysql/mysql.h>
#include "result.h"

result* insert(MYSQL *db, char *db_name, char *table_name, char *cols[], char *values[]);

#endif
