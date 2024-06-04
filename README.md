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
