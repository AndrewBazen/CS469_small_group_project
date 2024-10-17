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
#include <signal.h>
#include "repository/sql.h"
#include "repository/result.h"
#include "repository/columns.h"
#include "repository/delete.h"
#include "repository/insert.h"
#include "repository/select.h"
#include "repository/update.h"
#include "repository/table_name.h"
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

volatile bool server_shutdown = false;

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

MYSQL* connect_to_db() {
	char *user = getenv("MYSQL_USER"),
		*password = getenv("MYSQL_PASSWORD"),
		*database = getenv("MYSQL_DATABASE");
	MYSQL *db = db_connect("mysql", user, password, database, 3306);

	if (!db) {
		printf("Server: Unable to connect to database\n");
		exit(EXIT_FAILURE);
	}
	printf("Server: Connected to database\n");
	return db;
}

/**
 * @brief Query the database
 * 
 * @param ssl the SSL object
 * @param db the database
 * @param req the request
 */
result* query_database(SSL *ssl, MYSQL *db, Request *req, result *res) {
	printf("Server: Querying database\n");
	
	if (strcmp(req->operation, "tables") == 0) {
		res = get_table_names(db, req->db_name);
	} else if (strcmp(req->operation, "columns") == 0) {
		res = get_columns(db, req->db_name, req->table_name);
	} else if (strcmp(req->operation, "insert") == 0) {
		res = insert(db, req->db_name, req->table_name, (char **)req->values);
	} else if (strcmp(req->operation, "select") == 0) {
		res = select_all(db, req->db_name, req->table_name);
	} else if (strcmp(req->operation, "delete") == 0) {
		res = delete_from(db, req->table_name, req->field_name, req->field_value);
	} else if (strcmp(req->operation, "update") == 0) {
		res = update(db, req->table_name, req->set_field, req->set_value,
		req->where_field, req->where_value);
	}
	return res;
}

/**
 * @brief Handle the request from the client
 * 
 * @param ssl the SSL object
 * @param user the username
 * @param password the password
 * @param database the database
 */
void handle_request(SSL *ssl, MYSQL *db) {
	char buffer[BUFFER_SIZE];
	char response[BUFFER_SIZE];
	Request *req = malloc(sizeof(Request));
	result *res = malloc(sizeof(result));
	int nbytes_read;
	bool exit = false;

	while(!server_shutdown && !exit) {
		// Read the HTTP GET message from the client
		// and send the HTTP response
		bzero(buffer, BUFFER_SIZE);
		nbytes_read = read_from_ssl(ssl, buffer, BUFFER_SIZE, "Server");
		if (nbytes_read < 0) {
			fprintf(stderr, "Server: Unable to read from socket: %s\n", strerror(errno));
			return;
		}

		req = process_request(req, buffer, ssl);
		if (!req) {
            fprintf(stderr, "Server: Failed to process request\n");
        }

		printf("req->operation: %s\n", req->operation);
		
		if (strcmp(req->operation, "exit") == 0) {
			exit = true;
	  	} else if (strcmp(req->operation, "seed-database") == 0) {
			sprintf(response, "Server: Database seeded\n");
			write_to_ssl(ssl, response, sizeof(response), "Server");
		} else if (req->operation[0] != '\0') {
			printf("Server: Handling request with database\n");
        	sprintf(response, "Server: Handling request with database\n");
			write_to_ssl(ssl, response, sizeof(response), "Server");
			res = query_database(ssl, db, req, res);
			if (res->buffer[0] != '\0') {
				printf(" res->buffer: %s\n", res->buffer);
                write_to_ssl(ssl, res->buffer, strlen(res->buffer), "Server");
            } else {
				printf("Server: Query failed\n");
                sprintf(response, "Server: Query failed\n");
                write_to_ssl(ssl, response, strlen(response), "Server");
            }
      	}
		bzero(response, BUFFER_SIZE);
		free(req);
	}
}

/**
 * @brief Handle the shutdown signal
 * 
 * @param signal the signal
 */
void handle_shutdown(int signal) {
	if (signal == SIGINT || signal == SIGTERM) {
	server_shutdown = true;
	printf("Server: Shutdown signal received. Shutting down...\n");
    }
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

	signal(SIGINT, handle_shutdown);
	signal(SIGTERM, handle_shutdown);

	setvbuf(stdout, NULL, _IONBF, 0);  // Disable stdout buffering
	
	// initialize and create/configure SSL context
	init_openssl();
	ctx = create_context();
	configure_context(ctx);

	// create a socket and listen for incoming connections
	listensd = create_socket(PORT);

	// connect to the database
	db = connect_to_db();

	while (!server_shutdown) {
		printf("Server: Listening on port %d\n", PORT);
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
			handle_request(ssl, db);
		}
		// Clean up the SSL data structures created for this client
		printf("Server: Closing connection with client\n");
		SSL_free(ssl);
		close(clientsd);
	}
	printf("Server: Disconnecting from the database\n");
	db_close(db);

	close(listensd);
	SSL_CTX_free(ctx);
	cleanup_openssl();
	printf("Server: Shutdown complete\n");
	return EXIT_SUCCESS;
}
