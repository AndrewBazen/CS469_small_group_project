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
struct Request process_request(char *message, SSL *ssl) {
    struct Request req;
    const char *operations[] = {"get", "insert", "select", "delete", "exit"};
    const int num_operations = sizeof(operations) / sizeof(operations[0]);
    char  contents[CONTENT_SIZE],
        error_buffer[BUFFER_SIZE],
        dummy[BUFFER_SIZE],
        exit_buffer[BUFFER_SIZE];
    int arg_number;
    bool valid_request = false;

    // scan the message with a format that will account for all cases, while also
    // storing the arguments in the request struct and how many arguments there are
    arg_number = sscanf(message, "%s %s %s",req.operation, req.content, dummy);
    arg_number--; 

    // first check if the operation is valid
    for (int i = 0; i < num_operations; i++) {
        if (strcmp(req.operation, operations[i]) == 0) {
            valid_request = true;
            break;
        }
    }
    
    // if the operation is not valid, send an error code to the client
    if (!valid_request) {
        sprintf(error_buffer, "error_id-%d", 3);
        write_to_ssl(ssl, error_buffer, strlen(error_buffer), "Server");
        req.operation[0] = '\0';
        return req;
    }

	// if the operation is download and the file does not exist, send an error code
    // to the client
    if (strcmp(req.operation, "exit") == 0) {
        sprintf(exit_buffer, "Disconnecting\n");
        write_to_ssl(ssl, exit_buffer, strlen(exit_buffer), "Server");
        return req;
    } else {
        // if it is a valid operation, check the number of arguments and send an 
        // error code if there are too few or too many
        if (arg_number < 1) {
                sprintf(error_buffer, "error_id-%d", 1);
            write_to_ssl(ssl, error_buffer, strlen(error_buffer), "Server");
            req.operation[0] = '\0';
            return req;
        } else if (arg_number > 1) {
            sprintf(error_buffer, "error_id-%d", 2);
            write_to_ssl(ssl, error_buffer, strlen(error_buffer), "Server");
            req.operation[0] = '\0';
            return req;
        }
    }

    // if the operation is valid and the number of arguments is correct, return
    // the request with updated values
    return req;
}