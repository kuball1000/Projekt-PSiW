#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/time.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#define NAME_DL 128

struct msglogin {
    long type;
    char name[NAME_DL];
    int id;
};

struct msgsubscription {
    long type;
    char name[NAME_DL];
    int id;
    int id_topic;
    int type;
    int leftmessage;
};

int main(int argc, char* argv[]) {
    int kolejka_logowanie = msgget(0x111, 0600 | IPC_CREAT);
    struct msglogin msg_logowanie;

    while(1) {

        msgrcv(kolejka_logowanie, &msg_logowanie, sizeof(msg_logowanie)-sizeof(long), 1, 0);
        printf("Otrzymano od klienta:\nNazwa: %s\nIdentyfikator: %d\n", msg_logowanie.name, msg_logowanie.id);

    }
    return 0;
}
