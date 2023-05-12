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
char buffer[256];
int curr_child_index;

void child_code(int i, pid_t pid){
    //i=[0...n-1]
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
           char buffer[1025];
           int maximum = STDIN_FILENO;
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
            int maxfd = maximum+1;
            int num;
            while(1){
              int ready = select(maxfd,&rfds,NULL,NULL,NULL);
                if (ready == -1)
                    perror("select()");
                else if (ready){
                   //check id it is coming from terminal
                   if(FD_ISSET(STDIN_FILENO, &rfds)){
                      if ((n = read (STDIN_FILENO, buffer, 1024 ) ) > 0) {
                            buffer[n] = '\0';  //terminate the string
                            if (buffer[n-1] == '\n') {
                                    buffer[n-1] = '\0';
                                }
                            printf("read %d bytes from the pipe: %s\n", n, buffer);
                            if(strcmp(buffer, "help")==0){
                               fprintf(stdout,"Type a number to send job to a child! ");
                            }
                            else if(!strncmp(buffer,"exit",4)){
                                for(int i=0;i<n;i++){
                                    kill(children[i],SIGTERM);
                                }
                                //wait for all children to terminate
                                int i=0;
                                int curr,status;
                                while(i<n){
                                    fprintf(stdout,"Waiting for %d children to exit\n",n-i);
                                    curr=waitpid(-1,&status,0);
                                    if(curr==-1){
                                        fprintf(stderr,"Error while waiting for terminated children!\n");
                                        exit(1);
                                    }
                                i++;
                                }
                            }
                            else{
                                if(!num==atoi(buffer)){
                                    fprintf(stdout,"Type a number to send job to a child! ");
                                }
                                else{
                                    //check how to distribute tasks
                                    if(round_robin){
                                        write(pipefd[curr_child_index][1], buffer, strlen(buffer) + 1);
                                    }
                                    else{

                                    }
                                }
                            }     
                        }  
                   }
                }
                else
                    printf("No data within five seconds.\n");
              
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
    
    if(argc == 3){
        if(!strcmp(argv[2],"--random")){
            round_robin = 0;
        }
        else if(!strcmp(argv[2],"--round-robin")){
            curr_child_index = 0;
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