//
// Created by HAIRONG ZHU on 2023/4/24.
//

#ifndef OICQV2_OPERATE_CHAT_HISTORY_H
#define OICQV2_OPERATE_CHAT_HISTORY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON/cJSON.h"
#include "basic_info/basic_definition.h"

/*
 * 从 json 文件中读取聊天记录
 * @param filename json文件名
 * @param messages 聊天记录数组
 * @param max_messages 最大聊天记录数
 * @return 读取的聊天记录数
 */
int read_messages_from_json_file(const char *filename, SimpleMessageStruct *messages, int max_messages) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("Failed to open file %s\n", filename);
        return -1;
    }

    char *json_data;
    long json_size;
    fseek(fp, 0, SEEK_END);
    json_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    json_data = (char *) malloc(json_size + 1);
    if (!json_data) {
        printf("Failed to allocate memory for json data\n");
        fclose(fp);
        return -1;
    }

    if (fread(json_data, json_size, 1, fp) != 1) {
        printf("Failed to read json data\n");
        free(json_data);
        fclose(fp);
        return -1;
    }
    fclose(fp);
    json_data[json_size] = '\0';

    cJSON *json_root = cJSON_Parse(json_data);
    if (!json_root) {
        printf("Failed to parse json data: %s\n", cJSON_GetErrorPtr());
        free(json_data);
        return -1;
    }

    int num_messages = cJSON_GetArraySize(json_root);
    if (num_messages > max_messages) {
        printf("Too many messages in json file, max_messages=%d\n", max_messages);
        num_messages = max_messages;
    }

    for (int i = 0; i < num_messages; i++) {
        cJSON *json_message = cJSON_GetArrayItem(json_root, i);
        cJSON *json_username = cJSON_GetObjectItem(json_message, "username");
        cJSON *json_message_text = cJSON_GetObjectItem(json_message, "message");
        if (!json_username || !json_message_text) {
            printf("Invalid message data in json file\n");
            break;
        }
        snprintf(messages[i].username, sizeof(messages[i].username), "%s", json_username->valuestring);
        snprintf(messages[i].message, sizeof(messages[i].message), "%s", json_message_text->valuestring);
    }

    cJSON_Delete(json_root);
    free(json_data);

    return num_messages;
}

/*
 * 将聊天记录写入 json 文件
 * @param filename json文件名
 * @param messages 聊天记录数组
 * @param num_messages 聊天记录数
 * @return 0 成功，-1 失败
 */
int write_messages_to_json_file(const char *filename, const SimpleMessageStruct *messages, int num_messages) {
    cJSON *json_root = cJSON_CreateArray();
    if (!json_root) {
        printf("Failed to create json root\n");
        return -1;
    }

    for (int i = 0; i < num_messages; i++) {
        cJSON *json_message = cJSON_CreateObject();
        if (!json_message) {
            printf("Failed to create json message object\n");
            cJSON_Delete(json_root);
            return -1;
        }
        cJSON_AddStringToObject(json_message, "username", messages[i].username);
        cJSON_AddStringToObject(json_message, "message", messages[i].message);
        cJSON_AddItemToArray(json_root, json_message);
    }

    char *json_data = cJSON_Print(json_root);
    if (!json_data) {
        printf("Failed to print json data\n");
        cJSON_Delete(json_root);
        return -1;
    }

    FILE *fp = fopen(filename, "w");
    if (!fp) {
        printf("Failed to open file %s\n", filename);
        cJSON_Delete(json_root);
        free(json_data);
        return -1;
    }

    fwrite(json_data, strlen(json_data), 1, fp);
    fclose(fp);

    cJSON_Delete(json_root);
    free(json_data);

    return 0;
}

/*
 * 添加单条聊天记录到文件
 * @param filename json文件名
 * @param message 聊天记录
 * @return 0 成功，-1 失败
 */
int add_message_to_json_file(const char *filename, SimpleMessageStruct *message) {
//    SimpleMessageStruct messages[MAX_MESSAGES];
    SimpleMessageStruct *messages = malloc(MAX_MESSAGES * sizeof(SimpleMessageStruct));
    int num_messages = read_messages_from_json_file(filename, messages, MAX_MESSAGES);
    if (num_messages < 0) {
        printf("Failed to read messages from json file %s\n", filename);
        return -1;
    }
    if (num_messages >= MAX_MESSAGES) {
        printf("Reached maximum number of messages in json file\n");
        return -1;
    }
    messages[num_messages] = *message;
    num_messages++;

    int result = write_messages_to_json_file(filename, messages, num_messages);
    if (result < 0) {
        printf("Failed to write messages to json file %s\n", filename);
        return -1;
    }

    return 0;
}


#endif //OICQV2_OPERATE_CHAT_HISTORY_H
