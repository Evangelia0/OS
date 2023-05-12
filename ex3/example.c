#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

int main(int argc,char* argv[]){
    char buff[100];
    int n;
    if ((n = read (STDIN_FILENO, buff, 1024 ) ) > 0) {
        buff[n-1]='\0';
        printf("%d",strcmp(buff,"help"));
    }

}