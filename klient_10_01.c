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
#define MAX_ODBIERANE 3

struct temat {
    long type;
    int id_topic;
    int topic_type; //1-zwykla, 0-czasowa, 2-tworzenie
    int ilejeszcze;//zwykla to 1000000, liczba - czytasz
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

struct wiadomosc {
    long type;
    char tekst[1024];
};

struct wiadomosc_tematyczna {
    long type;
    char tekst[1024];
    int id_wysylanego;
    int id_subskrypcji;
    int priorytet; //1-10
};


int main(int argc, char* argv[]) {
    int logowanie = msgget(0x111, 0600 | IPC_CREAT);
    int subskrypcja = msgget(0x113, 0600 | IPC_CREAT);
    int odpowiedzi = msgget(0x114, 0600 | IPC_CREAT);
    int wszyscy = msgget(0x115, 0600 | IPC_CREAT);
    int do_banow = msgget(0x117, 0600 | IPC_CREAT);
    int wysylanie_wiadomosci = msgget(0x116, 0600 | IPC_CREAT);
    struct msglogin moje_logowanie;
    struct signal signal;
    struct User konto;
    struct signal odpowiedz;
    struct signal nadawca;

    odpowiedz.type = 1;
    odpowiedz.odp = 0;
    msgsnd(odpowiedzi, &odpowiedz, sizeof(odpowiedz)-sizeof(long), 0);

    printf("Podaj swoją nazwę: \n");
    fgets(moje_logowanie.name, sizeof(moje_logowanie.name), stdin);
    moje_logowanie.name[strcspn(moje_logowanie.name, "\n")] = '\0';

    printf("Podaj swoje id (xxx): \n");
    scanf("%d", &moje_logowanie.id);
    if (moje_logowanie.id < 100 || moje_logowanie.id > 999){
        printf("Złe ID\n");
        exit(1);
    }

    moje_logowanie.type = 1;
    msgsnd(logowanie, &moje_logowanie, sizeof(moje_logowanie)-sizeof(long), 0);

    msgrcv(logowanie, &signal, sizeof(signal)-sizeof(long), konto.id, 0);

    if (signal.odp == 0) {
        printf("Zajete id\n");
        exit(1);
    }

    strcpy(konto.name, moje_logowanie.name);
    konto.id = moje_logowanie.id;
    nadawca.type = 1;
    nadawca.odp = konto.id;


    while(1) {
        int opcja;
        struct wiadomosc wiadomosc_serwera;
        struct signal nowa_wiadomosc_serwera;
        struct wiadomosc_tematyczna wiadomosc_wysylana_od_serwera;

        strcpy(wiadomosc_serwera.tekst, "\n");
        strcpy(wiadomosc_wysylana_od_serwera.tekst, "\n");

        msgrcv(wszyscy, &wiadomosc_serwera, sizeof(wiadomosc_serwera) - sizeof(long), konto.id, IPC_NOWAIT);
        printf("%s\n", wiadomosc_serwera.tekst);
        strcpy(wiadomosc_serwera.tekst, "\n");


        for(int i=0; i < MAX_ODBIERANE; i++) {
            if (msgrcv(wszyscy, &wiadomosc_wysylana_od_serwera, sizeof(wiadomosc_wysylana_od_serwera) - sizeof(long), konto.id, IPC_NOWAIT) != -1) {
            printf("Otrzymałeś wiadomosc z tematu: %d\n", wiadomosc_wysylana_od_serwera.id_subskrypcji);
            printf("%s\n", wiadomosc_wysylana_od_serwera.tekst);
            strcpy(wiadomosc_wysylana_od_serwera.tekst, "");
        }
        }

        printf("Wpisz 1-subskrypcje tematu; 2-tworzenie tematu; 3-wysylanie wiadomosc; 4-odbieranie asynchroniczne; 5-zbanuj dziada; 9-odswiez\n");

        scanf("%d", &opcja);
        odpowiedz.odp = opcja;
        odpowiedz.type = 1;
        msgsnd(odpowiedzi, &odpowiedz, sizeof(odpowiedz)-sizeof(long), 0);


        if (odpowiedz.odp == 1) {
            struct temat nowa_subskrypcja;
            nowa_subskrypcja.type = 1;
            msgsnd(subskrypcja, &nadawca, sizeof(nadawca) - sizeof(long), 0);
            printf("Oto istniejące tematy: \n");
            msgrcv(subskrypcja, &wiadomosc_serwera, sizeof(wiadomosc_serwera)-sizeof(long), konto.id, 0);
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
            konto.type = 1;
            msgsnd(subskrypcja, &konto, sizeof(konto) - sizeof(long), 0);
            msgsnd(subskrypcja, &nowa_subskrypcja, sizeof(nowa_subskrypcja) - sizeof(long), 0);
            msgrcv(subskrypcja, &wiadomosc_serwera, sizeof(wiadomosc_serwera) - sizeof(long), konto.id, 0);
            printf("%s\n", wiadomosc_serwera.tekst);
            strcpy(wiadomosc_serwera.tekst, "");

        }

    if (odpowiedz.odp == 2) {
        struct temat nowy_temat;
        struct wiadomosc nowa_wiadomosc;
        nowy_temat.type = 1;
        printf("Podaj id tematu, ktory chcesz stowrzyc: \n");
        scanf("%d", &nowy_temat.id_topic);
        nowy_temat.topic_type = 2; //specjalny topic type do tworzenia tematu
        nowy_temat.ilejeszcze = 0;
        msgsnd(subskrypcja, &nowy_temat, sizeof(nowy_temat) - sizeof(long), 0);

        msgrcv(wszyscy, &nowa_wiadomosc, sizeof(nowa_wiadomosc) - sizeof(long), konto.id, 0);
        printf("%s\n", nowa_wiadomosc.tekst);
    }

    if (odpowiedz.odp == 3) {
        struct wiadomosc_tematyczna wysylana_wiadomosc;
        struct wiadomosc sukces_wyslania;
        msgsnd(wysylanie_wiadomosci, &nadawca, sizeof(nadawca) - sizeof(long), 0);
        printf("Oto istniejące tematy: \n");
        msgrcv(wysylanie_wiadomosci, &wiadomosc_serwera, sizeof(wiadomosc_serwera)-sizeof(long), konto.id, 0);
        printf("%s\n", wiadomosc_serwera.tekst);
        strcpy(wiadomosc_serwera.tekst, "");
        printf("Na jaki temat chcesz wyslać wiadomość? \n");
        //int id_tematu;
        scanf("%d", &wysylana_wiadomosc.id_subskrypcji);
        //wysylana_wiadomosc.type = id_tematu;
        printf("Jaką wiadomość chcesz wysłać? \n");
        scanf("%s", wysylana_wiadomosc.tekst);
        printf("Podaj priorytet z jakim wiadomość ma zostać nadana (1-10):\n");
        printf("1-najszybciej, 10-najwolniej\n");
        scanf("%d", &wysylana_wiadomosc.priorytet);

        wysylana_wiadomosc.type = 1;
        wysylana_wiadomosc.id_wysylanego = konto.id;
        msgsnd(wysylanie_wiadomosci, &wysylana_wiadomosc, sizeof(wysylana_wiadomosc) - sizeof(long), 0);
        strcpy(wysylana_wiadomosc.tekst, "");
        msgrcv(wysylanie_wiadomosci, &sukces_wyslania, sizeof(sukces_wyslania) - sizeof(long), konto.id, 0);
        printf("%s\n", sukces_wyslania.tekst);

    }

    if (odpowiedz.odp == 4) {
        int liczba_odebranych = 0;
        struct wiadomosc_tematyczna wiadomosci_odebrane[128];
        while(1) {
            if (msgrcv(wszyscy, &wiadomosc_wysylana_od_serwera, sizeof(wiadomosc_wysylana_od_serwera) - sizeof(long), konto.id, IPC_NOWAIT) != -1) {
                wiadomosci_odebrane[liczba_odebranych] = wiadomosc_wysylana_od_serwera;
                liczba_odebranych++;
            } else {
                break;
            }
        }
        for (int p = 1; p < 11; p++) {
            for(int i=0; i < liczba_odebranych; i++) {
                if (wiadomosci_odebrane[i].priorytet == p) {
                    printf("Otrzymałeś wiadomosc z tematu: %d\n", wiadomosci_odebrane[i].id_subskrypcji);
                    printf("%s\n", wiadomosci_odebrane[i].tekst);
                    strcpy(wiadomosci_odebrane[i].tekst, "\n");
                }

            }
        }

    }

    if (odpowiedz.odp == 5) {
        struct wiadomosc wiadomosc_banowana;
        struct signal ban;
        ban.type = 1;
        ban.odp = konto.id; //Przesłanie id użytkownika
        msgsnd(do_banow, &ban, sizeof(ban) - sizeof(long), 0);
        printf("Oto wszyscy użytkownicy: \n");
        msgrcv(do_banow, &wiadomosc_banowana, sizeof(wiadomosc_banowana) - sizeof(long), konto.id, 0);
        printf("%s\n", wiadomosc_banowana.tekst);
        printf("Podaj id użytkownika, którego chcesz zbanować: \n");
        scanf("%d", &ban.odp);
        ban.type = 1;
        msgsnd(do_banow, &ban, sizeof(ban) - sizeof(long), 0);

    }

    if (odpowiedz.odp == 9) {
        continue;

    }



    }

    return 0;
}
