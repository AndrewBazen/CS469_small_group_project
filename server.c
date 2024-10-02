/**
 * @file server.c
 * @authors ** Andrew Bazen, Ben Merritt, Austen Jarosz **
 * @brief  Main server file that exposes an API to the REPL and queries the database
 * @version 0.1
 * @date 2024-09-30
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "connector.c"
#include <string.h>  // For strlen and strcmp functions
#include <mysql/mysql.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define MAX_INPUT_SIZE 256
#define MAX_QUERY_SIZE 1024
#define BUFFER_SIZE 1024
#define PORT 4433
#define CERTIFICATE_FILE  "cert.pem"
#define KEY_FILE          "key.pem"


void repl(MYSQL *db) {
    char input[MAX_INPUT_SIZE];
    while (1) {
        printf("DATABASE QUERIES> ");
        if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL) {
            break;
        }

        //Remove trailing newline character from input
        size_t length = strlen(input);
        if (length > 0 && input[length - 1] == '\n') {
            input[length - 1] = '\0';
        }

        //use "exit" to quit repl
        if (strcmp(input, "exit") == 0) {
            printf("Goodbye\n");
            break;
        }

        // Placeholder: Later, process commands and interact with the database
        if (mysql_query(db, input)) {
            fprintf(stderr, "MySQL query failed: %s\n", mysql_error(db));
        } else {
            MYSQL_RES *result = mysql_store_result(db);
            if (result) {
                MYSQL_ROW row;
                while ((row = mysql_fetch_row(result))) {
                    for (int i = 0; i < mysql_num_fields(result); i++) {
                        printf("%s\t", row[i] ? row[i] : "NULL");
                    }
                    printf("\n");
                }
                mysql_free_result(result);
            } else {
                if (mysql_field_count(db) == 0) {
                    printf("%lu rows affected\n", (unsigned long)mysql_affected_rows(db));
                } else {
                    fprintf(stderr, "Could not retrieve result set: %s\n", mysql_error(db));
                }
            }
        }
    }
}

int create_socket(unsigned int port) {
  int sockfd;
  struct sockaddr_in serv_addr;

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    fprintf(stderr, "Server: Unable to create Socket: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);
  serv_addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    fprintf(stderr, "Server: Unable to bind Socket: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  if (listen(sockfd, 5) < 0) {
    fprintf(stderr, "Server: Unable to listen on Socket: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  printf("Server: Listening on port %d\n", port);

  return sockfd;
}

void init_openssl() {
  SSL_load_error_strings();
  OpenSSL_add_ssl_algorithms();
}

void cleanup_openssl() {
  EVP_cleanup();
}

SSL_CTX *create_context() {
  const SSL_METHOD *method;
  SSL_CTX *ctx;

  method = SSLv23_server_method();
  ctx = SSL_CTX_new(method);

  if (!ctx) {
    perror("Server: Unable to create SSL context:\n");
    ERR_print_errors_fp(stderr);
    exit(EXIT_FAILURE);
  }

  return ctx;
}

void configure_context(SSL_CTX *ctx) {
  SSL_CTX_set_ecdh_auto(ctx, 1);

  if (SSL_CTX_use_certificate_file(ctx, CERTIFICATE_FILE, SSL_FILETYPE_PEM) <= 0) {
    fprintf(stderr, "Server: cannot set certificate:\n");
    ERR_print_errors_fp(stderr);
    exit(EXIT_FAILURE);
  }

  if (SSL_CTX_use_PrivateKey_file(ctx, KEY_FILE, SSL_FILETYPE_PEM) <= 0) {
    fprintf(stderr, "Server: cannot set private key:\n");
    ERR_print_errors_fp(stderr);
    exit(EXIT_FAILURE);
  }
}



/**
 * @brief Main function that connects to the database and queries the database
 * 
 * @param argc 
 * @param argv 
 * @param envp 
 * @return int 
 */
int main(int argc, char **argv, char **envp) {
  SSL_CTX *ctx;
  MYSQL *db;
  unsigned int sockfd;
  char *user = getenv("MYSQL_USER"),
       *password = getenv("MYSQL_PASSWORD"),
       *database = getenv("MYSQL_DATABASE"),
       buffer[BUFFER_SIZE];

  // initialize and create/configure SSL context
  init_openssl();
  ctx = create_context();
  configure_context(ctx);

  // connect to the database
  db = db_connect("mysql", user, password, database, 3306);

  // check if the connection was successful
  if (!db) {
    db_close(db);
    printf("Server: Unable to connect to database\n");
    return EXIT_FAILURE;
  }
  printf("Server: Connected to database\n");

  // create a socket and listen for incoming connections
  sockfd = create_socket(PORT);

  while (1) {
    int client_sockfd;
    struct sockaddr_in addr;
    unsigned int addr_len = sizeof(addr);
    char client_addr[INET_ADDRSTRLEN];

    client_sockfd = accept(sockfd, (struct sockaddr *)&addr, &addr_len);

    if (client_sockfd < 0) {
      fprintf(stderr, "Server: Unable to accept connection: %s\n", strerror(errno));
      exit(EXIT_FAILURE);
    }

    inet_ntop(AF_INET, (struct in_addr*)&addr.sin_addr, client_addr,
	      INET_ADDRSTRLEN);
    printf("Server: Established TCP connection with client (%s) on port %u\n",
	  client_addr, PORT);

    SSL *ssl = SSL_new(ctx);
    SSL_set_fd(ssl, client_sockfd);

    if (SSL_accept(ssl) <= 0) {
      ERR_print_errors_fp(stderr);
    } else {
      SSL_read(ssl, buffer, BUFFER_SIZE);
      printf("Server: Received message: %s\n", buffer);
      SSL_write(ssl, buffer, BUFFER_SIZE);
    }

    SSL_free(ssl);
    close(client_sockfd);
  }



  printf("Welcome to the database REPL. Type 'exit' to quit.\n");
    repl(db);

  db_close(db);

  return EXIT_SUCCESS;
}
