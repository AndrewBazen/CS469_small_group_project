/**
 * @file server.c
 * @authors ** Andrew Bazen, Ben Merritt, Austen Jarosz **
 * @brief  Main server file that exposes an API to the REPL and queries the database
 * @version 0.2
 * @date 2024-10-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include "repository/sql.h"
#include "connector.h"
#include "utilities.h"
#include "request.h"
#include <string.h>
#include <mysql/mysql.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/bio.h>
#include <pthread.h>
#include <fcntl.h>

#define MAX_QUERY_SIZE 1024
#define BUFFER_SIZE 256
#define PORT 8443
#define CERTIFICATE_FILE  "cert.pem"
#define KEY_FILE          "key.pem"

/**
 * @brief Create a socket object
 * 
 * @param port 
 * @return int 
 */
int create_socket(unsigned int port) {
	int s;
	int flag;

	struct sockaddr_in serv_addr = {
		.sin_family = AF_INET,
		.sin_addr.s_addr = htonl(INADDR_ANY),
		.sin_port = htons(port)
	};

	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s < 0) {
		fprintf(stderr, "Server: Unable to create Socket: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));

	if (bind(s, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		fprintf(stderr, "Server: Unable to bind Socket: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (listen(s, 1) < 0) {
		fprintf(stderr, "Server: Unable to listen on Socket: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	printf("Server: Listening on port %d\n", port);

	return s;
}

/**
 * @brief Initialize the OpenSSL library
 * 
 */
void init_openssl() {
	SSL_load_error_strings();
	SSL_library_init();
	OpenSSL_add_ssl_algorithms();
}

/**
 * @brief Cleanup the OpenSSL library
 * 
 */
void cleanup_openssl() {
	EVP_cleanup();
}

/**
 * @brief Create a new SSL context
 * 
 * @return SSL_CTX*
 */
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

/**
 * @brief Configure the SSL context
 * 
 * @param ctx the SSL context
 */
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
	if (!SSL_CTX_check_private_key(ctx)) 
		abort();
  
}

int connect_to_db() {
	char *user = getenv("MYSQL_USER"),
		*password = getenv("MYSQL_PASSWORD"),
		*database = getenv("MYSQL_DATABASE");
	MYSQL *db = db_connect("mysql", user, password, database, 3306);

	if (!db) {
		printf("Server: Unable to connect to database\n");
		return -1;
	}
	printf("Server: Connected to database\n");
	return 0;
}

/**
 * @brief Handle the request from the client
 * 
 * @param ssl the SSL object
 * @param user the username
 * @param password the password
 * @param database the database
 */
void handle_request(SSL *ssl) {
	char buffer[BUFFER_SIZE];
	bool exit = false;
	int nbytes_read;
	MYSQL *db = NULL;

	while(!exit) {
		// Read the HTTP GET message from the client
		// and send the HTTP response
		bzero(buffer, BUFFER_SIZE);
		nbytes_read = read_from_ssl(ssl, buffer, BUFFER_SIZE, "Server");
		if (nbytes_read < 0) {
			fprintf(stderr, "Server: Unable to read from socket: %s\n", strerror(errno));
			return;
		}
		struct Request req = process_request(buffer, ssl);
		printf("Server: Received request: %s\n", req.content);

		if (req.operation[0] != '\0') {
        printf("Server: Processing request from client (%s %s)\n", req.operation, req.content);
      	}
		
		// exit if the user requests to
		if (strcmp(req.operation, "exit") == 0) {
			exit = true;
		} else {
			// Execute the query
		}
	}
	printf("Server: disconnecting from database\n");
	db_close(db);
}

/**
 * @brief Main function that connects to the database and queries the database
 * 
 * @param argc - number of arguments
 * @param argv - arguments
 * @param envp - environment variables
 * @return int 
 */
int main(int argc, char **argv) {
	struct sockaddr_in addr;
	uint len = sizeof(addr),
		port;
	SSL_CTX *ctx;
	SSL *ssl;
	MYSQL *db;
	int clientsd;
	int listensd;
	int fd;
	int rcount;
	char buffer[BUFFER_SIZE];

	setvbuf(stdout, NULL, _IONBF, 0);  // Disable stdout buffering
	
	// initialize and create/configure SSL context
	init_openssl();
	ctx = create_context();
	configure_context(ctx);

	// create a socket and listen for incoming connections
	listensd = create_socket(PORT);

	// connect to the database
	if (connect_to_db() < 0) {
		return EXIT_FAILURE;
	}

	while (true) {
		clientsd = accept(listensd, (struct sockaddr *)&addr, &len);
		if (clientsd < 0) {
		fprintf(stderr, "Server: Unable to accept connection: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
		}

		printf("Server: Established TCP connection with client (%s) on port %u\n",
				inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

		ssl = SSL_new(ctx);
		SSL_set_fd(ssl, clientsd);

		if (SSL_accept(ssl) <= 0) {
			ERR_print_errors_fp(stderr);
		} else {
			// Read the HTTP GET message from the client and send the HTTP response

			handle_request(ssl);
		}
		// Clean up the SSL data structures created for this client
		SSL_free(ssl);
		close(clientsd);
	}
	close(listensd);
	SSL_CTX_free(ctx);
	cleanup_openssl();
	return EXIT_SUCCESS;
}
