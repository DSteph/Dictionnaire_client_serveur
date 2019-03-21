/************* UDP SERVER CODE *******************/

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#define PORT IPPORT_USERRESERVED // = 5000
#define LG_MESSAGE 256
#define TAILLE 3


char ** strsplit(const char* str, const char* delim, size_t* numtokens);
char * getFR(char * motEN);
char * getEN(char * motFR);
char *fr[] = { "bleu", "vert", "rouge" };
char *en[] = { "blue", "green", "red" };

int main(){
  int udpSocket, nBytes;
  char buffer[LG_MESSAGE];
  struct sockaddr_in serverAddr, clientAddr;
  struct sockaddr_storage serverStorage;
  socklen_t addr_size, client_addr_size;
  int i, pid;

  /*Create UDP socket*/
  udpSocket = socket(AF_INET, SOCK_DGRAM, 0);

  /*Configure settings in address struct*/
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(IPPORT_USERRESERVED);
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  //memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
  memset(serverAddr.sin_zero, 0x00, sizeof serverAddr.sin_zero); 
		

  /*Bind socket with address struct*/
  bind(udpSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

  /*Initialize size variable to be used later on*/
  addr_size = sizeof serverStorage;

  while(1){
    /* Try to receive any incoming UDP datagram. Address and port of 
      requesting client will be stored on serverStorage variable */
    nBytes = recvfrom(udpSocket,buffer,1024,0,(struct sockaddr *)&serverStorage, &addr_size);

   pid=fork();

		if (pid == -1) {
			perror("Fork error");
			return 1;
		}
		if (pid == 0) { // FILS
			printf("Requête traitée par le processus %d\n", getpid());

		//Conversion message
		requete = strsplit(buffer, " ", &nbItems);

		//Check si client veut quitter
			if (strcmp(requete[0], "QUIT") == 0){
				close(socketEcoute);
				printf("Bye !\n");
				return EXIT_SUCCESS;
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
			   /*Send uppercase message back to client, using serverStorage as the address*/
    sendto(udpSocket,requete,nBytes,0,(struct sockaddr *)&serverStorage,addr_size);
		}
		else wait(NULL);
  }
  return 0;
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
			requete = strsplit("FR blue", " ", &nbItems);s
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
