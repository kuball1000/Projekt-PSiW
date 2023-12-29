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

struct odpowiedz {
    long type;
    int wybor; //1-subskrypcja 2-wyslanie wiadomosci, 3-
};


int main(int argc, char* argv[]) {
    int logowanie = msgget(0x111, 0600 | IPC_CREAT);
    int subskrypcja = msgget(0x113, 0600 | IPC_CREAT);
    int odpowiedzi = msgget(0x114, 0600 | IPC_CREAT);
    struct msglogin moje_logowanie;
    struct signal signal;
    struct User konto;
    struct odpowiedz odpowiedz;

    odpowiedz.type = 3;
    odpowiedz.wybor = 0;
    msgsnd(odpowiedzi, &odpowiedz, sizeof(odpowiedz)-sizeof(long), 0);

    printf("Podaj swoją nazwę: \n");
    fgets(moje_logowanie.name, sizeof(moje_logowanie.name), stdin);
    moje_logowanie.name[strcspn(moje_logowanie.name, "\n")] = '\0';

    printf("Podaj swoje id: \n");
    scanf("%d", &moje_logowanie.id);

    moje_logowanie.type = 1;
    msgsnd(logowanie, &moje_logowanie, sizeof(moje_logowanie)-sizeof(long), 0);

    msgrcv(logowanie, &signal, sizeof(signal)-sizeof(long), 2, 0);

    if (signal.odp == 0) {
        printf("Zajete id\n");
        exit(1);
    }

    strcpy(konto.name, moje_logowanie.name);
    konto.id = moje_logowanie.id;


    while(1) {
        int opcja;
        printf("Wpisz 1-subskrypcje\n");
        scanf("%d", &opcja);
        odpowiedz.wybor = opcja;
        odpowiedz.type = 3;
        msgsnd(odpowiedzi, &odpowiedz, sizeof(odpowiedz)-sizeof(long), 0);


        if (odpowiedz.wybor == 1){
            struct temat nowa_subskrypcja;
            nowa_subskrypcja.type = 4;
            printf("Podaj id tematu, ktory chcesz zasubowac: \n");
            //open()
            //while read ...
            scanf("%d", &nowa_subskrypcja.id_topic);
            printf("Podaj jaka ma być to subskrypcja: 1-stała, 0-czasowa: \n");
            scanf("%d", &nowa_subskrypcja.topic_type);

            if(nowa_subskrypcja.topic_type == 0) {
                printf("Ile chcesz wiadomości: \n");
                scanf("%d", &nowa_subskrypcja.ilejeszcze);
            } else {
                nowa_subskrypcja.ilejeszcze = 0;
            }

            konto.type = 5;
            msgsnd(subskrypcja, &konto, sizeof(konto)-sizeof(long), 0);
            printf("KONTO ID %d\n", konto.id);
            msgsnd(subskrypcja, &nowa_subskrypcja, sizeof(nowa_subskrypcja)-sizeof(long), 0);

        }


    }

    return 0;
}
