#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

#define MSG_KEY_1 0x1234
#define MSG_KEY_2 0x5678
#define PERMS 0644
#define MAX_TEXT 100

struct message {
    long msg_type;
    char msg_text[MAX_TEXT];
};

void *send_message(void *arg) {
    int msgid = *(int *)arg;
    struct message msg;
    msg.msg_type = 1;

    while (1) {
        printf("Process2: Enter message: ");
        fgets(msg.msg_text, MAX_TEXT, stdin);
        msgsnd(msgid, &msg, sizeof(msg.msg_text), 0);
    }
}

void *receive_message(void *arg) {
    int msgid = *(int *)arg;
    struct message msg;

    while (1) {
        msgrcv(msgid, &msg, sizeof(msg.msg_text), 0, 0);
        printf("Process2 received: %s\n", msg.msg_text);
    }
}

int main() {
    int msgid_send = msgget(MSG_KEY_2, PERMS | IPC_CREAT);
    int msgid_recv = msgget(MSG_KEY_1, PERMS | IPC_CREAT);

    pthread_t send_thread, receive_thread;
    pthread_create(&send_thread, NULL, send_message, &msgid_send);
    pthread_create(&receive_thread, NULL, receive_message, &msgid_recv);

    pthread_join(send_thread, NULL);
    pthread_join(receive_thread, NULL);

    msgctl(msgid_send, IPC_RMID, NULL);
    msgctl(msgid_recv, IPC_RMID, NULL);

    return 0;
}
