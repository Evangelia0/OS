#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <math.h>

int state;
time_t begin,total_sec;


//setting up the alarm function
void alarm_handler (int s){
    // if (s=SIGALRM){
    //     total_sec++;
    //     alarm(1);
    // }
    total_sec = time(NULL)-begin;
    if (state == 0 ) fprintf(stdout, "[GATE=%d/PID=%d/TIME=%d]The gates are open!\n",state,getpid(),total_sec);

    else if (state == 1) fprintf(stdout, "[GATE=%d/PID=%d/TIME=%d]The gates are closed!\n",state,getpid(),total_sec);
    else{
        fprintf(stderr, "Error while printing the gate state!\n");
        exit(1);
    }
    alarm(15);    
};

//setting up sigusr1 function !!the parameters need to be changed!!
void usr1_handler (int signum){
    total_sec = time(NULL)-begin;
    if (state == 0) {
        state = 1;
        fprintf(stdout, "[GATE=%d/PID=%d/TIME=%d]The gates are closed!\n",state,getpid(),total_sec);
    }
    else if (state == 1) {
        state = 0;
        fprintf(stdout, "[GATE=%d/PID=%d/TIME=%d]The gates are open!\n",state,getpid(),total_sec);
    }
    else {
        fprintf(stderr, "Error! Failed to flip the gate's state!\n");
        exit(1);
    }  
}

void usr2_handler (int signum){
    total_sec = time(NULL)-begin;

    if (state == 0 || state == 1) {
        fprintf(stdout, "Child with pid: %d, state: %d, %d seconds\n",getpid(),state, total_sec);
    }
    else {
        fprintf(stderr, "Error! Not valid gate state!\n");
        exit(1);
        };
}

int main(int argc, char* argv[]){
    begin = time(NULL);
    char str_state = argv[1][0];
    if(str_state=='t')
        state=0;
    else if(str_state=='f')
        state=1;
    //SIGUSR1
    struct sigaction usr1_action,usr2_action, alarm_action;
    sigset_t sigset_usr1, sigset_usr2, sigset_alarm;
    /* Set up the structure to specify the new action. */
    usr1_action.sa_handler = usr1_handler;
    sigemptyset(&sigset_usr1);
    sigaddset(&sigset_usr1, SIGUSR2);
    sigaddset(&sigset_usr1, SIGALRM);
    /*If you use sigaction to establish a signal handler, you can specify how that handler should behave.
     If you specify the SA_RESTART flag, return from that handler will resume a primitive;
    otherwise, return from that handler will cause EINTR. */
    usr1_action.sa_flags = SA_RESTART;
    usr2_action.sa_handler = usr2_handler;
    sigemptyset(&sigset_usr2);
    sigaddset(&sigset_usr2, SIGUSR1);
    sigaddset(&sigset_usr2, SIGALRM);
    usr2_action.sa_flags = SA_RESTART;

    //set up the alarm via sigaction
    alarm_action.sa_handler=alarm_handler;
    alarm_action.sa_flags=0; //no special flags are set for the alarm
    sigemptyset(&sigset_alarm);
    sigaddset(&sigset_alarm, SIGUSR1);
    sigaddset(&sigset_alarm, SIGUSR2); 

    if (0 != sigaction(SIGALRM, &alarm_action, NULL)){
        perror("sigaction () failed installing SIGALRM handler");
        exit(1);
    }
     if (0 != sigaction(SIGUSR1, &usr1_action, NULL)){
        perror("sigaction () failed installing SIGUSR1 handler");
        exit(1);
    }
    if (0 != sigaction(SIGUSR2, &usr2_action, NULL)){
        perror("sigaction () failed installing SIGUSR2 handler");
        exit(1);
    }
    kill(getpid(),SIGALRM);
    
    while(1);

    
    
    
}