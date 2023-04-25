//
// Created by HAIRONG ZHU on 2023/4/24.
//

#ifndef OICQV2_SERVER_SOCKET_SENDER_H
#define OICQV2_SERVER_SOCKET_SENDER_H

#include "basic_info/basic_definition.h"
#include "socket_tools/struct_json_transfer.h"

// 发送通用消息 info, msg Json
void send_general_server_json(CommonJsonServer *json_server, client_info *client) {
    char *json = send_status_message_to_client(json_server);
    send(client->socket_fd, json, strlen(json), 0);
    free(json);
}

void send_general_server_text(char *info, client_info *client) {
    send(client->socket_fd, info, strlen(info), 0);
}

#endif //OICQV2_SERVER_SOCKET_SENDER_H
