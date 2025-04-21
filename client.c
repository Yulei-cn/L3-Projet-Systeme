#include <stdio.h>        
#include <stdlib.h>         
#include <string.h>         
#include <unistd.h>         
#include <pthread.h>        
#include <sys/socket.h>     
#include <netinet/in.h>     
#include <arpa/inet.h>      

#define PORT 8080
#define BUFFER_SIZE 1024


void* traiter_client(void* arg) {
    int sservice = *(int*)arg;  
    char message[BUFFER_SIZE] = "Bienvenue sur le serveur !\n";  
    write(sservice, message, strlen(message));  //vien de Prof. serveurUnix.c 
    close(sservice);  
    pthread_exit(NULL);  
}

int main() {
    int secoute, sservice;
    struct sockaddr_in saddr;  
    struct sockaddr_in caddr;  
    socklen_t caddrlen = sizeof(caddr);  
    pthread_t thread_id;  
    
    secoute = socket(AF_INET, SOCK_STREAM, 0);
    if (secoute == -1) {
        perror("Erreur creation socket");
        exit(1);
    }


    memset(&saddr, 0, sizeof(saddr));  
    saddr.sin_family = AF_INET;        
    saddr.sin_addr.s_addr = INADDR_ANY; 
    saddr.sin_port = htons(PORT);      

    if (bind(secoute, (struct sockaddr*)&saddr, sizeof(saddr)) < 0) {
        perror("Erreur bind");
        exit(1);
    }


    if (listen(secoute, 10) < 0) {
        perror("Erreur listen");
        exit(1);
    }

    printf("Serveur TCP en écoute sur le port %d...\n", PORT);

    
    while (1) {
   
        sservice = accept(secoute, (struct sockaddr*)&caddr, &caddrlen);
        if (sservice < 0) {
            perror("Erreur accept");
            continue;
        }

        printf("Connexion de %s:%d\n", inet_ntoa(caddr.sin_addr), ntohs(caddr.sin_port));


        pthread_create(&thread_id, NULL, traiter_client, (void*)&sservice);
        pthread_detach(thread_id);  
    }

    close(secoute);  
    return 0;
}
