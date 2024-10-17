#ifndef REQUEST_H
#define REQUEST_H

#include "utilities.h"
#include <openssl/ssl.h>
#include <string.h>

#define OPERATION_SIZE    10
#define ARGUMENT_SIZE     32
#define BUFFER_SIZE     256
#define CONTENT_SIZE    256
#define VALUES_SIZE     12

// Struct definition
typedef struct Request {
  char  operation[OPERATION_SIZE],
        db_name[ARGUMENT_SIZE],
        table_name[ARGUMENT_SIZE],
        field_name[ARGUMENT_SIZE],
        field_value[ARGUMENT_SIZE],
        set_field[ARGUMENT_SIZE],
        set_value[ARGUMENT_SIZE],
        where_field[ARGUMENT_SIZE],
        where_value[ARGUMENT_SIZE],
        values[VALUES_SIZE][ARGUMENT_SIZE];
} Request;

// Function prototypes
Request process_request(char *message, SSL *ssl);

#endif // REQUEST_H
