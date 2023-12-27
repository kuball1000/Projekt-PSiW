#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/time.h>
#include <ctype.h>
#include<fcntl.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<string.h>

struct msglogin {
long type;
char name[128];
int id;
};

int main(int argc, char* argv[]){
    int logowanie = msgget(0x111, 0600 | IPC_CREAT);
    struct msglogin moje_logowanie;
    moje_logowanie.type = 1;
    strcpy(moje_logowanie.name, "Adam");
    moje_logowanie.id = 11;
    msgsnd(logowanie, &moje_logowanie, sizeof(moje_logowanie)-sizeof(long), 0);

    return 0;
}
