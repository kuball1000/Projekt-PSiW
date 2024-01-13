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
#include <errno.h>


#define NAME_DL 128
#define MAX_USER 10
#define MAX_TEMAT 20

struct temat {
    long type;
    int id_topic;
    int topic_type; //1-zwykla, 0-czasowa, 2-tworzenie
    int ilejeszcze;//zwykla to wywalone, 0 - czytasz
};

struct User {
    long type;
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

struct wiadomosc {
    long type;
    char tekst[1024];
};

struct wiadomosc_tematyczna {
    long type;
    char tekst[1024];
    int id_wysylanego;
    int id_subskrypcji;
};


int main(int argc, char* argv[]) {
    int logowanie = msgget(0x111, 0600 | IPC_CREAT);
    int subskrypcja = msgget(0x113, 0600 | IPC_CREAT);
    int odpowiedzi = msgget(0x114, 0600 | IPC_CREAT);
    int wszyscy = msgget(0x115, 0600 | IPC_CREAT);
    int wysylanie_wiadomosci = msgget(0x116, 0600 | IPC_CREAT);
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
        struct wiadomosc wiadomosc_serwera;
        struct signal nowa_wiadomosc_serwera;
        struct wiadomosc_tematyczna wiadomosc_wysylana_od_serwera;


        msgrcv(wszyscy, &wiadomosc_serwera, sizeof(wiadomosc_serwera) - sizeof(long), konto.id, IPC_NOWAIT);
        printf("%s\n", wiadomosc_serwera.tekst);
        strcpy(wiadomosc_serwera.tekst, "\n");


        //for(int i=0; i < 3; i++) {
            msgrcv(wysylanie_wiadomosci, &wiadomosc_wysylana_od_serwera, sizeof(wiadomosc_wysylana_od_serwera) - sizeof(long), konto.id, IPC_NOWAIT);
           // if(errno == ENOMSG) {
            //    break;
            //}
            printf("Otrzymałeś wiadomosc z tematu: %d\n", wiadomosc_wysylana_od_serwera.id_subskrypcji);
            printf("%s\n", wiadomosc_wysylana_od_serwera.tekst);
            strcpy(wiadomosc_wysylana_od_serwera.tekst, "\n");
        //}

        printf("Wpisz 1-subskrypcje tematu; 2-tworzenie tematu; 3-wysylanie wiadomosc; 9-odswiez\n");

        scanf("%d", &opcja);
        odpowiedz.wybor = opcja;
        odpowiedz.type = 3;
        msgsnd(odpowiedzi, &odpowiedz, sizeof(odpowiedz)-sizeof(long), 0);


        if (odpowiedz.wybor == 1){
            struct temat nowa_subskrypcja;
            nowa_subskrypcja.type = 4;
            printf("Oto istniejące tematy: \n");
            msgrcv(subskrypcja, &wiadomosc_serwera, sizeof(wiadomosc_serwera)-sizeof(long), 7, 0);
            printf("%s\n", wiadomosc_serwera.tekst);
            strcpy(wiadomosc_serwera.tekst, "");
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
                nowa_subskrypcja.ilejeszcze = 1000000;
            }
            konto.type = 5;
            msgsnd(subskrypcja, &konto, sizeof(konto)-sizeof(long), 0);
            msgsnd(subskrypcja, &nowa_subskrypcja, sizeof(nowa_subskrypcja)-sizeof(long), 0);
            msgrcv(subskrypcja, &wiadomosc_serwera, sizeof(wiadomosc_serwera) - sizeof(long), 7, 0);
            printf("%s\n", wiadomosc_serwera.tekst);
            strcpy(wiadomosc_serwera.tekst, "");

        }

    if (odpowiedz.wybor == 2) {
        struct temat nowy_temat;
        struct wiadomosc nowa_wiadomosc;
        nowy_temat.type = 1;
        printf("Podaj id tematu, ktory chcesz stowrzyc: \n");
        scanf("%d", &nowy_temat.id_topic);
        nowy_temat.topic_type = 2;
        nowy_temat.ilejeszcze = 0;
        msgsnd(subskrypcja, &nowy_temat, sizeof(nowy_temat) - sizeof(long), 0);

        msgrcv(subskrypcja, &nowa_wiadomosc, sizeof(nowa_wiadomosc) - sizeof(long), 3, 0);
        printf("%s\n", nowa_wiadomosc.tekst);



    }

    if (odpowiedz.wybor == 3) {
        struct wiadomosc_tematyczna wysylana_wiadomosc;
        printf("Oto istniejące tematy: \n");
        msgrcv(wysylanie_wiadomosci, &wiadomosc_serwera, sizeof(wiadomosc_serwera)-sizeof(long), 7, 0);
        printf("%s\n", wiadomosc_serwera.tekst);
        strcpy(wiadomosc_serwera.tekst, "");
        printf("Na jaki temat chcesz wyslać wiadomość? \n");
        //int id_tematu;
        scanf("%d", &wysylana_wiadomosc.id_subskrypcji);
        //wysylana_wiadomosc.type = id_tematu;
        printf("Jaką wiadomość chcesz wysłać? \n");
        scanf("%s", wysylana_wiadomosc.tekst);
        wysylana_wiadomosc.type = 9;
        wysylana_wiadomosc.id_wysylanego = konto.id;
        msgsnd(wysylanie_wiadomosci, &wysylana_wiadomosc, sizeof(wysylana_wiadomosc) - sizeof(long), 0);
        strcpy(wysylana_wiadomosc.tekst, "");





    }

    if (odpowiedz.wybor == 9) {
        continue;

    }



    }

    return 0;
}
