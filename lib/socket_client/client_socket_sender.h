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

// 登录成功通用函数
void login_register_success(char *info) {
    printf("%s\n", info);
}

// 消息发送失败通用函数
void send_message_failed(char *info) {
    printf("%s\n", info);
}

// 消息发送成功函数
void send_message_success() {
    printf("Send message success\n");
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

// 显示单条消息
void show_message(SimpleMessageStruct *message) {
    printf("%s: %s\n", message->username, message->message);
}

// 以下是发送请求给服务器的函数
// 注册函数，输入用户名密码
char *register_user(char *username, char *password) {
    cJSON *json_obj = cJSON_CreateObject();
    cJSON_AddStringToObject(json_obj, "cmd", "register");
    cJSON_AddStringToObject(json_obj, "parm1", username);
    cJSON_AddStringToObject(json_obj, "parm2", password);
    return cJSON_PrintUnformatted(json_obj);
}

// 登录函数, 输入用户名密码
char *login_user(char *username, char *password) {
    cJSON *json_obj = cJSON_CreateObject();
    cJSON_AddStringToObject(json_obj, "cmd", "login");
    cJSON_AddStringToObject(json_obj, "parm1", username);
    cJSON_AddStringToObject(json_obj, "parm2", password);
    return cJSON_PrintUnformatted(json_obj);
}

// 获取历史记录函数，输入token
char *get_history(char *token_temp) {
    cJSON *json_obj = cJSON_CreateObject();
    cJSON_AddStringToObject(json_obj, "cmd", "get_history");
    cJSON_AddStringToObject(json_obj, "parm1", token_temp);
    return cJSON_PrintUnformatted(json_obj);
}

// 发送消息，输入token和发送消息的内容
char *send_message_client(char *token_temp, char *message) {
    cJSON *json_obj = cJSON_CreateObject();
    cJSON_AddStringToObject(json_obj, "cmd", "send_message");
    cJSON_AddStringToObject(json_obj, "parm1", token_temp);
    cJSON_AddStringToObject(json_obj, "parm2", message);
    return cJSON_PrintUnformatted(json_obj);
}


#endif //OICQV2_CLIENT_SOCKET_SENDER_H
