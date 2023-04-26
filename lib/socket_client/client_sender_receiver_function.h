//
// Created by HAIRONG ZHU on 2023/4/24.
//

/*
 * 本文件是客户端的发送和接收函数
 */

#ifndef OICQV2_CLIENT_SENDER_RECEIVER_FUNCTION_H
#define OICQV2_CLIENT_SENDER_RECEIVER_FUNCTION_H


#include "stdio.h"
#include "socket_client/client_response_main_function.h"
#include "shared_memory_dir/shared_memory.h"
#include "socket_client/display_function.h"

// 以下是获得服务器反馈后调用的函数
// 失败通用函数
void general_message_failed(char *info) {
    general_message_failed_menu(info);
}

// 成功通用函数
void general_message_success(char *info) {
    general_message_success_menu(info);
}

// 消息发送失败通用函数
void send_message_failed(char *info) {
    send_message_failed_menu(info);
}

// 消息发送成功函数
void send_message_success() {
//    send_message_success_menu();
    printf("");
}

// 保存token
void save_token(char *input_token) {
    strcpy(token, input_token);
}

// 显示历史记录
void show_history_messages(SimpleMessageStruct *history_messages, int count) {
    show_history_messages_menu(history_messages, count);
}

// 显示单条消息
void show_message(SimpleMessageStruct *message) {
    show_message_menu(message);
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

// 更新密码函数，输入token和新密码
char *update_password_user(char *token_func, char *new_password) {
    cJSON *json_obj = cJSON_CreateObject();
    cJSON_AddStringToObject(json_obj, "cmd", "update_password");
    cJSON_AddStringToObject(json_obj, "parm1", token_func);
    cJSON_AddStringToObject(json_obj, "parm2", new_password);
    return cJSON_PrintUnformatted(json_obj);
}

// 删除账户函数，输入token
char *delete_user(char *token_func) {
    cJSON *json_obj = cJSON_CreateObject();
    cJSON_AddStringToObject(json_obj, "cmd", "delete_user");
    cJSON_AddStringToObject(json_obj, "parm1", token_func);
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


#endif //OICQV2_CLIENT_SENDER_RECEIVER_FUNCTION_H
