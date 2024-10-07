#ifndef REQUEST_H
#define REQUEST_H

#include "utilities.h"
#include <openssl/ssl.h>

#define OPERATION_SIZE    10
#define BUFFER_SIZE     256
#define CONTENT_SIZE    256

// Struct definition
typedef struct Request {
  char operation[OPERATION_SIZE];
  char content[CONTENT_SIZE];
} Request;

// Function prototypes
Request process_request(char *message, SSL *ssl);

#endif // REQUEST_H
