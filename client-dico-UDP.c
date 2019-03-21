/************* UDP CLIENT CODE *******************/

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h> /* pour htons et inet_aton */
#define PORT IPPORT_USERRESERVED // = 5000
#define LG_MESSAGE 256
#define TAILLE 3

int main(){
  char *fr[] = { "bleu", "vert", "rouge" };
  char *en[] = { "blue", "green", "red" };
  int descripteurSocket;
  int clientSocket, portNum, nBytes;
  char messageEnvoi[LG_MESSAGE];
  char messageRecu[LG_MESSAGE];
  struct sockaddr_in pointDeRencontreDistant;
  socklen_t addr_size;

  /*Create UDP socket*/
  descripteurSocket = socket(AF_INET, SOCK_DGRAM, 0);

  if(descripteurSocket < 0) {
	perror("socket"); // Affiche le message d’erreur
	return 1; // On sort en indiquant un code erreur
  }
  printf("Socket créée avec succès ! (%d)\n", descripteurSocket);

  /*Configure settings in address struct*/
  pointDeRencontreDistant.sin_family = AF_INET;
  pointDeRencontreDistant.sin_port = htons(IPPORT_USERRESERVED);
  pointDeRencontreDistant.sin_addr.s_addr = inet_addr("127.0.0.1");
  memset(pointDeRencontreDistant.sin_zero, '\0', sizeof pointDeRencontreDistant.sin_zero);  

  /*Initialize size variable to be used later on*/
  addr_size = sizeof pointDeRencontreDistant;

  while(1){
    printf("Formulez votre demande:\n");
    fgets(messageEnvoi,LG_MESSAGE, stdin);

    nBytes = strlen(messageEnvoi) + 1;
    
    /*Send message to server*/
    sendto(descripteurSocket,messageEnvoi,nBytes,0,(struct sockaddr *)&pointDeRencontreDistant,addr_size);

    /*Receive message from server*/
                nBytes = recvfrom(clientSocket,messageRecu,LG_MESSAGE,0,NULL, NULL);

    printf("Reponse du serveur: %s\n",messageRecu);

  }

  return 0;
}
