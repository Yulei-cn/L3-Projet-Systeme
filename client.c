/* CODE CLIENT. Socket Unix : fichier local ./MySocket */

#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#include <sys/socket.h> 
#include <sys/types.h> 
#include <sys/un.h> 

#include <arpa/inet.h>   // 提供 inet_addr() 函数
#include <netinet/in.h> 

#define BUFFER_SIZE 1024 
#define SERVER_IP "127.0.0.1" 
#define SERVER_PORT 8080 

int main(int argc, char *argv[]) {
	int sclient; 			// File descripteur pour socket client 
	char message[BUFFER_SIZE] = {0};
	char reponse[BUFFER_SIZE] = {0};

	// Initialisation struct adress du serveur
	//struct sockaddr_un saddr = {0}; 	// Struct adress du serveur
	//saddr.sun_family = AF_UNIX;		// Address Family UNIX 
	//strcpy(saddr.sun_path, "./MySock");	// Chemin vers fichier socket

    struct sockaddr_in saddr = {0};
    saddr.sin_family = AF_INET;                         // Pv4
    saddr.sin_port = htons(SERVER_PORT);                
    saddr.sin_addr.s_addr = inet_addr(SERVER_IP);  

	// creation tcp client
    sclient = socket(AF_INET, SOCK_STREAM, 0);


	// tentative de connextion au serveur, connect renvoi -1 en cas d'erreur
    int essais = 0;
    while (connect(sclient, (struct sockaddr*)&saddr, sizeof(saddr)) == -1 && essais < 5) {
        printf("Tentative %d échouée\n", essais + 1);
        essais++;
        sleep(1);
    }
    if (essais == 5) {
        //printf("Connexion impossible après 5 tentatives\n");
        exit(1);
    }
    
	// Communication
	while(message[0] != 'f') {
		printf("Client: Entrez message.\n");
		scanf("%s", message); 
		write(sclient,message,strlen(message)+1);
		sleep(2);
		memset(reponse, '\0', BUFFER_SIZE);
		read(sclient,reponse,BUFFER_SIZE); 
		printf("Message reçu par client : %s \n",reponse); 
	}
	// fermeture service, et fermeture socket/serveur
	shutdown(sclient,SHUT_RDWR);
	close(sclient);
	return 0;
}
