/**
 * @file utilities.c
 * @author ** Andrew Bazen **
 * @date ** 22 September 2024 **
 * @brief This program contains utility functions that are used by both the server
 *       and client programs
*/
#include "utilities.h"

void seed_database() {
    // Define the command to execute the script
    const char *create = "docker exec -i mysql chmod +x /Docker/seed.sh";
    const char *seed = "docker exec -i mysql /Docker/seed.sh";
    
    
    // Execute the command
    int result = system(create);
    if (result == -1) {
        perror("Error changing permissions on MySQL container");
    } else {
        printf("Permissions changed successfully on MySQL container\n");
    }

    result = system(seed);
    
    if (result == -1) {
        perror("Error executing script on MySQL container");
    } else {
        printf("Script executed successfully on MySQL container\n");
    }
}

Request* check_args(Request *req, int arg_number, SSL *ssl) {
    char error_buffer[BUFFER_SIZE];

    if (strcmp(req->operation, "insert") == 0) {
        if (arg_number < 3) {
            sprintf(error_buffer, "error_id-%d: too few arguments", 1);
        } else if (arg_number > 3) {
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
            arguments[BUFFER_SIZE];
    char*   value = NULL;
    int arg_number;
    int value_number = 0;
    bool parsing_complete = false;

    arg_number = sscanf(contents, "%s %s %s %s", req->db_name, req->table_name, arguments, dummy);

    value = strtok(arguments, ",");
    while (value != NULL) {
        req->values[value_number] = strdup(value);
        printf("Value %d: %s\n", value_number, req->values[value_number]);
        value = strtok(NULL, ",");
        value_number++;
    }
    

    //parse the values in contents
    // for each value in contents
    // while (!parsing_complete) {
    //     if (value_number == 0) {
    //         value = strtok(arguments, ",");
    //         req->values[value_number] = value;
    //     } else if (value == NULL) {
    //         parsing_complete = true;
    //     } else {
    //         req->values[value_number] = value;
    //         value = strtok(NULL, ",");
    //     }
    //     printf("value: %s\n", value);  
    //     printf("Value: %s\n", req->values[value_number]);
    //     value_number++;
    // }
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
        free(ssl);
    } else if (strcmp(type, "Client") == 0) {
        // If nbytes_read is less than zero, an error occurred.  Print an error 
        // message and exit the program
        if (nbytes_read < 0) {
        fprintf(stderr, "Client: Error reading from socket: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
        }

        // If nbytes_read is zero, the client closed the connection.  Prints a message
        // and then closes the socket descriptor, continuing to the next iteration
        if (nbytes_read == 0) {
        printf("Client: Server closed connection\n");
        }
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
        free(ssl);
        exit(EXIT_FAILURE);
        }

    } else if (strcmp(type, "Client") == 0) {
        // If nbytes_written is less than zero, an error occurred.  Print an error 
        // message and exit the program
        if (nbytes_written < 0) {
        fprintf(stderr, "Client: Error writing to socket: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
        }

        // If nbytes_written is zero, the server closed the connection.  Prints a
        // message and then closes frees the ssl object, then exits the program
        if (nbytes_written == 0) {
        printf("Client: Server closed connection\n");
        free(ssl);
        exit(EXIT_FAILURE);
        }
    }
}