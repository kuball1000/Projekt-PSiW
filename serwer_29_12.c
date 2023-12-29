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
#define MAX_TEMAT 20

struct temat {
    long type;
    int id_topic;
    int id_type; //1-zwykla, 0-czasowa
    int ilejeszcze; //zwykla to wywalone, 0 - czytasz
};

struct User {
    char name[NAME_DL];
    int id;
    struct temat subskrypcje[MAX_TEMAT];
    int liczbasub;
};

struct msglogin {
    long type;
    char name[NAME_DL];
    int id;
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

struct odpowiedz {
    long type;
    int wybor; //1-subskrypcja 2-wyslanie wiadomosci, 3-
};

int main(int argc, char* argv[]) {
    int kolejka_logowanie = msgget(0x111, 0600 | IPC_CREAT);
    int subskrypcja = msgget(0x113, 0600 | IPC_CREAT);
    int odpowiedzi = msgget(0x114, 0600 | IPC_CREAT);
    struct msglogin msg_logowanie;
    struct User users[MAX_USER];
    struct signal signal;
    struct odpowiedz odpowiedz;
    int numUsers = 0;

    while(1) {
        msgrcv(odpowiedzi, &odpowiedz, sizeof(odpowiedz)-sizeof(long), 3,0);
        //printf("%d\n", odpowiedz.wybor);
        if(odpowiedz.wybor == 0) {
            msgrcv(kolejka_logowanie, &msg_logowanie, sizeof(msg_logowanie)-sizeof(long), 1, 0);
            if (czyistnieje(users, numUsers, msg_logowanie.id) == 0) {
                signal.type = 2;
                signal.odp = 0;
                msgsnd(kolejka_logowanie, &signal, sizeof(signal)-sizeof(long), 0);
                continue;
            } else {
                signal.type = 2;
                signal.odp = 1;
                msgsnd(kolejka_logowanie, &signal, sizeof(signal)-sizeof(long), 0);
            }

            if(numUsers < MAX_USER) {
                struct User newUser;
                strcpy(newUser.name, msg_logowanie.name);
                newUser.id = msg_logowanie.id;
                newUser.liczbasub = 0;
                users[numUsers++] = newUser;
                printf("Otrzymano od klienta:\nNazwa: %s\nIdentyfikator: %d\n", msg_logowanie.name, msg_logowanie.id);


            } else {
                printf("Za duzo userow, sory.. ;//\n");
            }
        }

        if(odpowiedz.wybor == 1) {
            struct temat nowa_subskrypcja;
            struct User konto_sub;
            msgrcv(subskrypcja, &konto_sub, sizeof(konto_sub)-sizeof(long), 5, 0);
            msgrcv(subskrypcja, &nowa_subskrypcja, sizeof(nowa_subskrypcja)-sizeof(long), 4, 0);
            printf("ID topic: %d,\n ID typ: %d,\n, Ile wiadomosci: %d,\n", nowa_subskrypcja.id_topic, nowa_subskrypcja.id_type, nowa_subskrypcja.ilejeszcze);
            printf("ID SUB %d\n", id_subskrybenta);

            for(int i = 0; i < numUsers; ++i) {
                if (id_subskrybenta == users[i].id) {
                    printf("ID %d\n", users[i].id);
                    users[i].subskrypcje[users[i].liczbasub] = nowa_subskrypcja;
                    users[i].liczbasub++;
                    printf("Liczba sub %d\n", users[i].liczbasub);
                    printf("%d\n", users[i].subskrypcje[users[i].liczbasub-1].id_topic);

                }
            }


        }



    }
    return 0;
}
