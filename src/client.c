#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include "socket_client/auto_client_transfer.h"

#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 8888
#define BUFFER_SIZE 4096

int socket_fd;

void recv_message() {
    char buffer[BUFFER_SIZE];
    while (1) {
        ssize_t n = recv(socket_fd, buffer, sizeof(buffer), 0);
        if (n <= 0) {
            printf("Server closed.\n");
            break;
        }
        printf("Recv: %s\n", buffer);
        client_response(buffer);
    }
}

void send_message() {
    char buffer[BUFFER_SIZE];
    printf("Enter a command: ");

    while (fgets(buffer, BUFFER_SIZE, stdin)) {
        buffer[strcspn(buffer, "\n")] = '\0'; // Remove newline character

        cJSON *json = cJSON_CreateObject();

        if (strcmp(buffer, "/exit") == 0) { // Exit
            printf("Exiting...\n");
            break;
        } else if (strcmp(buffer, "/login") == 0) {
            // Login, input username and password, then send to the server
            cJSON_AddStringToObject(json, "cmd", "login");
            cJSON_AddStringToObject(json, "parm1", "kivvi");
            cJSON_AddStringToObject(json, "parm2", "000000");
        } else if (strcmp(buffer, "/register") == 0) {
            // Register, input username and password, then send to the server
            cJSON_AddStringToObject(json, "cmd", "register");
            cJSON_AddStringToObject(json, "parm1", "admin");
            cJSON_AddStringToObject(json, "parm2", "123456");
        } else if (strcmp(buffer, "/history") == 0) {   // History
            cJSON_AddStringToObject(json, "cmd", "get_history");
            cJSON_AddStringToObject(json, "parm1", token);
            cJSON_AddStringToObject(json, "parm2", "10");
        } else if (strncmp(buffer, "/send ", 6) == 0) {
            cJSON_AddStringToObject(json, "cmd", "send_message");
            cJSON_AddStringToObject(json, "parm1", token);
            cJSON_AddStringToObject(json, "parm2", "HelloWorld form admin");
        } else {
            printf("Invalid command. Please try again.\n");
            continue;
        }

        char *json_string = cJSON_PrintUnformatted(json);
        printf("Send: %s\n", json_string);
        send(socket_fd, json_string, strlen(json_string), 0);
        cJSON_Delete(json);
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
//    SimpleMessageStruct *simple_message = (SimpleMessageStruct *) malloc(sizeof(SimpleMessageStruct));
//    strcpy(simple_message->username, "admin123");
//    strcpy(simple_message->message, "admin1234543");
//    add_message_to_json_file(CHAT_HISTORY_PATH, simple_message);
}
