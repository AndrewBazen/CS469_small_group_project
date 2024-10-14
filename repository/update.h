#ifndef REPO_UPDATE_H
#define REPO_UPDATE_H

#include <mysql/mysql.h>
#include "result.h"

result* update(MYSQL *db, char *table_name, char *set_field, char *set_value, char *where_field, char *where_value);

#endif
