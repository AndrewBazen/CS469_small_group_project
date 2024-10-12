#ifndef REPO_TABLE_NAME_H
#define REPO_TABLE_NAME_H

#include <mysql/mysql.h>
#include "result.h"

result* get_table_names(MYSQL *db, char *db_name);

#endif
