#include <stdio.h>          // 标准输入输出
#include <stdlib.h>         // 标准库函数
#include <string.h>         // 字符串函数
#include <stdbool.h>        // 布尔类型
#include <sys/socket.h>     // socket 相关函数
#include <netinet/in.h>     // IPv4 地址结构体
#include <arpa/inet.h>      // 网络字节序转换函数
#include <pthread.h>        // 多线程支持
#include <unistd.h>         // close() 函数
#include "basic_info/basic_definition.h"
#include "socket_server/server_socket_handler.h"

void server() {
    int server_socket_fd, new_socket_fd;    // 文件描述符
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_len = sizeof(client_address);

    // 创建服务器端socket, AF_INET 表示使用 IPv4 协议，SOCK_STREAM 表示使用 TCP 协议, 0 表示使用默认协议
    if ((server_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // 设置服务器socket参数
    int enable = true;
    if (setsockopt(server_socket_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) == -1) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // 初始化服务器地址信息
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("0.0.0.0");
    server_address.sin_port = htons(PORT);

    // 绑定socket和地址信息
    if (bind(server_socket_fd, (struct sockaddr *) &server_address, sizeof(server_address)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // 监听socket并设置允许连接个数
    if (listen(server_socket_fd, MAX_CLIENTS) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // 循环监听客户端请求
    while (true) {
        // 接受客户端连接，阻塞等待
        if ((new_socket_fd = accept(server_socket_fd, (struct sockaddr *) &client_address,
                                    &client_address_len)) == -1) {
            perror("accept");
            continue;
        }

        // 创建一个新的 client_info 结构体，将其加入到 clients 数组中保存
        struct client_info *client = (struct client_info *) malloc(sizeof(struct client_info));
        client->socket_fd = new_socket_fd;
        client->address = client_address;
        pthread_mutex_lock(&mutex);  // 加锁，保护 client_count 变量
        if (client_count < MAX_CLIENTS) {
            //  clients[client_count++] = client;     // 这么写可能会导致覆盖, 因为别的地方还有client_count--
            for (int i = 0; i < MAX_CLIENTS; i++) { // 循环检测是否有空位
                if (clients[i] == NULL) {
                    clients[i] = client;
                    client_count++;
                    break;
                }
            }
        } else {
            perror("max clients reached");
            close(new_socket_fd);
            free(client);
            continue;
        }
        pthread_mutex_unlock(&mutex);  // 解锁

        // 创建一个新线程来处理该客户端的请求
        pthread_t tid;
        if (pthread_create(&tid, NULL, handle_client, client) != 0) {
            perror("pthread_create");
        }
        pthread_detach(tid);   // 将子线程设置为 detach state，子线程结束后线程资源自动回收

    }

    // 关闭socket
    // close(server_socket_fd);
}

int main() {
    init_shared_memory();
    server();
}