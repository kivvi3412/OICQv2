//
// Created by HAIRONG ZHU on 2023/4/24.
//

#ifndef OICQV2_CLIENT_SOCKET_SENDER_H
#define OICQV2_CLIENT_SOCKET_SENDER_H


#include "stdio.h"
#include "socket_client/auto_client_transfer.h"
#include "shared_memory_dir/shared_memory.h"

// 以下是获得服务器反馈后调用的函数
// 登录注册失败通用函数
void login_register_failed(char *info) {
    printf("%s\n", info);
}

// 消息发送失败通用函数
void send_message_failed(char *info) {
    printf("%s\n", info);
}

// 保存token
void save_token(char *input_token) {
    strcpy(token, input_token);
}

// 显示历史记录
void show_history_messages(SimpleMessageStruct *history_messages, int count) {
    for (int i = 0; i < count; i++) {
        printf("%s: %s\n", history_messages[i].username, history_messages[i].message);
    }
}

// 消息发送成功函数
void send_message_success() {
    printf("Send message success\n");
}

// 显示单条消息
void show_message(SimpleMessageStruct *message) {
    printf("%s: %s\n", message->username, message->message);
}

// 打印token
void print_token() {
    printf("token: %s\n", token);
}

// 获取token
char *get_token() {
    return token;
}

// 以下是发送请求给服务器的函数
// 注册



#endif //OICQV2_CLIENT_SOCKET_SENDER_H
