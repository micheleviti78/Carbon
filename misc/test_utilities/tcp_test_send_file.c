#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SERVER_PORT 9999           // Server port
#define BUFFER_SIZE 1024           // Buffer size for reading file

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <binary file path> <ip address>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *file_path = argv[1];
    const char *ip_addr = argv[2];
    int sock, file_fd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read, bytes_sent;

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    // Define server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, ip_addr, &server_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
        0) {
        perror("Connection failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Open the binary file for reading
    if ((file_fd = open(file_path, O_RDONLY)) < 0) {
        perror("Failed to open file");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Read from the file and send data to the server
    while ((bytes_read = read(file_fd, buffer, BUFFER_SIZE)) > 0) {
        bytes_sent = send(sock, buffer, bytes_read, 0);
        if (bytes_sent < 0) {
            perror("Send failed");
            close(file_fd);
            close(sock);
            exit(EXIT_FAILURE);
        }
        printf("Sent %zd bytes to the server\n", bytes_sent);
    }

    if (bytes_read < 0) {
        perror("File read error");
    }

    // Clean up
    close(file_fd);
    close(sock);

    printf("File sent successfully.\n");
    return 0;
}
