//
// Created by HAIRONG ZHU on 2023/4/24.
//

#ifndef OICQV2_OPERATE_USER_INFO_H
#define OICQV2_OPERATE_USER_INFO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON/cJSON.h"
#include "basic_info/basic_definition.h"


/*
 * 从 json 文件中读取用户信息
 * @param filename json文件名
 * @param users 用户信息数组
 * @param max_users 最大用户数
 * @return 成功读取的用户数, -1表示失败
 */
int read_users_from_json_file(const char *filename, UserInfoStruct *users, int max_users) {
    char *json_data;
    long json_size;
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("Failed to open file %s\n", filename);
        return -1;
    }

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

    int num_users = cJSON_GetArraySize(json_root);
    if (num_users > max_users) {
        printf("Too many users in json file, max_users=%d\n", max_users);
        num_users = max_users;
    }

    for (int i = 0; i < num_users; i++) {
        cJSON *json_user = cJSON_GetArrayItem(json_root, i);
        cJSON *json_username = cJSON_GetObjectItem(json_user, "username");
        cJSON *json_password = cJSON_GetObjectItem(json_user, "password");
        cJSON *json_token = cJSON_GetObjectItem(json_user, "token");
        if (!json_username || !json_password || !json_token) {
            printf("Invalid user data in json file\n");
            break;
        }
        strncpy(users[i].username, json_username->valuestring, sizeof(users[i].username));
        strncpy(users[i].password, json_password->valuestring, sizeof(users[i].password));
        strncpy(users[i].token, json_token->valuestring, sizeof(users[i].token));
    }

    cJSON_Delete(json_root);
    free(json_data);

    return num_users;
}

/*
 * 将用户信息写入 json 文件
 * @param filename json文件名
 * @param users 用户信息数组
 * @param num_users 用户数
 * @return 0成功写入, -1表示失败
 */
int write_users_to_json_file(const char *filename, const UserInfoStruct *users, int num_users) {
    cJSON *json_root = cJSON_CreateArray();
    if (!json_root) {
        printf("Failed to create json root\n");
        return -1;
    }

    for (int i = 0; i < num_users; i++) {
        cJSON *json_user = cJSON_CreateObject();
        if (!json_user) {
            printf("Failed to create json user object\n");
            cJSON_Delete(json_root);
            return -1;
        }
        cJSON_AddStringToObject(json_user, "username", users[i].username);
        cJSON_AddStringToObject(json_user, "password", users[i].password);
        cJSON_AddStringToObject(json_user, "token", users[i].token);
        cJSON_AddItemToArray(json_root, json_user);
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
 * 将用户信息添加到 json 文件
 * @param filename json文件名
 * @param user_info 用户信息
 * @return 0成功添加, -1表示失败
 */
int add_user_to_json_file(const char *filename, UserInfoStruct *user_info) {
    UserInfoStruct users[MAX_USERS];
    int num_users = read_users_from_json_file(filename, users, MAX_USERS);
    if (num_users == -1) {
        printf("Failed to read users from json file\n");
        return -1;
    }

    if (num_users >= MAX_USERS) {
        printf("Maximum number of users reached\n");
        return -1;
    }

    strncpy(users[num_users].username, user_info->username, sizeof(users[num_users].username));
    strncpy(users[num_users].password, user_info->password, sizeof(users[num_users].password));
    strncpy(users[num_users].token, user_info->token, sizeof(users[num_users].token));

    if (write_users_to_json_file(filename, users, num_users + 1) == -1) {
        printf("Failed to write users to json file\n");
        return -1;
    }

    return 0;
}

/*
 * 从 json 文件中删除用户信息
 * @param filename json文件名
 * @param token_temp 用户token
 * @return 0成功删除, -1表示失败
 */
int remove_user_from_json_file(const char *filename, const char *token) {
    UserInfoStruct users[MAX_USERS];
    int num_users = read_users_from_json_file(filename, users, MAX_USERS);
    if (num_users < 0) {
        printf("Failed to read users from json file\n");
        return -1;
    }

    int user_index = -1;
    for (int i = 0; i < num_users; i++) {
        if (strcmp(users[i].token, token) == 0) {
            user_index = i;
            break;
        }
    }

    if (user_index == -1) {
        printf("User not found\n");
        return -1;
    }

    // Shift remaining users to fill the gap
    for (int i = user_index; i < num_users - 1; i++) {
        users[i] = users[i + 1];
    }

    if (write_users_to_json_file(filename, users, num_users - 1) < 0) {
        printf("Failed to write users to json file\n");
        return -1;
    }

    return 0;
}

/*
 * 修改 json 文件中的用户密码
 * @param filename json文件名
 * @param token 用户token
 * @param new_password 新密码
 * @return 0成功修改, -1表示失败
 */
int change_password_in_json_file(const char *filename, const char *token, const char *new_password) {
    UserInfoStruct users[MAX_USERS];
    int num_users = read_users_from_json_file(filename, users, MAX_USERS);
    if (num_users < 0) {
        printf("Failed to read users from json file\n");
        return -1;
    }

    int user_index = -1;
    for (int i = 0; i < num_users; i++) {
        if (strcmp(users[i].token, token) == 0) {
            user_index = i;
            break;
        }
    }

    if (user_index == -1) {
        printf("User not found\n");
        return -1;
    }

    strncpy(users[user_index].password, new_password, sizeof(users[user_index].password));
    char temp_token[100];
    sha256_string_with_salt(users[user_index].password, users[user_index].username, temp_token);
    strcpy(users[user_index].token, temp_token);

    if (write_users_to_json_file(filename, users, num_users) < 0) {
        printf("Failed to write users to json file\n");
        return -1;
    }

    return 0;
}



/* examples:
   ******************************************************************************************
   read_users_from_json_file: 从 json 文件中读取用户信息
   UserInfoStruct users[MAX_USERS];
    int num_users = read_users_from_json_file("../file_db/user_info.json", users, MAX_USERS);
    if (num_users < 0) {
        printf("Failed to read users from json file\n");
        return -1;
    }
    printf("Read %d users from json file:\n", num_users);
    for (int i = 0; i < num_users; i++) {
        printf("  UserInfoStruct %d: username=%s, password=%s, token=%s\n", i+1, users[i].username, users[i].password, users[i].token);
    }

   ******************************************************************************************
   write_users_to_json_file: 将用户信息写入 json 文件
   UserInfoStruct users[3] = {
                   {"kivvi",    "000000",      "SDF@EFSADFSAG"},
                   {"zhangsan", "3412",        "WDSGWQFQWERSDFAS"},
                   {"lisi",     "password1234", "!DFASDFAWEWFADGREWFWQ"}
           };
   int num_users = sizeof(users) / sizeof(users[0]);
   if (write_users_to_json_file("../file_db/user_info.json", users, num_users) != 0) {
       printf("Failed to write users to json file\n");
       return -1;
   }
   printf("Successfully wrote %d users to json file\n", num_users);
   ******************************************************************************************
*/
#endif //OICQV2_OPERATE_USER_INFO_H
