#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#define MAX(a, b) ((a) > (b) ? (a) : (b))


int round_robin = 1;
int (*pipefd)[2];
int n,nbytes;
pid_t* children;

void child_code(int i, pid_t pid){
    //i=[0...n-1]
    if(pid<0){
            fprintf(stderr,"Error while forking!\n");
            exit(1);
        }
        //child code
    else if(!pid){
            //for reading
            int i = pipefd[i][0];
        }
    else { 
           int maximum = STDIN_FILENO
           //reading set
           fd_set rfds;
           /* Watch stdin (fd 0) to see when it has input. */
           //initialization
           FD_ZERO(&rfds);
           //fd for terminal(input) is assigned to 0
           FD_SET(STDIN_FILENO, &rfds);
           for(int i=0;i<n;i++){
              FD_SET(pipefd[i][0],&rfds);
              maximum = MAX(pipefd[i][0],maximum);
           }
            maxfd = maximum+1;
            while(1){
              int ready = select(maxfd,&rfds,NULL,NULL,NULL);
              
            }
        }

}

int main(int argc,char* argv[]){

    //argument check
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
    //dynamic allocation of memory
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