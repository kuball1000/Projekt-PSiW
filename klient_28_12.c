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

struct signal {
    long type;
    int odp; // 1-true; 0-false
};

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
    int leftmessage;
};

int main(int argc, char* argv[]) {
    int logowanie = msgget(0x111, 0600 | IPC_CREAT);
    struct msglogin moje_logowanie;
    struct signal signal;

    printf("Podaj swoją nazwę: ");
    fgets(moje_logowanie.name, sizeof(moje_logowanie.name), stdin);
    moje_logowanie.name[strcspn(moje_logowanie.name, "\n")] = '\0';

    printf("Podaj swoje id: ");
    scanf("%d", &moje_logowanie.id);

    moje_logowanie.type = 1;
    msgsnd(logowanie, &moje_logowanie, sizeof(moje_logowanie)-sizeof(long), 0);

    msgrcv(logowanie, &signal, sizeof(signal)-sizeof(long), 2, 0);

    if (signal.odp == 0) {
        printf("Zajete id");
        exit(1);
    }

    while(1) {
    }

    return 0;
}
