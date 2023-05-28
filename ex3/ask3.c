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
//parent to child
int (*parent_to_child)[2];
//child to parent
int (*child_to_parent)[2];
int n,nbytes,upper,lower;
pid_t* children;
int curr_child_index;




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
            lower=0;
            upper=n;
        }
        else if(!strcmp(argv[2],"--round-robin")){
            curr_child_index = 0;
        }
        else {
            fprintf(stdout,"Usage: ask3 <nChildren> [--random] [--round-robin]");
            exit(1);
        }
    }
    
    parent_to_child = malloc(n * sizeof(int[2]));
    child_to_parent = malloc(n * sizeof(int[2]));

    children = (int*)malloc(n*sizeof(int));
    //dynamic allocation of memory
     if(children==NULL){
        fprintf(stderr,"Failed to allocate memory for children array!\n");
        exit(1);
    }

    for(int i=0;i<n;i++){
        if (pipe(parent_to_child[i]) == -1) {
               fprintf(stderr,"Error in pipe!\n");
               exit(1);
           }
        if (pipe(child_to_parent[i]) == -1) {
               fprintf(stderr,"Error in pipe!\n");
               exit(1);
           }
    }
   

  

    for(int i=0;i<n;i++){
        children[i]=fork();
        pid_t pid = children[i];
        if(pid<0){
            fprintf(stderr,"Error while forking!\n");
            exit(1);    
        }
        //child code
        else if(!pid){
            fprintf(stdout,"Child %d created\n",getpid());
            close(child_to_parent[i][0]);
            close(parent_to_child[i][1]);
            while(1){
                char buffer[256];
                char out[256];
                
                int rd = read(parent_to_child[i][0], buffer, 256);
                if(rd==-1){
                    fprintf(stderr,"Error while reading!\n");
                    exit(1);
                    }
                int val = atoi(buffer);
                printf("[Child %d][%d] Child received %d!\n", i, getpid(), val);
                fflush(stdout);
                val++;
                sleep(5);
                sprintf(out,"%d",val);
                //close end of pipe for readind
                close(child_to_parent[i][0]);
                write(child_to_parent[i][1], out, strlen(out)+1);
                printf("[Child %d][%d] Child finished hard work, writing back %d!\n", i, getpid(), val);
                fflush(stdout);
            }
        }

        
 }           
             //PARENT
             for(int i=0;i<n;i++){
                close(parent_to_child[i][0]);
                close(child_to_parent[i][1]);
            }
            while(1){

              fd_set rfds;
              /* Watch stdin (fd 0) to see when it has input. */
              //initialization
              FD_ZERO(&rfds);
              //fd for terminal(input) is assigned to 0
              FD_SET(STDIN_FILENO, &rfds);
              int maximum = STDIN_FILENO;
              for(int i=0;i<n;i++){
                  FD_SET(child_to_parent[i][0],&rfds);
                  maximum = MAX(child_to_parent[i][0],maximum);
              }
              int maxfd = maximum+1;
              int num;
              //check whether data has arrived
              int ready = select(maxfd,&rfds,NULL,NULL,NULL);
                if (ready == -1)
                    perror("select()");
                else if (ready){
                   //check if it is coming from terminal
                   if(FD_ISSET(STDIN_FILENO, &rfds)){
                      char read_buff[256];
                      int n_read = read(STDIN_FILENO, read_buff, 256);

                      read_buff[n_read] = '\0';  //terminate the string
                      if (n_read>0 && read_buff[n_read-1] == '\n') {
                              read_buff[n_read-1] = '\0';
                          }
                      char help[] = "help";
                      if(n_read>=4 && strcmp(read_buff, help)==0){
                         printf("Type a number to send job to a child!\n");
                         fflush(stdout);
                      }
                      
                      /* The fflush(stdout) function is used to flush the output buffer
                         and ensure that any buffered data is written to the standard output
                         immediately. In some cases, the output may not appear immediately on
                         the console if it is still in the buffer. By calling fflush(stdout),
                         you force the buffered data to be written immediately, making it 
                         visible on the console. In your code snippet, it seems that you're 
                         checking if the input received (read_buff) is equal to the string "help". 
                         If it matches, you print a message. However, if you don't flush the output 
                         buffer with fflush(stdout), the message may not appear immediately on the console.
                         Adding fflush(stdout) after printing the message ensures that the message is immediately
                         displayed on the console, providing a better user experience.
                         Here's the modified code snippet with the addition of fflush(stdout):*/
   
                      else if(n_read>=4 && strcmp(read_buff, "exit")==0){
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
                                for(int i=0;i<n;i++){
                                    close(parent_to_child[i][1]);
                                    close(child_to_parent[i][0]);
                                }
                                fflush(stdout);
                                exit(0);
                                

                            }
                       else{   
                               num=atoi(read_buff);
                               fflush(stdout);
                               if(!num){
                                    fprintf(stdout,"Type a number to send job to a child! ");
                                }
                                 else{
                                    //check how to distribute tasks
                                    if(round_robin){
                                        int wr = write(parent_to_child[curr_child_index][1], read_buff, strlen(read_buff)+1);
                                        if(wr==-1){
                                            fprintf(stderr,"Error while writing!\n");
                                            exit(1);
                                        }
                                        //increment child by one(circular)
                                        fprintf(stdout,"[Parent] Assigned %d to child %d\n",num,curr_child_index);
                                        fflush(stdout);
                                        curr_child_index++;
                                        curr_child_index%=n;
                                    }
                                    else{
                                        curr_child_index = (rand() % (upper - lower + 1)) + lower;
                                        int wr = write(parent_to_child[curr_child_index][1], read_buff, strlen(read_buff)+1);
                                        if(wr==-1){
                                            fprintf(stderr,"Error while writing!\n");
                                            exit(1);
                                        }
                                        fprintf(stdout,"[Parent] Assigned %d to child %d\n",num,curr_child_index);
                                        fflush(stdout);
                                    }
                                    
                                }
                            }  
                }
                else{
                    for(int i=0;i<n;i++){
                        if(FD_ISSET(child_to_parent[i][0],&rfds)){
                        char buffer[256];
                        int rd = read(child_to_parent[i][0], buffer, 256);
                        if(rd==-1){
                            fprintf(stderr,"Error while reading!\n");
                            exit(1);
                        }
                        fprintf(stdout,"[Parent] Received result from child %d --> %d\n",i,atoi(buffer));
                        fflush(stdout);
                        }
                    }
                }
           }
}
}