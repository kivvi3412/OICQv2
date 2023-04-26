#include "stdio.h"
//
// Created by zxb on 2023/4/26.
//

#ifndef OICQV2_DISPLAY_FUNCTION_H
#define OICQV2_DISPLAY_FUNCTION_H


// 失败通用窗口
void general_message_failed_menu(char *info) {
    //
    printf("\033c");
    printf("==========================================\n");
    printf("Ops! Error:%s happened!\n", info);
    printf("==========================================\n");
}

// 成功通用窗口
void general_message_success_menu(char *info) {
    printf("==========================================\n");
    printf("Congrats!: %s\n", info);
    printf("==========================================\n");
}

// 消息发送失败通用窗口
void send_message_failed_menu(char *info) {
    printf("\033c");
    printf("==========================================\n");
    printf("Ops! Error:%s happened!\n", info);
    printf("==========================================\n");
}

// 消息发送成功窗口
void send_message_success_menu() {
    printf("==========================================\n");
    printf("Congrats!: send massage success!\n");
    printf("==========================================\n");
}

// 服务端已关闭窗口
void server_closed_menu() {
    printf("\033c");
    printf("===================\n");
    printf("Server closed!\n");
    printf("===================\n");
}

// 初始化界面
void input_commend_menu() {
    printf("\033c");
    printf("=================================================================================\n");
    printf("                     Pls Enter the following commend to run:\n");
    printf("\n    1. /login[登陆]   2. /register[注册用户]   3. /update_password[更新用户密码]  \n");
    printf("     4. /delete[删除用户]     5. /history[加载历史记录]       6. /send[发送消息]  \n");
    printf("\n=================================================================================\n");
}

// 用户输入姓名
void name_input_menu() {
    printf("============================\n");
    printf("Enter your username -> ");
}

// 用户输入密码
void password_input_menu() {
    printf("============================\n");
    printf("Enter your password -> ");
}

//用户更新密码
void new_password_input_menu() {
    printf("============================\n");
    printf("Enter your new password -> ");
}

// 用户确认删除
void confirm_delete_menu() {
    printf("============================\n");
    printf("Enter delete to confirm -> ");
}

// abort
void abort_menu() {
    printf("============================\n");
    printf("           Abort\n");
    printf("============================\n");
}

// 获取历史记录
void show_history_messages_menu(SimpleMessageStruct *history_messages, int count) {
    printf("==================================\n");
    for (int i = 0; i < count; i++) {
        printf("[%s]: %s\n", history_messages[i].username, history_messages[i].message);
    }
    printf("===<history messages lod success>===\n");
}

// 输出单条消息
void show_message_menu(SimpleMessageStruct *message) {
    printf("[%s]: %s\n", message->username, message->message);
}

// 无效命令
void invalid_command() {
    printf("==================================\n");
    printf("Invalid command. Please try again!\n");
    printf("==================================\n");
}

// 无效输入
void invalid_input_the_input_json_menu() {
    printf("\033c");
    printf("=====================================\n");
    printf("Invalid input_json. Please try again!\n");
    printf("=====================================\n");
}

// 解析JSON失败
void failed_to_parse_JSON_menu(const char *str) {
    printf("\033c");
    printf("================================================\n");
    printf("Failed to parse JSON: %s. Please try again!\n", str);
    printf("================================================\n");
}

// 获取信息失败
void failed_to_get_info_menu() {
    printf("\033c");
    printf("=====================================\n");
    printf("Failed to get info. Please try again!\n");
    printf("=====================================\n");
}

// 获取消息失败
void failed_to_get_msg_menu() {
    printf("\033c");
    printf("=====================================\n");
    printf("Failed to get message. Please try again!\n");
    printf("=====================================\n");
}

#endif //OICQV2_DISPLAY_FUNCTION_H