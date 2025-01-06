# <p align="center">ft_ping</p>

> Ce projet consiste à recoder une partie de la commande ping.
>
> Recoder la commande ping vous permettra d'avoir une première approche de la communication (TCP/IP) entre deux machines dans un réseau.

## Options supportées

- [x] `-c --count NUMBER`: Nombre de paquets à envoyer
- [ ] `-i --interval NUMBER`: Intervalle entre chaque paquet
- [ ] `   --ttl NUMBER`: Temps de vie du paquet
- [ ] `-w --timeout NUMBER`: Temps d'exécution maximal du programme
- [ ] `-f --flood`: Mode flood
- [ ] `-l --preload NUMBER`: Mode flood pour NUMBER paquets
- [ ] `-s --size NUMBER`: Taille du paquet
- [ ] `-q --quiet`: Mode silencieux
- [ ] `-v --verbose`: Mode verbeux
- [ ] `-? --help`: Affiche l'aide

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

ft_ping -?
```
