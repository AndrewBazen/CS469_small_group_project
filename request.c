/**
 * @file request.c
 * @author ** Andrew Bazen **
 * @date ** 07 Oct 2024 **
 * @brief This program processes a request from a client and returns a struct
 *       containing the request information or an error code if the request is
 *      invalid
*/
#include "request.h"

/**
 * @brief This function processes a request from a client and returns a struct
 *        containing the request information or an error code if the request is
 *        invalid
 * 
 * @param message - the message from the client
 * @param ssl - the SSL object
 * @return struct Request - the request struct
 */
Request* process_request(Request *req, char *message, SSL *ssl) {
    printf("Server: Processing request from client (%s)\n", message);
    const char *operations[] = {"get", "insert", "select", "delete", "update", "seed-database", "exit"};
    const int num_operations = sizeof(operations) / sizeof(operations[0]);
    char    contents[CONTENT_SIZE],
            error_buffer[BUFFER_SIZE],
            dummy[BUFFER_SIZE],
            exit_buffer[BUFFER_SIZE];
    int arg_number;
    int offset = 0;
    bool valid_request = false;
    Request *temp_req = req;
    Request *new_req = malloc(sizeof(Request));

    sscanf(message, "%s %n", temp_req->operation, &offset);

    printf("operation: %s\n", temp_req->operation);

    strncpy(contents, message + offset, CONTENT_SIZE - 1);
    contents[CONTENT_SIZE - 1] = '\0';

    printf("contents: %s\n", contents);

    // first check if the operation is valid
    for (int i = 0; i < num_operations; i++) {
        if (strcmp(temp_req->operation, operations[i]) == 0) {
            valid_request = true;
            printf("Server: Valid Request\n");
            break;
        }
    }
    
    // if the operation is not valid, send an error code to the client
    if (!valid_request) {
        printf("Server: Invalid Request\n");
        sprintf(error_buffer, "error_id-%d: Invalid Request", 3);
        write_to_ssl(ssl, error_buffer, strlen(error_buffer), "Server");
        temp_req->operation[0] = '\0';
        req = temp_req;
        return req;
    }

	// if the operation is download and the file does not exist, send an error code
    // to the client
    if (strcmp(temp_req->operation, "exit") == 0) {
        sprintf(exit_buffer, "Disconnecting\n");
        write_to_ssl(ssl, exit_buffer, strlen(exit_buffer), "Server");
        req = temp_req;
        return req;
    } else {
        //get
        if (strcmp(temp_req->operation, "get") == 0) {
            req = get_request(temp_req, contents, ssl);
        }
        
        //select
        if (strcmp(temp_req->operation, "select") == 0) {
            req = select_request(temp_req, contents, ssl);
        }

        //insert
        if (strcmp(temp_req->operation, "insert") == 0) {
            req = insert_request(temp_req, contents, ssl);
        }

        //delete
        if (strcmp(temp_req->operation, "delete") == 0) {
            req = delete_request(temp_req, contents, ssl);
        }

        //update
        if (strcmp(temp_req->operation, "update") == 0) {
            req = update_request(temp_req, contents, ssl);
        }
    }

    // if the operation is valid and the number of arguments is correct, return
    // the request with updated values
    printf("Server: Request processed\n");
    return req;
}