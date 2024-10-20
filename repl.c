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

#define CERTIFICATE_FILE "cert.pem"
#define KEY_FILE "key.pem"
#define DEFAULT_PORT 8443
#define MAX_INPUT_SIZE 256

SSL_CTX * ctx;
SSL * ssl;
int server_socket;

void send_request(const char * input); //forward decl to resolve warnings
void close_connection();
void help_command();

void repl(int port) {
  char input[MAX_INPUT_SIZE];
  int complete = 0;

  while (!complete) {
    printf("DATABASE QUERIES> ");
    if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL) {
      printf("Error reading input\n");
      continue;
    }

    //Remove trailing newline character from input
    size_t length = strlen(input);
    if (length > 0 && input[length - 1] == '\n') {
      input[length - 1] = '\0';
    }

    //use "exit" to quit repl
    if (strcmp(input, "exit") == 0) {
      printf("Goodbye\n");
      complete = 1;
    } else if (strcmp(input, "help") == 0) { //use "help" to get a list of commands
      help_command();
      continue;
    }
    //process MySQL inputs
    send_request(input);
  }
  //close conection when user exits
  close_connection();
}

void send_request(const char * input) {
  int valread;
  char buffer[1024];

  memset(buffer, 0, sizeof(buffer)); //clear memory (set to 0)

  //send request to server
  SSL_write(ssl, input, strlen(input));

  //receive request from server
  valread = SSL_read(ssl, buffer, sizeof(buffer) - 1);
  if (valread > 0) {
    buffer[valread] = '\0';
    printf("Server's response: %s\n", buffer);
  } else {
    printf("ERROR: Did not read response.");
  }
}

int init_connection(int port) {
  struct sockaddr_in address;

  server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket < 0) {
    perror("Socket could not be created");
    return -1;
  }

  address.sin_family = AF_INET;
  //same port as server
  address.sin_port = htons(port);

  if (inet_pton(AF_INET, "127.0.0.1", & address.sin_addr) <= 0) {
    printf("Invalid address\n");
    return -1;
  }

  //connect to the server
  if (connect(server_socket, (struct sockaddr * ) & address, sizeof(address)) < 0) {
    printf("Connection Failed\n");
    return -1;
  }

  //setup  SSL
  SSL_library_init();
  ctx = SSL_CTX_new(TLS_client_method());
  ssl = SSL_new(ctx);
  SSL_set_fd(ssl, server_socket);

  if (SSL_connect(ssl) < 0) {
    ERR_print_errors_fp(stderr);
    return -1;
  }

  return 0;
}
//function to close connection.
void close_connection() {
  SSL_shutdown(ssl);
  SSL_free(ssl);
  SSL_CTX_free(ctx);
  close(server_socket);
}
//function to show usage
void help_command() {
  printf("Available Commands:\n");
  printf("  help: returns list of available commands\n");
  printf("  <SQL COMMAND>: can insert an SQL command to query to the server\n");
  printf("  exit: exits the REPL\n");
}

int main(int argc, char ** argv, char ** envp) {
  int port = DEFAULT_PORT;
  //start conection at the beginning of main
  if (init_connection(port) != 0) {
    return EXIT_FAILURE;
  }
  //start repl
  repl(port);
  return EXIT_SUCCESS;
}