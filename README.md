# Projet Système : Messagerie instantanée

Ce projet consiste à développer un serveur de messagerie instantanée multi-clients utilisant des sockets et des threads. Le serveur doit permettre à plusieurs clients de se connecter, d’envoyer et de recevoir des messages simultanément.

## Étapes de développement

### Étape 1 : Création du serveur

- Le serveur écoute les connexions entrantes sur un port donné.
- Pour chaque nouvelle connexion, le serveur crée un thread dédié.
- La communication se fait via des sockets TCP.

### Étape 2 : Gestion des messages

- Le serveur maintient une structure pour stocker les messages de chaque utilisateur.
- Utiliser une matrice de chaînes de caractères : lignes = utilisateurs, colonnes = messages.

### Étape 3 : Envoi de message

- Un client envoie une chaîne de caractères au serveur via son socket.

### Étape 4 : Réception et diffusion

- Le serveur reçoit un message d’un client.
- Il rediffuse ce message à tous les clients connectés.

### Étape 5 : Affichage côté client

- Le client affiche chaque message reçu dès réception.

### Étape 6 : Identification

- Chaque message affiché doit indiquer le numéro (ou identifiant) du client qui l’a envoyé.

### Étape 7 : Blocage pour rédaction

- Le client peut bloquer temporairement l’affichage (ex : Ctrl+C) pour rédiger un message sans interruptions.
- Après l’envoi, les messages reçus pendant l’interruption sont affichés.

## Compilation

```bash
gcc -o serveur serveur.c -lpthread
gcc -o client client.c
```

## Exécution

```bash
./serveur
./client
```
