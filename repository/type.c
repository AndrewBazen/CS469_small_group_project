#include <string.h>
#include <stdio.h>
#include "type.h"

int convert_type(char *type) {
  if (strcmp(type, "tinyint") == 0) {
    return MYSQL_TYPE_TINY;
  } else if (strcmp(type, "smallint") == 0) {
    return MYSQL_TYPE_INT24;
  } else if (strcmp(type, "mediumint") == 0) {
    return MYSQL_TYPE_INT24;
  } else if (strcmp(type, "int") == 0) {
    return MYSQL_TYPE_INT24;
  } else if (strcmp(type, "bigint") == 0) {
    return MYSQL_TYPE_INT24;
  } else if (strcmp(type, "float") == 0) {
    return MYSQL_TYPE_FLOAT;
  } else if (strcmp(type, "double") == 0) {
    return MYSQL_TYPE_DOUBLE;
  } else if (strcmp(type, "decimal") == 0) {
    return MYSQL_TYPE_DECIMAL;
  } else if (strcmp(type, "year") == 0) {
    return MYSQL_TYPE_YEAR;
  } else if (strcmp(type, "time") == 0) {
    return MYSQL_TYPE_TIME;
  } else if (strcmp(type, "date") == 0) {
    return MYSQL_TYPE_DATE;
  } else if (strcmp(type, "datetime") == 0) {
    return MYSQL_TYPE_DATETIME2;
  } else if (strcmp(type, "timestamp") == 0) {
    return MYSQL_TYPE_TIMESTAMP;
  } else if (strcmp(type, "char") == 0) {
    return MYSQL_TYPE_VARCHAR;
  } else if (strcmp(type, "varchar") == 0) {
    return MYSQL_TYPE_VARCHAR;
  } else if (strcmp(type, "blob") == 0) {
    return MYSQL_TYPE_BLOB;
  } else if (strcmp(type, "bit") == 0) {
    return MYSQL_TYPE_BIT;
  } else {
    return -1;
  }
}