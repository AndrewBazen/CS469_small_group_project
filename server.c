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
#include <mysql/mysql.h>
#include "./connector.h"
#include <netinet/in.h>

#define MAX_QUERY_SIZE 1024
#define BUFFER_SIZE 1024

int create_socket(int port) {
  int sockfd;
  struct sockaddr_in serv_addr;

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("Socket creation failed");
    exit(EXIT_FAILURE);
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(port);

  if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    perror("Socket bind failed");
    exit(EXIT_FAILURE);
  }

  if (listen(sockfd, 5) < 0) {
    perror("Socket listen failed");
    exit(EXIT_FAILURE);
  }

  return sockfd;
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
  char *user = getenv("MYSQL_USER"),
       *password = getenv("MYSQL_PASSWORD"),
       *database = getenv("MYSQL_DATABASE");
  MYSQL *db;

  db = db_connect("127.0.0.1", "root", "root", "dogs", 3306);

  if (!db) {
    db_close(db);
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}