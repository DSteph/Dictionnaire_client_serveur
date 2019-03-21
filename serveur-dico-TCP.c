#include <stdio.h>
#include <stdlib.h> /* pour exit */
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h> /* pour memset */
#include <netinet/in.h> /* pour struct sockaddr_in */
#include <arpa/inet.h> /* pour htons et inet_aton */
#include <unistd.h> /* pour sleep */
#define PORT IPPORT_USERRESERVED // = 5000
#define LG_MESSAGE 256
#define TAILLE 3

char ** strsplit(const char* str, const char* delim, size_t* numtokens);
char * getFR(char * motEN);
char * getEN(char * motFR);
char *fr[] = { "bleu", "vert", "rouge" };
char *en[] = { "blue", "green", "red" };

int main()
	{
	int socketEcoute,socketDialogue;
	int pid, status, codeRetour;
	struct sockaddr_in pointDeRencontreLocal; socklen_t longueurAdresse;
	struct sockaddr_in pointDeRencontreDistant;
	char messageEnvoi[LG_MESSAGE]; /* le message de la couche Application ! */
	char messageRecu[LG_MESSAGE]; /* le message de la couche Application ! */ 
	int ecrits, lus; /* nb d’octets ecrits et lus */
	char **requete;
	size_t nbItems;
	int retour;

	// Crée un socket de communication
	socketEcoute = socket(PF_INET, SOCK_STREAM, 0); /* 0 indique que l’on utilisera le
	protocole par défaut associé à SOCK_STREAM soit TCP */

	// Teste la valeur renvoyée par l’appel système socket()

	if(socketEcoute < 0) /* échec ? */
	{
		perror("socket"); // Affiche le message d’erreur
		exit(-1); // On sort en indiquant un code erreur
	}
	printf("Socket créée avec succès ! (%d)\n", socketEcoute);

	// On prépare l’adresse d’attachement locale 
	longueurAdresse = sizeof(struct sockaddr_in);
	memset(&pointDeRencontreLocal, 0x00, longueurAdresse);
	pointDeRencontreLocal.sin_family = PF_INET;
	pointDeRencontreLocal.sin_addr.s_addr = htonl(INADDR_ANY); // toutes les interfaces locales disponibles
	pointDeRencontreLocal.sin_port = htons(PORT);

	// On demande l’attachement local de la socket
	if((bind(socketEcoute, (struct sockaddr *)&pointDeRencontreLocal, longueurAdresse)) < 0)
	{
		perror("bind");
		exit(-2);
	}
	printf("Socket attachée avec succès !\n");

	// On fixe la taille de la file d’attente à 5 (pour les demandes de connexion non encore traitées)
	if(listen(socketEcoute, 5) < 0)
	{
		perror("listen");
		exit(-3);
	}
	printf("Socket placée en écoute passive ...\n");

	//--- Début de l’étape n°7 :
	// boucle d’attente de connexion : en théorie, un serveur attend indéfiniment !
	
		
		printf("Attente d’une demande de connexion (quitter avec Ctrl-C)\n\n");
		// c’est un appel bloquant
		socketDialogue = accept(socketEcoute, (struct sockaddr *)&pointDeRencontreDistant, & longueurAdresse);
		if (socketDialogue < 0)
		{
			perror("accept");
			close(socketDialogue);
			close(socketEcoute);
			exit(-4);
		}
	while(1)
		{
		memset(messageEnvoi, 0x00, LG_MESSAGE*sizeof(char));
		memset(messageRecu, 0x00, LG_MESSAGE*sizeof(char));
		// On réception les données du client (cf. protocole)
		lus = read(socketDialogue, messageRecu, LG_MESSAGE*sizeof(char)); // ici appel bloquant 

		pid=fork();

		if (pid == -1) {
			perror("Fork error");
			return 1;
		}
		if (pid == 0) { // FILS
			printf("Requête traitée par le processus %d\n", getpid());
			switch(lus)
			{
			case -1 : /* une erreur ! */
				perror("read");
				close(socketDialogue);
				exit(-5);
			case 0 : /* la socket est fermée */
				fprintf(stderr, "La socket a été fermée par le client !\n\n");
				close(socketDialogue);
				return 0;
			default: /* réception de n octets */
				printf("Message reçu : %s (%d octets)\n\n", messageRecu, lus);
			}

		//Conversion message
		requete = strsplit(messageRecu, " ", &nbItems);

		//Check si client veut quitter
			if (strcmp(requete[0], "QUIT") == 0){
				close(socketEcoute);
				return 2;
				}
		// Envoie un message au client
			if (strcmp(requete[0], "LIST") == 0){
				if (strcmp(requete[1], "FR") == 0)
					sprintf(messageEnvoi, "%s %s %s", fr[0],fr[1],fr[2]);
				else if (strcmp(requete[1], "EN") == 0)
					sprintf(messageEnvoi, "%s %s %s", en[0],en[1],en[2]);
				else sprintf(messageEnvoi, "Liste non disponible");
			}
			else{
				if (strcmp(requete[0], "FR") == 0)
					sprintf(messageEnvoi, "%s %s", requete[0], getFR(requete[1]));
				else if (strcmp(requete[0], "EN") == 0)
					sprintf(messageEnvoi, "%s %s", requete[0], getEN(requete[1]));
				else sprintf(messageEnvoi, "%s ?? %s", requete[0], requete[1]);
			}
		// On envoie des données vers le client (cf. protocole)
		ecrits = write(socketDialogue, messageEnvoi, strlen(messageEnvoi));
			switch(ecrits)
			{
			case -1 : /* une erreur ! */
				perror("write");
				close(socketDialogue);
				exit(-6);
			case 0 : /* la socket est fermée */
				fprintf(stderr, "La socket a été fermée par le client !\n\n");
				close(socketDialogue);
				return 0;
			default: /* envoi de n octets */
				printf("Message %s envoyé (%d octets)\n\n", messageEnvoi, ecrits);
		// On ferme la socket de dialogue et on se replace en attente ...
			}
		}
		else {
			wait(&status);
			codeRetour=WEXITSTATUS(status);
			if (codeRetour == 2){
				close(socketDialogue);
				printf("Bye !\n\n");
				//--- Fin de l’étape n°7 !
				// On ferme la ressource avant de quitter
				close(socketEcoute);
				return 0;
				}
			else {
				printf("Fermeture imprévue");
				return 1;
				}
			}
		}
	}
	
char * getFR(char * motEN) {
	int i;
	for (i=0; i<TAILLE; i++) {
		if (!strcmp(motEN,en[i])) break;
			}
		if (i==TAILLE)
			return "??" ; // Introuvable
		else return fr[i]; 
			}
			
char * getEN(char * motFR) {
	int i;
	for (i=0; i<TAILLE; i++) {
		if (!strcmp(motFR,fr[i])) break; 
			} 
		if (i==TAILLE)
			return "??"; // Introuvable
		else return en[i]; 
			}
			
			/* Fonction de découpage de chaine en fonction d'un (ou plusieurs) caractère(s) séparateur(s)
			Les chaines résultantes du découpage seront rangées dans un tableau (alloué) de taille nbItems
			Déclarations nécessaires :
			char **requete;
			size_t nbItems;
			Exemple d'utilisation:
			requete = strsplit("FR blue", " ", &nbItems);
			printf("%d\n",nbItems);
			// 2 printf("%s\n",requete[0]);
			// FR printf("%s\n",requete[1]);
			// blue
			*/
			
char ** strsplit(const char* str, const char* delim, size_t* numtokens) {
	char *s = strdup(str);
	size_t tokens_alloc = 1;
	size_t tokens_used = 0;
	char **tokens = calloc(tokens_alloc, sizeof(char*));
	char *token, *strtok_ctx;
	for (token = strtok_r(s, delim, &strtok_ctx);
		token != NULL;
	token = strtok_r(NULL, delim, &strtok_ctx)) {
		if (tokens_used == tokens_alloc) {
			tokens_alloc *= 2;
			tokens = realloc(tokens, tokens_alloc * sizeof(char*));
		}
			tokens[tokens_used++] = strdup(token);
	} 
		if (tokens_used == 0) {
			free(tokens);
			tokens = NULL;
				} 
		else {
			tokens = realloc(tokens, tokens_used * sizeof(char*));
				} 
			*numtokens = tokens_used;
			free(s);
			return tokens;
		}
