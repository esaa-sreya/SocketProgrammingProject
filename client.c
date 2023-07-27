#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>

#define SERVER_IP "127.0.0.1"
#define PORT 1234
#define BUFFER_SIZE 1024



int main(int argc, char* argv[]) {



    // Display 
    if (argc != 2) {
        printf("Usage: ./client <display>\n");
        exit(EXIT_FAILURE);
    }

    char* display = argv[1];
    
    
    
    // SOCKET PROGRAM

    int client_socket;
    struct sockaddr_in server_addr;

    // Create socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Prepare the server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to the server.\n");



    int display_interval;

    // Receive the display_interval value from the server
    if (recv(client_socket, &display_interval, sizeof(display_interval), 0) == -1) {
        perror("Failed to receive display_interval from the server");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    // Use the received display_interval value
    printf("Received display_interval from the server: %d\n", display_interval);



    char* buffer;

    // Allocate memory for the buffer using calloc
    buffer = (char*)calloc(BUFFER_SIZE, sizeof(char));
    if (buffer == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }



    // RECIEVE AND DISPLAY MESSAGES FROM SERVER

    if (strcmp(display, "hold") == 0) {

        // Infinite display mode    
        printf("Staying connected indefinitely...\n");

        while (1) {

            // Receive data and store it in the buffer
            ssize_t num_bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
            if (num_bytes_received > 0) {
                buffer[num_bytes_received] = '\0'; // Null-terminate the received data
                printf("Received from the server: \n%s\n", buffer);
            } else if (num_bytes_received == 0) {
                printf("Server closed the connection\n");
            } else {
                perror("Failed to receive data");
            }

            sleep(1);

        }
    } else {

        // Time-limited display mode
        int display_time = atoi(display);
        int remaining_time = display_time;

        printf("Staying connected for %d seconds...\n", remaining_time);

        while (remaining_time > 0) {

            sleep(1);


            printf("Remaining time: %d\n", remaining_time);


            // Receive data and store it in the buffer
            ssize_t num_bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
            if (num_bytes_received > 0) {
                buffer[num_bytes_received] = '\0'; // Null-terminate the received data
                printf("Received from the server: \n%s\n", buffer);
            } else if (num_bytes_received == 0) {
                printf("Server closed the connection\n");
            } else {
                perror("Failed to receive data");
            }


            remaining_time = remaining_time - display_interval;

        }

    }



    // Free the allocated buffer
    free(buffer);



    close(client_socket);
    printf("Client socket closed.\n");


    return 0;

}