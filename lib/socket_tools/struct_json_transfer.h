//
// Created by HAIRONG ZHU on 2023/4/24.
//

#ifndef OICQV2_STRUCT_JSON_TRANSFER_H
#define OICQV2_STRUCT_JSON_TRANSFER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "basic_info/basic_definition.h"
#include "common_tools/common_tools.h"
#include "socket_tools/basic_struct_transfer.h"
#include "file_db_operate/operate_user_info.h"
#include "file_db_operate/operate_chat_history.h"

/*
 * 校验token是否存在，如果存在返回用户名
 * 输入 char *token , 输出 char *username
 * 不存在返回NULL
 */
char *check_token(char *token) {
    UserInfoStruct *user_info = (UserInfoStruct *) malloc(sizeof(UserInfoStruct) * MAX_USERS);
    // 从文件中读取用户信息
    char *username = NULL;
    if (read_users_from_json_file(USER_INFO_PATH, user_info, MAX_USERS) != -1) {
        // 验证用户名和密码
        for (int i = 0; i < MAX_USERS; i++) {
            if (strcmp(user_info[i].token, token) == 0 && strcmp(token, "") != 0) { // 这个地方一定要注意token有可能是空字符串
                username = strdup(user_info[i].username);
                free(user_info);
                return username;
            }
        }
        return username;
    } else {
        free(user_info);
        return NULL;
    }
}

/*
 * 用户注册
 * 输入json: "{\"cmd\":\"register\",\"parm1\":\"test\",\"parm2\":\"test\"}";
 * 输出json: "{\"info\":\"register\",\"msg\":\"token\"}";
 */
char *register_from_json(char *json) {
    CommonJsonClient *common_json = common_json_client_from_string(json);
    UserInfoStruct *user_info = (UserInfoStruct *) malloc(sizeof(UserInfoStruct));
    CommonJsonServer *common_json_server_response = (CommonJsonServer *) malloc(sizeof(CommonJsonServer));
    char temp_token[100];
    strcpy(user_info->username, common_json->parm1);
    strcpy(user_info->password, common_json->parm2);
    sha256_string_with_salt(user_info->password, user_info->username, temp_token);
    strcpy(user_info->token, temp_token);
    if (add_user_to_json_file(USER_INFO_PATH, user_info) == 0) {
        strcpy(common_json_server_response->info, "register");
        strcpy(common_json_server_response->msg, temp_token);
    } else {
        strcpy(common_json_server_response->info, "register");
        strcpy(common_json_server_response->msg, "failed");
    }
    free(user_info);
    return common_json_server_to_string(common_json_server_response);
}

/*
 * 用户登录，返回token
 * 输入json: "{\"cmd\":\"login\",\"parm1\":\"test\",\"parm2\":\"test\"}";
 * 输出json: "{\"info\":\"login\",\"msg\":\"token\"}";
 */
char *login_from_json(char *json) {
    CommonJsonClient *common_json = common_json_client_from_string(json);
    UserInfoStruct *user_info = (UserInfoStruct *) malloc(sizeof(UserInfoStruct));
    CommonJsonServer *common_json_server_response = (CommonJsonServer *) malloc(sizeof(CommonJsonServer));
    // 从文件中读取用户信息
    if (read_users_from_json_file(USER_INFO_PATH, user_info, MAX_USERS) != -1) {
        // 验证用户名和密码
        for (int i = 0; i < MAX_USERS; i++) {
            if (strcmp(user_info[i].username, common_json->parm1) == 0) {
                if (strcmp(user_info[i].password, common_json->parm2) == 0) {
                    strcpy(common_json_server_response->info, "login");
                    strcpy(common_json_server_response->msg, user_info[i].token);
                    free(user_info);
                    return common_json_server_to_string(common_json_server_response);
                }
            }
        }
        strcpy(common_json_server_response->info, "login");
        strcpy(common_json_server_response->msg, "failed");
        free(user_info);
        return common_json_server_to_string(common_json_server_response);
    } else {
        strcpy(common_json_server_response->info, "login");
        strcpy(common_json_server_response->msg, "failed");
        free(user_info);
        return common_json_server_to_string(common_json_server_response);
    }
}

/*
 * 更新用户密码
 * 输入json: "{\"cmd\":\"update_password\",\"parm1\":\"token\",\"parm2\":\"new_password\"}";
 * 输出json: "{\"info\":\"update_password\",\"msg\":\"success\"}" or "{\"info\":\"update_password\",\"msg\":\"failed\"}";
 */
char *update_password_by_token(char *json) {
    CommonJsonClient *common_json = common_json_client_from_string(json);
    CommonJsonServer *common_json_server_response = (CommonJsonServer *) malloc(sizeof(CommonJsonServer));
    char temp_token[100];
    char new_password[100];
    strcpy(temp_token, common_json->parm1);
    strcpy(new_password, common_json->parm2);

    char *username_temp = check_token(temp_token);

    if (change_password_in_json_file(USER_INFO_PATH, temp_token, new_password) == 0) {
        // 获取新的token
        char new_token[100];
        sha256_string_with_salt(new_password, username_temp, new_token);
        strcpy(common_json_server_response->info, "update_password");
        strcpy(common_json_server_response->msg, new_token);
    } else {
        strcpy(common_json_server_response->info, "update_password");
        strcpy(common_json_server_response->msg, "failed");
    }

    return common_json_server_to_string(common_json_server_response);
}


/*
 * 删除用户
 * 输入json: "{\"cmd\":\"delete_user\",\"parm1\":\"token\"}";
 * 输出json: "{\"info\":\"delete_user\",\"msg\":\"success\"}" or "{\"info\":\"delete_user\",\"msg\":\"failed\"}";
 */
char *remove_user_by_token(char *json) {
    CommonJsonClient *common_json = common_json_client_from_string(json);
    CommonJsonServer *common_json_server_response = (CommonJsonServer *) malloc(sizeof(CommonJsonServer));
    char token[100];
    strcpy(token, common_json->parm1);

    if (remove_user_from_json_file(USER_INFO_PATH, token) == 0) {
        strcpy(common_json_server_response->info, "delete_user");
        strcpy(common_json_server_response->msg, "success");
    } else {
        strcpy(common_json_server_response->info, "delete_user");
        strcpy(common_json_server_response->msg, "failed");
    }

    return common_json_server_to_string(common_json_server_response);
}



/*
 * 用户获取历史记录, 返回json
 * 输入json: "{\"cmd\":\"history\",\"parm1\":\"token\"}"; //测试 05a671c66aefea124cc08b76ea6d30bb
 * 输出json: {
    "info": "history",
    "msg": [
               {
                   "username": "kivvi",
                   "message": "HelloWorld!Message!"
               },
               {
                   "username": "kivvi",
                   "message": "HelloWorld!Message!"
               }
         ]
    }
 */
char *get_history_from_json(char *json) {
    CommonJsonClient *common_json = common_json_client_from_string(json);
    CommonJsonServer *common_json_server_response = (CommonJsonServer *) malloc(sizeof(CommonJsonServer));
    // 判断是否存在，存在继续，不存在返回错误
    if (check_token(common_json->parm1) != NULL) {
        // 从文件中读取历史记录
        SimpleMessageStruct *simple_message = (SimpleMessageStruct *) malloc(
                sizeof(SimpleMessageStruct) * MAX_MESSAGES);
        int count = read_messages_from_json_file(CHAT_HISTORY_PATH, simple_message, MAX_MESSAGES);
        if (count != -1) {
            // 生成json
            cJSON *root = cJSON_CreateObject();
            cJSON *info = cJSON_CreateString("history");
            cJSON *msg = cJSON_CreateArray();
            for (int i = 0; i < count; i++) {
                cJSON *item = cJSON_CreateObject();
                cJSON_AddItemToObject(item, "username", cJSON_CreateString(simple_message[i].username));
                cJSON_AddItemToObject(item, "message", cJSON_CreateString(simple_message[i].message));
                cJSON_AddItemToArray(msg, item);
            }
            cJSON_AddItemToObject(root, "info", info);
            cJSON_AddItemToObject(root, "msg", msg);
            char *response = cJSON_Print(root);
            cJSON_Delete(root);
            free(simple_message);
            return response;
        } else {
            strcpy(common_json_server_response->info, "history");
            strcpy(common_json_server_response->msg, "failed");
            free(simple_message);
            return common_json_server_to_string(common_json_server_response);
        }
    } else {
        strcpy(common_json_server_response->info, "history");
        strcpy(common_json_server_response->msg, "failed");
        return common_json_server_to_string(common_json_server_response);
    }
}

/*
 * 服务端发送消息给客户端, 输入结构体，转化为json
 * {
    "info": "send_message_status",
    "msg": "success"
}
 */
char *send_status_message_to_client(CommonJsonServer *json_server) {
    cJSON *root = cJSON_CreateObject();
    cJSON *info = cJSON_CreateString(json_server->info);
    cJSON *msg = cJSON_CreateString(json_server->msg);
    cJSON_AddItemToObject(root, "info", info);
    cJSON_AddItemToObject(root, "msg", msg);
    char *response = cJSON_Print(root);
    cJSON_Delete(root);
    return response;
}

/*
 * 用户消息转化为结构体
 * 输入json: "{\"cmd\":\"send\",\"parm1\":\"token\",\"parm2\":\"testMessage\"}"
 */
SimpleMessageStruct *get_message_from_json(char *json) {
    CommonJsonClient *common_json = common_json_client_from_string(json);
    SimpleMessageStruct *simple_message = (SimpleMessageStruct *) malloc(sizeof(SimpleMessageStruct));
    // 判断是否存在，存在继续，不存在返回错误
    if (check_token(common_json->parm1) != NULL) {
        strcpy(simple_message->username, check_token(common_json->parm1));
        strcpy(simple_message->message, common_json->parm2);
        return simple_message;
    } else {
        return NULL;
    }
}

/*
 * 获取历史记录json返回为结构体
 * 输入json: "{\"code\": 200, \"msg\": [{\"username\": \"kivvi\", \"message\": \"HelloWorld!Message!\"}, {\"username\": \"admin\", \"message\": \"YEAH!IGET!!!!!!!!!!!Message!\"}, {\"username\": \"admin\", \"message\": \"hello world哈哈哈😂\"}, {\"username\": \"kivvi\", \"message\": \"你好世界你好世界😂\"}, {\"username\": \"admin\", \"message\": \"好的，💬测试结束\"}]}";
 * 返回结构体列表, 输入
 * @param json_str json字符串
 * @param messages 结构体列表SimpleMessageStruct messages[MAX_MESSAGES];
 * @param max_messages 最大结构体数量
 * @return 返回结构体数量(消息数量) -1为失败
 */
int read_history_messages_from_json_string(const char *json_str, SimpleMessageStruct *messages, int max_messages) {
    cJSON *root = cJSON_Parse(json_str);
    if (root == NULL) {
        cJSON_Delete(root);
        return -1;
    }

    cJSON *msg = cJSON_GetObjectItem(root, "msg");
    int count = cJSON_GetArraySize(msg);
    if (count > max_messages) {
        count = max_messages;
    }

    for (int i = 0; i < count; i++) {
        cJSON *item = cJSON_GetArrayItem(msg, i);
        cJSON *username = cJSON_GetObjectItem(item, "username");
        cJSON *message = cJSON_GetObjectItem(item, "message");
        if (username != NULL && message != NULL) {
            strcpy(messages[i].username, username->valuestring);
            strcpy(messages[i].message, message->valuestring);
        }
    }

    cJSON_Delete(root);
    return count;
}

/*
 * 服务端发送消息给客户端, 输入结构体SimpleMessageStruct，转化为json
 * // 群发消息发给个人
{
    "info": "group_message",
    "msg": {
        "username": "admin",
        "message": "hello world哈哈哈😂"
    }
}
 */
char *send_message_to_client_by_struct(SimpleMessageStruct *simple_message) {
    cJSON *root = cJSON_CreateObject();
    cJSON *info = cJSON_CreateString("group_message");
    cJSON *msg = cJSON_CreateObject();
    cJSON_AddItemToObject(msg, "username", cJSON_CreateString(simple_message->username));
    cJSON_AddItemToObject(msg, "message", cJSON_CreateString(simple_message->message));
    cJSON_AddItemToObject(root, "info", info);
    cJSON_AddItemToObject(root, "msg", msg);
    char *response = cJSON_Print(root);
    cJSON_Delete(root);
    return response;
}

/*
 * 根据json消息转化为结构体SimpleMessageStruct
 * {
    "info": "group_message",
    "msg": {
        "username": "admin",
        "message": "hello world哈哈哈😂"
    }
}
 */
SimpleMessageStruct *get_message_from_json_string(char *json) {
    cJSON *root = cJSON_Parse(json);
    if (root == NULL) {
        cJSON_Delete(root);
        return NULL;
    }
    cJSON *info = cJSON_GetObjectItem(root, "info");
    cJSON *msg = cJSON_GetObjectItem(root, "msg");
    if (info != NULL && msg != NULL) {
        if (strcmp(info->valuestring, "group_message") == 0) {
            cJSON *username = cJSON_GetObjectItem(msg, "username");
            cJSON *message = cJSON_GetObjectItem(msg, "message");
            if (username != NULL && message != NULL) {
                SimpleMessageStruct *simple_message = (SimpleMessageStruct *) malloc(sizeof(SimpleMessageStruct));
                strcpy(simple_message->username, username->valuestring);
                strcpy(simple_message->message, message->valuestring);
                cJSON_Delete(root);
                return simple_message;
            }
        }
    }
    cJSON_Delete(root);
    return NULL;
}


#endif //OICQV2_STRUCT_JSON_TRANSFER_H
