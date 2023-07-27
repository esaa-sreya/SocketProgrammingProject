#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 1234
#define BUFFER_SIZE 1024



void sendCommand1Output(int client_socket, const char* command) {
    FILE* fp;
    char buffer[BUFFER_SIZE];

    // Execute the command and retrieve output
    fp = popen(command, "r");
    if (fp == NULL) {
        perror("popen failed");
        exit(EXIT_FAILURE);
    }

    while (fgets(buffer, BUFFER_SIZE, fp) != NULL) {

        // Send output to the client socket
        if (send(client_socket, buffer, strlen(buffer), 0) < 0) {
            perror("send failed");
            exit(EXIT_FAILURE);
        }
    }

    pclose(fp);
}



void sendCommand2Output(int client_socket, const char* command) {
    FILE* fp;
    char buffer[BUFFER_SIZE];
    int line_count = 0;

    // Execute the command and retrieve output
    fp = popen(command, "r");
    if (fp == NULL) {
        perror("popen failed");
        exit(EXIT_FAILURE);
    }

    while (fgets(buffer, BUFFER_SIZE, fp) != NULL) {
        line_count++;

        // Send only the third line to the client socket
        if (line_count == 3) {

            if (send(client_socket, buffer, strlen(buffer), 0) < 0) {
                perror("send failed");
                exit(EXIT_FAILURE);
            }
            break;
        }
    }

    pclose(fp);
}



int main(int argc, char *argv[]) {



    // Display
    if (argc != 2) {
        printf("Usage: ./server <display>\n");
        exit(EXIT_FAILURE);
    }

    int display_interval = atoi(argv[1]);
    int terminate = 0;



    // SOCEKET PROGRAM

    int server_fd, client_socket;
    //server_fd -> server socket file descriptor (unique identifier)
    //read_size -> to store no. of bytes read from socket

    struct sockaddr_in server_addr, client_addr;


    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {    //SOCK_STREAM -> arg specifying socket type (TCP socket) ; 0 -> protocol arg
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Prepare the server address structure
    server_addr.sin_family = AF_INET;             // IPv4 addresses
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);           // port number

    // Bind the socket to the specified IP and port
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {     //three -> maximum number of pending connections allowed
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    // Accept incoming connection
    int client_addr_len = sizeof(client_addr);
    if ((client_socket = accept(server_fd, (struct sockaddr *)&client_addr, (socklen_t *)&client_addr_len)) < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }



    //Execute the command date and capture its output
    FILE *fp;
    char content[1035];

    fp = popen("date", "r");
    if (fp == NULL) {
        printf("Failed to run command\n");
        return 1;
    }

    while (fgets(content, sizeof(content), fp) != NULL) {
        printf("Client connected at: %s", content);
        memset(content, 0, sizeof(content));
    }

    pclose(fp);



    // Send the display_interval value to the client
    if (send(client_socket, &display_interval, sizeof(display_interval), 0) == -1) {
        perror("Failed to send display_interval to the client");
        close(client_socket);
        close(server_fd); 
        exit(EXIT_FAILURE);
    }



    // Send output of multiple commands to the client socket
    while (1) {

        sendCommand1Output(client_socket, "free -h");
        sendCommand2Output(client_socket, "df -h");
        sendCommand2Output(client_socket, "top");

        sleep(display_interval);

    }
    
    
    
    close(server_fd);
    printf("Server socket closed.\n");



    return 0;

}