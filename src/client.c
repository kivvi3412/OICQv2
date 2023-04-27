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
#include "socket_client/display_function.h"

#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 8888

int socket_fd;

void recv_message() {   // Receive message from server
    char buffer[BUFFER_SIZE_CLIENT];    // 创建缓冲区
    while (1) { // 循环监听客户端请求
        memset(buffer, 0, sizeof(buffer));  // 清空缓冲区
        ssize_t n = recv(socket_fd, buffer, sizeof(buffer), 0); // 接收客户端消息
        if (n <= 0) {   // 接收出错
            server_closed_menu();   // 提示服务器关闭
            break;  //  跳出循环
        }
        client_response(buffer);    // 处理客户端消息
    }
}

void send_message() {   // Send message to server
    char buffer[BUFFER_SIZE_CLIENT];    // 创建缓冲区
    input_commend_menu();   // 提示输入命令

    while (fgets(buffer, BUFFER_SIZE_CLIENT, stdin)) {  // 从标准输入读取命令
        buffer[strcspn(buffer, "\n")] = '\0'; // Remove newline character

        char *json_string = (char *) malloc(sizeof(char) * BUFFER_SIZE_CLIENT);
        if (strcmp(buffer, "/login") == 0) {    // Login, input username and password, then send to the server
            // Login, input username and password, then send to the server
            char username[BUFFER_SIZE_CLIENT];  // 创建缓冲区
            char password[BUFFER_SIZE_CLIENT];

            name_input_menu();  // 提示输入用户名
            fgets(username, BUFFER_SIZE_CLIENT, stdin); // 从标准输入读取用户名
            username[strcspn(username, "\n")] = '\0';   //  去除换行符

            password_input_menu();
            fgets(password, BUFFER_SIZE_CLIENT, stdin);
            password[strcspn(password, "\n")] = '\0';

            json_string = login_user(username, password);
        } else if (strcmp(buffer, "/register") == 0) {
            // Register, input username and password, then send to the server
            char username[BUFFER_SIZE_CLIENT];
            char password[BUFFER_SIZE_CLIENT];

            name_input_menu();
            fgets(username, BUFFER_SIZE_CLIENT, stdin);
            username[strcspn(username, "\n")] = '\0';

            password_input_menu();
            fgets(password, BUFFER_SIZE_CLIENT, stdin);
            password[strcspn(password, "\n")] = '\0';

            json_string = register_user(username, password);
        } else if (strcmp(buffer, "/update_password") == 0) {
            char password[BUFFER_SIZE_CLIENT];

            new_password_input_menu();
            fgets(password, BUFFER_SIZE_CLIENT, stdin);
            password[strcspn(password, "\n")] = '\0';

            json_string = update_password_user(token, password);
        } else if (strcmp(buffer, "/delete") == 0) {
            char delete[BUFFER_SIZE_CLIENT];

            // 输入delete确认删除
            confirm_delete_menu();
            fgets(delete, BUFFER_SIZE_CLIENT, stdin);
            delete[strcspn(delete, "\n")] = '\0';
            if (strcmp(delete, "delete") != 0) {
                abort_menu();
                continue;
            }
            json_string = delete_user(token);
        } else if (strcmp(buffer, "/history") == 0) {   // History
            json_string = get_history(token);
        } else if (strncmp(buffer, "/send ", 6) == 0) {
            char *message = buffer + 6;
            json_string = send_message_client(token, message);
        } else {
            invalid_command();  // 提示无效命令
            continue;
        }

        send(socket_fd, json_string, strlen(json_string), 0);   //  发送消息
        free(json_string);  // 释放内存
    }
}


void client() {
    struct sockaddr_in server_address;  // 创建服务器地址

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);    // 创建套接字
    memset(&server_address, 0, sizeof(server_address)); //  清空服务器地址
    server_address.sin_family = AF_INET;    // 设置地址族
    server_address.sin_addr.s_addr = inet_addr(SERVER_ADDRESS); // 设置IP地址
    server_address.sin_port = htons(SERVER_PORT);   // 设置端口号

    if (connect(socket_fd, (struct sockaddr *) &server_address, sizeof(server_address)) == -1) {    // 连接服务器
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
    init_shared_memory();   // 初始化共享内存
    client();   // 启动客户端
}
