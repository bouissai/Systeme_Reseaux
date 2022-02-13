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

#include <sys/stat.h>
#include <fcntl.h>

#define MAX 1000
#define SIZE 1024





void deposer(int a, char* fichier){

    //ouverture fichier
    int descripteur_fichier=open(fichier,O_RDONLY);
    //verification ouverture
    if(descripteur_fichier==-1){
        perror("Erreur ouverture fichier\n");
        exit(-1);
    }


    //calcul taille fichier
    struct stat sb;
    if (fstat(descripteur_fichier, &sb) == -1) {
        perror("Erreur stat\n");
        exit(-1);
    }
    int taille_fichier=sb.st_size;

    //calcul nombre de buff[SIZE] a ecrire
    int nbr_de_write = taille_fichier / SIZE;
    //calcul nombre de buff[reste] a ecrire
    int reste = taille_fichier % SIZE;

    
    //taille fichier
    write(a,&taille_fichier,sizeof(int));


    //envoie de packets
    char buff [SIZE];
    char buff_reste [reste];
    int i=0;
    while(i<nbr_de_write){

        read(descripteur_fichier,buff,SIZE);
        write(a,buff,sizeof(buff));
        i++;
    }

    read(descripteur_fichier,buff_reste,SIZE);
    write(a,buff_reste,sizeof(buff_reste));


    //fermeture fichier
    close(descripteur_fichier);


}

















void stop_client(int a){
        //si le client veut se deconnecter,
    //il envoie un message précis au serveur ("111")
    //pour lui informer qu'il va bien se deconnecter

    // - envoie message de confirmation
    // - puis sort des boucles de discussion
    // - et fait exit(0) dans le main

    int rep = 111;
    write(a, &rep, sizeof(rep)); ///// write C
    printf(" --- confirmation deconnexion du client\n");


}

void afficher(int sockfd) // affochage de la liste 
{   
    char buff[1500];
    read(sockfd,buff,1500);
    printf("%s \n",buff);
}

void menu_client(int socketEnvoi){
    int cpt1=1;
    int rep;
    //boucle infini pour afficher l'interface, sort de la boucle si on Quitte
    while(cpt1==1){
        //affichage interface 
        printf("\n-----------------------------\n");
        printf("\nListe des options:\n");
        printf("1 - Afficher la liste des noms de fichiers images disponibles dans le serveur\n");
        printf("2 - Déposer une image dans le serveur\n");
        printf("3 - Quitter\n");
        printf("\n>");
    
        char *n;
        int nombre;
        //free(n);
        n = (char*) malloc(50 * sizeof(char));

        //l'utilisateur entre au clavier un entier correspondant à ce qu'il veut faire
        scanf("%s", n);

        //verification  de la commande entrée
        //si ne correspond pas à 1,2 ou 3 on redemande à  l'utilisateur de taper
        while(strcmp(n,"1")!=0 && strcmp(n,"2")!=0 && strcmp(n,"3")!=0){
            printf("ce n'est pas une commande\n");
            printf(">");
            scanf(" %s", n);
        }
        
        //on converti la commande entrée en entier
        nombre=atoi(n);

        //on ecrit dans la socketEnvoi l'entier
        write(socketEnvoi, &nombre, sizeof(nombre));

        //on lit dans la socketEnvoi la réponse de confirmation du serveur 
        read(socketEnvoi, &rep, sizeof(rep));


        char *fichier_entree;

        fichier_entree = (char*) malloc(500 * sizeof(char));      
        //selon la reponse, le client va consulter,deposer ou se deconnecter 
        switch(rep){
            case 1:
                printf(" --- client : CONSULTER\n\n");
                afficher(socketEnvoi);

                break;

            case 2:
                printf(" --- client : DEPOSER\n\n");
                
                printf("Entrez un nom de fichier :\n>");
                scanf(" %s",fichier_entree);
                printf("<%s\n",fichier_entree);

                //verifier_type(fichier_entree):
                
                deposer(socketEnvoi,fichier_entree);
                
                printf("Fichier envoyé\n");
                break;

            case 3:
                printf(" --- client : DECONNEXION\n\n");
                
                stop_client(socketEnvoi);
                //sort des 2 boucles write
                cpt1=-1;


                exit(0);
                break;

            default:
                //cas d'erreur, le client reçoit autre chose que 1-2-3
                perror("Erreur switch client\n");
                exit(-1);
        }

        
    
    
        
    }
}


int main(int argc, char *argv[]){

    //on verifie qu'il y'a bien 2 paramettres dans la commande
    if(argc != 3) {
        perror("Erreur : 2 arguments attendus - ./client nom_machine(string) port(int) \n");
        exit(-1);
    }

    //on récupère le port correspondant au deuxième paramètre 
    int port= atoi(argv[2]);
    //on recupère l'ip de la machine donnée

    
    //instantiation des la structure host (infos de la machine)
    struct hostent* host  =  gethostbyname(argv[1]);

    //creation de la socketEnvoi
    int socketEnvoi = socket(AF_INET, SOCK_STREAM, 0);
    //verification de la socketEnvoi
    if(socketEnvoi ==-1){
        printf("Failed socketEnvoi \n ");
        exit(-1);
    }

    //instantiation de la structure
    struct sockaddr_in socket_adresse;

    //instantiation des la structure host (infos de la machine)

    //initialisation de la structure
    socket_adresse.sin_family = AF_INET;
    socket_adresse.sin_addr.s_addr = *(long *)host->h_addr_list[0];
    socket_adresse.sin_port = htons(port);

    //établi une connection avec le serveur
    int c = connect(socketEnvoi, (struct sockaddr*)&socket_adresse, sizeof(socket_adresse));
    //verification
    if(c==-1){
        printf("Erreur connexion échouée \n");
        exit(-1);
    }
    printf("Client connecté au serveur\n");


    //dialogue avec le serveur à travers un menu
    menu_client(socketEnvoi);

    //fermeture de la socketEnvoi
    close(socketEnvoi);

    //mort processus client
    exit(0);

}

