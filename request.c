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
    const char *operations[] = {"get", "insert", "select", "delete", "update", "seed-database", "exit"};
    const int num_operations = sizeof(operations) / sizeof(operations[0]);
    char    contents[CONTENT_SIZE],
            error_buffer[BUFFER_SIZE],
            dummy[BUFFER_SIZE],
            exit_buffer[BUFFER_SIZE];
    int arg_number;
    bool valid_request = false;

    sscanf(message, "%s %s", req->operation, contents);

    // first check if the operation is valid
    for (int i = 0; i < num_operations; i++) {
        if (strcmp(req->operation, operations[i]) == 0) {
            valid_request = true;
            break;
        }
    }
    
    // if the operation is not valid, send an error code to the client
    if (!valid_request) {
        sprintf(error_buffer, "error_id-%d: Invalid Request", 3);
        write_to_ssl(ssl, error_buffer, strlen(error_buffer), "Server");
        req->operation[0] = '\0';
        return req;
    }

	// if the operation is download and the file does not exist, send an error code
    // to the client
    if (strcmp(req->operation, "exit") == 0) {
        sprintf(exit_buffer, "Disconnecting\n");
        write_to_ssl(ssl, exit_buffer, strlen(exit_buffer), "Server");
        return req;
    } else {
        if (strcmp(req->operation, "seed-database") == 0) {
            seed_database();
        }
        //get
        if (strcmp(req->operation, "get") == 0) {
            req = get_request(req, contents, ssl);
        }
        
        //select
        if (strcmp(req->operation, "select") == 0) {
            req = select_request(req, contents, ssl);
        }

        //insert
        if (strcmp(req->operation, "insert") == 0) {
            req = insert_request(req, contents, ssl);
        }

        //delete
        if (strcmp(req->operation, "delete") == 0) {
            req = delete_request(req, contents, ssl);
        }

        //update
        if (strcmp(req->operation, "update") == 0) {
            req = update_request(req, contents, ssl);
        }
    }

    // if the operation is valid and the number of arguments is correct, return
    // the request with updated values
    return req;
}