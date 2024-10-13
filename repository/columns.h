#ifndef REPO_COLUMNS_H
#define REPO_COLUMNS_H

#include <mysql/mysql.h>
#include "result.h"

result* get_columns(MYSQL *db, char *db_name, char *table_name);

#endif
