/******************************************************************************
/ PROGRAM:  sql.h
/ SYNOPSIS: A collection of predefined MySQL queries
******************************************************************************/

#ifndef SQL_H

#define SQL_H

#define SQL_CHECK                     "SELECT 1+1;"
#define SQL_TABLE_NAMES_FROM_DB       "SELECT table_name FROM information_schema.tables WHERE table_schema = ?;"
#define SQL_COLUMN_NAMES_FROM_TABLE   "SELECT column_name, column_type, data_type, character_maximum_length FROM information_schema.columns WHERE table_schema = ? AND table_name = ?;"
#define SQL_SELECT_ALL_FROM_TABLE     "SELECT * FROM %s;"
#define SQL_SELECT_FROM_TABLE         "SELECT %s FROM %s;"
#define SQL_INSERT                    "INSERT INTO %s (%s) VALUES (%s);"
#define SQL_DELETE                    "DELETE FROM %s WHERE %s = ?;"
#define SQL_UPDATE                    "UPDATE %s SET %s = ? WHERE %s = ?;"

#endif