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
#include <signal.h>
#define SIZE 1024


#define MAX 100
struct sigaction ac;



void handler(int s){
    wait(NULL);
}

void write_file(int sockfd){
  int n;
  FILE *fp;
  char *filename = "recv.ppm";
  char buffer[SIZE];

  fp = fopen(filename, "w");
  while (1) {
    n = recv(sockfd, buffer, SIZE, 0);
    if (n <= 0){
      break;
      return;
    }
    fprintf(fp, "%s", buffer);
    bzero(buffer, SIZE);
  }
  return;
}

void dialogue(int a){
	/*int n;
    FILE *f = fopen("Files/mickey.ppm", "r");
    if (f == NULL) {
        perror("[-]Error in reading file.");
        exit(1);
    }
    send_file(f, a);*/
	//char buff[MAX] ;
	//while(1){
	//bzero(buff,MAX); //on met à 0 (octets contenant « \0 ») les n premiers octets du buffer.
	//lire le message du client puis le copier dans le buffer
	//read(a, buff, sizeof(buff));
    
    //char *fname = strcat("Files/", buff);

    //f = fopen(fname, "w");
	// on affiche le message qui vient du client
    //printf("client à envoyé: %s\t message pour client :", buff);
	//bzero(buff, MAX);
     //n = 0;
          // on copie le message du serveur dans le buffer
 //while ((buff[n++] = getchar()) != '\n');//La fonction getchar() lit un caractère sur stdin et le retourne, c'est ce caractère là qui est évalué dans la condition du while
          // et on envoie le buffer  à client
       // write(a, buff, sizeof(buff));
        //write(a, f, sizeof(f));

//}
}

int main(int argc, char *argv[])
{

	    //instantiation de la structure
    struct sockaddr_in socket_adresse;

    //creation de la socket
    int socketEcoute = socket(AF_INET, SOCK_STREAM, 0);
    
    if (socketEcoute==-1)
    {
    	printf("erreur lors de la Création de socket \n");
    }else{
    	printf("socket a été crée avec succès\n");
    }


    //initialisation de la structure ou on peut dire assignation des attributs de la structure socket_adresse
    socket_adresse.sin_family = AF_INET;
    socket_adresse.sin_addr.s_addr = htonl(INADDR_ANY);
    socket_adresse.sin_port = htons(9999);

    //attachement de la socket aux informations de la structure

    int b = bind(socketEcoute, (struct sockaddr*) &socket_adresse, sizeof(socket_adresse));
    
    // si n=-1 erreur
    if (b==-1){
    	printf("le bind a échoué \n");
    	exit(0);
    }else{
    	printf("le bind a réussi \n");
    }

    //ouverture de service
    int l = listen(socketEcoute,2);

    // si l=-1 erreur 
        if (l==-1){
    	printf("le listen a échoué \n");
    	exit(0);
    }else{
    	printf("le serveur écoute \n");
    }


    //boucle infinie
    while(1){


        struct sockaddr_in socket_client;

        socklen_t len = sizeof(socket_client);

        int a = accept(socketEcoute, (struct sockaddr*)&socket_client,&len);
   
        
        ac.sa_handler = handler;
        ac.sa_flags = SA_RESTART;

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

                        /*ac.sa_handler = handler;
                        ac.sa_flags = SA_RESTART;*/
                        sigaction(SIGCHLD,&ac,NULL);
                        
                    // processus pere

                    // ferme le socket  a et on se met en mode attente d'une nouvelle connection
                        close (a);
                }

        }



    } 
    
}