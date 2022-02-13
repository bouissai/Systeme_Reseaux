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
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include<dirent.h>
#include <sys/stat.h>

#include <signal.h>


#define MAX 1000
#define SIZE 1024


void handler(int s){
    printf(" --- handler :");
    wait(NULL);
    printf("confirmation mort fils\n");
}

void consulter(int sockt){
        
    int env=1;      
    write(sockt,&env,sizeof(env));///write B
    
    struct dirent *repertoire_entree;
    char *rep_name = "Files";
        DIR *rep = opendir(rep_name);
        if (rep == NULL){
            perror("on peut pas ouvrir le répertoire courant\n");

        }
        char buff[1500];
        strcpy(buff,"");
        strcat(buff, " | ");
        while((repertoire_entree=readdir(rep))!=NULL){
                              if((strcmp(repertoire_entree->d_name,"..")!=0) && (strcmp(repertoire_entree->d_name,".")!=0)){
                strcat(buff,repertoire_entree->d_name);
                strcat(buff, " | ");
             }    
        }
       
        //modifier le 1er write
        write(sockt,buff,1500);    
        closedir(rep);
      

}

void deposer(int a){

    int env=2;
    //envoi au client l'ordre de se placer dans le cas du DEPOSER
    write(a,&env,sizeof(env));

    
    //printf("%s",nom_fichier);
    //on créé le fichier 
    int fichier = open("NouveauFichier.ppm",O_CREAT|O_WRONLY, 0666);
    //verification ouverture
    if(fichier==-1){
        perror("Erreur ouverture fichier\n");
        exit(-1);
    }


    //on reçoit la taille du fichier
    int taille_fichier;
    read(a,&taille_fichier,sizeof(taille_fichier));

    //calcul nombre de buff[SIZE] a ecrire
    int nbr_de_write = taille_fichier / SIZE;
    //calcul nombre de buff[reste] a ecrire
    int reste = taille_fichier % SIZE;

    //ecrit dans le nouveau fichier les n buff[SIZE]
    char buff [SIZE];
    char buff_reste [reste];
    int i=0;
    while(i<nbr_de_write){

        read(a,buff,SIZE);
        write(fichier,buff,SIZE);
        i++;
    }

    //ecrit le dernier buff[reste]
    read(a,buff_reste,SIZE);
    write(fichier,buff,SIZE);
    
    
    //fermeture fichier
    close(fichier);

}

void stop_client(int a){
    
    printf(" --- Le client veut se deconnecter \n");
    int env=3;
    //envoi au client l'ordre au client de se placer dans le cas du STOP
    write(a,&env,sizeof(env));

    //reçoi la confirmation de la deconnection
    read(a,&env,sizeof(env));
    if(env==111){
        //affiche la commande reçu du client
        printf(" --- confirmation deconnexion client \n");
    }else{
        printf(" --- Erreur deco 111 pas reçu \n");
        exit(-1);
    }

}

void dialogue(int a){


    int cpt=1;
    int rep;

    //boucle infini pour afficher l'interface serveur dialoguant avec le client
    //sort de la boucle si on Quitte
    while(cpt==1){

        //attend de lire la commande que le client veut faire
        read(a, &rep, sizeof(rep));

        //selon la reponse client, le serveur va lancer le processus de
        //consultation, dépôt, deconnection
        switch(rep){
            
            case 1:
                printf(" --- client: CONSULTER\n");

                //lance de processus consulter
                consulter(a);
                
                
                break;

            case 2:
                printf(" --- client: DEPOSER\n");

                //lance de processus deposer
                deposer(a);


                break;

            case 3:
                printf(" --- client: STOP\n");


                stop_client(a);
                
                cpt=-1;
                break;

            default:
                printf(" --- Attend la réponse client\n");
                exit(-1);
        }

    }



}








int main(int argc, char *argv[]){

    //verification commande terminal
    if(argc != 2) {
        perror("Erreur : 1 argument attendu - ./serveur port(int) \n");
        exit(-1);
    }
    //conversion port en int
    int port= atoi(argv[1]);



    //instantiation du sigaction
    struct sigaction ac;
    ac.sa_handler = handler;
    ac.sa_flags = SA_RESTART;
    sigaction(SIGCHLD,&ac,NULL);



    //instantiation de la socket
    struct sockaddr_in socket_adresse;

    //initialisation de la socket ecoute
    int socketEcoute = socket(AF_INET, SOCK_STREAM, 0);

    //verification de la socketEcoute
    if (socketEcoute==-1){
        printf("erreur lors de la Création de socket \n");
        exit(-1);
    }else{
        printf(" - socket a été crée avec succès\n");
    }

    //initialisation de la structure ou on peut dire assignation des attributs de la structure socket_adresse
    socket_adresse.sin_family = AF_INET;
    socket_adresse.sin_addr.s_addr = htonl(INADDR_ANY);
    socket_adresse.sin_port = htons(port);

    //attachement de la socket aux informations de la structure
    int b = bind(socketEcoute, (struct sockaddr*) &socket_adresse, sizeof(socket_adresse));
    if (b==-1){
        printf("Erreur bind\n");
        exit(-1);
    }

    //ouverture de service
    int l = listen(socketEcoute,2);
    //verification listen 
    if (l==-1){
        printf("Erreur listen \n");
        exit(-1);
    }else{
        printf(" - le serveur écoute \n\n");
    }


    //instantiation de la socket_client
    struct sockaddr_in socket_client;

    socklen_t len = sizeof(socket_client);
    int a;
    
    //boucle infinie, le serveur tourne en boucle
    while(1){
        
        //attend une connexion client
        a = accept(socketEcoute, (struct sockaddr*)&socket_client,&len);
    
        //si le serveur reçoit une connection il crée un fils 
        if(a!=-1){
            
            //creation du fils
            switch (fork()){
                    //erreur creation fils
                    case -1:
                        perror("Erreur fork");
                        exit(-1);


                    //processus fils 
                    case 0:
                    
                        //ferme la socket d'écoute
                        close(socketEcoute);

                        printf(" --- Création d'un fils %d \n",getpid());
                        printf("     Un client s'est connecté:\n");
                        
                        //dialogue avec le client connecté
                        dialogue(a);

                        //ferme a une fois le dialogue terminé
                        close(a);

                        //le fils meurt
                        exit(0);

                    //processus père
                    default:
                        //retourne au while 
                        break;
                }
        }
    }
    //ferme la socketEcoute 
    close (a);


}

