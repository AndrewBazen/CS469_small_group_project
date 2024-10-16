/**
 * @file utilities.c
 * @author ** Andrew Bazen **
 * @date ** 22 September 2024 **
 * @brief This program contains utility functions that are used by both the server
 *       and client programs
*/
#include "utilities.h"

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