#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <netdb.h>
#include <stdarg.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))


//setting the default host, port and bool parameters to check if hpst, port and debug are chosen
char host[50]="iot.dslab.pub.ds.open-cloud.xyz";
char code[50];
int check_host = 0;
char port[10] = "18080";
int check_port = 0 ;
int debug = 0;
int get=0;
int verification=0;
char str[4][64];
char* token;

int count_words(char* s){
     int count = 0, i;
     for (i = 0; s[i] != '\0';i++)
     {
         if (s[i] == ' ' && s[i+1] != ' ' && i!=0)
             count++;
     }
     return count+1;
}
int main(int argc, char *argv[]){

    if(argc<1 || argc>6){
        fprintf(stdout, "Usage: ./a.out [--host HOST] [--port PORT] [--debug]");
        exit(1);
    }
    for(int i=1;i<argc;i+=2){
        if(!strcmp(argv[i],"[--host")){
            strcpy(host,argv[i+1]);
            host[strlen(host)-1]='\0';
        }

        if(!strcmp(argv[i],"[--port")){
            strcpy(port,argv[i+1]);
            port[strlen(port)-1]='\0';
            printf("%s\n",port);
        }
        if(!strcmp(argv[i],"[--debug]")){
            debug=1;
        }
    }

    //define socket
    int domain = AF_INET;
    int type = SOCK_STREAM;
    /*
         Stream communication (SOCK_STREAM), όπου τα δεδοµένα στέλνονται πάνω σε ήδη υπάρχον κανάλι
     επικοινωνίας (για αυτό και ο συγκεκριµένος τρόπος ονοµάζεται connection oriented). Το πρωτόκολλο
     επικοινωνίας TCP µέσα στον πυρήνα του λειτουργικού συστήµατος φροντίζει και αναλαµβάνει όλους τους
     ελέγχους για λάθη στη µετάδοση των δεδοµένων καθώς και την επαναµετάδοση των πακέτων που
     χάθηκαν. Έτσι εξασφαλίζονται αξιόπιστα κανάλια επικοινωνίας στα οποία οι πληροφορίες φτάνουν µε τη
     σωστή σειρά και πλήρεις, αν και µε µικρότερη ταχύτητα. Αυτός ο τρόπος δεν κρατά τα όρια των
     µηνυµάτων που στέλνονται, όπως ακριβώς συµβαίνει και µε τα pipes. Αυτό σηµαίνει ότι κάθε ανάγνωση
     δεν επιστρέφει απαραίτητα τα δεδοµένα µίας µόνο αποστολής δεδοµένων, αλλά όσα δεδοµένα είναι στον
     buffer του συστήµατος από προηγούµενες αποστολές. Αυτόν τον τρόπο επικοινωνίας χρησιµοποιούν
     συνήθως εφαρµογές που έχουν µεταξύ τους µια καθορισµένη σχέση “πελάτη - εξυπηρετητή”. 
     */

    int sd = socket(domain, type, 0);

    if (sd < 0){
        perror("Error while creating client socket");
        exit(1);
    }
    /*
     struct sockaddr_in {
     unsigned char  sin_len;
     unsigned char  sin_family;
     unsigned short sin_port;
     struct in_addr sin_addr;
     unsigned char  sin_zero[8];
 
    };*/
    struct hostent *hostnm;    /* server host name information        */
    struct sockaddr_in server; /* Server to connect */
    memset(&server, 0, sizeof(server));

    if ( (hostnm=gethostbyname(host)) == NULL ) {
        fprintf(stderr, "unknown host %s\n", host);
        exit(1);
        } 
    /*
            struct hostent { 
        char *h_name; /* official name of host */ 
        //char **h_aliases; /* alias list */ 
        //int h_addrtype; /* host address type */ 
        //int h_length; /* length of address */ 
        //char **h_addr_list; /* list of addresses from name server */ 
        //#define h_addr h_addr_list[0] /* address, for backward compatiblity */ 
        //};
        /*
        Στη δοµή hostent, το πεδίο h_name περιέχει το όνοµα του µηχανήµατος στο δίκτυο, το
        h_addrtype έχει την τιµή AF_INET για τα πρωτόκολλα internet και για τον ίδιο λόγο το h_length
        έχει πάντα την τιµή 4. Αυτό όµως που µας ενδιαφέρει περισσότερο είναι το πεδίο h_addr_list το οποίο
        είναι ένας πίνακας από δείκτες όχι σε χαρακτήρες αλλά σε δοµές τύπου in_addr (Σχήµα 3). Ο αριθµός των
        στοιχείων του πίνακα δεν είναι γνωστός, αλλά γνωρίζουµε ότι το τελευταίο του στοιχείο είναι ένας δείκτης
        NULL. Όπως θα έχει γίνει ήδη κατανοητό αυτές είναι οι internet διευθύνσεις όλων των δικτύων στα οποία ίσως
        συµµετέχει το µηχάνηµα. Επειδή στη γενικότερη περίπτωση η µηχανή θα ανήκει σε ένα µόνο δίκτυο έχει
        ορισθεί ένα macro ώστε να είναι ευκολότερη η προσπέλαση του πρώτου (και µοναδικού στην περίπτωση αυτή) 
        στοιχείου του πίνακα h_addr_list*/    
    server.sin_family = AF_INET; 
    server.sin_port = htons(atoi(port)); 
    bcopy((char*)hostnm->h_addr, (char*)&server.sin_addr,hostnm->h_length); 
    if (connect(sd, (struct sockaddr *)&server, sizeof(server)) < 0 ) {
        fprintf(stderr, "%s: cannot connect to server: ", host);
        perror(0);
        exit(1);
        } 
    
    fd_set rfds,master;
    int maxfd,nbytes;
    FD_ZERO(&master);                // we must initialize before each call to select
    FD_SET(STDIN_FILENO, &master);   // select will check for input from stdin
    FD_SET(sd, &master);          // select will check for input from socket
    /*
        int select(int nfds, fd_set *read_ds, fd_set *write_ds, 
       fd_set *except_ds, struct timeval *timeout); 
       Η select() δέχεται σαν παραµέτρους δείκτες σε πεδία από bits (bit fields) τύπου fd_set που σηµαίνουν
       “αν το i-οστό bit είναι 1 τότε η select() αναµένει είσοδο/έξοδο από τον i-οστό descriptor”. Πιο
       συγκεκριµένα, κάθε δοµή fd_set δουλεύει σαν µάσκα και δηλώνει ποιοί είναι οι descriptors (file ή/και
       socket) που θα ελεγχθούν. Η select() ελέγχει κάθε µάσκα και αφαιρεί από αυτή όσους descriptor δεν
       βρίσκει έτοιµους. Η πρώτη παράµετρος είναι η αριθµητική τιµή του µεγαλύτερου descriptor αυξηµένη κατά 1. 
       Όσοι descriptor είναι στην read_ds θα ελεγχθούν αν είναι έτοιµοι για ανάγνωση δεδοµένων, στην
       write_ds για γράψιµο και στην except_ds για ορισµένες καταστάσεις που είναι δυνατόν να συµβούν στα
       sockets. Εδώ µας ενδιαφέρει κυρίως η read_ds. Αν κάποια κατάσταση δεν µας ενδιαφέρει, µπορούµε απλώς
       να περάσουµε NULL στην αντίστοιχη παράµετρο. */
    maxfd = MAX(STDIN_FILENO, sd) + 1;


    while(1){
        /*
        On the other hand, the rfds set is used as a temporary set
         that will be modified by the select function. The memcpy 
         operation copies the contents of the master set into the rfds 
         set, creating an identical copy. This is necessary because 
         the select function modifies the passed-in sets, so it's important 
         to use a temporary set (in this case, rfds) to preserve the original master set.*/
        memcpy(&rfds, &master, sizeof(master));

        // select only considers file descriptors that are smaller than maxfd

        // wait until any of the input file descriptors are ready to receive
        int ready_fds = select(maxfd, &rfds, NULL, NULL, NULL);
        if (ready_fds <= 0) {
            perror("select");
            continue;                                       // just try again
        }
        
        if(FD_ISSET(STDIN_FILENO, &rfds)){
            char read_buff[256];
            int n_read = read(STDIN_FILENO, read_buff, 256);
            read_buff[n_read] = '\0';  //terminate the string
            if (n_read>0 && read_buff[n_read-1] == '\n') {
                    read_buff[n_read-1] = '\0';
                }
            if(n_read>=4 && strcmp(read_buff, "help")==0){
               if(debug) printf("[DEBUG] read 'help'\n");
               printf("[program]\n#Type 'get' to receive info about the server\n#Type '(Number) name surname reason' to ask for curfew permission\n#Type 'exit' to disconnect and shut donw the client.\n");
               fflush(stdout);
               memset(read_buff,0,sizeof(read_buff));
               continue;
            }

            if(n_read>=4 && strcmp(read_buff, "exit")==0){
               if(debug) fprintf(stdout,"[DEBUG] read 'exit'\n");
               fprintf(stdout, "I'm leaving, bye!\n");
               close(sd);
               fflush(stdout);
               exit(0);
            }

            if(n_read>=3 && strcmp(read_buff, "get")==0){
                if(debug) fprintf(stdout,"[DEBUG] read 'get'\n");
                get=1;
                

            }

            if(count_words(read_buff)==4){
                if(debug)
                   fprintf(stdout,"[DEBUG] sent '%s'\n",read_buff);
                               
            }
            //κοιτάμε αν οι κωδικοί είναι ίδιοι
            if(!atoi(read_buff) && verification==1){
                if(strcmp(read_buff,code)>=1){
                    fprintf(stdout,"Try again\n");
                    continue;
                }
                verification=0;
            }

            
            int wr = write(sd,read_buff,strlen(read_buff)+1);
            if(wr==-1){
                fprintf(stderr,"Error while writing!\n");
                exit(1);
                
            }
           

        }

        else if(FD_ISSET(sd, &rfds)){
            char buffer[256];
            nbytes=read(sd,buffer,sizeof(buffer));
            if(nbytes<0){
                perror("Error while reading\n");
                exit(1);
            }
            if(debug){
                fprintf(stdout,"[DEBUG] read '%s'\n",buffer);
            }
            //αν έχει σταλεί αριθμός
            if(!atoi(buffer)){
               fprintf(stdout,"Send verification code: '%s'\n",buffer); 
               strcpy(code,buffer);
               verification=1;
            }
            if(count_words(buffer)==5){
                fprintf(stdout,"Response: '%s\n'",buffer);
            }
            //αν στείλαμε get
            if(get){
               fprintf(stdout,"---------------------------");
               token = strtok(buffer," ");
               strcpy(str[0],token);
               for(int i=1;i<4;i++){
                 //για να πάρεις το επόμενο τοκεν
                 token = strtok(NULL," ");
                 strcpy(str[i],token);
               }
               int num=atoi(str[0]);
               if(num ==0)
                   printf("Latest event:\nBoot(%d)\n",num);   
               if(num ==1)
                   printf("Latest event:\nSetup(%d)\n",num);                                              
			   if(num == 2)
                   printf("Latest event:\nInterval(%d)\n",num);   
               if(num ==3)
                   printf("Latest event:\nButton(%d)\n",num);
               if(num == 4)
                   printf("Latest event:\nMotion(%d)\n",num);
               printf("Temperature is %f\n",(float) atoi(str[2])/100);
               printf("Light level is: %d\n",atoi(str[1]));
               time_t now = atoi(str[3]);
               struct tm ts;
               // Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
               /*
               The C library function struct tm *localtime(const time_t *timer)
                uses the time pointed by timer to fill a tm structure with the values
                 that represent the corresponding local time. The value of timer is broken
                  up into the structure tm and expressed in the local time zone.

                */
               ts = *localtime(&now);
               strftime(buffer, sizeof(buffer), "%a %Y-%m-%d %H:%M:%S %Z", &ts);
               printf("%s\n", buffer);
               get=0;
                                                                 
            }
            memset(buffer,0,sizeof(buffer));

        }

        
    }
    


   
    
    return 0;
}   