#ifndef REPO_DELETE_H
#define REPO_DELETE_H

#include <mysql/mysql.h>
#include "result.h"

result* delete_from(MYSQL *db, char *table_name, char *field, char *value);

#endif
