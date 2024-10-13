#ifndef REPO_SELECT_H
#define REPO_SELECT_H

#include <mysql/mysql.h>
#include "result.h"

result* select_all(MYSQL *db, char *db_name, char *table_name);

#endif
