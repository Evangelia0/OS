#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

int round_robin = 1;
int (*pipefd)[2];
int n,nbytes;
pid_t* children;
char buffer[256];

void child_code(int i, pid_t pid){
    if(pid<0){
            fprintf(stderr,"Error while forking!\n");
            exit(1);    
        }
        //child code
    else if(!pid){
        read(pipefd[i][0], buffer, sizeof(buffer));
        printf("[Child][%d] Child received %d!\n", i, getpid(), buffer);
        int val = atoi(buffer);
        val++;
        sleep(5);
        write(pipefd[i][1], val, sizeof(val));
        printf("[Child][%d] Child finished hard work, writing back %d!\n", i, getpid(), val);
        exit(1);
        }
    else {
           //reading set
           fd_set rfds;
           /* Watch stdin (fd 0) to see when it has input. */
           //initialization
           FD_ZERO(&rfds);
           //fd for terminal(input) is assigned to 0
           FD_SET(0, &rfds);
           for(int i=0;i<n;i++){
              FD_SET(pipefd[i][0],&rfds);
           }

           


            while(1){

            }
        }

}

int main(int argc,char* argv[]){
    if(argc<=1){
        fprintf(stdout,"Usage: ask3 <nChildren> [--random] [--round-robin]");
        exit(1);
    }

    n = atoi(argv[1]);
    
    if(argc == 2){
        if(!strcmp(argv[2],"--random")){
            round_robin = 0;
        }
        else if(!strcmp(argv[2],"--round-robin")){
            round_robin = 1;
        }
        else {
            fprintf(stdout,"Usage: ask3 <nChildren> [--random] [--round-robin]");
            exit(1);
        }
    }
    
    pipefd = malloc(n * sizeof(int[2]));
    children = (int*)malloc(n*sizeof(int));
     if(children==NULL){
        fprintf(stderr,"Failed to allocate memory for children array!\n");
        exit(1);
    }

    for(int i=0;i<n;i++){
        if (pipe(pipefd[i]) == -1) {
               fprintf(stderr,"Error in pipe!\n");
               exit(1);
           }
    }

    for(int i=0;i<n;i++){
        children[i]=fork();
        child_code(i,children[i]);
    }
        

    
}