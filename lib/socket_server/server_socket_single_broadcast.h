//
// Created by HAIRONG ZHU on 2023/4/24.
//

#ifndef OICQV2_SERVER_SOCKET_SINGLE_BROADCAST_H
#define OICQV2_SERVER_SOCKET_SINGLE_BROADCAST_H

#include <pthread.h>
#include <string.h>
#include "basic_info/basic_definition.h"

// 广播消息函数
void broadcast_message_func(char *message, int sender_socket_fd) {   // 不给某人发消息sender_socket_fd
    pthread_mutex_lock(&mutex);   // 加锁，保护 clients, 防止被其他线程修改
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] != NULL && clients[i]->socket_fd != sender_socket_fd) {    // 不给自己发消息
            // 使用 send 函数向指定客户端发送消息
            send(clients[i]->socket_fd, message, strlen(message), 0);
        }
    }
    pthread_mutex_unlock(&mutex);  // 解锁
}

// 给某人发消息函数
void send_message_func(char *message, int receiver_socket_fd) {   // 给某人发消息
    pthread_mutex_lock(&mutex);   // 加锁，保护 clients, 防止被其他线程修改
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] != NULL && clients[i]->socket_fd == receiver_socket_fd) {
            // 使用 send 函数向指定客户端发送消息
            send(clients[i]->socket_fd, message, strlen(message), 0);
        }
    }
    pthread_mutex_unlock(&mutex);  // 解锁
}

#endif //OICQV2_SERVER_SOCKET_SINGLE_BROADCAST_H
