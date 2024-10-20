/**
 * @file utilities.c
 * @author ** Andrew Bazen **
 * @date ** 22 September 2024 **
 * @brief This program contains utility functions that are used by both the server
 *       and client programs
*/
#include "utilities.h"

Request* check_args(Request *req, int arg_number, SSL *ssl) {
    char error_buffer[BUFFER_SIZE];

    if (strcmp(req->operation, "insert") == 0) {
        if (arg_number < 4) {
            sprintf(error_buffer, "error_id-%d: too few arguments", 1);
        } else if (arg_number > 4) {
            sprintf(error_buffer, "error_id-%d: too many arguments", 2);
        } else {
            return req;
        }
    } else if (strcmp(req->operation, "select") == 0) {
        if (arg_number < 2) {
            sprintf(error_buffer, "error_id-%d: too few arguments", 1);
        } else if (arg_number > 2) {
            sprintf(error_buffer, "error_id-%d: too many arguments", 2);
        } else {
            return req;
        }
    } else if (strcmp(req->operation, "delete") == 0) {
        if (arg_number < 3) {
            sprintf(error_buffer, "error_id-%d: too few arguments", 1);
        } else if (arg_number > 3) {
            sprintf(error_buffer, "error_id-%d: too many arguments", 2);
        } else {
            return req;
        }
    } else if (strcmp(req->operation, "update") == 0) {
        if (arg_number < 5) {
            sprintf(error_buffer, "error_id-%d: too few arguments", 1);
        } else if (arg_number > 5) {
            sprintf(error_buffer, "error_id-%d: too many arguments", 2);
        } else {
            return req;
        }
    } else if (strcmp(req->operation, "get") == 0) {
        if (strcmp(req->type, "tables") == 0) {
            if (arg_number < 1) {
                sprintf(error_buffer, "error_id-%d: too few arguments", 1);
            } else if (arg_number > 1) {
                sprintf(error_buffer, "error_id-%d: too many arguments", 2);
            } else {
                return req;
            }
        } else if (strcmp(req->type, "columns") == 0) {
            if (arg_number < 2) {
                sprintf(error_buffer, "error_id-%d: too few arguments", 1);
            } else if (arg_number > 2) {
                sprintf(error_buffer, "error_id-%d: too many arguments", 2);
            } else {
                return req;
            }
        }
    }
    write_to_ssl(ssl, error_buffer, strlen(error_buffer), "Server");
    req->operation[0] = '\0';
    return req;
}

Request* get_request(Request *req, char *contents, SSL *ssl) {
    printf("Get request\n");
    printf("Contents: %s\n", contents);
    char    arguments[BUFFER_SIZE],
            type[BUFFER_SIZE],
            dummy[BUFFER_SIZE];
    int arg_number;
    int offset = 0;

    sscanf(contents, "%s %n", req->type, &offset);

    strncpy(arguments, contents + offset, BUFFER_SIZE - 1);
    arguments[BUFFER_SIZE - 1] = '\0';

    if (strcmp(req->type, "tables") == 0) {
        arg_number = sscanf(arguments, "%s %s", req->db_name, dummy);

        req = check_args(req, arg_number, ssl);
    } else if (strcmp(req->type, "columns") == 0) {
        arg_number = sscanf(arguments, "%s %s %s", req->db_name, req->table_name, dummy);

        req = check_args(req, arg_number, ssl);
    }
    return req;
}

Request* insert_request(Request *req, char *contents, SSL *ssl) {
    char    dummy[BUFFER_SIZE],
            arguments[BUFFER_SIZE],
            columns[BUFFER_SIZE];
    char*   value = NULL;
    int arg_number;
    int value_number = 0;
    bool parsing_complete = false;

    arg_number = sscanf(contents, "%s %s %s %s %s", req->db_name, req->table_name, columns, arguments, dummy);

    value = strtok(arguments, ",");
    while (value != NULL) {
        req->values[value_number] = strdup(value);
        printf("Value %d: %s\n", value_number, req->values[value_number]);
        value = strtok(NULL, ",");
        value_number++;
    }

    value = strtok(columns, ",");
    value_number = 0;
    while (value != NULL) {
        req->cols[value_number] = strdup(value);
        printf("Column %d: %s\n", value_number, req->cols[value_number]);
        value = strtok(NULL, ",");
        value_number++;
    }
    req = check_args(req, arg_number, ssl);
    return req;
}

Request* select_request(Request *req, char *contents, SSL *ssl) {
    printf("Select request\n");
    char dummy[BUFFER_SIZE];
    int arg_number; 

    arg_number = sscanf(contents, "%s %s %s", req->db_name, req->table_name, dummy);
    req = check_args(req, arg_number, ssl);
    return req;
}

Request* delete_request(Request *req, char *contents, SSL *ssl) {
    char dummy[BUFFER_SIZE];
    int arg_number;

    arg_number = sscanf(contents, "%s %s %s %s", req->table_name, req->field_name,
        req->field_value, dummy);
    req = check_args(req, arg_number, ssl);
    return req;
}

Request* update_request(Request *req, char *contents, SSL *ssl) {
    char  dummy[BUFFER_SIZE];
    int arg_number;

    arg_number = sscanf(contents, "%s %s %s %s %s %s", req->table_name, req->set_field,
        req->set_value, req->where_field, req->where_value, dummy);
    req = check_args(req, arg_number, ssl);
    return req;
}

void add_columns_to_buffer(MYSQL *db, Request *req, char *buffer, result *res, int num_cols) {
    if (res == NULL) {
			printf("Server: Error getting columns\n");
			return;
		}
    num_cols = res->num_rows;
    char *buffer_ptr = buffer;
    int i = 0;

    while (res != NULL) {
        i = 0;
        buffer_ptr += sprintf(buffer_ptr, "+-----------------");
        for (int i = 0; i < num_cols - 1; i++) {
            buffer_ptr += sprintf(buffer_ptr, "+-----------------");
        }
        buffer_ptr += sprintf(buffer_ptr, "+\n|");
        
        // add the column names
        i = 0;
        if (num_cols % 2 == 0) {
            while (res != NULL) {
                if (i % (num_cols + 2) == 0) {
                    buffer_ptr += sprintf(buffer_ptr, " %-15s |", *res->buffer);
                }
                res = res->next;
                i++;
            }
        } else {
            while (res != NULL) {
                if (i % (num_cols + 1) == 0) {
                    buffer_ptr += sprintf(buffer_ptr, " %-15s |", *res->buffer);
                }
                res = res->next;
                i++;
            }
        }	

        // add the bottom of the column headers
        buffer_ptr += sprintf(buffer_ptr,"\n+-----------------");
        for (int i = 0; i < num_cols - 1; i++) {
            buffer_ptr += sprintf(buffer_ptr, "+-----------------");
        }
        buffer_ptr += sprintf(buffer_ptr, "+\n");
    }
}

void add_columns_to_insert_buffer(MYSQL *db, Request *req, char *buffer, result *res, int num_cols) {
    if (res == NULL) {
			printf("Server: Error getting columns\n");
			return;
		}
    char *buffer_ptr = buffer;
    num_cols = res->num_rows;
    int i = 0;

    // add the top of the table
    for (int i = 0; i < num_cols - 1; i++) {
        buffer_ptr += sprintf(buffer_ptr, "+-----------------");
    }
    buffer_ptr += sprintf(buffer_ptr, "+\n|");
    
    // add the column names
    i = 0;
    while (i < num_cols) {
        printf("i: %d\n", i);
        printf("res->buffer: %s\n",req->cols[i]);
        buffer_ptr += sprintf(buffer_ptr, " %-15s |", req->cols[i]);
        i++;
    }

    // add the bottom of the column headers
    buffer_ptr += sprintf(buffer_ptr,"\n+-----------------");
    for (int i = 0; i < num_cols - 1; i++) {
        buffer_ptr += sprintf(buffer_ptr, "+-----------------");
    }
    buffer_ptr += sprintf(buffer_ptr, "+\n");
}

void add_rows_to_buffer(char *buffer, result *res, int num_cols) {
    if (res == NULL) {
			printf("Server: Error getting rows\n");
			return;
		}
    char *buffer_ptr = buffer;
    int i = 0;

    // add the rows
    while (res != NULL) {
        i = 0;
        buffer_ptr += sprintf(buffer_ptr, "|");
        while (i < num_cols) {
            if (res->end_of_row) {
                buffer_ptr += sprintf(buffer_ptr, " %-15s |\n", *res->buffer);
                res = res->next;	
            } else {
                buffer_ptr += sprintf(buffer_ptr, " %-15s |", *res->buffer);
                res = res->next;
            }
            i++;
        }
    }

    // add the bottom of the table
		buffer_ptr += sprintf(buffer_ptr,"+-----------------");
		for (int i = 0; i < num_cols - 1; i++) {
			buffer_ptr += sprintf(buffer_ptr, "+-----------------");
		}
		buffer_ptr += sprintf(buffer_ptr, "+\n");
}

/**
 * @brief - This function reads a message from  an SSL object and returns the
 *         number of bytes read
 * 
 * @param ssl - SSL object
 * @param buffer - buffer to store the message
 * @param size - size of the buffer
 * @param type - type of the object reading from the SSL object
 * @return int - number of bytes read
 */
int read_from_ssl(SSL *ssl, char* buffer, int size, char* type) {
    int nbytes_read = SSL_read(ssl, buffer, size);

    // determine if server or client is reading
    if (strcmp(type, "Server") == 0) {
        // If nbytes_read is less than zero, an error occurred.  Print an error 
        // message and exit the program
        if (nbytes_read < 0) {
            fprintf(stderr, "Server: Error reading from socket: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        
        if (nbytes_read != 0) {
            printf("Server: request received from client: \"%s\"\n", buffer);
            return nbytes_read;
        }
        printf("Server: Client closed connection\n");
    } 
    return nbytes_read;
}


/**
 * @brief - This function writes a message to an SSL object and returns the number
 *        of bytes written
 * 
 * @param ssl - SSL object
 * @param buffer - buffer to store the message
 * @param size - size of the buffer
 * @param type - type of the object writing to the SSL object
 * @return int - number of bytes written
 */
void write_to_ssl(SSL *ssl, char* buffer, int size, char* type) {

    int nbytes_written = SSL_write(ssl, buffer, size);

    if (strcmp(type, "Server") == 0) {
        if (strcmp(buffer, "exit") == 0) {
            printf("Server: Client Disconnected from server\n");
            free(ssl);
        }
        // If nbytes_written is less than zero, an error occurred.  Print an error 
        // message and exit the program
        else if (nbytes_written < 0) {
            fprintf(stderr, "Server: Error writing to socket: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        // If nbytes_written is zero, the client closed the connection.  Prints a 
        // message and then closes the socket descriptor, continuing to the next
        // iteration
        else if (nbytes_written == 0) {
            printf("Server: Client closed connection\n");
        }
    } 
}