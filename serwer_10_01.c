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
    int topic_type; // 1-zwykla, 0-czasowa
    int ilejeszcze; // zwykla to wywalone, 0 - czytasz
};

struct User {
    long type;
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

struct odpowiedz {
    long type;
    int wybor; // 1-subskrypcja 2-wyslanie wiadomosci, 3-
};

struct wiadomosc {
    long type;
    char tekst[1024];
};

int czyistnieje(struct User users[], int numUsers, int id) {
    for (int i = 0; i < numUsers; ++i) {
        if (users[i].id == id) {
            return 0;
        }
    }
    return 1;
}

int main(int argc, char *argv[]) {
    int kolejka_logowanie = msgget(0x111, 0600 | IPC_CREAT);
    int subskrypcja = msgget(0x113, 0600 | IPC_CREAT);
    int odpowiedzi = msgget(0x114, 0600 | IPC_CREAT);
    int wszyscy = msgget(0x115, 0600 | IPC_CREAT);
    struct msglogin msg_logowanie;
    struct User users[MAX_USER];
    struct signal signal;
    struct odpowiedz odpowiedz;
    struct signal wyslij_sygnal;
    int numUsers = 0;
    int tematy[MAX_TEMAT];
    int numTemats = 0;
    struct wiadomosc nowa_wiadomosc;


    while (1) {
        wyslij_sygnal.odp = 0;
        strcpy(nowa_wiadomosc.tekst, "\n");
        msgrcv(odpowiedzi, &odpowiedz, sizeof(odpowiedz) - sizeof(long), 3, 0);
        //printf("%d\n", odpowiedz.wybor);
        if (odpowiedz.wybor == 0) {
            msgrcv(kolejka_logowanie, &msg_logowanie, sizeof(msg_logowanie) - sizeof(long), 1, 0);
            if (czyistnieje(users, numUsers, msg_logowanie.id) == 0) {
                signal.type = 2;
                signal.odp = 0;
                msgsnd(kolejka_logowanie, &signal, sizeof(signal) - sizeof(long), 0);
                continue;
            } else {
                signal.type = 2;
                signal.odp = 1;
                msgsnd(kolejka_logowanie, &signal, sizeof(signal) - sizeof(long), 0);
            }

            if (numUsers < MAX_USER) {
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

        if (odpowiedz.wybor == 1) { //subskrypcja dnego tematu
            struct temat nowa_subskrypcja;
            struct User konto_sub;
            msgrcv(subskrypcja, &konto_sub, sizeof(konto_sub) - sizeof(long), 5, 0);
            msgrcv(subskrypcja, &nowa_subskrypcja, sizeof(nowa_subskrypcja) - sizeof(long), 4, 0);
            printf("ID topic: %d,\n ID typ: %d,\n, Ile wiadomosci: %d,\n", nowa_subskrypcja.id_topic,
                   nowa_subskrypcja.topic_type, nowa_subskrypcja.ilejeszcze);
            printf("ID SUB %d\n", konto_sub.id);

            for (int i = 0; i < numUsers; ++i) {
                if (konto_sub.id == users[i].id) {
                    //printf("ID %d\n", users[i].id);
                    users[i].subskrypcje[users[i].liczbasub] = nowa_subskrypcja;
                    users[i].liczbasub++;
                    //printf("Liczba sub %d\n", users[i].liczbasub);
                    //printf("%d\n", users[i].subskrypcje[users[i].liczbasub - 1].id_topic);
                }
            }
        }
    if (odpowiedz.wybor == 2) { //tworzenie tematu
        struct temat nowy_temat;

        nowa_wiadomosc.type = 3;
        int istnieje = 0;
        msgrcv(subskrypcja, &nowy_temat, sizeof(nowy_temat) - sizeof(long), 1, 0);
        printf("%d \n", nowy_temat.id_topic);
        printf("%d \n", nowy_temat.topic_type);

        for (int i = 0; i < numTemats; i++) {
            if (tematy[i] == nowy_temat.id_topic) {
                strcpy(nowa_wiadomosc.tekst, "Nie udalo sie utowrzyc tematu, gdyz temat o tym id istnieje");
                msgsnd(subskrypcja, &nowa_wiadomosc, sizeof(nowa_wiadomosc) - sizeof(long), 0);
                istnieje = 1;
            }
        }

        if (istnieje == 0) {
            strcpy(nowa_wiadomosc.tekst, "Temat utworzony poprawnie");
            msgsnd(subskrypcja, &nowa_wiadomosc, sizeof(nowa_wiadomosc) - sizeof(long), 0);
            tematy[numTemats] = nowy_temat.id_topic;
            numTemats++;

            char stringowyTemat[10];
            sprintf(stringowyTemat, "%d",nowy_temat.id_topic);
            char x[1024] = "Zostal stworzony nowy temat o id: ";
            strcat(x, stringowyTemat);
            strcpy(nowa_wiadomosc.tekst, x);
            printf("%s\n", nowa_wiadomosc.tekst);
            //

            nowa_wiadomosc.type = 9;

            //wyslij_sygnal.type = 8;
            //wyslij_sygnal.odp = 1;
            //msgsnd(wszyscy, &wyslij_sygnal, sizeof(wyslij_sygnal) - sizeof(long), 0);
            for(int i = 0; i < numUsers; i++) {
                nowa_wiadomosc.type = users[i].id;
                msgsnd(wszyscy, &nowa_wiadomosc, sizeof(nowa_wiadomosc) - sizeof(long), 0);
            }
        }



    }




    }
    return 0;
}
