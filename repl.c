#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "repository/sql.h"
#include "connector.h"
#include <string.h>
#include <mysql/mysql.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define CERTIFICATE_FILE  "cert.pem"
#define KEY_FILE          "key.pem"

#define MAX_INPUT_SIZE 256

void send_request(const char * input);  //forward decl to resolve warnings

void repl() {
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

        //process MySQL inputs
        send_request(input);
    }
}

void send_request(const char * input) {
  int server_socket;
  int valread;
  SSL_CTX * ctx;
  SSL * ssl;
  struct sockaddr_in address;
  char buffer[1024];
  
  memset(buffer, 0, sizeof(buffer)); //clear memory (set to 0)

  // Create socket
  server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket < 0) {
    perror("Socket could not be created");
    return;
  }

  address.sin_family = AF_INET;
  //same port as server
  address.sin_port = htons(8443);

  if (inet_pton(AF_INET, "127.0.0.1", & address.sin_addr) <= 0) {
    printf("Invalid address\n");
    return;
  }

  //connect to the server
  if (connect(server_socket, (struct sockaddr * ) & address, sizeof(address)) < 0) {
    printf("Connection Failed\n");
    return;
  }

  //setup  SSL
  SSL_library_init();
  ctx = SSL_CTX_new(TLS_client_method());
  ssl = SSL_new(ctx);
  SSL_set_fd(ssl, server_socket);

  if (SSL_connect(ssl) < 0) {
    ERR_print_errors_fp(stderr);
  } else {

    //send request to server
    SSL_write(ssl, input, strlen(input));

    //receive request from server
    valread = SSL_read(ssl, buffer, sizeof(buffer) - 1);
    buffer[valread] = '\0';
    printf("Server's response: %s\n", buffer);

    SSL_shutdown(ssl);
  }

  SSL_free(ssl);
  SSL_CTX_free(ctx);
  close(server_socket);
}


int main(int argc, char **argv, char **envp) {
    //start repl
    repl();
    return EXIT_SUCCESS;
}