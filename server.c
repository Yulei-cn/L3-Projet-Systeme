#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024
#define PORT 8080
#define MAX_CLIENTS 100
#define NOM_SIZE 50
#define FULL_SIZE 2048
#define MAX_MESSAGES 100
#define MSG_LENGTH 1024

// Structure pour représenter un client
typedef struct {
    int socket;
    char nom[NOM_SIZE];
    int id;
} Client;

// Variables globales
Client clients[MAX_CLIENTS];
int nb_clients = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

char historique_messages[MAX_CLIENTS][MAX_MESSAGES][MSG_LENGTH];
int compteur_messages[MAX_CLIENTS] = {0};

// Fonction exécutée dans un thread pour chaque client connecté
void* traiter_client(void* arg) {
    Client client = *(Client*)arg;
    char message[BUFFER_SIZE] = {0};
    char annonce[FULL_SIZE] = {0};

    // Lecture du nom du client
    read(client.socket, client.nom, NOM_SIZE);

    printf("[Serveur] Client %d (%s) a rejoint le chat.\n", client.id, client.nom);
    snprintf(annonce, sizeof(annonce), "[Serveur] Client %d (%s) a rejoint le chat.", client.id, client.nom);

    // Diffusion de l'annonce aux autres clients
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < nb_clients; i++) {
        if (clients[i].socket != client.socket) {
            write(clients[i].socket, annonce, strlen(annonce) + 1);
        }
    }
    pthread_mutex_unlock(&mutex);