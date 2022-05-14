#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

char byte[1];

int cpip[2]; // pipe from child to parent
int ppip[2]; // pipe from parent to child
//read from 0, write to 1
int
main(int argc, char *argv[])
{
    byte[0] = 'a';
    if (pipe(cpip) == -1) {
        printf("Failed to create child pipe");
    }
    if (pipe(ppip) == -1) {
        printf("Failed to create parent pipe");
    }
    int pid = fork();
    if (pid == 0) {
        close(ppip[1]);
        char buff[1];
        read(ppip[0], buff, 1);
        printf("%d: received ping\n", getpid());
        write(cpip[1], buff, strlen(buff));
        exit(0);
        //child
    } else {
        close(ppip[0]);
        char buff[1];
        write(ppip[1], byte, strlen(byte));
        read(cpip[0], buff, 1);
        printf("%d: received pong\n", getpid());
        exit(0);
        //parent
    }

}
