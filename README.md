# <p align="center">ft_ping</p>
> Ce projet consiste à recoder une partie de la commande ping.
>
>Recoder la commande ping vous permettra d'avoir une première approche de la communication (TCP/IP) entre deux machines dans un réseau.

## Options supportées
- [ ] `-t TTL`: Temps de vie du paquet
- [ ] `-s SIZE`: Taille du paquet
- [ ] `-c COUNT`: Nombre de paquets à envoyer
- [ ] `-i INTERVAL`: Intervalle entre chaque paquet
- [ ] `-I INTERFACE`: Interface à utiliser
- [ ] `-w DEADLINE`: Temps d'exécution maximal du programme
- [ ] `-W TIMEOUT`: Temps d'attente maximal pour un paquet
- [ ] `-n`: Ne résoud pas les noms de domaine
- [ ] `-D`: Affiche l'horodatage avant chaque ligne
- [ ] `-f`: Mode flood
- [ ] `-v`: Mode verbeux
- [ ] `-q`: Mode silencieux
- [ ] `-?` `-h`: Affiche l'aide

## Install
```bash
mkdir -p ~/.local/src
mkdir -p ~/.local/bin

apt update -y
apt install -y git
apt install -y make
apt install -y gcc
```

```bash
cd ~/.local/src
git clone https://github.com/Skalyaeve/ft_ping.git
cd ft_ping && make
ln -s $PWD/ft_ping ~/.local/bin/ft_ping
export PATH=~/.local/bin:$PATH
```

1. **Résolution DNS** : Utilisez `getaddrinfo()` pour résoudre le nom de domaine en adresse IP.

2. **Création de socket** : Créez un socket RAW en utilisant `socket()`. Les sockets RAW sont utilisés pour envoyer ou recevoir des paquets de données bruts sans aucun protocole de transport.

3. **Préparation du paquet ICMP** : Préparez un paquet ICMP. ICMP est le protocole utilisé par `ping`. Le paquet ICMP contient un en-tête et des données. L'en-tête contient des informations telles que le type de message ICMP, le code de message, un checksum, un identifiant et un numéro de séquence.

4. **Envoi du paquet ICMP** : Envoyez le paquet ICMP à l'adresse IP cible en utilisant `sendto()`.

5. **Réception de la réponse ICMP** : Recevez la réponse ICMP en utilisant `recvfrom()`. La réponse ICMP est également un paquet ICMP, mais le type de message sera différent.

6. **Calcul du temps de round-trip** : Calculez le temps qu'il a fallu pour envoyer le paquet ICMP et recevoir la réponse. C'est le temps de round-trip.

7. **Répétez les étapes 4 à 6** : Répétez les étapes 4 à 6 pour un certain nombre de fois ou jusqu'à ce que l'utilisateur interrompe le processus.

8. **Affichage des statistiques** : À la fin, affichez des statistiques telles que le nombre de paquets envoyés, reçus et perdus, ainsi que le temps de round-trip minimum, maximum et moyen.
