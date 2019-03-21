#include <stdio.h>
#include <stdlib.h> /* pour exit */
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h> /* pour memset */
#include <netinet/in.h> /* pour struct sockaddr_in */
#include <arpa/inet.h> /* pour htons et inet_aton */
#define PORT IPPORT_USERRESERVED // = 5000
#define LG_MESSAGE 256
#define TAILLE 3

int main(int argc, char**argv)
{
	char *fr[] = { "bleu", "vert", "rouge" };
	char *en[] = { "blue", "green", "red" };
	int descripteurSocket;
	struct sockaddr_in pointDeRencontreDistant,adresse; 
	socklen_t longueurAdresse;
	char messageEnvoi[LG_MESSAGE]; /* le message de la couche Application ! */ 
	char messageRecu[LG_MESSAGE]; /* le message de la couche Application ! */ 
	int ecrits, lus; /* nb d’octets ecrits et lus */
	int retour;
	int longueur;


	// Crée un socket de communication
	descripteurSocket = socket(PF_INET, SOCK_STREAM, 0);
	// Teste la valeur renvoyée par l’appel système socket()

	if(descripteurSocket < 0) {
		perror("socket"); // Affiche le message d’erreur
		exit(-1); // On sort en indiquant un code erreur
	}
	printf("Socket créée avec succès ! (%d)\n", descripteurSocket);


	//--- Début de l’étape n°2 :
	// Obtient la longueur en octets de la structure sockaddr_in
	longueurAdresse = sizeof(pointDeRencontreDistant);
	// Initialise à 0 la structure sockaddr_in
	memset(&pointDeRencontreDistant, 0x00, longueurAdresse);
	// Renseigne la structure sockaddr_in avec les informations du serveur distant
	pointDeRencontreDistant.sin_family = PF_INET; // IPv4
	// On choisit le numéro de port d’écoute du serveur
	pointDeRencontreDistant.sin_port = htons(IPPORT_USERRESERVED); // = 5000
	// On choisit l’adresse IPv4 du serveur
	inet_aton("127.0.0.1", &pointDeRencontreDistant.sin_addr); // à modifier selon ses besoins
	// Débute la connexion vers le processus serveur distant 

	if((connect(descripteurSocket, (struct sockaddr      *)&pointDeRencontreDistant,longueurAdresse)) == -1)
	{
		perror("c#define LG_MESSAGE 256onnect"); // Affiche le message d’erreur 
		close(descripteurSocket); // On ferme la ressource avant de quitter 
		exit(-2); // On sort en indiquant un code erreur
	}

	//--- Fin de l’étape n°2 !

	printf("Connexion au serveur réussie avec succès !\n");
	longueur = sizeof(struct sockaddr_in);
		if(getsockname(descripteurSocket, (struct sockaddr *) &adresse, &longueur ) < 0 ){
			perror ("getsockname");
			return(-1);
		}
				
		fprintf(stdout,"IP = %s, Port = %u \n", inet_ntoa(adresse.sin_addr), ntohs(adresse.sin_port) );


	//--- Début de l’étape n°4 :
	while (1){
		memset(messageEnvoi, 0x00, LG_MESSAGE*sizeof(char)); 
		memset(messageRecu, 0x00, LG_MESSAGE*sizeof(char));

	// Envoie un message au serveur

		printf("Formulez votre demande:\n");
		fgets(messageEnvoi,LG_MESSAGE, stdin);
		messageEnvoi[strcspn(messageEnvoi, "\n")] = 0;
		ecrits = write(descripteurSocket, messageEnvoi, strlen(messageEnvoi));
		// message à TAILLE variable 
		switch(ecrits)
		{
			case -1 : /* une erreur ! */ 
				perror("write"); 
				close(descripteurSocket); 
				exit(-3);
			case 0 : /* la socket est fermée */
				fprintf(stderr, "La socket a été fermée par le serveur !\n\n"); 
				close(descripteurSocket);
				return 0;
			default: /* envoi de n octets */
				printf("Message %s envoyé avec succès (%d octets)\n\n", messageEnvoi, ecrits);
		}

		/* Reception des données du serveur */
		lus = read(descripteurSocket, messageRecu, LG_MESSAGE*sizeof(char)); 
		/* attend un message de TAILLE fixe */ 
		switch(lus)
		{
			case -1 : /* une erreur ! */ 
				perror("read"); 
				close(descripteurSocket); exit(-4);
			case 0 : /* la socket est fermée */
				fprintf(stderr, "La socket a été fermée par le serveur !\n\n"); 
				close(descripteurSocket);
				return 0;
			default: /* réception de n octets */
				printf("Message reçu du serveur : %s (%d octets)\n\n", messageRecu, lus);
		}
	}
	//--- Fin de l’étape n°4 !
	// On ferme la ressource avant de quitter
	close(descripteurSocket);
	return 0;
}
