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
    int topic_type; // 1-zwykla, 0-czasowa, 2-tworzysz
    int ilejeszcze; // zwykla to 1000000, liczba - czytasz
};

struct User {
    long type;
    char name[NAME_DL];
    int id;
    struct temat subskrypcje[MAX_TEMAT];
    int liczbasub;
    int zbanowani[MAX_USER];
    int liczbazbnowanych;
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

struct wiadomosc_tematyczna {
    long type;
    char tekst[1024];
    int id_wysylanego;
    int id_subskrypcji;
    int priorytet; //1-10
};


int czyistnieje(struct User users[], int numUsers, int id) {
    for (int i = 0; i < numUsers; ++i) {
        if (users[i].id == id) {
            return 0;
        }
    }
    return 1;
}

char* intArrayToString(int *array, int size) {
    int maxLength = size * (11 + 1); // 11 for int, 1 for newlin
    char *result = malloc(maxLength);

    result[0] = '\0'; // Start with an empty string

    for (int i = 0; i < size; i++) {
        char buffer[12]=""; // Temporary buffer
        sprintf(buffer, "%d\t", array[i]); // Convert int to string with newline
        strcat(result, buffer); // Concatenate to the result
    }

    return result;
}



int main(int argc, char *argv[]) {
    int kolejka_logowanie = msgget(0x111, 0600 | IPC_CREAT);
    int subskrypcja = msgget(0x113, 0600 | IPC_CREAT);
    int odpowiedzi = msgget(0x114, 0600 | IPC_CREAT);
    int wszyscy = msgget(0x115, 0600 | IPC_CREAT);
    int wysylanie_wiadomosci = msgget(0x116, 0600 | IPC_CREAT);
    int do_banow = msgget(0x117, 0600 | IPC_CREAT);
    struct msglogin msg_logowanie;
    struct User users[MAX_USER];
    struct signal signal;
    struct odpowiedz odpowiedz;
    struct signal wyslij_sygnal;
    struct signal nadawca;
    int numUsers = 0;
    int tematy[MAX_TEMAT];
    int numTemats = 0;
    struct wiadomosc nowa_wiadomosc;


    while (1) {
        wyslij_sygnal.odp = 0;
        strcpy(nowa_wiadomosc.tekst, "\n");
        msgrcv(odpowiedzi, &odpowiedz, sizeof(odpowiedz) - sizeof(long), 1, 0);
        //printf("%d\n", odpowiedz.wybor);
        if (odpowiedz.wybor == 0) {
            msgrcv(kolejka_logowanie, &msg_logowanie, sizeof(msg_logowanie) - sizeof(long), 1, 0);
            if (czyistnieje(users, numUsers, msg_logowanie.id) == 0) {
                signal.type = msg_logowanie.id;
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
                newUser.liczbazbnowanych = 0;
                users[numUsers++] = newUser;
                printf("Otrzymano od klienta:\nNazwa: %s\nIdentyfikator: %d\n", msg_logowanie.name, msg_logowanie.id);

            } else {
                printf("Za duzo userow, sory.. ;//\n");
            }
        }

        if (odpowiedz.wybor == 1) { //subskrypcja dnego tematu
            struct temat nowa_subskrypcja;
            struct User konto_sub;
            msgrcv(subskrypcja, &nadawca, sizeof(nadawca) - sizeof(long), 1, 0);
            char *string_tematy = intArrayToString(tematy, numTemats);
            strcpy(nowa_wiadomosc.tekst, string_tematy);
            printf("%s", nowa_wiadomosc.tekst);
            nowa_wiadomosc.type = nadawca.odp;
            msgsnd(subskrypcja, &nowa_wiadomosc, sizeof(nowa_wiadomosc) - sizeof(long), 0);
            msgrcv(subskrypcja, &konto_sub, sizeof(konto_sub) - sizeof(long), 1, 0);
            msgrcv(subskrypcja, &nowa_subskrypcja, sizeof(nowa_subskrypcja) - sizeof(long), 1, 0);

            for (int i=0; i < numTemats; i++) {
                printf("%d %d\n", nowa_subskrypcja.id_topic, tematy[i]);
                if (nowa_subskrypcja.id_topic == tematy[i]) {
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
                            strcpy(nowa_wiadomosc.tekst, "Udało się zasubskrybować");


                }
            } break;
                } else {
                    strcpy(nowa_wiadomosc.tekst, "Nie udało się zasubskrybować - podany temat nie istnieje ;(");


                }
            }
            msgsnd(subskrypcja, &nowa_wiadomosc, sizeof(nowa_wiadomosc)-sizeof(long), 0);


        }
    if (odpowiedz.wybor == 2) { //tworzenie tematu
        struct temat nowy_temat;

        nowa_wiadomosc.type = nadawca.odp;
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

            char stringowyTemat[10]="";
            sprintf(stringowyTemat, "%d",nowy_temat.id_topic);
            char x[1024] = "Zostal stworzony nowy temat o id: ";
            strcat(x, stringowyTemat);
            strcpy(nowa_wiadomosc.tekst, x);
            printf("%s\n", nowa_wiadomosc.tekst);

            for(int i = 0; i < numUsers; i++) {
                nowa_wiadomosc.type = users[i].id;
                msgsnd(wszyscy, &nowa_wiadomosc, sizeof(nowa_wiadomosc) - sizeof(long), 0);
            }
            strcpy(nowa_wiadomosc.tekst, "");
        }
    }

        if (odpowiedz.wybor == 3) {
            struct wiadomosc_tematyczna wiadomosc_wysylana_od_serwera;
            struct wiadomosc_tematyczna wysylana_wiadomosc;
            struct wiadomosc sukces_wyslania;
            char *string_tematy = intArrayToString(tematy, numTemats);
            msgrcv(wysylanie_wiadomosci, &nadawca, sizeof(nadawca) - sizeof(long), 1, 0);
            strcpy(nowa_wiadomosc.tekst, string_tematy);
            printf("%s\n", nowa_wiadomosc.tekst);
            nowa_wiadomosc.type = nadawca.odp;
            msgsnd(wysylanie_wiadomosci, &nowa_wiadomosc, sizeof(nowa_wiadomosc) - sizeof(long), 0);
            msgrcv(wysylanie_wiadomosci, &wysylana_wiadomosc, sizeof(wysylana_wiadomosc) - sizeof(long), 1, 0);
            int temat_istnieje = 0;
            int dobry_priorytet = 0;


            printf("%s\n", wysylana_wiadomosc.tekst);
            for(int a = 0; a < numTemats; a++) {
                if(wysylana_wiadomosc.id_subskrypcji == tematy[a]) {
                    temat_istnieje = 1;
                    if(wysylana_wiadomosc.priorytet > 0 && wysylana_wiadomosc.priorytet < 11) {
                        dobry_priorytet = 1;
                    }
                }
            }

            if (temat_istnieje && dobry_priorytet) {
                strcpy(sukces_wyslania.tekst, "Wiadomosc została wysłana");
            for(int i = 0; i < numUsers; i++) {
                for(int j=0; j < users[i].liczbasub; j++) {
                    if (wysylana_wiadomosc.id_subskrypcji == users[i].subskrypcje[j].id_topic && wysylana_wiadomosc.id_wysylanego != users[i].id) {
                        int czy_zbanowany = 0;
                        strcpy(wiadomosc_wysylana_od_serwera.tekst, wysylana_wiadomosc.tekst);
                        wiadomosc_wysylana_od_serwera.id_subskrypcji = wysylana_wiadomosc.id_subskrypcji;
                        wiadomosc_wysylana_od_serwera.id_wysylanego = wysylana_wiadomosc.id_wysylanego;
                        wiadomosc_wysylana_od_serwera.priorytet = wysylana_wiadomosc.priorytet;
                        wiadomosc_wysylana_od_serwera.type = users[i].id;
                        for(int k=0; k < users[i].liczbazbnowanych; k++) {
                            if (users[i].zbanowani[k] == wiadomosc_wysylana_od_serwera.id_wysylanego) {
                                czy_zbanowany = 1;
                            }
                        }

                        if (czy_zbanowany == 0) {
                            if (users[i].subskrypcje[j].topic_type == 1) {
                                msgsnd(wszyscy, &wiadomosc_wysylana_od_serwera, sizeof(wiadomosc_wysylana_od_serwera) - sizeof(long), 0);
                            } else if(users[i].subskrypcje[j].topic_type == 0) {
                                if(users[i].subskrypcje[j].ilejeszcze > 0) {
                                msgsnd(wszyscy, &wiadomosc_wysylana_od_serwera, sizeof(wiadomosc_wysylana_od_serwera) - sizeof(long), 0);
                                users[i].subskrypcje[j].ilejeszcze--;
                            }
                            }
                    }
                    strcpy(sukces_wyslania.tekst, "Wiadomosc została wysłana");

                    }
                }
                    }//dlugi if


                } else if (temat_istnieje == 0) {
                    strcpy(sukces_wyslania.tekst, "Wiadomosc nie zostala wyslana, zostal podany nieistniejący temat");
                } else {
                    strcpy(sukces_wyslania.tekst, "Wiadomosc nie została wysłana, podany zly priorytet");
                }
            sukces_wyslania.type = wysylana_wiadomosc.id_wysylanego;
            msgsnd(wysylanie_wiadomosci, &sukces_wyslania, sizeof(sukces_wyslania) - sizeof(long), 0);
    }



    if (odpowiedz.wybor == 5) {
        struct signal ban;
        struct wiadomosc wiadomosc_banowana;
        msgrcv(do_banow, &ban, sizeof(ban) - sizeof(long), 1, 0);
        printf("1\n");
        int id_banujacego = ban.odp; //ziomek ktory banuje
        char string[1024] = "";
        for (int i = 0; i < numUsers; i++) {
            if (id_banujacego != users[i].id) {
                char nazwa[1028] = "";
                strcpy(nazwa, users[i].name);
                int id_zioma = users[i].id;
                char id_string[12]="";
                sprintf(id_string, "%d", id_zioma);
                strcat(nazwa, " ");
                strcat(nazwa, id_string);
                strcat(nazwa, "\t");
                strcat(string, nazwa);
                //strcpy("", nazwa);

            }
        }
        strcpy(wiadomosc_banowana.tekst, string);
        wiadomosc_banowana.type = id_banujacego;
        msgsnd(do_banow, &wiadomosc_banowana, sizeof(wiadomosc_banowana) - sizeof(long), 0);
        strcpy(string, "");
        msgrcv(do_banow, &ban, sizeof(ban) - sizeof(long), 1, 0);
        printf("%d\n",ban.odp);
        int id_zbanowanego = ban.odp;
        for(int i=0; i < numUsers; i++) {
            if (users[i].id == id_banujacego) {
                users[i].zbanowani[users[i].liczbazbnowanych] = id_zbanowanego;
                users[i].liczbazbnowanych++;
            }

        }


    }




    }


    //ipcs -q|tr -s ' ' '\t'|cut -f2 | sed '1,3d' | xargs -I {} ipcrm -q {}
    return 0;
}
