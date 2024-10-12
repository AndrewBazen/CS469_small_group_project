#ifndef REPO_STATEMENT_H
#define REPO_STATEMENT_H

#include <mysql/mysql.h>

MYSQL_STMT* create_prepared_statement(MYSQL *db, char *statement);

#endif
