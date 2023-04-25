//
// Created by HAIRONG ZHU on 2023/4/24.
//
/*
 * 此代码用于结构体和 JSON 字符串之间的转换
 */

#ifndef OICQV2_BASIC_STRUCT_TRANSFER_H
#define OICQV2_BASIC_STRUCT_TRANSFER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON/cJSON.h"
#include "basic_info/basic_definition.h"

/*
 * 通用的 JSON 结构体
 * 用于将结构体转换为 JSON 字符串
 * @param common_json_client: 通用的 JSON 结构体
 * @return: JSON 字符串
 */
char *common_json_client_to_string(CommonJsonClient *common_json_client) {
    if (!common_json_client) {
        printf("Invalid input common_json_client\n");
        return NULL;
    }

    cJSON *json = cJSON_CreateObject();
    if (!json) {
        printf("Failed to create cJSON object\n");
        return NULL;
    }

    cJSON_AddStringToObject(json, "cmd", common_json_client->cmd);
    cJSON_AddStringToObject(json, "parm1", common_json_client->parm1);
    cJSON_AddStringToObject(json, "parm2", common_json_client->parm2);

    char *json_str = cJSON_Print(json);
    cJSON_Delete(json);

    return json_str;
}

/*
 * 通用的 JSON 结构体
 * 用于将 JSON 字符串转换为结构体
 * @param json_str: JSON 字符串
 * @return: 通用的 JSON 结构体
 */
CommonJsonClient *common_json_client_from_string(char *json_str) {
    if (!json_str) {
        printf("Invalid input json_str\n");
        return NULL;
    }

    cJSON *json = cJSON_Parse(json_str);
    if (!json) {
        printf("Failed to parse JSON: %s\n", cJSON_GetErrorPtr());
        return NULL;
    }

    CommonJsonClient *common_json = (CommonJsonClient *) malloc(sizeof(CommonJsonClient));
    if (!common_json) {
        printf("Failed to allocate memory for CommonJsonClient\n");
        cJSON_Delete(json);
        return NULL;
    }

    cJSON *cmd = cJSON_GetObjectItem(json, "cmd");
    if (cmd) {
        strcpy(common_json->cmd, cmd->valuestring);
    }

    cJSON *parm1 = cJSON_GetObjectItem(json, "parm1");
    if (parm1) {
        strcpy(common_json->parm1, parm1->valuestring);
    }

    cJSON *parm2 = cJSON_GetObjectItem(json, "parm2");
    if (parm2) {
        strcpy(common_json->parm2, parm2->valuestring);
    }

    cJSON_Delete(json);

    return common_json;
}

/*
 * 通用的 JSON 结构体
 * 用于将结构体转换为 JSON 字符串
 * @param common_json: 通用的 JSON 结构体
 * @return: JSON 字符串
 */
char *common_json_server_to_string(CommonJsonServer *common_json_server) {
    if (!common_json_server) {
        printf("Invalid input common_json_server\n");
        return NULL;
    }

    cJSON *json = cJSON_CreateObject();
    if (!json) {
        printf("Failed to create cJSON object\n");
        return NULL;
    }

    cJSON_AddStringToObject(json, "info", common_json_server->info);
    cJSON_AddStringToObject(json, "msg", common_json_server->msg);

    char *json_str = cJSON_Print(json);
    cJSON_Delete(json);

    return json_str;
}

/*
 * 通用的 JSON 结构体
 * 用于将 JSON 字符串转换为结构体
 * @param json_str: JSON 字符串
 * @return: 通用的 JSON 结构体
 */
CommonJsonServer *common_json_server_from_string(char *json_str) {
    if (!json_str) {
        printf("Invalid input json_str\n");
        return NULL;
    }

    cJSON *json = cJSON_Parse(json_str);
    if (!json) {
        printf("Failed to parse JSON: %s\n", cJSON_GetErrorPtr());
        return NULL;
    }

    CommonJsonServer *common_json_server = (CommonJsonServer *) malloc(sizeof(CommonJsonServer));
    if (!common_json_server) {
        printf("Failed to allocate memory for CommonJsonServer\n");
        cJSON_Delete(json);
        return NULL;
    }

    cJSON *info = cJSON_GetObjectItem(json, "info");
    if (info) {
        strcpy(common_json_server->info, info->valuestring);
    }

    cJSON *msg = cJSON_GetObjectItem(json, "msg");
    if (msg) {
        strcpy(common_json_server->msg, msg->valuestring);
    }

    cJSON_Delete(json);

    return common_json_server;
}

/*
 * 获取历史记录结构体列表从JSON
 * @param json_str: JSON 字符串
 * @param num_msg: 消息数量
 * @return: 历史记录结构体列表
 */
SimpleMessageStruct *json_to_history_struct_list(char *json_str, int *num_msg) {
    if (!json_str) {
        printf("Invalid input json_str\n");
        return NULL;
    }

    cJSON *json = cJSON_Parse(json_str);
    if (!json) {
        printf("Failed to parse JSON: %s\n", cJSON_GetErrorPtr());
        return NULL;
    }

    cJSON *msg_arr = cJSON_GetObjectItem(json, "msg");
    if (!msg_arr) {
        printf("Failed to get msg array\n");
        cJSON_Delete(json);
        return NULL;
    }

    int arr_size = cJSON_GetArraySize(msg_arr);
    if (arr_size == 0) {
        printf("Message array is empty\n");
        cJSON_Delete(json);
        return NULL;
    }

    SimpleMessageStruct *msg_list = (SimpleMessageStruct *) malloc(arr_size * sizeof(SimpleMessageStruct));
    if (!msg_list) {
        printf("Failed to allocate memory for SimpleMessageStruct array\n");
        cJSON_Delete(json);
        return NULL;
    }

    for (int i = 0; i < arr_size; i++) {
        cJSON *msg_obj = cJSON_GetArrayItem(msg_arr, i);
        if (!msg_obj) {
            printf("Failed to get message object at index %d\n", i);
            free(msg_list);
            cJSON_Delete(json);
            return NULL;
        }

        cJSON *username = cJSON_GetObjectItem(msg_obj, "username");
        cJSON *message = cJSON_GetObjectItem(msg_obj, "message");

        if (!username || !message) {
            printf("Failed to get username and message\n");
            free(msg_list);
            cJSON_Delete(json);
            return NULL;
        }

        strcpy(msg_list[i].username, username->valuestring);
        strcpy(msg_list[i].message, message->valuestring);
    }

    cJSON_Delete(json);

    *num_msg = arr_size;
    return msg_list;
}


/*
 * 将历史记录结构体列表转换为 JSON
 * @param messages: 历史记录结构体列表
 * @param num_messages: 消息数量
 * @return: JSON 字符串
 */
char *struct_list_to_json(SimpleMessageStruct *messages, int num_messages) {
    if (!messages || num_messages == 0) {
        printf("Invalid input struct list or number of messages\n");
        return NULL;
    }

    cJSON *json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "info", "history");

    cJSON *msg_arr = cJSON_CreateArray();

    for (int i = 0; i < num_messages; i++) {
        cJSON *msg_obj = cJSON_CreateObject();
        cJSON_AddStringToObject(msg_obj, "username", messages[i].username);
        cJSON_AddStringToObject(msg_obj, "message", messages[i].message);
        cJSON_AddItemToArray(msg_arr, msg_obj);
    }

    cJSON_AddItemToObject(json, "msg", msg_arr);

    char *json_str = cJSON_Print(json);
    cJSON_Delete(json);

    return json_str;
}


#endif //OICQV2_BASIC_STRUCT_TRANSFER_H
