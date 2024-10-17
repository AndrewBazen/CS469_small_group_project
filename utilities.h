#ifndef UTILITIES_H
#define UTILITIES_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
#include <fcntl.h>
#include "request.h"
#include <openssl/ssl.h>
#include <sys/types.h>
#include <dirent.h>

#define BUFFER_SIZE 256

// Function prototypes
Request get_request(Request req, char *message, SSL *ssl);
Request insert_request(Request req, char *message, SSL *ssl);
Request select_request(Request req, char *message, SSL *ssl);
Request delete_request(Request req, char *message, SSL *ssl);
Request update_request(Request req, char *message, SSL *ssl);
int read_from_ssl(SSL *ssl, char *buffer, int size, char *error);
void write_to_ssl(SSL *ssl, char *buffer, int size, char *error);

#endif // UTILITIES_H
