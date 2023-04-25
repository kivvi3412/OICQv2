//
// Created by HAIRONG ZHU on 2023/4/24.
//

#ifndef OICQV2_AUTO_SERVER_TRANSFER_H
#define OICQV2_AUTO_SERVER_TRANSFER_H

#include <string.h>
#include "stdio.h"
#include "stdlib.h"
#include "cJSON/cJSON.h"
#include "socket_tools/struct_json_transfer.h"
#include "socket_server/server_socket_handler.h"
#include "socket_server/server_socket_sender.h"
#include "server_socket_single_broadcast.h"
#include "shared_memory_dir/shared_memory.h"

/*
 * 此函数用于检测客户端的消息，首先检查消息是否为 JSON 格式，然后检查 JSON 格式是否符合规范
 * 检查用户发来的cmd指令，再由cmd指令调用对应的函数
 * @param input_json: 客户端发送的消息
 * @return: 服务器响应的消息
 */
void *server_response(char *input_json, client_info *client) {
    if (!input_json) {
        printf("Invalid input input_json\n");
        return NULL;
    }

    cJSON *json_obj = cJSON_Parse(input_json);
    if (!json_obj) {
        printf("Failed to parse JSON: %s\n", cJSON_GetErrorPtr());
        return NULL;
    }

    cJSON *cmd = cJSON_GetObjectItem(json_obj, "cmd");
    if (!cmd) {
        printf("Failed to get cmd\n");
        return NULL;
    }

    if (strcmp(cmd->valuestring, "login") == 0) {   // login
        cJSON *username = cJSON_GetObjectItem(json_obj, "parm1");
        cJSON *password = cJSON_GetObjectItem(json_obj, "parm2");
        if (!username || !password) {
            printf("Failed to get username or password\n");
            CommonJsonServer *temp_message = (CommonJsonServer *) malloc(sizeof(CommonJsonServer));
            strcpy(temp_message->info, "login");
            strcpy(temp_message->msg, "failed");
            send_general_server_json(temp_message, client);
            free(temp_message);
            return NULL;
        }
        send_general_server_text(login_from_json(input_json), client);  // 发送登录结果
        return NULL;
    } else if (strcmp(cmd->valuestring, "register") == 0) { // register
        cJSON *username = cJSON_GetObjectItem(json_obj, "parm1");
        cJSON *password = cJSON_GetObjectItem(json_obj, "parm2");
        if (!username || !password) {
            printf("Failed to get username or password\n");
            CommonJsonServer *temp_message = (CommonJsonServer *) malloc(sizeof(CommonJsonServer));
            strcpy(temp_message->info, "register");
            strcpy(temp_message->msg, "failed");
            send_general_server_json(temp_message, client);
            free(temp_message);
            return NULL;
        }
        send_general_server_text(register_from_json(input_json), client);   // 发送注册结果
        return NULL;
    } else if (strcmp(cmd->valuestring, "update_password") == 0) {
        cJSON *token_cjson = cJSON_GetObjectItem(json_obj, "parm1");
        cJSON *new_password = cJSON_GetObjectItem(json_obj, "parm2");
        if (!token_cjson || !new_password) {
            printf("Failed to get token_cjson or new_password\n");
            CommonJsonServer *temp_message = (CommonJsonServer *) malloc(sizeof(CommonJsonServer));
            strcpy(temp_message->info, "update_password");
            strcpy(temp_message->msg, "failed");
            send_general_server_json(temp_message, client);
            free(temp_message);
            return NULL;
        }
        send_general_server_text(update_password_by_token(input_json), client);   // 发送更新密码结果
        return NULL;
    } else if (strcmp(cmd->valuestring, "delete_user") == 0) {
        cJSON *token_cjson = cJSON_GetObjectItem(json_obj, "parm1");
        if (!token_cjson) {
            printf("Failed to get token_cjson\n");
            CommonJsonServer *temp_message = (CommonJsonServer *) malloc(sizeof(CommonJsonServer));
            strcpy(temp_message->info, "delete_user");
            strcpy(temp_message->msg, "failed");
            send_general_server_json(temp_message, client);
            free(temp_message);
            return NULL;
        }
        send_general_server_text(remove_user_by_token(input_json), client);   // 发送删除用户结果
        return NULL;
    }else if (strcmp(cmd->valuestring, "get_history") == 0) {  // get_history
        cJSON *token_cjson = cJSON_GetObjectItem(json_obj, "parm1");
        // 校验token是否正确
        if (check_token(token_cjson->valuestring) == NULL) {
            printf("Invalid token\n");
            CommonJsonServer *temp_message = (CommonJsonServer *) malloc(sizeof(CommonJsonServer));
            strcpy(temp_message->info, "login");
            strcpy(temp_message->msg, "failed");
            send_general_server_json(temp_message, client);
            free(temp_message);
            return NULL;
        }
        send_general_server_text(get_history_from_json(input_json), client);    // 发送聊天记录
        return get_history_from_json(input_json);
    } else if (strcmp(cmd->valuestring, "send_message") == 0) { // send_message
        cJSON *token_cjson = cJSON_GetObjectItem(json_obj, "parm1");
        cJSON *message = cJSON_GetObjectItem(json_obj, "parm2");
        // 校验token是否正确
        if (check_token(token_cjson->valuestring) == NULL) {
            printf("Invalid token\n");
            CommonJsonServer *temp_message = (CommonJsonServer *) malloc(sizeof(CommonJsonServer));
            strcpy(temp_message->info, "login");
            strcpy(temp_message->msg, "failed");
            send_general_server_json(temp_message, client);
            free(temp_message);
            return NULL;
        }
        // 添加消息到数据库
        char *username = check_token(token_cjson->valuestring);
        if (!username) {
            printf("Invalid token\n");
            return NULL;
        }
        strcpy(username_temp, username);
        strcpy(message_value_string, message->valuestring);
        SimpleMessageStruct *simple_message = (SimpleMessageStruct *) malloc(sizeof(SimpleMessageStruct));
        strcpy(simple_message->username, username_temp);
        strcpy(simple_message->message, message_value_string);
        add_message_to_json_file(CHAT_HISTORY_PATH, simple_message);
        // 发送消息给客户端
        CommonJsonServer *json_server = (CommonJsonServer *) malloc(sizeof(CommonJsonServer));
        strcpy(json_server->info, "send_message_status");
        strcpy(json_server->msg, "success");
        send_message_func(send_status_message_to_client(json_server), client->socket_fd);
        // 群发消息
        broadcast_message_func(send_message_to_client_by_struct(simple_message), client->socket_fd);
        return NULL;
    } else {
        printf("Invalid cmd\n");
        return input_json;
    }
}


#endif //OICQV2_AUTO_SERVER_TRANSFER_H
