//
// Created by HAIRONG ZHU on 2023/4/24.
//

#ifndef OICQV2_AUTO_CLIENT_TRANSFER_H
#define OICQV2_AUTO_CLIENT_TRANSFER_H

#include <string.h>
#include "stdio.h"
#include "stdlib.h"
#include "cJSON/cJSON.h"
#include "socket_tools/struct_json_transfer.h"
#include "socket_client/client_socket_sender.h"

/*
 * 此函数用于检测服务器的消息，首先检查消息是否为 JSON 格式，然后检查 JSON 格式是否符合规范
 * 检查用户发来的info指令，再由info指令调用对应的函数
 * @param input_json: 服务器发过来的消息
 * @return: 发送给服务器的信息
 */

char *client_response(char *input_json) {
    if (!input_json) {
        printf("Invalid input input_json\n");
        return NULL;
    }

    cJSON *json_obj = cJSON_Parse(input_json);
    if (!json_obj) {
        printf("Failed to parse JSON: %s\n", cJSON_GetErrorPtr());
        return NULL;
    }

    cJSON *info = cJSON_GetObjectItem(json_obj, "info");
    if (!info) {
        printf("Failed to get info\n");
        return NULL;
    }

    cJSON *msg = cJSON_GetObjectItem(json_obj, "msg");
    if (!msg) {
        printf("Failed to get msg\n");
        return NULL;
    }

    if (strcmp(info->valuestring, "login") == 0) {   // login
        if (strcmp(msg->valuestring, "failed") == 0) {
            login_register_failed("login failed");
        } else {
            save_token(msg->valuestring);
        }
    } else if (strcmp(info->valuestring, "register") == 0) { // register
        if (strcmp(msg->valuestring, "failed") == 0) {
            login_register_failed("register failed");   //调用失败函数
        } else {
            save_token(msg->valuestring);   //调用成功函数, 保存token
        }
    } else if (strcmp(info->valuestring, "history") == 0) {  // 发过来历史记录
        SimpleMessageStruct history_messages[MAX_MESSAGES];
        int count = read_history_messages_from_json_string(input_json, history_messages, MAX_MESSAGES);
        show_history_messages(history_messages, count); // 调用显示历史记录的函数
    } else if (strcmp(info->valuestring, "send_message_status") == 0) {  // 发过来新消息
        if (strcmp(msg->valuestring, "failed") == 0) {
            send_message_failed("send_message_failed!");//调用失败函数
        } else {
            send_message_success(); //调用成功函数
        }
    } else if (strcmp(info->valuestring, "group_message") == 0) {  // 发过来新消息
        // 转化为SimpleMessageStruct
        SimpleMessageStruct *message = get_message_from_json_string(input_json);
        show_message(message);  // 调用显示单条新消息的函数
    } else {
        printf("Invalid info\n");
        return NULL;
    }
}

#endif //OICQV2_AUTO_CLIENT_TRANSFER_H
