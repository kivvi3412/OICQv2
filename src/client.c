#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include "socket_client/client_response_main_function.h"
#include "socket_client/client_sender_receiver_function.h"

#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 8888

int socket_fd;

void recv_message() {   // Receive message from server
    char buffer[BUFFER_SIZE_CLIENT];
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        ssize_t n = recv(socket_fd, buffer, sizeof(buffer), 0);
        if (n <= 0) {
            printf("Server closed.\n");
            break;
        }
        client_response(buffer);
    }
}

void send_message() {
    char buffer[BUFFER_SIZE_CLIENT];
    printf("Enter a command: ");

    while (fgets(buffer, BUFFER_SIZE_CLIENT, stdin)) {
        buffer[strcspn(buffer, "\n")] = '\0'; // Remove newline character

        char *json_string = (char *) malloc(sizeof(char) * BUFFER_SIZE_CLIENT);
        if (strcmp(buffer, "/login") == 0) {
            // Login, input username and password, then send to the server
            char username[BUFFER_SIZE_CLIENT];
            char password[BUFFER_SIZE_CLIENT];

            printf("Enter your username: ");
            fgets(username, BUFFER_SIZE_CLIENT, stdin);
            username[strcspn(username, "\n")] = '\0';

            printf("Enter your password: ");
            fgets(password, BUFFER_SIZE_CLIENT, stdin);
            password[strcspn(password, "\n")] = '\0';

            json_string = login_user(username, password);
        } else if (strcmp(buffer, "/register") == 0) {
            // Register, input username and password, then send to the server
            char username[BUFFER_SIZE_CLIENT];
            char password[BUFFER_SIZE_CLIENT];

            printf("Enter your username: ");
            fgets(username, BUFFER_SIZE_CLIENT, stdin);
            username[strcspn(username, "\n")] = '\0';

            printf("Enter your password: ");
            fgets(password, BUFFER_SIZE_CLIENT, stdin);
            password[strcspn(password, "\n")] = '\0';

            json_string = register_user(username, password);
        } else if (strcmp(buffer, "/update_password") == 0) {
            char password[BUFFER_SIZE_CLIENT];

            printf("Enter your new password: ");
            fgets(password, BUFFER_SIZE_CLIENT, stdin);
            password[strcspn(password, "\n")] = '\0';

            json_string = update_password_user(token, password);
        } else if (strcmp(buffer, "/delete") == 0) {
            char delete[BUFFER_SIZE_CLIENT];

            // 输入delete确认删除
            printf("Enter delete to confirm: ");
            fgets(delete, BUFFER_SIZE_CLIENT, stdin);
            delete[strcspn(delete, "\n")] = '\0';
            if (strcmp(delete, "delete") != 0) {
                printf("Abort.\n");
                continue;
            }
            json_string = delete_user(token);
        } else if (strcmp(buffer, "/history") == 0) {   // History
            json_string = get_history(token);
        } else if (strncmp(buffer, "/send ", 6) == 0) {
            char *message = buffer + 6;
            json_string = send_message_client(token, message);
        } else {
            printf("Invalid command. Please try again.\n");
            continue;
        }

        send(socket_fd, json_string, strlen(json_string), 0);
        free(json_string);
    }
}


void client() {
    struct sockaddr_in server_address;

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
    server_address.sin_port = htons(SERVER_PORT);

    if (connect(socket_fd, (struct sockaddr *) &server_address, sizeof(server_address)) == -1) {
        printf("Connection failed.\n");
        exit(1);
    }

    pid_t pid = fork(); // 创建子进
    if (pid == -1) {
        perror("fork");
        exit(1);
    } else if (pid == 0) { // 子进程
        recv_message();
        exit(0);
    } else { // 父进程
        send_message();
        // 等待子进程结束
        int status;
        waitpid(pid, &status, 0);
    }
}

int main(void) {
    init_shared_memory();
    client();
}
