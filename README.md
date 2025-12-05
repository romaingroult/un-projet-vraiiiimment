# Documentation du Projet Yubino

## Architecture du projet

Le projet est organisé en plusieurs modules, chacun responsable d'une fonctionnalité spécifique, orchestrés par un fichier `main.c` central :

### Modules principaux

- **buffer** : Gestion de la lecture d'un flux de données
- **crypto** : Génération aléatoire d'identifiants et de paires de clés de chiffrement (publique/privée), ainsi que la logique de signature/vérification cryptographique
- **button** : Interface avec un bouton branché sur le port `PORTD2` de l'Arduino
- **led** : Interface avec une LED branchée sur le port `PORTD4` de l'Arduino
- **eeprom_registry** : Gestion d'un registre de credentials dans la mémoire non volatile de l'Arduino

---

## Gestion du buffer et lecture des commandes

### Stratégie de réception

Afin de permettre au client d'envoyer l'intégralité de sa requête, un délai d'attente de **100 ms** est implémenté dès la réception des premières données. Cette approche garantit :

- La réception complète de la requête
- La vérification du format attendu (taille en octets)

### Traitement des requêtes

Une fois la requête interceptée :

1. Lecture du **premier octet** pour identifier la commande
2. Exécution de la fonction associée
3. Transmission de la requête complète selon les besoins (uniquement pour la génération de credentials et la signature de hash)

---

## Gestion du registre EEPROM

### Organisation de la mémoire

La mémoire EEPROM de **1024 octets** est découpée virtuellement en slots :
```
Capacité = 1024 / CREDENTIAL_SIZE
Capacité = 1024 / (20 + 21 + 16)
Capacité = 17 slots de credentials
```

### Détection des slots libres

Un slot est considéré comme **libre** si son premier octet correspond à une constante prédéfinie (`EMPTY_MARKER`).

### Initialisation du registre

Pour éviter qu'au premier démarrage l'Arduino considère le registre comme plein, un mécanisme d'initialisation est mis en place :

- **Position** : Les 2 octets suivant la zone allouée au registre servent de marqueur d'initialisation
- **Valeurs** : Ces octets doivent correspondre à des constantes prédéfinies (`MAGIC_MARKER`)
- **Processus** :
  1. Vérification des marqueurs au démarrage
  2. Si non initialisé : réinitialisation de tous les slots (marquage comme vides)
  3. Écriture des marqueurs d'initialisation

Ce mécanisme garantit qu'une seule initialisation est effectuée lors du premier démarrage.

---

## Compilation et déploiement

### Prérequis

Vérifier que le **PORT** de l'Arduino est correctement configuré dans le `Makefile` (ligne 7).

### Commande de compilation et flash
```bash
make flash
```

Cette commande compile le programme et le flash sur l'Arduino.