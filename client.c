#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>

#define NOM_SIZE 50
#define BUFFER_SIZE 1024
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080

int sclient;
volatile sig_atomic_t mode_silencieux = 0;
char buffer_attente[100][BUFFER_SIZE];
int nb_messages_attente = 0;
pthread_mutex_t mutex_silence = PTHREAD_MUTEX_INITIALIZER;

void gestion_ctrl_c(int sig) {
    mode_silencieux = 1;
    printf("\n[SILENCIEUX] Mode silencieux activé. Tapez votre message :\n");
}

void* envoyer(void* arg) {
    char message[BUFFER_SIZE] = {0};

    while (1) {
        if (!mode_silencieux) {
            printf("Client: Entrez message.\n");
        }

        fgets(message, BUFFER_SIZE, stdin);
        message[strcspn(message, "\n")] = '\0';

        write(sclient, message, strlen(message) + 1);

        if (message[0] == 'f') {
            break;
        }

        pthread_mutex_lock(&mutex_silence);
        if (mode_silencieux) {
            mode_silencieux = 0;
            printf("\n[SILENCIEUX] Fin du mode silencieux. Messages reçus pendant l'interruption :\n");
            for (int i = 0; i < nb_messages_attente; i++) {
                printf("Message reçu par client : %s\n", buffer_attente[i]);
            }
            nb_messages_attente = 0;
        }
        pthread_mutex_unlock(&mutex_silence);
    }

    pthread_exit(NULL);
}

void* recevoir(void* arg) {
    char reponse[BUFFER_SIZE] = {0};

    while (1) {
        memset(reponse, '\0', BUFFER_SIZE);
        int n = read(sclient, reponse, BUFFER_SIZE);

        if (n <= 0) break;

        pthread_mutex_lock(&mutex_silence);
        if (mode_silencieux) {
            if (nb_messages_attente < 100) {
                strncpy(buffer_attente[nb_messages_attente++], reponse, BUFFER_SIZE);
            }
        } else {
            printf("Message reçu par client : %s\n", reponse);
        }
        pthread_mutex_unlock(&mutex_silence);
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    char message[BUFFER_SIZE] = {0};
    char reponse[BUFFER_SIZE] = {0};
    struct sockaddr_in saddr = {0};
    pthread_t t1, t2;

    signal(SIGINT, gestion_ctrl_c);

    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(SERVER_PORT);
    saddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    sclient = socket(AF_INET, SOCK_STREAM, 0);

    int essais = 0;
    while (connect(sclient, (struct sockaddr*)&saddr, sizeof(saddr)) == -1 && essais < 5) {
        printf("Tentative %d échouée\n", essais + 1);
        essais++;
        sleep(1);
    }
    if (essais == 5) {
        printf("Connexion impossible après 5 tentatives\n");
        exit(1);
    }

    char nom[NOM_SIZE];
    printf("Entrez votre nom : ");
    scanf("%s", nom);
    getchar();

    write(sclient, nom, strlen(nom) + 1);

    pthread_create(&t1, NULL, envoyer, NULL);
    pthread_create(&t2, NULL, recevoir, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    shutdown(sclient, SHUT_RDWR);
    close(sclient);
    return 0;
}
