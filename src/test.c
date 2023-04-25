#include "stdio.h"
#include "file_db_operate/operate_chat_history.h"
#include "pthread.h"

void *thread_1(void *arg) {
    SimpleMessageStruct *message = malloc(sizeof(SimpleMessageStruct));
    strcpy(message->username, "zhr");
    strcpy(message->message, "hello world");
    add_message_to_json_file(CHAT_HISTORY_PATH, message);
}

void *thread_2(void *arg) {
//    SimpleMessageStruct messages[MAX_MESSAGES];
    SimpleMessageStruct *messages = malloc(MAX_MESSAGES * sizeof(SimpleMessageStruct));
    int num_messages = read_messages_from_json_file(CHAT_HISTORY_PATH, messages, MAX_MESSAGES);
    for (int i = 0; i < num_messages; i++) {
        printf("%s: %s\n", messages[i].username, messages[i].message);
    }
}

int main() {
//    pthread_t tid1;
//    pthread_create(&tid1, NULL, (void *) thread_1, NULL);
//    pthread_join(tid1, NULL);
//    thread_1(NULL);

    pthread_t tid1;
    pthread_create(&tid1, NULL, thread_2, NULL);
    pthread_join(tid1, NULL);
//    thread_2(NULL);

}