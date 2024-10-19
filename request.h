#ifndef REQUEST_H
#define REQUEST_H


#include <openssl/ssl.h>
#include <string.h>

#define OPERATION_SIZE    10
#define ARGUMENT_SIZE     32
#define BUFFER_SIZE     256
#define CONTENT_SIZE    256
#define VALUES_SIZE     10

// Struct definition
typedef struct Request {
  char  operation[OPERATION_SIZE],
        type[ARGUMENT_SIZE],
        db_name[ARGUMENT_SIZE],
        table_name[ARGUMENT_SIZE],
        field_name[ARGUMENT_SIZE],
        field_value[ARGUMENT_SIZE],
        set_field[ARGUMENT_SIZE],
        set_value[ARGUMENT_SIZE],
        where_field[ARGUMENT_SIZE],
        where_value[ARGUMENT_SIZE],
        *values[VALUES_SIZE],
        *cols[VALUES_SIZE];
} Request;

#include "utilities.h"

// Function prototypes
Request* process_request(Request *req, char *message, SSL *ssl);

#endif // REQUEST_H
