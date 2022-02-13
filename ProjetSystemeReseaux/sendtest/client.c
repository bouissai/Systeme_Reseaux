#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#define MAX 100

void dialogue_cli(int socketEnvoi){

  char buff[MAX];
  int n;
  int cpt=1;
    while(cpt==1){

    bzero(buff, sizeof(buff));
  printf("Enter une  chaine de characteres à envoyer:");
  n = 0;
  
  while ((buff[n++] = getchar()) != '\n'){;}

 

  write(socketEnvoi, buff, sizeof(buff));
  bzero(buff, sizeof(buff));
  read(socketEnvoi, buff, sizeof(buff));
  printf("réponse du serveur : %s", buff);

    if(strncmp(buff,"stop",4)==0){
    printf("deconnexion\n");
      exit(0);
      cpt=-1;
  }

}

}






int main(int argc, char *argv[]){

    //creation de la socket
    int socketEnvoi = socket(AF_INET, SOCK_STREAM, 0);

    //verification de la socketEnvoi
    if(socketEnvoi ==-1){
        printf("Failed socketEnvoi \n ");
        exit(-1);
    }

    //instantiation des la structure host (infos de la machine)
    struct hostent* host  =  gethostbyname("localhost");



    //instantiation de la structure
    struct sockaddr_in socket_adresse;

    //initialisation de la structure
    socket_adresse.sin_family = AF_INET;
    socket_adresse.sin_addr.s_addr = *(long *)host->h_addr_list[0];
    socket_adresse.sin_port = htons(9999);

    //établi une connection avec le serveur
    int c = connect(socketEnvoi, (struct sockaddr*)&socket_adresse, sizeof(socket_adresse));
    
    //verification
    if(c==-1){
        printf("Failed connect \n");
        exit(-1);
    }
 


    //dialogue avec le serveur
    dialogue_cli( socketEnvoi);
 
    //fermeture de la socketEnvoi
      close(socketEnvoi);
      exit(0);

      while(1){


        struct sockaddr_in socket_client;

        socklen_t len = sizeof(socket_client);

        int a = accept(socketEcoute, (struct sockaddr*)&socket_client,&len);
   
        

        if(a!=-1){
            
            switch (fork()){
                    case -1:
                        perror("erreur");
                
                    case 0:
                    //processus fils 
                    //ferme la socket d'écoute
                    	
                        printf("Création d'un fils %d \n",getpid());
                    	//dialogue(a);
                        write_file(a);
                        close(socketEcoute);
                    	//close(a);

                        exit(0);

                    default:
                        //a faire wait avec handler et sigchild

                        ac.sa_handler = handler;
                        ac.sa_flags = SA_RESTART;
                        sigaction(SIGCHLD,&ac,NULL);
                        
                    // processus pere

                    // ferme le socket  a et on se met en mode attente d'une nouvelle connection
                        close (a);
                }

        }
    
}