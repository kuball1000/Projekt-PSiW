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
    int kolejka_logowanie = msgget(0x111, 0600 | IPC_CREAT);
    struct msglogin msg_logowanie;
    msgrcv(kolejka_logowanie, &msg_logowanie, sizeof(msg_logowanie)-sizeof(long), 1,0);
    printf("%s\n%d\n", msg_logowanie.name, msg_logowanie.id);

    return 0;
}
