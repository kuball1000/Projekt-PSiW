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
    int logowanie = msgget(0x111, 0600 | IPC_CREAT);
    struct msglogin moje_logowanie;

    printf("Podaj swoją nazwę: ");
    fgets(moje_logowanie.name, sizeof(moje_logowanie.name), stdin);
    moje_logowanie.name[strcspn(moje_logowanie.name, "\n")] = '\0';

    moje_logowanie.id = getpid();

    moje_logowanie.type = 1;
    msgsnd(logowanie, &moje_logowanie, sizeof(moje_logowanie)-sizeof(long), 0);
    while(1) {
    }

    return 0;
}
