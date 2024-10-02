/******************************************************************************
/ PROGRAM:  sql.h
/ SYNOPSIS: A collection of predefined MySQL queries
******************************************************************************/

#ifndef SQL_H

#define SQL_H

#define SQL_CHECK                     "SELECT 1+1;"
#define SQL_TABLE_NAMES_FROM_DB       "SELECT table_name FROM information_schema.tables WHERE table_schema = ?;"
#define SQL_COLUMN_NAMES_FROM_TABLE   "SELECT column_name FROM information_schema.columns WHERE table_schema = ? AND table_name = ?;"

#endif