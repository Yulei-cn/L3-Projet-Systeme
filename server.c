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

typedef struct {
    int socket;
    char nom[NOM_SIZE];
    int id;
} Client;

Client clients[MAX_CLIENTS];
int nb_clients = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

char historique_messages[MAX_CLIENTS][MAX_MESSAGES][MSG_LENGTH];
int compteur_messages[MAX_CLIENTS] = {0};

void* traiter_client(void* arg) {
    Client client = *(Client*)arg;
    char message[BUFFER_SIZE] = {0};
    char annonce[FULL_SIZE] = {0};

    read(client.socket, client.nom, NOM_SIZE);

    printf("[Serveur] Client %d (%s) a rejoint le chat.\n", client.id, client.nom);

    snprintf(annonce, sizeof(annonce), "[Serveur] Client %d (%s) a rejoint le chat.", client.id, client.nom);

    pthread_mutex_lock(&mutex);
    for (int i = 0; i < nb_clients; i++) {
        if (clients[i].socket != client.socket) {
            write(clients[i].socket, annonce, strlen(annonce) + 1);
        }
    }
    pthread_mutex_unlock(&mutex);

    while (1) {
        memset(message, '\0', BUFFER_SIZE);
        int n = read(client.socket, message, BUFFER_SIZE);

        if (n <= 0 || message[0] == 'f') break;

        // Gestion historique messages
        if (compteur_messages[client.id - 1] < MAX_MESSAGES) {
            strncpy(historique_messages[client.id - 1][compteur_messages[client.id - 1]], message, MSG_LENGTH);
            compteur_messages[client.id - 1]++;
        }

        // Afficher l'historique si le client le demande
        if (strcmp(message, "historique") == 0) {
            for (int i = 0; i < compteur_messages[client.id - 1]; i++) {
                write(client.socket, historique_messages[client.id - 1][i], strlen(historique_messages[client.id - 1][i]) + 1);
            }
            continue;
        }

        printf("[Serveur] Message reçu de %s : %s\n", client.nom, message);

        char full[FULL_SIZE];
        snprintf(full, sizeof(full), "[Client %d - %s] %s", client.id, client.nom, message);

        pthread_mutex_lock(&mutex);
        for (int i = 0; i < nb_clients; i++) {
            if (clients[i].socket != client.socket) {
                write(clients[i].socket, full, strlen(full) + 1);
            }
        }
        pthread_mutex_unlock(&mutex);
    }

    printf("[Serveur] Client %d (%s) a quitté le chat.\n", client.id, client.nom);
    snprintf(annonce, sizeof(annonce), "[Serveur] Client %d (%s) a quitté le chat.", client.id, client.nom);

    pthread_mutex_lock(&mutex);
    for (int i = 0; i < nb_clients; i++) {
        if (clients[i].socket == client.socket) {
            clients[i] = clients[nb_clients - 1];
            nb_clients--;
            break;
        }
    }
    for (int i = 0; i < nb_clients; i++) {
        write(clients[i].socket, annonce, strlen(annonce) + 1);
    }
    pthread_mutex_unlock(&mutex);

    close(client.socket);
    pthread_exit(NULL);
}

int main() {
    int secoute, sservice;
    struct sockaddr_in saddr = {0};
    struct sockaddr_in caddr = {0};
    socklen_t caddrlen = sizeof(caddr);
    pthread_t thread_id;

    secoute = socket(AF_INET, SOCK_STREAM, 0);
    if (secoute == -1) {
        perror("Erreur création socket");
        exit(1);
    }

    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;
    saddr.sin_port = htons(PORT);

    if (bind(secoute, (struct sockaddr*)&saddr, sizeof(saddr)) < 0) {
        perror("Erreur bind");
        exit(1);
    }

    listen(secoute, 10);
    printf("Serveur en écoute sur le port %d...\n", PORT);

    while (1) {
        sservice = accept(secoute, (struct sockaddr*)&caddr, &caddrlen);
        if (sservice < 0) {
            perror("Erreur accept");
            continue;
        }

        printf("Connexion de %s:%d\n", inet_ntoa(caddr.sin_addr), ntohs(caddr.sin_port));

        Client new_client;
        new_client.socket = sservice;
        new_client.id = nb_clients + 1;

        pthread_mutex_lock(&mutex);
        if (nb_clients < MAX_CLIENTS) {
            clients[nb_clients++] = new_client;
        } else {
            close(new_client.socket);
            pthread_mutex_unlock(&mutex);
            continue;
        }
        pthread_mutex_unlock(&mutex);

        pthread_create(&thread_id, NULL, traiter_client, (void*)&clients[nb_clients - 1]);
        pthread_detach(thread_id);
    }

    close(secoute);
    return 0;
}
