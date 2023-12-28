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
    int topic_type; //1-zwykla, 0-czasowa
    int ilejeszcze;//zwykla to wywalone, 0 - czytasz
};

struct User {
    char name[NAME_DL];
    int id;
    struct temat subskrypcje[MAX_TEMAT];
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

int main(int argc, char* argv[]) {
    int logowanie = msgget(0x111, 0600 | IPC_CREAT);
    int subskrypcja = msgget(0x113, 0600 | IPC_CREAT);
    struct msglogin moje_logowanie;
    struct signal signal;
    struct User konto;


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

    konto.name = moje_logowanie.name;
    konto.id = moje_logowanie.id;

    while(1) {
        int opcja;
        printf("Wpisz 1-subskrypcje");
        scanf("%d", &opcja);

        if (opcja == 1){
            struct temat nowa_subskrypcja;
            printf("Podaj id tematu, ktory chcesz zasubowac: ");
            //open()
            //while read ...
            scanf("%d", &nowa_subskrypcja.id_topic);
            printf("Podaj jaka ma być to subskrypcja: 1-stała, 0-czasowa: ");
            scanf("%d", &nowa_subskrypcja.topic_type);

            if(nowa_subskrypcja.topic_type == 0) {
                printf("Ile chcesz wiadomości: ");
                scanf("%d", &nowa_subskrypcja.ilejeszcze);
            } else {
                nowa_subskrypcja.ilejeszcze = 0;
            }

            msgsnd(subskrypcja, &konto.id, sizeof(konto.id), 0);
            msgsnd(subskrypcja, &nowa_subskrypcja, sizeof(nowa_subskrypcja)-sizeof(long), 0);

        }


    }

    return 0;
}
