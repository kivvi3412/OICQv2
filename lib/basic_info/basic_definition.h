//
// Created by HAIRONG ZHU on 2023/4/24.
//

#ifndef OICQV2_BASIC_DEFINITION_H
#define OICQV2_BASIC_DEFINITION_H

#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8888
#define MAX_CLIENTS 1000
#define BUFFER_SIZE_CLIENT 4096      // 缓冲区大小
struct client_info *clients[MAX_CLIENTS];    // 连接的客户端
int client_count = 0;    //连接客户端的数量
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;  // pthread锁, 可以不用init或者destroy进行初始化和销毁

#define MAX_USERS 1000
#define MAX_MESSAGES 1000
#define CHAT_HISTORY_PATH "../file_db/chat_history.json"
#define USER_INFO_PATH "../file_db/user_info.json"

// sockets用户结构体
typedef struct client_info {
    int socket_fd;           // 客户端连接的socket
    struct sockaddr_in address;    // 客户端地址信息
} client_info;

// 用户名和密码存储结构
typedef struct {
    char username[20];
    char password[20];
    char token[50];
} UserInfoStruct;

// 历史聊天记录存储结构, 单一聊天记录结构
typedef struct {
    char username[20];
    char message[4096];
} SimpleMessageStruct;

// 通用json格式, 客户端发送给服务器的json格式
typedef struct {
    char cmd[100];
    char parm1[4096];
    char parm2[4096];
} CommonJsonClient;

// 通用json格式, 服务器发送给客户端的json格式
typedef struct {
    char info[100];
    char msg[4096];
} CommonJsonServer;


#endif //OICQV2_BASIC_DEFINITION_H
