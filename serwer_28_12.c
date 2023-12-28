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
#define MAX_USER 10

struct User {
    char name[NAME_DL];
    int id;
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

struct signal {
    long type;
    int odp; // 1-true; 0-false
};

int czyistnieje(struct User users[], int numUsers, int id) {
    for (int i=0; i < numUsers; ++i) {
        if (users[i].id == id) {
            return 0;
        }
    }
    return 1;
}


int main(int argc, char* argv[]) {
    int kolejka_logowanie = msgget(0x111, 0600 | IPC_CREAT);
    struct msglogin msg_logowanie;
    struct User users[MAX_USER];
    struct signal signal;
    int numUsers = 0;

    while(1) {
        msgrcv(kolejka_logowanie, &msg_logowanie, sizeof(msg_logowanie)-sizeof(long), 1, 0);
        if (czyistnieje(users, numUsers, msg_logowanie.id) == 0) {
            signal.type = 2;
            signal.odp = 0;
            msgsnd(kolejka_logowanie, &signal, sizeof(signal)-sizeof(long), 0);
            continue;
        }

        if(numUsers < MAX_USER) {
            struct User newUser;
            strcpy(newUser.name, msg_logowanie.name);
            newUser.id = msg_logowanie.id;
            users[numUsers++] = newUser;
            printf("Otrzymano od klienta:\nNazwa: %s\nIdentyfikator: %d\n", msg_logowanie.name, msg_logowanie.id);

        } else {
            printf("Za duzo userow, sory.. ;//");
        }

    }
    return 0;
}
