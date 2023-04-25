//
// Created by HAIRONG ZHU on 2023/4/24.
//

#ifndef OICQV2_SERVER_SOCKET_HANDLER_H
#define OICQV2_SERVER_SOCKET_HANDLER_H
#include <string.h>
#include <unistd.h>
#include "auto_server_transfer.h"

// 处理客户端请求的线程函数
void *handle_client(void *arg) {
    struct client_info *client = (struct client_info *) arg;
    char buffer[BUFFER_SIZE_CLIENT];
    ssize_t len;

    printf("Client connected: %s:%d\n",
           inet_ntoa(client->address.sin_addr), ntohs(client->address.sin_port));

    // If the recv function returns 0, it means that the client has disconnected.
    // If the recv function returns a negative number, it means that an error has occurred.
    while ((len = recv(client->socket_fd, buffer, BUFFER_SIZE_CLIENT, 0)) > 0) {   // 单个用户发消息广播函数
        buffer[len] = '\0';
        printf("Received message from client: %s\n", buffer);
        // 处理客户端请求
        server_response(buffer, client);
    }
    // 添加断开连接释放代码
    pthread_mutex_lock(&mutex);  // 加锁，保护 clients 变量
    for (int i = 0; i < MAX_CLIENTS; i++) { // 将断开连接的客户端从 clients 中移除
        if (clients[i] != NULL && clients[i]->socket_fd == client->socket_fd) {
            printf("Deleted client: %d\n", client->socket_fd);
            clients[i] = NULL;
            client_count--;
            break;
        }
    }
    pthread_mutex_unlock(&mutex);  // 解锁
    printf("client number: %d\n", client_count);
    printf("Client disconnected: %s:%d\n",
           inet_ntoa(client->address.sin_addr), ntohs(client->address.sin_port));
    close(client->socket_fd);   // 关闭客户端socket
    free(client);    // 释放 client_info 结构体内存
    return NULL;
}

#endif //OICQV2_SERVER_SOCKET_HANDLER_H
